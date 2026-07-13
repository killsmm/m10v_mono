/*----------------------------------------------------

IPU 1.1 driver API
Copyright (c) FotoNation
All rights reserved. Confidential.

----------------------------------------------------*/

#pragma once

#include "gtypes.h"

#ifdef  __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////
// Overview (Main Page)
/**
@mainpage Overview

  This documentation describes the FotoNation's <b>AHIP Driver
  API</b> which serves as an API layer between the application and HW. \n

  AHIP API is divided into several parts which are related to the corresponding
  hardware blocks:
  - @ref api_pre
  - @ref api_vis
  - @ref api_fd
  - @ref api_norm
  - @ref api_mio
  - @ref api_gdc
  
  VIS and PRE blocks can work independently of each other either in preview or in playback mode.
  In case they work in the same mode, VIS and PRE use the same input frame. \n
  NORM, GDC, and MIO blocks always work in playback mode independently from VIS and
  FD blocks.

  The detailed API Reference can be found @ref api "here".
*/

/// @defgroup api API Reference
/// @{

#ifndef __GNUC__
#pragma region COMMON API
#endif
///////////////////////////////////////////////////////////////////////////
// Status Codes
/**
  @defgroup api_status Status Codes
  The following constants indicate the result of an API call.
@{
**************************************************************************/
enum {
    GTS_INVALID_MODE        = -20,  ///< The API cannot be used in this mode. @hideinitializer
    GTS_HW_ERROR            = -21,  ///< A hardware error has occurred. @hideinitializer
    GTS_BUFFER_TOO_SMALL    = -22,  ///< The buffer size is too small. @hideinitializer
    GTS_BUSY                = -23,  ///< The HW block is busy. @hideinitializer
    GTS_STOPPED             = -24,  ///< The operation was cancelled by application. @hideinitializer
    GTS_WAIT                =   1,  ///< The operation has not completed yet. @hideinitializer
};

/// @} end of api_status

///////////////////////////////////////////////////////////////////////////
// Image formats
/**
  @defgroup api_formats Image Formats
  The following constants specify image formats supported by AHIP.
@{ 
**************************************************************************/
enum {
    /**
      Planar YUV 4:2:0 - the luminance (Y) and chrominance (U,V) components
      are stored in three separate 8-bit sample planes. The chrominance components
      are averaged for 2x2 pixels. For example, a 4x4 image is stored as:
      <pre style="line-height:16px;">
              Y                  U                  V
      ┌───┬───┬───┬───┐  ┌───────┬───────┐  ┌───────┬───────┐
      │0,0│0,1│0,2│0,3│  │       │       │  │       │       │
      ├───┼───┼───┼───┤  │  0,0  │  0,1  │  │  0,0  │  0,1  │
      │1,0│1,1│1,2│1,3│  │       │       │  │       │       │
      ├───┼───┼───┼───┤  ├───────┼───────┤  ├───────┼───────┤
      │2,0│2,1│2,2│2,3│  │       │       │  │       │       │
      ├───┼───┼───┼───┤  │  1,0  │  1,1  │  │  1,0  │  1,1  │
      │3,0│3,1│3,2│3,3│  │       │       │  │       │       │
      └───┴───┴───┴───┘  └───────┴───────┘  └───────┴───────┘
      </pre>
    */
    AHIP_FMT_YUV_420P,

    /**
      Planar YUV 4:2:2 - the luminance (Y) and chrominance (U,V) components 
      are stored in three separate 8-bit sample planes. The chrominance components
      are averaged for 2x1 pixels. For example, a 4x4 image is stored as:
      <pre style="line-height:16px;">
              Y                  U                  V
      ┌───┬───┬───┬───┐  ┌───────┬───────┐  ┌───────┬───────┐
      │0,0│0,1│0,2│0,3│  │  0,0  │  0,1  │  │  0,0  │  0,1  │
      ├───┼───┼───┼───┤  ├───────┼───────┤  ├───────┼───────┤
      │1,0│1,1│1,2│1,3│  │  1,0  │  1,1  │  │  1,0  │  1,1  │
      ├───┼───┼───┼───┤  ├───────┼───────┤  ├───────┼───────┤
      │2,0│2,1│2,2│2,3│  │  2,0  │  2,1  │  │  2,0  │  2,1  │
      ├───┼───┼───┼───┤  ├───────┼───────┤  ├───────┼───────┤
      │3,0│3,1│3,2│3,3│  │  3,0  │  3,1  │  │  3,0  │  3,1  │
      └───┴───┴───┴───┘  └───────┴───────┘  └───────┴───────┘
      </pre>
    */
    AHIP_FMT_YUV_422P,

    /**
      Planar YUV 4:4:4 - the luminance (Y) and chrominance (U,V) components
      are stored in three separate 8-bit sample planes. For example, a 4x4 image
      is stored as:
      <pre style="line-height:16px;">
              Y                  U                  V
      ┌───┬───┬───┬───┐  ┌───┬───┬───┬───┐  ┌───┬───┬───┬───┐
      │0,0│0,1│0,2│0,3│  │0,0│0,1│0,2│0,3│  │0,0│0,1│0,2│0,3│
      ├───┼───┼───┼───┤  ├───┼───┼───┼───┤  ├───┼───┼───┼───┤
      │1,0│1,1│1,2│1,3│  │1,0│1,1│1,2│1,3│  │1,0│1,1│1,2│1,3│
      ├───┼───┼───┼───┤  ├───┼───┼───┼───┤  ├───┼───┼───┼───┤
      │2,0│2,1│2,2│2,3│  │2,0│2,1│2,2│2,3│  │2,0│2,1│2,2│2,3│
      ├───┼───┼───┼───┤  ├───┼───┼───┼───┤  ├───┼───┼───┼───┤
      │3,0│3,1│3,2│3,3│  │3,0│3,1│3,2│3,3│  │3,0│3,1│3,2│3,3│
      └───┴───┴───┴───┘  └───┴───┴───┴───┘  └───┴───┴───┴───┘
      </pre>
    */
    AHIP_FMT_YUV_444P,

    /**
      Semi-planar YUV 4:2:0 - the luminance (Y) and chrominance (U,V) components 
      are stored in two separate 8-bit sample planes. The chrominance components
      are averaged for 2x2 pixels. For example, a 4x4 image is stored as:
      <pre style="line-height:16px;">
                Y                     U,V
      ┌────┬────┬────┬────┐  ┌────┬────┬────┬────┐
      │Y0,0│Y0,1│Y0,2│Y0,3│  │    │    │    │    │
      ├────┼────┼────┼────┤  │U0,0│V0,0│U0,1│V0,1│
      │Y1,0│Y1,1│Y1,2│Y1,3│  │    │    │    │    │
      ├────┼────┼────┼────┤  ├────┼────┼────┼────┤
      │Y2,0│Y2,1│Y2,2│Y2,3│  │    │    │    │    │
      ├────┼────┼────┼────┤  │U1,0│V1,0│U1,1│V1,1│
      │Y3,0│Y3,1│Y3,2│Y3,3│  │    │    │    │    │
      └────┴────┴────┴────┘  └────┴────┴────┴────┘
      </pre>
    */
    AHIP_FMT_YUV_420SP,

