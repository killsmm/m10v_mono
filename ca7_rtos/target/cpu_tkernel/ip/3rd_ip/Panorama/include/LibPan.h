/*------------------------------------------------------------------------------

  LibPan API
  Copyright (c) FotoNation.
  All rights reserved. Confidential.

------------------------------------------------------------------------------*/

#ifndef _LIBPAN_H_
#define _LIBPAN_H_

#include "gtypes.h"

#ifdef  __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
 *  API Version
 *----------------------------------------------------------------------------*/
/**
@weakgroup panAPI_Ver

  API Version

  The API version is a 32-bit value where the higher 16 bits represent the
  major version and the lower 16 bits represent the minor version.
  When the minor is increased the API remains backward compatible. When
  the major is increased the API may not be backward compatible.

  To retrieve the API version, call PAN_LIB::GetLibInfo() and read
  the PAN_LIB_INFO::apiVer field. It is different from the library version
  (PAN_LIB_INFO::libVer). Library version refers to the implementation,
  however API version refers only to the API definition.

  The following is the list of API versions and the corresponding changes:

@par 0x000A0002
  - Asynchronous image transformation callback functions
@par 0x000A0001
  - Optical Center Coordinates Specification in PAN_CFG.
@par 0x00090001
  - First LibPan API.

*******************************************************************************/

/*------------------------------------------------------------------------------
 *  GTS_PAN_*
 *----------------------------------------------------------------------------*/
/**
  Status Codes
@addtogroup gtAPI_Status
@{
*******************************************************************************/

/** @name LibPan specific error codes */
/** @{ */
#define GTS_PAN_FRAME_ADDED      ( 101) /**< Frame was added. */
#define GTS_PAN_DONE             ( 102) /**< Reached end of panorama. */
#define GTS_PAN_LOW_CONTRAST     (-101) /**< Not enough "features" in the scene. */
#define GTS_PAN_OUT_OF_SCENE     (-102) /**< Image is out of scene and not good for stitching. */
#define GTS_PAN_HIGH_SPEED       (-103) /**< The speed of panning is too high. */
#define GTS_PAN_WRONG_DIRECTION  (-104) /**< Direction of panning is wrong. */
#define GTS_PAN_BIG_DISPLACEMENT (-105) /**< Orthogonal offset exceeds specified in PAN_CFG::maxDisplacement. */
/** @} */

/** @} end of gtAPI_Status */

/*------------------------------------------------------------------------------
 *  PAN_FEAT
 *----------------------------------------------------------------------------*/
/**
  Used in PAN_LIB_INFO::features.
@ingroup panAPI_Enums
**************************************************************************/
enum PAN_FEAT
{
    /** Library implements a custom heap. See PAN_LIB::CreateHeap().
    */
    PAN_FEAT_HEAP   = (1 << 0)
};

/*------------------------------------------------------------------------------
 *  PAN_FLAG
 *----------------------------------------------------------------------------*/
/**
  Used in PAN_CFG::flags.
@ingroup panAPI_Enums
**************************************************************************/
enum PAN_FLAG
{
    /** Indicates to not crop panorama such that it may contain black
        areas where there is no scene due to projection and movement.
    */
    PAN_FLAG_LEAVE_BLACK_AREA   = (1 << 0),

    /** Indicates that panorama stitching will be done on the fly in real
        time when PAN_STITCHER_VFT::ProcessFrame() is called per each frame
        during the acquisition. Otherwise the stitching is done in
        PAN_STITCHER_VFT::Stitch(), after all frames are acquired. In the
        later case, the library requires to store the intermediate images
        in the buffer specified in PAN_CFG::keyFramesBuf.
    */
    PAN_FLAG_REALTIME_STITCH    = (1 << 1),

    /** Indicates that the application supplies to PAN_STITCHER_VFT::ProcessFrame()
        motion vectors obtained with [HRE](@ref panAPI_HRE).
    */
    PAN_FLAG_USE_HRE            = (1 << 2),

    /** Indicates that the application supplies gyroscope samples.
        See PAN_STITCHER_VFT::AddGyroSample().
    */
    PAN_FLAG_USE_GYRO           = (1 << 3)
};

