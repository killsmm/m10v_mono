/**
 * @file		user_utility_dump_rawyuv.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include <stdlib.h>
#include "user.h"
#include "fj_filesystem.h"
#include "l1l2cache.h"
#include "user_utility_dump_rawyuv.h"
#include "fj_dcf.h"
#include "os_if.h"
#include "sdram_map_define.h"
#include "im_iip.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

#define __BAYER_16BIT_WRITE
#define __YUV_CBCR_WRITE

#define D_AVAILABLE_SDRAM_CH 0
/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define ROUNDUP_8(val)	(((UINT32)(val) +7) & 0xFFFFFFF8)

#define OSD_FONT_SIZE_DEF	2		// ���”蝷箝���������(Bayer�蝝������)
#define FONTSIZE4 4
#define FONTSIZE2 2

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

typedef enum{
	E_FONT_SPACE,			// 蝛箇
	E_FONT_PRINT,			// 銵函內�
	E_FONT_LF,				// �銵�
	E_FONT_TYPE_NUM,
} E_FONT_TYPE;

// 銵函內�
typedef enum{
	E_SEL_COLOR_BACK,		// ���
	E_SEL_COLOR_TEXT,		// ���
	E_SEL_COLOR_NUM,
} E_SEL_COLOR;

// �蝔桀
typedef enum{
	E_OSD_COLOR_Y,
	E_OSD_COLOR_U,
	E_OSD_COLOR_V,
	E_OSD_COLOR_NUM,
} E_OSD_COLOR;

// enum hack
typedef enum{
	OSD_CHAR_WIDTH  = 8,	// ���璅芸��(�摰��)
	OSD_CHAR_HEIGHT = 8,	// ���蝮血��(�摰��)
} E_OSD_CHAR_SIZE;

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static BOOL g_ipcu_raw_en = FALSE;
static BOOL g_ipcu_yuv_en = FALSE;

static const UCHAR	g_font[][OSD_CHAR_WIDTH*OSD_CHAR_HEIGHT] = 
{
	#include "osd_font.csv"
};							//�������(CSV�摰儔)

static UCHAR		g_font_yuv_size = OSD_FONT_SIZE_DEF;		//文字表示サイズ(Bayer画素単位：変更可能)
static UCHAR		g_font_raw_size = OSD_FONT_SIZE_DEF;		//文字表示サイズ(Bayer画素単位：変更可能)

static UCHAR		g_font_color[E_SEL_COLOR_NUM][E_OSD_COLOR_NUM] = 
{
	{0, 128, 128},			//black for background
	{255, 128, 128},		//white for text
};

static UCHAR		g_visible_bgcolor = TRUE;				//���銵函內��

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static int conv_yuv_semi_to_yuv_chunky(const T_SHARE_LAYOUT yuvsrc, ULONG out_addr);
static E_FONT_TYPE user_utility_dump_get_font( const UCHAR** font, UCHAR ch );
static VOID user_utility_dump_font_write_bayer( UINT32 bayer_addr, USHORT pshdma, USHORT pos_x, USHORT pos_y, const UCHAR* str );
static VOID user_utility_dump_font_write_yuv( const FJ_T_ADDR_YCC *yuv_addr, USHORT pshdma, E_SHARE_YUV_FORMAT format, USHORT pos_x, USHORT pos_y, const UCHAR* str );
static int iip_bayer_memcpy(const ULONG src_addr, const ULONG dst_addr, const ULONG src_h_byte, const USHORT src_v_size, const UCHAR pixel_depth);
/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void User_Utility_SaveFile_RAW_Divided( void )
{
	T_SHARE_SYSTEM_PARAM param;
	T_SHARE_RAW raw;
	static ULONG file_cnt = 0;
	CHAR string_file_name[30];
	ULONG ret_wr_size;
	ULONG fid;
	ULONG size;
	INT32 i;
	UCHAR *raw_buf = NULL;
	T_SHARE_SYSTEM_CONFIG sys_cfg;

	Get_Share_System_Config(&sys_cfg);
	Get_Frame_Share_Sensor_Raw(&raw, 0);
	Get_Share_System_Param(&param);

	if ( sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS) {
		param.lock_address_raw = raw.rect[0].addr;
	} else if ( sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) {
		param.lock_address_raw1 = raw.rect[0].addr1;
	}
	Set_Share_System_Param(&param);

	for (i = 0; i < raw.rect_num; i++) {
		if (raw.rect[i].addr) BF_L1l2cache_Flush_Addr(raw.rect[i].addr, raw.rect[i].h_byte * raw.rect[i].v_size);
		if (raw.rect[i].addr1) BF_L1l2cache_Flush_Addr(raw.rect[i].addr1, raw.rect[i].h_byte * raw.rect[i].v_size);
	}

	for( i = 0; i < E_SHARE_H_DIV_MAX; i++ ) {
		size = (ULONG)raw.rect[i].h_byte * raw.rect[i].v_size;
		if ((raw.rect[i].addr != 0 && size != 0) || (raw.rect[i].addr1 != 0 && size != 0)) {
			sprintf(string_file_name, "I:\\RECT%d_%02lu.RAW", i, file_cnt);
			UPRINTF("--- Raw Info: %d(H) X %d(V) 16bit ---\n", raw.rect[i].h_byte / 2, raw.rect[i].v_size);

			fid = FJ_Fs_Open ( (CHAR *)string_file_name, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
			if ( sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS) {
				raw_buf = (UCHAR*) (raw.rect[i].addr + raw.rect[i].v_ofs * raw.rect[i].h_byte);
			} else if ( sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) {
				raw_buf = (UCHAR*) (raw.rect[i].addr1 + raw.rect[i].v_ofs * raw.rect[i].h_byte);
			}

			FJ_Fs_Write ( fid, (VOID*)raw_buf, size, &ret_wr_size );
			FJ_Fs_Close( fid );

			UPRINTF("--- Raw Save completed %s ---\n", string_file_name);
			file_cnt++;
		} else {
			UPRINTF("--- Raw Save Fail ---\n");
		}
	}

	Get_Share_System_Param(&param);
	param.lock_address_raw = D_ADDR_UNLOCK;
	param.lock_address_raw1 = D_ADDR_UNLOCK;
	Set_Share_System_Param(&param);
	Snd_Event_to_User( E_USER_EVENT_SAVE_RAW_END, 0 );
}

void User_Utility_Dump_RAW_One_Sensor( const ULLONG frame_no, T_DUMP_INTERFACE interface )
{
	T_SHARE_SYSTEM_PARAM param;
	T_SHARE_RAW raw;
	ULONG ret_wr_size;
	ULONG fid = 0;
	INT32 i;
	UCHAR *raw_buf = NULL;
	UCHAR *p_pix = NULL;
	UCHAR *p_rect0 = NULL;
	UCHAR *p_rect1 = NULL;
	UCHAR *p_rect2 = NULL;
	UCHAR *p_rect3 = NULL;
	CHAR filepath[256];
	FJ_ERR_CODE ret = 0;
	T_SHARE_SYSTEM_CONFIG sys_cfg;

	Get_Share_System_Config(&sys_cfg);
	Get_Frame_Share_Sensor_Raw(&raw, frame_no);
	Get_Share_System_Param(&param);

	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_35, FJ_MBALOG_FUNC_ID_LINUX_STR_RAW, FJ_MBALOG_TYPE_START, "raw"));

	if ( sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS ) {
		param.lock_address_raw = raw.rect[0].addr;
	} else if ( sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) {
		param.lock_address_raw1 = raw.rect[0].addr1;
	}
	Set_Share_System_Param(&param);

	for (i = 0; i < raw.rect_num; i++) {
		if (raw.rect[i].addr) BF_L1l2cache_Flush_Addr(raw.rect[i].addr, raw.rect[i].h_byte * raw.rect[i].v_size);
		if (raw.rect[i].addr1) BF_L1l2cache_Flush_Addr(raw.rect[i].addr1, raw.rect[i].h_byte * raw.rect[i].v_size);
	}

	if ( raw.rect_num == raw.sensor_num) {
		for (i = 0; i < raw.sensor_num; i++) {
			UPRINTF("--- Raw Info: %d(H) X %d(V) 16bit ---\n", raw.rect[i].h_byte >> 1, raw.rect[i].v_size);
			if ( sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS ) {
				raw_buf =  (UCHAR*)(raw.rect[i].addr + raw.rect[i].v_ofs * raw.rect[i].h_byte);
			} else if (sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B)  {
				raw_buf =  (UCHAR*) (raw.rect[i].addr1 + raw.rect[i].v_ofs * raw.rect[i].h_byte);
			}
			
			if ( interface == E_DUMP_STORAGE ) {
				ret = FJ_Dcf_Make_New_File_Path_By_Ext("RAW", filepath);
				UPRINTF_ERR(ret);
				fid = FJ_Fs_Open ( (CHAR *)filepath, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
				ret = FJ_Dcf_Update_DcfDb_Add_File( filepath );
				UPRINTF_ERR(ret);
				FJ_Fs_Write( fid, (VOID*)raw_buf, raw.rect[i].h_byte * raw.rect[i].v_size, &ret_wr_size );
				FJ_Fs_Close( fid );
			} else {
				OS_IF_Rtos_RAW_Stream_Send((ULONG)raw_buf, raw.rect[i].h_byte * raw.rect[i].v_size, raw.rect[i].h_byte >> 1, raw.rect[i].v_size, frame_no);
			}
		}
	} else if ( raw.sensor_num == 1) {

		if ( raw.sensor[0].raw_wo_mergin )  {
			UPRINTF("--- Raw Info: %d(H) X %d(V) 16bit ---\n", raw.rect[0].h_byte >> 1, raw.rect[0].v_size);
			if ( sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS ) {
				raw_buf =  (UCHAR*)(raw.rect[0].addr + raw.rect[0].v_ofs * raw.rect[0].h_byte);
			} else if (sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B)  {
				raw_buf =  (UCHAR*) (raw.rect[0].addr1 + raw.rect[0].v_ofs * raw.rect[0].h_byte);
			}
		} else {
			UPRINTF("--- Raw Info: %d(H) X %d(V) 16bit ---\n", raw.sensor[0].h_size, raw.rect[0].v_size);
			raw_buf = (UCHAR *)User_Utility_MM_alloc(raw.sensor[0].h_size * raw.rect[0].v_size * 2, D_AVAILABLE_SDRAM_CH);
			p_pix = raw_buf;
			if ( raw_buf == NULL) {
				UPRINTF_ERR(-1);
				return;
			}
		}

		if ( interface == E_DUMP_STORAGE ) {
			ret = FJ_Dcf_Make_New_File_Path_By_Ext( "RAW", filepath );
			UPRINTF_ERR(ret);
			fid = FJ_Fs_Open ( (CHAR *)filepath, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
			ret = FJ_Dcf_Update_DcfDb_Add_File( filepath );
			UPRINTF_ERR(ret);
		}
		if ( (sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS) &&
			 !raw.sensor[0].raw_wo_mergin ) {
			// Shift V offset
			p_rect0 = (UCHAR*) (raw.rect[0].addr + raw.rect[0].v_ofs * raw.rect[0].h_byte);
			p_rect1 = (UCHAR*) (raw.rect[1].addr + raw.rect[1].v_ofs * raw.rect[1].h_byte);
			p_rect2 = (UCHAR*) (raw.rect[2].addr + raw.rect[2].v_ofs * raw.rect[2].h_byte);
			p_rect3 = (UCHAR*) (raw.rect[3].addr + raw.rect[3].v_ofs * raw.rect[3].h_byte);
		} else if ( (sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) &&
					   !raw.sensor[0].raw_wo_mergin) {
			// Shift V offset
			p_rect0 = (UCHAR*) (raw.rect[0].addr1 + raw.rect[0].v_ofs * raw.rect[0].h_byte);
			p_rect1 = (UCHAR*) (raw.rect[1].addr1 + raw.rect[1].v_ofs * raw.rect[1].h_byte);
			p_rect2 = (UCHAR*) (raw.rect[2].addr1 + raw.rect[2].v_ofs * raw.rect[2].h_byte);
			p_rect3 = (UCHAR*) (raw.rect[3].addr1 + raw.rect[3].v_ofs * raw.rect[3].h_byte);
		}

		for (i = 0; (i < raw.rect[0].v_size) &&  !raw.sensor[0].raw_wo_mergin; i++) {
			if ( raw.rect[1].h_ofs_frame > raw.rect[0].h_ofs_frame ) {
				memcpy(p_pix, p_rect0, (raw.rect[1].h_ofs_frame - raw.rect[0].h_ofs_frame) * 2);
				p_pix += (raw.rect[1].h_ofs_frame - raw.rect[0].h_ofs_frame) * 2;
				p_rect0 += raw.rect[0].h_byte;
				// sensorx1 divx2
				if ( ((sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS) && raw.rect[2].addr == 0) ||
					 ((sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) && raw.rect[2].addr1 == 0)) {
					memcpy(p_pix, p_rect1, raw.rect[1].h_size * 2);
					p_pix += raw.rect[1].h_size * 2;
					p_rect1 += raw.rect[1].h_byte;
				}
			}
			// sensorx1 divx4
			if ( ((sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS) && raw.rect[2].addr) ||
				  ((sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) && raw.rect[2].addr1)) {
				if ( raw.rect[2].h_ofs_frame > raw.rect[1].h_ofs_frame ) {
					memcpy(p_pix, p_rect1, (raw.rect[2].h_ofs_frame - raw.rect[1].h_ofs_frame) * 2);
					p_pix += (raw.rect[2].h_ofs_frame - raw.rect[1].h_ofs_frame) * 2;
					p_rect1 += raw.rect[1].h_byte;
				}
				if ( raw.rect[3].h_ofs_frame > raw.rect[2].h_ofs_frame ) {
					memcpy(p_pix, p_rect2, (raw.rect[3].h_ofs_frame - raw.rect[2].h_ofs_frame) * 2);
					p_pix += (raw.rect[3].h_ofs_frame - raw.rect[2].h_ofs_frame) * 2;
					p_rect2 += raw.rect[2].h_byte;

					memcpy(p_pix, p_rect3, raw.rect[3].h_size * 2);
					p_pix += raw.rect[3].h_size * 2;
					p_rect3 += raw.rect[3].h_byte;
				}
			}
		}
		if ( interface == E_DUMP_STORAGE ) {
			if ( raw.sensor[0].raw_wo_mergin )  {
				FJ_Fs_Write( fid, (VOID *)raw_buf, raw.rect[0].h_byte * raw.rect[0].v_size, &ret_wr_size );
			} else {
				FJ_Fs_Write( fid, (VOID *)raw_buf, raw.sensor[0].h_size * raw.rect[0].v_size * 2, &ret_wr_size );
				User_Utility_MM_free(raw_buf, D_AVAILABLE_SDRAM_CH);
			}
			FJ_Fs_Close( fid );
		} else {
			if ( raw.sensor[0].raw_wo_mergin ) {
				OS_IF_Rtos_RAW_Stream_Send((UINT32)(raw_buf), raw.rect[0].h_byte * raw.rect[0].v_size, raw.rect[0].h_byte >> 1, raw.rect[0].v_size, frame_no);
			} else {
				OS_IF_Rtos_RAW_Stream_Send((ULONG)raw_buf, raw.sensor[0].h_size * raw.rect[0].v_size * 2, raw.sensor[0].h_size, raw.rect[0].v_size, frame_no);
				User_Utility_MM_free(raw_buf, D_AVAILABLE_SDRAM_CH);
			}
		}
	} else {
		// sensorx2 divx2
		// sensor0
		if ( raw.sensor[0].raw_wo_mergin )  {
			UPRINTF("--- Raw Info: %d(H) X %d(V) 16bit ---\n", raw.rect[0].h_byte >> 1, raw.rect[0].v_size);
			if ( sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS ) {
				raw_buf =  (UCHAR*)(raw.rect[0].addr + raw.rect[0].v_ofs * raw.rect[0].h_byte);
			} else if (sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B)  {
				raw_buf =  (UCHAR*) (raw.rect[0].addr1 + raw.rect[0].v_ofs * raw.rect[0].h_byte);
			}
		} else {
			UPRINTF("--- Raw Info: %d(H) X %d(V) 16bit ---\n", raw.sensor[0].h_size, raw.rect[0].v_size);
			raw_buf = (UCHAR *)User_Utility_MM_alloc(raw.sensor[0].h_size * raw.rect[0].v_size * 2, D_AVAILABLE_SDRAM_CH);
			p_pix = raw_buf;
			if ( raw_buf == NULL) {
				UPRINTF_ERR(-1);
				return;
			}
		}
	
   		if ( interface == E_DUMP_STORAGE ) {
   			ret = FJ_Dcf_Make_New_File_Path_By_Ext( "RAW", filepath );
   			UPRINTF_ERR(ret);
   			fid = FJ_Fs_Open ( (CHAR *)filepath, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
   			ret = FJ_Dcf_Update_DcfDb_Add_File( filepath );
   			UPRINTF_ERR(ret);
   		}

   		if ( (sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS) &&
			 !raw.sensor[0].raw_wo_mergin ) {
   			p_rect0 = (UCHAR*) (raw.rect[0].addr + raw.rect[0].v_ofs * raw.rect[0].h_byte);
   			p_rect1 = (UCHAR*) (raw.rect[1].addr + raw.rect[1].v_ofs * raw.rect[1].h_byte);
   		} else if ( (sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) &&
			 !raw.sensor[0].raw_wo_mergin) {
   			p_rect0 = (UCHAR*) (raw.rect[0].addr1 + raw.rect[0].v_ofs * raw.rect[0].h_byte);
   			p_rect1 = (UCHAR*) (raw.rect[1].addr1 + raw.rect[1].v_ofs * raw.rect[1].h_byte);
   		}

   		for (i = 0; (i < raw.rect[0].v_size) && !raw.sensor[0].raw_wo_mergin; i++) {
   			if ( raw.rect[1].h_ofs_frame > raw.rect[0].h_ofs_frame ) {
   				memcpy(p_pix, p_rect0, (raw.rect[1].h_ofs_frame - raw.rect[0].h_ofs_frame) * 2);
   				p_pix += (raw.rect[1].h_ofs_frame - raw.rect[0].h_ofs_frame) * 2;
   				p_rect0 += raw.rect[0].h_byte;

   				memcpy(p_pix, p_rect1, raw.rect[1].h_size * 2);
   				p_pix += raw.rect[1].h_size * 2;
   				p_rect1 += raw.rect[1].h_byte;
   			}
   		}
   		if ( interface == E_DUMP_STORAGE ) {
			if ( raw.sensor[0].raw_wo_mergin )  {
				FJ_Fs_Write( fid, (VOID *)raw_buf, raw.rect[0].h_byte * raw.rect[0].v_size, &ret_wr_size );
			} else {
				FJ_Fs_Write( fid, (VOID *)raw_buf, raw.sensor[0].h_size * raw.rect[0].v_size * 2, &ret_wr_size );
				User_Utility_MM_free(raw_buf, D_AVAILABLE_SDRAM_CH);
			}
   			FJ_Fs_Close( fid );
   		} else {
			if ( raw.sensor[0].raw_wo_mergin ) {
				OS_IF_Rtos_RAW_Stream_Send((ULONG)raw_buf, raw.rect[0].h_byte * raw.rect[0].v_size, raw.rect[0].h_byte >> 1, raw.rect[0].v_size, frame_no);
			} else {
				OS_IF_Rtos_RAW_Stream_Send((ULONG)raw_buf, raw.sensor[0].h_size * raw.rect[0].v_size * 2, raw.sensor[0].h_size, raw.rect[0].v_size, frame_no);
				User_Utility_MM_free(raw_buf, D_AVAILABLE_SDRAM_CH);
			}
   		}

		// sensorx2 divx2
		// sensor1
		if ( raw.sensor[0].raw_wo_mergin )  {
			UPRINTF("--- Raw Info: %d(H) X %d(V) 16bit ---\n", raw.rect[2].h_byte >> 1, raw.rect[2].v_size);
			if ( sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS ) {
				raw_buf =  (UCHAR*)(raw.rect[2].addr + raw.rect[2].v_ofs * raw.rect[2].h_byte);
			} else if (sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B)  {
				raw_buf =  (UCHAR*) (raw.rect[2].addr1 + raw.rect[2].v_ofs * raw.rect[2].h_byte);
			}
		} else {
			UPRINTF("--- Raw Info: %d(H) X %d(V) 16bit ---\n", raw.sensor[1].h_size, raw.rect[2].v_size);
			raw_buf = (UCHAR *)User_Utility_MM_alloc(raw.sensor[1].h_size * raw.rect[0].v_size * 2, D_AVAILABLE_SDRAM_CH);
			p_pix = raw_buf;
			if ( raw_buf == NULL) {
				UPRINTF_ERR(-1);
				return;
			}
		}

   		if ( interface == E_DUMP_STORAGE ) {
   			ret = FJ_Dcf_Make_New_File_Path_By_Ext( "RAW", filepath );
   			UPRINTF_ERR(ret);
   			fid = FJ_Fs_Open ( (CHAR *)filepath, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
   			ret = FJ_Dcf_Update_DcfDb_Add_File( filepath );
   			UPRINTF_ERR(ret);
   		}

   		if ( (sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS) &&
			 !raw.sensor[1].raw_wo_mergin ) {
   			p_rect2 = (UCHAR*) (raw.rect[2].addr + raw.rect[2].v_ofs * raw.rect[2].h_byte);
   			p_rect3 = (UCHAR*) (raw.rect[3].addr + raw.rect[3].v_ofs * raw.rect[3].h_byte);
   		} else if ( (sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) &&
			 !raw.sensor[1].raw_wo_mergin) {
   			p_rect2 = (UCHAR*) (raw.rect[2].addr1 + raw.rect[2].v_ofs * raw.rect[2].h_byte);
   			p_rect3 = (UCHAR*) (raw.rect[3].addr1 + raw.rect[3].v_ofs * raw.rect[3].h_byte);
   		}

   		for (i = 0; ( i < raw.rect[2].v_size) &&  !raw.sensor[1].raw_wo_mergin; i++) {
   			if ( raw.rect[3].h_ofs_frame > raw.rect[2].h_ofs_frame ) {
   				memcpy(p_pix, p_rect2, (raw.rect[3].h_ofs_frame - raw.rect[2].h_ofs_frame) * 2);
   				p_pix += (raw.rect[3].h_ofs_frame - raw.rect[2].h_ofs_frame) * 2;
   				p_rect2 += raw.rect[2].h_byte;

   				memcpy(p_pix, p_rect3, raw.rect[3].h_size * 2);
   				p_pix += raw.rect[3].h_size * 2;
   				p_rect3 += raw.rect[3].h_byte;
   			}
   		}
   		if ( interface == E_DUMP_STORAGE ) {
			if ( raw.sensor[1].raw_wo_mergin )  {
				FJ_Fs_Write( fid, (VOID *)raw_buf, raw.rect[2].h_byte * raw.rect[2].v_size, &ret_wr_size );
			} else {
				FJ_Fs_Write( fid, (VOID *)raw_buf, raw.sensor[1].h_size * raw.rect[2].v_size * 2, &ret_wr_size );
				User_Utility_MM_free(raw_buf, D_AVAILABLE_SDRAM_CH);
			}
   			FJ_Fs_Close( fid );
   		} else {
			if ( raw.sensor[1].raw_wo_mergin )  {
				OS_IF_Rtos_RAW_Stream_Send((ULONG)raw_buf, raw.rect[2].h_byte * raw.rect[2].v_size, raw.rect[2].h_byte >> 1, raw.rect[2].v_size, frame_no);
			} else {
				OS_IF_Rtos_RAW_Stream_Send((ULONG)raw_buf, raw.sensor[1].h_size * raw.rect[2].v_size * 2, raw.sensor[1].h_size, raw.rect[2].v_size, frame_no);
				User_Utility_MM_free(raw_buf, D_AVAILABLE_SDRAM_CH);
			}
   		}
	}

	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_35, FJ_MBALOG_FUNC_ID_LINUX_STR_RAW, FJ_MBALOG_TYPE_END, "raw"));
	Get_Share_System_Param(&param);
	param.lock_address_raw = D_ADDR_UNLOCK;
	param.lock_address_raw1 = D_ADDR_UNLOCK;
	Set_Share_System_Param(&param);
	Snd_Event_to_User( E_USER_EVENT_SAVE_RAW_END, 0 );
}

void User_Utility_SaveFile_DOL_YUV( void )
{
	static ULONG file_cnt = 0;
	CHAR string_file_name[30];
	ULONG ret_wr_size;
	ULONG fid;
	ULONG size;
	T_SHARE_PIPE_DOL_YUV share_yuv;

	Get_Frame_Share_PIPE_DOL_YUV( &share_yuv, 0 );

	size = (ULONG)share_yuv.long_yuv.h_byte * share_yuv.long_yuv.v_size;
	BF_L1l2cache_Flush_Addr(share_yuv.long_yuv.addr_y, size);
	BF_L1l2cache_Flush_Addr(share_yuv.long_yuv.addr_c, share_yuv.long_yuv.format == E_SHARE_YUV_FORMAT_420_SEMI ? size >> 1 : size);

	// long
	sprintf(string_file_name, "I:\\LONG%04lu.YUV", file_cnt++);
	UPRINTF("--- YUV Info: %d(H) X %d(V) ---\n", share_yuv.long_yuv.h_size, share_yuv.long_yuv.v_size);

	fid = FJ_Fs_Open ( (CHAR *)string_file_name, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	FJ_Fs_Write ( fid, (VOID*)share_yuv.long_yuv.addr_y, size, &ret_wr_size );
	if (share_yuv.long_yuv.format == E_SHARE_YUV_FORMAT_420_SEMI) {
		size >>= 1;
		UPRINTF("YUV420 SEMI\n");
	} else {
		UPRINTF("YUV422 SEMI\n");
	}
	FJ_Fs_Write( fid, (VOID *)share_yuv.long_yuv.addr_c, size, &ret_wr_size );
	FJ_Fs_Close( fid );

	UPRINTF("--- Long YUV Save completed %s ---\n", string_file_name);

	// short
	size = (ULONG)share_yuv.short_yuv.h_byte * share_yuv.short_yuv.v_size;
	BF_L1l2cache_Flush_Addr(share_yuv.short_yuv.addr_y, size);
	BF_L1l2cache_Flush_Addr(share_yuv.short_yuv.addr_c, share_yuv.short_yuv.format == E_SHARE_YUV_FORMAT_420_SEMI ? size >> 1 : size);
	
	sprintf(string_file_name, "I:\\SHORT%03lu.YUV", file_cnt++);
	UPRINTF("--- YUV Info: %d(H) X %d(V) ---\n", share_yuv.short_yuv.h_size, share_yuv.short_yuv.v_size);

	fid = FJ_Fs_Open ( (CHAR *)string_file_name, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	FJ_Fs_Write ( fid, (VOID*)share_yuv.short_yuv.addr_y, size, &ret_wr_size );
	if (share_yuv.short_yuv.format == E_SHARE_YUV_FORMAT_420_SEMI) {
		size >>= 1;
		UPRINTF("YUV420 SEMI\n");
	} else {
		UPRINTF("YUV422 SEMI\n");
	}
	FJ_Fs_Write( fid, (VOID *)share_yuv.short_yuv.addr_c, size, &ret_wr_size );
	FJ_Fs_Close( fid );

	UPRINTF("--- Short YUV Save completed %s ---\n", string_file_name);
}

void User_Utility_Dump_YUV( const E_SHARE_YUV_OUT yuv_out, const ULLONG frame_no, T_DUMP_INTERFACE interface )
{
	T_SHARE_SYSTEM_PARAM param;
	ULONG ret_wr_size;
	ULONG fid;
	T_SHARE_YUV share_yuv;
	CHAR filepath[256];
	FJ_ERR_CODE ret = 0;
	ULONG y_size = 0;
	ULONG cbcr_size = 0;
	void *ipcu_buf = NULL;

	Get_Frame_Share_YUV( &share_yuv, frame_no );
	Get_Share_System_Param(&param);

	// Check image exist or not
	if(share_yuv.layout[yuv_out].addr_y == 0 || share_yuv.layout[yuv_out].addr_c == 0) {
		UPRINTF_ERR(1);
		return;
	}
	param.lock_ipu_address_y = share_yuv.layout[yuv_out].addr_y;
	param.lock_ipu_address_c = share_yuv.layout[yuv_out].addr_c;
	Set_Share_System_Param(&param);

	if ( interface == E_DUMP_STORAGE ) {
		UPRINTF("--- YUV Info: %d(H) X %d(V) ---\n", share_yuv.layout[yuv_out].h_size, share_yuv.layout[yuv_out].v_size);
		y_size = (ULONG)share_yuv.layout[yuv_out].h_byte * share_yuv.layout[yuv_out].v_size;
		cbcr_size = share_yuv.layout[yuv_out].format == E_SHARE_YUV_FORMAT_420_SEMI ? y_size >> 1 : y_size;
		ret = FJ_Dcf_Make_New_File_Path_By_Ext( "YUV", filepath );
		UPRINTF_ERR(ret);
		fid = FJ_Fs_Open((CHAR *)filepath, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC));
		ret = FJ_Dcf_Update_DcfDb_Add_File( filepath );
		UPRINTF_ERR(ret);
#if 0
		// To verify YUYV on file
		cbcr_size = y_size; // output is fixed at 422
		ipcu_buf = User_Utility_MM_alloc(y_size + cbcr_size, D_AVAILABLE_SDRAM_CH);
		if (ipcu_buf == NULL) {
			UPRINTF_ERR(1);
			return;
		}
		conv_yuv_semi_to_yuv_chunky(share_yuv.layout[yuv_out], (UINT32)ipcu_buf);
		FJ_Fs_Write ( fid, (VOID*)ipcu_buf, y_size + cbcr_size, &ret_wr_size );
		User_Utility_MM_free(ipcu_buf, D_AVAILABLE_SDRAM_CH);
#else
		FJ_Fs_Write ( fid, (VOID*)share_yuv.layout[yuv_out].addr_y, y_size, &ret_wr_size );
		FJ_Fs_Write( fid, (VOID *)share_yuv.layout[yuv_out].addr_c, cbcr_size, &ret_wr_size );
#endif
		FJ_Fs_Close( fid );
	} else {
		y_size = (ULONG)share_yuv.layout[yuv_out].h_size * share_yuv.layout[yuv_out].v_size;
		// conv_yuv_semi_to_yuv_chunky to YUV422 so CbCr size = Y size
		cbcr_size = y_size;
		
		if( OS_IF_Rtos_UVC_Is_Header() ){
			ipcu_buf = (void*)OS_IF_Rtos_UVC_Get_Image_Address();
			conv_yuv_semi_to_yuv_chunky(share_yuv.layout[yuv_out], (UINT32)ipcu_buf);
			OS_IF_Rtos_UVC_YUV_Stream_Send((ULONG)ipcu_buf, y_size + cbcr_size, share_yuv.layout[yuv_out].h_size, share_yuv.layout[yuv_out].v_size, frame_no);
		}else{
			ipcu_buf = User_Utility_MM_alloc(y_size + cbcr_size, D_AVAILABLE_SDRAM_CH);
			
			if (ipcu_buf == NULL) {
				UPRINTF_ERR(1);
				Get_Share_System_Param(&param);
				param.lock_ipu_address_y = D_ADDR_UNLOCK;
				param.lock_ipu_address_c = D_ADDR_UNLOCK;
				Set_Share_System_Param(&param);
				return;
			}
			conv_yuv_semi_to_yuv_chunky(share_yuv.layout[yuv_out], (UINT32)ipcu_buf);
			OS_IF_Rtos_YUV_Stream_Send((ULONG)ipcu_buf, y_size + cbcr_size, share_yuv.layout[yuv_out].h_size, share_yuv.layout[yuv_out].v_size, frame_no);
			User_Utility_MM_free(ipcu_buf, D_AVAILABLE_SDRAM_CH);
		}
	}

	Get_Share_System_Param(&param);
	param.lock_ipu_address_y = D_ADDR_UNLOCK;
	param.lock_ipu_address_c = D_ADDR_UNLOCK;
	Set_Share_System_Param(&param);
	Snd_Event_to_User( E_USER_EVENT_SAVE_YUV_END, 0 );
}

void User_Utility_Dump_RAW_YUV( const ULONG aval_mem, const ULLONG frame_no, T_DUMP_INTERFACE interface_yuv, T_DUMP_INTERFACE interface_raw)
{
	CHAR filepath[256];
	T_SHARE_SYSTEM_PARAM param;
	T_SHARE_RAW raw;
	T_SHARE_YUV share_yuv;
	ULONG ret_wr_size;
	ULONG fid = 0;
	UCHAR *raw_buf = NULL;
	UCHAR *p_pix = NULL;
	UCHAR *p_rect0 = NULL;
	UCHAR *p_rect1 = NULL;
	UCHAR *p_rect2 = NULL;
	UCHAR *p_rect3 = NULL;
	FJ_ERR_CODE ret = 0;
	ULONG y_size = 0;
	ULONG cbcr_size = 0;
	INT32 i;
	T_SHARE_SYSTEM_CONFIG sys_cfg;
	void *ipcu_buf = NULL;

	// -------------- YUV -----------------
	Get_Frame_Share_YUV( &share_yuv, 0 );
	Get_Share_System_Param(&param);
	param.lock_ipu_address_y = share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y;
	param.lock_ipu_address_c = share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c;
	Set_Share_System_Param(&param);

	// Check image exist or not
	if(share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y == 0 || share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c == 0) {
		UPRINTF_ERR(1);
		return;
	}

	UPRINTF("--- YUV Info: %d(H) X %d(V) ---\n", share_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_size, share_yuv.layout[E_SHARE_YUV_OUT_USER_0].v_size);


	if ( interface_yuv == E_DUMP_STORAGE ) {
		y_size = (ULONG)share_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_byte * share_yuv.layout[E_SHARE_YUV_OUT_USER_0].v_size;
		cbcr_size = share_yuv.layout[E_SHARE_YUV_OUT_USER_0].format == E_SHARE_YUV_FORMAT_420_SEMI ? y_size >> 1 : y_size;
		ret = FJ_Dcf_Make_New_File_Path_By_Ext( "YUV", filepath );
		UPRINTF_ERR(ret);
		fid = FJ_Fs_Open ( (CHAR *)filepath, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
		ret = FJ_Dcf_Update_DcfDb_Add_File( filepath );
		UPRINTF_ERR(ret);

		FJ_Fs_Write ( fid, (VOID*)share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y, y_size, &ret_wr_size );
		FJ_Fs_Write( fid, (VOID *)share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c, cbcr_size, &ret_wr_size );
		FJ_Fs_Close( fid );
	} else {
		y_size = (ULONG)share_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_size * share_yuv.layout[E_SHARE_YUV_OUT_USER_0].v_size;
		// conv_yuv_semi_to_yuv_chunky to YUV422 so cbcr_size = y_size
		cbcr_size = y_size;
		ipcu_buf = User_Utility_MM_alloc(y_size + cbcr_size, D_AVAILABLE_SDRAM_CH);
		if (ipcu_buf == NULL) {
			Get_Share_System_Param(&param);
			param.lock_ipu_address_y = D_ADDR_UNLOCK;
			param.lock_ipu_address_c = D_ADDR_UNLOCK;
			Set_Share_System_Param(&param);
			UPRINTF_ERR(1);
			return;
		}
		conv_yuv_semi_to_yuv_chunky(share_yuv.layout[E_SHARE_YUV_OUT_USER_0], (UINT32)ipcu_buf);
		OS_IF_Rtos_YUV_Stream_Send((ULONG)ipcu_buf, y_size + cbcr_size, share_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_size, share_yuv.layout[E_SHARE_YUV_OUT_USER_0].v_size, frame_no);
		User_Utility_MM_free(ipcu_buf, D_AVAILABLE_SDRAM_CH);
	}

	Get_Share_System_Param(&param);
	param.lock_ipu_address_y = D_ADDR_UNLOCK;
	param.lock_ipu_address_c = D_ADDR_UNLOCK;
	Set_Share_System_Param(&param);

	// -------------- RAW ------------------
	Get_Share_System_Config(&sys_cfg);
	Get_Frame_Share_Sensor_Raw(&raw, frame_no);
	Get_Share_System_Param(&param);
	if ( sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS ) {
		param.lock_address_raw = raw.rect[0].addr;
	} else if ( sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) {
		param.lock_address_raw1 = raw.rect[0].addr1;
	}
	Set_Share_System_Param(&param);

	for (i = 0; i < raw.rect_num; i++) {
		if (raw.rect[i].addr) BF_L1l2cache_Flush_Addr(raw.rect[i].addr, raw.rect[i].h_byte * raw.rect[i].v_size);
		if (raw.rect[i].addr1) BF_L1l2cache_Flush_Addr(raw.rect[i].addr1, raw.rect[i].h_byte * raw.rect[i].v_size);
	}

	if ( raw.sensor[0].raw_wo_mergin )  {
   		UPRINTF("--- Raw Info: %d(H) X %d(V) 16bit ---\n", raw.rect[0].h_byte >> 1, raw.rect[0].v_size);
   		if ( sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS ) {
   			raw_buf =  (UCHAR*)(raw.rect[0].addr + raw.rect[0].v_ofs * raw.rect[0].h_byte);
   		} else if (sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B)  {
   			raw_buf =  (UCHAR*) (raw.rect[0].addr1 + raw.rect[0].v_ofs * raw.rect[0].h_byte);
   		}
   	} else {
   		UPRINTF("--- Raw Info: %d(H) X %d(V) 16bit ---\n", raw.sensor[0].h_size, raw.rect[0].v_size);
   		raw_buf = (UCHAR *)User_Utility_MM_alloc(raw.sensor[0].h_size * raw.rect[i].v_size * 2, D_AVAILABLE_SDRAM_CH);
   		p_pix = raw_buf;
   		if ( raw_buf == NULL) {
   			UPRINTF_ERR(-1);
   			return;
   		}
   	}

	if ( interface_raw == E_DUMP_STORAGE ) {
		ret = FJ_Dcf_Make_New_File_Path_By_Ext( "RAW", filepath );
		UPRINTF_ERR(ret);
		fid = FJ_Fs_Open ( (CHAR *)filepath, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
		ret = FJ_Dcf_Update_DcfDb_Add_File( filepath );
		UPRINTF_ERR(ret);
	}

	if ( (sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS) &&
		 !raw.sensor[0].raw_wo_mergin ) {
		p_rect0 = (UCHAR*) (raw.rect[0].addr + raw.rect[0].v_ofs * raw.rect[0].h_byte);
		p_rect1 = (UCHAR*) (raw.rect[1].addr + raw.rect[1].v_ofs * raw.rect[1].h_byte);
		p_rect2 = (UCHAR*) (raw.rect[2].addr + raw.rect[2].v_ofs * raw.rect[2].h_byte);
		p_rect3 = (UCHAR*) (raw.rect[3].addr + raw.rect[3].v_ofs * raw.rect[3].h_byte);
	} else if ( (sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) &&
				   !raw.sensor[0].raw_wo_mergin) {
		p_rect0 = (UCHAR*) (raw.rect[0].addr1 + raw.rect[0].v_ofs * raw.rect[0].h_byte);
		p_rect1 = (UCHAR*) (raw.rect[1].addr1 + raw.rect[1].v_ofs * raw.rect[1].h_byte);
		p_rect2 = (UCHAR*) (raw.rect[2].addr1 + raw.rect[2].v_ofs * raw.rect[2].h_byte);
		p_rect3 = (UCHAR*) (raw.rect[3].addr1 + raw.rect[3].v_ofs * raw.rect[3].h_byte);;
	}

	for (i = 0; i < (raw.rect[0].v_size) && !raw.sensor[0].raw_wo_mergin; i++) {
		if ( raw.rect[1].h_ofs_frame > raw.rect[0].h_ofs_frame ) {
			memcpy(p_pix, p_rect0, (raw.rect[1].h_ofs_frame - raw.rect[0].h_ofs_frame) * 2);
			p_pix += (raw.rect[1].h_ofs_frame - raw.rect[0].h_ofs_frame) * 2;
			p_rect0 += raw.rect[0].h_byte;
			// sensorx1 divx2
			if ( ((sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS) && raw.rect[2].addr == 0) ||
				 ((sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) && raw.rect[2].addr1 == 0)) {
				memcpy(p_pix, p_rect1, raw.rect[1].h_size * 2);
				p_pix += raw.rect[1].h_size * 2;
				p_rect1 += raw.rect[1].h_byte;
			}
		}
		// sensorx1 divx4
		if ( ((sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SEN || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_DIS) && raw.rect[2].addr) ||
			  ((sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || sys_cfg.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) && raw.rect[2].addr1)) {
			if ( raw.rect[2].h_ofs_frame > raw.rect[1].h_ofs_frame ) {
				memcpy(p_pix, p_rect1, (raw.rect[2].h_ofs_frame - raw.rect[1].h_ofs_frame) * 2);
				p_pix += (raw.rect[2].h_ofs_frame - raw.rect[1].h_ofs_frame) * 2;
				p_rect1 += raw.rect[1].h_byte;
			}
			if ( raw.rect[3].h_ofs_frame > raw.rect[2].h_ofs_frame ) {
				memcpy(p_pix, p_rect2, (raw.rect[3].h_ofs_frame - raw.rect[2].h_ofs_frame) * 2);
				p_pix += (raw.rect[3].h_ofs_frame - raw.rect[2].h_ofs_frame) * 2;
				p_rect2 += raw.rect[2].h_byte;

				memcpy(p_pix, p_rect3, raw.rect[3].h_size * 2);
				p_pix += raw.rect[3].h_size * 2;
				p_rect3 += raw.rect[3].h_byte;
			}
		}
	}
	if ( interface_raw == E_DUMP_STORAGE ) {
		if ( raw.sensor[0].raw_wo_mergin )  {
			FJ_Fs_Write( fid, (VOID *)raw_buf, raw.rect[0].h_byte * raw.rect[0].v_size, &ret_wr_size );
			if( aval_mem) iip_bayer_memcpy((UINT32)raw_buf, (UINT32)aval_mem, raw.rect[0].h_byte, raw.rect[0].v_size, 16);
   		} else {
   			FJ_Fs_Write( fid, (VOID *)raw_buf, raw.sensor[0].h_size * raw.rect[0].v_size * 2, &ret_wr_size );
   			User_Utility_MM_free(raw_buf, D_AVAILABLE_SDRAM_CH);
   		}
		FJ_Fs_Close( fid );
	} else {
		if ( raw.sensor[0].raw_wo_mergin ) {
			OS_IF_Rtos_RAW_Stream_Send((ULONG)raw_buf, raw.rect[0].h_byte * raw.rect[0].v_size, raw.rect[0].h_byte >> 1, raw.rect[0].v_size, frame_no);
		} else {
			OS_IF_Rtos_RAW_Stream_Send((ULONG)raw_buf, raw.sensor[0].h_size * raw.rect[0].v_size * 2, raw.sensor[0].h_size, raw.rect[0].v_size, frame_no);
			User_Utility_MM_free(raw_buf, D_AVAILABLE_SDRAM_CH);
		}
	}

	Get_Share_System_Param(&param);
	param.lock_address_raw = D_ADDR_UNLOCK;
	param.lock_address_raw1= D_ADDR_UNLOCK;
	Set_Share_System_Param(&param);

	Snd_Event_to_User( E_USER_EVENT_RAWYUV_SAVE_END, frame_no );
}

void User_Utility_IPCU_RAW_Enable(BOOL en)
{
	g_ipcu_raw_en = en;
}

BOOL User_Utility_is_IPCU_RAW_Enable(void)
{
	return g_ipcu_raw_en;
}

void User_Utility_IPCU_YUV_Enable(BOOL en)
{
	g_ipcu_yuv_en = en;
}

BOOL User_Utility_is_IPCU_YUV_Enable(void)
{
	return g_ipcu_yuv_en;
}

VOID User_Utility_Dump_Raw_Printf(UINT32 image_addr, USHORT image_width, USHORT pos_x, USHORT pos_y, ULONG img_size_byte, char *fmt, ... )
{
	char string_work[128];		// �������

	if( image_addr == 0 ){
		return;
	}

	va_list ap;

	va_start( ap, fmt );	/* ap �����������閮剖���� */ /*pgr0834*/
	vsprintf( string_work, fmt, ap );
	// ��
	user_utility_dump_font_write_bayer( image_addr, image_width, pos_x, pos_y, (UCHAR*)string_work );
	va_end( ap );
	BF_L1l2cache_Flush_Addr(image_addr, img_size_byte);
}