    /**
      Semi-planar YUV 4:2:2 - the luminance (Y) and chrominance (U,V) components 
      are stored in two separate 8-bit sample planes. The chrominance components
      are averaged for 2x1 pixels. For example, a 4x4 image is stored as:
      <pre style="line-height:16px;">
                Y                     U,V
      ┌────┬────┬────┬────┐  ┌────┬────┬────┬────┐
      │Y0,0│Y0,1│Y0,2│Y0,3│  │U0,0│V0,0│U0,1│V0,1│
      ├────┼────┼────┼────┤  ├────┼────┼────┼────┤
      │Y1,0│Y1,1│Y1,2│Y1,3│  │U1,0│V1,0│U1,1│V1,1│
      ├────┼────┼────┼────┤  ├────┼────┼────┼────┤
      │Y2,0│Y2,1│Y2,2│Y2,3│  │U2,0│V2,0│U2,1│V2,1│
      ├────┼────┼────┼────┤  ├────┼────┼────┼────┤
      │Y3,0│Y3,1│Y3,2│Y3,3│  │U3,0│V3,0│U3,1│V3,1│
      └────┴────┴────┴────┘  └────┴────┴────┴────┘
      </pre>
    */
    AHIP_FMT_YUV_422SP,

    /**
      Block semi-planar YUV 4:2:0 - the luminance (Y) and chrominance (U,V)
      components are stored in two separate 8-bit sample planes. The planes
      are not stored in raster scan order, but instead in blocks scan order.
      The Y plane is composed of 64 pixels x 16 lines blocks, and UV plane is
      composed of 32 pixels x 8 lines blocks.
      The chrominance components are averaged for 2x2 pixels. For example,
      a 64x16 image is stored as:
      <pre style="line-height:16px;">
                   Y                            U,V
      ┌─────┬─────┬─────┬──────┐  ┌─────┬─────┬─────┬─────┬─────┐
      │Y0,0 │Y0,1 │ ... │Y0,63 │  │     │     │     │     │     │
      ├─────┼─────┼─────┼──────┤  │U0,0 │V0,0 │ ... │U0,31│V0,31│
      │Y1,0 │Y1,1 │ ... │Y1,63 │  │     │     │     │     │     │
      ├─────┼─────┼─────┼──────┤  ├─────┼─────┼─────┼─────┼─────┤
      │ ... │ ... │ ... │ ...  │  │ ... │ ... │ ... │ ... │ ... │
      ├─────┼─────┼─────┼──────┤  ├─────┼─────┼─────┼─────┼─────┤
      │Y14,0│Y14,1│ ... │Y14,63│  │     │     │     │     │     │
      ├─────┼─────┼─────┼──────┤  │U7,0 │V7,0 │ ... │U7,31│V7,31│
      │Y15,0│Y15,1│ ... │Y15,63│  │     │     │     │     │     │
      └─────┴─────┴─────┴──────┘  └─────┴─────┴─────┴─────┴─────┘
      </pre>
    */
    AHIP_FMT_YUV_420B,

    AHIP_FMT_MAX
};

/// @} end of api_formats

///////////////////////////////////////////////////////////////////////////
// Common API
/**
  @defgroup api_common Common API
  The common API manages global AHIP configuration.

  The following code shows the typical sequence of calls for preview mode:
  @code
    AHIP_PREVIEW_CONFIG cfg;

    // initialize the driver
    AhipInit();
    // open the driver
    AhipOpen( &cfg );
    // setup and use HW blocks
    // ...
    // close the driver
    AhipClose();
    // cleanup driver resources
    AhipCleanup();
  @endcode

  The following code shows the typical sequence of calls for playback mode:
  @code
    GT_IMAGE frame;

    // initialize the driver
    AhipInit();
    // open the driver
    AhipOpen( NULL );
    // setup HW blocks
    // ...
    while( 1 ) {
        // process playback frame
        AhipProcessFrame( 0, &frame );
    }
    // close the driver
    AhipClose();
    // cleanup driver resources
    AhipCleanup();
  @endcode
@{
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
// AHIP_RECT
/**
  Defines the coordinates of a rectangle.
**************************************************************************/
typedef struct
{
    /// Specifies the x-coordinate of the upper-left corner of the rectangle.
    UINT32  left;
    /// Specifies the y-coordinate of the upper-left corner of the rectangle.
    UINT32  top;
    /// Specifies the width of the rectangle.
    UINT32  width;
    /// Specifies the height of the rectangle.
    UINT32  height;
} AHIP_RECT;

///////////////////////////////////////////////////////////////////////////
// AHIP_SIZE
/**
  Defines the size of a rectangle.
**************************************************************************/
typedef struct
{
    /// Specifies the width of the rectangle.
    UINT32  width;
    /// Specifies the height of the rectangle.
    UINT32  height;
} AHIP_SIZE;

///////////////////////////////////////////////////////////////////////////
// Preview configuration flags
/** 
  @defgroup api_prvf Preview flags
  The following constants specify preview configuration flags.
@{ 
**************************************************************************/
enum {
    AHIP_PRF_PRE        = (1 << 0),     ///< PRE works in preview mode. @hideinitializer
    AHIP_PRF_VIS        = (1 << 1),     ///< VIS works in preview mode. @hideinitializer
};
/** @} end of api_prvf */

///////////////////////////////////////////////////////////////////////////
// Sensor modes
/**
  @defgroup api_prvm Sensor modes
  The following constants specify sensor modes.
@{
**************************************************************************/
enum {
    AHIP_SENSOR_A         = 0,          ///< Only sensor A is used. @hideinitializer
    AHIP_SENSOR_B         = 1,          ///< Only sensor B is used. @hideinitializer
    AHIP_SENSOR_AB_MERGED = 6,          ///< Input merged from sensors A and B. @hideinitializer
    AHIP_SENSOR_BA_MERGED = 7,          ///< Input merged from sensors B and A. @hideinitializer
};
/** @} end of api_prvm */

///////////////////////////////////////////////////////////////////////////
// AHIP_PREVIEW_CONFIG
/**
  Defines the global preview mode parameters.
@remarks
  In case of merged sensors mode the frame heights must be equal, and widths must
  be aligned on 4. @n
  The resulting frame size is `(size_a.width/2 + size_b.width/2) x (size_a.height)`.
**************************************************************************/
typedef struct
{
    /// Specifies one or more of preview flags. For more information
    /// see @ref api_prvf.
    UINT32  flags;
    /// Specifies the preview frame format. For more information see @ref api_formats.
    /// @note Only ::AHIP_FMT_YUV_420P or ::AHIP_FMT_YUV_422P formats can be set.
    UINT32  format;
    /// Specifies the sensor mode. For more information see @ref api_prvm.
    UINT32  mode;
    /// Specifies the input frame size for sensor interface @b A.
    /// Minimum size is 32x32. Maximum size is 2360 x 8190.
    /// @a width must be even. In case of 4:2:0 format @a height must be even.
    AHIP_SIZE size_a;
    /// Specifies the input frame size for sensor interface @b B.
    /// Minimum size is 32x32. Maximum size is 2360 x 8190.
    /// @a width must be even. In case of 4:2:0 format @a height must be even.
    AHIP_SIZE size_b;
} AHIP_PREVIEW_CONFIG;

