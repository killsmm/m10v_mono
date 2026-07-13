/*----------------------------------------------------

High Dynamic Range library

Copyright (c) FotoNation
All rights reserved

----------------------------------------------------*/

#ifndef __HDR_H__
#define __HDR_H__


#include "gtypes.h"


#ifdef  __cplusplus
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////
/**
@defgroup api_ver API Versions
  API version list and history of changes.
@{
  The API version is a 32-bit value where the higher 16 bits represent the
  major version and the lower 16 bits represent the minor version.
  When the minor is increased the API remains backward compatible. When
  the major is increased the API may not be backward compatible.

  To retrieve the API version, call HDR_LIB_VFT::GetLibInfo() and read
  the HDR_LIB_INFO::apiVer field. It is different from the library version
  (HDR_LIB_INFO::libVer). Library version refers to the implementation,
  however API version refers only to the API definition.

  Below is the list of API versions, and the corresponding changes which
  were introduced in every version.

  - 0x00070002
    - Added the flags for face priority and contrast enhancement. These are passed via the "options" structure to GetResult function.

  - 0x00070000
    - Added a new parameter for effect strength in GetResult function.

  - 0x00060000
    - API full redesign for VideoHDR.

  - 0x00050000
    - Removed HDR_ENGINE_VFT::GetMaxRequiredImages. The array of settings returned by
      HDR_ENGINE_VFT::GetAcquisitionExposures will be allocated by the library;
      the function now returns the pointer to the array.

  - 0x00040000
    - Split exposure representation in two types: by camera settings (aperture, exposure time, iso),
      and by EV value directly, to support the latter case. Added @ref HDR_EXPOSURE "HDR_EXPOSURE"
      structure, and HDR_CONFIG::expIndicationType to select the representation. Modified HDR_METADATA
      and camera parameter ranges representation in HDR_CONFIG accordingly.
    - Removed HDR_PREVIEW_PARAMS and HDR_AQ_PARAMS and replaced with HDR_EXPOSURE the respective
      parameters in HDR_ENGINE_VFT::GetAcquisitionExposures.

  - 0x00030000
    - Added processing modes for describing how the engine processing is performed.
      A single mode is currently available: one step processing (HDRE_MODE_ONE_STEP).
    - Changed HDR_LIB_VFT::GetRequiredHeapSize to take a HDR_USAGE_INFO
      structure as input.
    - Added HDR_OPTIONS parameter to HDR_ENGINE_VFT::GetResult.
    - Removed HDR_OPTIONS::cameraIsStatic flag.
    - Removed GT_IMAGE::imageInUse flag.
    - Moved the HDR_PREVIEW_PARAMS::fpsFullSize parameter to HDR_CONFIG.

  - 0x00020000
    - Using common base header <i>gtypes.h</i>.
    - Added preview analysis and guided acquisition by introducing HDR_ENGINE object
      with the following functions:
        HDR_ENGINE_VFT::AnalyzePreview,
        HDR_ENGINE_VFT::GetMaxRequiredImages
        HDR_ENGINE_VFT::GetAcquisitionExposures
        HDR_ENGINE_VFT::ProcessSequence
        HDR_ENGINE_VFT::GetResult
    - Processing functions now refer to images using arrays of structures.
    - Added HDRS_AQ_* status codes.
    - Renamed <i>HDR_INFO</i> to <i>HDR_LIB_INFO</i>, and in <i>HDR_LIB_VFT</i> <i>GetInfo</i> to <i>GetLibInfo</i>.
    - Added to HDR_LIB_INFO the @ref HDR_LIB_INFO::supportedFmts "supportedFmts" and @ref HDR_LIB_INFO::label "label"
      parameters, and changed @ref HDR_LIB_INFO::libVer "libVer" to an array of 5 numbers.
    - Added HDR_LIB* parameter to HDR_LIB object functions.
    - Renamed <i>HdrGetLib</i> to <i>HdrLibGet</i>.

  - 0x00010001
    - Added to the HDR_LIB object the following function:
        @ref HDR_LIB_VFT::GetRequiredHeapSize "GetRequiredHeapSize".
    - Added <i>HDRS_IM_*</i> error codes.
    - Added progress notification support.

  - 0x00010000
    - Initial version.
*/
/// @} end of api_ver


