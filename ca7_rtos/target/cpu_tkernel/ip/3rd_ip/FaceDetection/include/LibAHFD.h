/*--------------------------------------------------------------------------------------------------

  LibAHFD - Advanced Hardware-Based Face Detection API

  Copyright (c) FotoNation.
  All rights reserved. Confidential.

--------------------------------------------------------------------------------------------------*/
#ifndef __LIBAHFD_H__
#define __LIBAHFD_H__

#include "gtypes.h"

#ifdef  __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////
/**
@weakgroup AHFDAPI_Ver

  API Version

  The API version is a 32-bit value where the higher 16 bits represent the
  major version and the lower 16 bits represent the minor version.
  When the minor is increased the API remains backward compatible. When
  the major is increased the API may not be backward compatible.

  To retrieve the API version, call AHFD_LIB_VFT::GetLibInfo() and read
  the AHFD_LIB_INFO::apiVer field. It is different from the library version
  (AHFD_LIB_INFO::libVer). Library version refers to the implementation,
  however API version refers only to the API definition.

  The following is the list of API versions and the corresponding changes:

@par 0x00010006
  - Added ::AHFD_FEATURE_PITCH_ANGLES and ::AHFD_DISABLE_PITCH_ANGLES
  - Added AHFD_FACE::pitchAngle, to report the detection pitch angle

@par 0x00010005
  - Added AHFD_RAW_FACE structure and AHFD_DETECTOR_VFT::GetRawFaceData function.

@par 0x00010004
  - Added AHFD_DETECTOR_CFG::boostFDvsFP configuration parameter.

@par 0x00010003
  - Added ::AHFD_AUTO_ORIENTATION and ::AHFD_BETTER_PRECISION_HW_MAP configuration flags.
  - Added ::AHFD_FEATURE_BETTER_PRECISION_HW_MAP to report if the HW supports this feature.
  - Added ::AHFD_FEATURE_INTERNAL_HEAP to report if default internal heap is supported.

@par 0x00010002
  - Added AHFD_DETECTOR_VFT::GetMainFace() function.

@par 0x00010001
  - First LibAHFD API.

**************************************************************************/

///////////////////////////////////////////////////////////////////////////
// AHFDS_*
/**
  Status Codes
@addtogroup gtAPI_Status
@{
**************************************************************************/

/// @name AHFD specific error codes
/// @{
#define AHFDS_INVALID_ENDIANNESS       (-100) ///< The platform endianness is different from the one used at compile time.
#define AHFDS_HW_ERROR                 (-101) ///< Unspecified hardware error.
/// @}

/// @name AHFD specific status (non error) codes
/// @{
#define AHFDS_NOT_FOUND                (100) ///< No face has been found
#define AHFDS_CALL_AGAIN               (101) ///< Need to call hardware and software again for detections in static mode.
/// @}

/** @} end of gtAPI_Status */

