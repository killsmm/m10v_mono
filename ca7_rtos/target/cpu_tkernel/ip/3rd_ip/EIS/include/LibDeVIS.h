/*------------------------------------------------------------------------------

  LibDeVIS API
  Copyright (c) FotoNation.
  All rights reserved. Confidential.

------------------------------------------------------------------------------*/

#ifndef _LIBDEVIS_H_
#define _LIBDEVIS_H_

#include "gtypes.h"

#ifdef  __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
 *  API Version
 *----------------------------------------------------------------------------*/
/**
@weakgroup dvsAPI_Ver

  API Version

  The API version is a 32-bit value where the higher 16 bits represent the
  major version and the lower 16 bits represent the minor version.
  When the minor is increased the API remains backward compatible. When
  the major is increased the API may not be backward compatible.

  To retrieve the API version, call DVS_LIB::GetLibInfo() and read
  the DVS_LIB_INFO::apiVer field. It is different from the library version
  (DVS_LIB_INFO::libVer). Library version refers to the implementation,
  however API version refers only to the API definition.

  The following is the list of API versions and the corresponding changes:

@par 0x00020001
  - Moved focalLength from DVS_FRAME_INFO to DVS_CFG.
    It now has X and Y components.
  - Moved opticalCenterX,Y from DVS_FRAME_INFO to DVS_CFG.
@par 0x00010001
  - First API.

*******************************************************************************/

/*------------------------------------------------------------------------------
 *  GTS_DVS_*
 *----------------------------------------------------------------------------*/
/**
  Status Codes
@addtogroup gtAPI_Status
@{
*******************************************************************************/

/** @name DVS specific status codes */
/** @{ */
#define GTS_DVS_IN_PROGRESS     ( 100) /**< More processing required. */
#define GTS_DVS_GRID_NOT_READY  (-101) /**< The correction grid is not ready. */
/** @} */

/** @} end of gtAPI_Status */

/*------------------------------------------------------------------------------
 *  DVS_FRAME_INFO
 *----------------------------------------------------------------------------*/
/**
  Contains information about the current video frame.
*******************************************************************************/
typedef struct
{
    /** The size, in bytes, of this structure. Set to sizeof(DVS_FRAME_INFO)
        before calling any function that uses this structure.
    */
    UINT32 structSize;

    /** Indicates the time of Start of Frame in microseconds.
        - Only the differences between two time stamps is used.
        - Timestamp must be always provided. Not providing the timestamp impairs
          the rolling shutter correction and gyroscope stabilisation will not work.
    */
    UINT64 timeStamp;

    /** The time in microseconds that corresponds to readout of lines contained in the
        image contained in DVS_CFG::originalSize.
        - A value of 0 means that the readout time is not available. In this case the
          rolling shutter correction will assume that readout time equals to the time
          between frames (frames timestamps must be provided).
    */
    INT32 readout;

    /** Frame exposure time in microseconds.
    */
    UINT32 exposureTime;

} DVS_FRAME_INFO;

/*------------------------------------------------------------------------------
 *  DVS_CAMERA_MODEL
 *----------------------------------------------------------------------------*/
/**
  Contains information about the camera projection.
*******************************************************************************/
typedef struct
{
     /** The size, in bytes, of this structure. Set to sizeof(DVS_CAMERA_MODEL)
        before calling any function that uses this structure.
    */
    UINT32 structSize;

    /** The X position on the original image of the optical center.
        - The original image resolution is given in DVS_CFG::originalSize.
        - In ideal situation the optical center is in the center of the image.
        - A value of 0 means that the optical center is not provided and the
          library will use by default the center of the original image.
    */
    float opticalCenterX;

    /** The Y position on the original image of the optical center.
        See opticalCenterX for more details.
    */
    float opticalCenterY;

    /** The focal length in pixels of the original image that corresponds to
        DVS_CFG::originalSize resolution.
        - A value of 0 means that the focal length is not provided.
        - Focal length must be provided in case in case if gyro is used.
          See @ref DVS_FLAG_USE_GYRO.
        - Normally focalLengthX == focalLengthY.
    */
    float focalLengthX;

    /** See focalLengthX for more details.
    */
    float focalLengthY;

    /** Length of the forward projection LUT.
        If set to 0 the pinhole camera model is used. 
    */
    UINT32 forwardProjLength;

    /** Lookup table describing camera's forward projection characteristics.
    */
    float *forwardProjLUT;  

    /** Length of the backward projection LUT.
        If set to 0 the pinhole camera model is used.
    */
    UINT32 backwardProjLength;

    /** Lookup table describing camera's backward projection characteristics.
    */
    float *backwardProjLUT;  


}DVS_CAMERA_MODEL;

