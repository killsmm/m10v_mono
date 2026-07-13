/**
 * @file		audio_auif.h
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _AUDIO_AUIF_H_
#define _AUDIO_AUIF_H_


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_AUDIO_AUIF_CH0		(0)
#define D_AUDIO_AUIF_CH1		(1)
#define D_AUDIO_AUIF_CH2		(2)
#define D_AUDIO_AUIF_CH3		(3)
#define D_AUDIO_AUIF_CH4		(4)
#define D_AUDIO_AUIF_CH5		(5)
#define D_AUDIO_AUIF_CH0_CH1	(16)
#define D_AUDIO_AUIF_CH4_CH5	(17)
#define D_AUDIO_AUIF_CH_NUM		(6)

// right-justified / left-justified
#define D_AUDIO_AUIF_AHB_FORMAT_R	(0)
#define D_AUDIO_AUIF_AHB_FORMAT_L	(1)


#define D_AUDIO_AUIF_GAIN_0DB	(0)
#define D_AUDIO_AUIF_GAIN_6DB	(1)
#define D_AUDIO_AUIF_GAIN_12DB	(2)
#define D_AUDIO_AUIF_GAIN_18DB	(3)


typedef enum {
	E_AUDIO_DMA_MODE_IN_1CH		= 0,
	E_AUDIO_DMA_MODE_IN_2CH,
	E_AUDIO_DMA_MODE_OUT_1CH,
	E_AUDIO_DMA_MODE_OUT_2CH
}E_AUDIO_DMA_MODE;

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/
extern INT32 BF_Audio_Auif_Init_AudioMacro( VOID );
extern INT32 BF_Audio_Auif_OpenInput( UINT8 dev_ch);
extern INT32 BF_Audio_Auif_CloseInput( UINT8 dev_ch);
extern INT32 BF_Audio_Auif_OpenOutput( UINT8 dev_ch);
extern INT32 BF_Audio_Auif_CloseOutput( UINT8 dev_ch);
extern INT32 BF_Audio_Auif_Set_I2sIn_Config( UINT8 dev_ch, T_FJ_AUDIO_I2S_CFG* i2s_cfg );
extern INT32 BF_Audio_Auif_Set_I2sOut_Config( UINT8 dev_ch, const T_FJ_AUDIO_I2S_CFG* i2s_cfg );
extern INT32 BF_Audio_Auif_Set_Channel_Config( UINT8 dev_ch, UINT8 ch_type );
extern INT32 BF_Audio_Auif_Input_Ctrl( UINT8 dev_ch, UINT8 format );
extern INT32 BF_Audio_Auif_Output_Ctrl( UINT8 dev_ch, UINT8 format );
extern INT32 BF_Audio_Auif_Set_Dma_ModeIn( UINT8 dev_ch, E_AUDIO_DMA_MODE mode, USHORT sample );
extern INT32 BF_Audio_Auif_Set_Dma_ModeOut( UINT8 dev_ch, E_AUDIO_DMA_MODE mode, USHORT sample );
extern UINT16 BF_Audio_Auif_Get_Input_Trns_Smaple( UINT8 dev_ch );
extern UINT16 BF_Audio_Auif_Get_Output_Trns_Smaple( UINT8 dev_ch );
extern UINT32 BF_Audio_Auif_Get_Addr_Reg_AUIDLR( UINT8 dev_ch );
extern UINT32 BF_Audio_Auif_Get_Addr_Reg_AUODLR( UINT8 dev_ch );
extern UINT32 BF_Audio_Auif_Get_Addr_Reg_AUIDDMAPT( UINT8 dev_ch );
extern UINT32 BF_Audio_Auif_Get_Addr_Reg_AUODDMAPT( UINT8 dev_ch );
extern VOID BF_Audio_Auif_Input_Start( UINT8 dev_ch );
extern VOID BF_Audio_Auif_Input_Stop( UINT8 dev_ch );
extern VOID BF_Audio_Auif_Output_Start( UINT8 dev_ch );
extern VOID BF_Audio_Auif_Output_Stop( UINT8 dev_ch );
extern VOID BF_Audio_Auif_Set_Enable_OverFlow_Int( UINT8 dev_ch );
extern VOID BF_Audio_Auif_Set_Disable_OverFlow_Int( UINT8 dev_ch );
extern VOID BF_Audio_Auif_Set_Enable_UnderFlow_Int( UINT8 dev_ch );
extern VOID BF_Audio_Auif_Set_Disable_UnderFlow_Int( UINT8 dev_ch );
extern VOID BF_Audio_Auif_Over_Flow_Error_Ch0( VOID );
extern VOID BF_Audio_Auif_Under_Flow_Error_Ch0( VOID );
extern VOID BF_Audio_Auif_Over_Flow_Error_Ch1( VOID );
extern VOID BF_Audio_Auif_Under_Flow_Error_Ch1( VOID );
extern VOID BF_Audio_Auif_Over_Flow_Error_Ch2( VOID );
extern VOID BF_Audio_Auif_Under_Flow_Error_Ch2( VOID );
extern VOID BF_Audio_Auif_Under_Flow_Error_Ch3( VOID );
extern VOID BF_Audio_Auif_Over_Flow_Error_Ch4( VOID );
extern VOID BF_Audio_Auif_Over_Flow_Error_Ch5( VOID );
extern VOID BF_Audio_Auif_Swap_InputData( UINT8 dev_ch, UINT8 enable );
extern VOID BF_Audio_Auif_Swap_OutputData( UINT8 dev_ch, UINT8 enable );
extern INT32 BF_Audio_Auif_Set_InputGain( UINT8 dev_ch, UINT8 volume );
extern INT32 BF_Audio_Auif_Set_OutputGain( UINT8 dev_ch, UINT8 volume );



#endif		//_AUDIO_AUIF_H_