///////////////////////////////////////////////////////////////////////////
/// Locking Angles Array for AHFD_DETECTOR_CFG::lockAngles parameter.
/// For details regarding rollAngle and yawAngle, please see their definition
/// in AHFD_FACE structure
enum AHFD_LOCK
{
    AHFD_LOCK_END,          ///< Note: The angle array should be finished with AHFD_LOCK_END.
    AHFD_LOCK_0,            ///< rollAngle = [- 45;  45],                   yawAngle = 0             pitchAngle =  0
    AHFD_LOCK_90,           ///< rollAngle = [  45; 135],                   yawAngle = 0             pitchAngle =  0
    AHFD_LOCK_180,          ///< rollAngle = [ 135; 180] and [ -135; -179], yawAngle = 0             pitchAngle =  0
    AHFD_LOCK_270,          ///< rollAngle = [- 45;-135],                   yawAngle = 0             pitchAngle =  0
    AHFD_LOCK_SP_0,         ///< rollAngle =   0,                           yawAngle = 45 and -45    pitchAngle =  0
    AHFD_LOCK_P_0,          ///< rollAngle =   0,                           yawAngle = 90 and -90    pitchAngle =  0
    AHFD_LOCK_SP_90,        ///< rollAngle =  90,                           yawAngle = 45 and -45    pitchAngle =  0
    AHFD_LOCK_P_90,         ///< rollAngle =  90,                           yawAngle = 90 and -90    pitchAngle =  0
    AHFD_LOCK_SP_180,       ///< rollAngle = 180,                           yawAngle = 45 and -45    pitchAngle =  0
    AHFD_LOCK_P_180,        ///< rollAngle = 180,                           yawAngle = 90 and -90    pitchAngle =  0
    AHFD_LOCK_SP_270,       ///< rollAngle = -90,                           yawAngle = 45 and -45    pitchAngle =  0
    AHFD_LOCK_P_270,        ///< rollAngle = -90,                           yawAngle = 90 and -90    pitchAngle =  0
    AHFD_LOCK_SP_UP_0,      ///< rollAngle =   0,                           yawAngle = 45 and -45    pitchAngle =  30
    AHFD_LOCK_SP_DOWN_0,    ///< rollAngle =   0,                           yawAngle = 45 and -45    pitchAngle = -30
    AHFD_LOCK_P_UP_0,       ///< rollAngle =   0,                           yawAngle = 45 and -45    pitchAngle =  30
    AHFD_LOCK_P_DOWN_0,     ///< rollAngle =   0,                           yawAngle = 45 and -45    pitchAngle = -30
    AHFD_LOCK_SP_UP_90,     ///< rollAngle =  90,                           yawAngle = 45 and -45    pitchAngle =  30
    AHFD_LOCK_SP_DOWN_90,   ///< rollAngle =  90,                           yawAngle = 45 and -45    pitchAngle = -30
    AHFD_LOCK_P_UP_90,      ///< rollAngle =  90,                           yawAngle = 45 and -45    pitchAngle =  30
    AHFD_LOCK_P_DOWN_90,    ///< rollAngle =  90,                           yawAngle = 45 and -45    pitchAngle = -30
    AHFD_LOCK_SP_UP_180,    ///< rollAngle = 180,                           yawAngle = 45 and -45    pitchAngle =  30
    AHFD_LOCK_SP_DOWN_180,  ///< rollAngle = 180,                           yawAngle = 45 and -45    pitchAngle = -30
    AHFD_LOCK_P_UP_180,     ///< rollAngle = 180,                           yawAngle = 45 and -45    pitchAngle =  30
    AHFD_LOCK_P_DOWN_180,   ///< rollAngle = 180,                           yawAngle = 45 and -45    pitchAngle = -30
    AHFD_LOCK_SP_UP_270,    ///< rollAngle = -90,                           yawAngle = 45 and -45    pitchAngle =  30
    AHFD_LOCK_SP_DOWN_270,  ///< rollAngle = -90,                           yawAngle = 45 and -45    pitchAngle = -30
    AHFD_LOCK_P_UP_270,     ///< rollAngle = -90,                           yawAngle = 45 and -45    pitchAngle =  30
    AHFD_LOCK_P_DOWN_270    ///< rollAngle = -90,                           yawAngle = 45 and -45    pitchAngle = -30
};

