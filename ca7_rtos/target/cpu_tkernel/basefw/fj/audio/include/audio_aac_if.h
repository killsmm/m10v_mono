/**
 * @file		audio_aac_if.h
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _AUDIO_AAC_IF_H_
#define _AUDIO_AAC_IF_H_


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Standard Return Value */
#define AAC_RET_TRUE				(0x8000U)
#define AAC_RET_FALSE				(0x8FFFU)

#define AAC_RET_SUCCESS				(0x00000000UL)
#define AAC_RET_ERROR				(0xF0000000UL)
#define AAC_RET_WARNING				(0xA0000000UL)

/* Detail Return Value */
#if 0
#define AAC_RET_L1_AACENC			(0x0100U)
#define AAC_RET_L1_AACENC_RAW		(0x0200U)
#define AAC_RET_L1_AACDEC			(0x0300U)
#define AAC_RET_L1_AACDEC_RAW		(0x0400U)
#define AAC_RET_L1_FRAMEWORK		(0x0F00U)

#define AAC_RET_L2_HEAP_OVER		(0x0010U)
#define AAC_RET_L2_CMD_INIT			(0x0020U)
#define AAC_RET_L2_CMD_PROCESS		(0x0030U)
#define AAC_RET_L2_CMD_CLOSE		(0x0040U)
#define AAC_RET_L2_CMD_DEBUG		(0x0050U)
#define AAC_RET_L2_LIB				(0x00A0U)
#define AAC_RET_L2_LIB_INIT			(0x00B0U)
#define AAC_RET_L2_LIB_CONFIG		(0x0090U)
#define AAC_RET_L2_LIB_SPCONFIG		(0x0080U)
#define AAC_RET_L2_LIB_GETINFO		(0x00C0U)
#define AAC_RET_L2_LIB_GETFRAME		(0x00D0U)
#define AAC_RET_L2_LIB_DECENC		(0x00E0U)
#define AAC_RET_L2_LIB_CLOSE		(0x00F0U)

#define AAC_RET_L3_OPR0				(0x000FU)
#define AAC_RET_L3_OPR1				(0x0001U)
#define AAC_RET_L3_OPR2				(0x0002U)
#define AAC_RET_L3_OPR3				(0x0003U)
#define AAC_RET_L3_OPR4				(0x0004U)
#define AAC_RET_L3_OPR5_6			(0x0005U)
#define AAC_RET_L3_OPR7				(0x0007U)
#define AAC_RET_L3_OPR8_9			(0x0008U)
#define AAC_RET_L3_OPR10_11			(0x000AU)
#define AAC_RET_L3_OPR12_13			(0x000CU)
#define AAC_RET_L3_OPR14_15			(0x000EU)
#endif

/*********************************************
 COMMAND CODE DEFINE                          
*********************************************/

/*// COMMON COMMAND */
#define AAC_CMD_ERR_COMMAND			(0xF000U)


/*********************************************
 OPERAND SETTING DEFINE                       
*********************************************/

/* ---------------
    VALUES DEFINE 
-----------------*/
   
#define AAC_LIB_AACENC				(0x0)		// 000
#define AAC_LIB_AACENC_RAW			(0x1)		// 001
#define AAC_LIB_AACDEC				(0x4)		// 100
#define AAC_LIB_AACDEC_RAW			(0x5)		// 101

#define AAC_ORIGINAL_YES			(0x2)		// xxxx xxxx xxxx xx1x
#define AAC_ORIGINAL_NO				(0x0)     // xxxx xxxx xxxx xx0x
#define AAC_COPYRIGHT_YES			(0x1)
#define AAC_COPYRIGHT_NO			(0x0)

#define AAC_MODE_PCM_MONO			(0x1)
#define AAC_MODE_PCM_STEREO			(0x0)
#define AAC_MODE_STRM_STEREO 		(0x0)
#define AAC_MODE_STRM_JSTEREO		(0x1)
#define AAC_MODE_STRM_DUAL  		(0x2)
#define AAC_MODE_STRM_MONO 			(0x3)
#define AAC_MODE_STRM_2CH_MONO 		(0x4)
#define AAC_ENDIAN_LITTLE			(0x0)
#define AAC_ENDIAN_BIG				(0x1)

/* SAMPLEING RATE */

// Notification of Decoded aac
									//       base div(x1=00 x2=01 x3=10 x4=11)
#define AAC_SMP_7350				(0x0)	// 0000    00 00
#define AAC_SMP_8000				(0x4)	// 0100    01 00
#define AAC_SMP_11025				(0x8)	// 1000    10 00
#define AAC_SMP_12000				(0xC)	// 1100    11 00
#define AAC_SMP_16000				(0x5)	// 0101    01 01
#define AAC_SMP_22050				(0x9)	// 1001    10 01
#define AAC_SMP_24000				(0xD)	// 1101    11 01
#define AAC_SMP_32000				(0x6)	// 0110    01 10
#define AAC_SMP_44100				(0xA)	// 1010    10 10
#define AAC_SMP_48000				(0xE)	// 1110    11 10
#define AAC_SMP_64000				(0x7)	// 0111    01 11
#define AAC_SMP_88200				(0xB)	// 1011    10 11
#define AAC_SMP_96000				(0xF)	// 1111    11 11
#define AAC_SMP_OTHER				(0x3)	// 0011

// Encode request sampling rate
#define AUDIO_AAC_IF_REQ_SMP_8000	(0x0)
#define AUDIO_AAC_IF_REQ_SMP_11025	(0x1)
#define AUDIO_AAC_IF_REQ_SMP_12000	(0x2)
#define AUDIO_AAC_IF_REQ_SMP_16000	(0x3)
#define AUDIO_AAC_IF_REQ_SMP_22050	(0x4)
#define AUDIO_AAC_IF_REQ_SMP_24000	(0x5)
#define AUDIO_AAC_IF_REQ_SMP_32000	(0x6)
#define AUDIO_AAC_IF_REQ_SMP_44100	(0x7)
#define AUDIO_AAC_IF_REQ_SMP_48000	(0x8)




/* ---------------
    BITSHIFT      
-----------------*/
#define AAC_BSFT_OPR1_ENC_ORIGINAL	(1)
#define AAC_BSFT_OPR1_ENC_COPYRIGHT	(0)
#define AAC_BSFT_OPR2_MODE_PCM		(0)
#define AAC_BSFT_OPR2_ENDIAN		(15)


/* ---------------
    MASK          
-----------------*/
									          //              ---- ---- ---- ----
#define AAC_M_OPR1_ORGCOPY			(0x00000002)  // 1         :  0000 0000 0000 0010
#define AAC_M_OPR1_COPYRIGHT		(0x00000001)  // 0         :  0000 0000 0000 0001

#define AAC_M_OPR2_MODE_PCM			(0x00000001)  // 0         :  0000 0000 0000 0001


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

extern ULONG* BF_Audio_Aac_If_AAC_Stream_Init( const ULONG* );
extern ULONG* BF_Audio_Aac_If_AAC_Chunk_Process( const ULONG* );
extern VOID BF_Audio_Aac_If_AAC_Stream_Stop( VOID );
extern VOID BF_Audio_Aac_If_AAC_Stream_End( VOID );
extern BYTE BF_Audio_Aac_If_AAC_Get_StreamStopFlag( VOID );
extern VOID BF_Audio_Aac_If_WriteOperand(INT32, ULONG );
extern ULONG BF_Audio_Aac_If_ReadOperand( INT32, ULONG* );
extern ULONG BF_Audio_Aac_If_AAC_Get_EncVersion( VOID );

#endif		// _AUDIO_AAC_IF_H_