///////////////////////////////////////////////////////////////////////////
/**
@defgroup api_supfmts Supported Image Formats
  List of supported color formats.
@{
  The High Dynamic Range library supports only the formats below:
  - GT_FMT_YUV_422_P3
  - GT_FMT_YUV_420_P3
  - GT_FMT_YUYV_422
  - GT_FMT_UYVY_422
  - GT_FMT_YUV_422_P2 (NV12)
  - GT_FMT_YUV_420_P2 (NV12)
  - GT_FMT_YVU_420_P2 (NV21)

  These formats are all supported natively by the library. Other formats can be
  added on request.

  For more details about these color formats, please see @ref gtAPI_Fmt
*/
/// @} end of api_supfmts



///////////////////////////////////////////////////////////////////////////
// HDRS_*
/**
@weakgroup gtAPI_Status Status Codes
  These constants indicate the result of an API function call.
@{
**************************************************************************/

/** Returned by HDR_ENGINE_VFT::RegisterImage to indicate that the information sent in the
  HDR_EXPOSURE parameter does not correspond to the camera settings declared previously in HDR_CONFIG. @hideinitializer */
#define HDRS_INVALID_EXP_SETTING        (-101)

/** Returned by HDR_ENGINE_VFT::GetExposureUpdate if it is called in the wrong engine mode. @hideinitializer */
#define HDRS_NOT_AVAILABLE              (-102)

/** Returned by HDR_ENGINE_VFT::GetResult if it is called before calling HDR_ENGINE_VFT::RegisterImage. @hideinitializer */
#define HDRS_NO_REGISTERED_IMAGES       (-103)

/** Returned by HDR_ENGINE_VFT::GetResult if the registered images have an invalid content (overburned, images too different, etc) @hideinitializer */
#define HDRS_INVALID_IMAGE_CONTENT      (-104)

/// @} end of gtAPI_Status



///////////////////////////////////////////////////////////////////////////
// HDR Engine
/**
@defgroup api_eng HDR Engine
@{
**************************************************************************/


///////////////////////////////////////////////////////////////////////////
// HDRE_EXPOSURE_INDICATION
/**
  Group of constants which indicates how the system can offer information
  about the exposure.
*/
enum HDRE_EXPOSURE_INDICATION
{
    HDRE_EXP_UNUSED       = 0,  ///< Exposure descriptions are not used.
    HDRE_EXP_CAM_SETTINGS = 1,  ///< Exposure is described by providing the values for aperture, exposure time and ISO speed.
    HDRE_EXP_VALUE        = 2   ///< Exposure is described by providing directly the value in EV.
};

///////////////////////////////////////////////////////////////////////////
// HDR_EXPOSURE
/**
  Contains two representations for expressing exposure: either indirectly with
  the camera's aperture, exposure time, and ISO speed, either directly by specifying
  the exposure in EV units.
  The exposure representation used by the library in the functions which use an
  image's exposure is the one selected in @ref HDR_CONFIG::expIndicationType when
  creating the @ref HDR_ENGINE object.
*/
typedef struct
{
    /** One of the @ref HDRE_EXPOSURE_INDICATION constants which specify which parameters
      to use for expressing the current exposure settings. Cannot be set to HDRE_EXP_UNUSED. */
    UINT32  type;

    /** Used if <i>type</i> is set to <i>HDRE_EXP_CAM_SETTINGS</i>.
      Camera acquisition parameters which express the exposure. */
    struct {
        /** The exposure time expressed in microseconds. For example, for
          an exposure time of 1/2000s (0.0005s=500us), this parameter will be set to
          500 (0.00005ms * 10^6). */
        INT32   exposureTime;

        /** The aperture value in 1/10 units. For example, for f/5.6 this parameter
          will be set to 56. */
        INT32   aperture;
    
        /** The ISO speed rating on the arithmetic scale. */
        INT32   iso;
    } p;

    /** Used if <i>type</i> is set to <i>HDRE_EXP_VALUE</i>.
      Exposure value expressed directly in 1/10 of EV units. For example, if the
      exposure is of 9.5EV, this parameter will be set to 95. */
    INT32   ev;

} HDR_EXPOSURE;