///////////////////////////////////////////////////////////////////////////
// AHFD_FACE
/**
  Contains information about the detected face.
@remarks
  This structure is only created by the library and should be treated as
  semi-opaque and read-only.
**************************************************************************/
typedef struct AHFD_FACE
{
    /** The size, in bytes, of the AHFD_FACE structure. The library will set this field.
      The application may check this field to decide whether it can access the fields
      added later in API.
    */
    UINT32      structSize;

    /** Specifies the bounding rectangle of a face, in fixed point
      with 8 bits of precision.
    */
    GT_RECT     boundRect;

    /** The face roll angle in degrees. Valid output range is [-179, 180].
      Represents in plane rotation:
      @verbatim
         90 deg.       45 deg.       0 deg.       -45 deg.      -90 deg.       180 deg.
         ______        ______        ______        ______        ______        ______
       /        \    /     x  \    /        \    /   x    \    /        \    /        \
      |  |    x  |  | \        |  |  x    x  |  |        / |  |  x    |  |  |   ----   |
      |  |       |  |  \     x |  |          |  |  x    /  |  |       |  |  |          |
       \ |    x /    \  \     /    \  ----  /    \     /  /    \ x    | /    \ x    x /
         ------        ------        ------        ------        ------        ------
      @endverbatim
    */
    INT32       rollAngle;

    /** The face yaw angle in degrees. Valid output range is [-90, 90].
      Represents out of plane rotation:
      @verbatim
          90 deg.       45 deg.      0 deg.       -45 deg.       -90 deg.
         ______        ______        ______        ______         ______
       /        \    /        \    /        \    /        \     /        \
      /x         |  |x   x     |  |  x    x  |  |     x   x|   |        x \
      |          |  |          |  |          |  |          |   |          |
       \--      /    \ ---    /    \  ----  /    \    --- /     \      --/
         ------        ------        ------        ------         ------
      @endverbatim
    */
    INT32       yawAngle;

    /** Specifies the confidence of the detection. The higher the value is,
      the more the confidence of the detection algorithm is.
      Valid range is [1, 1000] interval.
    */
    UINT32      confidence;

    /** The unique ID for the tracked face. If the algorithm considers that
      this is the same face as previously detected or tracked then the same
      ID is assigned. If the face is lost then its ID is not reused.
    */
    UINT32      trackedFaceId;

    /** Specifies the bounding rectangle of the left eye, in fixed point
      with 8 bits of precision.

      @note
        In order the eyes to be detected the library must be compiled with
        corresponding support and ::AHFD_DETECT_EYES or ::AHFD_DETECT_BLINK flag
        must be set in AHFD_DETECTOR_CFG::flags.
    */
    GT_RECT     eyeL;

    /** Same meaning as for AHFD_FACE::eyeL, but for the right eye.
    */
    GT_RECT     eyeR;

    /** Specifies the bounding rectangle of the mouth, in fixed point
    with 8 bits of precision.

      @note
        To detect the mouth, the library must be compiled with corresponding
        support and ::AHFD_DETECT_MOUTH flag must be set in AHFD_DETECTOR_CFG::flags.
    */
    GT_RECT     mouth;

    /** Indicates the presence of a blink for the left eye.
      Following values are used:
        - (-1) - no blink.
        - (0) - not able to detect.
        - (1 ... 100) - the higher is the value the more confidence is
          that a blink is present.
        - a 50 threshold is recommended for a trade-off between detection rate
          and false positives rate.

      @note
      In order the blink to be detected, the library must be compiled with
      corresponding support and ::AHFD_DETECT_BLINK flag must be set in
      AHFD_DETECTOR_CFG::flags.
    */
    INT32       blinkLevelL;

    /** Same meaning as for AHFD_FACE::blinkLevelL,
      but for the right eye.
    */
    INT32       blinkLevelR;

    /** Indicates the presence of a smile. Following values are used:
      - (-1) - no smile.
      - (0) - not able to detect.
      - (1 ... 100) - the higher is the value the more confidence is
        that a smile is present.
      - a 50 threshold is recommended for a trade-off between detection rate
        and false positives rate.

      @note
        In order the smile to be detected the library must be compiled with
        corresponding support and ::AHFD_DETECT_SMILE flag must be set in
        AHFD_DETECTOR_CFG::flags.
    */
    INT32       smileLevel;

    /** Indicates the presence of a child. Following values are used:
      - (-1) - the face is of an adult.
      - (0) - not able to detect.
      - (1 ... 100) - the higher is the value the more confidence is
        that the face is of an child.
      - a 50 threshold is recommended for a trade-off between detection rate
        and false positives rate.

      @note
        In order the child detection to work, the library must be compiled with
        corresponding support and ::AHFD_DETECT_CHILD flag must be set in
        AHFD_DETECTOR_CFG::flags.
    */
    INT32       childLevel;

    /** The face pitch angle in degrees. Valid output range is [-90, 90].
        Represents down-up rotation.

      @note
        In order to detect faces at positive and negative pitch angles,
        the library must report the support using the flag
        ::AHFD_FEATURE_PITCH_ANGLES and also ::AHFD_DISABLE_PITCH_ANGLES
        must NOT be set by the user in AHFD_DETECTOR_CFG::flags.

      @verbatim
         30 deg.       0 deg.       -30 deg.
         ______        ______       ______
       / x    x \    /        \   /        \
      |          |  |  x    x  | |          |
      |   ----   |  |          | |  x    x  |
       \        /    \  ----  /   \  ----  /
         ------        ------       ------
      @endverbatim

      @version
        Minimal required @ref AHFDAPI_Ver is 0x00010006.
    */
    INT32       pitchAngle;
} AHFD_FACE;

///////////////////////////////////////////////////////////////////////////
// AHFD_RAW_FACE
/**
  Contains raw data about the detected face.
  Raw data is not post-processed any way after the detection is done.
@remarks
  This structure is only created by the library and should be treated as
  semi-opaque and read-only.
**************************************************************************/
typedef struct AHFD_RAW_FACE
{
    /** The size, in bytes, of the AHFD_RAW_FACE structure. The library will set this field.
      The application may check this field to decide whether it can access the fields
      added later in API.
    */
    UINT32      structSize;

    /** Specifies the bounding rectangle of a face, in fixed point
      with 8 bits of precision.
    */
    GT_RECT     boundRect;

    /** Specifies the bounding rectangle of the left eye, in fixed point
      with 8 bits of precision.

      @note
        In order the eyes to be detected the library must be compiled with
        corresponding support and ::AHFD_DETECT_EYES or ::AHFD_DETECT_BLINK flag
        must be set in AHFD_DETECTOR_CFG::flags.
    */
    GT_RECT     eyeL;

    /** Same meaning as for AHFD_RAW_FACE::eyeL, but for the right eye.
    */
    GT_RECT     eyeR;
} AHFD_RAW_FACE;

///////////////////////////////////////////////////////////////////////////
// AHFD_CONFIG_FLAGS
/** The list of flags used by detector configuration.
  These flags are used in @ref AHFD_DETECTOR_CFG::flags.
@ingroup AHFDAPI_Det
**************************************************************************/
enum AHFD_CONFIG_FLAGS
{
    /// If set, the library performs detections on tracking, treating
    /// faces and images as part of a time sequence (video frame).
    /// @hideinitializer
    AHFD_TRACKING_MODE                        = (1 << 0),

