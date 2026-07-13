//------------------------------------------------------------------------------
// File: config.h
//
// Copyright (c) 2006, Chips & Media.  All rights reserved.
// This file should be modified by some developers of C&M according to product version.
//------------------------------------------------------------------------------
#ifndef __CONFIG_H__
#define __CONFIG_H__

#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WIN32) || defined(__MINGW32__)
#	define PLATFORM_WIN32
#elif defined(linux) || defined(__linux) || defined(ANDROID)
#	define PLATFORM_LINUX
#elif defined(unix) || defined(__unix)
#   define PLATFORM_QNX
#else
#	define PLATFORM_NON_OS
#endif

#if defined(_MSC_VER)
#	include <windows.h>
#	define inline _inline
#elif defined(__GNUC__)
#elif defined(__ARMCC__)
#else
#  error "Unknown compiler."
#endif

#define API_VERSION_MAJOR       5
#define API_VERSION_MINOR       5
#define API_VERSION_PATCH       46
#define API_VERSION             ((API_VERSION_MAJOR<<16) | (API_VERSION_MINOR<<8) | API_VERSION_PATCH)

#if defined(PLATFORM_NON_OS) || defined (ANDROID) || defined(MFHMFT_EXPORTS) || defined(PLATFORM_QNX)
//#define SUPPORT_FFMPEG_DEMUX
#else
#define SUPPORT_FFMPEG_DEMUX
#endif



#define SOCIOCEZ_107_SERIAL_NUM
#define SOCIOCEZ_66_MORE_GOPPRESET
#define SOCIOCEZ_64_DDR_CH_SELECTION
#define SUPPORT_NAL_SIZE_REPORT
#define SOCIOCEZ_86_CORRECT_PICTYPE
#define SOCIOCEZ_90_BUG_FIX
#define SOCIOCE_91_PARAMETER_CHANGE
#define SOCIOCEZ_98_SNI_RING_BUFFER
#define SOCIOCEZ_98_SNI_RING_BUFFER_MULTI_INST
#define SOCIOCEZ_100_SLEEP_WAKEUP
#define SOCIOCEZ_103_BUG_FIX

//------------------------------------------------------------------------------
// COMMON
//------------------------------------------------------------------------------


// do not define BIT_CODE_FILE_PATH in case of multiple product support. because wave410 and coda980 has different firmware binary format.
#define CORE_0_BIT_CODE_FILE_PATH   "cezzane.bin"     // for wave420
#define CORE_1_BIT_CODE_FILE_PATH   "coda980.out"     // for coda980
#define CORE_2_BIT_CODE_FILE_PATH   "pissarro.bin"    // for wave412
#define CORE_3_BIT_CODE_FILE_PATH   "michelangel.bin" // for wave410
#define CORE_4_BIT_CODE_FILE_PATH   "coda960.out"     // for coda960
#define CORE_5_BIT_CODE_FILE_PATH   "monet.bin"       // for wave420L
#define CORE_6_BIT_CODE_FILE_PATH   "mondrian.bin"    // for wave510
#define CORE_7_BIT_CODE_FILE_PATH   "picasso.bin"    // for wave510

//------------------------------------------------------------------------------
// OMX
//------------------------------------------------------------------------------





//------------------------------------------------------------------------------
// WAVE420
//------------------------------------------------------------------------------
#define PIXEL_ORDER
#define SUPPORT_PACKED_STREAM_FORMAT
#define SUPPORT_WAVE4ENC_SLICE_INTERRUPT

#define SUPPORT_W4ENC_STILLPICTURE_PROFILE
#undef SUPPORT_CROPPED_ENCODING
#undef SUPPORT_W4ENC_RC_REPORT
#undef SUPPORT_W4ENC_RC_REPORT_QP_HISTOGRAM // for Dahua




#endif /* __CONFIG_H__ */
