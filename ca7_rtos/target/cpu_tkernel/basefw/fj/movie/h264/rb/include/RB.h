/**
 * @file		RB.h
 * @brief		RIBERY Function Driver header file
 * @author		K.Oonuma (FJ)
 * @since		2010/4/20
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _RB_H_
#define _RB_H_


#include "sdram_map_movie_record_pattern0_define.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// [Master Definition] *compile option
#define CO_RB_SECTION_ON						// arm section
#define CO_RB_ACT_CLOCK							// AHB clock Control
//#define CO_RB_7_5FPS							// 7.5fps support
#define CO_RB_12_5FPS							// 12.5fps support
//#define CO_RB_3DNR_ENABLE						// 3DNR enable
#define CO_RB_ERROR_INFO_ON						// Error Infomation enable
//#define CO_RB_CBR_FLAG_ON						// SPS Syntax cbr_flag value
//#define CO_RB_THIN_OUT_FRAME_NUM_ON				// THIN_OUT_FRAME_NUM value
//#define CO_V_Q_DFVLD_ON							// DeblockingFilter ON
#define CO_RB_VUI_ON							// VUI param set ON
#define CO_RB_VUI_CHROMA_LOC_INFO_ON			// Chroma_Loc_Info in VUI ON
#define CO_RB_SCALINGLIST_ON					// Scalinglist param set ON
#define CO_RB_PARTICAL_CODING_ON				// PARTICAL_CODING set ON
#define CO_RB_SEARCH_RANGE_ON					// SEARCH RANGE ON
#define CO_RB_SLICE_ON							// SLICE ON
#define CO_RB_CHG_BITRATE_MODE_ON				// CHG_BITRATE_MODE ON
#define CO_IMAGE_QUALITY_MODE_ON				// Image Quality mode ON
#define CO_MB_LIGHT_MODE						// MB Light mode
#define CO_FIXQP_EN_ON							// fix qp en ON

// •K‚¸1‚Â‘I‘ð‚µ‚Ä‚­‚¾‚³‚¢
//#define CO_MULTI_STREAM_1CH_ON				// multi stream 1ch ON
//#define CO_MULTI_STREAM_2CH_ON				// multi stream 2ch ON
#define CO_MULTI_STREAM_3CH_ON					// multi stream 3ch ON
//#define CO_MULTI_STREAM_4CH_ON				// multi stream 4ch ON

//#define CO_RB_DEBUG								// for debug (emulation test root)
//#define CO_RB_SELFMODE						// for debug (selfmode test root)
#define CO_RB_PROCESSCUT_OFF					// PROCESS CUT Mode OFF
//#define CO_RB_PROCESSCUT_DEBUG				// for debug (PROCESS CUT Mode)

//#define CO_RB_INFO_PRINT						// for debug log
//#define CO_RB_MAP_PRINT							// for debug log (mapping address log)
//#define CO_RB_EMU_PRINT							// for debug log (emulation test  log)

//----------------------
// return value
//----------------------
#define RB_ON						(1)			// ON
#define RB_OFF						(0)			// OFF

#define RB_COMPLETE					( 1)		// Normal end & complete
#define RB_OK						( 0)		// Normal end
#define RB_INVALID_PARAM			(-1)		// Input parameter error
#define RB_NG						(-2)		// Processing NG
#define RB_TIMEOUT					(-3)		// Time Out

//----------------------
// COMMON
//----------------------
#define RB_CODEC_ENC				(0)			// Codec Type : ENCODE
#define RB_CODEC_DEC				(1)			// Codec Type : DECODE
#define RB_CODEC_TRANSCODE			(2)			// Codec Type : TRANSCODE

#define RB_SDRAM_CH0				(0)			// SDRAM ch select : ch0
#define RB_SDRAM_CH1				(1)			// SDRAM ch select : ch1

#define RB_SDRAM_IF_CHA				(0)			// SDRAM_IF ch select : ch A
#define RB_SDRAM_IF_CHB				(1)			// SDRAM_IF ch select : ch B
#define RB_SDRAM_IF_CHC				(2)			// SDRAM_IF ch select : ch C

#ifdef CO_MULTI_STREAM_1CH_ON
#define RB_MULTI_ENC_CH_NUM			(1)			// MULTI Encoding ch
#define RB_MULTI_ENC_CH_0			(0)			// MULTI Encoding : ch0
#endif
#ifdef CO_MULTI_STREAM_2CH_ON
#define RB_MULTI_ENC_CH_NUM			(2)			// MULTI Encoding ch
#define RB_MULTI_ENC_CH_0			(0)			// MULTI Encoding : ch0
#define RB_MULTI_ENC_CH_1			(1)			// MULTI Encoding : ch1
#endif
#ifdef CO_MULTI_STREAM_3CH_ON
#define RB_MULTI_ENC_CH_NUM			(3)			// MULTI Encoding ch
#define RB_MULTI_ENC_CH_0			(0)			// MULTI Encoding : ch0
#define RB_MULTI_ENC_CH_1			(1)			// MULTI Encoding : ch1
#define RB_MULTI_ENC_CH_2			(2)			// MULTI Encoding : ch2
#endif
#ifdef CO_MULTI_STREAM_4CH_ON
#define RB_MULTI_ENC_CH_NUM			(4)			// MULTI Encoding ch
#define RB_MULTI_ENC_CH_0			(0)			// MULTI Encoding : ch0
#define RB_MULTI_ENC_CH_1			(1)			// MULTI Encoding : ch1
#define RB_MULTI_ENC_CH_2			(2)			// MULTI Encoding : ch2
#define RB_MULTI_ENC_CH_3			(3)			// MULTI Encoding : ch3
#endif

#define RB_CORE_MODE_1				(1)			// use core_1
#define RB_CORE_MODE_2				(2)			// use core_2

#define RB_SIZE_FHD					(0)			// Size : FULL HD      1920x1080
#define RB_SIZE_1440				(1)			// Size : 1440         1440x1080
#define RB_SIZE_1440P				(16)		// Size : 1440p        1920x1440
#define RB_SIZE_HD					(2)			// Size : HD           1280x 720
#define RB_SIZE_SD_NTSC				(3)			// Size : SD-NTSC       720x 480
#define RB_SIZE_SD_PAL				(4)			// Size : SD-PAL        720x 576
#define RB_SIZE_VGA					(5)			// Size : VGA           640x 480
#define RB_SIZE_QVGA				(6)			// Size : QVGA          320x 240
#define RB_SIZE_SXVGA				(8)			// Size : SXVGA        1280x 960
#define RB_SIZE_HVGAW				(9)			// Size : HVGAW         640x 360
#define RB_SIZE_FWVGA				(17)		// Size : FWVGA         848x 480
#define RB_SIZE_FWQVGA				(18)		// Size : FWQVGA        432x 240
#define RB_SIZE_4K2K_16_9			(10)		// Size : 4K2K(16:9)   4096x2304
#define RB_SIZE_4K2K_1_9_1			(11)		// Size : 4K2K(1.90:1) 4096x2160
#define RB_SIZE_4K2K_2_1			(12)		// Size : 4K2K(2:1)    4096x2048
#define RB_SIZE_4K2K_QFHD			(13)		// Size : 4K2K(QFHD)   3840x2160
#define RB_SIZE_2_7K_4_3			(14)		// Size : 2.7K(4:3)    2704x2028
#define RB_SIZE_2_7K_16_9			(15)		// Size : 2.7K(16:9)   2704x1520
#define RB_SIZE_2704x2160			(33)		// Size :              2704x2160
#define RB_SIZE_2880x1524			(20)		// Size :              2880x1524
#define RB_SIZE_4000x2000			(21)		// Size :              4000x2000
#define RB_SIZE_3072x1536			(22)		// Size :              3072x1536
#define RB_SIZE_2816x2112			(23)		// Size :              2816x2112
#define RB_SIZE_2048x1536			(24)		// Size :              2048x1536
#define RB_SIZE_2048x1024			(25)		// Size :              2048x1024
#define RB_SIZE_1024x512			(26)		// Size :              1024x512
#define RB_SIZE_2_5K_4_3			(27)		// Size : 2.5K(4:3)    2560x1920
#define RB_SIZE_1_5K_1_1			(28)		// Size : 1.5K(1:1)    1500x1500
#define RB_SIZE_3K_1_1				(29)		// Size : 3K(1:1)      3000x3000
#define RB_SIZE_1440x720			(30)		// Size :              1440x 720
#define RB_SIZE_960x480				(31)		// Size :               960x 480
#define RB_SIZE_864x480				(32)		// Size :               864x 480
#define RB_SIZE_1920x960			(34)		// Size :              1920x 960
#define RB_SIZE_2560x1600			(35)		// Size :              2560x1600
#define RB_SIZE_2560x720			(36)		// Size :              2570x720
#define RB_SIZE_3072x1728			(37)		// Size :              3072x1728
#define RB_SIZE_3200x2400			(38)		// Size :              3200x2400
#define RB_SIZE_1600x1200			(39)		// Size :              1600x1200
#define RB_SIZE_1024x768			(40)		// Size :              1024x768
#define RB_SIZE_2304x1296			(41)		// Size :              2304x1296
#define RB_SIZE_1296x2304			(42)		// Size :              1296x2304
#define RB_SIZE_1080x1920			(43)		// Size :              1080x1920
#define RB_SIZE_720x1280			(44)		// Size :               720x1280
#define RB_SIZE_360x640				(45)		// Size :               360x 640
#define RB_SIZE_320x180				(46)		// Size :               320x 180
#define RB_SIZE_3840x1920			(47)		// Size :              3840x1920
#define RB_SIZE_1280x640			(48)		// Size :              1280x 640
#define RB_SIZE_640x320				(49)		// Size :               640x 320
#define RB_SIZE_960x720				(50)		// Size :               960x 720
#define RB_SIZE_1632x1088			(51)		// Size :              1632x1088
#define RB_SIZE_1920x1280			(52)		// Size :              1920x1280
#define RB_SIZE_2880x1440			(53)		// Size :              2880x1440
#define RB_SIZE_1408x704			(54)		// Size :              1408x 704
#define RB_SIZE_960x1080			(55)		// Size :               960x1080
#define RB_SIZE_1920x540			(56)		// Size :              1920x 540
#define RB_SIZE_1280x1024			(57)		// Size :              1280x1024
#define RB_SIZE_1280x768			(58)		// Size :              1280x 768
#define RB_SIZE_800x600				(59)		// Size :               800x 540
#define RB_SIZE_2272x1472			(60)		// Size :              2272x1472
#define RB_SIZE_2560x1440			(61)		// Size :              2560x1440
#define RB_SIZE_960x540				(62)		// Size :               960x 540
#define RB_SIZE_480x270				(63)		// Size :               480x 270
#define RB_SIZE_1600x896			(64)		// Size :              1600x 896
#define RB_SIZE_1024x576			(65)		// Size :              1024x 576
#define RB_SIZE_800x448				(66)		// Size :               800x 448
#define RB_SIZE_352x288				(67)		// Size :               352x 288
#define RB_SIZE_2592x1944			(68)		// Size :              2592x1944
#define RB_SIZE_1920_1280			(69)		// Size :              1920x1280
#define RB_SIZE_640_512				(70)		// Size :               640x 512
#define RB_SIZE_384_288				(71)		// Size :               384x 288
												
												// [Frame Rate : NTSC     (1000/1001) ]
#define RB_FR_239_76FPS				(22)		//   - 240fps  (239.76 = 240000/1001)
#define RB_FR_199_80FPS				(34)		//   - 200fps  (199.80 = 200000/1001)
#define RB_FR_119_88FPS				(23)		//   - 120fps  (119.88 = 120000/1001)
#define RB_FR_99_90FPS				(35)		//   - 100fps  ( 99.90 = 100000/1001)
#define RB_FR_89_91FPS				(24)		//   - 90fps   ( 89.91 =  90000/1001)
#define RB_FR_79_92FPS				(25)		//   - 80fps   ( 79.92 =  80000/1001)
#define RB_FR_59_94FPS				(1)			//   - 60fps   ( 59.94 =  60000/1001)
#define RB_FR_47_95FPS				(26)		//   - 48fps   ( 47.95 =  48000/1001)
#define RB_FR_29_97FPS				(2)			//   - 30fps   ( 29.97 =  30000/1001)
#define RB_FR_23_97FPS				(0)			//   - 24fps   ( 23.97 =  24000/1001)
#define RB_FR_19_98FPS				(40)		//   - 20fps   ( 19.98 =  20000/1001)
#define RB_FR_14_98FPS				(3)			//   - 15fps   ( 14.98 =  15000/1001)
#define RB_FR_11_98FPS				(38)		//   - 12fps   ( 11.98 =  12000/1001)
#define RB_FR_9_99FPS				(12)		//   - 10fps   (  9.99 =  10000/1001)
#define RB_FR_4_99FPS				(13)		//   -  5fps   (  4.99 =   5000/1001)
#define RB_FR_2_99FPS				(14)		//   -  3fps   (  2.99 =   3000/1001)
#define RB_FR_1_99FPS				(15)		//   -  2fps   (  1.99 =   2000/1001)
#define RB_FR_0_99FPS				(16)		//   -  1fps   (  0.99 =   1000/1001)
												// [Frame Rate : Special ]
#define RB_FR_240_0FPS				(27)		//   - 240fps  (240.0  = 240/1)
#define RB_FR_200_0FPS				(36)		//   - 200fps  (200.0  = 200/1)
#define RB_FR_120_0FPS				(28)		//   - 120fps  (120.0  = 120/1)
#define RB_FR_100_0FPS				(37)		//   - 100fps  (100.0  = 100/1)
#define RB_FR_90_0FPS				(29)		//   - 90fps   ( 90.0  =  90/1)
#define RB_FR_80_0FPS				(30)		//   - 80fps   ( 80.0  =  80/1)
#define RB_FR_60_0FPS				(5)			//   - 60fps   ( 60.0  =  60/1)
#define RB_FR_48_0FPS				(31)		//   - 48fps   ( 48.0  =  48/1)
#define RB_FR_30_0FPS				(6)			//   - 30fps   ( 30.0  =  30/1)
#define RB_FR_24_0FPS				(4)			//   - 24fps   ( 24.0  =  24/1)
#define RB_FR_20_0FPS				(41)		//   - 20fps   ( 20.0  =  20/1)
#define RB_FR_15_0FPS				(7)			//   - 15fps   ( 15.0  =  15/1)
#define RB_FR_12_0FPS				(39)		//   - 12fps   ( 12.0  =  12/1)
#define RB_FR_10_0FPS				(17)		//   - 10fps   ( 10.0  =  10/1)
#define RB_FR_5_0FPS				(18)		//   -  5fps   (  5.0  =   5/1)
#define RB_FR_3_0FPS				(19)		//   -  3fps   (  3.0  =   3/1)
#define RB_FR_2_0FPS				(20)		//   -  2fps   (  2.0  =   2/1)
#define RB_FR_1_0FPS				(21)		//   -  1fps   (  1.0  =   1/1)
												// [Frame Rate : PAL ]
#define RB_FR_50_0FPS				(8)			//   - 50fps   ( 50.0  =  50/1)
#define RB_FR_25_0FPS				(9)			//   - 25fps   ( 25.0  =  25/1)
#ifdef CO_RB_7_5FPS
#define RB_FR_7_5FPS				(10)		//   - 7.5fps  (  7.5  =  75/10)
#endif
#ifdef CO_RB_12_5FPS
#define RB_FR_12_5FPS				(11)		//   - 12.5fps ( 12.5  = 125/10)
#endif

//----------------------
// INIT_PARAM
//----------------------
#define RB_GOP_STR_IBBP				(0)			// GOP stracture type : "IBBP.."  
#define RB_GOP_STR_IPPP				(1)			// GOP stracture type : "IPPP.."  
#define RB_GOP_STR_IBPB				(3)			// GOP stracture type : "IBPB.."
//#define RB_GOP_STR_IIII			(3)			// GOP stracture type : "IIII.."	*not support

#define RB_RATE_MODE_CBR			(0)			// RATE MODE type : CBR
#define RB_RATE_MODE_VBR			(1)			// RATE MODE type : VBR
#define RB_RATE_MODE_CONSTANT_QP	(2)			// RATE MODE type : CONSTANT QP

#define RB_ENC_MODE_MP4				(0)			// enc mode : mp4
#define RB_ENC_MODE_TS				(1)			// enc mode : ts

#define RB_CLOSED_GOP_ON			(0)			// Closed GOP type : ClosedGOP
#define RB_CLOSED_GOP_OFF			(1)			// Closed GOP type : OpenGOP

#define RB_PROFILE_HIGH					(0)		// PROFILE type : High Profile
#define RB_PROFILE_MAIN					(1)		// PROFILE type : Main Profile
#define RB_PROFILE_CONSTRAINED_BASELINE	(2)		// PROFILE type : Constrained Baseline Profile

#define RB_VLC_MODE_CABAC			(0)			// VLC MODE type : CABAC
#define RB_VLC_MODE_CAVLC			(1)			// VLC MODE type : CAVLC

#define RB_SCAN_MODE_PROGRESSIVE	(0)			// SCAN MODE type : Progressive
#define RB_SCAN_MODE_INTERLACE		(1)			// SCAN MODE type : Interlace

#ifdef CO_RB_SEARCH_RANGE_ON
#define RB_SEARCH_RANGE_SEL_MODE0	(0)			// SEARCH_RANGE select : MODE0
#define RB_SEARCH_RANGE_SEL_MODE1	(1)			// SEARCH_RANGE select : MODE1
#define RB_SEARCH_RANGE_SEL_MODE2	(2)			// SEARCH_RANGE select : MODE2
#define RB_SEARCH_RANGE_SEL_MODE3	(3)			// SEARCH_RANGE select : MODE3
#endif

#ifdef CO_RB_SLICE_ON
#define RB_SLICE_ENC_NO_DIV			(0)			// SLICE NO DIV
#define RB_SLICE_ENC_4_DIV			(4)			// SLICE 4  DIV
#endif

#ifdef CO_IMAGE_QUALITY_MODE_ON
#define RB_IMAGE_QUALITY_MODE0		(0)			// Image Quality mode 0 : default
#define RB_IMAGE_QUALITY_MODE1		(1)			// Image Quality mode 1 : mode1

#define RB_INTRA_WEIGHT_3			(3)
#define RB_INTRA_WEIGHT_2			(2)
#define RB_INTRA_WEIGHT_1			(1)
#define RB_DEFAULT					(0)
#define RB_INTER_WEIGHT_1			(-1)
#define RB_INTER_WEIGHT_2			(-2)
#define RB_INTER_WEIGHT_3			(-3)
#endif

//----------------------
// INT
//----------------------
// to Ribery (request)										   bit/priority
#define RB_INT_REQUEST_START					(0x0100)	//	 8	2	Encode/Decord start
#define RB_INT_REQUEST_RESET					(0x0200)	//	 9	1	reset ready
#define RB_INT_REQUEST_STOP						(0x0400)	//	10	2	Encode/Decord stop
#define RB_INT_REQUEST_FREE						(0x1000)	//	12	4	VSTRM/VDPB FIFO
#define RB_INT_REQUEST_REQ						(0x2000)	//	13	4	VRAW /VCPB FIFO

#define RB_INT_REQUEST_FIFO_VRAW_ENCREQ_SET		(0x0001)
#define RB_INT_REQUEST_FIFO_VSTRM_FREE_SET		(0x0008)
#define RB_INT_REQUEST_FIFO_VRAW_FREE_GET		(0x0002)
#define RB_INT_REQUEST_FIFO_VSTRM_ENCDONE_GET	(0x0004)

#define RB_INT_REQUEST_FIFO_VCPB_DECREQ_SET		(0x0001)
#define RB_INT_REQUEST_FIFO_VDPB_FREE_SET		(0x0008)
#define RB_INT_REQUEST_FIFO_VCPB_FREE_GET		(0x0002)
#define RB_INT_REQUEST_FIFO_VDPB_DECDONE_GET	(0x0004)



// from Ribery (status:response/request)
#define RB_INT_STATUS_START_END					(0x0100)	//	 8	2	complete Encode/Decord start
#define RB_INT_STATUS_RESET_END					(0x0200)	//	 9	1	complete reset ready
#define RB_INT_STATUS_STOP_END					(0x0400)	//	10	2	complete Encode/Decord stop
#define RB_INT_STATUS_BOOT_END					(0x0800)	//	11	-	complete boot
#define RB_INT_STATUS_DONE						(0x1000)	//	12	4	VSTRM/VDPB FIFO
#define RB_INT_STATUS_FREE						(0x2000)	//	13	4	VRAW /VCPB FIFO
#define RB_INT_STATUS_Y_VEC						(0x0010)	//	 4	-	Y_VEC FIFO

#define RB_INT_STATUS_FIFO_VRAW_ENCREQ_SET		(0x0001)
#define RB_INT_STATUS_FIFO_VSTRM_FREE_SET		(0x0008)
#define RB_INT_STATUS_FIFO_VRAW_FREE_GET		(0x0002)
#define RB_INT_STATUS_FIFO_VSTRM_ENCDONE_GET	(0x0004)

#define RB_INT_STATUS_FIFO_VCPB_DECREQ_SET		(0x0001)
#define RB_INT_STATUS_FIFO_VDPB_FREE_SET		(0x0008)
#define RB_INT_STATUS_FIFO_VCPB_FREE_GET		(0x0002)
#define RB_INT_STATUS_FIFO_VDPB_DECDONE_GET		(0x0004)


// ERROR
#define RB_INT_STATUS_ERROR						(0x0100)	//	 1 		error: notification to MainCPU

//----------------------
// INFO_TABLE
//----------------------
#define RB_PIC_IDR					(0)			// Picture Type : IDR
#define RB_PIC_I					(1)			// Picture Type : I
#define RB_PIC_P					(2)			// Picture Type : P
#define RB_PIC_B					(3)			// Picture Type : B

// Encode
#define RB_VRAW_INFO_TABLE_NUM		(54)		// VRAW_INFO_TABLE     bank num
#define RB_VSTRM_INFO_TABLE_NUM		(288)		// VSTRM_INFO_TABLE    bank num
#define RB_Y_VEC_INFO_TABLE_NUM		(64)		// Y_VEC_INFO_TABLE    bank num
#define RB_NAL_UNIT_SIZE_TBL_NUM	(11)		// NAL UNIT SIZE TABLE NUM

#ifdef CO_RB_3DNR_ENABLE
#define RB_3DNR_OFF					(0)			// 3DNR level : OFF
#define RB_3DNR_LEVEL_1				(1)			// 3DNR level : 1
#define RB_3DNR_LEVEL_2				(2)			// 3DNR level : 2
#define RB_3DNR_LEVEL_3				(3)			// 3DNR level : 3
#define RB_3DNR_LEVEL_4				(4)			// 3DNR level : 4
#define RB_3DNR_LEVEL_5				(5)			// 3DNR level : 5
#define RB_3DNR_LEVEL_6				(6)			// 3DNR level : 6
#define RB_3DNR_LEVEL_7				(7)			// 3DNR level : 7
#define RB_3DNR_LEVEL_8				(8)			// 3DNR level : 8
#define RB_3DNR_LEVEL_9				(9)			// 3DNR level : 9
#define RB_3DNR_LEVEL_10			(10)		// 3DNR level : 10
#define RB_3DNR_LEVEL_11			(11)		// 3DNR level : 11
#define RB_3DNR_LEVEL_12			(12)		// 3DNR level : 12
#define RB_3DNR_LEVEL_13			(13)		// 3DNR level : 13
#define RB_3DNR_LEVEL_14			(14)		// 3DNR level : 14
#endif

// Decode
#define RB_VCPB_INFO_TABLE_NUM		(32)		// VCPB_INFO_TABLE     bank num (32)
#define RB_VDPB_INFO_TABLE_NUM		(256)		// VDPB_INFO_TABLE     bank num (256)

//----------------------
// FIFO MAX
//----------------------
#define RB_FIFO_MAX_COUNT_NUM		(4)			// FIFO Entry Max Count

#ifdef CO_RB_ERROR_INFO_ON
//----------------------
// Error Info Table MAX
//----------------------
#define RB_ERRORINFO_COUNT_NUM		(40)		// Error Info Entry Max Count
#endif

/*----------------------------------------------------------------------*/
/* MAP	*ribery use														*/
/*----------------------------------------------------------------------*/
//----------------------
// size
//----------------------
/* --- enc:work0  --- */
#define RB_MAP_SIZ_ENC_WORK0				(0x00100000)	//  1[M]

// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#ifdef CO_MEMORY_1GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_1GB BEGIN ---
/* --- enc:work1  --- */
#define RB_MAP_SIZ_ENC_WORK1				(0x02200000)	// 34[M]
// --- REMOVE_MEMORY_1GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_1GB
#ifdef CO_MEMORY_2GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_2GB BEGIN ---
/* --- enc:work1  --- */
#define RB_MAP_SIZ_ENC_WORK1				(0x03300000)	// 51[M]
// --- REMOVE_MEMORY_2GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_2GB
#ifdef CO_MEMORY_3GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_3GB BEGIN ---
/* --- enc:work1  --- */
#define RB_MAP_SIZ_ENC_WORK1				(0x03300000)	// 51[M]
// --- REMOVE_MEMORY_3GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_3GB
// --- REMOVE_MEMORY_SIZE_OPT END ---

/* --- enc:work2  --- */
#define RB_MAP_SIZ_ENC_WORK2				(0x01700000)	// 23[M]

/* --- dec:work0  --- */
#define RB_MAP_SIZ_DEC_WORK0				(0x00C00000)	// 12[M]

/* --- dec:work1  --- */
#define RB_MAP_SIZ_DEC_WORK1				(0x01700000)	// 23[M]

/* --- dec:work2  --- */
#define RB_MAP_SIZ_DEC_WORK2				(0x01700000)	// 23[M]

/* --- stream --- */
#define RB_MAP_SIZ_ENC_STREAM_OFFSET		(0x00000008)	//  8byte
#define RB_MAP_SIZ_DEC_STREAM_OFFSET 		(0x00000008)	//  8byte

/* --- YCC --- */
#define RB_MAP_BNK_ENC_YCC					(SDRAM_BNK_MOVIE_RECORD0_RIBERY_YC)			// default bank num
#define RB_MAP_BNK_DEC_YCC					(20)			// default bank num

/* --- Localdec(enc only) --- */
#define RB_MAP_BNK_ENC_LOCALDEC_YCC			(2)				// default bank num

/* --- reduction(enc only) --- */
#define RB_MAP_BNK_ENC_REDUCTION_YCC		(SDRAM_BNK_MOVIE_RECORD0_RIBERY_YC_REDUCTION)				// default bank num