///////////////////////////////////////////////////////////////////////////
// HDRE_PROGRESS_REASON
/**
  These constants indicate HDR_FN_PROGRESS callback routine reason values.
*/
enum HDRE_PROGRESS_REASON
{
    HDR_PROGRESS_BEGIN = 0,     ///< Start of operation. @hideinitializer
    HDR_PROGRESS_UPDATE,        ///< Operation step completed. @hideinitializer
    HDR_PROGRESS_END,           ///< End of operation. @hideinitializer
};

///////////////////////////////////////////////////////////////////////////
// HDR_FN_PROGRESS
/**
  Progress callback routine.
  The address of this callback is passed to library functions. It is called
  back when a portion of an operation is completed.
@param[in] context
  The application specific context passed to a library function alongside the
  progress callback routine.
@param[in] reason
  Indicates why the function is being called. This parameter can be one the
  @ref HDRE_PROGRESS_REASON values:
  <table>
  <tr><td class="indexkey">Value</td>
      <td class="indexkey">Meaning</td></tr>
  <tr><td>HDR_PROGRESS_BEGIN</td>
      <td>Indicates the beginning of the operation. The parameter
      @a param specifies the total number of steps this operation has.</td></tr>
  <tr><td>HDR_PROGRESS_UPDATE</td>
      <td>Indicates that the operation is being executed and
      the parameter @a param specifies number of completed steps.</td></tr>
  <tr><td>HDR_PROGRESS_END</td>
      <td>Indicates that the operation has finished.</td></tr>
  </table>
@param[in] param
  Parameter of callback routine.
@retval 0 Continue current operation.
@retval 1 Cancel current operation. The corresponding library function will return a
  GTS_CANCELLED code.
@remarks
  For example, an operation consisting of three steps will make the following sequence
  of calls to the progress callback:
  - HDR_PROGRESS_BEGIN(3)
  - HDR_PROGRESS_UPDATE(1)
  - HDR_PROGRESS_UPDATE(2)
  - HDR_PROGRESS_UPDATE(3)
  - HDR_PROGRESS_END(-)
*/
typedef UINT32 (GTCALL *HDR_FN_PROGRESS)(
    IN void*  context,
    IN UINT32 reason,
    IN UINT32 param
    );

///////////////////////////////////////////////////////////////////////////
// HDRE_CONFIG_FLAGS
/**
  Bitwise flags used for HDR configuration and during operation (GetResult)
*/
enum HDRE_CONFIG_FLAGS
{
	HDRF_ENABLE_FACE_PRIORITY        = 1,  ///< the algorithm will try to minimize artefacts on faces
	HDRF_ENABLE_CONTRAST_ENHANCEMENT = 2,  ///< apply an extra contrast enhancement process on the final image
};

///////////////////////////////////////////////////////////////////////////
// HDR_OPTIONS
/**
  HDR processing options structure.
*/
typedef struct
{
    /** The size, in bytes, of the ::HDR_OPTIONS structure. Set this member
      to <code>sizeof(HDR_OPTIONS)</code>. Be sure to set this member before calling
      any function that uses this structure. */
    UINT32          structSize;

    /** Caller-supplied function which will be called by the library to indicate
      progress information. This parameter can be NULL. */
    HDR_FN_PROGRESS progressCallback  OPTIONAL;

    /** Caller-supplied data passed to the progress callback. If @ref progressCallback
      is NULL, this parameter will be ignored. This parameter can be NULL. */
    void*           progressContext   OPTIONAL;

    /** Bit mask with flags defined in HDRE_CONFIG_FLAGS */
    UINT32  hdr_flags;

} HDR_OPTIONS;

