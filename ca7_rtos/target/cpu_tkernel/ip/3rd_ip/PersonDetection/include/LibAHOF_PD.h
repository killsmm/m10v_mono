/*--------------------------------------------------------------------------------------------------

  LibAHOF_PD - Advanced Hardware-Based Person Detector API

  Copyright (c) FotoNation.
  All rights reserved. Confidential.

--------------------------------------------------------------------------------------------------*/
#ifndef __LIBAHOF_PERSON_DETECTOR_H__
#define __LIBAHOF_PERSON_DETECTOR_H__

#include "AHOFDetector.h"

#ifdef  __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////
/**
@weakgroup ahof_pdAPI_Ver

  API Version

  The API version is a 32-bit value where the higher 16 bits represent the
  major version and the lower 16 bits represent the minor version.
  When the minor is increased the API remains backward compatible. When
  the major is increased the API may not be backward compatible.

  To retrieve the API version, call AHOF_LIB_VFT::GetLibInfo() and read
  the AHOF_LIB_INFO::apiVer field. It is different from the library version
  (AHOF_LIB_INFO::libVer). Library version refers to the implementation,
  however API version refers only to the API definition.

  The following is the list of API versions and the corresponding changes:

@par 0x00010001
  - First LibAHOF_PD API.

**************************************************************************/

///////////////////////////////////////////////////////////////////////////
// AHOF Detector Interface
/**
@weakgroup ahof_pdAPI_Det
  The AHOF detector object exposes the main functionality needed in
  order to generate and post-process HW requests.
  HW requests consists of HW configuration, input image data and HW
  results from TME-R module.
  The AHOF detector is represented by AHOF_DETECTOR structure that
  provides pointers to the detector functions which implements the
  following functionality:
  - Generate HW requests that will be sent to the HW to process.
  - Post-process the HW request (process the HW results)
@sa AHOF_PERSON_DETECTOR
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
// AHOF_PERSON_FLAGS
/**
  Contains the configuration flags used when the person detector is created.
@ingroup ahof_pdAPI_Det
**************************************************************************/
enum AHOF_PERSON_FLAGS
{
    /// If set, the library performs detections on tracking, treating
    /// images as part of a time sequence (video frame).
    /// @hideinitializer
    AHOF_PERSON_FLAGS_TRACKING_MODE             = (1 << 0)

};

///////////////////////////////////////////////////////////////////////////
// AHOF_PERSON_DETECTOR_CFG
/**
  Contains the configuration used to create the person detector.
@ingroup ahof_pdAPI_Det
**************************************************************************/
typedef struct AHOF_PERSON_DETECTOR_CFG
{
    /** The size, in bytes, of this structure. Set to <i>sizeof(AHOF_PERSON_DETECTOR_CFG)</i>
      before calling any function that uses this structure.
    */
    UINT32      structSize;

    /** @ref AHOF_PERSON_FLAGS "Configuration flags." All other bits that
      are not specified here must be zero.
    */
    UINT32      flags;

    /** Specifies the minimum person size in pixels the library should
      try to detect.

      The allowed range for this parameter is <i>[64, min(image.width, image.height)]</i>.
      If zero is specified (default) then it will try to find persons down until 64 pixels.

      Increasing this value will result in better performance.
    */
    UINT32      minPersonSize;

    /** Specifies the maximum person size in pixels the library should
      try to detect.

      The allowed range for this parameter is <i>[max(64, minPersonSize), min(image.width, image.height)]</i>.
      If zero is specified (default) then no upper limit will be used.
    */
    UINT32      maxPersonSize;

    /** Specifies the maximum number of persons to be detected and tracked.
      After this number is reached the algorithm stops searching for additional
      persons until the count of tracked persons decreases under this value.
      If zero is used (default) then there is no limitation for the number of persons.

      ! Not yet implemented !
    */
    UINT32      maxPersonCount;

    /** Sets a trade-off between detection rate and false positives.
      It is a value between -10 ... 10:
        - boostDRvsFP = 0 - recommended default trade-off.
        - boostDRvsFP > 0 - higher detection rate and higher false positives.
        - boostDRvsFP < 0 - lower detection rate and lower false positives.

      By changing this value, the execution time is affected to a smaller extent.
    */
    INT32       boostDRvsFP;

    /** Sets a trade-off between detection rate and execution speed.
      It is a value between -10 ... 10:
        - boostDRvsSPEED = 0 - recommended default trade-off.
        - boostDRvsSPEED > 0 - higher detection rate and slower execution (up to several times).
        - boostDRvsSPEED < 0 - lower detection rate and faster execution.

      By changing this value, the false positives are affected to a smaller extent.

      ! Not yet implemented !
    */
    INT32       boostDRvsSPEED;

    /** Optional buffer containing externally given detection templates
      Set to NULL if not used.
    */
    void*       detectorsBuffer;

    /** Size of the optional buffer
      Set to 0 if not used.
    */
    UINT32      detectorsBufferSize;

    /** Sets the maximum number of persons that can be tracked on a frame
      using a more advanced and resource demanding tracking algorithm.
      If this value is zero (default), then the advanced tracking
      algorithm will not be used at all.
      @note
        This value is used only when tracking mode is used (@sa AHOF_PERSON_FLAGS).
    */
    UINT32      maxPersonCountForTracking;

} AHOF_PERSON_DETECTOR_CFG;