///////////////////////////////////////////////////////////////////////////
// AhipInit()
/**
  Performs driver initialization. This function must be called before
  any other function of the API.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  When the application has finished using the AHIP driver, it must call
  ::AhipCleanup in order to free any resources allocated in ::AhipInit. @n
  This function should not be called from an interrupt context.
*/
GTSTATUS AhipInit( void );

///////////////////////////////////////////////////////////////////////////
// AhipOpen()
/**
  Opens the driver.
@param[in] config
  Address of the optional ::AHIP_PREVIEW_CONFIG structure that describes
  preview parameters. Should be NULL in order to open the driver in playback-only mode.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  Individual HW blocks can be started after the driver has been successfully opened. @n
  The driver can not be opened more than once. The application must call
  ::AhipClose in order to close the driver before opening it again. @n
  This function should not be called from an interrupt context.
*/
GTSTATUS AhipOpen( const AHIP_PREVIEW_CONFIG *config );

///////////////////////////////////////////////////////////////////////////
// AhipProcessFrame()
/**
  Processes a single playback frame with PRE and/or VIS blocks.
@param[in] frame
  Address of the ::GT_IMAGE structure that describes the input frame. \n
  The maximum frame resolution is 12288 x 12288. The plane strides and
  addresses should be aligned on 8.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  The function blocks until the HW processing is complete.
  This function can be used only if PRE or VIS are working in playback mode.
*/
GTSTATUS AhipProcessFrame( const GT_IMAGE *frame );

///////////////////////////////////////////////////////////////////////////
// AhipClose()
/**
  Closes the driver.
@remarks
  This function can be called even if the call to ::AhipOpen failed. @n
  This function should not be called from an interrupt context.
*/
void AhipClose( void );

///////////////////////////////////////////////////////////////////////////
// AhipCleanup()
/**
  Terminates use of AHIP driver API. This function must be called
  to allow the driver to free any resources allocated for application.
@remarks
  This function must not be called from an interrupt context. @n
*/
void AhipCleanup( void );

///////////////////////////////////////////////////////////////////////////
// AhipIntHandler()
/**
  Interrupt handler function. Should be called from the HW interrupt context.
@remarks
  The interrupt servicing routine must be connected before calling ::AhipInit
  function.
*/
void AhipIntHandler( void );

/// @} end of api_common
#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region PRE API
#endif
///////////////////////////////////////////////////////////////////////////
// PRE API
/**
  @defgroup api_pre Preprocessing Engine API
  The PRE API serves as a layer between integration code and HW.

  The following code shows the typical sequence of calls:
  @code
    AHIP_PRE_CONFIG cfg;

    // setup the PRE params and start PRE
    AhipPreStart( &cfg, buffer, size );
    //...
    // stop the PRE block
    AhipPreStop();
  @endcode
@{
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
// AHIP_PRE_CONFIG
/**
  Defines the hardware parameters for PRE.
  @note The hardware operates the following pipeline:
  @startuml
    left to right direction
    rectangle "SDRAM" as ram
    rectangle "ISP" as isp
    rectangle "Crop" as crop
    rectangle "Scale" as scale
    rectangle "PRE" as pre
    rectangle "TME-A" as tmea
    rectangle "TME-R" as tmer
    ram --> crop
    isp --> crop
    crop --> scale
    scale --> pre
    pre --> tmea
    pre --> tmer
  @enduml
**************************************************************************/
typedef struct
{
    /// Specifies PRE cropping rectangle inside the input image.
    /// The maximum size is 12288x12288. @a left and @a width must be even.
    /// In case of 4:2:0 format @a top and @a height must be even.
    AHIP_RECT crop;
    /// Specifies the size of PRE hi-res map after scaling. @a width must be
    /// aligned on 4. Minimum size is 32x32. Maximum size is 2560 x 2560.
    /// Can be set to zeros, in which case hi-res map will not be generated.
    AHIP_SIZE size_hi;
    /// Specifies the size of PRE low-res maps after scaling. @a width must be
    /// aligned on 8, @a height must be aligned on 4. Minimum size is 64x64.
    /// Maximum size is 1280 x 1280.
    AHIP_SIZE size_lo;
    /// PRE frame rate multiplier in u0.4 format. It allows to reduce the
    /// applicable processing rate comparing to the input frame rate. For example,
    /// - value 0 means processing rate is equal to input rate;
    /// - value 1 means 1/16 of input rate;
    /// - value 2 means 1/8 of input rate;
    /// - ...
    /// - value 8 means 1/2 of input rate;
    /// - and so on.
    UINT32    frame_rate;
} AHIP_PRE_CONFIG;

///////////////////////////////////////////////////////////////////////////
// AhipPreStart()
/**
  Sets HW parameters and starts asynchronous processing.
@param[in] config
  Address of the ::AHIP_PRE_CONFIG structure that describes HW parameters.
@param[in] buffer
  Address of the buffer that will be used for internal maps allocations.
@param[in] size
  Specifies the size of @a buffer in bytes.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  The application must call ::AhipOpen first in order to use this function.
  The necessary buffer size depends on the configuration parameters.
  The @a buffer must remain valid until ::AhipPreStop is called.
*/
GTSTATUS AhipPreStart( const AHIP_PRE_CONFIG *config, void *buffer, UINT32 size );

///////////////////////////////////////////////////////////////////////////
// AhipPreGetFrameNo()
/**
  Obtains the current value of a free running frame counter. This counter
  will increment on start of frame.
@return
  The return value is a 32-bit counter value.
@remarks
  This function is non-blocking and can be called from any context, including
  interrupt service routine.
*/
UINT32 AhipPreGetFrameNo( void );

///////////////////////////////////////////////////////////////////////////
// AhipVisStop()
/**
  Stops the asynchronous HW processing and disables PRE block.
*/
void AhipPreStop( void );