VOID User_Utility_Dump_YUV_Printf( const T_SHARE_YUV* yuv, E_SHARE_YUV_OUT index, USHORT pos_x, USHORT pos_y, char *fmt, ... )
{
	FJ_T_ADDR_YCC yuv_addr;
	char string_work[128];		// �������
	ULONG y_size, cbcr_size;

	if( yuv == NULL ){
		return;
	}

	va_list ap;

	yuv_addr.AddressY  = yuv->layout[index].addr_y;
	yuv_addr.AddressCb = yuv->layout[index].addr_c;
	yuv_addr.AddressCr = yuv->layout[index].addr_c;

	va_start( ap, fmt );	/* ap �����������閮剖���� */ /*pgr0834*/
	vsprintf( string_work, fmt, ap );
	// ��
	user_utility_dump_font_write_yuv( &yuv_addr, yuv->layout[index].h_byte, yuv->layout[index].format, pos_x, pos_y, (UCHAR*)string_work );
	va_end( ap );
	y_size = yuv->layout[index].h_byte * yuv->layout[index].v_size;
	BF_L1l2cache_Flush_Addr(yuv->layout[index].addr_y, y_size);
	cbcr_size = yuv->layout[index].format == E_SHARE_YUV_FORMAT_420_SEMI ? y_size >> 1 : y_size;
	BF_L1l2cache_Flush_Addr(yuv->layout[index].addr_c, cbcr_size);
}