/* --- local decode bank/yc bank size --- */
/* * free bank size support
   * Reference size
   * width           : align 128
   * y_lines,c_lines : align 32
   *   (width*y_lines) = y bank size(16KB align)
   *   (width*c_lines) = c bank size(16KB align)
   * ** select tb_format(tb_flg on)
   * ** width/2             : align 128
   * ** y_lines/2,c_lines/2 : align 32
*/
#if 0
// FHD (1920x1080)
#define RB_MAP_SIZE_FHD_WIDTH			1920
#define RB_MAP_SIZE_FHD_Y_LINES			1088
#define RB_MAP_SIZE_FHD_C_LINES			544
#define RB_MAP_SIZE_FHD_Y_BANK			(0x00200000)
#define RB_MAP_SIZE_FHD_C_BANK			(0x00100000)
// 1440(1440x1080)
#define RB_MAP_SIZE_1440_WIDTH			1536
#define RB_MAP_SIZE_1440_Y_LINES		1088
#define RB_MAP_SIZE_1440_C_LINES		544
#define RB_MAP_SIZE_1440_Y_BANK			(0x00198000)
#define RB_MAP_SIZE_1440_C_BANK			(0x000CC000)
// 1440P(1920x1440)
#define RB_MAP_SIZE_1440P_WIDTH			1920
#define RB_MAP_SIZE_1440P_Y_LINES		1440
#define RB_MAP_SIZE_1440P_C_LINES		736
#define RB_MAP_SIZE_1440P_Y_BANK		(0x002A4000)
#define RB_MAP_SIZE_1440P_C_BANK		(0x0015C000)
// HD(1280x720)
#define RB_MAP_SIZE_HD_WIDTH			1280
#define RB_MAP_SIZE_HD_Y_LINES			736
#define RB_MAP_SIZE_HD_C_LINES			384
#define RB_MAP_SIZE_HD_Y_BANK			(0x000E8000)
#define RB_MAP_SIZE_HD_C_BANK			(0x00078000)
// SD-NTSC(720x480)
#define RB_MAP_SIZE_SD_NTSC_WIDTH		768
#define RB_MAP_SIZE_SD_NTSC_Y_LINES		480
#define RB_MAP_SIZE_SD_NTSC_C_LINES		256
#define RB_MAP_SIZE_SD_NTSC_Y_BANK		(0x0005C000)
#define RB_MAP_SIZE_SD_NTSC_C_BANK		(0x00038000)
// SD-PAL(720x576)
#define RB_MAP_SIZE_SD_PAL_WIDTH		768
#define RB_MAP_SIZE_SD_PAL_Y_LINES		576
#define RB_MAP_SIZE_SD_PAL_C_LINES		288
#define RB_MAP_SIZE_SD_PAL_Y_BANK		(0x0006C000)
#define RB_MAP_SIZE_SD_PAL_C_BANK		(0x00078000)
// VGA(640x480)
#define RB_MAP_SIZE_VGA_WIDTH			640
#define RB_MAP_SIZE_VGA_Y_LINES			480
#define RB_MAP_SIZE_VGA_C_LINES			256
#define RB_MAP_SIZE_VGA_Y_BANK			(0x0004C000)
#define RB_MAP_SIZE_VGA_C_BANK			(0x00028000)
// QVGA(320x240)
#define RB_MAP_SIZE_QVGA_WIDTH			384
#define RB_MAP_SIZE_QVGA_Y_LINES		256
#define RB_MAP_SIZE_QVGA_C_LINES		128
#define RB_MAP_SIZE_QVGA_Y_BANK			(0x00018000)
#define RB_MAP_SIZE_QVGA_C_BANK			(0x0000C000)
// SXVGA(1280x960)
#define RB_MAP_SIZE_SXVGA_WIDTH			1280
#define RB_MAP_SIZE_SXVGA_Y_LINES		960
#define RB_MAP_SIZE_SXVGA_C_LINES		480
#define RB_MAP_SIZE_SXVGA_Y_BANK		(0x0012C000)
#define RB_MAP_SIZE_SXVGA_C_BANK		(0x00098000)
// HVGAW(640x380)
#define RB_MAP_SIZE_HVGAW_WIDTH			640
#define RB_MAP_SIZE_HVGAW_Y_LINES		384
#define RB_MAP_SIZE_HVGAW_C_LINES		192
#define RB_MAP_SIZE_HVGAW_Y_BANK		(0x0003C000)
#define RB_MAP_SIZE_HVGAW_C_BANK		(0x00020000)
// FWVGA(848x480)
#define RB_MAP_SIZE_FWVGA_WIDTH			896
#define RB_MAP_SIZE_FWVGA_Y_LINES		480
#define RB_MAP_SIZE_FWVGA_C_LINES		256
#define RB_MAP_SIZE_FWVGA_Y_BANK		(0x0006C000)
#define RB_MAP_SIZE_FWVGA_C_BANK		(0x00038000)
// FWQVGA(432x240)
#define RB_MAP_SIZE_FWQVGA_WIDTH		512
#define RB_MAP_SIZE_FWQVGA_Y_LINES		256
#define RB_MAP_SIZE_FWQVGA_C_LINES		128
#define RB_MAP_SIZE_FWQVGA_Y_BANK		(0x00020000)
#define RB_MAP_SIZE_FWQVGA_C_BANK		(0x00010000)
// 4K2K(16:9)(4096x2304)
#define RB_MAP_SIZE_4K2K_16_9_WIDTH		4096
#define RB_MAP_SIZE_4K2K_16_9_Y_LINES	2304
#define RB_MAP_SIZE_4K2K_16_9_C_LINES	1152
#define RB_MAP_SIZE_4K2K_16_9_Y_BANK	(0x00900000)
#define RB_MAP_SIZE_4K2K_16_9_C_BANK	(0x00480000)
// 4K2K(1.90:1)(4096x2160)
#define RB_MAP_SIZE_4K2K_1_9_1_WIDTH	4096
#define RB_MAP_SIZE_4K2K_1_9_1_Y_LINES	2176
#define RB_MAP_SIZE_4K2K_1_9_1_C_LINES	1088
#define RB_MAP_SIZE_4K2K_1_9_1_Y_BANK	(0x00880000)
#define RB_MAP_SIZE_4K2K_1_9_1_C_BANK	(0x00440000)
// 4K2K(2:1)(4096x2048)
#define RB_MAP_SIZE_4K2K_2_1_WIDTH		4096
#define RB_MAP_SIZE_4K2K_2_1_Y_LINES	2048
#define RB_MAP_SIZE_4K2K_2_1_C_LINES	1024
#define RB_MAP_SIZE_4K2K_2_1_Y_BANK		(0x00800000)
#define RB_MAP_SIZE_4K2K_2_1_C_BANK		(0x00400000)
// 4K2K(QFHD)(3840x2160)
#define RB_MAP_SIZE_4K2K_QFHD_WIDTH		3840
#define RB_MAP_SIZE_4K2K_QFHD_Y_LINES	2176
#define RB_MAP_SIZE_4K2K_QFHD_C_LINES	1088
#define RB_MAP_SIZE_4K2K_QFHD_Y_BANK	(0x007F8000)
#define RB_MAP_SIZE_4K2K_QFHD_C_BANK	(0x003FC000)
// 2.7K(4:3)(2704x2028)
#define RB_MAP_SIZE_2_7K_4_3_WIDTH		2816
#define RB_MAP_SIZE_2_7K_4_3_Y_LINES	2048
#define RB_MAP_SIZE_2_7K_4_3_C_LINES	1024
#define RB_MAP_SIZE_2_7K_4_3_Y_BANK		(0x00580000)
#define RB_MAP_SIZE_2_7K_4_3_C_BANK		(0x002C0000)
// 2.7K(16:9)(2704x1520)
#define RB_MAP_SIZE_2_7K_16_9_WIDTH		2816
#define RB_MAP_SIZE_2_7K_16_9_Y_LINES	1536
#define RB_MAP_SIZE_2_7K_16_9_C_LINES	768
#define RB_MAP_SIZE_2_7K_16_9_Y_BANK	(0x00420000)
#define RB_MAP_SIZE_2_7K_16_9_C_BANK	(0x00210000)
//           (2704x2160)
#define RB_MAP_SIZE_2704x2160_WIDTH		2816
#define RB_MAP_SIZE_2704x2160_Y_LINES	2176
#define RB_MAP_SIZE_2704x2160_C_LINES	1088
#define RB_MAP_SIZE_2704x2160_Y_BANK	(0x005D8000)
#define RB_MAP_SIZE_2704x2160_C_BANK	(0x002EC000)
//           (2880x1524)
#define RB_MAP_SIZE_2880x1524_WIDTH		2944
#define RB_MAP_SIZE_2880x1524_Y_LINES	1536
#define RB_MAP_SIZE_2880x1524_C_LINES	768
#define RB_MAP_SIZE_2880x1524_Y_BANK	(0x00450000)
#define RB_MAP_SIZE_2880x1524_C_BANK	(0x00228000)
//           (4000x2000)
#define RB_MAP_SIZE_4000x2000_WIDTH		4096
#define RB_MAP_SIZE_4000x2000_Y_LINES	2016
#define RB_MAP_SIZE_4000x2000_C_LINES	1024
#define RB_MAP_SIZE_4000x2000_Y_BANK	(0x007E0000)
#define RB_MAP_SIZE_4000x2000_C_BANK	(0x00400000)
//           (3072x1536)
#define RB_MAP_SIZE_3072x1536_WIDTH		3072
#define RB_MAP_SIZE_3072x1536_Y_LINES	1536
#define RB_MAP_SIZE_3072x1536_C_LINES	768
#define RB_MAP_SIZE_3072x1536_Y_BANK	(0x00480000)
#define RB_MAP_SIZE_3072x1536_C_BANK	(0x00240000)
//           (2816x2112)
#define RB_MAP_SIZE_2816x2112_WIDTH		2816
#define RB_MAP_SIZE_2816x2112_Y_LINES	2112
#define RB_MAP_SIZE_2816x2112_C_LINES	1056
#define RB_MAP_SIZE_2816x2112_Y_BANK	(0x005AC000)
#define RB_MAP_SIZE_2816x2112_C_BANK	(0x002D8000)
//           (2048x1536)
#define RB_MAP_SIZE_2048x1536_WIDTH		2048
#define RB_MAP_SIZE_2048x1536_Y_LINES	1536
#define RB_MAP_SIZE_2048x1536_C_LINES	768
#define RB_MAP_SIZE_2048x1536_Y_BANK	(0x00300000)
#define RB_MAP_SIZE_2048x1536_C_BANK	(0x00180000)
//           (2048x1024)
#define RB_MAP_SIZE_2048x1024_WIDTH		2048
#define RB_MAP_SIZE_2048x1024_Y_LINES	1024
#define RB_MAP_SIZE_2048x1024_C_LINES	512
#define RB_MAP_SIZE_2048x1024_Y_BANK	(0x00200000)
#define RB_MAP_SIZE_2048x1024_C_BANK	(0x00100000)
//           (1024x512)
#define RB_MAP_SIZE_1024x512_WIDTH		1024
#define RB_MAP_SIZE_1024x512_Y_LINES	512
#define RB_MAP_SIZE_1024x512_C_LINES	256
#define RB_MAP_SIZE_1024x512_Y_BANK		(0x00080000)
#define RB_MAP_SIZE_1024x512_C_BANK		(0x00040000)
// 2.5K(4:3)(2560x1920)
#define RB_MAP_SIZE_2_5K_4_3_WIDTH		2560
#define RB_MAP_SIZE_2_5K_4_3_Y_LINES	1920
#define RB_MAP_SIZE_2_5K_4_3_C_LINES	960
#define RB_MAP_SIZE_2_5K_4_3_Y_BANK		(0x004B0000)
#define RB_MAP_SIZE_2_5K_4_3_C_BANK		(0x00258000)
// 1.5K(1:1)(1500x1500)
#define RB_MAP_SIZE_1_5K_1_1_WIDTH		1536
#define RB_MAP_SIZE_1_5K_1_1_Y_LINES	1504
#define RB_MAP_SIZE_1_5K_1_1_C_LINES	768
#define RB_MAP_SIZE_1_5K_1_1_Y_BANK		(0x00234000)
#define RB_MAP_SIZE_1_5K_1_1_C_BANK		(0x00120000)
// 3K(1:1)      3000x3000
#define RB_MAP_SIZE_3K_1_1_WIDTH		3072
#define RB_MAP_SIZE_3K_1_1_Y_LINES		3008
#define RB_MAP_SIZE_3K_1_1_C_LINES		1504
#define RB_MAP_SIZE_3K_1_1_Y_BANK		(0x008D0000)
#define RB_MAP_SIZE_3K_1_1_C_BANK		(0x00468000)
//              1440x 720
#define RB_MAP_SIZE_1440x720_WIDTH		1536
#define RB_MAP_SIZE_1440x720_Y_LINES	736
#define RB_MAP_SIZE_1440x720_C_LINES	384
#define RB_MAP_SIZE_1440x720_Y_BANK		(0x00114000)
#define RB_MAP_SIZE_1440x720_C_BANK		(0x00090000)
//               960x 480
#define RB_MAP_SIZE_960x480_WIDTH		1024
#define RB_MAP_SIZE_960x480_Y_LINES		480
#define RB_MAP_SIZE_960x480_C_LINES		256
#define RB_MAP_SIZE_960x480_Y_BANK		(0x00078000)
#define RB_MAP_SIZE_960x480_C_BANK		(0x00040000)
//               864x 480
#define RB_MAP_SIZE_864x480_WIDTH		896
#define RB_MAP_SIZE_864x480_Y_LINES		480
#define RB_MAP_SIZE_864x480_C_LINES		256
#define RB_MAP_SIZE_864x480_Y_BANK		(0x0006C000)
#define RB_MAP_SIZE_864x480_C_BANK		(0x00038000)
//              1920x 960
#define RB_MAP_SIZE_1920x960_WIDTH		1920
#define RB_MAP_SIZE_1920x960_Y_LINES	960
#define RB_MAP_SIZE_1920x960_C_LINES	480
#define RB_MAP_SIZE_1920x960_Y_BANK		(0x001C4000)
#define RB_MAP_SIZE_1920x960_C_BANK		(0x000E4000)
//              2560x1600
#define RB_MAP_SIZE_2560x1600_WIDTH		2560
#define RB_MAP_SIZE_2560x1600_Y_LINES	1600
#define RB_MAP_SIZE_2560x1600_C_LINES	832
#define RB_MAP_SIZE_2560x1600_Y_BANK	(0x003E8000)
#define RB_MAP_SIZE_2560x1600_C_BANK	(0x00208000)
//              2560x 720
#define RB_MAP_SIZE_2560x720_WIDTH		2560
#define RB_MAP_SIZE_2560x720_Y_LINES	768
#define RB_MAP_SIZE_2560x720_C_LINES	384
#define RB_MAP_SIZE_2560x720_Y_BANK		(0x001E0000)
#define RB_MAP_SIZE_2560x720_C_BANK		(0x000F0000)
//              3072x1728
#define RB_MAP_SIZE_3072x1728_WIDTH		3072
#define RB_MAP_SIZE_3072x1728_Y_LINES	1728
#define RB_MAP_SIZE_3072x1728_C_LINES	864
#define RB_MAP_SIZE_3072x1728_Y_BANK	(0x00510000)
#define RB_MAP_SIZE_3072x1728_C_BANK	(0x00288000)
//              3200x2400
#define RB_MAP_SIZE_3200x2400_WIDTH		3200
#define RB_MAP_SIZE_3200x2400_Y_LINES	2400
#define RB_MAP_SIZE_3200x2400_C_LINES	1216
#define RB_MAP_SIZE_3200x2400_Y_BANK	(0x00754000)
#define RB_MAP_SIZE_3200x2400_C_BANK	(0x003B8000)
//              1600x1200
#define RB_MAP_SIZE_1600x1200_WIDTH		1664
#define RB_MAP_SIZE_1600x1200_Y_LINES	1216
#define RB_MAP_SIZE_1600x1200_C_LINES	608
#define RB_MAP_SIZE_1600x1200_Y_BANK	(0x001F0000)
#define RB_MAP_SIZE_1600x1200_C_BANK	(0x000F8000)
//              1024x768
#define RB_MAP_SIZE_1024x768_WIDTH		1024
#define RB_MAP_SIZE_1024x768_Y_LINES	768
#define RB_MAP_SIZE_1024x768_C_LINES	384
#define RB_MAP_SIZE_1024x768_Y_BANK		(0x000C0000)
#define RB_MAP_SIZE_1024x768_C_BANK		(0x00060000)
//              2304x1296
#define RB_MAP_SIZE_2304x1296_WIDTH		2304
#define RB_MAP_SIZE_2304x1296_Y_LINES	1344
#define RB_MAP_SIZE_2304x1296_C_LINES	672
#define RB_MAP_SIZE_2304x1296_Y_BANK	(0x002F4000)
#define RB_MAP_SIZE_2304x1296_C_BANK	(0x0017C000)
//              1296x2304
#define RB_MAP_SIZE_1296x2304_WIDTH		1408
#define RB_MAP_SIZE_1296x2304_Y_LINES	2304
#define RB_MAP_SIZE_1296x2304_C_LINES	1152
#define RB_MAP_SIZE_1296x2304_Y_BANK	(0x00318000)
#define RB_MAP_SIZE_1296x2304_C_BANK	(0x0018C000)
//              1080x1920
#define RB_MAP_SIZE_1080x1920_WIDTH		1152
#define RB_MAP_SIZE_1080x1920_Y_LINES	1920
#define RB_MAP_SIZE_1080x1920_C_LINES	960
#define RB_MAP_SIZE_1080x1920_Y_BANK	(0x0021C000)
#define RB_MAP_SIZE_1080x1920_C_BANK	(0x00110000)
//              720 x1280
#define RB_MAP_SIZE_720x1280_WIDTH		768
#define RB_MAP_SIZE_720x1280_Y_LINES	1280
#define RB_MAP_SIZE_720x1280_C_LINES	640
#define RB_MAP_SIZE_720x1280_Y_BANK		(0x000F0000)
#define RB_MAP_SIZE_720x1280_C_BANK		(0x00078000)
//              360 x 640
#define RB_MAP_SIZE_360x640_WIDTH		384
#define RB_MAP_SIZE_360x640_Y_LINES		640
#define RB_MAP_SIZE_360x640_C_LINES		320
#define RB_MAP_SIZE_360x640_Y_BANK		(0x0003C000)
#define RB_MAP_SIZE_360x640_C_BANK		(0x00020000)
//              320 x 180
#define RB_MAP_SIZE_320x180_WIDTH		384
#define RB_MAP_SIZE_320x180_Y_LINES		192
#define RB_MAP_SIZE_320x180_C_LINES		96
#define RB_MAP_SIZE_320x180_Y_BANK		(0x00014000)
#define RB_MAP_SIZE_320x180_C_BANK		(0x0000C000)
//              3840x1920
#define RB_MAP_SIZE_3840x1920_WIDTH		3840
#define RB_MAP_SIZE_3840x1920_Y_LINES	1920
#define RB_MAP_SIZE_3840x1920_C_LINES	960
#define RB_MAP_SIZE_3840x1920_Y_BANK	(0x00708000)
#define RB_MAP_SIZE_3840x1920_C_BANK	(0x001C4000)
//              1280x 640
#define RB_MAP_SIZE_1280x640_WIDTH		1280
#define RB_MAP_SIZE_1280x640_Y_LINES	640
#define RB_MAP_SIZE_1280x640_C_LINES	320
#define RB_MAP_SIZE_1280x640_Y_BANK		(0x000C8000)
#define RB_MAP_SIZE_1280x640_C_BANK		(0x00064000)
//               640x 320
#define RB_MAP_SIZE_640x320_WIDTH		640
#define RB_MAP_SIZE_640x320_Y_LINES		320
#define RB_MAP_SIZE_640x320_C_LINES		160
#define RB_MAP_SIZE_640x320_Y_BANK		(0x00034000)
#define RB_MAP_SIZE_640x320_C_BANK		(0x0001C000)
//               960x 720
#define RB_MAP_SIZE_960x720_WIDTH		960
#define RB_MAP_SIZE_960x720_Y_LINES		736
#define RB_MAP_SIZE_960x720_C_LINES		384
#define RB_MAP_SIZE_960x720_Y_BANK		(0x000B0000)
#define RB_MAP_SIZE_960x720_C_BANK		(0x0005C000)
//              1632x1088
#define RB_MAP_SIZE_1632x1088_WIDTH		1664
#define RB_MAP_SIZE_1632x1088_Y_LINES	1088
#define RB_MAP_SIZE_1632x1088_C_LINES	544
#define RB_MAP_SIZE_1632x1088_Y_BANK	(0x001BC000)
#define RB_MAP_SIZE_1632x1088_C_BANK	(0x000E0000)
//              1920x1280
#define RB_MAP_SIZE_1920x1280_WIDTH		1920
#define RB_MAP_SIZE_1920x1280_Y_LINES	1280
#define RB_MAP_SIZE_1920x1280_C_LINES	640
#define RB_MAP_SIZE_1920x1280_Y_BANK	(0x00258000)
#define RB_MAP_SIZE_1920x1280_C_BANK	(0x0012C000)
//              2880x1440
#define RB_MAP_SIZE_2880x1440_WIDTH		2944
#define RB_MAP_SIZE_2880x1440_Y_LINES	1440
#define RB_MAP_SIZE_2880x1440_C_LINES	736
#define RB_MAP_SIZE_2880x1440_Y_BANK	(0x0040C000)
#define RB_MAP_SIZE_2880x1440_C_BANK	(0x00214000)
//              1408x 704
#define RB_MAP_SIZE_1408xx704_WIDTH		1408
#define RB_MAP_SIZE_1408xx704_Y_LINES	704
#define RB_MAP_SIZE_1408x704_C_LINES	352
#define RB_MAP_SIZE_1408x704_Y_BANK		(0x000F4000)
#define RB_MAP_SIZE_1408x704_C_BANK		(0x0007C000)
//               960x1080
#define RB_MAP_SIZE_960x1080_WIDTH		960
#define RB_MAP_SIZE_960x1080_Y_LINES	1088
#define RB_MAP_SIZE_960x1080_C_LINES	544
#define RB_MAP_SIZE_960x1080_Y_BANK		(0x00100000)
#define RB_MAP_SIZE_960x1080_C_BANK		(0x00080000)
//              1920x 540
#define RB_MAP_SIZE_1920x540_WIDTH		1920
#define RB_MAP_SIZE_1920x540_Y_LINES	544
#define RB_MAP_SIZE_1920x540_C_LINES	272
#define RB_MAP_SIZE_1920x540_Y_BANK		(0x00100000)
#define RB_MAP_SIZE_1920x540_C_BANK		(0x00080000)
//              1280x1024
#define RB_MAP_SIZE_1280x1024_WIDTH		1280
#define RB_MAP_SIZE_1280x1024_Y_LINES	1024
#define RB_MAP_SIZE_1280x1024_C_LINES	512
#define RB_MAP_SIZE_1280x1024_Y_BANK	(0x00140000)
#define RB_MAP_SIZE_1280x1024_C_BANK	(0x000A0000)
//              1280x 768
#define RB_MAP_SIZE_1280x768_WIDTH		1280
#define RB_MAP_SIZE_1280x768_Y_LINES	768
#define RB_MAP_SIZE_1280x768_C_LINES	384
#define RB_MAP_SIZE_1280x768_Y_BANK		(0x000F0000)
#define RB_MAP_SIZE_1280x768_C_BANK		(0x00078000)
//              800x 600
#define RB_MAP_SIZE_800x600_WIDTH		896
#define RB_MAP_SIZE_800x600_Y_LINES		608
#define RB_MAP_SIZE_800x600_C_LINES		320
#define RB_MAP_SIZE_800x600_Y_BANK		(0x00088000)
#define RB_MAP_SIZE_800x600_C_BANK		(0x00048000)
//             2272x1472
#define RB_MAP_SIZE_2272x1472_WIDTH		2272
#define RB_MAP_SIZE_2272x1472_Y_LINES	1472
#define RB_MAP_SIZE_2272x1472_C_LINES	736
#define RB_MAP_SIZE_2272x1472_Y_BANK	(0x00334000)
#define RB_MAP_SIZE_2272x1472_C_BANK	(0x0019C000)
#endif
/* --- 1/16 reduction bank size --- */
/* * free bank size support
   * Reference size
   * width           : align 128
   * y_lines,c_lines : align 32
   *   (width*y_lines) = y bank size(16KB align)
   *   (width*c_lines) = c bank size(16KB align)
*/
#if 0
// FHD (1920x1080)
#define RB_MAP_SIZE_FHD_REDUCTION_WIDTH				512
#define RB_MAP_SIZE_FHD_REDUCTION_Y_LINES			288
#define RB_MAP_SIZE_FHD_REDUCTION_C_LINES			160
#define RB_MAP_SIZE_FHD_REDUCTION_Y_BANK			(0x00024000)
#define RB_MAP_SIZE_FHD_REDUCTION_C_BANK			(0x00014000)
// 1440(1440x1080)
#define RB_MAP_SIZE_1440_REDUCTION_WIDTH			384
#define RB_MAP_SIZE_1440_REDUCTION_Y_LINES			288
#define RB_MAP_SIZE_1440_REDUCTION_C_LINES			160
#define RB_MAP_SIZE_1440_REDUCTION_Y_BANK			(0x0001C000)
#define RB_MAP_SIZE_1440_REDUCTION_C_BANK			(0x00010000)
// 1440P(1920x1440)
#define RB_MAP_SIZE_1440P_REDUCTION_WIDTH			512
#define RB_MAP_SIZE_1440P_REDUCTION_Y_LINES			384
#define RB_MAP_SIZE_1440P_REDUCTION_C_LINES			192
#define RB_MAP_SIZE_1440P_REDUCTION_Y_BANK			(0x00030000)
#define RB_MAP_SIZE_1440P_REDUCTION_C_BANK			(0x00018000)
// HD(1280x720)
#define RB_MAP_SIZE_HD_REDUCTION_WIDTH				384
#define RB_MAP_SIZE_HD_REDUCTION_Y_LINES			192
#define RB_MAP_SIZE_HD_REDUCTION_C_LINES			96
#define RB_MAP_SIZE_HD_REDUCTION_Y_BANK				(0x00014000)
#define RB_MAP_SIZE_HD_REDUCTION_C_BANK				(0x0000C000)
// SD-NTSC(720x480)
#define RB_MAP_SIZE_SD_NTSC_REDUCTION_WIDTH			256
#define RB_MAP_SIZE_SD_NTSC_REDUCTION_Y_LINES		128
#define RB_MAP_SIZE_SD_NTSC_REDUCTION_C_LINES		64
#define RB_MAP_SIZE_SD_NTSC_REDUCTION_Y_BANK		(0x00008000)
#define RB_MAP_SIZE_SD_NTSC_REDUCTION_C_BANK		(0x00004000)
// SD-PAL(720x576)
#define RB_MAP_SIZE_SD_PAL_REDUCTION_WIDTH			256
#define RB_MAP_SIZE_SD_PAL_REDUCTION_Y_LINES		160
#define RB_MAP_SIZE_SD_PAL_REDUCTION_C_LINES		96
#define RB_MAP_SIZE_SD_PAL_REDUCTION_Y_BANK			(0x0000C000)
#define RB_MAP_SIZE_SD_PAL_REDUCTION_C_BANK			(0x00008000)
// VGA(640x480)
#define RB_MAP_SIZE_VGA_REDUCTION_WIDTH				256
#define RB_MAP_SIZE_VGA_REDUCTION_Y_LINES			128
#define RB_MAP_SIZE_VGA_REDUCTION_C_LINES			64
#define RB_MAP_SIZE_VGA_REDUCTION_Y_BANK			(0x00008000)
#define RB_MAP_SIZE_VGA_REDUCTION_C_BANK			(0x00004000)
// QVGA(320x240)
#define RB_MAP_SIZE_QVGA_REDUCTION_WIDTH			128
#define RB_MAP_SIZE_QVGA_REDUCTION_Y_LINES			64
#define RB_MAP_SIZE_QVGA_REDUCTION_C_LINES			32
#define RB_MAP_SIZE_QVGA_REDUCTION_Y_BANK			(0x00004000)
#define RB_MAP_SIZE_QVGA_REDUCTION_C_BANK			(0x00004000)
// SXVGA(1280x960)
#define RB_MAP_SIZE_SXVGA_REDUCTION_WIDTH			384
#define RB_MAP_SIZE_SXVGA_REDUCTION_Y_LINES			256
#define RB_MAP_SIZE_SXVGA_REDUCTION_C_LINES			128
#define RB_MAP_SIZE_SXVGA_REDUCTION_Y_BANK			(0x00018000)
#define RB_MAP_SIZE_SXVGA_REDUCTION_C_BANK			(0x0000C000)
// HVGAW(640x380)
#define RB_MAP_SIZE_HVGAW_REDUCTION_WIDTH			256
#define RB_MAP_SIZE_HVGAW_REDUCTION_Y_LINES			96
#define RB_MAP_SIZE_HVGAW_REDUCTION_C_LINES			64
#define RB_MAP_SIZE_HVGAW_REDUCTION_Y_BANK			(0x00008000)
#define RB_MAP_SIZE_HVGAW_REDUCTION_C_BANK			(0x00004000)
// FWVGA(848x480)
#define RB_MAP_SIZE_FWVGA_REDUCTION_WIDTH			256
#define RB_MAP_SIZE_FWVGA_REDUCTION_Y_LINES			128
#define RB_MAP_SIZE_FWVGA_REDUCTION_C_LINES			64
#define RB_MAP_SIZE_FWVGA_REDUCTION_Y_BANK			(0x00008000)
#define RB_MAP_SIZE_FWVGA_REDUCTION_C_BANK			(0x00004000)
// FWQVGA(432x240)
#define RB_MAP_SIZE_FWQVGA_REDUCTION_WIDTH			128
#define RB_MAP_SIZE_FWQVGA_REDUCTION_Y_LINES		64
#define RB_MAP_SIZE_FWQVGA_REDUCTION_C_LINES		32
#define RB_MAP_SIZE_FWQVGA_REDUCTION_Y_BANK			(0x00004000)
#define RB_MAP_SIZE_FWQVGA_REDUCTION_C_BANK			(0x00004000)
// 4K2K(16:9)(4096x2304)
#define RB_MAP_SIZE_4K2K_16_9_REDUCTION_WIDTH		1024
#define RB_MAP_SIZE_4K2K_16_9_REDUCTION_Y_LINES		576
#define RB_MAP_SIZE_4K2K_16_9_REDUCTION_C_LINES		288
#define RB_MAP_SIZE_4K2K_16_9_REDUCTION_Y_BANK		(0x00090000)
#define RB_MAP_SIZE_4K2K_16_9_REDUCTION_C_BANK		(0x00048000)
// 4K2K(1.90:1)(4096x2160)
#define RB_MAP_SIZE_4K2K_1_9_1_REDUCTION_WIDTH		1024
#define RB_MAP_SIZE_4K2K_1_9_1_REDUCTION_Y_LINES	544
#define RB_MAP_SIZE_4K2K_1_9_1_REDUCTION_C_LINES	288
#define RB_MAP_SIZE_4K2K_1_9_1_REDUCTION_Y_BANK		(0x00088000)
#define RB_MAP_SIZE_4K2K_1_9_1_REDUCTION_C_BANK		(0x00048000)
// 4K2K(2:1)(4096x2048)
#define RB_MAP_SIZE_4K2K_2_1_REDUCTION_WIDTH		1024
#define RB_MAP_SIZE_4K2K_2_1_REDUCTION_Y_LINES		512
#define RB_MAP_SIZE_4K2K_2_1_REDUCTION_C_LINES		256
#define RB_MAP_SIZE_4K2K_2_1_REDUCTION_Y_BANK		(0x00080000)
#define RB_MAP_SIZE_4K2K_2_1_REDUCTION_C_BANK		(0x00040000)
// 4K2K(QFHD)(3840x2160)
#define RB_MAP_SIZE_4K2K_QFHD_REDUCTION_WIDTH		1024
#define RB_MAP_SIZE_4K2K_QFHD_REDUCTION_Y_LINES		544
#define RB_MAP_SIZE_4K2K_QFHD_REDUCTION_C_LINES		288
#define RB_MAP_SIZE_4K2K_QFHD_REDUCTION_Y_BANK		(0x00088000)
#define RB_MAP_SIZE_4K2K_QFHD_REDUCTION_C_BANK		(0x00048000)
// 2.7K(4:3)(2704x2028)
#define RB_MAP_SIZE_2_7K_4_3_REDUCTION_WIDTH		768
#define RB_MAP_SIZE_2_7K_4_3_REDUCTION_Y_LINES		512
#define RB_MAP_SIZE_2_7K_4_3_REDUCTION_C_LINES		256
#define RB_MAP_SIZE_2_7K_4_3_REDUCTION_Y_BANK		(0x00060000)
#define RB_MAP_SIZE_2_7K_4_3_REDUCTION_C_BANK		(0x00030000)
// 2.7K(16:9)(2704x1520)
#define RB_MAP_SIZE_2_7K_16_9_REDUCTION_WIDTH		768
#define RB_MAP_SIZE_2_7K_16_9_REDUCTION_Y_LINES		384
#define RB_MAP_SIZE_2_7K_16_9_REDUCTION_C_LINES		192
#define RB_MAP_SIZE_2_7K_16_9_REDUCTION_Y_BANK		(0x00048000)
#define RB_MAP_SIZE_2_7K_16_9_REDUCTION_C_BANK		(0x00024000)
//           (2704x2160)
#define RB_MAP_SIZE_2704x2160_REDUCTION_WIDTH		768
#define RB_MAP_SIZE_2704x2160_REDUCTION_Y_LINES		544
#define RB_MAP_SIZE_2704x2160_REDUCTION_C_LINES		288
#define RB_MAP_SIZE_2704x2160_REDUCTION_Y_BANK		(0x00066000)
#define RB_MAP_SIZE_2704x2160_REDUCTION_C_BANK		(0x00036000)
//           (2880x1524)
#define RB_MAP_SIZE_2880x1524_REDUCTION_WIDTH		768
#define RB_MAP_SIZE_2880x1524_REDUCTION_Y_LINES		384
#define RB_MAP_SIZE_2880x1524_REDUCTION_C_LINES		192
#define RB_MAP_SIZE_2880x1524_REDUCTION_Y_BANK		(0x00048000)
#define RB_MAP_SIZE_2880x1524_REDUCTION_C_BANK		(0x00024000)
//           (4000x2000)
#define RB_MAP_SIZE_4000x2000_REDUCTION_WIDTH		1024
#define RB_MAP_SIZE_4000x2000_REDUCTION_Y_LINES		512
#define RB_MAP_SIZE_4000x2000_REDUCTION_C_LINES		256
#define RB_MAP_SIZE_4000x2000_REDUCTION_Y_BANK		(0x00080000)
#define RB_MAP_SIZE_4000x2000_REDUCTION_C_BANK		(0x00040000)
//           (3072x1536)
#define RB_MAP_SIZE_3072x1536_REDUCTION_WIDTH		768
#define RB_MAP_SIZE_3072x1536_REDUCTION_Y_LINES		384
#define RB_MAP_SIZE_3072x1536_REDUCTION_C_LINES		192
#define RB_MAP_SIZE_3072x1536_REDUCTION_Y_BANK		(0x00048000)
#define RB_MAP_SIZE_3072x1536_REDUCTION_C_BANK		(0x00024000)
//           (2816x2112)
#define RB_MAP_SIZE_2816x2112_REDUCTION_WIDTH		768
#define RB_MAP_SIZE_2816x2112_REDUCTION_Y_LINES		544
#define RB_MAP_SIZE_2816x2112_REDUCTION_C_LINES		288
#define RB_MAP_SIZE_2816x2112_REDUCTION_Y_BANK		(0x00068000)
#define RB_MAP_SIZE_2816x2112_REDUCTION_C_BANK		(0x00038000)
//           (2048x1536)
#define RB_MAP_SIZE_2048x1536_REDUCTION_WIDTH		512
#define RB_MAP_SIZE_2048x1536_REDUCTION_Y_LINES		384
#define RB_MAP_SIZE_2048x1536_REDUCTION_C_LINES		192
#define RB_MAP_SIZE_2048x1536_REDUCTION_Y_BANK		(0x00030000)
#define RB_MAP_SIZE_2048x1536_REDUCTION_C_BANK		(0x00018000)
//           (2048x1024)
#define RB_MAP_SIZE_2048x1024_REDUCTION_WIDTH		512
#define RB_MAP_SIZE_2048x1024_REDUCTION_Y_LINES		256
#define RB_MAP_SIZE_2048x1024_REDUCTION_C_LINES		128
#define RB_MAP_SIZE_2048x1024_REDUCTION_Y_BANK		(0x00020000)
#define RB_MAP_SIZE_2048x1024_REDUCTION_C_BANK		(0x00010000)
//           (1024x512)
#define RB_MAP_SIZE_1024x512_REDUCTION_WIDTH		256
#define RB_MAP_SIZE_1024x512_REDUCTION_Y_LINES		128
#define RB_MAP_SIZE_1024x512_REDUCTION_C_LINES		64
#define RB_MAP_SIZE_1024x512_REDUCTION_Y_BANK		(0x00008000)
#define RB_MAP_SIZE_1024x512_REDUCTION_C_BANK		(0x00004000)
// 2.5K(4:3)(2560x1920)
#define RB_MAP_SIZE_2_5K_4_3_REDUCTION_WIDTH		640
#define RB_MAP_SIZE_2_5K_4_3_REDUCTION_Y_LINES		480
#define RB_MAP_SIZE_2_5K_4_3_REDUCTION_C_LINES		256
#define RB_MAP_SIZE_2_5K_4_3_REDUCTION_Y_BANK		(0x0004C000)
#define RB_MAP_SIZE_2_5K_4_3_REDUCTION_C_BANK		(0x00028000)
// 1.5K(1:1)(1500x1500)
#define RB_MAP_SIZE_1_5K_1_1_REDUCTION_WIDTH		384
#define RB_MAP_SIZE_1_5K_1_1_REDUCTION_Y_LINES		384
#define RB_MAP_SIZE_1_5K_1_1_REDUCTION_C_LINES		192
#define RB_MAP_SIZE_1_5K_1_1_REDUCTION_Y_BANK		(0x00024000)
#define RB_MAP_SIZE_1_5K_1_1_REDUCTION_C_BANK		(0x00014000)
// 3K(1:1)      3000x3000
#define RB_MAP_SIZE_3K_1_1_REDUCTION_WIDTH			768
#define RB_MAP_SIZE_3K_1_1_REDUCTION_Y_LINES		768
#define RB_MAP_SIZE_3K_1_1_REDUCTION_C_LINES		384
#define RB_MAP_SIZE_3K_1_1_REDUCTION_Y_BANK			(0x00090000)
#define RB_MAP_SIZE_3K_1_1_REDUCTION_C_BANK			(0x00048000)
//              1440x 720
#define RB_MAP_SIZE_1440x720_REDUCTION_WIDTH		384
#define RB_MAP_SIZE_1440x720_REDUCTION_Y_LINES		192
#define RB_MAP_SIZE_1440x720_REDUCTION_C_LINES		96
#define RB_MAP_SIZE_1440x720_REDUCTION_Y_BANK		(0x00014000)
#define RB_MAP_SIZE_1440x720_REDUCTION_C_BANK		(0x0000C000)
//               960x 480
#define RB_MAP_SIZE_960x480_REDUCTION_WIDTH			256
#define RB_MAP_SIZE_960x480_REDUCTION_Y_LINES		128
#define RB_MAP_SIZE_960x480_REDUCTION_C_LINES		64
#define RB_MAP_SIZE_960x480_REDUCTION_Y_BANK		(0x00008000)
#define RB_MAP_SIZE_960x480_REDUCTION_C_BANK		(0x00004000)
//               864x 480
#define RB_MAP_SIZE_864x480_REDUCTION_WIDTH			256
#define RB_MAP_SIZE_864x480_REDUCTION_Y_LINES		128
#define RB_MAP_SIZE_864x480_REDUCTION_C_LINES		64
#define RB_MAP_SIZE_864x480_REDUCTION_Y_BANK		(0x00008000)
#define RB_MAP_SIZE_864x480_REDUCTION_C_BANK		(0x00004000)
//              1920x 960
#define RB_MAP_SIZE_1920x960_REDUCTION_WIDTH		512
#define RB_MAP_SIZE_1920x960_REDUCTION_Y_LINES		256
#define RB_MAP_SIZE_1920x960_REDUCTION_C_LINES		128
#define RB_MAP_SIZE_1920x960_REDUCTION_Y_BANK		(0x00020000)
#define RB_MAP_SIZE_1920x960_REDUCTION_C_BANK		(0x00010000)
//              2560x1600
#define RB_MAP_SIZE_2560x1600_REDUCTION_WIDTH		640
#define RB_MAP_SIZE_2560x1600_REDUCTION_Y_LINES		416
#define RB_MAP_SIZE_2560x1600_REDUCTION_C_LINES		224
#define RB_MAP_SIZE_2560x1600_REDUCTION_Y_BANK		(0x00044000)
#define RB_MAP_SIZE_2560x1600_REDUCTION_C_BANK		(0x00024000)
//              2560x 720
#define RB_MAP_SIZE_2560x720_REDUCTION_WIDTH		640
#define RB_MAP_SIZE_2560x720_REDUCTION_Y_LINES		192
#define RB_MAP_SIZE_2560x720_REDUCTION_C_LINES		96
#define RB_MAP_SIZE_2560x720_REDUCTION_Y_BANK		(0x00020000)
#define RB_MAP_SIZE_2560x720_REDUCTION_C_BANK		(0x00010000)
//              3072x1728
#define RB_MAP_SIZE_3072x1728_REDUCTION_WIDTH		768
#define RB_MAP_SIZE_3072x1728_REDUCTION_Y_LINES		448
#define RB_MAP_SIZE_3072x1728_REDUCTION_C_LINES		224
#define RB_MAP_SIZE_3072x1728_REDUCTION_Y_BANK		(0x00054000)
#define RB_MAP_SIZE_3072x1728_REDUCTION_C_BANK		(0x0002C000)
//              3200x2400
#define RB_MAP_SIZE_3200x2400_REDUCTION_WIDTH		896
#define RB_MAP_SIZE_3200x2400_REDUCTION_Y_LINES		608
#define RB_MAP_SIZE_3200x2400_REDUCTION_C_LINES		320
#define RB_MAP_SIZE_3200x2400_REDUCTION_Y_BANK		(0x00088000)
#define RB_MAP_SIZE_3200x2400_REDUCTION_C_BANK		(0x00048000)
//              1600x1200
#define RB_MAP_SIZE_1600x1200_REDUCTION_WIDTH		512
#define RB_MAP_SIZE_1600x1200_REDUCTION_Y_LINES		320
#define RB_MAP_SIZE_1600x1200_REDUCTION_C_LINES		160
#define RB_MAP_SIZE_1600x1200_REDUCTION_Y_BANK		(0x00028000)
#define RB_MAP_SIZE_1600x1200_REDUCTION_C_BANK		(0x00014000)
//              1024x768
#define RB_MAP_SIZE_1024x768_REDUCTION_WIDTH		256
#define RB_MAP_SIZE_1024x768_REDUCTION_Y_LINES		192
#define RB_MAP_SIZE_1024x768_REDUCTION_C_LINES		96
#define RB_MAP_SIZE_1024x768_REDUCTION_Y_BANK		(0x000C000)
#define RB_MAP_SIZE_1024x768_REDUCTION_C_BANK		(0x0008000)
//              2304x1296
#define RB_MAP_SIZE_2304x1296_REDUCTION_WIDTH		640
#define RB_MAP_SIZE_2304x1296_REDUCTION_Y_LINES		384
#define RB_MAP_SIZE_2304x1296_REDUCTION_C_LINES		192
#define RB_MAP_SIZE_2304x1296_REDUCTION_Y_BANK		(0x003C000)
#define RB_MAP_SIZE_2304x1296_REDUCTION_C_BANK		(0x0020000)
//              1296x2304
#define RB_MAP_SIZE_1296x2304_REDUCTION_WIDTH		384
#define RB_MAP_SIZE_1296x2304_REDUCTION_Y_LINES		576
#define RB_MAP_SIZE_1296x2304_REDUCTION_C_LINES		288
#define RB_MAP_SIZE_1296x2304_REDUCTION_Y_BANK		(0x0038000)
#define RB_MAP_SIZE_1296x2304_REDUCTION_C_BANK		(0x001C000)
//              1080x1920
#define RB_MAP_SIZE_1080x1920_REDUCTION_WIDTH		384
#define RB_MAP_SIZE_1080x1920_REDUCTION_Y_LINES		480
#define RB_MAP_SIZE_1080x1920_REDUCTION_C_LINES		256
#define RB_MAP_SIZE_1080x1920_REDUCTION_Y_BANK		(0x0030000)
#define RB_MAP_SIZE_1080x1920_REDUCTION_C_BANK		(0x0018000)
//              720 x1280
#define RB_MAP_SIZE_720x1280_REDUCTION_WIDTH		256
#define RB_MAP_SIZE_720x1280_REDUCTION_Y_LINES		320
#define RB_MAP_SIZE_720x1280_REDUCTION_C_LINES		160
#define RB_MAP_SIZE_720x1280_REDUCTION_Y_BANK		(0x0014000)
#define RB_MAP_SIZE_720x1280_REDUCTION_C_BANK		(0x000C000)
//              360 x 640
#define RB_MAP_SIZE_360x640_REDUCTION_WIDTH			128
#define RB_MAP_SIZE_360x640_REDUCTION_Y_LINES		160
#define RB_MAP_SIZE_360x640_REDUCTION_C_LINES		96
#define RB_MAP_SIZE_360x640_REDUCTION_Y_BANK		(0x0008000)
#define RB_MAP_SIZE_360x640_REDUCTION_C_BANK		(0x0004000)
//              320 x 180
#define RB_MAP_SIZE_320x180_REDUCTION_WIDTH			128
#define RB_MAP_SIZE_320x180_REDUCTION_Y_LINES		64
#define RB_MAP_SIZE_320x180_REDUCTION_C_LINES		32
#define RB_MAP_SIZE_320x180_REDUCTION_Y_BANK		(0x0004000)
#define RB_MAP_SIZE_320x180_REDUCTION_C_BANK		(0x0004000)
//              3840x1920
#define RB_MAP_SIZE_3840x1920_REDUCTION_WIDTH		1024
#define RB_MAP_SIZE_3840x1920_REDUCTION_Y_LINES		480
#define RB_MAP_SIZE_3840x1920_REDUCTION_C_LINES		256
#define RB_MAP_SIZE_3840x1920_REDUCTION_Y_BANK		(0x0078000)
#define RB_MAP_SIZE_3840x1920_REDUCTION_C_BANK		(0x0040000)
//              1280x 640
#define RB_MAP_SIZE_1280x640_REDUCTION_WIDTH		384
#define RB_MAP_SIZE_1280x640_REDUCTION_Y_LINES		160
#define RB_MAP_SIZE_1280x640_REDUCTION_C_LINES		96
#define RB_MAP_SIZE_1280x640_REDUCTION_Y_BANK		(0x0010000)
#define RB_MAP_SIZE_1280x640_REDUCTION_C_BANK		(0x000C000)
//               640x 320
#define RB_MAP_SIZE_640x320_REDUCTION_WIDTH			256
#define RB_MAP_SIZE_640x320_REDUCTION_Y_LINES		96
#define RB_MAP_SIZE_640x320_REDUCTION_C_LINES		64
#define RB_MAP_SIZE_640x320_REDUCTION_Y_BANK		(0x0008000)
#define RB_MAP_SIZE_640x320_REDUCTION_C_BANK		(0x0004000)
//               960x 720
#define RB_MAP_SIZE_960x720_REDUCTION_WIDTH			256
#define RB_MAP_SIZE_960x720_REDUCTION_Y_LINES		192
#define RB_MAP_SIZE_960x720_REDUCTION_C_LINES		96
#define RB_MAP_SIZE_960x720_REDUCTION_Y_BANK		(0x000C000)
#define RB_MAP_SIZE_960x720_REDUCTION_C_BANK		(0x0008000)
//              1632x1088
#define RB_MAP_SIZE_1632x1088_REDUCTION_WIDTH		512
#define RB_MAP_SIZE_1632x1088_REDUCTION_Y_LINES		288
#define RB_MAP_SIZE_1632x1088_REDUCTION_C_LINES		160
#define RB_MAP_SIZE_1632x1088_REDUCTION_Y_BANK		(0x0024000)
#define RB_MAP_SIZE_1632x1088_REDUCTION_C_BANK		(0x0014000)
//              1920x1280
#define RB_MAP_SIZE_1920x1280_REDUCTION_WIDTH		512
#define RB_MAP_SIZE_1920x1280_REDUCTION_Y_LINES		320
#define RB_MAP_SIZE_1920x1280_REDUCTION_C_LINES		160
#define RB_MAP_SIZE_1920x1280_REDUCTION_Y_BANK		(0x0028000)
#define RB_MAP_SIZE_1920x1280_REDUCTION_C_BANK		(0x0014000)
//              2880x1440
#define RB_MAP_SIZE_2880x1440_REDUCTION_WIDTH		768
#define RB_MAP_SIZE_2880x1440_REDUCTION_Y_LINES		384
#define RB_MAP_SIZE_2880x1440_REDUCTION_C_LINES		192
#define RB_MAP_SIZE_2880x1440_REDUCTION_Y_BANK		(0x0048000)
#define RB_MAP_SIZE_2880x1440_REDUCTION_C_BANK		(0x0024000)
//              1408x 704
#define RB_MAP_SIZE_1408x704_REDUCTION_WIDTH		384
#define RB_MAP_SIZE_1408x704_REDUCTION_Y_LINES		192
#define RB_MAP_SIZE_1408x704_REDUCTION_C_LINES		96
#define RB_MAP_SIZE_1408x704_REDUCTION_Y_BANK		(0x0014000)
#define RB_MAP_SIZE_1408x704_REDUCTION_C_BANK		(0x000C000)
//               960x1080
#define RB_MAP_SIZE_960x1080_REDUCTION_WIDTH		256
#define RB_MAP_SIZE_960x1080_REDUCTION_Y_LINES		288
#define RB_MAP_SIZE_960x1080_REDUCTION_C_LINES		160
#define RB_MAP_SIZE_960x1080_REDUCTION_Y_BANK		(0x0014000)
#define RB_MAP_SIZE_960x1080_REDUCTION_C_BANK		(0x000C000)
//              1920x 540
#define RB_MAP_SIZE_1920x540_REDUCTION_WIDTH		512
#define RB_MAP_SIZE_1920x540_REDUCTION_Y_LINES		160
#define RB_MAP_SIZE_1920x540_REDUCTION_C_LINES		96
#define RB_MAP_SIZE_1920x540_REDUCTION_Y_BANK		(0x0014000)
#define RB_MAP_SIZE_1920x540_REDUCTION_C_BANK		(0x000C000)
//              1280x 1024
#define RB_MAP_SIZE_1280x1024_REDUCTION_WIDTH		384
#define RB_MAP_SIZE_1280x1024_REDUCTION_Y_LINES		256
#define RB_MAP_SIZE_1280x1024_REDUCTION_C_LINES		128
#define RB_MAP_SIZE_1280x1024_REDUCTION_Y_BANK		(0x0018000)
#define RB_MAP_SIZE_1280x1024_REDUCTION_C_BANK		(0x000C000)
//              1280x 768
#define RB_MAP_SIZE_1280x768_REDUCTION_WIDTH		384
#define RB_MAP_SIZE_1280x768_REDUCTION_Y_LINES		192
#define RB_MAP_SIZE_1280x768_REDUCTION_C_LINES		96
#define RB_MAP_SIZE_1280x768_REDUCTION_Y_BANK		(0x0014000)
#define RB_MAP_SIZE_1280x768_REDUCTION_C_BANK		(0x000C000)
//               800x 600
#define RB_MAP_SIZE_800x600_REDUCTION_WIDTH			256
#define RB_MAP_SIZE_800x600_REDUCTION_Y_LINES		160
#define RB_MAP_SIZE_800x600_REDUCTION_C_LINES		96
#define RB_MAP_SIZE_800x600_REDUCTION_Y_BANK		(0x000C000)
#define RB_MAP_SIZE_800x600_REDUCTION_C_BANK		(0x0008000)
//              2272x1472
#define RB_MAP_SIZE_2272x1472_REDUCTION_WIDTH		576
#define RB_MAP_SIZE_2272x1472_REDUCTION_Y_LINES		368
#define RB_MAP_SIZE_2272x1472_REDUCTION_C_LINES		192
#define RB_MAP_SIZE_2272x1472_REDUCTION_Y_BANK		(0x0034000)
#define RB_MAP_SIZE_2272x1472_REDUCTION_C_BANK		(0x001C000)
#endif