///////////////////////////////////////////////////////////////////////////
// HDRE_SCENE_EFFECT
/**
  These constants indicate the tone mapping effect type.
*/
enum HDRE_SCENE_EFFECT
{
    HDRE_EFF_NATURAL = 0,       ///< Normal tone mapping, the scene will look natural.
    HDRE_EFF_SURREAL = 1,       ///< Applies an artistic effect to the scene, giving it a surreal look.
};


///////////////////////////////////////////////////////////////////////////
// HDRE_EVALUATION_RESULT
/**
  These constants are the possible results of the image analysis performed by function AnalyzeImage()
*/
enum HDRE_EVALUATION_RESULT
{
    HDRE_EVAL_NO_HDR             = 1,       ///< the image is well exposed and does not need HDR
    HDRE_EVAL_USE_DARK_AND_REF   = 2,       ///< the image contains overexposed areas; beside this image, an underexposed image is needed for generating HDR
    HDRE_EVAL_USE_REF_AND_LIGHT  = 3,       ///< the image contains underexposed areas; beside this image, an overexposed image is needed for generating HDR
    HDRE_EVAL_USE_DARK_AND_LIGHT = 4,       ///< the image contains both underexposed and overexposed areas; other overexposed and underexposed images are needed for generating HDR
};

///////////////////////////////////////////////////////////////////////////
// HDR_EXTERNAL_RESIZE


///////////////////////////////////////////////////////////////////////////
// Init external resize
/**
Application defined function that initializes external resize.
@return
This function returns GTS_OK if the initialization was successful.
If an error occurs, it will fail with a relevant general error code.
*/
typedef GTSTATUS (GTCALL *INIT_EXT_RESIZE)();

///////////////////////////////////////////////////////////////////////////
// External resize
/**
Application defined function that resizes the input GT_IMAGE and stores the result in the output GT_IMAGE.
@param[in] in
Pointer to the input @ref GT_IMAGE structure which represents the input image.
@param[out] out
Pointer to the output @ref GT_IMAGE structure which represents the output resized image.
@return
This function returns GTS_OK if the processing was successful.
If an error occurs, it will fail with a relevant general error code.
@remarks
- Input images adresses must be aligned at 8.
- In case of downsampling minimum input size is 128x128, maximum input size is 8192x8192,
minimum output size is 32x32, maximum size is 2048x2048.
- In case of downsampling the input cannot be grayscale.
- In case of upsampling maximum resize is 32x.
*/
typedef GTSTATUS(GTCALL *EXT_RESIZE)(
            IN  GT_IMAGE*           in,
            OUT GT_IMAGE*           out
            );

///////////////////////////////////////////////////////////////////////////
// Clean external resize
/**
Application defined function that frees external resize resources.
@return
This function returns GTS_OK if the closing was successful.
If an error occurs, it will fail with a relevant general error code.
*/
typedef GTSTATUS(GTCALL *CLEAN_EXT_RESIZE)();

///////////////////////////////////////////////////////////////////////////
// HDR_ENGINE
/**
  Engine object.
  Contains the processing functionality.
*/
typedef struct
{
    /** Pointer to the function table. */
    struct HDR_ENGINE_VFT* vft;

} HDR_ENGINE;

///////////////////////////////////////////////////////////////////////////
// HDR_ENGINE_VFT
/**
  Engine object interface definition.
  Contains pointers to the object's functions. Please note that these functions
  are not thread safe.
*/
struct HDR_ENGINE_VFT
{
    ///////////////////////////////////////////////////////////////////////////
    // Destroy
    /**
    Destroys the HDR engine object.
    @param[in] eng
      Pointer to this HDR engine object.
    */
    void (GTCALL *Destroy)(
        IN  HDR_ENGINE*  eng
        );

