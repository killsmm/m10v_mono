/***************************************************************************
 *  MPEG-4/2 AAC LC Decoder Library (2ch)                                  *
 *  Copyright 2015 Socionext Inc.                                          *
 *  LICENSED MATERIAL - PROGRAM PROPERTY OF SOCIONEXT INC.                 *
 *  file name : aacdecoder.h                                               *
 ***************************************************************************/
#ifndef AACDECODER_H
#define AACDECODER_H


/****************************************************************************
*
*  Variable Type
*
****************************************************************************/

typedef unsigned char AACD_BOOL;


/****************************************************************************
*
*  Structure
*
****************************************************************************/
/* for output stream */
typedef struct _AACD_OutputDataHolder
{
    unsigned long *center;
    unsigned long *left;
    unsigned long *right;
    unsigned long *leftBack;
    unsigned long *rightBack;
    unsigned long *lowFrequencyEnhancement;
} AACD_OutputDataHolder;


/* for API AACD_getHeaderData */
typedef struct _AACD_HeaderProgConfig
{
    unsigned char  ele_inst_tag;
    unsigned char  profile;
    unsigned char  sampling_freq_idx;
    unsigned char  num_front_ch_eles;
    unsigned char  num_side_ch_eles;
    unsigned char  num_back_ch_eles;
    unsigned char  num_lfe_ch_eles;
    unsigned char  num_assoc_data_eles;
    unsigned char  num_valid_cc_eles;
    unsigned char  mono_mixdown_present;
    unsigned char  mono_mixdown_ele_num;
    unsigned char  stereo_mixdown_present;
    unsigned char  stereo_mixdown_ele_num;
    unsigned char  matrix_mixdown_idx_present;
    unsigned char  matrix_mixdown_idx;
    unsigned char  pseudo_surround_enable;
    unsigned char  front_ele_is_cpe[16];
    unsigned char  front_ele_tag_select[16];
    unsigned char  side_ele_is_cpe[16];
    unsigned char  side_ele_tag_select[15];
    unsigned char  back_ele_is_cpe[16];
    unsigned char  back_ele_tag_select[16];
    unsigned char  lfe_ele_tag_select[4];
    unsigned char  assoc_data_ele_tag_select[8];
    unsigned char  cc_ele_is_ind_sw[16];
    unsigned char  valid_cc_ele_tag_select[16];
    unsigned char  comment_bytes;
    unsigned char  comment_data[256];
}AACD_HeaderProgConfig;

typedef struct _AACD_HeaderDataHolder
{
    unsigned char  headertype;
/* ADTS */
    unsigned char  ADTS_id;
    unsigned char  ADTS_layer;
    unsigned char  ADTS_protection_absent;
    unsigned char  ADTS_profile;
    unsigned char  ADTS_sampling_frequency_index;
    unsigned char  ADTS_private_bit;
    unsigned char  ADTS_channel_config;
    unsigned char  ADTS_original_copy;
    unsigned char  ADTS_home;
    unsigned char  ADTS_Emphasis;
/* ADTS variable header */
    unsigned char  ADTS_copyright_id_bit;
    unsigned char  ADTS_copyright_id_start;
    unsigned short ADTS_frame_length;
    unsigned short ADTS_buffer_fullness;
    unsigned char  ADTS_number_of_rawdata_blocks;
/* ADIF */
    unsigned char  common_copyright_id[10];
    unsigned char  ADIF_copyright_id_present;
    unsigned char  ADIF_original_copy;
    unsigned char  ADIF_home;
    unsigned char  ADIF_bitstream_type;
    unsigned long  ADIF_bitrate;
    unsigned char  ADIF_num_PCE;
    unsigned long  ADIF_buffer_fullness;
/* ASC */
    unsigned char  ASC_ObjectType;
    unsigned char  ASC_sampling_frequency_index;
    unsigned long  ASC_sampling_frequency;
    unsigned char  ASC_channel_config;
    unsigned char  GASC_frameLengthFlag;
    unsigned char  GASC_dependsOnCoreCoder;
    unsigned char  GASC_extensionFlag;

/* PCE */    
    AACD_HeaderProgConfig pceData;

} AACD_HeaderHolder;