//----------------------
// address
//----------------------
// enc
// work0 base
#define RB_MAP_ADR_ENC_WORK0			(0x00000000)													// work0
#define RB_MAP_ADR_ENC_WORK0_END		(RB_MAP_ADR_ENC_WORK0 + RB_MAP_SIZ_ENC_WORK0)					// work0 end

// work1 base
#define RB_MAP_ADR_ENC_WORK1			(0x00000000)													// work1
#define RB_MAP_ADR_ENC_WORK1_END		(RB_MAP_ADR_ENC_WORK1 + RB_MAP_SIZ_ENC_WORK1)					// work1 end

// work2 base
#define RB_MAP_ADR_ENC_WORK2			(0x00000000)													// work2
#define RB_MAP_ADR_ENC_WORK2_END		(RB_MAP_ADR_ENC_WORK2 + RB_MAP_SIZ_ENC_WORK2)					// work2 end

// dec
// work base
#define RB_MAP_ADR_DEC_WORK0			(0x00000000)													// work0
#define RB_MAP_ADR_DEC_WORK0_END		(RB_MAP_ADR_DEC_WORK0 + RB_MAP_SIZ_DEC_WORK0)					// work0 end

#define RB_MAP_ADR_DEC_WORK1			(0x00000000)													// work1
#define RB_MAP_ADR_DEC_WORK1_END		(RB_MAP_ADR_DEC_WORK1 + RB_MAP_SIZ_DEC_WORK1)					// work1 end