    ///////////////////////////////////////////////////////////////////////////
    // AnalyzeImage
    /**
    Evaluates an image to see how it can be improved by HDR. This function will
    tell what other additional images are required in order to apply the HDR
    algorithm effectively.
    @param[in] eng
      Pointer to this HDR engine object.
    @param[in] img
      Pointer to a @ref GT_IMAGE structure which represents the image to be analyzed.
    @param[out] res
      Pointer to an unsigned integer which will take values from the
      enumeration @ref HDRE_EVALUATION_RESULT. The result of the image
      analysis will be placed at this address.
    @param[out] reserved
      Reserved parameter to be used in the future. Should be NULL in current version.
    @return
      The function will return GTS_OK if the analysis was performed correctly.
      If there is not enough heap memory, the error GTS_NO_SYSTEM_RESOURCES
      will be returned. If the input pointers are incorrect, error
      GTS_INVALID_ARG will be returned.
    */
    GTSTATUS (GTCALL *AnalyzeImage)(
        IN  HDR_ENGINE*  eng,
        IN  const GT_IMAGE* img,
        OUT UINT32 *res,
        OUT UINT32 *reserved 
        );

    ///////////////////////////////////////////////////////////////////////////
    // RegisterImage
    /**
    Registers an image as input to the HDR algorithm. When enough images are registered
    (depending on the selected HDR mode), the HDR result can be retrieved
    by calling @ref HDR_ENGINE_VFT::GetResult.
    @param[in] eng
      Pointer to this HDR engine object.
    @param[in] img
      Pointer to a @ref GT_IMAGE structure which represents the input image.
    @param[in] exp
      Optional pointer to a @ref HDR_EXPOSURE structure indicating the exposure settings
      used when capturing the current input image. This will be used only if a valid
      exposure indication type and camera settings were specified in @ref HDR_CONFIG.
    @return
      This function returns GTS_OK if the processing was successful.
      If an error occurs, it will fail with a relevant general error code.
    @remarks
      - Input Images
        - All input images must have the same size.
        - If the exposure is specified in the <i>exp</i> parameter, it will be used only if
        a valid exposure indication and camera settings were specified in @ref HDR_CONFIG
        when creating the engine. In this case, <i>exp->type</i> must match the type in
        HDR_CONFIG::expIndicationType, and the parameters from <i>exp</i> must be in the
        defined ranges, otherwise a GTS_INVALID_ARG error code will be returned.
      - Image Acquisition
        - For Still HDR, the images must be acquired one after the other sufficiently fast. The maximum
        time interval between two consecutive acquisitions should not exceed 1s.
        - The exposure difference between consecutive input images should be below 4EV.
    */
    GTSTATUS (GTCALL *RegisterImage)(
        IN  HDR_ENGINE*         eng,
        IN  GT_IMAGE*           img,
        IN  const HDR_EXPOSURE* exp     OPTIONAL
        );