    /// If set, the library will perform tracking only on
    /// luminance only, ignoring the color channels.
    /// @hideinitializer
    AHFD_LUMINANCE_ONLY                       = (1 << 1),

    /// If set, only faces that were confirmed at least on one more
    /// frame will be reported by the library.
    /// @hideinitializer
    AHFD_WAIT_FACE_CONFIRMATION               = (1 << 2),

    /// If set and the library is compiled with eye detection
    /// support then the library reports the position of eyes
    /// in AHFD_FACE::eyeL and AHFD_FACE::eyeR.
    ///
    /// If the library does not include the corresponding
    /// support, then AHFD_LIB_VFT::CreateDetector() returns
    /// ::GTS_NOT_SUPPORTED when setting this flag.
    ///
    /// Setting this flag improves the smile, mouth and
    /// child detection (see ::AHFD_DETECT_SMILE,
    /// ::AHFD_DETECT_MOUTH and ::AHFD_DETECT_CHILD).
    /// @hideinitializer
    AHFD_DETECT_EYES                          = (1 << 3),

    /// If set and the library is compiled with blink detection
    /// support then the library reports the presence of a blink
    /// in AHFD_FACE::blinkLevel, AHFD_FACE::blinkLevelL and
    /// AHFD_FACE::blinkLevelR.
    ///
    /// If the library does not include the corresponding
    /// support, then AHFD_LIB_VFT::CreateDetector() returns
    /// ::GTS_NOT_SUPPORTED when setting this flag.
    /// @hideinitializer
    AHFD_DETECT_BLINK                         = (1 << 4),

    /// If set and the library is compiled with smile detection
    /// support then the library reports the presence of a smile
    /// in AHFD_FACE::smileLevel.
    ///
    /// If the library does not include the corresponding
    /// support, then AHFD_LIB_VFT::CreateDetector() returns
    /// ::GTS_NOT_SUPPORTED when setting this flag.
    ///
    /// To improve the smile detection also set the ::AHFD_DETECT_EYES
    /// (or ::AHFD_DETECT_BLINK) flag.
    /// @hideinitializer
    AHFD_DETECT_SMILE                         = (1 << 5),

    /// If set and the library is compiled with mouth detection
    /// support then the library reports the position of mouth
    /// in AHFD_FACE::mouth.
    ///
    /// If the library does not include the corresponding
    /// support, then AHFD_LIB_VFT::CreateDetector() returns
    /// ::GTS_NOT_SUPPORTED when setting this flag.
    /// @hideinitializer
    AHFD_DETECT_MOUTH                         = (1 << 6),

    /// If set and the library is compiled with child detection
    /// support then the library reports the presence of a child
    /// in AHFD_FACE::childLevel.
    ///
    /// If the library does not include the corresponding
    /// support, then AHFD_LIB_VFT::CreateDetector() returns
    /// ::GTS_NOT_SUPPORTED when setting this flag.
    /// @hideinitializer
    AHFD_DETECT_CHILD                         = (1 << 7),

    /// If set, the library will request from the AHFD Hardware a
    /// better precision Hardware map. This implies a higher
    /// memory bandwidth generated by the Hardware and more memory
    /// consumption, with the benefit of better detection.
    ///
    /// If this feature is not supported by the Hardware,
    /// then AHFD_LIB_VFT::CreateDetector() returns
    /// ::GTS_NOT_SUPPORTED when setting this flag.
    /// The support for this feature is reported by
    /// ::AHFD_FEATURE_BETTER_PRECISION_HW_MAP flag.
    ///
    /// @note
    ///     For now this flag improves the detection of face features
    ///     and is used only if at least one of the following flags is set:
    ///     ::AHFD_DETECT_EYES, ::AHFD_DETECT_BLINK.
    ///
    /// @version
    ///     Minimal required @ref AHFDAPI_Ver is 0x00010003.
    /// @hideinitializer
    AHFD_BETTER_PRECISION_HW_MAP              = (1 << 8),

    /// If set, the library will cycle the orientation of the
    /// image by 90 degrees when no faces are detected. The
    /// change of the image orientation is performed after
    /// a number of frames from the last update. This allows
    /// the detection of faces oriented at angles not covered by the
    /// @ref AHFD_DETECTOR_CFG::lockAngles "lock angles sequence".
    ///
    /// If this flag is activated, AHFD_DETECTOR_VFT::UpdateImageOrientation
    /// calls are ignored, and GTS_NOT_SUPPORTED is returned if that function
    /// is called.
    ///
    /// @note
    ///     This flag is ignored if ::AHFD_TRACKING_MODE flag is not set.
    /// @version
    ///     Minimal required @ref AHFDAPI_Ver is 0x00010003.
    /// @hideinitializer
    AHFD_AUTO_ORIENTATION                     = (1 << 9),

