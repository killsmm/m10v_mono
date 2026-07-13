/**
 * @file		user_boot.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include "user.h"
#include "fj_peripheral.h"
#include "category_parameter.h"
#include "os_if.h"
#include "fj_filesystem.h"
#include "nf_custom.h"
#include "emmc_custom.h"
#include "fj_disp.h"
#include "lcd_config.h"
#if defined (D_LCD_DRV_ST7796S_RGB_DELTA) || defined (D_LCD_DRV_LPM030M369F_YUV_STRIPE)
#include "drv_lcd.h"
#endif
#include "sdram_map_define.h"
#include "audio_codec_wrapper.h"
#include "detect_task.h"
#include "dd_tmr64.h"
#include "fj_peripheral.h"
#include "camera_config.h"
#include "pmc.h"
#include "iq_dpc_tbl.h"
#include "iq_dpc_tbl_sel.h"
#include "iq_dpc_multipipe_tbl.h"
#include "iq_dpc_multipipe_tbl_mng.h"
#include "clk.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define	Dd_PMC_MDR_Get_MD()					(IO_PMC.MDR.word & 0x0000003F)				/**< Get MD5-MD0 pin's value directry. */

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
T_RESOURCE g_resource;
volatile unsigned long g_MDR_value;
UINT32 g_Boot_Wav_ID = 0;

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static int lcd_init(void);
static void bootup_music_play(void);
static void bootup_music_play_hdmi(void);
static void mount_EMMC(void);
static void mount_NAND(void);
static void assign_resource(void);
static void start_timer64_gyro(void);
/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void User_Boot( void )
{
	FJ_ERR_CODE	ercd;
	FJ_UART_DEBUG_CONFIG cfg;
	int lcd_conneted = 0;
	
	start_timer64_gyro();

	cfg.overwrite = TRUE;
	cfg.suspend = FALSE;
	FJ_UART_Debug_Configure(&cfg);

	FJ_UART_Debug_Start((FJ_PERI_CH)UART_CTRL_CH, FJ_USIO_DMA_CH_NOT_USE);
	
	Cate_Init();
	
	OS_IF_Init();
	
	lcd_conneted = lcd_init();
	
	if(User_Is_EmmcBoot_Mode() == TRUE){
		mount_EMMC();
	}else{
		mount_NAND();
	}
	
	assign_resource();
	
	OS_User_Sta_Tsk( TID_DETECT, 0 );
	OS_User_Sta_Tsk( TID_AWB, 0 );
	OS_User_Sta_Tsk( TID_AE, 0 );
	OS_User_Sta_Tsk( TID_AF, 0 );
	OS_User_Sta_Tsk( TID_MOTOR_CALC_0, 0 );
	OS_User_Sta_Tsk( TID_MOTOR_CALC_1, 0 );

	if(lcd_conneted == 0){
		Bootup_Image_Display_LCD();
	}

	// Initial audio parameter for boot
	ercd = FJ_AudioOut_DirectLoadWav( ( UINT32 )g_resource.BOOTUP_addr, ( ULONG )g_resource.BOOTUP_size, &g_Boot_Wav_ID );
	UPRINTF_ERR(ercd);

	// Play audio
	Bootup_Music_Play(FALSE);
	
	BF_IQ_Dpc_Tbl_Init();
	BF_IQ_Dpc_Tbl_Sel_Init();
	BF_IQ_Dpc_MultiPipeTbl_Init();
	BF_IQ_Dpc_MultiPipeTblMng_Init();

	UPRINTF("[CLOCK]Change Image Clock Mode to Idle\n");
	FJ_Top_Set_Image_Clock( IMAGE_CLOCK[E_IMG_CLOCK_MODE_IDLE] );
	
}


const T_RESOURCE* User_Get_Resource( void )
{
	return &g_resource;
}

