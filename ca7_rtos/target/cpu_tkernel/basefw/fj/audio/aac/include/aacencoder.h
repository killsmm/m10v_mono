/****************************************************************************
 *    MPEG-4/2 AAC-LC Encoder Library                                       *
 *    Copyright 2015 Socionext Inc.                                         *
 *    LICENSED MATERIAL - PROGRAM PROPERTY OF SOCIONEXT INC.                *
 *    file name : aacencoder.h                                              *
 ****************************************************************************/
#ifndef _aacencoder_h_
#define _aacencoder_h_
/****************************************************************************
*
*  Structure
*
****************************************************************************/
#define AACE_LC_SAMPLE_NUMBER    1024

typedef struct _AACE_InputDataHolder
{
    signed short *center;
    signed short *left;
    signed short *right;
} AACE_InputDataHolder;

/****************************************************************************
*
*  Enumeration
*
****************************************************************************/
typedef enum _AACE_SamplingRate
{
    AACE_SAMPLING_RATE_48000Hz = 0x0,
    AACE_SAMPLING_RATE_44100Hz = 0x1,
    AACE_SAMPLING_RATE_32000Hz = 0x2,
    AACE_SAMPLING_RATE_24000Hz = 0x3,
    AACE_SAMPLING_RATE_22050Hz = 0x4,
    AACE_SAMPLING_RATE_16000Hz = 0x5,
    AACE_SAMPLING_RATE_12000Hz = 0x6,
    AACE_SAMPLING_RATE_11025Hz = 0x7,
    AACE_SAMPLING_RATE_8000Hz  = 0x8
} AACE_SamplingRate; 

typedef enum _AACE_ChannelStatus
{
    AACE_BILINGUAL_MODE        = 0,
    AACE_MONAURAL_MODE         = 1,
    AACE_STEREO_MODE           = 2
} AACE_ChannelStatus;

typedef enum _AACE_ChInfo
{
    SAMPLING_RATE_INFO         = 0,
    CHANNEL_INFO               = 1,
    BITRATE_INFO               = 2
} AACE_ChInfo;

/****************************************************************************
*
*  AAC Eecoder workRAM
*
****************************************************************************/
/* for get/set MemorySize */
typedef struct _AACE_MemorySize
{
    unsigned long  ContextMemSize;
    unsigned long  IntMem1Size;
    unsigned long  IntMem2Size;
} AACE_MemorySize;

/* for set MemoryAdrs */
typedef struct _AACE_MemoryAdrs
{
    unsigned long *ContextMem;
    unsigned long *IntMem1;
    unsigned long *IntMem2;
} AACE_MemoryAdrs;

typedef enum _AACE_ProcessMode
{
    AACE_NORMAL_MODE = 0,
    AACE_HIGH_MODE = 1
}AACE_ProcessMode;

typedef enum _AACE_retMemSize
{
    AACE_GMS_SUCCESS = 0,
    AACE_GMS_INVALID_ARGUMENT = -1,
    AACE_GMS_NOT_SUPPORTED = -2
}AACE_retMemSize;

typedef enum _AACE_retMemInit
{
    AACE_INT_SUCCESS = 0,
    AACE_INT_INVALID_ARGUMENT = -1,
    AACE_INT_NOT_SUPPORTED = -2,
    AACE_INT_MEMORY_ERR = -3
}AACE_retMemInit;

/****************************************************************************
*
*  Function Prototype
*
****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

extern AACE_retMemSize AACEncGetContextSize(
    AACE_ProcessMode *procMode,
    AACE_MemorySize  *memorySize);


extern unsigned long AACEncEncode(
    unsigned long *context,
    AACE_InputDataHolder *inputStream,
    unsigned long *encodedDataSize,
    unsigned char **encodedData);


extern unsigned long AACEncGetAudioSpecificConfig(
    unsigned long *context,
    unsigned long *audioSpecificConfigBitSize,
    unsigned char **audioSpecificConfig);


extern unsigned long AACEncGetRawDataBlock(
    unsigned long *context,
    unsigned long *bytes);


extern AACE_retMemInit AACEncSetConfig(
    AACE_ProcessMode *procMode,
    AACE_MemoryAdrs  *memoryAdrs,
    AACE_MemorySize  *memorySize,
    AACE_SamplingRate  samplingRate,
    AACE_ChannelStatus channelStatus,
    unsigned long bitRate,
    unsigned long nstereoEnable,
    unsigned long reserve);


extern unsigned long AACEncGetConfig(
    unsigned long *context,
    AACE_SamplingRate  *samplingRate,
    AACE_ChannelStatus *channelStatus,
    unsigned long *bitRate,
    unsigned long *nstereoEnable,
    unsigned long *reserve);

extern unsigned long AACEncGetVersion(void);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef _aacencoder_h_ */
