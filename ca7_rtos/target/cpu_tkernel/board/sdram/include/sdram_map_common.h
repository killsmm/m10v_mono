/**
 * @file		sdram_map_common.h
 * @brief		SocioNext SDRAM Common Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _SDRAM_MAP_COMMON_H_
#define _SDRAM_MAP_COMMON_H_

//#define CO_Sdram_Map_Common_DOUBLE_FRAME_BUFFER
#ifdef CO_Sdram_Map_Common_DOUBLE_FRAME_BUFFER
extern int gSdram_Map_Osd_buf_index_plus;
#endif

#include "sdram_map_define.h"

#include "fj_custom.h"
#include "fj_still.h"
#include "fj_memory.h"
#include "fj_disp.h"
#include "im_disp.h"

// Top of SDRAM address
// for Linux Co-op
//#define D_SDRAM_MAP_COMMON_SDRAM_BASE			(0x40000000UL)
//#define D_SDRAM_MAP_COMMON_DCACHE_OR			(0x20000000UL)	// set D-cache area
#define D_SDRAM_MAP_COMMON_SDRAM_BASE			(0x70000000UL)
#define D_SDRAM_MAP_COMMON_DCACHE_OR			(0x40000000UL)	// set D-cache area
#define D_SDRAM_MAP_COMMON_DCACHE_AND			(0x3FFFFFFFUL)	// set D-cache area
#define D_SDRAM_MAP_COMMON_DCACHE_PHY2LOG		(0x80000000UL)	// set D-cache area
#define D_SDRAM_MAP_COMMON_DCACHE_LOG2PHY		(0x40000000UL)	// set D-cache area
#define D_SDRAM_MAP_COMMON_DCACHE_60P			(0xC0000000UL)	// set D-cache area

// Bytes of SDRAM
// for Linux Co-op
//#define D_SDRAM_MAP_COMMON_SDRAM_SIZ			(0x20000000UL)
#define D_SDRAM_MAP_COMMON_SDRAM_SIZ			(0x10000000UL)


// End of SDRAM address
#define D_SDRAM_MAP_COMMON_SDRAM_END			(D_SDRAM_MAP_COMMON_SDRAM_BASE + D_SDRAM_MAP_COMMON_SDRAM_SIZ)

// End of Firmware area
// for Linux Co-op
//#define D_SDRAM_MAP_COMMON_SDRAM_FW_END			(0x48200000UL)
#define D_SDRAM_MAP_COMMON_SDRAM_FW_END			(0x78200000UL)


// Top of IMG-SRAM address
#define D_SDRAM_MAP_COMMON_IMG_SRAM_TOP			(0x30000000UL)

#define SDRAM_BNK_LIMIT(pos, total) ((pos < total) ? pos : 0)

/* Size */
#define WIDTH_36M			(7360)						/* temporary */
#define LINES_36M			(4912)						/* temporary */
#define PIX_SIZ_36M			(WIDTH_36M * LINES_36M)		/* temporary */

#define WIDTH_22M			(11520)						/* temporary */
#define LINES_22M			(1920)						/* temporary */
#define PIX_SIZ_22M			(WIDTH_22M * LINES_22M)		/* temporary */

#define WIDTH_20M			(5184)						/* temporary */
#define LINES_20M			(3888)						/* temporary */
#define PIX_SIZ_20M			(WIDTH_20M * LINES_20M)		/* temporary */

#define WIDTH_16M			(4608)
#define LINES_16M			(3456)
#define PIX_SIZ_16M			(WIDTH_16M * LINES_16M)

#define WIDTH_14M			(4320)
#define LINES_14M			(3240)
#define PIX_SIZ_14M			(WIDTH_14M * LINES_14M)

#define WIDTH_12M			(4000)
#define LINES_12M			(3000)
#define PIX_SIZ_12M			(WIDTH_12M * LINES_12M)

#define WIDTH_12M_3_2		(4000)
#define LINES_12M_3_2		(2672)
#define PIX_SIZ_12M_3_2		(WIDTH_12M_3_2 * LINES_12M_3_2)