BOOL User_Is_EmmcBoot_Mode(VOID)
{
	g_MDR_value = (Dd_PMC_MDR_Get_MD() & 0xF);
	
	if( g_MDR_value == 1 ) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static int lcd_init(void)
{
	FJ_ERR_CODE			ercd;
	UCHAR				disp_id; // dummy
	FJ_DISP_LCD_CONFIG	lcd_config;
	
	ercd = FJ_Disp_Open( FJ_DISP_TYPE_LCD, &disp_id );
	if(ercd != FJ_ERR_OK){
		UPRINTF_ERR(ercd);
		return FJ_ERR_NG;
	}
	
	// depends on lcd
	lcd_config.lcd_clk						= D_LCD_CONFIG_CLOCK;
	lcd_config.lcd_polarity.clock			= (FJ_DISP_LCD_EDGE_TYPE)D_LCD_CONFIG_POLARITY_CLOCK;
	lcd_config.lcd_polarity.vsync			= (FJ_DISP_LCD_POLARITY_TYPE)D_LCD_CONFIG_POLARITY_VSYNC;
	lcd_config.lcd_polarity.hsync			= (FJ_DISP_LCD_POLARITY_TYPE)D_LCD_CONFIG_POLARITY_HSYNV;
	lcd_config.lcd_polarity.venable			= (FJ_DISP_LCD_POLARITY_TYPE)D_LCD_CONFIG_POLARITY_VENABLE;
	lcd_config.lcd_polarity.henable			= (FJ_DISP_LCD_POLARITY_TYPE)D_LCD_CONFIG_POLARITY_HENABLE;
	lcd_config.lcd_timing.scan_mode			= (FJ_DISP_LCD_SCAN_MODE)D_LCD_CONFIG_SCAN_MODE;
	lcd_config.lcd_timing.vcyc				= D_LCD_CONFIG_VCYC;
	lcd_config.lcd_timing.vcyc_shift		= D_LCD_CONFIG_VCYC_SHIFT;
	lcd_config.lcd_timing.hcyc				= D_LCD_CONFIG_HCYC;
	lcd_config.lcd_timing.vpw				= D_LCD_CONFIG_VPW;
	lcd_config.lcd_timing.hpw				= D_LCD_CONFIG_HPW;
	lcd_config.lcd_timing.vblk				= D_LCD_CONFIG_VBLK;
	lcd_config.lcd_timing.hblk				= D_LCD_CONFIG_HBLK;
	lcd_config.lcd_timing.vsize				= D_LCD_CONFIG_VSIZE;
	lcd_config.lcd_timing.hsize				= D_LCD_CONFIG_HSIZE;

	lcd_config.lcd_format.output_mode		= (FJ_DISP_LCD_OUTPUT_MODE)D_LCD_CONFIG_OUTPUT_MODE;
	lcd_config.lcd_format.output_order[0]	= (FJ_DISP_LCD_OUTPUT_ORDER)D_LCD_CONFIG_OUTPUT_ORDER_1;
	lcd_config.lcd_format.output_order[1]	= (FJ_DISP_LCD_OUTPUT_ORDER)D_LCD_CONFIG_OUTPUT_ORDER_2;
	lcd_config.lcd_format.output_order[2]	= (FJ_DISP_LCD_OUTPUT_ORDER)D_LCD_CONFIG_OUTPUT_ORDER_3;
	lcd_config.lcd_format.output_order[3]	= (FJ_DISP_LCD_OUTPUT_ORDER)D_LCD_CONFIG_OUTPUT_ORDER_4;
	lcd_config.lcd_format.output_order[4]	= (FJ_DISP_LCD_OUTPUT_ORDER)D_LCD_CONFIG_OUTPUT_ORDER_5;
	lcd_config.lcd_format.output_order[5]	= (FJ_DISP_LCD_OUTPUT_ORDER)D_LCD_CONFIG_OUTPUT_ORDER_6;
	lcd_config.lcd_format.pix_fmt			= (FJ_DISP_LCD_PIX_FMT)D_LCD_CONFIG_PIXEL_FORMAT;

	ercd = FJ_Disp_LCD_Config( FJ_DISP_TYPE_LCD, &lcd_config );
	if(ercd != FJ_ERR_OK){
		UPRINTF_ERR(ercd);
		return FJ_ERR_NG;
	}
	
	ercd = FJ_Disp_LCD_Start_Sync( FJ_DISP_TYPE_LCD );
	if(ercd != FJ_ERR_OK){
		UPRINTF_ERR(ercd);
		return FJ_ERR_NG;
	}
	
#if defined (D_LCD_DRV_ST7796S_RGB_DELTA) || defined (D_LCD_DRV_LPM030M369F_YUV_STRIPE)
	ercd = drv_lcd_init();
	if(ercd != FJ_ERR_OK){
		UPRINTF_ERR(ercd);
		return FJ_ERR_NG;
	}
	
	ercd = drv_lcd_enable();
	if(ercd != FJ_ERR_OK){
		UPRINTF_ERR(ercd);
		return FJ_ERR_NG;
	}
#endif
	
	ercd = FJ_Disp_Video_Enable( FJ_DISP_TYPE_LCD );
	if(ercd != FJ_ERR_OK){
		UPRINTF_ERR(ercd);
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

void Bootup_Image_Display_LCD(void)
{
	FJ_ERR_CODE				ercd;
	UINT32					width, in_width;
	UINT32					height, in_height;
	UINT32					rotation, keep_ar_height, offset_y;
	FJ_DISP_IMAGE_CONFIG	image_config;
	#define ALIGN_2(x) ((x+2) & 0xfffffffe)

	if( ( User_VIew_Get_Stat() != E_USER_STATE_IDLE)
			|| ( User_Simple_VIew_Get_Stat() != E_USER_STATE_IDLE )
			|| ( User_Capture_Get_Stat() != E_USER_STATE_IDLE )
			|| ( User_Play_Is_Busy() == TRUE ) ) {
		return;
	}

	in_width = 1920; // temporary
	in_height = 1080; // temporary
	
	drv_lcd_get_resolution(&width, &height, &rotation);
	keep_ar_height = width * in_height / in_width;
	offset_y = ALIGN_2((height - keep_ar_height) / 2);

	image_config.window.in_x_pos	= 0;
	image_config.window.in_y_pos	= 0;
	image_config.window.in_width	= in_width;
	image_config.window.in_lines	= in_height;
	image_config.window.out_x_pos	= 0;
	image_config.window.out_y_pos	= offset_y;
	image_config.window.out_width	= width;
	image_config.window.out_lines	= keep_ar_height;
	image_config.block				= FJ_DISP_TYPE_LCD;
	image_config.addr				= ( UINT32 )g_resource.BOOTUP_HDMI_addr; // temporary
	image_config.g_width			= image_config.window.in_width;
	image_config.g_height			= image_config.window.in_lines;
	
	// disp resize limit
	if( image_config.window.out_width * 2 < image_config.window.in_width ){
		image_config.window.in_width = image_config.window.out_width * 2;
		image_config.window.in_x_pos = (in_width - image_config.window.in_width ) / 2;
	}
	if( image_config.window.out_lines * 2 < image_config.window.in_lines ){
		image_config.window.in_lines = image_config.window.out_lines * 2;
		image_config.window.in_y_pos = (in_height - image_config.window.in_lines ) / 2;
	}
	ercd = FJ_Disp_Show_Boot_Image( &image_config );
	UPRINTF_ERR(ercd);
}

void Bootup_Image_Display(void)
{
	FJ_ERR_CODE				ercd;
	UINT32					width;
	UINT32					height;
	FJ_DISP_IMAGE_CONFIG	image_config;

	if( ( User_VIew_Get_Stat() != E_USER_STATE_IDLE)
			|| ( User_Simple_VIew_Get_Stat() != E_USER_STATE_IDLE )
			|| ( User_Capture_Get_Stat() != E_USER_STATE_IDLE )
			|| ( User_Play_Is_Busy() == TRUE ) ) {
		return;
	}

	hdmi_get_video_id_reso( &width, &height );
	image_config.window.in_x_pos	= 0;
	image_config.window.in_y_pos	= 0;
	image_config.window.in_width	= 1920;
	image_config.window.in_lines	= 1080;
	image_config.window.out_x_pos	= 0;
	image_config.window.out_y_pos	= 0;
	image_config.window.out_width	= width;
	image_config.window.out_lines	= height;
	image_config.block				= FJ_DISP_TYPE_HDMI;
	image_config.addr				= ( UINT32 )g_resource.BOOTUP_HDMI_addr;
	image_config.g_width			= image_config.window.in_width;
	image_config.g_height			= image_config.window.in_lines;
	ercd = FJ_Disp_Show_Boot_Image( &image_config );
	UPRINTF_ERR(ercd);

	Bootup_Music_Play(TRUE);

}

void Bootup_Music_Play(BOOL hdmi)
{
	if(hdmi == TRUE){
		bootup_music_play_hdmi();
	}
	else{
		bootup_music_play();
	}
}

static void bootup_music_play(void)
{
	UCHAR 					audio_id = 0;
	FJ_ERR_CODE				ercd;
	T_FJ_AUDIO_OUT_CFG		audio_out_cfg;

	ercd = FJ_AudioOut_Open( E_FJ_AUDIO_OUT_0, &audio_id );
	UPRINTF_ERR(ercd);

	audio_out_cfg.audio_out_port		= E_FJ_AUDIO_OUT_PORT_I2S0;
	audio_out_cfg.i2s_cfg.master_slave	= FJ_AUDIO_IF_MASTER;
	audio_out_cfg.i2s_cfg.lr_clock		= 48000;
	audio_out_cfg.i2s_cfg.data_cycle	= 64;
	audio_out_cfg.i2s_cfg.i2s_format	= E_FJ_AUDIO_I2S_Data_FORMAT_LEFT;
	audio_out_cfg.i2s_cfg.bit_count		= 16;
	ercd = FJ_AudioOut_Config( audio_id, &audio_out_cfg );
	UPRINTF_ERR(ercd);

	AudioCodecWrapper_PowerUp();
	AudioCodecWrapper_Play_Start();

	ercd = FJ_AudioOut_DirectPlayWav( audio_id, g_Boot_Wav_ID );
	UPRINTF_ERR(ercd);
}

static void bootup_music_play_hdmi(void)
{
	UCHAR 					audio_id = 0;
	FJ_ERR_CODE				ercd;
	T_FJ_AUDIO_OUT_CFG		audio_out_cfg;

	OS_User_Dly_Tsk(2000); // Tempoary to wait for hdmi stable

	ercd = FJ_AudioOut_Open( E_FJ_AUDIO_OUT_0, &audio_id );
	UPRINTF_ERR(ercd);

	audio_out_cfg.audio_out_port		= E_FJ_AUDIO_OUT_PORT_I2S3;
	audio_out_cfg.i2s_cfg.master_slave	= FJ_AUDIO_IF_MASTER;
	audio_out_cfg.i2s_cfg.lr_clock		= 48000;
	audio_out_cfg.i2s_cfg.data_cycle	= 64;
	audio_out_cfg.i2s_cfg.i2s_format	= E_FJ_AUDIO_I2S_Data_FORMAT_LEFT;
	audio_out_cfg.i2s_cfg.bit_count		= 16;
	ercd = FJ_AudioOut_Config( audio_id, &audio_out_cfg );
	UPRINTF_ERR(ercd);

	AudioCodecWrapper_PowerUp();
	AudioCodecWrapper_Play_Start();

	ercd = FJ_AudioOut_DirectPlayWav( audio_id, g_Boot_Wav_ID );
	UPRINTF_ERR(ercd);
}

static void mount_EMMC(void)
{
	FJ_ERR_CODE			ercd;
	FW_ULONG			media_id;
	FW_ULONG			part_num;
	FJ_EMMC_PARTITION	part_info[8] = { { 0 } };
	UCHAR				partition_check;
	UINT32				part_size;
	static CHAR*		drive_name[] = {
		D_USER_EM0_DRIVE,
		D_USER_EM1_DRIVE,
		D_USER_EM2_DRIVE,
		D_USER_EM3_DRIVE,
		NULL,		// Terminator
	};

	FJ_EMMC_Config(E_MEDIA_EMMC_ACCESS_MODE_HS_SDR); // temporary to avoid write fail issue
	
	// eMMC mount
	ercd = FJ_EMMC_Init();
	UPRINTF_ERR(ercd);

	ercd = FJ_EMMC_Device_Init();
	UPRINTF_ERR(ercd);

	ercd = FJ_EMMC_Partition_Read( part_info, &part_num );
	UPRINTF_ERR(ercd);
	
	// partition check
	partition_check = 0;
	if( part_num != D_EM_STORAGE_NUM ) {
		partition_check = 1;
	}
	else {
		for( media_id = 0; media_id < part_num; media_id++ ) {
			switch(media_id) {
				case 0 :
					part_size = D_EM_STORAGE_0_SIZE;
					break;
				case 1 :
					part_size = D_EM_STORAGE_1_SIZE;
					break;
				case 2 :
					part_size = D_EM_STORAGE_2_SIZE;
					break;
				case 3 :
					part_size = D_EM_STORAGE_3_SIZE;
					break;
				case 4 :
					part_size = D_EM_STORAGE_4_SIZE;
					break;
				case 5 :
					part_size = D_EM_STORAGE_5_SIZE;
					break;
				case 6 :
					part_size = D_EM_STORAGE_6_SIZE;
					break;
				default :
					part_size = 0;
					break;
			}
			if( part_info[media_id].part_size != part_size ) {
				partition_check = 1;
				break;
			}
		}
	}
	
	// Partition
	if( partition_check == 1 ) {
		UPRINTF("EMMC partition execute\n");

		ercd = FJ_EMMC_Partition();
		UPRINTF_ERR(ercd);
	
		ercd = FJ_EMMC_Partition_Read( part_info, &part_num );
		UPRINTF_ERR(ercd);
	}

	for( media_id = 0; media_id < part_num; media_id++ ) {
		if( drive_name[media_id] == NULL ) {
			continue;
		}
		ercd = FJ_Mount( (FJ_MEDIA_ID)(media_id + FJ_MEDIA_EMMC_STORAGE0), drive_name[media_id] );
		UPRINTF_ERR(ercd);
	}

}

static void mount_NAND(void)
{
	FJ_ERR_CODE				ercd;
	static FJ_NF_ATTRIBUTE	attr = {
		.ce_num					= 1,
		.max_bad_block_num		= 96,
		.device_block_num		= 4096,
		.bad_block_search_page	= 1,
		.drivability0			= E_MEDIA_NF_DRIVABILITY_1,
		.drivability1			= E_MEDIA_NF_DRIVABILITY_1,
		.drivability2			= E_MEDIA_NF_DRIVABILITY_1,
	};
	FJ_NF_PARTITION			part_info[8] = { { 0 } };
	ULONG					part_num;
	static CHAR*			nf_drive_name[] = {
		D_USER_NF0_DRIVE,
		D_USER_NF1_DRIVE,
		D_USER_NF2_DRIVE,
		D_USER_NF3_DRIVE,
		NULL,		// Terminator
	};
	UINT32					cur_part_size;
	UCHAR					partition_check;
	ULONG					media_id;
	
	// access device
	ercd = FJ_NF_Init();
	if( ercd ){
		UPRINTF_ERR(ercd);
		return;
	}
	
	ercd = FJ_NF_Config( &attr );
	if( ercd ){
		UPRINTF_ERR(ercd);
		return;
	}
	
	ercd = FJ_NF_Device_Init();
	if( ercd ){
		UPRINTF_ERR(ercd);
		return;
	}
	
	ercd = FJ_NF_Partition_Read( part_info, &part_num );
	if( ercd ){
		UPRINTF_ERR(ercd);
		return;
	}
	
	// partition check
	partition_check = 0;
	if( part_num != D_NF_STORAGE_NUM ) {
		partition_check = 1;
	}
	else {
		for( media_id = 0; media_id < part_num; media_id++ ) {
			switch(media_id) {
				case 0 :
					cur_part_size = D_NF_STORAGE_0_SIZE;
					break;
				case 1 :
					cur_part_size = D_NF_STORAGE_1_SIZE;
					break;
				case 2 :
					cur_part_size = D_NF_STORAGE_2_SIZE;
					break;
				case 3 :
					cur_part_size = D_NF_STORAGE_3_SIZE;
					break;
				case 4 :
					cur_part_size = D_NF_STORAGE_4_SIZE;
					break;
				case 5 :
					cur_part_size = D_NF_STORAGE_5_SIZE;
					break;
				case 6 :
					cur_part_size = D_NF_STORAGE_6_SIZE;
					break;
				default :
					cur_part_size = 0;
					break;
			}
			if( part_info[media_id].part_size != cur_part_size ) {
				partition_check = 1;
				break;
			}
		}
	}
	
	// Partition
	if( partition_check == 1 ) {
		UPRINTF("NF partition execute\n");
		ercd = FJ_NF_Partition();
		if( ercd ){
			UPRINTF_ERR(ercd);
			return;
		}
	
		ercd = FJ_NF_Partition_Read( part_info, &part_num );
		if( ercd ){
			UPRINTF_ERR(ercd);
			return;
		}
	
	}
	
	// Mount
	for( media_id = 0; media_id < part_num; media_id++ ) {
		if( nf_drive_name[media_id] == NULL ) {
			continue;
		}
		ercd = FJ_Mount( (FJ_MEDIA_ID)media_id, nf_drive_name[media_id] );
		UPRINTF_ERR(ercd);
	}

}


static void assign_resource(void)
{
	ULONG* addr;
	ULONG  size = 0;
	ULONG  mark = 0;
	
	addr = (ULONG*)SDRAM_ADR_HOST_RESOURCE_DATA_AREA;
	
	while(1)
	{
		// address check
		if( (ULONG)addr > (SDRAM_ADR_HOST_RESOURCE_DATA_AREA + SDRAM_SIZ_HOST_RESOURCE_DATA_AREA) ){
			break;
		}
		// end check
		if( mark == 0xEDEDEDED ){
			break;
		}
		// get mark and size
		mark = *(addr);
		size = *(addr+1);
		//UPRINTF("addr: 0x%x, mark 0x%x, size: 0x%x\n", (INT32)addr, (INT32)mark, (INT32)size );
		
		// assign
		switch( mark ){
		case 0xA1A2A3A4:	g_resource.BOOTUP_HDMI_addr = (addr+2);	g_resource.BOOTUP_HDMI_size = size;	break;
		case 0xC3C4C5C6:	g_resource.BOOTUP_addr     = (addr+2);	g_resource.BOOTUP_size     = size;	break;
		case 0xB3333435:	g_resource.IQ_BIN_addr[0] = (addr+2);	g_resource.IQ_BIN_size[0] = size;	break;
		case 0xB3363718:	g_resource.IQ_BIN_addr[1] = (addr+2);	g_resource.IQ_BIN_size[1] = size;	break;
		case 0xB3393A3B:	g_resource.IQ_BIN_addr[2] = (addr+2);	g_resource.IQ_BIN_size[2] = size;	break;
		case 0xB33C3D3E:	g_resource.IQ_BIN_addr[3] = (addr+2);	g_resource.IQ_BIN_size[3] = size;	break;
		case 0xB33F4041:	g_resource.IQ_BIN_addr[4] = (addr+2);	g_resource.IQ_BIN_size[4] = size;	break;
		case 0xB3424344:	g_resource.IQ_BIN_addr[5] = (addr+2);	g_resource.IQ_BIN_size[5] = size;	break;
		case 0xB3454647:	g_resource.IQ_BIN_addr[6] = (addr+2);	g_resource.IQ_BIN_size[6] = size;	break;
		case 0xB348494A:	g_resource.IQ_BIN_addr[7] = (addr+2);	g_resource.IQ_BIN_size[7] = size;	break;
		case 0xB34B4C4D:	g_resource.IQ_BIN_addr[8] = (addr+2);	g_resource.IQ_BIN_size[8] = size;	break;
		case 0xB5505152:	g_resource.EVP_MAP_addr   = (addr+2);	g_resource.EVP_MAP_size   = size;	break;
		case 0xB2202122:	g_resource.FSHD_BIN_addr[0] = (addr+2);	g_resource.FSHD_BIN_size[0] = size;	break;
		case 0xB2232425:	g_resource.FSHD_BIN_addr[1] = (addr+2);	g_resource.FSHD_BIN_size[1] = size;	break;
		case 0xB2262728:	g_resource.FSHD_BIN_addr[2] = (addr+2);	g_resource.FSHD_BIN_size[2] = size;	break;
		case 0xB2292A2B:	g_resource.FSHD_BIN_addr[3] = (addr+2);	g_resource.FSHD_BIN_size[3] = size;	break;
		case 0xB22C2D2E:	g_resource.FSHD_BIN_addr[4] = (addr+2);	g_resource.FSHD_BIN_size[4] = size;	break;
		case 0xB22F3031:	g_resource.FSHD_BIN_addr[5] = (addr+2);	g_resource.FSHD_BIN_size[5] = size;	break;
		case 0xB2323334:	g_resource.FSHD_BIN_addr[6] = (addr+2);	g_resource.FSHD_BIN_size[6] = size;	break;
		case 0xB2353637:	g_resource.FSHD_BIN_addr[7] = (addr+2);	g_resource.FSHD_BIN_size[7] = size;	break;
		case 0xB238393A:	g_resource.FSHD_BIN_addr[8] = (addr+2);	g_resource.FSHD_BIN_size[8] = size;	break;
		case 0xB7707172:	g_resource.CCM1_addr      	= (addr+2);	g_resource.CCM1_size      	= size;	break;
		case 0xB8808182:	g_resource.RGB_BIN_addr		= (addr+2);	g_resource.RGB_BIN_size		= size;	break;
		case 0xB4434445:	g_resource.LDC_BIN_addr[0]	= (addr+2);	g_resource.LDC_BIN_size[0]	= size;	break;
		case 0xB4464748:	g_resource.LDC_BIN_addr[1]	= (addr+2);	g_resource.LDC_BIN_size[1]	= size;	break;
		case 0xB4494A4B:	g_resource.LDC_BIN_addr[2]	= (addr+2);	g_resource.LDC_BIN_size[2]	= size;	break;
		case 0xB4434455:	g_resource.LDC_BIN_addr[3]	= (addr+2);	g_resource.LDC_BIN_size[3]	= size;	break;
		case 0xB4464768:	g_resource.LDC_BIN_addr[4]	= (addr+2);	g_resource.LDC_BIN_size[4]	= size;	break;
		case 0xB4494A7B:	g_resource.LDC_BIN_addr[5]	= (addr+2);	g_resource.LDC_BIN_size[5]	= size;	break;
		case 0xB4434485:	g_resource.LDC_BIN_addr[6]	= (addr+2);	g_resource.LDC_BIN_size[6]	= size;	break;
		case 0xB4464798:	g_resource.LDC_BIN_addr[7]	= (addr+2);	g_resource.LDC_BIN_size[7]	= size;	break;
		case 0xB44C4D4E:	g_resource.WARP_L_BIN_addr	= (addr+2);	g_resource.WARP_L_BIN_size	= size;	break;
		case 0xB44F5051:	g_resource.WARP_R_BIN_addr	= (addr+2);	g_resource.WARP_R_BIN_size	= size;	break;
		case 0xB4525354:	g_resource.CAM_CAL_BIN_addr[0] = (addr+2);	g_resource.CAM_CAL_BIN_size[0] = size;	break;
		case 0xB4555657:	g_resource.CAM_CAL_BIN_addr[1] = (addr+2);	g_resource.CAM_CAL_BIN_size[1] = size;	break;
		case 0xB9909192:	g_resource.BADPIX_addr[0]	= (addr+2);	g_resource.BADPIX_size[0]	= size;	break;
		case 0xB9939495:	g_resource.BADPIX_addr[1]	= (addr+2);	g_resource.BADPIX_size[1]	= size;	break;
		case 0xB9969798:	g_resource.BADPIX_addr[2]	= (addr+2);	g_resource.BADPIX_size[2]	= size;	break;
		case 0xB9999A9B:	g_resource.BADPIX_addr[3]	= (addr+2);	g_resource.BADPIX_size[3]	= size;	break;
		case 0xB6000102:	g_resource.GAMFL_addr[0]	= (addr+2);	g_resource.GAMFL_size[0]	= size;	break;
		case 0xB6030405:	g_resource.GAMFL_addr[1]	= (addr+2);	g_resource.GAMFL_size[1]	= size;	break;
		case 0xB6060708:	g_resource.GAMFL_addr[2]	= (addr+2);	g_resource.GAMFL_size[2]	= size;	break;
		case 0xB6090a0b:	g_resource.GAMFL_addr[3]	= (addr+2);	g_resource.GAMFL_size[3]	= size;	break;
		case 0xB60c0d0e:	g_resource.GAMFL_addr[4]	= (addr+2);	g_resource.GAMFL_size[4]	= size;	break;
		case 0xB60f1011:	g_resource.GAMFL_addr[5]	= (addr+2);	g_resource.GAMFL_size[5]	= size;	break;
		case 0xB6121314:	g_resource.GAMFL_addr[6]	= (addr+2);	g_resource.GAMFL_size[6]	= size;	break;
		case 0xB6151617:	g_resource.GAMFL_addr[7]	= (addr+2);	g_resource.GAMFL_size[7]	= size;	break;
		case 0xB618191a:	g_resource.GAMDF_addr[0]	= (addr+2);	g_resource.GAMDF_size[0]	= size;	break;
		case 0xB61b1c1d:	g_resource.GAMDF_addr[1]	= (addr+2);	g_resource.GAMDF_size[1]	= size;	break;
		case 0xB61e1f20:	g_resource.GAMDF_addr[2]	= (addr+2);	g_resource.GAMDF_size[2]	= size;	break;
		case 0xB6212223:	g_resource.GAMDF_addr[3]	= (addr+2);	g_resource.GAMDF_size[3]	= size;	break;
		case 0xB6242526:	g_resource.GAMDF_addr[4]	= (addr+2);	g_resource.GAMDF_size[4]	= size;	break;
		case 0xB6272829:	g_resource.GAMDF_addr[5]	= (addr+2);	g_resource.GAMDF_size[5]	= size;	break;
		case 0xB62a2b2c:	g_resource.GAMDF_addr[6]	= (addr+2);	g_resource.GAMDF_size[6]	= size;	break;
		case 0xB62d2e2f:	g_resource.GAMDF_addr[7]	= (addr+2);	g_resource.GAMDF_size[7]	= size;	break;
		default:																						break;
		}
		
		// update
		addr += ( ( (size + 8 + 15 ) & 0xFFFFFFF0 ) / 4 );
	}
	
}

static void start_timer64_gyro(void)
{
#ifdef TIMER64_START_FROM_BOOT
	INT32			ret;
	T_DD_TMR64_CTRL	tmr64_ctrl;
	FJ_Top_Set_Clock_Select_GYRO( FJ_TOP_GYRO_CLKSEL_2MHZ ); 
	
	// Open TMR64
	ret = Dd_TMR64_Open( D_DDIM_USER_SEM_WAIT_POL );
	if( ret != D_DDIM_OK ) {
		printf("FJ_GyroIF: Dd_TMR64_Open Error!! : line=%d, ret=0x%08x\n", __LINE__, ret);
		//return FJ_ERR_NG;
	}

	// Ctrl TMR64
	tmr64_ctrl.csr1.word	= 0;
	tmr64_ctrl.csr1.bit.csl	= FJ_GYROIF_TMR_CLK_DIV_2;
	tmr64_ctrl.csr2.word	= 0;

	ret = Dd_TMR64_Ctrl( &tmr64_ctrl );
	if( ret != D_DDIM_OK ) {
		printf("FJ_GyroIF: Dd_TMR64_Ctrl Error!! : line=%d, ret=0x%08x\n", __LINE__, ret);
		Dd_TMR64_Close();
		//return FJ_ERR_NG;
	}

	// Start TMR64
	ret = Dd_TMR64_Start( FALSE );
	if( ret != D_DDIM_OK ) {
		printf("FJ_GyroIF: Dd_TMR64_Start Error!! : line=%d, ret=0x%08x\n", __LINE__, ret);
		Dd_TMR64_Close();
		//return FJ_ERR_NG;
	}

	//return FJ_ERR_OK;
#endif
}