///////////////////////////////////////////////////////////////////////////
// AHOF_DETECTION_TYPES
/**
  Contains the types of detections.
@ingroup ahof_pdAPI_Types
**************************************************************************/
enum AHOF_DETECTION_TYPES
{
    //Full-Body detection
    AHOF_DETECTION_TYPE_FULL_BODY         =   0,
    //Upper-Body detection
    AHOF_DETECTION_TYPE_UPPER_BODY

};

///////////////////////////////////////////////////////////////////////////
// AHOF_PERSON_DETECTOR
/**
  Contains information about a detected/tracked person.
@ingroup ahof_pdAPI_Det
**************************************************************************/
typedef struct AHOF_PERSON
{
    /** The size, in bytes, of this structure. Set to <i>sizeof(AHOF_PERSON)</i>
      before calling any function that uses this structure.
    */
    UINT32      structSize;

    /** The center of the detection.
      This is specified in fixed point, with 8 bits of precision.
    */
    GT_POINT    center;

    /** The size of the detection, in pixels.
      This is specified in fixed point, with 8 bits of precision.
    */
    GT_SIZE     size;

    /** The clockwise roll angle of the person, in degrees.
      This is specified in fixed point, with 8 bits of precision.
    */
    INT32       rollAngle;

    /** The yaw angle of the person, in degrees.
      This is specified in fixed point, with 8 bits of precision.
    */
    INT32       yawAngle;

    /** The pitch angle of the person, in degrees.
      This is specified in fixed point, with 8 bits of precision.
    */
    INT32       pitchAngle;

    /** @ref AHOF_DETECTION_TYPES "Type of detection."
    */
    UINT32       type;

    /** The tracking ID of the person.
      This number is unique for each tracked person.
    */
    UINT32      trackId;

    /** The confidence that the detected person is correct.
      This value can range between [0, 256], where a value
      of 256 represent the maximum confidence that the
      person was correctly detected.
    */
    INT32      confidence;
} AHOF_PERSON;

///////////////////////////////////////////////////////////////////////////
// AHOF_PERSON_DETECTOR
/**
  Represents the interface to the person detector that uses AHOF.
  This interface can be cast to AHOF_DETECTOR if needed.
@ingroup ahof_pdAPI_Det
**************************************************************************/
typedef struct AHOF_PERSON_DETECTOR
{
    /** Pointer to the function table. */
    const struct AHOF_PERSON_DETECTOR_VFT* vft;
}AHOF_PERSON_DETECTOR;

///////////////////////////////////////////////////////////////////////////
// AHOF_PERSON_DETECTOR_VFT
/**
  Contains pointers to AHOF_PERSON_DETECTOR functions.
@ingroup ahof_pdAPI_Det
**************************************************************************/
struct AHOF_PERSON_DETECTOR_VFT
{
    // Destroy()
    //
    /**
      Destroys the detector object.
    @param [in] det
      Pointer to the object to be destroyed.
    */
    void (GTCALL* Destroy)(
        IN AHOF_PERSON_DETECTOR*            det
        );

