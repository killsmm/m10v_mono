/**
 * @file		sdram_map_play.c
 * @brief		SocioNext SDRAM for Play.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "debug.h"
#include "sdram_map_cache_define.h"
#include "sdram_map_play.h"
#include "fj_audio.h"
#include "mba_if.h"
/* ------------------------------------------------------------------------- */
/* Structure of Play SDRAM Mapping                                           */
/* ------------------------------------------------------------------------- */

/**
 * common map structure on SDRAM
 */
typedef struct {
	ULONG vde[ D_SDRAM_MAP_PLAY_SDRAM_BNK_VDE ];
} T_SDRAM_MAP_PLAY;
/**
 * HD display buffer map on SDRAM
 */
static T_SDRAM_MAP_PLAY gSdram_Map_Play_HD = {
	{ SDRAM_ADR_PLAY_HDMI_MAIN_0,	SDRAM_ADR_PLAY_HDMI_MAIN_1,	SDRAM_ADR_PLAY_HDMI_MAIN_2, SDRAM_ADR_PLAY_HDMI_MAIN_3 },
};
static INT32 gHDMI_HD_Index = 0;
static INT32 gHDMI_HD_LockIndex = 0;


/******************************/
/**  Boot Section(FastBoot)  **/
/******************************/

/**
* Get VDE vram new address for display (with VRAM index increment)
* @param   VOID
* @return  VDE vram new address
 */
ULONG BF_Sdram_Map_Play_Get_Vga_Out_Addr( VOID )
{
	return Sdram_Map_Common_Change_Vde(TRUE);
}

/**
* Get HD vram new address for display (with VRAM index increment)
* @param   VOID
* @return  HD vram new address
 */
ULONG BF_Sdram_Map_Play_Get_Hd_Out_Addr( VOID )
{
	return Sdram_Map_Common_Change_HDMI_HD();
}

/******************************/
/**  PB Section(FastBoot)    **/
/******************************/

/**
* Get Play Image Load Address for Loading data from Media
* @param   VOID
* @return  Image Load Address
*/
ULONG BF_Sdram_Map_Play_Get_Image_Load_Addr( VOID )
{
  return (ULONG)SDRAM_ADR_PLAY_IMAGE_LOAD_AREA;
}

/**
* Get Play image work Address for JPEG decoded data
* @param   VOID
* @return  image work Address
*/
ULONG BF_Sdram_Map_Play_Get_Image_Work_Addr( VOID )
{
  return (ULONG)SDRAM_ADR_PLAY_IMAGE_DECODE_AREA;
}


/******************************/
/**  Last Section(FastBoot)  **/
/******************************/

#if 0 
/**
* Get Play Thumbnail Load Address for Loading data from Media
* @param   VOID
* @return  Thumbnail Load Address
*/
ULONG BF_Sdram_Map_Play_Get_Index_Load_Addr( VOID )
{
  return (ULONG)SDRAM_ADR_PLAY_THUMBINDEX_LOAD;
}


/**
* Get Play Image Work Address for Rotate data from Media
* @param   VOID
* @return  Rotate Load Address
*/
ULONG BF_Sdram_Map_Play_Get_Rotate_Img_Work_0_Addr( VOID )
{
	return (ULONG)SDRAM_ADR_PLAY_ROTATE_WORK_0;
}

/**
* Get Play Image Work Address for Rotate data from Media
* @param   VOID
* @return  Rotate Load Address
*/
ULONG BF_Sdram_Map_Play_Get_Rotate_Img_Work_1_Addr( VOID )
{
	return (ULONG)SDRAM_ADR_PLAY_ROTATE_WORK_1;
}

/**
* Get Play Image Work Address for Rotate data from Media
* @param   VOID
* @return  Rotate Load Address
*/
ULONG BF_Sdram_Map_Play_Get_Rotate_Img_Work_2_Addr( VOID )
{
	return (ULONG)SDRAM_ADR_PLAY_ROTATE_WORK_2;
}
#endif 

/**
* Get Play Image Work Address for Half Rec Area Address
* @param   VOID
* @return  Half Rec Area Address
*/
ULONG BF_Sdram_Map_Play_Get_Half_Rec_Addr( VOID )
{
	return (ULONG)SDRAM_ADR_PLAY_IMAGE_HALF_REC_AREA_0_Y;
}


/*--------------------------------------*/
/* Voice Memo Record & Play, Audio Play */
/*--------------------------------------*/
typedef struct{
	USHORT in_pos;	/**< data int postion  */
	USHORT out_pos;	/**< data out postion */
	USHORT count;	/**< not read data remain */
}FJ_AUDIO_PLAY_RINGBUFFER;

typedef struct {
	FJ_AUDIO_PLAY_RINGBUFFER	PCM;
	FJ_AUDIO_PLAY_RINGBUFFER	uLAW;
}FJ_AUDIO_Play_SDRAM_BUFFER_INFO;