/* for get/set MemorySize */
typedef struct _AACD_MemorySize
{
    unsigned long  ContextMemSize;
    unsigned long  IntMem1Size;
    unsigned long  IntMem2Size;
} AACD_MemorySize;

/* for set MemoryAdrs */
typedef struct _AACD_MemoryAdrs
{
    unsigned long *ContextMem;
    unsigned long *IntMem1;
    unsigned long *IntMem2;
} AACD_MemoryAdrs;

/****************************************************************************
*
*  Enumeration
*
****************************************************************************/
typedef enum _AACD_SamplingRate
{
    AACD_SAMPLING_RATE_96000Hz = 0x0,
    AACD_SAMPLING_RATE_88200Hz = 0x1,
    AACD_SAMPLING_RATE_64000Hz = 0x2,
    AACD_SAMPLING_RATE_48000Hz = 0x3,
    AACD_SAMPLING_RATE_44100Hz = 0x4,
    AACD_SAMPLING_RATE_32000Hz = 0x5,
    AACD_SAMPLING_RATE_24000Hz = 0x6,
    AACD_SAMPLING_RATE_22050Hz = 0x7,
    AACD_SAMPLING_RATE_16000Hz = 0x8,
    AACD_SAMPLING_RATE_12000Hz = 0x9,
    AACD_SAMPLING_RATE_11025Hz = 0xa,
    AACD_SAMPLING_RATE_8000Hz = 0xb,
    AACD_SAMPLING_RATE_7350Hz = 0xc,
    AACD_SAMPLING_RATE_OTHERS = 0xFF
} AACD_SamplingRate;


/* | 0 | Dual Mono | Center | Left | Right | Left Back | Right Back | LFE | */
typedef enum _AACD_ChannelStatus
{
    AACD_DUAL_MONO_ON          = 0x40,
    AACD_CHANNEL_CENTER_ON     = 0x20,
    AACD_CHANNEL_LEFT_ON       = 0x10,
    AACD_CHANNEL_RIGHT_ON      = 0x08,
    AACD_CHANNEL_LEFT_BACK_ON  = 0x04,
    AACD_CHANNEL_RIGHT_BACK_ON = 0x02,
    AACD_CHANNEL_LFE_ON        = 0x01
} AACD_ChannelStatus;


typedef enum _AACD_AudioSpecificConfig
{
    AACD_ASC_SUCCESS,
    AACD_ASC_AUDIO_OBJECT_TYPE_NOT_SUPPORTED,
    AACD_ASC_SAMPLING_RATE_NOT_SUPPORTED,
    AACD_ASC_CHANNEL_CONFIGURATION_NOT_SUPPORTED,
    AACD_ASC_FRAME_LENGTH_NOT_SUPPORTED,
    AACD_ASC_CORE_CODER_NOT_SUPPORTED,
    AACD_ASC_EXTENSION_FLAG_NOT_SUPPORTED,
    AACD_ASC_PCE_INVALID_CONFIGURATION,
    AACD_ASC_INVALID_ARGUMENT,
    AACD_ASC_PCE_PROFILE_NOT_SUPPORTED,
    AACD_ASC_PCE_SAMPLING_NOT_SUPPORTED,
    AACD_ASC_PCE_INVALID_OTHERS

} AACD_AudioSpecificConfig;


typedef enum _AACD_BytesPerSample
{
    AACD_BYTES_PER_SAMPLE_4,
    AACD_BYTES_PER_SAMPLE_2
} AACD_BytesPerSample;


typedef enum _AACD_LROutputFormat
{
    AACD_LR_SEPARATED,
    AACD_LR_INTERLEAVED,
    AACD_RL_INTERLEAVED,
    AACD_LL_INTERLEAVED,
    AACD_RR_INTERLEAVED
} AACD_LROutputFormat;


typedef enum _AACD_CenterDuplicated
{
    AACD_CENTER_NOT_DUPLICATED,
    AACD_CENTER_DUPLICATED
} AACD_CenterDuplicated;


typedef enum _AACD_BoolType
{
    AACD_TRUE  = 1,
    AACD_FALSE = 0
} AACD_BoolType;