    // SetManagerCtrl()
    //
    /**
      Sets the interface to the AHOF manager Control, that can be
      used by the detector to make HW requests.
    @param [in] det
      Pointer to the detector object.
    @param [in] ahofCtrl
      Pointer to the AHOF manager Control interface.
      Note that if this is NULL, then no future calls
      should be made to the AHOF manager Control.
    */
    GTSTATUS (GTCALL* SetManagerCtrl)(
        IN AHOF_PERSON_DETECTOR*        det,
        IN AHOF_MANAGER_CTRL*           ahofCtrl
        );

    // Process()
    //
    /**
      Process the given image, in order to detect objects.
    @param [in] det
      Pointer to the detector object.
    @param [in] img
      Pointer to the image on which the detection will be performed.
    @param [in] imgMeta
      Pointer to a linked list of metadata associated with the image.
    @param [in] reserved
      Reserved for future use. Should be set to NULL.
    @return
      GTS_OK if everything was OK, else the associated error code.
    */
    GTSTATUS (GTCALL* Process)(
        IN AHOF_PERSON_DETECTOR*        det,
        IN const GT_IMAGE*              img,
        IN const AHOF_IMAGE_METADATA*   imgMeta OPTIONAL
        );

    // GetResults()
    //
    /**
      Retrieves the results for the processed image.
    @param [in] det
      Pointer to the detector object.
    @param [out] numDetections
      Pointer to a user supplied variable, that on return will
      contain the number of detected objects on the current frame.
    @param [out] detArray
      Pointer to a user supplied variable, that on return will
      contain a pointer to the detections array.
    @return
      GTS_OK if everything was OK, else the associated error code.
    */
    GTSTATUS (GTCALL* GetResults)(
        IN AHOF_PERSON_DETECTOR*        det,
        OUT UINT32*                     numDetections,
        OUT AHOF_PERSON**               detArray
        );

    // AbortProcess()
    //
    /**
      Signals the Process function that it should abort as fast as possible.
      This call will not be made on the same thread on which Process executes!
    @param [in] det
      Pointer to the detector object.
    @return
      GTS_OK if everything was OK, else the associated error code.
    */
    GTSTATUS (GTCALL* AbortProcess)(
        IN AHOF_PERSON_DETECTOR*        det
        );
};

///////////////////////////////////////////////////////////////////////////
// Library Interface
/**
@weakgroup ahof_pdAPI_Lib
  The library object is the root object in the library API.
  It is represented by AHOF_PERSON_LIB structure that provides pointers to the
  library functions.
@sa AHOF_PERSON_LIB
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
// AHOF_PERSON_FEATURE_*
/** The list of flags used to specify the features that are available
  in the library. These flags are found in AHOF_PERSON_LIB_INFO::features.
@ingroup ahof_pdAPI_Lib
**************************************************************************/
enum AHOF_PERSON_FEATURES_FLAGS
{
    /// This flag is present when the library can create its own heap,
    /// so the user is not required to provide a custom heap, if he chooses so.
    /// @hideinitializer
    AHOF_PERSON_FEATURE_INTERNAL_HEAP           = (1 << 0),

    /// This flag is present when the library has support to
    /// detect full-body persons.
    /// @hideinitializer
    AHOF_PERSON_FEATURE_FULLBODY                = (1 << 1),

    /// This flag is present when the library has support to
    /// detect half-body persons.
    /// @hideinitializer
    AHOF_PERSON_FEATURE_HALFBODY                = (1 << 2)
};

///////////////////////////////////////////////////////////////////////////
// AHOF_PERSON_LIB_INFO
/**
  Contains information about the library version.
  See AHOF_PERSON_LIB_VFT::GetLibInfo().
@ingroup ahof_pdAPI_Lib
**************************************************************************/
typedef struct AHOF_PERSON_LIB_INFO
{
    /** The size, in bytes, of this structure.
    */
    UINT32 structSize;

    /** The library version numbers.
    */
    UINT32 libVer[5];

    /** @ref ahof_pdAPI_Ver "API version" number.
    */
    UINT32 apiVer;

    /** Pointer to the string specifying build date in form "Mmm dd yyyy hh:mm:ss".
        Same format as in __DATE__ and __TIME__ macros.
    */
    const char* buildDate;

    /** Pointer to the optional string specifying the release label.
    */
    const char* label;

    /** A bitmask specifying supported features. See ::AHOF_PERSON_FEATURES_FLAGS.
    */
    UINT32 features;

    /** An array of @ref ahof_pdAPI_SupportedFmts "supported color formats" with
        the last item set to zero.
    */
    const UINT32* supportedFmts;
} AHOF_PERSON_LIB_INFO;