    /// If set, the library will disable the detection of faces
    /// that have positive or negative pitch angles.
    /// In this case, @ref AHFD_FACE::pitchAngle will be always zero.
    ///
    /// @note
    ///     Setting this flag, may decrease the overall
    ///     processing time (HW & SW).
    /// @note
    ///     The support for pitch angles feature is reported by
    ///     ::AHFD_FEATURE_PITCH_ANGLES flag.
    /// @version
    ///     Minimal required @ref AHFDAPI_Ver is 0x00010006.
    /// @hideinitializer
    AHFD_DISABLE_PITCH_ANGLES                 = (1 << 10)
};

///////////////////////////////////////////////////////////////////////////
// AHFD_DETECTOR_CFG
/**
  Contains detector configuration used when calling AHFD_LIB_VFT::CreateDetector().
@ingroup AHFDAPI_Det
**************************************************************************/
typedef struct AHFD_DETECTOR_CFG
{
    /** The size, in bytes, of this structure. Set to <i>sizeof(AHFD_DETECTOR_CFG)</i>
      before calling any function that uses this structure.
    */
    UINT32      structSize;

    /** @ref AHFD_CONFIG_FLAGS "Configuration flags." All other bits that
      are not specified here must be zero.
    */
    UINT32      flags;

    /**
      The resolution of the image used to detect faces.
    */
    GT_SIZE     workRes;

    /** Specifies the minimum face size in pixels the library should try to
      detect.

      The allowed range for this parameter is <i>[12, min(workRes.width, workRes.height)]</i>.
      If zero is specified (default) then it will try to find faces down until 14x14 pixels.

      Increasing this value will result in better performance.
    */
    UINT32      minFaceSize;

    /** Pointer to an array that specifies the angles on which
      faces will be detected.See enum AHFD_LOCK for possible values.

      Typical example: {AHFD_LOCK_0,    AHFD_LOCK_90,  AHFD_LOCK_270,
                        AHFD_LOCK_SP_0, AHFD_LOCK_P_0, AHFD_LOCK_END};

      @note
        - The order in which the angles are specified is not taken into
          account, one angle should not appear more than once in the array
          and the last element in the array must be AHFD_LOCK_END.
        - This parameter can also be NULL. In this case default lock
          angle sequence will be used.
      */
    const INT32 *lockAngles;

    /** Specifies the level of smoothing of the face tracking output.
      Smoothing prevents abrupt changes of the reported face position and size
      that can happen because of random factors.
      The valid values are in range [0 ... 100], where:
        - 0 - no smoothing;
        - 50 - medium smoothing;
        - 100 - highest smoothing.
      <P><B>Recommended value: 50 (in tracking mode)</B></P> */
    UINT32      smoothing;

    /** Specifies the number of frames to keep reporting a face after it is lost.
        - 0 - the library uses a default number of frames.
        - -1 - do not keep reporting the face after it is lost.
        - 1...30 - keep the lost face for specified number of frames more.
    */
    INT32       keepFacesOverMoreFrames;

    /** In static mode, image can be scanned more than once, to improve
      chances of finding faces in difficult lighting conditions.
      Possible values:
       - 0 or 1 - search image only once
       - 2 - search image twice
      Other values are reserved for future use.
      */
    UINT32      numStaticSearches;

    /** Maximum number of faces that will go through feature
      detection (eye, blink, smile, ...). If this value is 0,
      then no limitation will be imposed.

      @note
        If more faces than this number are detected, then only on
        the biggest faces the features will be detected.
    */
    UINT32      maxFaceCountForFeatures;

    /** Maximum number of faces that will be tracked.
    */
    UINT32      maxFaceCount;

    /** Boost Detection Rate(DR) vs False Positives(FP).
      Accepted values [-10;+10]
       - -10 - very strict, low DR, low FP
       -   0 - default
       -  10 - very relaxed, high DR, high FP

      @version
        Minimal required @ref AHFDAPI_Ver is 0x00010004.
     */
    INT32      boostFDvsFP;
} AHFD_DETECTOR_CFG;

///////////////////////////////////////////////////////////////////////////
// Advanced Hardware-Based Face Detection Interface
/**
@weakgroup AHFDAPI_Det
  The detector object exposes the main functionality.
  It is represented by AHFD_DETECTOR structure that provides pointers to the
  detector functions which implements the following functionality:
  - Face detection.
  - Eye detection (optional).
  - Blink detection (optional).
  - Smile detection (optional).
  - Mouth detection (optional).
  - Child detection (optional).
@sa AHFD_DETECTOR
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
// AHFD_DETECTOR
/**
  Represents the detector interface.
  To retrieve this structure, call AHFD_LIB_VFT::CreateDetector().
@ingroup AHFDAPI_Det
**************************************************************************/
typedef struct AHFD_DETECTOR
{
    /** Pointer to the function table. */
    const struct AHFD_DETECTOR_VFT* vft;
} AHFD_DETECTOR;