#define WIDTH_12M_16_9		(4000)
#define LINES_12M_16_9		(2248)
#define PIX_SIZ_12M_16_9	(WIDTH_12M_16_9 * LINES_12M_16_9)

#define WIDTH_10M			(3648)
#define LINES_10M			(2736)
#define PIX_SIZ_10M			(WIDTH_10M * LINES_10M)

#define WIDTH_10M_3_2		(3648)
#define LINES_10M_3_2		(2432)
#define PIX_SIZ_10M_3_2		(WIDTH_10M_3_2 * LINES_10M_3_2)

#define WIDTH_10M_16_9		(3648)
#define LINES_10M_16_9		(2056)
#define PIX_SIZ_10M_16_9	(WIDTH_10M_16_9 * LINES_10M_16_9)

#define WIDTH_8M			(3264)
#define LINES_8M			(2448)
#define PIX_SIZ_8M			(WIDTH_8M * LINES_8M)

#define WIDTH_8M_3_2		(3264)
#define LINES_8M_3_2		(2176)
#define PIX_SIZ_8M_3_2		(WIDTH_8M_3_2 * LINES_8M_3_2)

#define WIDTH_8M_16_9		(3264)
#define LINES_8M_16_9		(1836)
#define PIX_SIZ_8M_16_9		(WIDTH_8M_16_9 * LINES_8M_16_9)

#define WIDTH_7M			(3072)
#define LINES_7M			(2304)
#define PIX_SIZ_7M			(WIDTH_7M * LINES_7M)

#define WIDTH_6M			(2816)
#define LINES_6M			(2112)
#define PIX_SIZ_6M			(WIDTH_6M * LINES_6M)

#define WIDTH_5M			(2592)
#define LINES_5M			(1944)
#define PIX_SIZ_5M			(WIDTH_5M * LINES_5M)

#define WIDTH_4M			(2304)
#define LINES_4M			(1728)
#define PIX_SIZ_4M			(WIDTH_4M * LINES_4M)

#define WIDTH_3M			(2048)
#define LINES_3M			(1536)
#define PIX_SIZ_3M			(WIDTH_3M * LINES_3M)

#define WIDTH_3M_16_9		(1920)
#define LINES_3M_16_9		(1080)
#define PIX_SIZ_3M_16_9		(WIDTH_3M_16_9 * LINES_3M_16_9)

#define WIDTH_2M			(1600)
#define LINES_2M			(1200)
#define PIX_SIZ_2M			(WIDTH_2M * LINES_2M)

#define WIDTH_1440			(1440)
#define LINES_1440			(1080)
#define PIX_SIZ_1440		(WIDTH_1440 * LINES_1440)

#define WIDTH_1_2M			(1280)
#define LINES_1_2M			(960)
#define LINES_1_2M_3_2		(854)
#define LINES_1_2M_16_9		(720)
#define PIX_SIZ_1_2M		(WIDTH_1_2M * LINES_1_2M)
#define PIX_SIZ_1_2M_16_9	(WIDTH_1_2M * LINES_1_2M_16_9)

#define WIDTH_1M			(1024)
#define LINES_1M			(768)
#define PIX_SIZ_1M			(WIDTH_1M * LINES_1M)

#define WIDTH_VGA			(640)
#define WIDTH_VGA_AFN		(704)
#define LINES_VGA			(480)
#define LINES_VGA_3_2		(426)
#define LINES_VGA_16_9		(360)
#define LINES_VGA_AFN		(528)
#define PIX_SIZ_VGA			(WIDTH_VGA * LINES_VGA)
#define PIX_SIZ_VGA_3_2		(WIDTH_VGA * LINES_VGA_3_2)
#define PIX_SIZ_VGA_16_9	(WIDTH_VGA * LINES_VGA_16_9)

#define WIDTH_HVGA			(640)
#define LINES_HVGA			(240)
#define LINES_HVGA_16_9		(180)
#define PIX_SIZ_HVGA		(WIDTH_HVGA * LINES_HVGA)

