/**
 * @file		ip_shdr.h
 * @brief		SHDR wrapper
 * @author		yamauchi
 * @since		2016/07/25
 * @date		2016/07/25
 * @note		SHDR wrapper
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; Socionext Inc. 2016</I></B>
 */

#ifndef _IP_SHDR_H_
#define _IP_SHDR_H_

#include "ddim_typedef.h"

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/

/**
   @name Returned value
   @{
*/
#define D_IP_SHDR_OK				(0)					/**< OK */
#define D_IP_SHDR_NG				(0x801A0000)		/**< NG */
#define D_IP_SHDR_NG_ARG			(D_IP_SHDR_NG|0x00000001)	/**< Argument  Error		*/
#define D_IP_SHDR_NG_OPEN			(D_IP_SHDR_NG|0x00000002)	/**< Open      Error		*/
#define D_IP_SHDR_NG_SEQUENCE		(D_IP_SHDR_NG|0x00000004)	/**< Sequence  Error		*/
#define D_IP_SHDR_NG_FORMAT			(D_IP_SHDR_NG|0x00000008)	/**< Format    Error		*/
#define D_IP_SHDR_NG_SIZE			(D_IP_SHDR_NG|0x00000010)	/**< Size      Error		*/
#define D_IP_SHDR_NG_PARAM			(D_IP_SHDR_NG|0x00000020)	/**< Parameter Error		*/
#define D_IP_SHDR_NG_MEMORY			(D_IP_SHDR_NG|0x00000040)	/**< Memory    Error		*/
#define D_IP_SHDR_NG_HW				(D_IP_SHDR_NG|0x00000080)	/**< HW        Error		*/
#define D_IP_SHDR_NG_UNKNOWN		(D_IP_SHDR_NG|0x00008000)	/**< Unknown   Error		*/

#define  D_IP_SHDR_NG_FAIL                  (D_IP_SHDR_NG|0x00002001)  /**< Operation was cancelled. */
#define  D_IP_SHDR_NG_CANCELLED             (D_IP_SHDR_NG|0x00002002)  /**< Operation was cancelled. */
#define  D_IP_SHDR_NG_NO_SYSTEM_RESOURCES   (D_IP_SHDR_NG|0x00002003)  /**< Not enough system resources. */
#define  D_IP_SHDR_NG_INVALID_ARG           (D_IP_SHDR_NG|0x00002004)  /**< One of the arguments passed is invalid. */
#define  D_IP_SHDR_NG_NOT_SUPPORTED         (D_IP_SHDR_NG|0x00002006)  /**< Feature is not supported. */
#define  D_IP_SHDR_NG_FMT_NOT_SUPPORTED     (D_IP_SHDR_NG|0x00002007)  /**< Image format not supported. */
#define  D_IP_SHDR_NG_NOT_IMPLEMENTED       (D_IP_SHDR_NG|0x00002008)  /**< Feature is not implemented. */
#define  D_IP_SHDR_NG_BAD_STRUCT_SIZE       (D_IP_SHDR_NG|0x00002009)  /**< Invalid structure size. */
#define  D_IP_SHDR_NG_BAD_IMAGE_SIZE        (D_IP_SHDR_NG|0x0000200A)  /**< The size of the input image is not valid. */

#define D_IP_SHDR_INVALID_EXP_SETTING   (D_IP_SHDR_NG|0x100)		
#define D_IP_SHDR_NOT_AVAILABLE  		(D_IP_SHDR_NG|0x101)		
#define D_IP_SHDR_NO_REGISTERED_IMAGES  (D_IP_SHDR_NG|0x102)		
#define D_IP_SHDR_INVALID_IMAGE_CONTENT (D_IP_SHDR_NG|0x103)		


/** @} */	// name

/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/
enum	E_IP_SHDR_STATE {
	E_IP_SHDR_SEQ_IDLE = 0,
	E_IP_SHDR_SEQ_OPEN = 1,
	E_IP_SHDR_SEQ_INIT = 2,
	E_IP_SHDR_SEQ_CTRL = 3,
	E_IP_SHDR_SEQ_PROCESS = 4,
	E_IP_SHDR_SEQ_STICH = 5,
};

enum	E_IP_SHDR_FORMAT {
	E_IP_SHDR_FMT_YUV_444    = 0x10000001,
	E_IP_SHDR_FMT_YUV_444_P2 = 0x10100001,
	E_IP_SHDR_FMT_YUV_444_P3 = 0x10200001,
	E_IP_SHDR_FMT_YUV_422_P2 = 0x20100001,
	E_IP_SHDR_FMT_YUV_422_P3 = 0x20200001,
	E_IP_SHDR_FMT_YUV_420_P2 = 0x30100001,
	E_IP_SHDR_FMT_YUV_420_P3 = 0x30200001
};

enum E_IP_SHDR_EXPOSURE_INDICATION
{
    E_IP_SHDR_EXP_UNUSED       = 0,  ///< Exposure descriptions are not used.
    E_IP_SHDR_EXP_CAM_SETTINGS = 1,  ///< Exposure is described by providing the values for aperture, exposure time and ISO speed.
    E_IP_SHDR_EXP_VALUE        = 2   ///< Exposure is described by providing directly the value in EV.
};