#define RB_MAP_ADR_DEC_WORK2			(0x00000000)													// work2
#define RB_MAP_ADR_DEC_WORK2_END		(RB_MAP_ADR_DEC_WORK2 + RB_MAP_SIZ_DEC_WORK2)					// work2 end

	/*
		definition of "LocalDec" Area is required.(Encode only)
		YCC : 2bank(RB_Bridge_Set_MemoryMap() map_size param(localdec_y_bank+localdec_c_bank))
		Y/C separate
		Y   : 2bank(RB_Bridge_Set_MemoryMap() map_size param(localdec_y_bank))
		C   : 2bank(RB_Bridge_Set_MemoryMap() map_size param(localdec_c_bank))
	*/
	/*
		definition of "Reduction" Area is required.(Encode only)
		YCC : 5bank(RB_Bridge_Set_MemoryMap() map_size param(reduction_y_bank+reduction_c_bank))
		Y/C separate
		Y   : 5bank(RB_Bridge_Set_MemoryMap() map_size param(reduction_y_bank))
		C   : 5bank(RB_Bridge_Set_MemoryMap() map_size param(reduction_c_bank))
	*/
	/*
		definition of "YCC" Area is required.
		  Encode	RB_MAP_BNK_ENC_YCC * (RB_Bridge_Set_MemoryMap() map_size param(y_bank+c_bank))
		  Decode	RB_MAP_BNK_DEC_YCC * (RB_Bridge_Set_MemoryMap() map_size param(y_bank+c_bank))
		Y/C separate
		  Encode	Y : RB_MAP_BNK_ENC_YCC * (RB_Bridge_Set_MemoryMap() map_size param(y_bank))
		  			C : RB_MAP_BNK_ENC_YCC * (RB_Bridge_Set_MemoryMap() map_size param(c_bank))
		  Decode	Y : RB_MAP_BNK_DEC_YCC * (RB_Bridge_Set_MemoryMap() map_size param(y_bank))
		  			C : RB_MAP_BNK_DEC_YCC * (RB_Bridge_Set_MemoryMap() map_size param(c_bank))
	 */