/*------------------------------------------------------------------------------
 *  PAN_DIR
 *----------------------------------------------------------------------------*/
/**
  Used in PAN_CFG::direction.
@ingroup panAPI_Enums
**************************************************************************/
enum PAN_DIR
{
    /** Panning direction is determined automatically.
    */
    PAN_DIR_AUTO = 0,

    /** Panning direction is negative in respect to image coordinates.
        For a horizontal panorama and orientation = 0, this corresponds
        to panning from right to left.
    */
    PAN_DIR_BACKWARD = 1,

    /** Panning direction is positive in respect to image coordinates.
        For a horizontal panorama and orientation = 0, this corresponds
        to panning from left to right.
    */
    PAN_DIR_FORWARD = 2
};

/*------------------------------------------------------------------------------
 *  PAN_VECTOR
 *----------------------------------------------------------------------------*/
/**
  Specifies a motion vector returned by [HRE](@ref panAPI_HRE).
*******************************************************************************/
typedef struct 
{
    /** Horizontal shift in pixels as 23.8 fixed point format.
        Positive value indicates right shift.
    */
    INT32  dx;

    /** Horizontal quality factor. Positive value indicates remaining difference.
        Negative value indicates an error.
    */
    INT32  qx;

    /** Vertical shift in pixels as 23.8 fixed point format.
        Positive value indicates down shift.
    */
    INT32  dy;

    /** Vertical quality factor. Positive value indicates remaining difference.
        Negative value indicates an error.
    */
    INT32  qy;

} PAN_VECTOR;

/*------------------------------------------------------------------------------
 *  PAN_NODE
 *----------------------------------------------------------------------------*/
/**
  Specifies a node in the transformation grid.
*******************************************************************************/
typedef struct 
{
    float u; /**< Horizontal position in input image in pixels. */
    float v; /**< Vertical position in input image in pixels. */
} PAN_NODE;

/*------------------------------------------------------------------------------
 *  PAN_LGRID
 *----------------------------------------------------------------------------*/
/**
  Specifies a local transformation grid.
*******************************************************************************/
typedef struct 
{
    GT_SIZE   gridSize; /**< Grid dimension in nodes. */
    GT_SIZE   cellSize; /**< Dimension in pixels between nodes. Must be a power of 2. */
    GT_POINT  outPos;   /**< Position in pixels of top left node on the output image. */
    PAN_NODE* nodes;    /**< Pointer to a 2D array [gridSize.height][gridSize.width] of nodes. */
} PAN_LGRID;

/*------------------------------------------------------------------------------
 *  PAN_IMU_SAMPLE
 *----------------------------------------------------------------------------*/
/**
  Represents a generic 3 axis data sampled from the Inertial Measurement Unit
  (IMU) sensor.

  - The sample values must be in the following units:
    - Gyroscope: rad/sec
    - Accelerometer: G (g-force)
  - Positive sign means counterclockwise rotation (standard for most gyroscopes). 
  - Axes orientation in respect to the image plane:
    - X - on image sensor plane, perpendicular to image lines, positive direction
          corresponds to decreasing line number.
    - Y - on image sensor plane, paralel to image lines, positive direction
          corresponds to decreasing pixel number.
    - Z - perpendicular to the image sensor plane, positive direction is
          "towards" as in the below image.
  - See [Acquiring IMU samples](@ref panAPI_IMU).
  .
  @image html gyro_orientation.png
*******************************************************************************/
typedef struct
{
    float x; /**< X-axis value */
    float y; /**< Y-axis value */
    float z; /**< Z-axis value */
} PAN_IMU_SAMPLE;

/*------------------------------------------------------------------------------
 *  PAN_TRANSFORM_OP
 *----------------------------------------------------------------------------*/