/*------------------------------------------------------------------------------
 *  DVS_IMU_SAMPLE
 *----------------------------------------------------------------------------*/
/**
  Represents a generic 3 axis data sampled from the Inertial Measurement Unit
  (IMU) sensor.

  - The sample values must be in the following units:
    - Gyroscope: rad/sec
    - Accelerometer: G (g-force)
    - Magnetometer: uT (micro tesla)
  - Bias removal must be done outside the library.
  - Positive sign means counterclockwise rotation (standard for most gyroscopes). 
  - Axes orientation in respect to the image plane: X - short image edge 
    pointing upwards, Y - long edge, Z - perpendicular to the image surface
  - The sampling frequency must be at least 100 Hz.
  - See [Acquiring IMU samples](@ref dvsAPI_IMU).
  .
  @image html Gyro_orientation.png
*******************************************************************************/
typedef struct
{
    float x; /**< X-axis value */
    float y; /**< Y-axis value */
    float z; /**< Z-axis value */
} DVS_IMU_SAMPLE;

/*------------------------------------------------------------------------------
 *  DVS_FLAGS
 *----------------------------------------------------------------------------*/
/**
  Used in DVS_CFG::flags
@ingroup dvsAPI_Filter
**************************************************************************/
enum DVS_FLAGS
{
    /** Indicates that the application will supply gyro samples.
      See [Choosing Input Data](@ref dvsAPI_InputData).
    */
    DVS_FLAG_USE_GYRO         = (1 << 0),

    /** Indicates that the application will supply accelerometer samples.
      See [Choosing Input Data](@ref dvsAPI_InputData).
    */
    DVS_FLAG_USE_ACCELERATION = (1 << 1),

    /** Indicates that the application will supply magnetometer samples.
      See [Choosing Input Data](@ref dvsAPI_InputData).
    */
    DVS_FLAG_USE_MAGNET       = (1 << 2),

    /** Indicates that the application will supply motion vectors.
      See [Choosing Input Data](@ref dvsAPI_InputData).
    */
    DVS_FLAG_USE_MOTION       = (1 << 3),

    /** Indicates that the application will supply input image.
      See [Choosing Input Data](@ref dvsAPI_InputData).
    */
    DVS_FLAG_USE_IMAGE        = (1 << 4),

    /** Indicates [low latency mode](@ref dvsAPI_LowLatency).
    */
    DVS_FLAG_LOW_LATENCY_MODE = (1 << 5),
    
    /** Indicates that the correction grid will be in forward mapping (geometry mapping for GPU).
        By default grid is generated for backward mapping (SGDE or texture mapping on GPU).
    */
    DVS_FLAG_FORWARD_GRID = (1 << 6),

    /** This flag enables the horizon locking feature.
        If enabled, the horizon line will be kept in the middle of the screen as long as sufficient image data is available.
    */
    DVS_FLAG_HORIZON_LOCK = (1 << 7),

    /** Disables the limits applied to stabilization.
    */
    DVS_FLAG_DISABLE_LIMIT = (1 << 8),

    /** If enabled, the library will not re-center the frame over time.
    */
    DVS_FLAG_FREEZE_IMAGE = (1 << 9),
};

/*------------------------------------------------------------------------------
 *  DVS_VECTOR
 *----------------------------------------------------------------------------*/