    ///////////////////////////////////////////////////////////////////////////
    // GetResult
    /**
    Builds the the tone-mapped HDR image.
    @param[in] eng
      Pointer to this HDR engine object.
    @param[in] effectType
      Constant from the @ref HDRE_SCENE_EFFECT enumeration, describing the desired
      tone mapping effect which gives the aspect of the scene.
    @param[in] effectStrength
      Integer value from 0 to 9, describing the strength of the color vibrance.
      Higher values lead to a more saturated HDR Image. This parameter is normally used in the Surreal Effect.
      For the Natural Effect the recommended value is 2.
    @param[in] opts
      Optional pointer to a @ref HDR_OPTIONS structure with processing options.
      The opts.hdr_flags bitfield should have values from the set HDRE_CONFIG_FLAGS.
      In order to use HDRF_ENABLE_FACE_PRIORITY then the engine must be created with this flag set.
    @param[out] imgRes
      Receives upon successful completion a pointer to the @ref GT_IMAGE structure
      which represents the HDR output image.
    @return
      If successful, this function will return GTS_OK, and <i>imgRes</i> will receive a
      valid pointer to the result image.
      If no images were previously registered with <i>RegisterImage</i>, the function will return
      HDRS_NO_REGISTERED_IMAGES.
      If an error occurs, a relevant general error code will be returned.
    @remarks
      - Still HDR mode
        - The number of images which need to be registered is either 1, or 2, depending on
        the exposure selection.
        - After registering all input images, this function can be called repeatedly any number
        of times in case different tone mapping effect types are desired, until @ref HDR_ENGINE_VFT::RegisterImage
        is called again, thus resetting previous results and starting over with a new image set.
        - After calling this function a subsequent time, the previously returned result image
        pointer must be treated as invalid, and must no longer be accessed.
        - The resulting image will have a smaller size than that of the input images if the
        input images are not aligned (camera moved between captures). In this case, the library
        will align them and crop out the non-overlapping parts.
    */
    GTSTATUS (GTCALL *GetResult)(
        IN  HDR_ENGINE*         eng,
        IN  UINT32              effectType,
        IN  UINT32              effectStrength,
        IN  const HDR_OPTIONS*  opts        OPTIONAL,
        OUT GT_IMAGE**          imgRes
        );

    ///////////////////////////////////////////////////////////////////////////
    // Register external resize functions.
    /**
    Used for registering external resize functions.
    @param[in] eng
    Pointer to this HDR engine object.
    @param[in] initRes
    Application defined function that initializes external resize.
    @param[in] extRes
    Application defined function that resizes the input GT_IMAGE and stores the result in the output GT_IMAGE.
    @param[in] cleanExtRes
    Application defined function that frees external resize resources.
    @remarks
    - This function is useful only when the lib is built with external resize support.
    */
    GTSTATUS(GTCALL *SetExtResizeFunctions)(
        IN  HDR_ENGINE*         eng,
        IN  INIT_EXT_RESIZE     initExtRes,
        IN  EXT_RESIZE          extRes,
        IN  CLEAN_EXT_RESIZE    cleanExtRes
        );
};


/// @} end of api_eng */



///////////////////////////////////////////////////////////////////////////
// Library Interface
/**
@defgroup api_lib Library Interface
  Applications can access the library's functions through the library object.
@{
**************************************************************************/


///////////////////////////////////////////////////////////////////////////
// HDR_LIB_INFO
/**
  Library information structure.
*/
typedef struct
{
    /** The size, in bytes, of the ::HDR_LIB_INFO structure. The field is set
      by the library. The application may check this field to decide whether it can
      access the fields added later in API. */
    UINT32        structSize;

    /** Library version, represented by an array of numbers.
      The first four numbers in the array represent the main version numbers,
      from the major version number (first number in the array, element [0])
      to the minor version (fourth number, element [3]).
      The last number (element [4]) is an optional revision number. */
    UINT32        libVer[5];

    /** API version, represented as a 32-bit value, where the higher 16 bits
      represent the major version and the lower 16 bits represent the minor version.
      This is the version of library API, and does not change when there are only
      changes in implementation. The API version changes when the API is modified,
      so an application might query whether newly added features are available.
      For the current list of API versions see @ref api_ver. */
    UINT32        apiVer;

    /** Optional string specifying build date in form "MMM DD YYYY HH:MM:SS". */
    const char*   buildDate;

    /** Optional release label. */
    const char*   label;

    /** An array of @ref api_supfmts "supported color formats". The array is
      terminated with a zero item (GT_FMT_UNKNOWN). */
    const UINT32* supportedFmts;

} HDR_LIB_INFO;


///////////////////////////////////////////////////////////////////////////
// HDRE_MODE
/**
  These constants indicate the HDR engine processing mode.
*/
enum HDRE_MODE
{
    /** Takes at input a sequence of still images, with exposures from lower to higher,
      and generates a still HDR image. */
    HDRE_STILL = 1,
    /** Takes at input a sequence of still images, with exposures from lower to higher,
    and generates a still HDR image using a different overlay technique than the one used in HDRE_STILL. */
    HDRE_STILL_FAST_OVERLAY = 2,
};


