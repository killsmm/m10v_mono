/******************************************************************************/
/**
 *  @file   camera_if.h
 *  @brief  sample
 */
/*
 *  Copyright 2018 Socionext Inc.
 ******************************************************************************/
#ifndef __CAMERA_IF_H__
#define __CAMERA_IF_H__

/* Debug print */
#define CAMERA_IF_DEBUG 0

/********************************************************************
 *  Common define definition
 ********************************************************************/
/* camera_if command parameters.
 * for details, see camera_if specifications. */
#define IMAGE_TYPE_JPEG			0x00
#define IMAGE_STORE_CHANNEL_SD1	0x01
#define IMAGE_CAPTURE_SIZE_12M	0x01
#define IMAGE_QUALITY_LOW		0x00
#define IMAGE_QUALITY_MIDDLE	0x01
#define IMAGE_QUALITY_HIGH		0x02
#define IMAGE_FRAME_RATE_5FPS	0x01
#define IMAGE_FRAME_RATE_10FPS	0x02
#define IMAGE_FRAME_RATE_15FPS	0x03
#define IMAGE_FRAME_RATE_30FPS	0x04
#define BOOT_MODE				0x00
#define IDLE_MODE				0x01
#define PREVIEW_MODE			0x02
#define SINGLE_MODE				0x03
#define BURST_MODE				0x04
#define VIDEO_MODE				0x08
#define PLAYBACK_MODE			0x09
#define VIDEO_SIZE_4K 			0x00
#define VIDEO_60FPS				0x02
#define VIDEO_HEVC				0x00
#define VIDEO_H264				0x01
#define FORMAT_SD1				0x01
#define PLAYBACK_JPEG_PLAY		0x01
#define PLAYBACK_VIDEO_PLAY		0x03

#define CAMERA_IF_STRING_ADDRESS 0x4FE90050

/********************************************************************
 *  Function prototype
 ********************************************************************/
int Finish_Boot(UINT32* result, UINT32* err_code);
int Set_Ev_Compenstion(UINT32* result, UINT32* err_code, UINT32 ev);
int Set_White_Balance(UINT32* result, UINT32* err_code, UINT32 wb, UINT32 r_gain, UINT32 b_gain);
int Set_Sharpness(UINT32* result, UINT32* err_code, UINT32 sharpness, UINT32 edge_strength);
int Set_ISO(UINT32* result, UINT32* err_code, UINT32 iso);
int Set_AE_Coverage_Speed(UINT32* result, UINT32* err_code, UINT32 ae_coverage_speed);
int Set_AE_Lock(UINT32* result, UINT32* err_code, UINT32 ae_lock);
int Set_Size_Framerate(UINT32* result, UINT32* err_code, UINT32 size, UINT32 frame_rate, UINT32 codec_type);
int Set_Image_Type(UINT32* result, UINT32* err_code, UINT32 format);
int Set_Store_Channel(UINT32* result, UINT32* err_code, UINT32 sd_card_slot);
int Set_Capture_Size(UINT32* result, UINT32* err_code, UINT32 capture_size);
int Set_Quality(UINT32* result, UINT32* err_code, UINT32 quality);
int Change_Camera_Mode(UINT32* result, UINT32* err_code, UINT32 change_mode);
int Format_SD_Card(UINT32* result, UINT32* err_code, UINT32 format);
int Get_Ev_Compenstion(UINT32* ev);
int Get_White_Balance(UINT32* wb, UINT32* r_gain, UINT32* b_gain);
int Get_Sharpness(UINT32* sharpness, UINT32* edge_strength);
int Get_ISO(UINT32* iso);
int Get_AE_Coverage_Speed(UINT32* ae_coverage_speed);
int Get_AE_Lock(UINT32* ae_lock);
int Get_Size_Framerate(UINT32* size, UINT32* frame_rate, UINT32* codec_type);
int Get_Image_Type(UINT32* format);
int Get_Store_Channel(UINT32* sd_card_slot);
int Get_Capture_Size(UINT32* capture_size);
int Get_Quality(UINT32* quality);
int Boot_Up(void);
int Preview(void);
int Video_Capture(UINT32 v_size, UINT32 v_frame_rate, UINT32 v_codec_type);
int Single_Capture(UINT32 s_format, UINT32 s_sd_card_slot, UINT32 s_capture_size, UINT32 s_quality);
int Media_Format(UINT32 format);
void Sample_Menu(void);

extern volatile UCHAR gFlag_video_capture_complete;
extern volatile UCHAR gFlag_single_capture_complete;
extern volatile UCHAR gFlag_play_back_complete;
extern volatile UINT32 gStatus_sample_program;

#endif /* __CAMERA_IF_H__ */