#define WIDTH_QVGA			(320)//320->360
#define WIDTH_QVGA_AFN		(384)
#define LINES_QVGA			(240)
#define LINES_QVGA_3_2		(213)
#define LINES_QVGA_16_9		(180)
#define LINES_QVGA_AFN		(256)
#define PIX_SIZ_QVGA		(WIDTH_QVGA * LINES_QVGA)
#define PIX_SIZ_QVGA_3_2	(WIDTH_QVGA * LINES_QVGA_3_2)
#define PIX_SIZ_QVGA_16_9	(WIDTH_QVGA * LINES_QVGA_16_9)

#define WIDTH_THUMB			(160)
#define LINES_THUMB			(120)
#define LINES_THUMB_3_2		(106)
#define LINES_THUMB_16_9	(90)
#define PIX_SIZ_THUMB		(WIDTH_THUMB * LINES_THUMB)
#define PIX_SIZ_THUMB_3_2	(WIDTH_THUMB * LINES_THUMB_3_2)
#define PIX_SIZ_THUMB_16_9	(WIDTH_THUMB * LINES_THUMB_16_9)

#define WIDTH_FHD			(1920)
#define WIDTH_FHD_3_2		(1620)
#define WIDTH_FHD_4_3		(1440)
#define WIDTH_FHD_AFN		(1984)
#define LINES_FHD			(1080)
#define LINES_FHD_AFN		(1152)
#define PIX_SIZ_FHD			(WIDTH_FHD      * LINES_FHD)
#define PIX_SIZ_FHD_3_2		(WIDTH_FHD_3_2  * LINES_FHD)
#define PIX_SIZ_FHD_4_3		(WIDTH_FHD_4_3  * LINES_FHD)

#define WIDTH_4K2K_QFHD	(3840)
#define WIDTH_4K2K_2_1		(4096)
#define LINES_4K2K_QFHD	(2160)
#define LINES_4K2K_2_1		(2048)
#define LINES_4K2K_QFHD_4_3	(2880)
#define WIDTH_4K2K_4096		(4096)
#define LINES_4K2K_2160		(2160)
#ifdef CO_4K2K_QFHD
#define WIDTH_4K2K				WIDTH_4K2K_QFHD
#define LINES_4K2K				LINES_4K2K_QFHD
#else
#define WIDTH_4K2K				WIDTH_4K2K_2_1
#define LINES_4K2K				LINES_4K2K_2_1
#endif
#define PIX_SIZ_4K2K		(WIDTH_4K2K      * LINES_4K2K)

#define WIDTH_3K3K			(3000)
#define LINES_3K3K			(3000)
#define PIX_SIZ_3K3K		(WIDTH_3K3K    * LINES_3K3K)

#define WIDTH_2K2K			(2000)
#define LINES_2K2K			(2000)
#define PIX_SIZ_2K2K		(WIDTH_2K2K    * LINES_2K2K)

#define WIDTH_2P7K			(2704)
#define LINES_2P7K			(1520)
#define PIX_SIZ_2P7K		(WIDTH_2P7K      * LINES_2P7K)

#define WIDTH_HD			(1280)
#define WIDTH_HD_4_3		(960)
#define WIDTH_HD_AFN		(1344)
#define LINES_HD			(720)
#define LINES_HD_AFN		(768)
#define LINES_HD_16_9			(540)
#define WIDTH_HD_256		(768)
#define LINES_HD_135		(405)
#define PIX_SIZ_HD			(WIDTH_HD       * LINES_HD)
#define PIX_SIZ_HD_4_3		(WIDTH_HD_4_3   * LINES_HD)

#define WIDTH_SD			(720)
#define WIDTH_SD_4_3		(640)
#define LINES_SD_NTSC		(480)
#define LINES_SD_PAL		(576)
#define LINES_SD_NTSC_16_9	(360)
#define LINES_SD_PAL_16_9	(432)
#define PIX_SIZ_SD_NTSC		(WIDTH_SD      * LINES_SD_NTSC)
#define PIX_SIZ_SD_PAL		(WIDTH_SD      * LINES_SD_PAL)