/**
  Contains information about an image motion vector. This vector is compatible 
  with the vector forma treturned by the HRE library.
**************************************************************************/
typedef struct
{
    /// Contains horizontal shift in pixels. Contains 23.8 fixed point value.
    /// Positive value indicates right shift.
    INT32  dx;
    /// Contains horizontal quality factor. Positive value indicates remaining difference.
    /// Negative value indicates an error.
    INT32  qx;
    /// Contains vertical shift in pixels. Contains 23.8 fixed point value.
    /// Positive value indicates down shift.
    INT32  dy;
    /// Contains vertical quality factor. Positive value indicates remaining difference.
    /// Negative value indicates an error.
    INT32  qy;
} DVS_VECTOR;


/*------------------------------------------------------------------------------
 *  DVS_NODE
 *----------------------------------------------------------------------------*/
/**
  A single node of the correction grid that can be used to stablize the image 
  and remove the high frequency rolling shutter distortions.
*******************************************************************************/
typedef struct
{
    float x; /**< Horizontal position in input image in pixels. */
    float y; /**< Vertical position in input image in pixels. */
} DVS_NODE;

/*------------------------------------------------------------------------------
 *  DVS_CFG
 *----------------------------------------------------------------------------*/
/**
  Contains configuration information.
*******************************************************************************/
typedef struct
{
    /** The size, in bytes, of this structure. Set to sizeof(DVS_CFG)
        before calling any function that uses this structure.
    */
    UINT32 structSize;

    /** Bitmask containing configuration flags as specified in ::DVS_FLAGS.
    */
    UINT32 flags;

    /** The size in pixels of the original video frames before the
        stabilization and correction.
        - See [Configuring Image Size](@ref dvsAPI_ImageSize)
    */
    GT_SIZE originalSize;

    /** The rectangle in pixels representing the part of the input images that
        are used for motion vector computation.
        - Used only if @ref DVS_FLAG_USE_MOTION or @ref DVS_FLAG_USE_IMAGE are
          specified
        - Size must be greater or equal to the workingSize returned by
          DVS_LIB::GetWorkingSize().
        - The rectangle should be entirely inside of the image boundaries.
        - See [Configuring Image Size](@ref dvsAPI_ImageSize)
    */
    GT_RECT hreInputCrop;

    /** The size in pixels of the down-sampled input images used for motion
        vector computation.
        - Used only if @ref DVS_FLAG_USE_MOTION or @ref DVS_FLAG_USE_IMAGE are
          specified
        - May be obtained through DVS_LIB::GetWorkingSize() with the size of
          the input crop as input parameter.
        - See [Configuring Image Size](@ref dvsAPI_ImageSize)
    */
    GT_SIZE hreWorkingSize;

    /** The size in pixels representing the part of the original image that will 
        be visible to the user after stabilisation. Must be smaller than the 
        rectangle defined in stabLimits. The difference in size between those 
        two rectangles determine the maximum displacement that can be corrected.
        - See [Configuring Image Size](@ref dvsAPI_ImageSize)
    */
    GT_SIZE outputSize;

    /** The rectangle in pixels representing boundaries within which the outputSize
      is moved in order to provide stabilisation effect. Typically those boundaries 
      corellate with the boundaries of the original image.
      - See [Configuring Image Size](@ref dvsAPI_ImageSize)
    */
    GT_RECT stabLimits;

    /** The size in nodes of the correction grid.        
    */
    GT_SIZE gridSize;

    /** The width in pixels between nodes in the correction grid.
    */
    float cellWidth;

    /** The height in pixels between nodes in the correction grid.
    */
    float cellHeight;

    /** Position of the first node of the correction grid.
        This node can be located outside of the input image.
    */
    float gridLeft;
    float gridTop;

    /** Pointer to the DVS_CAMERA_MODEL structure containing information about 
        the system camera projection model, including distortions.
    */
    DVS_CAMERA_MODEL cameraModel;
    
    /** Determines the stabilisation strength. Strength 0 means no
        stabilisation and strength 100 means full stabilisation (image will not
        move until stabilisation margin is reached).
        The effect of this parameter varies with frame rate.
		strngth - initial strength
		minstrngth - minimum adaptation strength
		maxstrngth - maximum adaptation strength
    */
	float strength;
	float minstrength;
	float maxstrength;

    /**
        IMU samples clock jitter in microseconds. If samples are acquired at strictly fixed intervals 
        set imuClockJitter = 0.
        Otherwise, set it to the maximum deviation in microseconds from the intervals 
        specified in gyroInterval, accelInterval and magnetInterval.
    */
    UINT32 imuClockJitter;

    /**
    Maximum interval between frame samples in microseconds.
    */
    UINT32 frameInterval;

    /**
    Intervals between gyro samples in microseconds.
    */
    UINT32 gyroInterval;

    /**
        Intervals between accelerometer samples in microseconds.
    */
    UINT32 accelInterval;

    /**
    Intervals between magnetometer samples in microseconds.
    */
    UINT32 magnetInterval;

} DVS_CFG;