/**
  Represents an image transformation operation used in #PAN_TRANSFORM_ROUTINE.
  See [image transformation](@ref panAPI_Transform).
*******************************************************************************/
typedef struct
{
    /** The size, in bytes, of this structure. Set to sizeof(PAN_TRANSFORM_OP)
        before calling any function that uses this structure.
    */
    UINT32 structSize;

    /** The input image before transformation.
        The color format is either GT_FMT_GRAY or same as in PAN_CFG::frameFormat.
    */
    GT_IMAGE* inImg;

    /** The output image after transformation.
        The color format is either GT_FMT_GRAY or same as in PAN_CFG::frameFormat.
    */
    GT_IMAGE* outImg;

    /** Pointer to local transformation grid.
    */
    PAN_LGRID* localGrid;
} PAN_TRANSFORM_OP;

typedef struct PAN_STITCHER PAN_STITCHER;
/*------------------------------------------------------------------------------
 *  PAN_TRANSFORM_START_ROUTINE
 *----------------------------------------------------------------------------*/
/**
  Pointer to the function that starts the image transformation.
  @param [in] st
    Pointer to the stitcher object.
  @param [in] context
    User defined pointer provided in PAN_CFG::transformContext.
  @param [in] op
    Describes image transformation operation.
    - The data is guaranteed to be consistent and valid until
      PAN_TRANSFORM_COMPLETE_ROUTINE is called.
    @return
    - #GTS_OK - success.
    - otherwise - other error.
  @remarks
    - See [image transformation](@ref panAPI_Transform).
  @ingroup panAPI_Stitcher
*******************************************************************************/
typedef GTSTATUS (GTCALL* PAN_TRANSFORM_START_ROUTINE)(
    IN PAN_STITCHER* st,
    IN void* context,
    IN const PAN_TRANSFORM_OP* op);

/*------------------------------------------------------------------------------
 *  PAN_TRANSFORM_COMPLETE_ROUTINE
 *----------------------------------------------------------------------------*/
/**
  Pointer to the function that obtains the results of image transformation.
  @param [in] st
    Pointer to the stitcher object.
  @param [in] context
    User defined pointer provided in PAN_CFG::transformContext.
  @param [in] op
    Describes image transformation operation.
  @return
    - #GTS_OK - success.
    - otherwise - other error.
  @remarks
    - See [image transformation](@ref panAPI_Transform).
  @ingroup panAPI_Stitcher
*******************************************************************************/
typedef GTSTATUS(GTCALL* PAN_TRANSFORM_COMPLETE_ROUTINE)(
    IN PAN_STITCHER* st,
    IN void* context,
    IN const PAN_TRANSFORM_OP* op);

/*------------------------------------------------------------------------------
 *  PAN_CFG
 *----------------------------------------------------------------------------*/