#define WIDTH_PAL_VGA		(WIDTH_VGA)
#define LINES_PAL_VGA		(576)
#define PIX_SIZ_PAL_VGA		(WIDTH_PAL_VGA * LINES_PAL_VGA)

#define WIDTH_PAL_QVGA		(WIDTH_QVGA)
#define LINES_PAL_QVGA		(288)
#define PIX_SIZ_PAL_QVGA	(WIDTH_PAL_QVGA * LINES_PAL_QVGA)

#define WIDTH_480x270		(480)
#define LINES_480x270		(270)
#define PIX_SIZ_480x270		(WIDTH_480x270  * LINES_480x270)

#define	WIDTH_THM			(160)
#define	LINES_THM			(120)
#define PIX_SIZ_THM			(WIDTH_THM * LINES_THM)

#define	WIDTH_RGB_DELTA		(640)
#define	LINES_RGB_DELTA		(234)
#define PIX_SIZ_RGB_DELTA	(WIDTH_RGB_DELTA * LINES_RGB_DELTA)

#define WIDTH_SXVGA			(1280)
#define LINES_SXVGA			(960)

#define WIDTH_HVGAW			(640)
#define LINES_HVGAW			(360)
#define PIX_SIZ_HVGAW		(WIDTH_HVGAW * LINES_HVGAW)

#define WIDTH_320x480		(320)
#define LINES_320x480		(480)
#define PIX_SIZ_320x480		(WIDTH_320x480  * LINES_320x480)

#define WIDTH_FWQVGA		(432)
#define LINES_FWQVGA		(240)
#define LINES_FWQVGA_4_3	(324)
#define PIX_SIZ_FWQVGA		(WIDTH_FWQVGA * LINES_FWQVGA)


#define	LINES_FILL_16_9				(306)
#define	LINES_FILL_3_2				(136)
#define	LINES_FILL_16_9_VGA			(60)
#define	LINES_FILL_3_2_VGA			(28)
#define	LINES_FILL_16_9_QVGA		(30)
#define	LINES_FILL_3_2_QVGA			(14)
#define	LINES_FILL_16_9_THUMB		(15)
#define	LINES_FILL_3_2_THUMB		(7)


#define MAX_ZOOM_RATIO_ENLARGE	(4)			// x4



#define SDRAM_SIZ_VGR				((PIX_SIZ_QVGA * FJ_OSD_COMM_RGBAXXXX))				// 320x240x4 =0x1C200


// bank definitions
//#define SDRAM_BNK_SOFTWARE	(1)
//#define SDRAM_BNK_GUI			(4)					// display(2), OG1ch buffer, OCch Buffer
#define D_SDRAM_MAP_COMMON_SDRAM_BNK_VDE			(3)

//Get OC draw buffer
//#define GET_SDRAM_OC_DRAW_BUFFER (SDRAM_ADR_REL2ABS(SDRAM_ADR_GUI_OC_DRAW))

//The OSD display layer
typedef enum {
	E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_0 = 0,	// display buffer 0
	E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_1,		// display buffer 1
	E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_2,		// display buffer 2
	E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_THE_END	// END flg
} E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF;

typedef enum {
	E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_HDMI = 0,//DISP's HDMI block
	E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_LCD ,//DISP's LCD block
	E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_END
} E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK;
typedef enum {
	E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0 = 0,//GR0
	E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_1 ,//GR1
	E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_END
} E_BF_SDRAM_MAP_COMMON_OSD_GR_NO;

typedef enum {
	E_BF_SDRAM_MAP_COMMON_COPY_NONE = 0,
	E_BF_SDRAM_MAP_COMMON_COPY_HDMI_FROM_LCD,
	E_BF_SDRAM_MAP_COMMON_COPY_LCD_FROM_HDMI,
	E_BF_SDRAM_MAP_COMMON_COPY_END
} E_BF_SDRAM_MAP_COMMON_COPY_SELECT;

