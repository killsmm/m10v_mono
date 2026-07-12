#include <stdio.h>
#include <stdlib.h>
#include "osd_font.h"

#define PIXEL_SIZE_RGBA4444			2
#define TRANSPARENT_RGBA4444		0
#define NOT_TRANSPARENT_RGBA4444	15


UCHAR yuv_color[E_SEL_COLOR_NUM][E_OSD_COLOR_NUM];
UCHAR alpha_color[E_SEL_COLOR_NUM][E_OSD_COLOR_NUM];

UCHAR ba_color[E_SEL_COLOR_NUM];
UCHAR rg_color[E_SEL_COLOR_NUM];

const UCHAR colorset[6][6] ={
	
	{255, 128, 128,   0, 128, 128},	// 0 WHITE, BLACK
	{  0, 128, 128, 255, 128, 128},	// 1 BLACK, WHITE
	{128,   0,   0,   0, 128, 128},	// 2 GREEN, BLACK
	{128, 255,   0,   0, 128, 128},	// 3 BLUE, BLACK
	{128, 255, 255,   0, 128, 128},	// 4 PEACH, BLACK
	{128,   0, 255,   0, 128, 128}	// 5 YELLOW, BLACK
};

const UCHAR alpha_colorset[2][6] ={
	{  0,   0,   0,   0,   0,   0},	// 0 NO TRANS PARENT
	{  0,   0,   0, 255,   0,   0}	// 1    TRANS PARENT
};

const UCHAR colorset_rgba[6][6] ={
	//R   G   B   R   G   B			// Must 4bit width
	{15, 15, 15,  0,  0,  0},		// 0 WHITE, BLACK
	{ 0,  0,  0, 15, 15, 15},		// 1 BLACK, WHITE
	{ 0, 15,  0,  0,  0,  0},		// 2 GREEN, BLACK
	{ 0,  0, 15,  0,  0,  0},		// 3 BLUE, BLACK
	{15,  0, 15,  0,  0,  0},		// 4 PEACH, BLACK
	{15, 15,  0,  0,  0,  0}		// 5 YELLOW, BLACK
};

VOID set_osd_font_color( UCHAR color_id )
{
	// Disp color
	yuv_color[E_SEL_COLOR_TEXT][E_OSD_COLOR_Y] = colorset[color_id][0];
	yuv_color[E_SEL_COLOR_TEXT][E_OSD_COLOR_U] = colorset[color_id][1];
	yuv_color[E_SEL_COLOR_TEXT][E_OSD_COLOR_V] = colorset[color_id][2];
	yuv_color[E_SEL_COLOR_BACK][E_OSD_COLOR_Y] = colorset[color_id][3];
	yuv_color[E_SEL_COLOR_BACK][E_OSD_COLOR_U] = colorset[color_id][4];
	yuv_color[E_SEL_COLOR_BACK][E_OSD_COLOR_V] = colorset[color_id][5];
	
	ba_color[E_SEL_COLOR_TEXT] = (colorset_rgba[color_id][2]<<4) | NOT_TRANSPARENT_RGBA4444;
	ba_color[E_SEL_COLOR_BACK] = (colorset_rgba[color_id][5]<<4) | (ba_color[E_SEL_COLOR_BACK]&0x0F);
	rg_color[E_SEL_COLOR_TEXT] = (colorset_rgba[color_id][0]<<4) | colorset_rgba[color_id][1];
	rg_color[E_SEL_COLOR_BACK] = (colorset_rgba[color_id][3]<<4) | colorset_rgba[color_id][4];
}