// error info
#define RB_MAP_ADR_DEC_MVCPU_ERROR_INFO		(RB_MAP_ADR_DEC_WORK0  + 0x00008480)	// error DEC MVCPU ERROR INFO
#define RB_MAP_ADR_DEC_SVCPU_ERROR_INFO		(RB_MAP_ADR_DEC_WORK0  + 0x00008484)	// error DEC SVCPU ERROR INFO
#define RB_MAP_ADR_DEC_ERROR_INFO_EX_MV		(RB_MAP_ADR_DEC_WORK0  + 0x00008488)	// error DEC INFO  EX MV
#define RB_MAP_ADR_DEC_ERROR_INFO_EX_SV		(RB_MAP_ADR_DEC_WORK0  + 0x0000848C)	// error DEC INFO  EX SV
#define RB_MAP_ADR_ENC_ERROR_INFO			(RB_MAP_ADR_ENC_WORK0  + 0x0001A100)	// error ENC ERROR INFO       
#define RB_MAP_ADR_ENC_ERROR_INFO_EX		(RB_MAP_ADR_ENC_WORK0  + 0x0001A104)	// error ENC ERROR INFO_EX    
#define RB_MAP_ADR_ENC_ERROR_INFO_GLOBAL	(RB_MAP_ADR_ENC_WORK0  + 0x0001A108)	// error ENC ERROR INFO_GLOBAL


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */


/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
//----------------------
// BRIDGE_PARAM
//----------------------
typedef struct{
	int							work0_ch;			// work0 area  of SDRAM0/1	*RB_SDRAM_CH0/1
	int							work1_ch;			// work1 area  of SDRAM0/1	*RB_SDRAM_CH0/1
	int							work2_ch;			// work2 area  of SDRAM0/1	*RB_SDRAM_CH0/1
	int							stream_ch;			// stream area of SDRAM0/1	*RB_SDRAM_CH0/1
} T_RB_BRIDGE_MAP_COMMON_CH;

typedef struct{
	unsigned long				work0_addr;			// work0 area top address	*1KB align
	unsigned long				work1_addr;			// work1 area top address	*1KB align
	unsigned long				work2_addr;			// work2 area top address	*1KB align
	unsigned long				stream_addr;		// stream area top address	*1KB align
} T_RB_BRIDGE_MAP_COMMON_ADDR;

typedef struct{
	unsigned long				ch0_top_addr;		// SDRAM ch0 Top address
	unsigned long				ch1_top_addr;		// SDRAM ch1 Top address
	int							strm_bank;			// stream bank size(1MB align)
	T_RB_BRIDGE_MAP_COMMON_CH	map_common_ch;		// map common ch
	T_RB_BRIDGE_MAP_COMMON_ADDR	map_common_addr;	// map common address
} T_RB_BRIDGE_MAP_COMMON;

typedef struct{
	int				width;				//                width  size	*128  align
	int				y_bank;				//                y_bank size	*16KB align(tb_flg=ON : y_bank/2 16KB align)
	int				c_bank;				//                c_bank size	*16KB align(tb_flg=ON : c_bank/2 16KB align)
	int				localdec_width;		// localdec       width  size	*128  align
	int				localdec_y_bank;	// localdec       y_bank size	*16KB align(tb_flg=ON : y_bank/2 16KB align)
	int				localdec_c_bank;	// localdec       c_bank size	*16KB align(tb_flg=ON : c_bank/2 16KB align)
	int				reduction_width;	// 1/16 reduction width  size	*128  align
	int				reduction_y_bank;	// 1/16 reduction y_bank size	*16KB align
	int				reduction_c_bank;	// 1/16 reduction c_bank size	*16KB align
} T_RB_BRIDGE_MAP_YC_SIZE;

typedef struct{
	int				y_ch;				//                Y area of SDRAM0/1	*RB_SDRAM_CH0/1
	int				c_ch;				//                C area of SDRAM0/1	*RB_SDRAM_CH0/1
	int				localdec_y_ch;		// local decode   Y area of SDRAM0/1	*RB_SDRAM_CH0/1
	int				localdec_c_ch;		// local decode   C area of SDRAM0/1	*RB_SDRAM_CH0/1
	int				reduction_y_ch;		// 1/16 reduction Y area of SDRAM0/1	*RB_SDRAM_CH0/1
	int				reduction_c_ch;		// 1/16 reduction C area of SDRAM0/1	*RB_SDRAM_CH0/1
} T_RB_BRIDGE_MAP_YC_CH;

typedef struct{
	unsigned long	y_addr;				//                Y area top address	*16KB align
	unsigned long	c_addr;				//                C area top address	*16KB align
	unsigned long	localdec_y_addr;	// local decode   Y area top address	*16KB align
	unsigned long	localdec_c_addr;	// local decode   C area top address	*16KB align
	unsigned long	reduction_y_addr;	// 1/16 reduction Y area top address	*16KB align
	unsigned long	reduction_c_addr;	// 1/16 reduction C area top address	*16KB align
} T_RB_BRIDGE_MAP_YC_ADDR;

