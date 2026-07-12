#ifndef _OSD_FONT_H
#define _OSD_FONT_H

#define OSD_CHAR_HEIGHT	8
#define OSD_CHAR_WIDTH	8

#define OSD_NUM			20
#define BUFF_NUM		2
#define YUV_ALPHA		2
#define YUV_DRAW_BUFF_SIZE		0x30000		// 192KB
#define ALPHA_DRAW_BUFF_SIZE	0x20000		// 128KB

#define RGBA_DRAW_BUFF_SIZE		0x40000		// 256KB

#define OSD_ID_0		(0)
#define OSD_ID_1		(1)
#define OSD_ID_2		(2)
#define OSD_ID_3		(3)
#define OSD_ID_4		(4)
#define OSD_ID_5		(5)
#define OSD_ID_6		(6)
#define OSD_ID_7		(7)
#define OSD_ID_8		(8)
#define OSD_ID_9		(9)
#define OSD_ID_10		(10)
#define OSD_ID_11		(11)
#define OSD_ID_12		(12)
#define OSD_ID_13		(13)
#define OSD_ID_14		(14)
#define OSD_ID_15		(15)
#define OSD_ID_16		(16)
#define OSD_ID_17		(17)
#define OSD_ID_18		(18)
#define OSD_ID_19		(19)
#define OSD_ID_MAX		(19)

#define CAM_ID_0		(0)
#define CAM_ID_1		(1)
#define CAM_ID_2		(2)
#define CAM_ID_3		(3)
#define CAM_ID_MAX		(3)
#define CAM_ID_INVALID	(255)


#define X_POS_MAX		(65535)
#define Y_POS_MAX		(65535)

#define FONTCOLOR_MAX	(5)
#define FONTSIZE_MAX	(2)
#define PARENCY_MAX		(1)
#define TEXT_LEN_MAX	(32)


typedef void				VOID;
typedef int					INT32;
typedef unsigned int		UINT32;
typedef short				SHORT;
typedef unsigned short		USHORT;
//typedef char				CHAR;
typedef unsigned char		UCHAR;

typedef enum{
	E_FONT_SPACE,			// 空白
	E_FONT_PRINT,			// 表示可
	E_FONT_LF,				// 改行
	E_FONT_TYPE_NUM,
}E_FONT_TYPE;
// 表示色
typedef enum{
	E_SEL_COLOR_BACK,		// 背景色
	E_SEL_COLOR_TEXT,		// 文字色
	E_SEL_COLOR_NUM,
} E_SEL_COLOR;
// 色種別
typedef enum{
	E_OSD_COLOR_Y,
	E_OSD_COLOR_U,
	E_OSD_COLOR_V,
	E_OSD_COLOR_NUM,
} E_OSD_COLOR;

typedef struct{
	UINT32	cam_id;
	UINT32	yuv_dst_addr;
	UINT32	linux_busy_flg;
	UINT32	rtos_busy_flg;
	UINT32	x_pos;
	UINT32	y_pos;
	UINT32	yuv_width;
	UINT32	yuv_height;
}T_OSD_INFORMATION_HEADER;

typedef struct{
	T_OSD_INFORMATION_HEADER osdih[OSD_NUM];
	UCHAR osd_image_area[(YUV_DRAW_BUFF_SIZE+ALPHA_DRAW_BUFF_SIZE)*(OSD_NUM+4)];
}T_OSDCMD_COMMON_MEM;

VOID set_osd_font_color( UCHAR color_id );
VOID set_osd_font_alpha( UCHAR color_id );
VOID font_write_yuv( UCHAR *yuv_addr, UCHAR *alpha_addr, USHORT pshdma, const UCHAR* str, UCHAR font_size );
VOID font_write_RGBA4444( UCHAR *yuv_addr, USHORT pshdma, const UCHAR* str, UCHAR font_size );
E_FONT_TYPE get_write_font( const UCHAR** font, UCHAR ascii );

#endif