///////////////////////////////////////////////////////////////////////////
// AHFD_DETECTOR_VFT
/**
  Contains pointers to AHFD_DETECTOR functions.
@ingroup AHFDAPI_Det
**************************************************************************/
struct AHFD_DETECTOR_VFT
{
    // Destroy()
    //
    /**
      Destroys the detector object.
    @param [in] det
      Pointer to the detector object to be destroyed.
    */
    void (GTCALL* Destroy)(
        IN AHFD_DETECTOR* det
        );

    // DetectFaces()
    //
    /**
       Detects faces in an image and face features (eyes, blink, smile..) if they are enabled.
    @param [in] det
      Pointer to the detector object.
    @return
      - ::GTS_OK - success.
      - ::AHFDS_NOT_FOUND - no faces were found.
      - ::AHFDS_CALL_AGAIN - An intermediary step of the face detection was completed.
                             To finish the face detection, an additional hardware processing
                             is required, followed by another call to DetectFaces.
      - Other - error.
    @note
      In @ref AHFDAPI_Modes "static mode", the face detection may require more hardware passes on
      the image before the algorithm finishes.
      This condition is signaled by the return code being equal to AHFDS_CALL_AGAIN.
    */
    GTSTATUS (GTCALL* DetectFaces)(
      IN AHFD_DETECTOR* det
      );

    // EnumFaces()
    //
    /**
      Enumerates faces found by the last call to DetectFaces.
    @param[in] det
      Pointer to detector object.
    @param[in] face
      Pointer to the current face.
    @return
      Returns the pointer to a face that follows the face specified by @a face parameter.
    @remarks
      - If @a face is the last face in the list, the function returns NULL.
      - If @a face is NULL, the function returns first face in the list.
      - The returned AHFD_FACE pointer is valid until next call of
        AHFD_DETECTOR_VFT::DetectFaces(). Calling AHFD_DETECTOR_VFT::Destroy()
        will also make the pointer invalid.
    @par Example:
      @ref AHFDAPI_Examples_EnumFaces "Example"
    */
    const AHFD_FACE* (GTCALL *EnumFaces)(
        IN AHFD_DETECTOR* det,
        IN const AHFD_FACE *face
        );

    // ClearFaces()
    //
    /**
      Empties the history of face detection entries.
      This is only relevant for detectors configured in @ref AHFDAPI_Modes "tracking mode".
    @param[in] det
      Pointer to detector object.
    @return
      - ::GTS_OK - success.
      - Other return codes indicate an error.
    */
    GTSTATUS (GTCALL *ClearFaces)(
        IN AHFD_DETECTOR* det
        );

    // UpdateImageOrientation()
    //
    /**
      Updates the angle of the image at the input in AHFD hardware.
    @param[in] det
      Pointer to detector object.
    @param[in] rollAngle
      Represents the clockwise angle for in-plane rotation of the image.
      Valid range is [-179, 180].
    @return
      - ::GTS_OK - success.
      - Other return codes indicate an error.
    @remarks
      - This function can be called anytime.
      - The updated image angle is used for processing the next frame.
      - If AHFD_AUTO_ORIENTATION flag is set, this function is ignored, and
      a GTS_NOT_SUPPORTED error is returned if it is called.
    */
    GTSTATUS (GTCALL *UpdateImageOrientation)(
        IN AHFD_DETECTOR* det,
        IN INT32 rollAngle
        );

    // GetMainFace()
    //
    /**
      Selects a main face from the current face lists.
    @param[in] det
      Pointer to detector object.
    @param[in] flags
      Reserved parameter. Must be set to ZERO.
    @return
      - The pointer to the selected face
      - NULL - if the face list is empty or the main face cannot be chosen.
    @remarks
      - If not NULL, then the returned pointer is the one among the faces in the
        list which can be obtained by AHFD_DETECTOR_VFT::EnumFaces().
      - The returned AHFD_FACE pointer is valid until next call of
        AHFD_DETECTOR_VFT::DetectFaces() - same life time as for faces returned
        by AHFD_DETECTOR_VFT::EnumFaces().
    @version
      Minimal required @ref AHFDAPI_Ver is 0x00010002.
    */
    const AHFD_FACE* (GTCALL *GetMainFace)(
        IN AHFD_DETECTOR* det,
        IN UINT32 flags
        );

    // GetRawFaceData()
    //
    /**
      Returns raw data for the given face.
      The raw data is not post-processed in any way after the detection is done.
    @param[in] det
      Pointer to detector object.
    @param[in] face
      The face for which the raw data must be returned.
    @return
      - The pointer to the raw data associated with the given face.
      - NULL - if the given face is NULL or invalid.
    @remarks
      - If the detector is configured to run in static mode,
        this function will return the same information as the one found in AHFD_FACE,
        because no post-processing is applied for static mode.
      - The returned AHFD_RAW_FACE pointer is valid until next call of
        AHFD_DETECTOR_VFT::DetectFaces() - same life time as for faces returned
        by AHFD_DETECTOR_VFT::EnumFaces().
    @version
      Minimal required @ref AHFDAPI_Ver is 0x00010005.
    */
    const AHFD_RAW_FACE* (GTCALL *GetRawFaceData)(
        IN AHFD_DETECTOR* det,
        IN const AHFD_FACE* face
        );
};