/*------------------------------------------------------------------------------
 *  DVS_FILTER
 *----------------------------------------------------------------------------*/
/**
  Represents the filter interface.
  To retrieve this structure, call DVS_LIB::CreateFilter().
@ingroup dvsAPI_Filter
*******************************************************************************/
typedef struct DVS_FILTER
{
    /** Pointer to the function table. */
    const struct DVS_FILTER_VFT* vft;
} DVS_FILTER;

/*------------------------------------------------------------------------------
 *  DVS_FILTER_VFT
 *----------------------------------------------------------------------------*/
/**
  Contains pointers to DVS_FILTER functions.
@ingroup dvsAPI_Filter
*******************************************************************************/
struct DVS_FILTER_VFT
{
    /* Destroy()
    */
    /**
      Destroys the filter object.
    @param [in] filter
      Pointer to the filter object to be destroyed.
    */
    void (GTCALL* Destroy)(
        IN DVS_FILTER* filter);

    /* AddIMUSample()
    */
    /**
      Adds Inertial Measurement Unit (IMU) sample.
    @param [in] filter
      Pointer to the filter object.
    @param [in] timeStamp
      Time when sample was aquired in microseconds
    @param [in] exposureTime
      Current image sensor exposure time (integration time)
    @param [in] gyroSample
      The pointer to the vector containing gyroscope rates in rad/s. Can be NULL if not used.
    @param [in] accelSample
      The pointer to the linear acceleration vector in G. Can be NULL if not used.
    @param [in] magnSample
      The pointer to the magnetic field vector in uT. Can be NULL if not used.
    @returns
      - GTS_OK - success
      - otherwise - error
    @remarks
      - See [Acquiring IMU samples](@ref dvsAPI_IMU).
      .
      @image html Gyro_orientation.png
    */
    GTSTATUS (GTCALL* AddIMUSample)(
        IN DVS_FILTER* filter,
        IN UINT64 timeStamp,
        IN UINT32 exposureTime,
        IN const DVS_IMU_SAMPLE* gyroSample,
        IN const DVS_IMU_SAMPLE* accelSample,
        IN const DVS_IMU_SAMPLE* magnSample);

    /* StartFrameProcessing()
    */
    /**
      Indicates that a new video frame image is about to be processed.
    @param [in] filter
      Pointer to the filter object.
    @param [in] image
      (Optional) Input video frame image.
      - The size of the image must be equal to DVS_CFG::inputSize.
      - See [supported color formats](@ref dvsAPI_SupportedFmts).
      - Can be NULL - see [Choosing Input Data](@ref dvsAPI_InputData).
    @param[in] motion
      (Optional) Pointer to the caller allocated array of DVS_VECTOR structures which
      will contain motion vectors returned by the HRE module.
      - The length of this array must be 256 vectors.
      - Can be NULL - see [Choosing Input Data](@ref dvsAPI_InputData).
    @param [in] info
      Additional information about the video frame.
    @returns
      - GTS_OK - success
      - otherwise - error
    */
    GTSTATUS (GTCALL* StartFrameProcessing)(
        IN DVS_FILTER* filter,
        IN const GT_IMAGE* image OPTIONAL,
        IN const DVS_VECTOR* motion OPTIONAL,
        IN const DVS_FRAME_INFO* info);

