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
 @header Error values
 Error values returned by both user and kernel functions.

 @copyright Vivante Corporation
 */

#ifndef _vg_lite_error_h_
#define _vg_lite_error_h_

#ifdef __cplusplus
extern "C" {
#endif


/*!
 @abstract Error codes that the vg_lite functions can return.

 @discussion
 All API functions return a status code. On success, <code>VG_LITE_SUCCESS</code> will be returned when a function is
 successful. This value is set to zero, so if any function returns a non-zero value, an error has occured.
 */
typedef enum vg_lite_error
{
    VG_LITE_SUCCESS = 0,        /*! Success. */
    VG_LITE_INVALID_ARGUMENT,   /*! An invalid argument was specified. */
    VG_LITE_OUT_OF_MEMORY,      /*! Out of memory. */
    VG_LITE_NO_CONTEXT,         /*! No context or an unintialized context specified. */
    VG_LITE_TIMEOUT,            /*! A timeout has occured during a wait. */
    VG_LITE_OUT_OF_RESOURCES,   /*! Out of system resources. */
    VG_LITE_GENERIC_IO          /*! Cannot communicate with the kernel driver. */
}
vg_lite_error_t;

#ifdef __cplusplus
}
#endif
#endif /* _vg_lite_error_h_ */
