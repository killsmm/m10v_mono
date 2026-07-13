/*************************************************************************************\
*                                                                                     *
* Copyright (C) CEVA Inc. All rights reserved                                         *
*                                                                                     *
*                                                                                     *
* THIS PRODUCT OR SOFTWARE IS MADE AVAILABLE EXCLUSIVELY TO LICENSEES THAT HAVE       *
* RECEIVED EXPRESS WRITTEN AUTHORIZATION FROM CEVA TO DOWNLOAD OR RECEIVE THE         *
* PRODUCT OR SOFTWARE AND HAVE AGREED TO THE END USER LICENSE AGREEMENT (EULA).       *
* IF YOU HAVE NOT RECEIVED SUCH EXPRESS AUTHORIZATION AND AGREED TO THE               *
* CEVA EULA, YOU MAY NOT DOWNLOAD, INSTALL OR USE THIS PRODUCT OR SOFTWARE.           *
*                                                                                     *
* The information contained in this document is subject to change without notice and  *
* does not represent a commitment on any part of CEVA®, Inc. CEVA®, Inc. and its      *
* subsidiaries make no warranty of any kind with regard to this material, including,  *
* but not limited to implied warranties of merchantability and fitness for a          *
* particular purpose whether arising out of law, custom, conduct or otherwise.        *
*                                                                                     *
* While the information contained herein is assumed to be accurate, CEVA®, Inc.       *
* assumes no responsibility for any errors or omissions contained herein, and         *
* assumes no liability for special, direct, indirect or consequential damage,         *
* losses, costs, charges, claims, demands, fees or expenses, of any nature or kind,   *
* which are incurred in connection with the furnishing, performance or use of this    *
* material.                                                                           *
*                                                                                     *
* This document contains proprietary information, which is protected by U.S. and      *
* international copyright laws. All rights reserved. No part of this document may be  *
* reproduced, photocopied, or translated into another language without the prior      *
* written consent of CEVA®, Inc.                                                      *
*                                                                                     *
\*************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \file       frame_buf.h
/// \brief      Header file for frame buffer structures definition and related function declarations
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FRAME_BUF_H__
#define __FRAME_BUF_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Header files
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Type definitions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// \brief Image color components enumeration
typedef enum {  IMAGE_COMP_R = 0, 
                IMAGE_COMP_G, 
                IMAGE_COMP_B, 
                IMAGE_COMP_A, 
                IMAGE_COMP_Y = 0, 
                IMAGE_COMP_CB, 
                IMAGE_COMP_CR, 
                IMAGE_COMP_H = 0, 
                IMAGE_COMP_S, 
                IMAGE_COMP_V, 
                IMAGE_COMP_I = IMAGE_COMP_V, 
                IMAGE_COMP_NUM = 4
} image_comp_e;

/// \brief Bit-depth enumeration
typedef enum {  BIT_DEPTH_4BPP,
				BIT_DEPTH_8BPP,
                BIT_DEPTH_10BPP,
                BIT_DEPTH_12BPP,
                BIT_DEPTH_14BPP,
                BIT_DEPTH_16BPP,
                BIT_DEPTH_24BPP,
                BIT_DEPTH_32BPP
} bit_depth_e;

/// \brief Frame buffer structure for a single color plane (or packed format)
typedef struct frame_buf_
{
    unsigned char*         data;       ///< pointer to image data
    unsigned short         width;      ///< width (in elements)
    unsigned short         height;     ///< height
    unsigned short         stride;     ///< stride (bytes)
    unsigned short         elem_size;  ///< element size (bytes)
    unsigned short         id;         ///< buffer identifier (application-dependent)
    unsigned short         padding;    // padding for DWORD alignment
} frame_buf_t;

/// \brief Color format enumeration
typedef enum {  COLOR_FMT_RGB444P,
                COLOR_FMT_YUV444P,
                COLOR_FMT_YUV422P,
                COLOR_FMT_YUV420P,
				COLOR_FMT_NV12,
				COLOR_FMT_YUYV422,
				COLOR_FMT_RGBA444P,
                COLOR_FMT_MONO
} color_fmt_e;

/// \brief Rectangle structure for a roi
typedef struct
{
	signed short x;      /** x-coordinate of top left corner of rectangle */
	signed short y;      /** y-coordinate of top left corner of rectangle */
	signed short width;  /** width of the rectangle  */
	signed short height; /** height of the rectangle */
} S16Rect;

/// \brief Structure for color image
typedef struct image_
{
    frame_buf_t		comp[IMAGE_COMP_NUM];   ///< array of color planes
    color_fmt_e		fmt;                    ///< color format
    bit_depth_e		bit_depth;              ///< bit-depth (per channel)
    unsigned int	coi;                    ///< Channel Of Interest (0 - no COI (all channels are selected), 1 - 0th channel is selected )*/
    S16Rect			roi;                    ///< Region Of Interest
    unsigned int	tag;                    ///< application-specific tag
    void*			user_data;              ///< pointer to an application-dependent data structure
} image_t; 


typedef enum { IMAGE_SET_ADDR_MODE_HOST, IMAGE_SET_ADDR_MODE_CORE } image_set_addr_mode_e;

#define IMAGE_SET_MAX_NUM_IMAGES       7

/// \brief Structure for a set of color images
/// \note the pointer can point to an array of image_t structures, or a head of linked list
///  (e.g. implemented using the user_data pointer)
typedef struct image_set_
{
    unsigned int			num_images;                         ///< number of images in the array
    image_set_addr_mode_e	addr_mode;							///< addressing mode of pointers in p_image
    image_t*				p_image[IMAGE_SET_MAX_NUM_IMAGES];	///< pointer to the image array   
} image_set_t; 

/// \brief Bit-mask values for buffer copy modes
typedef enum {
    FRAME_BUF_COPY_MODE_DEFAULT = 0,
    FRAME_BUF_COPY_MODE_CROP_EN = 1,
    FRAME_BUF_COPY_MODE_DUP2_EN = 2,
    FRAME_BUF_COPY_MODE_DUP4_EN = 4,
    FRAME_BUF_COPY_MODE_DUP8_EN = 8
} frame_buf_copy_mode_e;

#endif //__FRAME_BUF_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