///////////////////////////////////////////////////////////////////////////
// AHOF_PERSON_LIB
/**
  Represents the library interface.
  To retrieve this structure, call AhofPersonLibGet().
@ingroup ahof_pdAPI_Lib
**************************************************************************/
typedef struct AHOF_PERSON_LIB
{
    // GetLibInfo()
    //
    /** Returns information about library version.
    @return
       A pointer to AHOF_PERSON_LIB_INFO containing version information.
    */
    const AHOF_PERSON_LIB_INFO* (GTCALL* GetLibInfo)(void);

    // CreateDetector()
    //
    /** Creates the AHOF person detector object.
    @param [in] heap
       Address of an optional heap object that shall be used for memory
       management.
       There are three options of providing the heap:
       - NULL value, to use the @ref ::AHOF_PERSON_FEATURE_INTERNAL_HEAP "default internal heap":
         - If the library supports a default internal heap
           for given platform, that heap will be used instead.
         - Otherwise ::GTS_NO_SYSTEM_RESOURCES error is returned.
       - Call AHOF_LIB_VFT::CreateHeap() to create a heap from a buffer.
       - Implement a custom GT_HEAP. See @ref gtAPI_Heap for an example.
       .
    @param [in] cfg
       Pointer to the configuration structure.
    @param [out] det
       A pointer to a caller supplied variable. On success it will contain the
       pointer to the newly created person detector object.
    @return
       - ::GTS_OK - success.
       - Otherwise - error.
    @remarks
      - When the detector object is no longer needed, call AHOF_PERSON_DETECTOR_VFT::Destroy().
      - If the application provided a non NULL heap, then it is responsible for
        destroying the heap after destroying the detector.
      - Example:
        @code
          AHOF_PERSON_LIB*            lib = AhofPersonLibGet();
          AHOF_PERSON_DETECTOR*       det = NULL;
          GT_HEAP*                    heap= NULL;
          AHOF_PERSON_DETECTOR_CFG    cfg;

          memset(&cfg, 0, sizeof(AHOF_PERSON_DETECTOR_CFG));
          cfg.structSize = sizeof(AHOF_PERSON_DETECTOR_CFG);
          ...
          status = lib->vft->CreateDetector(heap, &cfg, &det);
          if (GTS_OK == status)
          {
              ...
              det->vft->Destroy(det);
          }
        @endcode
    */
    GTSTATUS (GTCALL* CreateDetector)(
        IN GT_HEAP*                             heap OPTIONAL,
        IN const AHOF_PERSON_DETECTOR_CFG*      cfg,
        OUT AHOF_PERSON_DETECTOR**              det);

    // GetRequiredHeapSize()
    //
    /** Returns the required size of the heap needed by AHOF_PERSON_DETECTOR to
      function properly.
    @param [in] cfg
       Pointer to the configuration structure used by CreateDetector.
    @return
       The required size (in bytes) of the heap for AHOF_PERSON_DETECTOR,
       or 0 if the given configuration is invalid.
    */
    UINT32 (GTCALL* GetRequiredHeapSize)(
        IN const AHOF_PERSON_DETECTOR_CFG*      cfg);
} AHOF_PERSON_LIB;

///////////////////////////////////////////////////////////////////////////
// AhofPersonLibGet()
/**
  Library entry point function.
@return
  The pointer to AHOF_PERSON_LIB structure representing the library interface.
@remarks
  This is the first function that needs to be called. The AHOF_PERSON_LIB
  structure gives access to other function pointers.
  @code
    AHOF_PERSON_LIB* lib = AhofPersonLibGet();
    const AHOF_PERSON_LIB_INFO* libInfo = lib->vft->GetLibInfo(lib);
  @endcode
@ingroup ahof_pdAPI_Lib
**************************************************************************/
GTAPI AHOF_PERSON_LIB* GTCALL AhofPersonLibGet(void);

#ifdef __cplusplus
}
#endif

#endif // __LIBAHOF_PERSON_DETECTOR_H__