///////////////////////////////////////////////////////////////////////////
// Library Interface
/**
@weakgroup AHFDAPI_Lib
  The library object is the root object in the library API.
  It is represented by AHFD_LIB structure that provides pointers to the
  library functions.
@sa AHFD_LIB
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
// AHFD_FEATURE_*
/** The list of flags used to specify the features that are available
  in the library.  This flags are found in AHFD_LIB_INFO::features.
@ingroup AHFDAPI_Lib
**************************************************************************/
enum AHFD_FEATURES_FLAGS
{
    /// This flag is present when the library can create heap
    /// objects from a given buffer.
    /// @hideinitializer
    AHFD_FEATURE_HEAP                       = (1 << 0),

    /// This flag is present when library supports eye detection.
    /// @hideinitializer
    AHFD_FEATURE_EYE                        = (1 << 1),

    /// This flag is present when library supports blink detection.
    /// @hideinitializer
    AHFD_FEATURE_BLINK                      = (1 << 2),

    /// This flag is present when library supports smile detection.
    /// @hideinitializer
    AHFD_FEATURE_SMILE                      = (1 << 3),

    /// This flag is present when library supports mouth detection.
    /// @hideinitializer
    AHFD_FEATURE_MOUTH                      = (1 << 4),

    /// This flag is present when library supports child detection.
    /// @hideinitializer
    AHFD_FEATURE_CHILD                      = (1 << 5),

    /// This flag is present when the Hardware supports output of
    /// better precision map, to improve detection.
    /// @version
    ///     This feature is reported beginning with @ref AHFDAPI_Ver 0x00010003.
    /// @hideinitializer
    AHFD_FEATURE_BETTER_PRECISION_HW_MAP    = (1 << 6),

    /// This flag is present when the library can create its own heap,
    /// so the user is not required to provide a custom heap, if he chooses so.
    /// @version
    ///     This feature is reported beginning with @ref AHFDAPI_Ver 0x00010003.
    /// @hideinitializer
    AHFD_FEATURE_INTERNAL_HEAP              = (1 << 7),

    /// This flag is present when the library can detect faces at positive and
    /// negative pitch angles.
    /// @version
    ///     This feature is reported beginning with @ref AHFDAPI_Ver 0x00010006.
    /// @hideinitializer
    AHFD_FEATURE_PITCH_ANGLES               = (1 << 8)
};

///////////////////////////////////////////////////////////////////////////
// AHFD_LIB_INFO
/**
  Contains information about the library version.
  See AHFD_LIB_VFT::GetLibInfo().
@ingroup AHFDAPI_Lib
**************************************************************************/
typedef struct AHFD_LIB_INFO
{
    /** The size, in bytes, of this structure.
    */
    UINT32 structSize;

    /** The library version numbers.
    */
    UINT32 libVer[5];

    /** @ref AHFDAPI_Ver "API version" number.
    */
    UINT32 apiVer;

    /** Pointer to the string specifying build date in form "Mmm dd yyyy hh:mm:ss".
        Same format as in __DATE__ and __TIME__ macros.
    */
    const char* buildDate;

    /** Pointer to the optional string specifying the release label.
    */
    const char* label;

    /** A bitmask specifying supported features. See ::AHFD_FEATURES_FLAGS.
    */
    UINT32 features;
} AHFD_LIB_INFO;

///////////////////////////////////////////////////////////////////////////
// AHFD_LIB
/**
  Represents the library interface.
  To retrieve this structure, call AhfdLibGet().
@ingroup AHFDAPI_Lib
**************************************************************************/
typedef struct AHFD_LIB
{
    /** Pointer to the function table. */
    const struct AHFD_LIB_VFT* vft;
} AHFD_LIB;

///////////////////////////////////////////////////////////////////////////
// AHFD_LIB_VFT
/**
  Contains pointers to AHFD_LIB functions.
@ingroup AHFDAPI_Lib
**************************************************************************/
struct AHFD_LIB_VFT
{
    // GetLibInfo()
    //
    /** Returns information about library version.
    @param [in] lib
       A pointer to the library object.
    @return
       A pointer to AHFD_LIB_INFO containing version information.
    */
    const AHFD_LIB_INFO* (GTCALL* GetLibInfo)(
        IN AHFD_LIB* lib);