/**
  Contains configuration information.
  - The fields marked with (*) are used in PAN_LIB::GetParams().
  - All the fields are used in PAN_LIB::CreateStitcher().
*******************************************************************************/
typedef struct 
{
    /** (*) The size, in bytes, of this structure. Set to sizeof(PAN_CFG)
        before calling any function that uses this structure.
    */
    UINT32 structSize;

    /** (*) Bitmask containing configuration flags, see #PAN_FLAG.
    */
    UINT32 flags;

    /** (*) The image color format.
        See [supported formats](@ref panAPI_SupportedFmts).
    */
    UINT32 frameFormat;

    /** (*) The resolution in pixels of input frames.
    */
    GT_SIZE frameSize;

    /** (*) Frame orientation to be used at stitching. Valid values are:
      - 0 - frames are stitched along their width.
      - +90 - frames are added along their height.
    */
    UINT32 frameOrientation;

    /** (*) Focal length in pixels corresponding to frame resolution
        specified in PAN_CFG::frameSize.
    */
    UINT32 focalLength;

    /** (*) Optical center coordinates in pixels corresponding to frame resolution
        specified in PAN_CFG::frameSize.
        Note: For an ideal lens this is the center of the image.
    */
    GT_POINT opticalCenter;

    /** (*) Panning direction as specified in ::PAN_DIR.
    */
    UINT32 direction;

    /** (*) The crop of the input frame that is used to calculate the motion.
        See [setting working size](@ref panAPI_WorkSize).
    */
    GT_RECT workCrop;

    /** (*) The down-scaled resolution of the working image that is used to
        calculate the motion.
        See [setting working size](@ref panAPI_WorkSize).
    */
    GT_SIZE workSize;

    /** (*) The biggest deviation from a "straight" panning that should be accepted
        by the library. For example, when a horizontal panorama is acquired, the
        user's movement might not be accurate and she can deviate up or down from
        the original straight line. For a vertical panorama the deviation is counted
        to the left or right.
        - The displacement is specified in % of the frame height (orientation=0)
          or width (orientation=90).
        - If the displacement is bigger than specified, #GTS_PAN_BIG_DISPLACEMENT
          error is returned and panorama is finished.
        - If zero, displacement is ignored.
    */
    UINT32 maxDisplacement;

    /** (*) The maximum panorama length in pixels. If the user
        does not cancel the acquisition and no errors occur,
        the panorama finishes the panorama when this length
        is reached.
    */
    UINT32 panoramaLength;

    /** Pointer to the application supplied buffer for storing resulting 
        panorama image. The size of this buffer is provided by #panoramaBufSize.
    */
    void* panoramaBuf;

    /** Size in bytes of the #panoramaBuf buffer.
        Can be obtained from PAN_PARAMS::panoramaBufSize.
    */
    UINT32 panoramaBufSize;

    /** Pointer to the application supplied buffer for intermediate
        panorama images. This buffer is needed only if
        #PAN_FLAG_REALTIME_STITCH flag is not set.
    */
    void* keyFramesBuf;

    /** Size in bytes of the #keyFramesBuf buffer.
        Can be obtained from PAN_PARAMS::keyFramesBufSize.
    */
    UINT32 keyFramesBufSize;

    /** A pointer to an application function that is called to
        start image transformation.
    */
    PAN_TRANSFORM_START_ROUTINE transformStart;

    /** A pointer to an application function that is called to
        obtain image transformation results.
    */
    PAN_TRANSFORM_COMPLETE_ROUTINE transformComplete;

    /** An application defined pointer that is passed by the
        library to #transformRoutine function.
    */
    void* transformContext;
} PAN_CFG;

/*------------------------------------------------------------------------------
 *  PAN_PARAMS
 *----------------------------------------------------------------------------*/
/**
  Contains stitcher parameters.
  The application obtains this structure by calling PAN_LIB::GetParams().
*******************************************************************************/
typedef struct 
{
    /** The size, in bytes, of this structure. Set to sizeof(PAN_PARAMS)
        before calling any function that uses this structure.
    */
    UINT32 structSize;

    /** Specifies the amount of heap required by the library (in bytes).
        The application sets this size when calling PAN_LIB::CreateHeap().
        @note Please note, different heap managers may require supplementary 
        memory due to overhead and fragmentation.
     */
    UINT32 heapSize;

    /** Size in bytes for the intermediate buffer storing key frame images
        which the panorama will be built from.
        The application sets this size in PAN_CFG::keyFramesBufSize and
        provides a buffer of respective size in PAN_CFG::keyFramesBuf upon
        calling PAN_LIB::CreateStitcher().
     */
    UINT32 keyFramesBufSize;

    /** Size in bytes for the panorama image buffer.
        The application sets this size in PAN_CFG::panoramaBufSize and
        provides a buffer of respective size in PAN_CFG::panoramaBuf upon
        calling PAN_LIB::CreateStitcher().
     */
    UINT32 panoramaBufSize;
} PAN_PARAMS;

/*------------------------------------------------------------------------------
 *  PAN_STITCHER
 *----------------------------------------------------------------------------*/
/**
  Represents the stitcher interface.
  To retrieve this structure, call PAN_LIB::CreateStitcher().
@ingroup panAPI_Stitcher
*******************************************************************************/
struct PAN_STITCHER
{
    /** Pointer to the function table. */
    const struct PAN_STITCHER_VFT* vft;
};

/*------------------------------------------------------------------------------
 *  PAN_STITCHER_VFT
 *----------------------------------------------------------------------------*/