    /* ProcessFrame()
    */
    /**
      Processes a video frame and calculates the correction.
    @param [in] filter
      Pointer to the filter object.
    @returns
      - GTS_OK - the entire frame was processed.
      - GTS_DVS_IN_PROGRESS - the frame was partially processed. This function
        must be called again after more IMU samples are added. This return code is
        used in [low latency mode](@ref dvsAPI_LowLatency).
      - otherwise - error.
    */
    GTSTATUS (GTCALL* ProcessFrame)(
        IN DVS_FILTER* filter);

    /* Reset()
    */
    /**
      Resets internal state to initial values. This is an alternative to
      destroying and re-creation of the filter object.
    @param [in] filter
      Pointer to the filter object.
    */
    void (GTCALL* Reset)(
        IN DVS_FILTER* filter);

    /* GetCorrectionMatrix()
    */
    /**
      Retrieves transformation matrix that should be applied to the current
      unstabilized video frame in order to stabilize the video.
    @param [in] filter
      Pointer to the filter object.
    @param [out] matrix
      The transformation matrix. See [image transformation](@ref dvsAPI_Matrix).
    @returns
      - GTS_OK - success
      - otherwise - error
    */
    GTSTATUS (GTCALL* GetCorrectionMatrix)(
        IN DVS_FILTER* filter,
        OUT float matrix[3][3]);

    /* GetDisplacementMatrix()
    */
    /**
      Retrieves transformation matrix that describes how current unstabilized
      frame has been displaced comparing to the previous unstabilized frame.
    @param [in] filter
      Pointer to the filter object.
    @param [out] matrix
      The transformation matrix. See [image transformation](@ref dvsAPI_Matrix).
    @returns
      - GTS_OK - success
      - otherwise - error
    @remarks
      - This is different than the matrix returned by GetCorrectionMatrix()
        and is not meant to stabilize the image. It describes the motion
        that occurred between current and previous unstabilized frames.
    */
    GTSTATUS (GTCALL* GetDisplacementMatrix)(
        IN DVS_FILTER* filter,
        OUT float matrix[3][3]);

    /* GetCorrectionGrid()
    */
    /**
      Retrives the correction grid that can be used to stablize the image 
      and remove the high frequency rolling shutter distortions. 
    @param [in] filter
      Pointer to the filter object.
    @param [in] lineBegin
      The begining grid line number in range from 0 to lineEnd-1.
    @param [in] lineEnd
      The ending line number in range from lineBegin+1 to DVS_CFG::gridSize.height.
    @param [out] grid
      Pointer to the application allocated array of DVS_NODE elements.
      The array must contain number of lines equal to DVS_CFG::gridSize.height.
      Each line must contain number of elements equal to DVS_CFG::gridSize.width.
    @returns
      - GTS_OK - success.
      - GTS_DVS_GRID_NOT_READY - the requested portion of the grid is not ready.
      - otherwise - error.
    */
    GTSTATUS (GTCALL* GetCorrectionGrid)(
        IN DVS_FILTER* filter,
        IN UINT32 lineBegin,
        IN UINT32 lineEnd,
        OUT DVS_NODE* grid);

    /* GetImageLines()
    */
    /**
      Returns the number of image lines that currently can be corrected.
    @param [in] filter
      Pointer to the filter object.
    @returns
      The number of image lines that currently can be corrected.
      - See also [Low Latency Mode](@ref dvsAPI_LowLatency).
    */
    UINT32 (GTCALL* GetImageLines)(
        IN DVS_FILTER* filter);

    /* GetGridLines()
    */
    /**
      Returns the number of grid lines that are available for the correction.
    @param [in] filter
      Pointer to the filter object.
    @returns
      The number of grid lines that are available for the correction.
      - The grid can be obtained by calling GetCorrectionGrid().
      - See also [Low Latency Mode](@ref dvsAPI_LowLatency).
    */
    UINT32 (GTCALL* GetGridLines)(
        IN DVS_FILTER* filter);