/// @} end of api_pre
#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region VIS API
#endif
///////////////////////////////////////////////////////////////////////////
// VIS API
/**
  @defgroup api_vis Video Image Stabilization API
  The VIS API serves as a layer between VIS SW library and HW.

  The following code shows the typical sequence of calls:
  @code
    AHIP_VIS_CONFIG cfg;

    // setup the VIS params and start VIS
    AhipVisStart( &cfg, buffer, size );

    while( 1 )
    {
      AHIP_VIS_DATA data;
      // synchronize with HW
      if( AhipVisIO( &data ) != GTS_OK ) break;
      // call VIS library
      // ...
    }
    // stop the VIS block
    AhipVisStop();
  @endcode
@{
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
// AHIP_VIS_CONFIG
/**
  Defines the hardware parameters for VIS.
  @note The hardware operates the following pipeline:
  @startuml
    left to right direction
    rectangle "SDRAM" as ram
    rectangle "ISP" as isp
    rectangle "Crop" as crop
    rectangle "Scale" as scale
    rectangle "AREA" as area
    rectangle "HRE" as hre
    ram --> crop
    isp --> crop
    crop --> scale
    scale --> area
    area --> hre
  @enduml
**************************************************************************/
typedef struct
{
    /// Specifies VIS cropping rectangle inside the input image.
    /// The maximum size is 12288x12288. @a left and @a width must be even.
    /// In case of 4:2:0 format @a top and @a height must be even.
    AHIP_RECT crop;
    /// Specifies the actual size of VIS input after scaling.
    /// Minimum size is 128x128, maximum size is 640x640.
    /// @a width must be aligned on 4. @a height must be aligned on 2.
    AHIP_SIZE size;
    /// Specifies maximum displacement for level #1. The valid range is 1..255.
    UINT8  maxDispH;
    /// Specifies maximum displacement for the rest of levels. The valid range is 1..254.
    UINT8  maxDispL;
    /// Specifies subpixel precision in bits. The valid range is 0..8.
    /// Value of 0 means there is no subpixel correlation.
    UINT8  subPrec;
    /// Specifies the minimum overlap. The valid range is 5..40.
    UINT8  minOverlap;
} AHIP_VIS_CONFIG;

///////////////////////////////////////////////////////////////////////////
// AHIP_VIS_VECTOR
/**
  Contains information about an image motion vector.
**************************************************************************/
typedef struct
{
    /// Contains horizontal shift in pixels in s23.8 format.
    /// Positive value indicates shift right.
    INT32  dx;
    /// Contains horizontal quality factor. Positive value indicates remaining
    /// difference, negative value indicates an error.
    INT32  qx;
    /// Contains vertical shift in pixels in s23.8 format.
    /// Positive value indicates shift down.
    INT32  dy;
    /// Contains vertical quality factor. Positive value indicates remaining
    /// difference, negative value indicates an error.
    INT32  qy;
} AHIP_VIS_VECTOR;

///////////////////////////////////////////////////////////////////////////
// AHIP_VIS_DATA
/**
  Defines the VIS data used for hardware I/O.
**************************************************************************/
typedef struct
{
    /// Contains frame number for which the motion was computed.
    /// The application can use ::AhipVisGetFrameNo function to match VIS metadata
    /// with a corresponding frame.
    UINT32  frame;
    /// Indicates the time of End of Frame in microseconds. 
    /// Only the difference between two time stamps should be used.
    UINT32  timeStamp;
    /// Contains width of the vector field in the @a vector array.
    UINT32  width;
    /// Contains height of the vector field in the @a vector array.
    /// @note The total number of vectors is (width * height).
    UINT32  height;
    /// Array of ::AHIP_VIS_VECTOR structures which contains the local motion
    /// vector field.
    const AHIP_VIS_VECTOR *vector;
} AHIP_VIS_DATA;

///////////////////////////////////////////////////////////////////////////
// AhipVisStart()
/**
  Sets HW parameters and starts asynchronous processing.
  The application should wait for the end of processing using function ::AhipVisIO.
@param[in] config
  Address of the ::AHIP_VIS_CONFIG structure that describes VIS parameters.
@param[in] buffer
  Address of the buffer that will be used for internal allocations.
@param[in] size
  Specifies the size of @a buffer in bytes.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  The application must call ::AhipOpen first in order to use this function. \n
  The necessary buffer size depends on the AHIP_VIS_CONFIG::size. It should
  be at least `(size.width * size.height * 12 + 8192)` bytes. \n
  The @a buffer must remain valid until ::AhipVisStop is called.
*/
GTSTATUS AhipVisStart( const AHIP_VIS_CONFIG *config, void *buffer, UINT32 size );

///////////////////////////////////////////////////////////////////////////
// AhipVisIO()
/**
  Waits for the HW processing to complete and obtains motion vectors.
@param[out] data
  Pointer to the ::AHIP_VIS_DATA structure which receives output data.
@return
  If the function succeeds, the return value is ::GTS_OK. @n
  If the HW processing was stopped by ::AhipVisStop, the return value is ::GTS_STOPPED.
@remarks
  This function must not be called from an interrupt context.
*/
GTSTATUS AhipVisIO( AHIP_VIS_DATA *data );

///////////////////////////////////////////////////////////////////////////
// AhipVisGetFrameNo()
/**
  Obtains the current value of a free running frame counter. This counter
  will increment on start of frame.
@return
  The return value is a 32-bit counter value.
@remarks
  This function is non-blocking and can be called from any context, including
  interrupt service routine.
*/
UINT32 AhipVisGetFrameNo( void );

///////////////////////////////////////////////////////////////////////////
// AhipVisStop()
/**
  Stops the asynchronous HW processing and disables HW blocks. As a result,
  function ::AhipVisIO will return ::GTS_STOPPED.
@remarks
  This function is non-blocking and can be called asynchronously in relation to
  ::AhipVisIO.
*/
void AhipVisStop( void );

/// @} end of api_vis
#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region FD API
#endif
///////////////////////////////////////////////////////////////////////////
// FD API
/**
  @defgroup api_fd Face Detection API
  The FD API serves as a layer between FD software library and HW.

  The following code shows the typical sequence of calls for FD:
  @code
    // setup the FD params and start HW
    AhipFdStart( sw_cfg, buffer, &size );

    while( 1 )
    {
      // synchronize with HW
      if( AhipFdIO( &frame_no ) != GTS_OK ) break;
      // call face detection to use AHIP results
      // ...
    }
    // stop the HW block
    AhipFdStop();
  @endcode

  The following diagram illustrates complete interaction between application,
  SW library and AHIP driver.
  @startuml
    participant "Application" as a
    participant "FD library" as b
    participant "AHIP driver" as c
    !definelong call(x,y,function,ret,color)
    x -> y: ""function""
    activate y #color
    y --> x: ""ret""
    deactivate y
    !enddefinelong

    group Initialization
    call(a, c, AhipInit(), GTSTATUS, DarkSalmon)
    call(a, c, AhipOpen(), GTSTATUS, DarkSalmon)
    call(a, b, lib->vft->CreateHeap(), GTSTATUS, FFBBBB)
    call(a, b, lib->vft->CreateDetector(), GTSTATUS, FFBBBB)
    call(a, c, AhipFdStart(), GTSTATUS, DarkSalmon)
    end
    ...
    group Face detection
    call(a, c, AhipFdIO(), GTSTATUS, DarkSalmon)
    call(a, b, det->vft->DetectFaces(), GTSTATUS, FFBBBB)
    call(a, b, det->vft->EnumFaces(), AHFD_FACE*, FFBBBB)
    end
    ...
    group Cleanup
    call(a, c, AhipFdStop(), void, DarkSalmon)
    call(a, c, AhipClose(), void, DarkSalmon)
    call(a, c, AhipCleanup(), void, DarkSalmon)
    destroy c
    end
  @enduml
@{
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
// AhipFdStart()
/**
  Sets HW parameters and starts asynchronous processing.
  The application should wait for the end of processing using function ::AhipFdIO.
@param[in] config
  Pointer to an opaque structure which specifies the software configuration.
  This structure is returned from SW library.
@param[in] buffer
  Address of the buffer that will contain output maps.
@param[in] size
  Specifies the size of @a buffer in bytes.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  The application must call ::AhipOpen first in order to use this function. \n
  The necessary buffer size should be at least 160K bytes. \n
  The @a buffer must remain valid until ::AhipFdStop is called.
*/
GTSTATUS AhipFdStart( const void *config, void *buffer, UINT32 size );