VOID set_osd_font_alpha( UCHAR color_id )
{
	UCHAR transparent;
	
	// TRANS PARENT or NOT
	alpha_color[E_SEL_COLOR_TEXT][E_OSD_COLOR_Y] = alpha_colorset[color_id][0];
	alpha_color[E_SEL_COLOR_TEXT][E_OSD_COLOR_U] = alpha_colorset[color_id][1];	//	not use
	alpha_color[E_SEL_COLOR_TEXT][E_OSD_COLOR_V] = alpha_colorset[color_id][2];	//	not use
	alpha_color[E_SEL_COLOR_BACK][E_OSD_COLOR_Y] = alpha_colorset[color_id][3];
	alpha_color[E_SEL_COLOR_BACK][E_OSD_COLOR_U] = alpha_colorset[color_id][4];	//	not use
	alpha_color[E_SEL_COLOR_BACK][E_OSD_COLOR_V] = alpha_colorset[color_id][5];	//	not use
	
	if( color_id == 0 ){
		transparent = NOT_TRANSPARENT_RGBA4444;
	}else{
		transparent = TRANSPARENT_RGBA4444;
	}
	ba_color[E_SEL_COLOR_BACK] = (ba_color[E_SEL_COLOR_BACK]&0xF0) | (transparent&0x0F);

}


VOID font_write_yuv( UCHAR *yuv_addr, UCHAR *alpha_addr, USHORT pshdma, const UCHAR* str, UCHAR font_size )
{
	// YUV420 SEMI-PLANE ONLY //
	INT32 i, j;				// 文字サイズ指定ループ制御用
	INT32 cnt = 0;			// 文字列内参照用カウンタ
	UCHAR ch;				// 文字データ
	INT32 zoom_x, zoom_y;	// フォントサイズループ制御用
	UCHAR *draw_pt_y, *draw_pt_alpha;
	UCHAR *draw_pt_cb;
	UCHAR *draw_pt_cr;
	const UCHAR *p_char;
	UCHAR write_col;
	UCHAR cbr_div   = 2;	// YUV420
	INT32 draw_offset;
	// 描画開始位置設定
	UCHAR *start_addr_y  = yuv_addr;
	UCHAR *start_addr_alpha  = alpha_addr;
	UCHAR *start_addr_cb = (UCHAR *)(start_addr_y + 8*8*pshdma*font_size*font_size);
//	UCHAR *start_addr_cr = (UCHAR *)(start_addr_cb + 8*8/4*pshdma*font_size*font_size);
	USHORT hor_size = 8*pshdma*font_size;
	INT32 write_x;

	
	for( cnt = 0, write_x = 0; (ch = str[cnt]) != '\0'; cnt++, write_x++ ){
		// 参照する文字を決定
		if( get_write_font( &p_char, ch ) != E_FONT_PRINT ){
			printf("illegal ascii0x%02x\n",ch);
			return;
		}
		
		// 文字データを書き込み
		for( i = 0; i < OSD_CHAR_WIDTH; i++ ) {
			for( j = 0; j < OSD_CHAR_HEIGHT; j++ ){
				write_col = p_char[i + j * OSD_CHAR_WIDTH];				// 書き込む2進数データを確定
				
				// Y
				draw_offset = (write_x * OSD_CHAR_WIDTH * font_size);
				// 描画対象の文字の位置に移動
				draw_pt_y = start_addr_y  + draw_offset;
				draw_pt_alpha = start_addr_alpha  + draw_offset;
				// 文字内の描画部分に移動
				draw_pt_y = draw_pt_y + (i + j * hor_size) * font_size;
				draw_pt_alpha = draw_pt_alpha + (i + j * hor_size) * font_size;
				// 1画素分の2進数データを指定サイズ描画処理
				for( zoom_x = 0; zoom_x < font_size; zoom_x++ ){
					for( zoom_y = 0; zoom_y < font_size; zoom_y++ ){
						*(draw_pt_y + zoom_x + zoom_y * hor_size) = yuv_color[write_col][E_OSD_COLOR_Y];
						*(draw_pt_alpha + zoom_x + zoom_y * hor_size) = alpha_color[write_col][E_OSD_COLOR_Y];
					}
				}
				// CbCr semi plane
				draw_offset = (write_x * OSD_CHAR_WIDTH / cbr_div * 2 * font_size);
				// 描画対象の文字の位置に移動
				draw_pt_cb = start_addr_cb + draw_offset;
//				draw_pt_cr = start_addr_cr + draw_offset;
				// 文字内の描画部分に移動
				draw_pt_cb = draw_pt_cb + (i + j * hor_size / cbr_div) * 2 * font_size / cbr_div;
				draw_pt_cr = draw_pt_cb + 1;
//				draw_pt_cr = draw_pt_cr + (i + j * hor_size / cbr_div) * font_size / cbr_div;
				// 1画素分の2進数データを指定サイズ描画処理
				for( zoom_x = 0; zoom_x < font_size/cbr_div; zoom_x++ ){
					for( zoom_y = 0; zoom_y < font_size/cbr_div; zoom_y++ ){
						*(draw_pt_cb + 2*zoom_x + 2*zoom_y * hor_size / cbr_div) = yuv_color[write_col][E_OSD_COLOR_U];
						*(draw_pt_cr + 2*zoom_x + 2*zoom_y * hor_size / cbr_div) = yuv_color[write_col][E_OSD_COLOR_V];
//						printf("0x%08x, 0x%08x\n",draw_pt_cb,draw_pt_cr);
					}
				}
			}
		}
	}
}