VOID User_Utility_Dump_YUV_SetFontSize( UCHAR font_size )
{
	if((font_size <= 0) || (font_size > 16)) font_size = OSD_FONT_SIZE_DEF;

	g_font_yuv_size = font_size;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static int conv_yuv_semi_to_yuv_chunky(const T_SHARE_LAYOUT yuv_src, const ULONG out_addr)
{
	int ercd = 0;
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_RR_BUF_BYTES];
	ULONG param_buffer_addr = ROUNDUP_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 1000;
	T_IM_IIP_UTIL_RR iip_util_rr0;

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_07, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "CONV YUV"));

	memset(&iip_util_rr0, 0, sizeof(T_IM_IIP_UTIL_RR));

	iip_util_rr0.afn_unitid = E_IM_IIP_UNIT_ID_AFN0;
	iip_util_rr0.sl_unitid = E_IM_IIP_UNIT_ID_SL2;
	iip_util_rr0.src_pixid = E_IM_IIP_PIXID_4;
	iip_util_rr0.dst_pixid = E_IM_IIP_PIXID_5;

	iip_util_rr0.src.gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_rr0.src.gbl.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	iip_util_rr0.src.gbl.pix_format = yuv_src.format == E_SHARE_YUV_FORMAT_422_SEMI ? E_IM_IIP_PFMT_YCC422 : E_IM_IIP_PFMT_YCC420;
	iip_util_rr0.src.gbl.chunky_color.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G;
	iip_util_rr0.src.gbl.chunky_color.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B;
	iip_util_rr0.src.gbl.chunky_color.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R;
	iip_util_rr0.src.gbl.chunky_color.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A;
	iip_util_rr0.src.gbl.sign_Y_G = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.src.gbl.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.src.gbl.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.src.gbl.sign_D3 = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.src.gbl.masterIF_Y_G = D_IM_IIP_MASTER_IF0;
	iip_util_rr0.src.gbl.masterIF_Cb_B = D_IM_IIP_MASTER_IF0;
	iip_util_rr0.src.gbl.masterIF_Cr_R = D_IM_IIP_MASTER_IF0;
	iip_util_rr0.src.gbl.masterIF_Alpha = D_IM_IIP_MASTER_IF0;

	iip_util_rr0.dst.gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_rr0.dst.gbl.frame_type = E_IM_IIP_FTYPE_CHUNKY;
	iip_util_rr0.dst.gbl.pix_format = E_IM_IIP_PFMT_YCC422;
	iip_util_rr0.dst.gbl.chunky_color.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G; // YUYV
	iip_util_rr0.dst.gbl.chunky_color.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B; // YUYV
	iip_util_rr0.dst.gbl.chunky_color.component2 = D_IM_IIP_CHUNKY_COLOR_Y1_A; // YUYV
	iip_util_rr0.dst.gbl.chunky_color.component3 = D_IM_IIP_CHUNKY_COLOR_CR_R; // YUYV
	iip_util_rr0.dst.gbl.sign_Y_G = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.dst.gbl.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.dst.gbl.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.dst.gbl.sign_D3 = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.dst.gbl.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.dst.gbl.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.dst.gbl.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.dst.gbl.masterIF_Alpha = D_IM_IIP_MASTER_IF1;

	iip_util_rr0.afn_cache_select = E_IM_IIP_PARAM_CSEL_0;
	iip_util_rr0.pix_depth = D_IM_IIP_PDEPTH_8BITS;
	iip_util_rr0.rotation = E_IM_IIP_UTIL_ROTATE_000;
	iip_util_rr0.flip = E_IM_IIP_UTIL_FLIP_OFF;
	iip_util_rr0.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;

	iip_util_rr0.src.rect.width				= yuv_src.h_size;
	iip_util_rr0.src.rect.lines				= yuv_src.v_size;
	iip_util_rr0.src.gbl.pix_format			= yuv_src.format == E_SHARE_YUV_FORMAT_422_SEMI ? E_IM_IIP_PFMT_YCC422 : E_IM_IIP_PFMT_YCC420;
	iip_util_rr0.src.gbl.width				= yuv_src.h_size;
	iip_util_rr0.src.gbl.lines				= yuv_src.v_size;
	iip_util_rr0.src.gbl.line_bytes.Y_G		= yuv_src.h_byte;
	iip_util_rr0.src.gbl.line_bytes.Cb_B	= yuv_src.h_byte;
	iip_util_rr0.src.gbl.line_bytes.Cr_R	= 0;
	iip_util_rr0.src.gbl.line_bytes.Alpha	= 0;
	iip_util_rr0.src.gbl.addr.Y_G			= yuv_src.addr_y;
	iip_util_rr0.src.gbl.addr.Cb_B			= yuv_src.addr_c;
	iip_util_rr0.src.gbl.addr.Cr_R			= 0;
	iip_util_rr0.src.gbl.addr.Alpha			= 0;

	iip_util_rr0.dst.rect.width				= yuv_src.h_size;
	iip_util_rr0.dst.rect.lines				= yuv_src.v_size;
	iip_util_rr0.dst.gbl.pix_format			= E_IM_IIP_PFMT_YCC422;
	iip_util_rr0.dst.gbl.width				= yuv_src.h_size;
	iip_util_rr0.dst.gbl.lines				= yuv_src.v_size;
	iip_util_rr0.dst.gbl.line_bytes.Y_G		= yuv_src.h_byte * 2;
	iip_util_rr0.dst.gbl.line_bytes.Cb_B	= 0;
	iip_util_rr0.dst.gbl.line_bytes.Cr_R	= 0;
	iip_util_rr0.dst.gbl.line_bytes.Alpha	= 0;
	iip_util_rr0.dst.gbl.addr.Y_G			= out_addr;
	iip_util_rr0.dst.gbl.addr.Cb_B			= 0;
	iip_util_rr0.dst.gbl.addr.Cr_R			= 0;
	iip_util_rr0.dst.gbl.addr.Alpha			= 0;

	ercd = IM_IIP_Utility_ResizeRotate( &iip_util_rr0,
										NULL,
										param_master_IF,
										param_buffer_addr,
										wait_time );
	UPRINTF_ERR(ercd);

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_07, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "CONV YUV"));
	return ercd;
}