//Audio Play
static const ULONG gAudio_Play_PCM_Array[SDRAM_BNK_PLAY_PCM]={
	SDRAM_ADR_PLAY_PCM_0, SDRAM_ADR_PLAY_PCM_1, SDRAM_ADR_PLAY_PCM_2, SDRAM_ADR_PLAY_PCM_3
};

static const ULONG gAudio_Play_uLAW_Array[SDRAM_BNK_PLAY_AUDIO]={
	SDRAM_ADR_PLAY_AUDIO_0, SDRAM_ADR_PLAY_AUDIO_1, SDRAM_ADR_PLAY_AUDIO_2, SDRAM_ADR_PLAY_AUDIO_3
};

/******************************/
/**  Last Section(FastBoot)  **/
/******************************/

/*--------------------------------------*/
/* Voice Memo Record & Play, Audio Play */
/*--------------------------------------*/
static const ULONG g_Play_Ahead_Addr_Array[SDRAM_BNK_PLAY_SINGLE_READ_AHEAD_AREA]={
	SDRAM_ADR_PLAY_SINGLE_READ_AHEAD_AREA_0, SDRAM_ADR_PLAY_SINGLE_READ_AHEAD_AREA_1, SDRAM_ADR_PLAY_SINGLE_READ_AHEAD_AREA_2, 
	SDRAM_ADR_PLAY_SINGLE_READ_AHEAD_AREA_3, SDRAM_ADR_PLAY_SINGLE_READ_AHEAD_AREA_4
};

/**
 * Get Audio uLAW output address
 * @param   VOID
 * @return  VOID
 */
ULONG BF_Sdram_Map_Play_Get_Ahead_Load_Addr(VOID)
{
	// Voice Memo Record & Play, Audio Play
	return M_SDRAM_MAP_COMMON_ADR_LOG2PHY( SDRAM_ADR_PLAY_AHEAD_AREA_LOAD_AREA );	/* pgr0080 */
}

/**
* Get Playback JPEG Decode YUV Buffer Address for Main
* @param   VOID
* @return  Top address of Jpeg decode YUV buffer for Main.
*/
UINT32 BF_Sdram_Map_Play_Get_Decoded_YUV_Main_Addr( VOID )
{
  return (UINT32)SDRAM_ADR_PLAY_DECODED_YUV_MAIN;
}

/**
* Get Playback JPEG Decode YUV Buffer Size for Main
* @param   VOID
* @return  Size of Jpeg decode YUV buffer for Main.
*/
UINT32 BF_Sdram_Map_Play_Get_Decoded_YUV_Main_Size( VOID )
{
  return (UINT32)SDRAM_SIZ_PLAY_DECODED_YUV_MAIN;
}

/**
* Get Playback JPEG Decode YUV Buffer Address for Thumbnail
* @param   VOID
* @return  Top address of Jpeg decode YUV buffer for thumbnail.
*/
UINT32 BF_Sdram_Map_Play_Get_Decoded_YUV_Thumb_Addr( VOID )
{
  return (UINT32)SDRAM_ADR_PLAY_DECODED_YUV_THM;
}

/**
* Get Playback JPEG Decode YUV Buffer Size for Thumbnail
* @param   VOID
* @return  Size of Jpeg decode YUV buffer for thumbnail.
*/
UINT32 BF_Sdram_Map_Play_Get_Decoded_YUV_Thumb_Size( VOID )
{
  return (UINT32)SDRAM_SIZ_PLAY_DECODED_YUV_THM;
}

/**
* Get Playback YUV Resize Buffer Address
* @param   VOID
* @return  Top address of Jpeg YUV resize buffer.
*/
UINT32 BF_Sdram_Map_Play_Get_Resize_YUV_Addr( VOID )
{
  return (UINT32)SDRAM_ADR_PLAY_DECODED_RESIZE_YUV_0;
}

/**
* Get Playback YUV Resize Buffer Size
* @param   VOID
* @return  Size of Jpeg YUV resize buffer.
*/
UINT32 BF_Sdram_Map_Play_Get_Resize_YUV_Size( VOID )
{
  return (UINT32)(SDRAM_SIZ_PLAY_DECODED_RESIZE_YUV * SDRAM_BNK_PLAY_DECODED_RESIZE_YUV);
}

/**
* Get Playback RGB Buffer Address for Screennail
* @param   VOID
* @return  Top address of RGB buffer for screennail.
*/
UINT32 BF_Sdram_Map_Play_Get_Screen_RGB_Addr( VOID )
{
  return (UINT32)SDRAM_ADR_PLAY_DECODED_RGB;
}

/**
* Get Playback RGB Buffer Size for Screennail
* @param   VOID
* @return  Size of RGB buffer for screennail.
*/
UINT32 BF_Sdram_Map_Play_Get_Screen_RGB_Size( VOID )
{
  return (UINT32)SDRAM_SIZ_PLAY_DECODED_RGB;
}