/**
  Contains pointers to PAN_STITCHER functions.
@ingroup panAPI_Stitcher
*******************************************************************************/
struct PAN_STITCHER_VFT
{
    /* Destroy()
    */
    /**
      Destroys the stitcher object.
    @param [in] st
      Pointer to the stitcher object to be destroyed.
    */
    void (GTCALL* Destroy)(IN PAN_STITCHER* st);

    /* AddGyroSample()
    */
    /**
      Adds a gyro sample.
    @param [in] st
      Pointer to the stitcher object.
    @param [in] timeStamp
      Time when sample was aquired in microseconds. This time
      must be synchronized with frame acquisition. See the
      timeStamp parameter in PAN_STITCHER_VFT::ProcessFrame().
    @param [in] sample
      Gyroscope rates in rad/s.
    @return
      - #GTS_OK - success.
      - otherwise - error.
    @remarks
      - Used when #PAN_FLAG_USE_GYRO is set.
      - The sampling frequency must be at least 100 Hz.
      - See [Acquiring IMU samples](@ref panAPI_IMU).
    */
    GTSTATUS (GTCALL* AddGyroSample)(
        IN PAN_STITCHER* st,
        IN UINT32 timeStamp,
        IN const PAN_IMU_SAMPLE* sample);

    /* ProcessFrame()
    */
    /**
      Process the acquired frame.
    @param [in] st
      Pointer to the stitcher object.
    @param [in] frame
      Input frame image.
      - Format must be as in PAN_CFG::frameFormat.
      - Image size must be as in PAN_CFG::frameSize.
      - Each plane address, as well as strides, must be aligned to 16 bytes.
    @param [in] workImage
      (Optional) Downsampled input image.
      - Ignored if motion parameter is not NULL.
      - Format must be GT_FMT_GRAY.
      - Image size must be as in PAN_CFG::workSize.
      - See [setting working size](@ref panAPI_WorkSize).
    @param [in] motion
      Pointer to the caller allocated array of PAN_VECTOR structures which
      will contain motion vectors returned by the [HRE](@ref panAPI_HRE) module.
      - The length of this array must be 256 vectors, which is an array
        of 16 rows, each row has 16 vectors.
      - The vectors contain the displacement of the frame in pixels at the
        [working resolution](@ref panAPI_WorkSize).
      - If #PAN_FLAG_USE_HRE is set, motion must not be NULL,
        otherwise must be NULL.
    @param [in] timeStamp
      (Optional) Time of Start of Frame in microseconds.
      - Only the differences between two time stamps is used.
      - Ignored if #PAN_FLAG_USE_GYRO is not set.
    @param [in] isLast
      TRUE if user wants to terminated panorama earlier.
    @return
      - #GTS_OK - success. The frame is ignored and will not be added to panorama.
        Call this function again for the next frame.
      - #GTS_PAN_FRAME_ADDED - success. The frame will be added to panorama.
        Call this function again for the next frame.
      - #GTS_PAN_DONE - success. This is the last frame that will be added to panorama.
        Do not call this function again.
      - If one of the below errors occur, no more frames can be accepted and panorama
        will be built only upon previously processed frames:
        - #GTS_PAN_LOW_CONTRAST - not enough "features" in the scene.
        - #GTS_PAN_OUT_OF_SCENE - image is out of scene and not good for stitching.
        - #GTS_PAN_HIGH_SPEED - the speed of panning is too high.
        - #GTS_PAN_WRONG_DIRECTION - direction of panning is wrong.
        - #GTS_PAN_BIG_DISPLACEMENT - orthogonal offset exceeds specified in
          PAN_CFG::maxDisplacement.
        - otherwise - other error.
    @remarks
      - This function may call PAN_CFG::transformRoutine in order to perform
        image transformation.
      - If #PAN_FLAG_REALTIME_STITCH is set then it stitches the current frame
        to the resulting panorama image.
        - Otherwise, it copies the required portion of the frame into
          PAN_CFG::keyFramesBuf. The actual stitching is performed by calling
          Stitch(), after ProcessFrame() has been called for all frames.
      - Dropped frame:
        - If the #PAN_FLAG_USE_HRE flag is set and motion vectors have been
          calculated for this frame, but the frame image has been lost, then
          application sets the 'frame' parameter to NULL and 'motion' parameter
          must not be NULL.
        - If the #PAN_FLAG_USE_HRE flag is not set, then the 'frame' parameter
          must not be NULL. The application just skips calling ProcessFrame()
          for lost frames as the motion vectors have not been calculated.
    */
    GTSTATUS (GTCALL* ProcessFrame)(
        IN PAN_STITCHER* st,
        IN const GT_IMAGE* frame,
        IN const GT_IMAGE* workImage OPTIONAL,
        IN const PAN_VECTOR* motion OPTIONAL,
        IN UINT32 timeStamp OPTIONAL,
        IN BOOL isLast);