    /* GetDisplacementOffset()
    */
    /**
      Returns the (x,y) offset between current unstablized frame and
      previous unstabilized frame.
    @param [in] filter
      Pointer to the filter object.
    @param [out] dx
      X offset in pixels.
    @param [out] dy
      Y offset in pixels.
    @returns
      - GTS_OK - success
      - otherwise - error
    */
    GTSTATUS (GTCALL* GetDisplacementOffset)(
        IN DVS_FILTER* filter,
        OUT float* dx,
        OUT float* dy);

    /* GetStabilizedOffset()
    */
    /**
      Returns the (x,y) offset between current stabilized frame and
      previous stabilized frame.
    @param [in] filter
      Pointer to the filter object.
    @param [out] dx
      X offset in pixels.
    @param [out] dy
      Y offset in pixels.
    @returns
      - GTS_OK - success
      - otherwise - error
    */
    GTSTATUS (GTCALL* GetStabilizedOffset)(
        IN DVS_FILTER* filter,
        OUT float* dx,
        OUT float* dy);

   
    /* GetStabilizedPoint()
    */
    /**
      Calculates the coordinates of the input point after stabilization
    @param [in] filter
      Pointer to the filter object.
    @param [in] inPoint
      Pointer to the input point.
    @param [out] outPoint
      Pointer to the output point
    @returns
      - GTS_OK - success
      - otherwise - error
    */
    GTSTATUS(GTCALL* GetStabilizedPoint)(
        IN  DVS_FILTER* filter,
        IN  GT_POINT *inPoint,
        OUT GT_POINT *outPoint);

    /* SetFlag()
    */
    /**
      Sets a specified configuration flag on or off
    @param [in] filter
      Pointer to the filter object.
    @param [in] inFlag
      Flag to be changed. Allowed flags are:
        - DVS_FLAG_HORIZON_LOCK
        - DVS_FLAG_DISABLE_LIMIT
        - DVS_FLAG_FREEZE_IMAGE
    @param [in] inValue
      - 0 to disable the flag
      - 1 to enable the flag
    @returns
      - GTS_OK - success
      - otherwise - error
    */
    GTSTATUS(GTCALL* SetFlag)(
        IN  DVS_FILTER* filter,
        IN  UINT32 inFlag,
        IN  UINT32 inValue);

    /* SetStrength()
    */
    /**
      Sets the stabilisation strength.
    @param [in] filter
      Pointer to the filter object.
    @param [in] strength
      Strength value. See DVS_CFG::strength
    @returns
      - GTS_OK - success
      - otherwise - error
    */
    GTSTATUS(GTCALL *SetStrength)(
        IN DVS_FILTER *filter,
        IN float minstrength,
		IN float maxstrength);
};

/*------------------------------------------------------------------------------
 *  DVS_LIB_INFO
 *----------------------------------------------------------------------------*/
/**
  Contains information about the library version.
  See DVS_LIB::GetLibInfo().
*******************************************************************************/
typedef struct DVS_LIB_INFO
{
    /** The size, in bytes, of this structure. 
    */
    UINT32 structSize;

    /** The library version numbers, first element is the major number.
    */
    UINT32 libVer[5];

    /** @ref dvsAPI_Ver "API version" number.
    */
    UINT32 apiVer;

    /** Pointer to the string specifying build date in form "Mmm dd yyyy hh:mm:ss".
        Same format as in __DATE__ and __TIME__ macros.
    */
    const char* buildDate;

    /** Pointer to the optional string specifying the release label.
    */
    const char* label;

} DVS_LIB_INFO;

/*------------------------------------------------------------------------------
 *  DVS_LIB
 *----------------------------------------------------------------------------*/