///////////////////////////////////////////////////////////////////////////
// AhipFdIO()
/**
  Waits for the HW processing to complete and obtains face detection data.
@param[out] frameNo
  Pointer to the caller supplied location to retrieve the frame counter
  corresponding to the frame for which maps were produced. \n 
  The application can use ::AhipPreGetFrameNo function to match FD metadata
  with a corresponding frame.
@return
  If the function succeeds, the return value is ::GTS_OK. @n
  If the HW processing was stopped by ::AhipFdStop, the return value is ::GTS_STOPPED.
@remarks
  This function must not be called from an interrupt context.
*/
GTSTATUS AhipFdIO( UINT32 *frameNo );

///////////////////////////////////////////////////////////////////////////
// AhipFdStop()
/**
  Stops the asynchronous processing and disables HW blocks. As a result,
  function ::AhipFdIO will return ::GTS_STOPPED.
@remarks
  This function is non-blocking and can be called asynchronously in relation to
  ::AhipFdIO.
*/
void AhipFdStop( void );

/// @} end of api_fd
#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region NORM API
#endif
///////////////////////////////////////////////////////////////////////////
// NORM API
/**
  @defgroup api_norm Norm API
  The NORM API serves as a layer between software libraries and HW.

  The following code shows the typical sequence of calls for NORM:
  @code
    // setup the NORM params and start HW
    AhipNormStart( &config );

    while( 1 )
    {
      // send request(s)
      AhipNormSubmitRequest( &frame, &batch, &req );
      // ...
      // retrieve maps
      AhipNormGetResult( req, &maps );
      // ...
      // free request(s)
      AhipNormFreeRequest( req );
    }
    // stop the HW block
    AhipNormStop();
  @endcode
@{
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
// AHIP_NORM_CONFIG
/**
  Defines the global NORM parameters.
**************************************************************************/
typedef struct
{
    /// Specifies fill value for pixels outside of the input image.
    UINT8   fillValue;
    /// Specifies horizontal step in pixels for HOG features.
    /// Valid values: 1, 2, 4.
    UINT8   stepX;
    /// Specifies vertical step in pixels for HOG features.
    /// Valid values: 1, 2, 4.
    UINT8   stepY;
    /// Specifies shift value. The valid range is 0..5.
    UINT8   shift;
    /// Specifies bin selection values.
    UINT32  select[8];
} AHIP_NORM_CONFIG;

///////////////////////////////////////////////////////////////////////////
// AHIP_NORM_FRAME
/**
  Defines the input image structure for NORM.
**************************************************************************/
typedef struct
{
    /// Specifies the number of pixels in the scan line.
    /// The valid range is 16..10240.
    UINT32  width;
    /// Specifies the number of scan lines. The valid range is 16..10240.
    UINT32  height;
    /// Offset, in bytes, between consecutive scan lines.
    /// The valid range is 16..10240. Must be aligned on 16.
    /// Must be equal to or bigger than the @a width.
    UINT32  stride;
    /// Pointer to the first scan line of the image plane.
    /// Must be aligned on 8.
    void   *data;
} AHIP_NORM_FRAME;

///////////////////////////////////////////////////////////////////////////
// ROI configuration flags
/** 
  @defgroup api_nrf ROI flags
  The following constants specify ROI configuration flags.
@{ 
**************************************************************************/
enum {
    AHIP_NRF_DS_AVG     = (1 << 0),     ///< Use average downsampler. @hideinitializer
    AHIP_NRF_DS_NN      = (0 << 0),     ///< Use nearest neighbor downsampler. @hideinitializer
    AHIP_NRF_ROT_BL     = (1 << 1),     ///< Use bilinear rotation. @hideinitializer
    AHIP_NRF_ROT_NN     = (0 << 1),     ///< Use nearest neighbor rotation. @hideinitializer
};
/** @} end of api_nrf */

///////////////////////////////////////////////////////////////////////////
// AHIP_NORM_ROI
/**
  Defines configuration of a single ROI.
  @image html roi.png
**************************************************************************/
typedef struct
{
    /// Specifies one or more of ROI scaling and rotation flags. For more information
    /// see @ref api_nrf.
    UINT32  flags;
    /// Specifies the ROI rotation angle in binary radians with 14 bits precision
    /// (i.e. value 0x4000 means @b &pi; radians).
    /// Positive value indicates the clock-wise rotation.
    INT32   angle;
    /// Specifies the x-coordinate (in s15.8 format) of top-left ROI corner
    /// inside the input frame. Can be negative.
    INT32   left;
    /// Specifies the y-coordinate (in s15.8 format) of top-left ROI corner
    /// inside the input frame. Can be negative.
    INT32   top;
    /// Specifies the ROI size in 15.8 format.
    UINT32  inSize;
    /// Specifies the application area for Ysum. First byte defines internal
    /// square area around the center of ROI, second byte defines external area.
    /// For example, if @a sumArea is {0,2}, then the internal area is comprised
    /// of 1 central pixel, and the external area is comprised of 24 (25-1) pixels.
    UINT8   sumArea[2];
} AHIP_NORM_ROI;

///////////////////////////////////////////////////////////////////////////
// Maps flags
/** 
  @defgroup api_map Maps flags
  The following constants specify which maps are enabled for the batch.
@{ 
**************************************************************************/
enum {
    AHIP_MAP_Y      = (1 << 0),     ///< Enable Y map generation. @hideinitializer
    AHIP_MAP_YSUM   = (1 << 1),     ///< Enable Ysum generation. @hideinitializer
    AHIP_MAP_YEN    = (1 << 2),     ///< Enable YEN map generation. @hideinitializer
    AHIP_MAP_II     = (1 << 3),     ///< Enable II map generation. @hideinitializer
    AHIP_MAP_II2    = (1 << 4),     ///< Enable II2 map generation. @hideinitializer
    AHIP_MAP_CENS   = (1 << 5),     ///< Enable Census map generation. @hideinitializer
    AHIP_MAP_LBP    = (1 << 6),     ///< Enable LBP map generation. @hideinitializer
    AHIP_MAP_HOG    = (1 << 7),     ///< Enable HOG map generation. @hideinitializer
    AHIP_MAP_HOG_N  = (1 << 8),     ///< Enable normalized HOG map generation. Cannot be used with AHIP_MAP_HOG. @hideinitializer
};
/** @} end of api_map */