    /* GetDirection()
    */
    /**
      Returns the direction of panning.
    @param [in] st
      Pointer to the stitcher object.
    @return
      - A value specified in PAN_DIR.
      - If the panning direction is not yet determined, it returns PAN_DIR_AUTO.
    */
    UINT32 (GTCALL* GetDirection)(
        IN PAN_STITCHER* st);

    /* GetOffset()
    */
    /**
      Returns the offset between first panorama frame and current frame.
    @param [in] st
      Pointer to the stitcher object.
    @param [out] offset
      - The offset in pixels between:
        - (first) the first frame that was added to panorama;
        - (last) the current frame that was passed to ProcessFrame().
      - The position of first frame is substracted from last frame.
      - The offset is calculated in the input frame resolution as specified in
        PAN_CFG::frameSize.
      - In case of error, the offset is set to zero.
    @return
      - #GTS_OK - success.
      - otherwise - other error.
    */
    GTSTATUS (GTCALL* GetOffset)(
        IN PAN_STITCHER* st,
        OUT GT_SIZE* offset);

    /* Stitch()
    */
    /**
      Stitch the frames and return a panorama image.
    @param [in] st
      Pointer to the stitcher object.
    @param [out] panorama
      Panorama result image. The image data is allocated in the buffer
      set in PAN_CFG::panoramaBuf.
    @return
      - #GTS_OK - success.
      - otherwise - other error.
    @remarks
      - If #PAN_FLAG_REALTIME_STITCH is set then this function merely returns
        the image that was already stitched during ProcessFrame() calls. Only
        minor tuning may take place.
        - Otherwise, it stitches the panorama image from image fragments
          previously saved in PAN_CFG::keyFramesBuf by ProcessFrame().
          In this case it also may calls PAN_CFG::transformRoutine for each
          frame in order to perform image transformation.
    */
    GTSTATUS (GTCALL* Stitch)(
        IN PAN_STITCHER* st,
        OUT GT_IMAGE* panorama);
};

/*------------------------------------------------------------------------------
 *  PAN_LIB_INFO
 *----------------------------------------------------------------------------*/
/**
  Contains information about the library version.
  See PAN_LIB::GetLibInfo().
*******************************************************************************/
typedef struct PAN_LIB_INFO
{
    /** The size, in bytes, of this structure. 
    */
    UINT32 structSize;

    /** The library version numbers, first element is the major number.
    */
    UINT32 libVer[5];

    /** @ref panAPI_Ver "API version" number.
    */
    UINT32 apiVer;

    /** Pointer to the string specifying build date in form "Mmm dd yyyy hh:mm:ss".
        Same format as in __DATE__ and __TIME__ macros.
    */
    const char* buildDate;

    /** Pointer to the optional string specifying the release label.
    */
    const char* label;

    /** A bitmask specifying supported features. See #PAN_FEAT.
    */
    UINT32 features;

    /** An array of @ref panAPI_SupportedFmts "supported color formats" with
        the last item set to zero.
    */
    const UINT32* supportedFmts;

} PAN_LIB_INFO;

/*------------------------------------------------------------------------------
 *  PAN_LIB
 *----------------------------------------------------------------------------*/
