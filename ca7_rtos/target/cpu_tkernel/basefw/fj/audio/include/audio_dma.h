/**
 * @file		audio_dma.h
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _AUDIO_DMA_H_
#define _AUDIO_DMA_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

// DMA ID
#define D_AUDIO_DMA_ID_0	(0)		// DMA ID 0
#define D_AUDIO_DMA_ID_1	(1)		// DMA ID 1
#define D_AUDIO_DMA_ID_2	(2)		// DMA ID 2
#define D_AUDIO_DMA_ID_3	(3)		// DMA ID 3
#define D_AUDIO_DMA_ID_NUM	(4)		// Number of DMA ID


// channel of HDMAC0
#define D_AUDIO_DMA_CH_0	(0)		// CH0(DMA ID 0)
#define D_AUDIO_DMA_CH_1	(1)		// CH1(DMA ID 0)
#define D_AUDIO_DMA_CH_2	(2)		// DMA ID 1
#define D_AUDIO_DMA_CH_3	(3)		// DMA ID 1
#define D_AUDIO_DMA_CH_4	(4)		// DMA ID 2
#define D_AUDIO_DMA_CH_5	(5)		// DMA ID 2
#define D_AUDIO_DMA_CH_6	(6)		// DMA ID 3
#define D_AUDIO_DMA_CH_7	(7)		// DMA ID 3
#define D_AUDIO_DMA_CH_NUM	(8)		// Number of DMA CH


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	UINT8		dma_id;
	UINT8		audio_dev_ch;
	UINT32		src_address;
	UINT32		dst_address;
	UINT32		trans_size;
} T_AUDIO_DMA_START_PARAM;

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/
extern INT32 BF_Audio_DMA_Init( UINT8* dma_id );
extern VOID BF_Audio_DMA_Set_Callback_Fnc0( UINT8 dma_id, VOID (*callback_fnc)(UCHAR dma_status) );
extern VOID BF_Audio_DMA_Set_Callback_Fnc1( UINT8 dma_id, VOID (*callback_fnc)(UCHAR dma_status) );
extern VOID BF_Audio_DMA_Input_Start0( T_AUDIO_DMA_START_PARAM* dma_param );
extern VOID BF_Audio_DMA_Input_Start1( T_AUDIO_DMA_START_PARAM* dma_param );
extern VOID BF_Audio_DMA_Output_Start0( T_AUDIO_DMA_START_PARAM* dma_param );
extern VOID BF_Audio_DMA_Output_Start1( T_AUDIO_DMA_START_PARAM* dma_param );
extern INT32 BF_Audio_DMA_Stop( UINT8 dma_id );
extern UINT32 BF_Audio_DMA_Get_Remain_Trns_Size( UINT8 dma_id );
extern UINT32 BF_Audio_DMA_Get_Trns_Size( UINT8 dma_id );
extern UINT32 BF_Audio_DMA_Get_RemainNumDmaID( VOID );

#endif	// _AUDIO_DMA_H_