typedef struct
{
	E_OSDLayer layer_name;
	E_IM_DISP_SEL_LAYER layer;
	E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no;
	E_BF_SDRAM_MAP_COMMON_OSD_GR_NO gr_no;
	UINT32 draw_area;
}T_BF_SDRAM_MAP_COMMON_OSD;



//---------------------------//
// Common(Made and Non-Made) //
//---------------------------//

#if FJ_OSD_COMM_HOC_SOFT_LAYER_ON == 1
VOID Set_SDRAM_GUI_HOC_Address(ULONG icon_layer,ULONG image_layer);
#endif

// VGR
extern ULONG Sdram_Map_Osd_Get_Current_OSD_Addr(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data);
extern ULONG Sdram_Map_Osd_Get_OSD_Addr(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data);
extern ULONG Sdram_Map_Osd_Get_OSD_Chg_Addr(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data);
extern VOID Sdram_Map_Osd_Change_OSD_Area_Index(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no
											,E_BF_SDRAM_MAP_COMMON_OSD_GR_NO gr_no
											,UINT32 draw_area
											);
extern ULONG Sdram_Map_Osd_Get_OSD_Next_Buffer(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data);
extern VOID Sdram_Map_Osd_Copy_Gui_Info(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK base_block_no,
												E_BF_SDRAM_MAP_COMMON_OSD_GR_NO base_gr_no,
												E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK copy_block_no,
												E_BF_SDRAM_MAP_COMMON_OSD_GR_NO copy_gr_no);
extern VOID Sdram_Map_Osd_Set_OSD_Draw_Area_Buffer_Addr(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data,
													const T_Rect* drawArea, UINT32 drawAreaNum, E_OSDLayer used_buffer);
// Vch
extern VOID Sdram_Map_Osd_Get_VDE_Chg_Addr(FJ_DISP_TYPE disp_type, ULONG* y_addr, ULONG* u_addr);
extern ULONG Sdram_Map_Osd_Get_OSD_Buffer_Size(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data);
//---------------------------//
// for Made                  //
//---------------------------//
extern ULONG Sdram_Map_Osd_Get_WORK_Addr(VOID);

//---------------------------//
// for Non-Made              //
//---------------------------//
extern ULONG Sdram_Map_Osd_Get_TV_Work_Area(VOID);

/****************************************/
/*  View VDE RAM Control API            */
/****************************************/
extern ULONG Sdram_Map_Common_Get_Vde(VOID);
extern ULONG Sdram_Map_Common_Get_Vde_Y(VOID);
extern ULONG Sdram_Map_Common_Get_Vde_Cb(VOID);
extern ULONG Sdram_Map_Common_Get_Last_Vde(VOID);

extern ULONG Sdram_Map_Common_Change_Vde(BOOL update);
extern ULONG Sdram_Map_Common_Next_Vde(VOID);

/****************************************/
/*  HD View RAM Control API            */
/****************************************/
extern ULONG Sdram_Map_Common_Get_HDMI_HD(VOID);
extern ULONG Sdram_Map_Common_Get_HDMI_HD_Y(VOID);
extern ULONG Sdram_Map_Common_Get_HDMI_HD_Cb(VOID);
extern ULONG Sdram_Map_Common_Change_HDMI_HD(VOID);
extern ULONG Sdram_Map_Common_Next_HDMI_HD(VOID);

extern VOID Sdram_Map_Common_Get_HDMI_HD_Lsit(ULONG* addr_list);
extern VOID Sdram_Map_Common_Change_HDMI_HD_By_Current_Adrr(ULONG current_addr);
extern INT32 Sdram_Map_Common_Get_HDMI_HD_Current_Index(VOID);

extern VOID Sdram_Map_Common_Init(VOID);
extern VOID Sdram_Map_Copy_Buffer_Adr(E_BF_SDRAM_MAP_COMMON_COPY_SELECT copy_select);

#endif