/**
  Contains pointers to functions which represent the library interface.
  To retrieve this structure, call PanLibGet().
@ingroup panAPI_Lib
*******************************************************************************/
typedef struct PAN_LIB
{
    /* GetLibInfo()
    */
    /** Returns information about library version.
    @return
      A pointer to PAN_LIB_INFO containing version information.
    */
    const PAN_LIB_INFO* (GTCALL* GetLibInfo)(void);

    /* GetParams()
    */
    /** Returns parameters for the stitcher object.
    @param [in] cfg
      Incomplete stitcher configuration upon which the library
      shall calculate the parameters.
      - See in #PAN_CFG which fields are required for this call.
    @param [in,out] params
      The parameters of the stitcher object.
      - On input the application must set structSize field.
      - On output the library sets the rest of the fields.
    @return
      - #GTS_OK - success.
      - otherwise - other error.
    @remarks
      - The application calls this function before
        PAN_LIB::CreateHeap() and PAN_LIB::CreateStitcher()
        to obtain the configuration fields that can be
        calculated by the library.
    */
    GTSTATUS (GTCALL* GetParams)(
        IN const PAN_CFG* cfg,
        IN OUT PAN_PARAMS* params);

    /* CreateHeap()
    */
    /** Creates a heap object from the input buffer.
    @param [in] lib
       A pointer to the library object.
    @param [in] buffer
       Pointer to a buffer that shall be used as a heap.
    @param [in] size
       Size of the buffer, in bytes.
       Can be obtained from PAN_PARAMS::heapSize.
    @param [out] heap
       A pointer to a caller supplied variable. On success it will contain the 
       pointer to the newly created heap object.
    @return
       - #GTS_OK - success.
       - #GTS_NOT_SUPPORTED - this functionality is not included in this
         library version.
       - Otherwise - other error.
    @remarks
       This function is intended for platforms without own heap management.
       It uses provided buffer to create a heap that can be supplied to 
       PAN_LIB::CreateStitcher(). After the heap is not needed, call
       GT_HEAP_VFT::Destroy().
    @warning
       The heap created by this function is not thread safe.
       If multiple stitcher objects are run in parallel, then each
       must be supplied with separate heaps created by this function.
       See also [thread safety](@ref panAPI_Threading).
    */
    GTSTATUS (GTCALL* CreateHeap)(
        IN void* buffer,
        IN UINT32 size,
        OUT GT_HEAP** heap);

    /* CreateStitcher()
    */
    /** Creates the stitcher object.
    @param [in] heap
       Address of an optional heap object that shall be used for memory 
       management.
       There are three options of providing the heap:
       - NULL value: use the default heap.
         If heap parameter is NULL then:
         - If the library supports a default heap for given platform,
           it will be used instead.
         - Otherwise #GTS_NO_SYSTEM_RESOURCES error is returned.
       - Call PAN_LIB::CreateHeap() to create a heap from a buffer.
       - Implement a custom GT_HEAP. See @ref gtAPI_Heap for an example.
       .
    @param [in] cfg
       Pointer to the configuration structure.
    @param [out] st
       A pointer to a caller supplied variable. On success it will contain the 
       pointer to the newly created stitcher object.
    @return
       - #GTS_OK - success.
       - Otherwise - error.
    @remarks
      - When the stitcher object is no longer needed, call PAN_STITCHER_VFT::Destroy().
      - If the application provided a non NULL heap, then it is responsible for
        destroying the heap after destroying the stitcher.
      - The application can create multiple stitcher objects and run them
        in parallel. See also [thread safety](@ref panAPI_Threading).
    */
    GTSTATUS (GTCALL* CreateStitcher)(
        IN GT_HEAP* heap OPTIONAL,
        IN const PAN_CFG* cfg,
        OUT PAN_STITCHER** st);

} PAN_LIB;

/*------------------------------------------------------------------------------
 *  PanLibGet()
 *----------------------------------------------------------------------------*/
/**
  Library entry point function.
@return
  The pointer to PAN_LIB structure representing the library interface.
@remarks
  This is the first function that needs to be called. The PAN_LIB
  structure gives access to other function pointers.
@ingroup panAPI_Lib
*******************************************************************************/
GTAPI const PAN_LIB* GTCALL PanLibGet(void);

#ifdef __cplusplus
}
#endif

#endif /*  _LIBPAN_H_ */