///////////////////////////////////////////////////////////////////////////
// AHIP_NORM_BATCH
/**
  Defines configuration of a batch of ROIs.
**************************************************************************/
typedef struct
{
    /// Specifies which maps are enabled for the batch. For more information
    /// see @ref api_map.
    UINT32  flags;
    /// Specifies the output size. The valid range is 8..64.
    UINT8   outSize;
    /// Pointer to the Y look-up table.
    /// @note
    /// - The LUT is required for all maps except Y and Ysum.
    /// - The LUT should remain valid until the request is complete.
    const void *lut;
    /// Specifies the number of ROIs. The valid range is 1..255.
    UINT32  numRois;
    /// Pointer to the ROI array.
    const AHIP_NORM_ROI *rois;
} AHIP_NORM_BATCH;

///////////////////////////////////////////////////////////////////////////
// AHIP_NORM_REQ
/**
  Defines an opaque handle to the request object.
**************************************************************************/
typedef struct _AHIP_NORM_REQ* AHIP_NORM_REQ;

///////////////////////////////////////////////////////////////////////////
// AHIP_NORM_MAPS
/**
  Defines the data returned by the NORM hardware. If a particular map wasn't enabled
  then the corresponding pointer would be NULL.
  @note The sizes below are given for one ROI only. In case of multiple ROIs
  the data size is aligned on 8, and the corresponding maps are located at
  multiples of that size.
**************************************************************************/
typedef struct
{
    /// Pointer to the Y map.
    /// The size of data is `(outSize * outSize)` bytes.
    void   *y;
    /// Pointer to the YEN map.
    /// The size of data is `(outSize * outSize)` bytes.
    void   *yen;
    /// Pointer to the II map.
    /// The size of data is `(outSize * outSize * 4)` bytes.
    void   *ii;
    /// Pointer to the II2 map.
    /// The size of data is `(outSize * outSize * 4)` bytes.
    void   *ii2;
    /// Pointer to the Census map.
    /// The size of data is `((outSize - 2) * (outSize - 2))` bytes.
    void   *cens;
    /// Pointer to the LBP map.
    /// The size of data is `((outSize - 2) * (outSize - 2))` bytes.
    void   *lbp;
    /// Pointer to the HOG map.
    /// The size of data depends on the @ref ::AHIP_NORM_CONFIG "configuration".
    void   *hog;
    /// Pointer to the Ysum map.
    /// The size of data is 8 bytes. The map contains a pair of 32-bit words
    /// where the first word is an internal, and the second is an external sum of pixels.
    /// For example, if @ref AHIP_NORM_ROI::sumArea "sumArea" is {0,2}, then 
    /// sum[0] will contain value of 1 central pixel, and sum[1] will contain sum of 24 pixels.
    void   *sum;
} AHIP_NORM_MAPS;

///////////////////////////////////////////////////////////////////////////
// AhipNormStart()
/**
  Performs NORM block initialization.
@param[in] config
  Address of the ::AHIP_NORM_CONFIG structure that describes global NORM parameters.
@param[in] buffer
  Address of the buffer that will be used for internal allocations.
@param[in] size
  Specifies the size of @a buffer in bytes.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  The application must call ::AhipOpen first in order to use this function.
  The @a buffer must remain valid until ::AhipNormStop is called.
*/
GTSTATUS AhipNormStart( const AHIP_NORM_CONFIG *config, void *buffer, UINT32 size );

///////////////////////////////////////////////////////////////////////////
// AhipNormSubmitRequest()
/**
  Submits request to the NORM block.
@param[in] frame
  Address of the ::AHIP_NORM_FRAME structure that describes the input image.
@param[in] batch
  Address of the ::AHIP_NORM_BATCH structure that describes the configuration of a batch of ROIs.
@param[out] req
  Pointer to the caller supplied location to retrieve the handle of the submitted
  request object. The application should call ::AhipNormGetResult in order to
  get the resulting maps.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  This function is non-blocking, and the request will be processed asynchronously.
  In order to get the result, application should call ::AhipNormGetResult. \n
  The application should attempt to do some software processing before asking
  for the result in order to achieve a better parallelism between SW and HW.
*/
GTSTATUS AhipNormSubmitRequest( const AHIP_NORM_FRAME *frame, const AHIP_NORM_BATCH *batch, AHIP_NORM_REQ *req );

///////////////////////////////////////////////////////////////////////////
// AhipNormGetResult()
/**
  Waits for the HW processing to complete and returns the requested data.
@param[in] req
  Request handle returned from ::AhipNormSubmitRequest.
@param[out] maps
  A pointer to a caller supplied variable. On success it will contain the pointer
  to the ::AHIP_NORM_MAPS structure.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  This function will block until the request processing is complete.
  The returned @a maps will remain valid until ::AhipNormFreeRequest is called.
*/
GTSTATUS AhipNormGetResult( AHIP_NORM_REQ req, const AHIP_NORM_MAPS **maps );

///////////////////////////////////////////////////////////////////////////
// AhipNormFreeRequest()
/**
  Releases resources associated with the request.
@param[in] req
  Request handle.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  This function can be called at any time and even before the request has been
  processed.
*/
GTSTATUS AhipNormFreeRequest( AHIP_NORM_REQ req );

///////////////////////////////////////////////////////////////////////////
// AhipNormStop()
/**
  Terminates use of NORM API and releases all associated resources.
@remarks
  This function can be called only after all activity is finished and all
  outstanding requests have been released.
*/
void AhipNormStop( void );