typedef struct{
	int							tb_flag;			// Top/Bottom Separator flag	* RB_ON/OFF
	T_RB_BRIDGE_MAP_YC_SIZE		map_yc_size;		// map yc size
	T_RB_BRIDGE_MAP_YC_CH		map_yc_ch;			// map yc ch
	T_RB_BRIDGE_MAP_YC_ADDR		map_yc_addr;		// map yc address
} T_RB_BRIDGE_MAP_YC;

typedef struct{
	int		PME_Vector_Read;					// set default RB_SDRAM_IF_CHB
	int		PME_Vector_Write;					// set default RB_SDRAM_IF_CHB
	int		EDGE_Read;							// set default RB_SDRAM_IF_CHB
	int		EDGE_Write;							// set default RB_SDRAM_IF_CHB
	int		UBG_Read;							// set default RB_SDRAM_IF_CHB
	int		UBG_Write;							// set default RB_SDRAM_IF_CHB
	int		Activity_Read;						// set default RB_SDRAM_IF_CHB
	int		Activity_Write;						// set default RB_SDRAM_IF_CHB
	int		Stream0_Read_Acore;					// set default RB_SDRAM_IF_CHB
	int		Stream0_Write_Acore;				// set default RB_SDRAM_IF_CHB
	int		Stream1_Read_Acore;					// set default RB_SDRAM_IF_CHB
	int		Stream1_Write_Acore;				// set default RB_SDRAM_IF_CHB
	int		CME_Vector_Read_Acore;				// set default RB_SDRAM_IF_CHB
	int		CME_Vector_Write_Acore;				// set default RB_SDRAM_IF_CHB
	int		COEF_Read_Acore;					// set default RB_SDRAM_IF_CHB
	int		COEF_Write_Acore;					// set default RB_SDRAM_IF_CHB
	int		Stream0_Read_Bcore;					// set default RB_SDRAM_IF_CHB
	int		Stream0_Write_Bcore;				// set default RB_SDRAM_IF_CHB
	int		Stream1_Read_Bcore;					// set default RB_SDRAM_IF_CHB
	int		Stream1_Write_Bcore;				// set default RB_SDRAM_IF_CHB
	int		CME_Vector_Read_Bcore;				// set default RB_SDRAM_IF_CHB
	int		CME_Vector_Write_Bcore;				// set default RB_SDRAM_IF_CHB
	int		COEF_Read_Bcore;					// set default RB_SDRAM_IF_CHB
	int		COEF_Write_Bcore;					// set default RB_SDRAM_IF_CHB
	int		SEQ_Read;							// set default RB_SDRAM_IF_CHB
	int		SEQ_Write;							// set default RB_SDRAM_IF_CHB
	int		CBUS_Read;							// set default RB_SDRAM_IF_CHB
	int		CBUS_Write;							// set default RB_SDRAM_IF_CHB
	int		CC_1_1_Ref_Picture_Y_Read_Acore;	// set default RB_SDRAM_IF_CHC
	int		CC_1_1_Ref_Picture_C_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		CC_1_1_Ref_Picture_Y_Write_Acore;	// set default RB_SDRAM_IF_CHA
	int		CC_1_1_Ref_Picture_C_Write_Acore;	// set default RB_SDRAM_IF_CHA
	int		CC_1_1_Org_Picture_Y_Read_Acore;	// set default RB_SDRAM_IF_CHC
	int		CC_1_1_Org_Picture_C_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		PP_1_1_Org_Picture_Y_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		PP_1_1_Org_Picture_C_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_Y_Read_Acore;		// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_C_Read_Acore;		// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_Y_Write_Acore;		// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_C_Write_Acore;		// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Org_Picture_Y_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Org_Picture_C_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Ref_Picture_Y_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Ref_Picture_C_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		CC_1_1_Ref_Picture_Y_Read_Bcore;	// set default RB_SDRAM_IF_CHC
	int		CC_1_1_Ref_Picture_C_Read_Bcore;	// set default RB_SDRAM_IF_CHA
	int		CC_1_1_Ref_Picture_Y_Write_Bcore;	// set default RB_SDRAM_IF_CHA
	int		CC_1_1_Ref_Picture_C_Write_Bcore;	// set default RB_SDRAM_IF_CHA
	int		CC_1_1_Org_Picture_Y_Read_Bcore;	// set default RB_SDRAM_IF_CHC
	int		CC_1_1_Org_Picture_C_Read_Bcore;	// set default RB_SDRAM_IF_CHA
	int		PP_1_1_Org_Picture_Y_Read_Bcore;	// set default RB_SDRAM_IF_CHA
	int		PP_1_1_Org_Picture_C_Read_Bcore;	// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_Y_Read_Bcore;		// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_C_Read_Bcore;		// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_Y_Write_Bcore;		// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_C_Write_Bcore;		// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Org_Picture_Y_Read_Bcore;	// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Org_Picture_C_Read_Bcore;	// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Ref_Picture_Y_Read_Bcore;	// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Ref_Picture_C_Read_Bcore;	// set default RB_SDRAM_IF_CHA
} T_RB_BRIDGE_MAP_PORT;

//----------------------
// INIT_PARAM
//----------------------
#ifdef CO_RB_VUI_ON
//  Encode: VUI (Video Usability Information)
typedef struct{
	int					video_signal_type_present_flag;		// video flag					*RB_ON/RB_OFF
	unsigned long		video_format;						//  + video_format				* 3bit data (0~5)
	int					video_full_range_flag;				//  + video_full_range_flag		*RB_ON/RB_OFF
	int					colour_description_present_flag;	//  + color flag				*RB_ON/RB_OFF
	unsigned long		colour_primaries;					//     + colour_primaries		* 8bit data
	unsigned long		transfer_characteristics;			//     + transfer_chara			* 8bit data
	unsigned long		matrix_coefficients;				//     + matrix_coefficients	* 8bit data
	int					aspect_ratio_info_present_flag;		// aspect_ratio_info_present_flag	*RB_ON/RB_OFF
	int					aspect_ratio_idc;					//  + aspect_ratio_idc				* 0		Unspecified
															//									* 1		1:1("square")
															//									* 2		12:11
															//									* 3		10:11
															//									* 4		16:11
															//									* 5		40:33
															//									* 6		24:11
															//									* 7		20:11
															//									* 8		32:11
															//									* 9		80:33
															//									*10		18:11
															//									*11		15:11
															//									*12		64:33
															//									*13		160:99
															//									*14		4:3
															//									*15		3:2
															//									*16		2:1
															//									*17-254	reserved
															//									*255	Extended_SAR
	int					sar_width;							//  + sar_width						*16bit
	int					sar_height;							//  + sar_height					*16bit
#ifdef CO_RB_VUI_CHROMA_LOC_INFO_ON
	int					chroma_loc_info_present_flag;		// chroma_loc_info_present_flag flag	*RB_ON/RB_OFF
	int					chroma_sample_loc_top_field;		// chroma_sample_loc_top_field flag		*3bit
	int					chroma_sample_loc_bottom_field;		// chroma_sample_loc_bottom_field flag	*3bit
#endif
} T_RB_INITPARAM_VUI;
#endif

#ifdef CO_RB_SCALINGLIST_ON
//  Encode: External ScalingList
typedef struct{
	int	ScalingListFlag;							// External ScalingList enable		*RB_ON/OFF
	int	ScalingList4x4_Y1[16];						// Y  ScalingList4x4(1)
	int	ScalingList4x4_Cb1[16];						// Cb ScalingList4x4(1)
	int	ScalingList4x4_Cr1[16];						// Cr ScalingList4x4(1)
	int	ScalingList4x4_Y2[16];						// Y  ScalingList4x4(2)
	int	ScalingList4x4_Cb2[16];						// Cb ScalingList4x4(2)
	int	ScalingList4x4_Cr2[16];						// Cr ScalingList4x4(2)
	int	ScalingList8x8_Y1[64];						// Y  ScalingList8x8(1)
	int	ScalingList8x8_Y2[64];						// Y  ScalingList8x8(2)
} T_RB_INITPARAM_SCALINGLIST;
#endif

#ifdef CO_V_Q_DFVLD_ON
// Encode: DeblockingFilter
typedef struct{
	int	V_Q_DfVld;									// DeblockingFilter enable			*RB_ON/OFF
	int	V_Q_DfAlpha;								// Alpha param(4bit)
	int	V_Q_DfBeta;									// Beta  param(4bit)
} T_RB_INITPARAM_V_Q_DFVLD;
#endif

// [Encode]
typedef struct{
	int					size;			// image size			*RB_SIZE_xxx
	int					ratemode;		// rate mode			*RB_RATE_MODE_CBR/VBR/CONSTANT_QP
	int					encmode;		// enc mode				*RB_ENC_MODE_MP4/TS
	unsigned long		bitrate;		// bit rate				*(kbps)
	unsigned long		avebitrate;		// ave bit rate			*(kbps)
	int					framerate;		// frame rate			*RB_FR_xxx
	int					scan_mode;		// scan mode mode		*RB_SCAN_MODE_PROGRESSIVE/INTERLACE
	int					gop_struct;		// GOP stracture type	*RB_GOP_STR_xxx
	unsigned long		gop_size;		// frame_count_in_gop	*(count num)
	int					cl_gop;			// ClosedGOP enable		*RB_CLOSED_GOP_ON/OFF
	int					profile;		// profile				*RB_PROFILE_HIGH/MAIN/CONSTRAINED_BASELINE
	int					level;			// LevelIdc Level * 10
	int					vlcmode;		// vlc mode				*RB_VLC_MODE_CABAC/CAVLC
	int					insert_pic_struct;			// PictureTimingSEI PicStruct insert		*RB_ON/OFF
	int					insert_buf_ctrl_param;		// hrd_parameters()@VUI
													// BuffeingPeriodSEI
													// removal_delay@PictureTimingSEI insert	*RB_ON/OFF
	int					insert_recovery_point_sei;	// RecoveryPointSEI insert					*RB_ON/OFF
	int					end_of_stream_en;			// EndOfStream insert						*RB_ON/OFF
	int					end_of_sequence_en;			// EndOfSequence insert						*RB_ON/OFF
	int					cpb_over_en;				// CPB Over enable		*RB_ON/OFF
	int					ubg_en;						// next frame mode		*RB_ON/OFF
	int					filler_en;					// Filler Data insert	*RB_ON/OFF
	int					max_qp;						// Maximum QP			*(0-51) 51:not used
	int					min_qp;						// Minimum QP			*(0-51) 0 :not used
	int					initial_cpb_removal_delay;	// initial_cpb_removal_delay(msec)
	int					vec_info_en;				// Vector info enalbe	*RB_ON/OFF
	int					frame_crop_top_offset;		// top    offset pixel croping value
	int					frame_crop_bottom_offset;	// bottom offset pixel croping value
	int					frame_crop_right_offset;	// right  offset pixel croping value
	int					frame_crop_left_offset;		// left   offset pixel croping value
#ifdef CO_RB_SEARCH_RANGE_ON
	int					search_range_sel;			// search range select
#endif
#ifdef CO_RB_SLICE_ON
	int					slice_notify;				// slice nitify mode
	int					slice;						// slice mode select
#endif
#ifdef CO_IMAGE_QUALITY_MODE_ON
	int					image_quality_mode;			// Image Quality mode
	int					qp_lambda_offset;
	int					inter_intra_weight_ctrl;	// inter_intra_weight_ctrl
#endif
#ifdef CO_MB_LIGHT_MODE
	int					mb_light_mode;				// MB Light mode							* RB_ON/OFF
#endif
#ifdef CO_FIXQP_EN_ON
	int					fixqp_en;					// fixqp mode								* RB_ON/OFF
#endif
#ifdef CO_RB_VUI_ON
	T_RB_INITPARAM_VUI*			vui;				// Video Usability Information
#endif
#ifdef CO_RB_SCALINGLIST_ON
	T_RB_INITPARAM_SCALINGLIST*	scalinglist;		// External ScalingList
#endif
#ifdef CO_V_Q_DFVLD_ON
	T_RB_INITPARAM_V_Q_DFVLD*	v_q_dfvld;			// DeblockingFilter
#endif
	int					noirq_mode;					// ENC_REQ NO IRQ			* RB_ON/OFF
	int					vrawinfo_interval;			// VRAW_FREE  count IRQ		* (count+1)
	int					vstrminfo_interval;			// VSTRM_DONE count IRQ		* (count+1)
} T_RB_INITPARAM_ENC;

// [Decode]
typedef struct{
	int					size;			// image size			*RB_SIZE_xxx
	int					framerate;		// frame rate			*RB_FR_xxx
	int					scan_mode;		// scan mode mode		*RB_SCAN_MODE_PROGRESSIVE/INTERLACE
	int					trick;			// trick flg 			*RB_ON/OFF
	unsigned int		keep_idx;		// keep bank index		*(index No)
	unsigned int		bank_num;		// use VDPB bank num
	unsigned int		self_rec_play;	// self recoding play	*RB_ON/OFF
} T_RB_INITPARAM_DEC;

//----------------------
// FIFO
//----------------------
typedef struct{
	unsigned int  ch;					// ch
	unsigned int  index;				// fifo index
} T_RB_FIFO_INDEX;

//----------------------
// INFO_TABLE
//----------------------
#ifdef CO_RB_PARTICAL_CODING_ON
typedef struct{
	int		start_position;				// start position
	int		width_size;					// width size(MB)
	int		height_size;				// hight size(MB)
	int		qp_down_range;				// QP down range
	int		qp_min;						// QP min
	int		adap_qp_en;					// ON/OFF select
} T_RB_INFOTABLE_SPECIFIC_FIELD;

typedef struct{
	int								field_num;				// partical coding number 0:no select 1:1select 2:1 and 2select
	T_RB_INFOTABLE_SPECIFIC_FIELD	field[2];				// partical coding
	int								out_field_qp_up_range;	// outside partical coding : QP up range
	int								out_field_qp_max;		// outside partical coding : QP max
	int								out_field_adap_qp_en;	// outside partical coding : ON/OFF select
} T_RB_INFOTABLE_PARTICAL_CODING;
#endif

typedef struct{
	int		pic_scaling_matrix_present_flag;	// pic_scaling_matrix_present_flag					*RB_ON/RB_OFF
	int		scalinglist_Y_sel;					// Y-ScalingList select  0:default 1/2:ScalingList(1/2)
	int		scalinglist_C_sel;					// C-ScalingList select  0:default 1/2:ScalingList(1/2)
} T_RB_INFOTABLE_SCALINGLIST;