///////////////////////////////////////////////////////////////////////////
// HDR_INTERVAL
/**
  Defines an interval by its lower and upper bounds.
*/
typedef struct
{
    INT32 lo;       ///< Interval lower bound.
    INT32 hi;       ///< Interval upper bound.
} HDR_INTERVAL;

///////////////////////////////////////////////////////////////////////////
// HDR_CONFIG
/**
  HDR engine configuration structure.
*/
typedef struct
{
    /** The size, in bytes, of the HDR_CONFIG structure. Set this member
      to <code>sizeof(HDR_CONFIG)</code>. Be sure to set this member before calling
      any function that uses this structure. */
    UINT32  structSize;

    /** Processing mode in which the engine will run, described by one of the
      @ref HDRE_MODE constants. */
    UINT32  mode;

    /** Color format of the input images, indicated by one of the
      @ref api_supfmts "supported format" constants. */
    UINT32  imgFormat;

    /** Width in pixels of the input images. */
    UINT32  imgWidth;

    /** Height in pixels of the input images. */
    UINT32  imgHeight;

    /** One of the @ref HDRE_EXPOSURE_INDICATION constants which indicates how the
      system can provide information about exposure. If it is set to <i>HDRE_EXP_CAM_SETTINGS</i>,
      then the <i>exposure.params</i> representation will be used for exposure. If it is set to
      <i>HDRE_EXP_VALUE</i>, then <i>exposure.evRange</i> will be used.
      If this is set to a valid value, then the <i>exp</i> parameter must be set to the camera
      settings, and afterwards the @ref HDR_EXPOSURE sent to @ref HDR_ENGINE_VFT::RegisterImage will be
      taken into account if specified. */
    UINT32  expIndicationType       OPTIONAL;

    /** Exposure parameters' ranges.
      These parameters must be specified according to the representation given in
      <i>expIndicationType</i>. If they are not specified, the library will use default ranges for
      the parameters of the selected representation. */
    struct {
        /** Value ranges for aperture, exposure time and ISO settings.
          This group of parameters is used if expIndicationType is set to <i>HDRE_EXP_CAM_SETTINGS<i>. */
        struct {
            /** Array with the aperture values supported by the camera, ordered from the lowest
              number of stops to the highest. Aperture is expressed in 1/10 units.
              If this is set to NULL, the aperture values are considered from f/2.0 to f/8.0 in
              increments of 0.1 stops. */
            INT32*       apertureValues     OPTIONAL;

            /** Number of elements in the @ref apertureValues array. If this is 0, then the
              <i>apertureValues</i> array is not used. */
            UINT32       countAps           OPTIONAL;

            /** The exposure time range supported by the camera. Exposure time is expressed in microseconds.
              If this is not defined (interval limits are 0), all the exposure times from 1/4000s to 1/2s in
              microsecond increments are considered. */
            HDR_INTERVAL expTimeRange       OPTIONAL;

            /** The ISO value range supported by the camera. ISO values are expressed on the arithmetic scale.
              If this is not defined (interval limits are 0), the ISO values are considered from 20 to 3600 in
              unit increments. */
            HDR_INTERVAL isoRange           OPTIONAL;
        } paramRanges;

        /** Value range for the camera's EV metering.
          This parameter is used if @ref HDR_CONFIG::expIndicationType is set to <i>HDRE_EXP_VALUE</i>.
          If this is not defined (interval limits are 0), all the values from -4EV to +16EV in
          0.1 increments are considered. */
        HDR_INTERVAL evRange        OPTIONAL;
    } exp;

    /** Flags from the set of HDRE_CONFIG_FLAGS. Please note that in order to use face priority flag in GetResult then the HDR engine must be created with this flag set. */
    UINT32 cfg_flags;

} HDR_CONFIG;