/**
  Contains pointers to functions which represent the library interface.
  To retrieve this structure, call DvsLibGet().
@ingroup dvsAPI_Lib
*******************************************************************************/
typedef struct DVS_LIB
{
    /* GetLibInfo()
    */
    /** Returns information about library version.
    @return
       A pointer to DVS_LIB_INFO containing version information.
    */
    const DVS_LIB_INFO* (GTCALL* GetLibInfo)(void);

    /* GetRequiredHeapSize()
    */
    /** Returns the required size of the heap needed by the library to
      function properly.
    @param [in] cfg
      Pointer to the configuration structure used by CreateFilter().
    @return
      The required size (in bytes) for CreateHeap(),
      or 0 if the given configuration is invalid.
    */
    UINT32 (GTCALL* GetRequiredHeapSize)(
        IN const DVS_CFG* cfg);

    /* CreateHeap()
    */
    /** Creates a heap object from the input buffer.
    @param [in] lib
       A pointer to the library object.
    @param [in] buffer
       Pointer to a buffer that shall be used as a heap.
    @param [in] size
       Size of the buffer, in bytes.
    @param [out] heap
       A pointer to a caller supplied variable. On success it will contain the 
       pointer to the newly created heap object.
    @return
       - GTS_OK - success.
       - GTS_NOT_SUPPORTED - this functionality is not included in this
         library version.
       - Otherwise - other error.
    @remarks
       This function is intended for platforms without own heap management.
       It uses provided buffer to create a heap that can be supplied to 
       DVS_LIB::CreateFilter(). After the heap is not needed, call
       GT_HEAP_VFT::Destroy().
    @warning
       The heap created by this function is not thread safe.
       If multiple detector objects are run in parallel, then each
       must be supplied with separate heaps created by this function.
       See also [thread safety](@ref dvsAPI_Threading).
    */
    GTSTATUS (GTCALL* CreateHeap)(
        IN void* buffer,
        IN UINT32 size,
        OUT GT_HEAP** heap);

    /* GetWorkingSize()
    */
    /**
      Retrieves the image resolution that will be used internally by the algorithm.
    @param [in] originalSize
      Same as in DVS_CFG::originalSize.
    @param [out] workingSize
      The size of the internal working image in pixels.
    @returns
      - GTS_OK - success
      - otherwise - error
    @remarks
      DVS_FILTER_VFT::StartFrameProcessing() will first downscale the input image to the
      working resolution, and then will process the image. If the application
      provides an input image that is already downscaled to the working
      resolution, then the library will not spend time for image scalling.
    */
    GTSTATUS (GTCALL* GetWorkingSize)(
        IN const GT_SIZE* originalSize,
        OUT GT_SIZE* workingSize);

    /* CreateFilter()
    */
    /** Creates the filter object.
    @param [in] heap
       Address of an optional heap object that shall be used for memory 
       management.
       There are three options of providing the heap:
       - NULL value: use the default heap.
         If heap parameter is NULL then:
         - If the library supports a default heap for given platform,
           it will be used instead.
         - Otherwise GTS_NO_SYSTEM_RESOURCES error is returned.
       - Call DVS_LIB::CreateHeap() to create a heap from a buffer.
       - Implement a custom GT_HEAP. See @ref gtAPI_Heap for an example.
       .
    @param [in] cfg
       Pointer to the configuration structure.
    @param [out] filter
       A pointer to a caller supplied variable. On success it will contain the 
       pointer to the newly created filter object.
    @return
       - GTS_OK - success.
       - Otherwise - error.
    @remarks
      - When the filter object is no longer needed, call DVS_DETECTOR_VFT::Destroy().
      - If the application provided a non NULL heap, then it is responsible for
        destroying the heap after destroying the filter.
      - The application can create multiple filter objects and run them
        in parallel to process different video streams.
        See also [thread safety](@ref dvsAPI_Threading).
    */
    GTSTATUS (GTCALL* CreateFilter)(
        IN GT_HEAP* heap OPTIONAL,
        IN const DVS_CFG* cfg,
        OUT DVS_FILTER** filter);
    
} DVS_LIB;

/*------------------------------------------------------------------------------
 *  DvsLibGet()
 *----------------------------------------------------------------------------*/
/**
  Library entry point function.
@return
  The pointer to Dvs_LIB structure representing the library interface.
@ingroup dvsAPI_Lib
*******************************************************************************/
GTAPI DVS_LIB* GTCALL DvsLibGet(void);

#ifdef __cplusplus
}
#endif

#endif /* _LIBDEVIS_H_ */