VOID font_write_RGBA4444( UCHAR *buf, USHORT chara_num, const UCHAR* str, UCHAR font_size )
{
	INT32 i, j;				// for pixel loop
	INT32 cnt;				// character count
	INT32 write_x;			// cnt * pixelbyte
	UCHAR ch;				// ascii data
	INT32 zoom_x, zoom_y;	// for font_size loop
	const UCHAR *p_char;	// pixel infomation of the font
	UCHAR write_col;		// TEXT or BACKGROUND
	INT32 draw_offset;
	USHORT hor_pixel = 8*chara_num*font_size;	// horizon pixels in the string
	UCHAR *start_addr_ba = buf;		// string BA address
	UCHAR *start_addr_rg = buf + 1;	// string RG address
	UCHAR *draw_font_ba;	// font BA address in the string
	UCHAR *draw_font_rg;	// font RG address in the string
	UCHAR *draw_pixel_ba;	// pixel BA address in the font
	UCHAR *draw_pixel_rg;	// pixel RG address in the font
	
	for( cnt = 0, write_x = 0; (ch = str[cnt]) != '\0'; cnt++, write_x+=PIXEL_SIZE_RGBA4444 ){
		// get font infomation
		if( get_write_font( &p_char, ch ) != E_FONT_PRINT ){
			printf("illegal ascii0x%02x\n",ch);
			return;
		}
		
		
		draw_offset = (write_x * OSD_CHAR_WIDTH * font_size);
		draw_font_ba = start_addr_ba + draw_offset;
		draw_font_rg = start_addr_rg + draw_offset;
		
		// drawing font pixel by pixel
		for( j = 0; j < OSD_CHAR_HEIGHT; j++ ){
			for( i = 0; i < OSD_CHAR_WIDTH; i++ ) {
				write_col = p_char[i + j * OSD_CHAR_WIDTH];	// TEXT or BACKGROUND
//				printf("p_char[i+j*8]=%d\n",write_col);
				draw_pixel_ba = draw_font_ba + (i + j * hor_pixel) * font_size * PIXEL_SIZE_RGBA4444;
				draw_pixel_rg = draw_font_rg + (i + j * hor_pixel) * font_size * PIXEL_SIZE_RGBA4444;
				// drawing (font_size * font_size)
				for( zoom_y = 0; zoom_y < font_size; zoom_y++ ){
					for( zoom_x = 0; zoom_x < font_size; zoom_x++ ){
						*(draw_pixel_ba + (zoom_x + zoom_y * hor_pixel) * PIXEL_SIZE_RGBA4444) = ba_color[write_col];
						*(draw_pixel_rg + (zoom_x + zoom_y * hor_pixel) * PIXEL_SIZE_RGBA4444) = rg_color[write_col];
//						printf("%08xh %08xh\n",draw_pixel_ba + (zoom_x + zoom_y * hor_pixel) * PIXEL_SIZE_RGBA4444,
//						draw_pixel_rg + (zoom_x + zoom_y * hor_pixel) * PIXEL_SIZE_RGBA4444);
					}
				}
//				printf("\n");
			}
		}
	}
}