typedef enum _AACD_FormatInfo
{
    AACD_FI_SUCCESS = 0x0,
    AACD_FI_FAILURE = 0x1,
    AACD_FI_INVALID_ARGUMENT = 0x3
} AACD_FormatInfo;


typedef enum _AACD_BitRateType
{
    AACD_BITRATE_VARIABLE = 0x00000000,
    AACD_BITRATE_CONSTANT = 0x00000001,
    AACD_BITRATE_UNKNOWN  = (int)0xFFFFFFFF
} AACD_BitRateType;


typedef enum _AACD_HeaderSearch
{
  AACD_SH_SUCCESS,
  AACD_SH_SUCCESS_INBUF_END,
  AACD_SH_FAILURE,
  AACD_SH_INVALID_ARGUMENT
} AACD_HeaderSearch;


typedef enum _AACD_ProcessMode
{
    AACD_NORMAL_MODE = 0,
    AACD_HIGH_MODE = 1
}AACD_ProcessMode;


typedef enum _AACD_retMemSize
{
    AACD_GMS_SUCCESS = 0,
    AACD_GMS_INVALID_ARGUMENT = -1,
    AACD_GMS_NOT_SUPPORTED = -2
}AACD_retMemSize;


typedef enum _AACD_retMemInit
{
    AACD_INT_SUCCESS = 0,
    AACD_INT_INVALID_ARGUMENT = -1,
    AACD_INT_NOT_SUPPORTED = -2,
    AACD_INT_MEMORY_ERR = -3
}AACD_retMemInit;

/****************************************************************************
*
*  Function Prototype
*
****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

extern unsigned long AACD_getVersion(void);


extern unsigned long AACD_getContextSize(void);


extern void AACD_initialize(
    unsigned long *context);


extern AACD_AudioSpecificConfig AACD_setAudioSpecificConfig(
    unsigned long *context,
    unsigned char *audioSpecificConfig,
    unsigned long audioSpecificConfigValidBytes);


extern unsigned long AACD_decode(
    unsigned long *context,
    unsigned char *inputStream,
    unsigned long *inputStreamSize,
    unsigned long *decodedDataSize,
    AACD_OutputDataHolder *decodedData);


extern unsigned long AACD_getRawDataBlockMaxBits(
    unsigned long *context);


extern AACD_SamplingRate AACD_getSamplingRate(
    unsigned long *context);


extern signed long AACD_getBitRate(
    unsigned long *context);


extern AACD_BoolType AACD_calcBitRate(
    unsigned long *context,
    AACD_BitRateType *bitRateType,
    unsigned long *bitRate);



extern unsigned char AACD_getChannelStatus(
    unsigned long *context);


extern unsigned char* AACD_getCopyrightID(
    unsigned long *context);


extern unsigned char AACD_getOriginalCopy(
    unsigned long *context);

extern AACD_FormatInfo AACD_getFormatInfo(
    unsigned long *context,
    AACD_HeaderHolder *HeaderData);

extern void AACD_setDownMix(
    unsigned long *context,
    AACD_BOOL validate);


extern void AACD_setBytesPerSample(
    unsigned long *context,
    AACD_BytesPerSample bytesPerSample);


extern void AACD_setLROutputFormat(
    unsigned long *context,
    AACD_LROutputFormat lrOutputFormat);


extern void AACD_setCenterDuplicated(
    unsigned long *context,
    AACD_CenterDuplicated centerDuplicated);

extern AACD_HeaderSearch
AACD_searchHeader (unsigned long *context,
                   unsigned char **inputStream,
                   unsigned long *inputStreamSize,
                   unsigned long *invalidBytes,
                   AACD_HeaderHolder *es1);

extern AACD_retMemSize AACD_getMemorySize(
    AACD_ProcessMode *procMode,
    AACD_MemorySize *memorySize);

extern AACD_retMemInit AACD_initializeDec(
    AACD_ProcessMode *procMode,
    AACD_MemoryAdrs *memoryAdrs,
    AACD_MemorySize *memorySize);

#ifdef __cplusplus
}
#endif

#endif  /* AACDECODER_H */