static E_FONT_TYPE user_utility_dump_get_font( const UCHAR** font, UCHAR ch )
{
	E_FONT_TYPE ret;
	// ��������捱摰�
//	if ( ch >= 0x20 && ch <= 0x7E ) {
	if ( ch >= 0x20 && ch <= 0x88 ) {
		*font = g_font[(ch-0x20)];
		ret = E_FONT_PRINT;
	}
	// �銵�
	else if( ch == '\n' ){
		ret = E_FONT_LF;
	}
	// 銵函內�������蝛箇
	else {
		*font = g_font[0];
		ret = E_FONT_SPACE;
	}
	return ret;
}

static VOID user_utility_dump_font_write_bayer( UINT32 bayer_addr, USHORT pshdma, USHORT pos_x, USHORT pos_y, const UCHAR* str )
{
	INT32 i, j;				// �����������敺∠
	INT32 cnt = 0;			// �����������
	UCHAR ch;				// �����
//	INT32 zoom_x;			// �����������敺∠
	INT32 zoom_y;			// �����������敺∠
	UCHAR *draw_pt;
	INT32 ofs_x = pos_x;
	INT32 ofs_y = pos_y;
	const UCHAR *p_char;
	UCHAR write_byte;
#ifdef __BAYER_16BIT_WRITE
	UCHAR bit_multiple = 2;	// 8 * 2 = 16 bit bayer
#else
	UCHAR bit_multiple = 1;	// 8 * 1 = 8 bit bayer
#endif
	UCHAR *start_addr = (UCHAR *)(bayer_addr) + ( ofs_x + ofs_y * pshdma ) * bit_multiple;			// ������蔭閮剖��
	INT32 write_x;
	UCHAR font_size = g_font_raw_size;
	
	for( cnt = 0, write_x = 0; (ch = str[cnt]) != '\0'; cnt++, write_x++ ){
		// ��������捱摰�
		switch( user_utility_dump_get_font( &p_char, ch ) ){
			// 銵函內
			case E_FONT_PRINT:
				break;
			// �銵�
			case E_FONT_LF:
				ofs_y += OSD_CHAR_HEIGHT * g_font_raw_size + 4;		// +4�銵��
				// ������蔭��身摰�����
				start_addr = (UCHAR *)(bayer_addr) + ( ofs_x + ofs_y * pshdma ) * bit_multiple;
				write_x = -1;
				continue;
			// 蝛箇
			case E_FONT_SPACE:
			default:
				// ��銵函內������征��SKIP
				if( !g_visible_bgcolor ){
					continue;
				}
				break;
		}
		// ���������噯�
		for( i = 0; i < OSD_CHAR_WIDTH; i++ ) {
			for( j = 0; j < OSD_CHAR_HEIGHT; j++ ){
				write_byte = p_char[i + j * OSD_CHAR_WIDTH];
				// ������憌���
				if( write_byte == E_SEL_COLOR_BACK && !g_visible_bgcolor ){
					continue;
				}
				write_byte *= 0xFF;			// ���噯��2�脫�����Ⅱ摰�
				draw_pt = start_addr + (write_x * OSD_CHAR_WIDTH * font_size) * bit_multiple;	// ��撖曇情����雿蔭�蝘餃��
				draw_pt = draw_pt + (i + j * pshdma) * font_size * bit_multiple;				// ���������蝘餃��
				// 1�蝝�2�脫���������������

				for( zoom_y = 0; zoom_y < font_size; zoom_y++ ){
					memset( (draw_pt + zoom_y * pshdma * bit_multiple), write_byte, font_size * bit_multiple );
				}
			}
		}
	}
}