/// @} end of api_norm
#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region MIO API
#endif
///////////////////////////////////////////////////////////////////////////
// MIO API
/**
  @defgroup api_mio Multiple Image Output API
  The MIO API serves as a layer between the application and HW.
@{
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
// AhipMioProcess()
/**
  Does down-sampling and/or conversion to another format.
  Can write up to three output images simultaneously.
@param[in] in
  Pointer to the input image which will be scaled and/or converted to another
  format. Minimum size is 128x128, maximum size is 8192x8192.
@param[out] out1
  Pointer to the optional first output image.
  Size should be equal to the input image.
  Can be NULL, in this case not used.
@param[out] out2
  Pointer to the optional second output image.
  Minimum size is 32x32, maximum size is 2048x2048. Cannot exceed input image size.
  Can be NULL, in this case not used.
@param[out] out3
  Pointer to the optional third output image.
  Minimum size is 32x32, maximum size is 4096x4096. Cannot exceed input image size.
  Can be NULL, in this case not used.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  All output buffers should be allocated by application. \n
  The function blocks until the HW processing is complete. \n
  For all images the plane strides and addresses should be aligned on 8.
  In case of ::AHIP_FMT_YUV_420B format the strides should be aligned on 128. \n
  The format conversion includes:
  - from semi-planar to planar and vice versa;
  - from any of the above to block semi-planar and vice versa (only for YUV 4:2:0).
  @note The format sub-sampling cannot be changed, i.e. it cannot convert YUV 4:2:2 to
  YUV 4:2:0.
*/
GTSTATUS AhipMioProcess( const GT_IMAGE *in, const GT_IMAGE *out1, const GT_IMAGE *out2, const GT_IMAGE *out3 );
/// @} end of api_mio
#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region GDC API
#endif
///////////////////////////////////////////////////////////////////////////
// GDC API
/**
  @defgroup api_gdc Geometric Distortion Correction API
  The GDC API serves as a layer between the application and HW.

  The following code shows the typical sequence of calls for GDC:
  @code
    GT_IMAGE in_frame, out_frame;
    static const AHIP_GDC_CONFIG config = {
        .flags = 0,
        .fill  = { 0, 0x80, 0x80 },
    };
    const GT_IMAGE *output[3] = { &out_frame, NULL, NULL };
    AHIP_GDC_LGRID local;
    float matrix[9];

    // create the GDC context
    AhipGdcCreateContext( &config, buffer, size, &ctx );
    // configure correction
    AhipGdcSetLocal( ctx, 1, &local );
    AhipGdcSetTransform( ctx, &matrix );
    // process frame
    AhipGdcProcess( ctx, &in_frame, output, TRUE );
    // release the context
    AhipGdcFreeContext( ctx );
  @endcode
@{
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
// GDC configuration flags
/** 
  @defgroup api_gdc_flg GDC configuration flags
  The following constants specify GDC configuration flags.
@{ 
**************************************************************************/
enum {
    /// Specifies whether the processing works in the low latency mode.
    /// In this mode the input image is processed as it is being written by ISP.
    AHIP_GDC_LOW_LATENCY    = (1 << 0), ///< @hideinitializer
    /// Specifies whether the processing works in the dynamic global grid mode.
    /// It allows global grid to be written while the processing takes place.
    /// Can be used only with AHIP_GDC_LOW_LATENCY.
    AHIP_GDC_DYN_GGRID      = (1 << 1), ///< @hideinitializer
};
/** @} end of api_gdc_flg */

///////////////////////////////////////////////////////////////////////////
// AHIP_GDC_CONFIG
/**
  Defines the basic hardware parameters for GDC.
**************************************************************************/
typedef struct
{
    /// Specifies @ref api_gdc_flg.
    UINT32 flags;
    /// Specifies (Y,U,V) values of pixels outside of the input image.
    /// In case of a semi-planar format only the first two values will be used.
    UINT8  fill[3];
    /// Specifies Y coordinate at which Y plane should be splitted to process it faster.
    /// Can be 0, in which case the Y plane is not split.
    /// Cannot be used with AHIP_GDC_DYN_GGRID.
    UINT32 split_y;
} AHIP_GDC_CONFIG;

///////////////////////////////////////////////////////////////////////////
// AHIP_GDC_XFORM
/**
  Defines a series of local transforms.
**************************************************************************/
typedef struct
{
    /// Perspective transform matrix. The matrix specifies transformation from
    /// output into input coordinates:
    /// @code
    ///   in_u = (a[0] * out_u + a[1] * out_v + a[2]) / s;
    ///   in_v = (a[3] * out_u + a[4] * out_v + a[5]) / s;
    ///   s    =  a[6] * out_u + a[7] * out_v + a[8];
    /// @endcode
    float   a[9];
    /// Next node index until which the transform should be applied. Can be 0,
    /// in which case the transform is applicable to all remaining nodes.
    UINT32  next;
} AHIP_GDC_XFORM;

///////////////////////////////////////////////////////////////////////////
// AHIP_GDC_LNODE
/**
  Defines local grid node.
**************************************************************************/
typedef struct
{
    /// Horizontal position in pixels on the input image.
    float  u;
    /// Vertical position in pixels on the input image.
    float  v;
} AHIP_GDC_LNODE;

///////////////////////////////////////////////////////////////////////////
// AHIP_GDC_LGRID
/**
  Defines local distortion grid parameters.
**************************************************************************/
typedef struct
{
    /// Grid width in nodes. The valid range is 2..255.
    UINT32  width;
    /// Grid height in nodes. The valid range is 2..1023.
    UINT32  height;
    /// Binary logarithm of the cell width in pixels. The valid range is 2..6
    /// (this corresponds to width 4, 8, ... 64).
    UINT8   pitchX;
    /// Binary logarithm of the cell height in pixels. The valid range is 2..6
    /// (this corresponds to height 4, 8, ... 64).
    UINT8   pitchY;
    /// Horizontal position of the grid in the output image in pixels.
    /// The valid range is -16384..16383.
    INT32   outputX;
    /// Vertical position of the grid in the output image in pixels.
    /// The valid range is -16384..16383.
    INT32   outputY;
    /// Pointer to the array of nodes. Can be NULL, in which case the grid is
    /// considered to be transparent (input coordinates are the same as output ones).
    const AHIP_GDC_LNODE *nodes;
    /// Pointer to the array of local transform matrices.
    /// Used only if the global transform matrix is not set.
    /// Can be NULL, in which case the local transform is not available.
    const AHIP_GDC_XFORM *xform;
} AHIP_GDC_LGRID;

///////////////////////////////////////////////////////////////////////////
// AHIP_GDC_GNODE
/**
  Defines grid node parameters.
**************************************************************************/
typedef struct
{
    /// Horizontal position in pixels on the input image in @b s15.5 format.
    INT32  u;
    /// Vertical position in pixels on the input image in @b s15.5 format.
    INT32  v;
} AHIP_GDC_GNODE;

///////////////////////////////////////////////////////////////////////////
// AHIP_GDC_GGRID
/**
  Defines global distortion grid parameters.
**************************************************************************/
typedef struct
{
    /// Grid width in nodes. The valid range is 4..255.
    UINT32  width;
    /// Grid height in nodes. The valid range is 4..255.
    /// @note The total number of nodes (width * height) cannot exceed 1024.
    UINT32  height;
    /// Horizontal cell pitch, equals to `1/(cell width in pixels)` in 0.16 format.
    UINT32  pitchX;
    /// Vertical cell pitch, equals to `1/(cell height in pixels)` in 0.16 format.
    UINT32  pitchY;
    /// Horizontal position of the input image on the grid (in pixels).
    /// The valid range is -32768..32767.
    INT32   offsetX;
    /// Vertical position of the input image on the grid (in pixels).
    /// The valid range is -32768..32767.
    INT32   offsetY;
    /// Pointer to the array of nodes, the array size is `(width * height)` nodes.
    /// Must be NULL in case of @ref AHIP_GDC_DYN_GGRID "dynamic global grid".
    const AHIP_GDC_GNODE *nodes;
} AHIP_GDC_GGRID;