/**
* Get Playback RGB Buffer Address for Thumbnail
* @param   VOID
* @return  Top address of RGB buffer for thumbnail.
*/
UINT32 BF_Sdram_Map_Play_Get_Thumb_RGB_Addr( VOID )
{
  return (UINT32)SDRAM_ADR_PLAY_THM_RGB_0;
}

/**
* Get Playback RGB Buffer Size for Thumbnail
* @param   VOID
* @return  Size of RGB buffer for thumbnail.
*/
UINT32 BF_Sdram_Map_Play_Get_Thumb_RGB_Size( VOID )
{
  return (UINT32)(SDRAM_SIZ_PLAY_THM_RGB * SDRAM_BNK_PLAY_THM_RGB);
}

/**
* Get Playback Disp YUV Buffer Address for LCD
* @param   VOID
* @return  Top address of disp YUV buffer for LCD.
*/
UINT32 BF_Sdram_Map_Play_Get_Disp_YUV_LCD_Addr( VOID )
{
	return (UINT32)SDRAM_ADR_LCD_MAIN_0;
}

/**
* Get Playback Disp YUV Buffer Size for LCD
* @param   VOID
* @return  Size of disp YUV buffer for LCD.
*/
UINT32 BF_Sdram_Map_Play_Get_Disp_YUV_LCD_Size( VOID )
{
	return (UINT32)(SDRAM_SIZ_LCD_MAIN * SDRAM_BNK_LCD_MAIN);
}

/**
* Get Playback Disp YUV Buffer Address for HDMI
* @param   VOID
* @return  Top address of disp YUV buffer for HDMI.
*/
UINT32 BF_Sdram_Map_Play_Get_Disp_YUV_HDMI_Addr( VOID )
{
	return (UINT32)SDRAM_ADR_PLAY_HDMI_MAIN_0;
}

/**
* Get Playback Disp YUV Buffer Size for HDMI
* @param   VOID
* @return  Size of disp YUV buffer for HDMI.
*/
UINT32 BF_Sdram_Map_Play_Get_Disp_YUV_HDMI_Size( VOID )
{
 	return (UINT32)(SDRAM_WIDTH_PLAY_HDMI_MAIN * SDRAM_LINES_PLAY_HDMI_MAIN);
}

/**
 * change HD Index
 */
UINT32 Sdram_Map_Play_Change_HDMI_HD(BOOL update)
{
	ULONG addr[3];
	UINT32 disp_addr;

	if (update) {
		FJ_GetMainDispBuffAddr( E_OSDLayer_HDMI_MAIN, addr );

		gHDMI_HD_Index++;
		if( gHDMI_HD_Index >= SDRAM_BNK_PLAY_HDMI_MAIN ){
			gHDMI_HD_Index = 0;
		}

		if (Sdram_Map_Play_Get_HDMI_HD() == addr[0]) {
			gHDMI_HD_Index++;
			if( gHDMI_HD_Index >= SDRAM_BNK_PLAY_HDMI_MAIN ){
				gHDMI_HD_Index = 0;
			}
		}
		gHDMI_HD_LockIndex = gHDMI_HD_Index;
		disp_addr = Sdram_Map_Play_Get_HDMI_HD();
	} else {
		if (gHDMI_HD_LockIndex == gHDMI_HD_Index) {
			gHDMI_HD_Index = (gHDMI_HD_Index - 1) < 0 ? SDRAM_BNK_PLAY_HDMI_MAIN - 1 : gHDMI_HD_Index - 1;
			disp_addr = Sdram_Map_Play_Get_HDMI_HD();
			gHDMI_HD_Index = (gHDMI_HD_Index + 1) >= SDRAM_BNK_PLAY_HDMI_MAIN ? 0 : gHDMI_HD_Index + 1;
		} else {
			disp_addr = Sdram_Map_Play_Get_HDMI_HD();
		}
	}

	return disp_addr;
}

VOID Sdram_Map_Play_Free_HDMI_LockIdx(VOID)
{
	gHDMI_HD_LockIndex = -1;
}

/**
 * Get HD address
 * @return ULONG: address
 */
UINT32 Sdram_Map_Play_Get_HDMI_HD(VOID)
{
	return gSdram_Map_Play_HD.vde[gHDMI_HD_Index];
}

/**
 * Get HD(Y) address
 * @return ULONG: address
 */
UINT32 Sdram_Map_Play_Get_HDMI_HD_Y(VOID)
{
	return gSdram_Map_Play_HD.vde[gHDMI_HD_Index];
}

/**
 * get HD(Cb) address
 * @return ULONG: address
 */
UINT32 Sdram_Map_Play_Get_HDMI_HD_Cb(VOID)
{
	return (gSdram_Map_Play_HD.vde[gHDMI_HD_Index] + BF_Sdram_Map_Play_Get_Disp_YUV_HDMI_Size());
}