static VOID user_utility_dump_font_write_yuv( const FJ_T_ADDR_YCC *yuv_addr, USHORT pshdma, E_SHARE_YUV_FORMAT format, USHORT pos_x, USHORT pos_y, const UCHAR* str )
{
	INT32 i, j;				// �����������敺∠
	INT32 cnt = 0;			// �����������
	UCHAR ch;				// �����
	INT32 zoom_x, zoom_y;	// �����������敺∠
	UCHAR *draw_pt_y;
#ifdef __YUV_CBCR_WRITE
	UCHAR *draw_pt_cb;
	UCHAR *draw_pt_cr;
#endif
	const UCHAR *p_char;
	UCHAR write_col;
	UCHAR font_size = g_font_yuv_size;
	INT32 ofs_x     = pos_x;
	INT32 ofs_y     = pos_y;
	INT32 draw_offset;
	// ������蔭閮剖��
	UCHAR *start_addr_y  = (UCHAR *)(yuv_addr->AddressY ) + ofs_x + ofs_y * pshdma;
#ifdef __YUV_CBCR_WRITE
	UCHAR cbr_div   = 2;	// format�撖曉��
	UCHAR *start_addr_cb;
	UCHAR *start_addr_cr;

	if( format == E_SHARE_YUV_FORMAT_422_SEMI ){
		start_addr_cb = (UCHAR *)(yuv_addr->AddressCb) + ofs_x + ofs_y * pshdma;
		start_addr_cr = (UCHAR *)(yuv_addr->AddressCr) + ofs_x + ofs_y * pshdma;
	}
	else if( format == E_SHARE_YUV_FORMAT_420_SEMI ){
		start_addr_cb = (UCHAR *)(yuv_addr->AddressCb) + ofs_x + ofs_y / cbr_div * pshdma;
		start_addr_cr = (UCHAR *)(yuv_addr->AddressCr) + ofs_x + ofs_y / cbr_div * pshdma;
	}
	else{
		return;
	}
#endif
	INT32 write_x;
	
	for( cnt = 0, write_x = 0; (ch = str[cnt]) != '\0'; cnt++, write_x++ ){
		// ��������捱摰�
		switch( user_utility_dump_get_font( &p_char, ch ) ){
			// 銵函內
			case E_FONT_PRINT:
				break;
			// �銵�
			case E_FONT_LF:
				ofs_y += OSD_CHAR_HEIGHT * g_font_yuv_size + 4;		// +4�銵��
				// ������蔭��身摰�����
				start_addr_y  = (UCHAR *)(yuv_addr->AddressY ) + ofs_x + ofs_y * pshdma;
#ifdef __YUV_CBCR_WRITE
				if( format == E_SHARE_YUV_FORMAT_422_SEMI ){
					start_addr_cb = (UCHAR *)(yuv_addr->AddressCb) + ofs_x + ofs_y * pshdma;
					start_addr_cr = (UCHAR *)(yuv_addr->AddressCr) + ofs_x + ofs_y * pshdma;
				}
				else if( format == E_SHARE_YUV_FORMAT_420_SEMI ){
					start_addr_cb = (UCHAR *)(yuv_addr->AddressCb) + ofs_x + ofs_y / cbr_div * pshdma;
					start_addr_cr = (UCHAR *)(yuv_addr->AddressCr) + ofs_x + ofs_y / cbr_div * pshdma;
				}
#endif
				write_x = -1;
				continue;
			// 蝛箇
			case E_FONT_SPACE:
			default:
				// ��銵函內������征��SKIP
				if( !g_visible_bgcolor ){
					continue;
				}
				break;
		}
		
		// ���������噯�
		for( i = 0; i < OSD_CHAR_WIDTH; i++ ) {
			for( j = 0; j < OSD_CHAR_HEIGHT; j++ ){
				// Y
				draw_offset = (write_x * OSD_CHAR_WIDTH * font_size);
				write_col = p_char[i + j * OSD_CHAR_WIDTH];				// ���噯��2�脫�����Ⅱ摰�
				// ������憌���
				if( write_col == E_SEL_COLOR_BACK && !g_visible_bgcolor ){
					continue;
				}
				// ��撖曇情����雿蔭�蝘餃��
				draw_pt_y = start_addr_y  + draw_offset;
				// ���������蝘餃��
				draw_pt_y = draw_pt_y + (i + j * pshdma) * font_size;
				// 1�蝝�2�脫���������������
				for( zoom_x = 0; zoom_x < font_size; zoom_x++ ){
					for( zoom_y = 0; zoom_y < font_size; zoom_y++ ){
						*(draw_pt_y + zoom_x + zoom_y * pshdma) = g_font_color[write_col][E_OSD_COLOR_Y];
					}
				}
#ifdef __YUV_CBCR_WRITE				// CbCr
				if( format == E_SHARE_YUV_FORMAT_422_SEMI ){
					draw_offset = (write_x * OSD_CHAR_WIDTH * font_size);
					// ��撖曇情����雿蔭�蝘餃��
					draw_pt_cb = start_addr_cb + draw_offset;
					draw_pt_cr = start_addr_cr + draw_offset + 1;
					// ���������蝘餃��
					draw_pt_cb = draw_pt_cb + (i + j * pshdma) * font_size;
					draw_pt_cr = draw_pt_cr + (i + j * pshdma) * font_size;
					// 1�蝝�2�脫���������������
					for( zoom_x = 0; zoom_x < font_size; zoom_x += cbr_div ){
						for( zoom_y = 0; zoom_y < font_size; zoom_y++ ){
							*(draw_pt_cb + zoom_x + zoom_y * pshdma) = g_font_color[write_col][E_OSD_COLOR_U];
							*(draw_pt_cr + zoom_x + zoom_y * pshdma) = g_font_color[write_col][E_OSD_COLOR_V];
						}
					}
				}
				else if( format == E_SHARE_YUV_FORMAT_420_SEMI ){
					draw_offset = (write_x * OSD_CHAR_WIDTH * font_size);
					// ��撖曇情����雿蔭�蝘餃��
					draw_pt_cb = start_addr_cb + draw_offset;
					draw_pt_cr = start_addr_cr + draw_offset + 1;
					// ���������蝘餃��
					draw_pt_cb = draw_pt_cb + (i + j * pshdma / cbr_div / cbr_div) * font_size;
					draw_pt_cr = draw_pt_cr + (i + j * pshdma / cbr_div / cbr_div) * font_size;
					// 1�蝝�2�脫���������������
					for( zoom_x = 0; zoom_x < font_size; zoom_x += 2 ){
						for( zoom_y = 0; zoom_y < font_size/cbr_div; zoom_y++ ){
							*(draw_pt_cb + zoom_x + zoom_y * pshdma) = g_font_color[write_col][E_OSD_COLOR_U];
							*(draw_pt_cr + zoom_x + zoom_y * pshdma) = g_font_color[write_col][E_OSD_COLOR_V];
						}
					}
				}
#endif
			}
		}
	}
}