// VRAW_info_table
typedef struct{
	unsigned long long serial_num;		// (for debug)						*user setting
	unsigned long end_flag;				// encode end request flag			*0:OFF, 1:ON
	unsigned long pic_type;				// IDR/I/P/B picture type			*RB_PIC_xxx
	unsigned long frame_count_in_gop;	// VOP num (in GOP)
	long long     PTS;					// PTS								*user setting value
	unsigned long X_offset;				// VIS: X-offset
	unsigned long Y_offset;				// VIS: Y-offset
	unsigned long physical_bank_idx;	// physical bank index
	unsigned long gop_size;				// change frame_count_in_gop		*(count num)
	unsigned long gop_struct;			// change GOP stracture type		*RB_GOP_STR_xxx
#ifdef CO_RB_3DNR_ENABLE
	int           nr3d;					// 3DNR level						*RB_3DNR_xxx
#endif
#ifdef CO_RB_PARTICAL_CODING_ON
	T_RB_INFOTABLE_PARTICAL_CODING*	partical_coding;	// partical coding
#endif
#ifdef CO_RB_SCALINGLIST_ON
	T_RB_INFOTABLE_SCALINGLIST*		scalinglist;		// external ScalingList
#endif
#ifdef CO_RB_THIN_OUT_FRAME_NUM_ON
	unsigned long thin_out_frame_num;					// thin_out_frame_num value (0-15)
#endif
	unsigned long constant_qp;							// constaant QP value
	unsigned long bitrate;								// target  bitrate(kbps) used CBR/I-pic
	unsigned long avebitrate;							// average bitrate(kbps) used VBR/I-pic
	
} T_RB_INFOTABLE_VRAW;

// VSTRM_info_table
typedef struct{
	unsigned long long serial_num;		// (for debug)						*ribery setting
	unsigned long long VRAW_serial_num;	// (for debug)						*VRAW user setting value that through
	unsigned long end_flag;				// 									*VRAW user setting flag that through
	unsigned long pic_type;				//									*VRAW user setting value that through
	unsigned long frame_count_in_gop;	//
	long long     PTS;					// PTS								*user setting value
	long long     DTS;					// DTS								*ribery calculatevalue
	unsigned long PES_header_stream_start_addr;						// PES header start address
	unsigned long first_stream_size;								// first_picture/field(stream) size
	unsigned long first_stream_start_addr;							// first_picture/field(stream) start address
	unsigned long first_nal_unit_num;								// NAL_unit num
	unsigned long first_VBV_boc;									// first VBV boc
	unsigned long first_rep_tbl_buf_num[RB_NAL_UNIT_SIZE_TBL_NUM];	// NAL unit size
	unsigned long second_stream_size;								// second_field(stream) size				* mode progressive don't care
	unsigned long second_stream_start_addr;							// second_field(stream) start address		* mode progressive don't care
	unsigned long second_nal_unit_num;								// NAL_unit num								* mode progressive don't care
	unsigned long second_VBV_boc;									// second VBV boc							* mode progressive don't care
	unsigned long second_rep_tbl_buf_num[RB_NAL_UNIT_SIZE_TBL_NUM];	// NAL unit size							* mode progressive don't care
#ifdef CO_RB_PROCESSCUT_DEBUG
	unsigned long ProcessCut_flag;
	unsigned long TimeOutCounter;
#endif
	unsigned long a_streambuf_write_limit_ptr;						// coreA stream buffer write pointer
	unsigned long b_streambuf_write_limit_ptr;						// coreB stream buffer write pointer
	unsigned long ch;												// ch
	unsigned long VRAW_frame_count_in_gop;							// VOP num (in GOP)
	unsigned long first_ave_qp;										// first  field average QP
	unsigned long second_ave_qp;									// second field average QP
	unsigned long first_cpb_over_flag;								// first  field cpb buffer over flag(0:not over 1:over)
	unsigned long second_cpb_over_flag;								// second field cpb buffer over flag(0:not over 1:over)
    long          poc;												// poc value
	unsigned long first_bitrate;									// first  field bit rate		*(bps)
	unsigned long second_bitrate;									// second field bit rate		*(bps)
	unsigned long avebitrate;										// ave bit rate					*(bps)
#ifdef CO_RB_SLICE_ON
	unsigned long slice_notify;										// slice_notify
	unsigned long slice_num;										// slice number
	unsigned long max_slice_num;									// max_slice_num
#endif
} T_RB_INFOTABLE_VSTRM;

// Y_VEC_info_table
typedef struct{
	unsigned int  ch;					// ch
	unsigned long long vraw_serial_num;	// (for debug)						*VRAW user setting value that through
	unsigned long pic_type;				//									*VRAW user setting value that through
	unsigned int  y_info_bank_idx;		// y_info_bank_idx
	unsigned long y_info_bank_addr;		// y_info_bank address
	unsigned int  vec_info_bank_idx;	// vec_info_bank_idx
	unsigned long vec_info_bank_addr;	// vec_info_bank address
	long long     PTS;					// PTS
	long long     DTS;					// DTS
	unsigned long bitrate;				// bit rate				*(bps)
	unsigned long avebitrate;			// ave bit rate			*(bps)
} T_RB_INFOTABLE_Y_VEC;

// VCPB_info_table
typedef struct{
	unsigned long serial_num;			// (for debug) 						*user setting
	long long     PTS;					// PTS								*user setting value
	long long     DTS;					// DTS								*user setting value
	unsigned long pic_size;				// picture/field(stream) size
	unsigned long pic_addr;				// picture/field(stream) start address
} T_RB_INFOTABLE_VCPB;

// VDPB_info_table
typedef struct{
	unsigned long serial_num;			// (for debug)						*ribery setting
	unsigned long VCPB_serial_num;		// (for debug)						*VCPB user setting value that through
	long long     PTS;					// PTS								*user setting value
	long long     DTS;					// DTS								*user setting value
	unsigned long err_code;				// input stream error				*ribery setting
} T_RB_INFOTABLE_VDPB;


/*----------------------------------------------------------------------*/
/* Changes Definition to User Function									*/
/*----------------------------------------------------------------------*/
#ifdef CO_RB_ACT_CLOCK
#define	RB_AHclock_Set(arg)		H264_Ribery_AHclock_Set(arg)	// user definition
extern 	void H264_Ribery_AHclock_Set(unsigned char arg);
#else
#define	RB_AHclock_Set(arg)
#endif
#define	RB_Clean(addr,size)		H264_Ribery_Clean(addr,size)	// user definition
extern 	void H264_Ribery_Clean( unsigned long addr, unsigned long size );
#define	RB_Flush(addr,size)		H264_Ribery_Flush(addr,size)	// user definition
extern 	void H264_Ribery_Flush( unsigned long addr, unsigned long size );

#define	RB_DSB(void)			H264_Ribery_DSB(void)			// user definition
extern 	void H264_Ribery_DSB( void );

#define RB_DI(void)				H264_Ribery_DI(void)			// user definition
extern	int H264_Ribery_DI( void );

#define RB_EI(status)			H264_Ribery_EI(status)			// user definition
extern	void H264_Ribery_EI( unsigned int status );


/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
//----------------------
// Interrupt Handler
//----------------------
extern	void	RB_HRWDT_IntHandler( void );
extern	void	RB_HRBR_IntHandler( void );

//----------------------
// Int
//----------------------
// common
extern	void	RB_Int_Set_Callback( void (*wdt_handler)(void), void (*rbr_handler)(void) );
// for normal enc/dec/transcode(enc)
extern	int		RB_Int_Set_Request( unsigned short request );
extern	int		RB_Int_Get_Status( unsigned short *irq );
extern	int		RB_Int_Get_ErrStatus( unsigned short *irq );
extern	void	RB_Int_Clear_Status( unsigned short irq );
extern	void	RB_Int_Clear_ErrStatus( unsigned short irq );
extern	void	RB_Int_Set_Enable( unsigned short irq );
extern	void	RB_Int_Set_ErrEnable( unsigned short irq );
extern	int		RB_Int_Get_Enable( unsigned short *irq );
extern	int		RB_Int_Get_ErrEnable( unsigned short *irq );
// for transcode(dec)
extern	int		RB_Int_Set_Request2( unsigned short request );
extern	int		RB_Int_Get_Status2( unsigned short *irq );
extern	int		RB_Int_Get_ErrStatus2( unsigned short *irq );
extern	void	RB_Int_Clear_Status2( unsigned short irq );
extern	void	RB_Int_Clear_ErrStatus2( unsigned short irq );
extern	void	RB_Int_Set_Enable2( unsigned short irq );
extern	void	RB_Int_Set_ErrEnable2( unsigned short irq );
extern	int		RB_Int_Get_Enable2( unsigned short *irq );
extern	int		RB_Int_Get_ErrEnable2( unsigned short *irq );

//----------------------
// Bridge
//----------------------
// common
extern	void	RB_Bridge_Release( int codec, int core_mode );
extern	void	RB_Bridge_Reset( void );
extern	int		RB_Bridge_Set_MemoryMap_Port( T_RB_BRIDGE_MAP_PORT* map_port );
// for normal enc/dec/transcode(enc) mode
extern	int		RB_Bridge_Set_MemoryMap_EncCommon( T_RB_BRIDGE_MAP_COMMON* map_common );
extern	int		RB_Bridge_Set_MemoryMap_DecCommon( T_RB_BRIDGE_MAP_COMMON* map_common );
extern	int		RB_Bridge_Set_MemoryMap_EncYC( int ch, T_RB_BRIDGE_MAP_YC* map_yc );
extern	int		RB_Bridge_Set_MemoryMap_DecYC( T_RB_BRIDGE_MAP_YC* map_yc );
// for transcode(dec) mode
extern	int		RB_Bridge_Set_MemoryMap_DecCommon2( T_RB_BRIDGE_MAP_COMMON* map_common );
extern	int		RB_Bridge_Set_MemoryMap_DecYC2( T_RB_BRIDGE_MAP_YC* map_yc );

//----------------------
// CPU
//----------------------
// common
extern	void	RB_Cpu_Firm_Unlock_Start( void );
extern	int		RB_Cpu_Firm_Download( int codec );
// for normal enc/dec/transcode(enc) mode
extern	void	RB_Cpu_Boot( void );
extern	void	RB_Cpu_Reset( void );
// for transcode(dec) mode
extern	void	RB_Cpu_Boot2( void );
extern	void	RB_Cpu_Reset2( void );

//----------------------
// Init Param
//----------------------
// for normal enc/dec/transcode(enc) mode
extern	void	RB_InitParam_Encode_Init( void );
extern	void	RB_InitParam_Decode_Init( void );
extern	void	RB_InitParam_Set_Encode_Status( int ch, int value );
extern	int		RB_InitParam_Get_Encode_Status( int ch );
extern	int		RB_InitParam_Set_Encode( int ch, T_RB_INITPARAM_ENC* initparam );
extern	int		RB_InitParam_Set_Decode( T_RB_INITPARAM_DEC* initparam );
// for transcode(dec) mode
extern	void	RB_InitParam_Decode_Init2( void );
extern	int		RB_InitParam_Set_Decode2( T_RB_INITPARAM_DEC* initparam );

//----------------------
// Info Table
//----------------------
// for normal enc/dec/transcode(enc) mode
extern	void	RB_InfoTable_Vraw_Init( void );
extern	int		RB_InfoTable_Vraw_Free( unsigned int count, T_RB_FIFO_INDEX* index );
extern	int		RB_InfoTable_Vraw_Set( T_RB_FIFO_INDEX index, T_RB_INFOTABLE_VRAW* info );
extern	int		RB_InfoTable_Vraw_Get( T_RB_FIFO_INDEX index, T_RB_INFOTABLE_VRAW* info );
extern	int		RB_InfoTable_Vstrm_Get( unsigned int count, T_RB_FIFO_INDEX* index, T_RB_INFOTABLE_VSTRM* info );
extern	int		RB_InfoTable_Y_VEC_Get( unsigned int index, T_RB_INFOTABLE_Y_VEC* info );
extern	void	RB_InfoTable_Vcpb_Init( void );
extern	int		RB_InfoTable_Vcpb_Free( unsigned int index );
extern	int		RB_InfoTable_Vcpb_Set( unsigned int index, T_RB_INFOTABLE_VCPB* info );
extern	int		RB_InfoTable_Vdpb_Get( unsigned int index, T_RB_INFOTABLE_VDPB* info );
// for transcode(dec) mode
extern	void	RB_InfoTable_Vcpb_Init2( void );
extern	int		RB_InfoTable_Vcpb_Free2( unsigned int index );
extern	int		RB_InfoTable_Vcpb_Set2( unsigned int index, T_RB_INFOTABLE_VCPB* info );
extern	int		RB_InfoTable_Vdpb_Get2( unsigned int index, T_RB_INFOTABLE_VDPB* info );

//----------------------
// FIFO
//----------------------
// for normal enc/dec/trabscode(enc) mode
extern	int		RB_Fifo_Vraw_EncReq_Set( unsigned int count, T_RB_FIFO_INDEX* index );
extern	int		RB_Fifo_Vraw_Free_Get( unsigned int* count, T_RB_FIFO_INDEX* index );
extern	int		RB_Fifo_Vstrm_EncDone_Get( unsigned int* count, T_RB_FIFO_INDEX* index );
extern	int		RB_Fifo_Vstrm_Free_Set( unsigned int count, T_RB_FIFO_INDEX* index );
extern	int		RB_Fifo_Y_VEC_Get( unsigned int* index );
extern	int		RB_Fifo_Vcpb_DecReq_Set( unsigned int index );
extern	int		RB_Fifo_Vcpb_Free_Get( unsigned int* index );
extern	int		RB_Fifo_Vdpb_DecDone_Get( unsigned int* index );
extern	int		RB_Fifo_Vdpb_Free_Set( unsigned int index );
// for transcode(dec) mode
extern	int		RB_Fifo_Vcpb_DecReq_Set2( unsigned int index );
extern	int		RB_Fifo_Vcpb_Free_Get2( unsigned int* index );
extern	int		RB_Fifo_Vdpb_DecDone_Get2( unsigned int* index );
extern	int		RB_Fifo_Vdpb_Free_Set2( unsigned int index );

#ifdef CO_RB_ERROR_INFO_ON
//----------------------
// Common
//----------------------
extern	void	RB_Common_ErrorInfo( unsigned short *info );
#endif

#ifdef CO_RB_DEBUG
extern	int		RB_Cpu_Firm_Download_Manual( unsigned int codec,
											 unsigned int enc_mvdata_1_adr,
											 unsigned int enc_mvcode_1_adr,
											 unsigned int enc_svadata_1_adr,
											 unsigned int enc_svacode_1_adr,
											 unsigned int enc_svbdata_1_adr,
											 unsigned int enc_svbcode_1_adr,
											 unsigned int dec_mvdata_1_adr,
											 unsigned int dec_mvcode_1_adr,
											 unsigned int dec_svadata_1_adr,
											 unsigned int dec_svacode_1_adr,
											 unsigned int dec_svbdata_1_adr,
											 unsigned int dec_svbcode_1_adr );
extern	void	RB_test( void );
#endif



#ifdef __cplusplus
}
#endif

#endif /* _RB_H_ */