///////////////////////////////////////////////////////////////////////////
// AHIP_GDC_CONTEXT
/**
  Defines an opaque context object. Pointer to the object serves as a handle.
**************************************************************************/
typedef struct _AHIP_GDC_CONTEXT AHIP_GDC_CONTEXT;

///////////////////////////////////////////////////////////////////////////
// AhipGdcStart()
/**
  Performs common GDC block initialization.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  The application must call ::AhipOpen first in order to use this function.
*/
GTSTATUS AhipGdcStart( void );

///////////////////////////////////////////////////////////////////////////
// AhipGdcCreateContext()
/**
  Creates GDC context object.
@param[in] config
  Address of the ::AHIP_GDC_CONFIG structure that describes global GDC parameters.
@param[in] buffer
  Address of the buffer that will be used for internal allocations.
@param[in] size
  Specifies the size of @a buffer in bytes.
@param[out] pctx
  Pointer to the caller supplied location to retrieve the handle of the
  context object. The application should call ::AhipGdcFreeContext in order to
  destroy the context.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  The application must call ::AhipOpen first in order to use this function. \n
  The @a buffer should be large enough to accomodate the biggest GDC
  configuration that will be used.
  The @a buffer must remain valid until ::AhipGdcFreeContext is called.
*/
GTSTATUS AhipGdcCreateContext( const AHIP_GDC_CONFIG *config, void *buffer, UINT32 size, AHIP_GDC_CONTEXT **pctx );

///////////////////////////////////////////////////////////////////////////
// AhipGdcSetGlobal()
/**
  Sets global distortion grid.
@param[in] ctx
  Context handle.
@param[in] global
  Pointer to the global distortion grid. Can be NULL, in which case the grid is
  considered transparent (input coordinates are the same as output ones).
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  The configuration is converted into an internal format, so the structures
  can be deallocated after this call.
*/
GTSTATUS AhipGdcSetGlobal( AHIP_GDC_CONTEXT *ctx, const AHIP_GDC_GGRID *global );

///////////////////////////////////////////////////////////////////////////
// AhipGdcWriteNodes()
/**
  Writes nodes of the global distortion grid.
@param[in] ctx
  Context handle.
@param[in] num
  Specifies the number of nodes to write.
@param[in] nodes
  Pointer to the array of nodes.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  This function can be used only in dynamic global grid mode.
*/
GTSTATUS AhipGdcWriteNodes( AHIP_GDC_CONTEXT *ctx, UINT32 num, const AHIP_GDC_GNODE *nodes );

///////////////////////////////////////////////////////////////////////////
// AhipGdcSetLocal()
/**
  Sets local distortion grid(s).
@param[in] ctx
  Context handle.
@param[in] numLocal
  Specifies the number of local grids. The valid range is 1..255.
@param[in] local
  Pointer to the array of local distortion grids. Cannot be NULL.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  The configuration is converted into an internal format, so the structures
  can be deallocated after this call.
@remarks
  This function must be called at least once.
*/
GTSTATUS AhipGdcSetLocal( AHIP_GDC_CONTEXT *ctx, UINT32 numLocal, const AHIP_GDC_LGRID *local );

///////////////////////////////////////////////////////////////////////////
// AhipGdcSetTransform()
/**
  Sets global transform matrix.
@param[in] ctx
  Context handle.
@param[in] xform
  Pointer to the global transform matrix. The matrix specifies
  transformation from output into input coordinates:
  @code
    in_u = (xform[0] * out_u + xform[1] * out_v + xform[2]) / s;
    in_v = (xform[3] * out_u + xform[4] * out_v + xform[5]) / s;
    s    =  xform[6] * out_u + xform[7] * out_v + xform[8];
  @endcode
  Can be NULL, in which case the global transform will not be available.
@return
  If the function succeeds, the return value is ::GTS_OK.
@remarks
  The configuration is converted into an internal format, so the array
  can be deallocated after this call.
*/
GTSTATUS AhipGdcSetTransform( AHIP_GDC_CONTEXT *ctx, const float xform[9] );

///////////////////////////////////////////////////////////////////////////
// AhipGdcProcess()
/**
  Performs geometric distortion correction on an image.
  Optionally can down-sample the main output image.
@param[in] ctx
  Context handle.
@param[in] in
  Pointer to the input image.
  Minimum size is 32x32, maximum size is 8192x8192.
@param[out] out0
  Pointer to the main output image. Cannot be NULL.
@param[out] out1
  Pointer to the optional down-sampled output image.
  The size can be from 1/16x to 1x of the main output image size.
  Can be NULL, in this case not used.
@param[out] out2
  Pointer to the optional down-sampled output image.
  The size can be either 1/4x, 1/8x, 1/16x, or 1/32x of the main output image size.
  Can be NULL, in this case not used.
@param[in] wait
  If not 0, wait until processing is finished.
@return
  If the function succeeds, the return value is either ::GTS_WAIT or ::GTS_OK.
@remarks
  This function initiates the asynchronous HW processing and returns.
  The application should call ::AhipGdcWait to wait until the processing ends.
  The image buffers must remain valid until ::AhipGdcWait returns.
@remarks
  The function uses configuration set by ::AhipGdcSetGlobal,
  ::AhipGdcSetLocal, ::AhipGdcSetTransform functions.
@remarks
  For all images the plane strides and addresses should be aligned on 8. \n
  Can convert from 4:2:0 semi-planar to block semi-planar format and vice versa.
  Additionally to normal @ref api_formats it supports @ref L_Gray "GT_FMT_GRAY".
  All output images should have the same format.
*/
GTSTATUS AhipGdcProcess( AHIP_GDC_CONTEXT *ctx, const GT_IMAGE *in, const GT_IMAGE *out0,
    const GT_IMAGE *out1, const GT_IMAGE *out2, BOOL wait );

///////////////////////////////////////////////////////////////////////////
// AhipGdcWait()
/**
  Waits until the processing initiated by AhipGdcStart is complete.
@param[in] ctx
  Context handle.
@return
  If the function succeeds, the return value is ::GTS_OK.
*/
GTSTATUS AhipGdcWait( AHIP_GDC_CONTEXT *ctx );

///////////////////////////////////////////////////////////////////////////
// AhipGdcFreeContext()
/**
  Releases resources associated with the context.
@param[in] ctx
  Context handle.
*/
void AhipGdcFreeContext( AHIP_GDC_CONTEXT *ctx );

///////////////////////////////////////////////////////////////////////////
// AhipGdcStop()
/**
  Terminates use of GDC API and releases all associated resources.
*/
void AhipGdcStop( void );

/// @} end of api_gdc
#ifndef __GNUC__
#pragma endregion
#endif

/// @} end of API

#ifdef __cplusplus
}
#endif