enum E_IP_SHDR_PROGRESS_REASON
{
    E_IP_SHDR_PROGRESS_BEGIN = 0,     ///< Start of operation. @hideinitializer
    E_IP_SHDR_PROGRESS_UPDATE,        ///< Operation step completed. @hideinitializer
    E_IP_SHDR_PROGRESS_END,           ///< End of operation. @hideinitializer
};  

enum E_IP_SHDR_CONFIG_FLAGS
{
	E_IP_SHDR_ENABLE_FACE_PRIORITY        = 1,  ///< the algorithm will try to minimize artefacts on faces
	E_IP_SHDR_ENABLE_CONTRAST_ENHANCEMENT = 2,  ///< apply an extra contrast enhancement process on the final image
};

enum E_IP_SHDR_SCENE_EFFECT
{
    E_IP_SHDR_EFF_NATURAL = 0,       ///< Normal tone mapping, the scene will look natural.
    E_IP_SHDR_EFF_SURREAL = 1,       ///< Applies an artistic effect to the scene, giving it a surreal look.
};

/**
  These constants are the possible results of the image analysis performed by function AnalyzeImage()
*/
enum E_IP_SHDR_EVALUATION_RESULT
{
    E_IP_SHDR_EVAL_NO_HDR             = 1,       ///< the image is well exposed and does not need HDR
    E_IP_SHDR_EVAL_USE_DARK_AND_REF   = 2,       ///< the image contains overexposed areas; beside this image, an underexposed image is needed for generating HDR
    E_IP_SHDR_EVAL_USE_REF_AND_LIGHT  = 3,       ///< the image contains underexposed areas; beside this image, an overexposed image is needed for generating HDR
    E_IP_SHDR_EVAL_USE_DARK_AND_LIGHT = 4,       ///< the image contains both underexposed and overexposed areas; other overexposed and underexposed images are needed for generating HDR
};

enum E_IP_SHDR_MODE
{
    /** Takes at input a sequence of still images, with exposures from lower to higher,
      and generates a still HDR image. */
    E_IP_SHDR_STILL = 1,
    E_IP_SHDR_STILL_FAST_OVERLAY = 2,
};

/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/

typedef struct
{
    INT32	stride;
    VOID*	data;
} T_IP_SHDR_PLANE;


typedef struct 
{
    UINT32   format;
    UINT32   width;
    UINT32   height;
    T_IP_SHDR_PLANE	planes[4];
} T_IP_SHDR_IMAGE;

typedef struct
{
    UINT32  type;
    struct {
        INT32   exposureTime;
        INT32   aperture;
        INT32   iso;
    } p;
    INT32   ev;
} T_IP_SHDR_EXPOSURE;

typedef struct
{
    UINT32          structSize;
    void* progressCallback;
    void*           progressContext;
    UINT32  hdr_flags;
} T_IP_SHDR_OPTIONS;

typedef struct
{
    UINT32        structSize;
    UINT32        libVer[5];
    UINT32        apiVer;
    const char*   buildDate;
    const char*   label;
    const UINT32* supportedFmts;
} T_IP_SHDR_LIB_INFO;

typedef struct
{
    INT32 lo;       ///< Interval lower bound.
    INT32 hi;       ///< Interval upper bound.
} T_IP_SHDR_INTERVAL;

typedef struct {
    INT32*              apertureValues;
    UINT32              countAps;
    T_IP_SHDR_INTERVAL  expTimeRange;
    T_IP_SHDR_INTERVAL  isoRange;
} T_IP_SHDR_RANGE;

typedef struct
{
    UINT32  reserve;
    UINT32  mode;
    UINT32  imgFormat;
    UINT32  imgWidth;
    UINT32  imgHeight;
    UINT32  expIndicationType;
    struct {
        T_IP_SHDR_RANGE     paramRanges;
        T_IP_SHDR_INTERVAL  evRange;
    } exp;
    UINT32  cfg_flags;
    VOID    *openResize;
    VOID    *runResize;
    VOID    *closeResize;
} T_IP_SHDR_CONFIG;

/*--------------------------------------------------------------------------------------*/
/* Global Data																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Macro																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Function  																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* API		  																			*/
/*--------------------------------------------------------------------------------------*/

extern	INT32	IP_SHDR_Open( VOID *heapBuffer, T_IP_SHDR_CONFIG *config);
extern	INT32	IP_SHDR_Close( VOID );
extern	UINT32	IP_SHDR_GetBufferSize( T_IP_SHDR_CONFIG *config );
extern	INT32	IP_SHDR_AnalyzeImage( T_IP_SHDR_IMAGE *image, UINT32 *results );
extern	INT32	IP_SHDR_Process( T_IP_SHDR_IMAGE *imageDark, T_IP_SHDR_IMAGE *imageLight, T_IP_SHDR_OPTIONS *opt, T_IP_SHDR_IMAGE **imageResult );

extern	VOID IP_SHDR_Get_Version(CHAR** str);

/*--------------------------------------------------------------------------------------*/
/* Version  																			*/
/*--------------------------------------------------------------------------------------*/

#endif	// _IP_SHDR_H_

/* V00L00 2016.07.25 IP_SHDR ver0.0 */