    // CreateHeap()
    //
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
       - ::GTS_OK - success.
       - ::GTS_NOT_SUPPORTED - this functionality is not included in this
                               library version.
       - Otherwise - other error.
    @remarks
       This function is intended for platforms without own heap management.
       It uses provided buffer to create a heap that can be supplied to
       AHFD_LIB_VFT::CreateDetector(). After the heap is not needed, call
       GT_HEAP_VFT::Destroy().
       @code
         AHFD_LIB* lib = AhfdLibGet();
         AHFD_DETECTOR* det = NULL;
         void *ahfdIO = NULL;
         GT_HEAP* heap = NULL;
         AHFD_DETECTOR_CFG cfg;

         memset(&cfg, 0, sizeof(AHFD_DETECTOR_CFG));
         cfg.structSize = sizeof(AHFD_DETECTOR_CFG);
         ...`
         status = lib->vft->CreateHeap(lib, heapBuf, heapSize, &heap);
         if (GTS_OK == status)
         {
             status = lib->vft->CreateDetector(lib, heap, &cfg, &ahfdIO, &det);
             if (GTS_OK == status)
             {
                 ...
                 det->vft->Destroy(det);
             }
             heap->vft->Destroy(heap);
         }
       @endcode
    @remarks
       Use GetRequiredHeapSize() to compute the required size of the heap buffer.
    */
    GTSTATUS (GTCALL* CreateHeap)(
        IN AHFD_LIB* lib,
        IN void* buffer,
        IN UINT32 size,
        OUT GT_HEAP** heap);

    // CreateDetector()
    //
    /** Creates the detector object.
    @param [in] lib
       A pointer to the library object.
    @param [in] heap
       Address of an optional heap object that shall be used for memory
       management.
       There are three options of providing the heap:
       - NULL value, to use the @ref ::AHFD_FEATURE_INTERNAL_HEAP "default internal heap":
         - If the library supports a default internal heap
           for given platform, that heap will be used instead.
         - Otherwise ::GTS_NO_SYSTEM_RESOURCES error is returned.
       - Call AHFD_LIB_VFT::CreateHeap() to create a heap from a buffer.
       - Implement a custom GT_HEAP. See @ref gtAPI_Heap for an example.
       .
    @param [in] cfg
       Pointer to the configuration structure.
    @param [out] ahfdIO
       A pointer to a caller supplied variable. On success it will contain the
       pointer to the AHFD hardware<->software communication interface.
    @param [out] det
       A pointer to a caller supplied variable. On success it will contain the
       pointer to the newly created detector object.
    @return
       - ::GTS_OK - success.
       - Otherwise - error.
    @remarks
      - When the detector object is no longer needed, call AHFD_DETECTOR_VFT::Destroy().
      - If the application provided a non NULL heap, then it is responsible for
        destroying the heap after destroying the detector.
      - Example:
        @code
          AHFD_LIB* lib = AhfdLibGet();
          AHFD_DETECTOR* det = NULL;
          GT_HEAP* heap = NULL;
          void *ahfdIO = NULL;
          AHFD_DETECTOR_CFG cfg;

          memset(&cfg, 0, sizeof(AHFD_DETECTOR_CFG));
          cfg.structSize = sizeof(AHFD_DETECTOR_CFG);
          ...
          status = lib->vft->CreateDetector(lib, heap, &cfg, &ahfdIO, &det);
          if (GTS_OK == status)
          {
              ...
              det->vft->Destroy(det);
          }
        @endcode
    */
    GTSTATUS (GTCALL* CreateDetector)(
        IN AHFD_LIB* lib,
        IN GT_HEAP* heap OPTIONAL,
        IN const AHFD_DETECTOR_CFG* cfg,
        OUT void** ahfdIO,
        OUT AHFD_DETECTOR** det);

    // GetRequiredHeapSize()
    //
    /** Returns the required size of the heap needed by AHFD_DETECTOR to
      function properly.
    @param [in] lib
       A pointer to the library object.
    @param [in] cfg
       Pointer to the configuration structure used by CreateDetector.
    @return
       The required size (in bytes) of the heap for AHFD_DETECTOR,
       or 0 if the given configuration is invalid.
    */
    UINT32 (GTCALL* GetRequiredHeapSize)(
        IN AHFD_LIB* lib,
        IN const AHFD_DETECTOR_CFG* cfg);
};

///////////////////////////////////////////////////////////////////////////
// AhfdLibGet()
/**
  Library entry point function.
@return
  The pointer to AHFD_LIB structure representing the library interface.
@remarks
  This is the first function that needs to be called. The AHFD_LIB
  structure gives access to other function pointers.
  @code
    AHFD_LIB* lib = AhfdLibGet();
    const AHFD_LIB_INFO* libInfo = lib->vft->GetLibInfo(lib);
  @endcode
@ingroup AHFDAPI_Lib
**************************************************************************/
GTAPI AHFD_LIB* GTCALL AhfdLibGet(void);

#ifdef __cplusplus
}
#endif

#endif // __LIBAHFD_H__