///////////////////////////////////////////////////////////////////////////
// HDR_LIB
/**
  Library object.
*/
typedef struct
{
    /** Pointer to the function table. */
    struct HDR_LIB_VFT* vft;

} HDR_LIB;

///////////////////////////////////////////////////////////////////////////
// HDR_LIB
/**
  Library object interface definition.
*/
struct HDR_LIB_VFT
{
    ///////////////////////////////////////////////////////////////////////////
    // GetLibInfo
    /**
    Retrieves versioning information about the library.
    @param[in] lib
      Pointer to this library object.
    @return
      Pointer to an HDR_LIB_INFO structure with library version information.
    */
    const HDR_LIB_INFO* (GTCALL *GetLibInfo)(
        IN  HDR_LIB* lib
        );

    ///////////////////////////////////////////////////////////////////////////
    // GetRequiredHeapSize
    /**
    Calculates with a very close approximation the total amount of memory which
    the library will require for all its internal allocations.
    @param[in] lib
      Pointer to this library object.
    @param[in] cfg
      Pointer to a @ref HDR_CONFIG structure describing the way the engine will be used.
      The optional parameters describing exposure indication are not used.
    @return
      The approximate amount of heap memory, expressed in bytes, required by the library.
      The amount returned will never be less than the total number of bytes the library
      could need to allocate, but it will always be close to it, so that very little
      memory might be wasted.
      If an invalid arguments is given, the function will return 0xFFFFFFFF.
    */
    UINT32 (GTCALL *GetRequiredHeapSize)(
        IN  HDR_LIB*          lib,
        IN  const HDR_CONFIG* cfg
        );

    ///////////////////////////////////////////////////////////////////////////
    // CreateHeap
    /**
    Creates a heap object, managed by the library, and backed by the specified
    memory buffer.
    @param[in] lib
      Pointer to this library object.
    @param[in] buf
      Pointer to the buffer which will back the heap.
    @param[in] size
      Size in bytes of the user buffer.
    @param[out] heap
      Pointer to the created heap object.
    @return
      Returns GTS_OK if successful, and an error code otherwise.
    */
    GTSTATUS (GTCALL *CreateHeap)(
        IN  HDR_LIB*  lib,
        IN  void*     buf,
        IN  UINT32    size,
        OUT GT_HEAP** heap
        );

    ///////////////////////////////////////////////////////////////////////////
    // CreateHdrEngine
    /**
    Creates a HDR engine object, which contains the HDR processing functions.
    @param[in] lib
      Pointer to this library object.
    @param[in] heap
      Pointer to a @ref GT_HEAP object which will be used for memory requests.
    @param[in] cfg
      Pointer to a @ref HDR_CONFIG structure with configuration parameters.
    @param[out] eng
      Points to the created HDR engine object.
    @return
      Returns GTS_OK if successful, or an error code otherwise.
    @remarks
      - If the calling application specifies an application-defined heap object
      in the <i>heap</i> paramater, the HDR engine will not own that heap object.
      This means that it won't destroy the heap by calling @ref GT_HEAP_VFT::Destroy
      on it when the HDR engine itself is destroyed, the calling application is
      responsible for destroying the heap object it created.
    */
    GTSTATUS (GTCALL *CreateHdrEngine)(
        IN  HDR_LIB*          lib,
        IN  GT_HEAP*          heap,
        IN  const HDR_CONFIG* cfg,
        OUT HDR_ENGINE**      eng
        );
};


///////////////////////////////////////////////////////////////////////////
// HdrLibGet
/**
Retrieves a pointer to the library object.
@return
  Pointer to a HDR_LIB structure which represents the library object,
  through which the library functions are accessible.
*/
GTAPI HDR_LIB* GTCALL HdrLibGet( void );


/// @} end of api_lib



#ifdef __cplusplus
}
#endif


#endif // __HDR_H__