static int iip_bayer_memcpy(const ULONG src_addr, const ULONG dst_addr, const ULONG src_h_byte, const USHORT src_v_size, const UCHAR pixel_depth)
{
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_RR_BUF_BYTES];
	ULONG param_buffer_addr = ROUNDUP_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 1000;
	T_IM_IIP_UTIL_RR iip_util_rr0;
	int rval = 0;

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_07, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "IIP bayer memcpy"));

	memset(&iip_util_rr0, 0, sizeof(T_IM_IIP_UTIL_RR));

	iip_util_rr0.afn_unitid = E_IM_IIP_UNIT_ID_AFN0;
	iip_util_rr0.sl_unitid = E_IM_IIP_UNIT_ID_SL2;
	iip_util_rr0.src_pixid = E_IM_IIP_PIXID_4;
	iip_util_rr0.dst_pixid = E_IM_IIP_PIXID_5;

	iip_util_rr0.src.gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_rr0.src.gbl.frame_type = E_IM_IIP_FTYPE_PLANE;
	iip_util_rr0.src.gbl.pix_format = E_IM_IIP_PFMT_BAYER;
	iip_util_rr0.src.gbl.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.src.gbl.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.src.gbl.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.src.gbl.masterIF_Alpha = D_IM_IIP_MASTER_IF1;

	iip_util_rr0.dst.gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_rr0.dst.gbl.frame_type = E_IM_IIP_FTYPE_PLANE;
	iip_util_rr0.dst.gbl.pix_format = E_IM_IIP_PFMT_BAYER;
	iip_util_rr0.dst.gbl.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.dst.gbl.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.dst.gbl.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.dst.gbl.masterIF_Alpha = D_IM_IIP_MASTER_IF1;

	iip_util_rr0.afn_cache_select = E_IM_IIP_PARAM_CSEL_0;
	iip_util_rr0.pix_depth = pixel_depth <= 8 ? D_IM_IIP_PDEPTH_8BITS : D_IM_IIP_PDEPTH_16BITS;
	iip_util_rr0.rotation = E_IM_IIP_UTIL_ROTATE_000;
	iip_util_rr0.flip = E_IM_IIP_UTIL_FLIP_OFF;
	iip_util_rr0.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;

	iip_util_rr0.src.rect.width				= pixel_depth == 8 ? src_h_byte : src_h_byte >> 1;
	iip_util_rr0.src.rect.lines				= src_v_size;
	iip_util_rr0.src.gbl.pix_format			= E_IM_IIP_PFMT_BAYER;
	iip_util_rr0.src.gbl.width				= pixel_depth == 8 ? src_h_byte : src_h_byte >> 1;
	iip_util_rr0.src.gbl.lines				= src_v_size;
	iip_util_rr0.src.gbl.line_bytes.Y_G		= src_h_byte;
	iip_util_rr0.src.gbl.line_bytes.Cb_B	= 0;
	iip_util_rr0.src.gbl.line_bytes.Cr_R	= 0;
	iip_util_rr0.src.gbl.line_bytes.Alpha	= 0;
	iip_util_rr0.src.gbl.addr.Y_G			= src_addr;
	iip_util_rr0.src.gbl.addr.Cb_B			= 0;
	iip_util_rr0.src.gbl.addr.Cr_R			= 0;
	iip_util_rr0.src.gbl.addr.Alpha			= 0;

	iip_util_rr0.dst.rect.width				= pixel_depth == 8 ? src_h_byte : src_h_byte >> 1;
	iip_util_rr0.dst.rect.lines				= src_v_size;
	iip_util_rr0.dst.gbl.pix_format			= E_IM_IIP_PFMT_BAYER;
	iip_util_rr0.dst.gbl.width				= pixel_depth == 8 ? src_h_byte : src_h_byte >> 1;
	iip_util_rr0.dst.gbl.lines				= src_v_size;
	iip_util_rr0.dst.gbl.line_bytes.Y_G		= src_h_byte;
	iip_util_rr0.dst.gbl.line_bytes.Cb_B	= 0;
	iip_util_rr0.dst.gbl.line_bytes.Cr_R	= 0;
	iip_util_rr0.dst.gbl.line_bytes.Alpha	= 0;
	iip_util_rr0.dst.gbl.addr.Y_G			= dst_addr;
	iip_util_rr0.dst.gbl.addr.Cb_B			= 0;
	iip_util_rr0.dst.gbl.addr.Cr_R			= 0;
	iip_util_rr0.dst.gbl.addr.Alpha			= 0;

	rval = IM_IIP_Utility_ResizeRotate( &iip_util_rr0,
										NULL,
										param_master_IF,
										param_buffer_addr,
										wait_time );
	UPRINTF_ERR(rval);

	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_07, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "IIP bayer memcpy"));

	return rval;
}

