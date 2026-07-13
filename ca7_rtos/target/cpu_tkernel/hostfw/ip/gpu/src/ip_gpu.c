/**
 * @file		ip_gpu.c
 * @brief		GPU rotate
 * @author		Hattori
 * @since		2016/07/11
 * @date		2016/07/14
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT 2016</I></B>
 */

// ****************************************************************************
//                          Include
// ****************************************************************************
#include <string.h>
#include "vg_lite.h"
#include "vg_lite_matrix.h"
#include "ip_gpu.h"
#include "measure_time.h"

#include "ddim_typedef.h"
#include "os_user_custom.h"
// ****************************************************************************
//                          Define
// ****************************************************************************

// ****************************************************************************
//                           Structure
// ****************************************************************************

// ****************************************************************************
//                           Global Data
// ****************************************************************************
static vg_lite_buffer_t buffer;     //offscreen framebuffer object for rendering.

static int vg_bank_no = 0;
static int vg_dir = 0;
// ****************************************************************************
//                        External Method Definition
// ****************************************************************************

// ****************************************************************************
//                        Local Method Definition
// ****************************************************************************

// ****************************************************************************
//                          	Method
// ****************************************************************************

/******************************************************************************
 * @brief	Library Open
 * @param	None
 * @retval	D_IP_GPU_OK
 * @retval	D_IP_GPU_NG
 */
INT32 IP_GPU_Open( VOID )
{
	INT32	ret;

	ret = 	vg_set_heap( D_IP_GPU_HEAP, 256*1024 );
	if( ret ){
		return (D_IP_GPU_NG);
	}

	return D_IP_GPU_OK;
}

VOID IP_GPU_rotateHDMI()
{
    vg_lite_matrix_t t_matrix;
	vg_lite_buffer_t image;
	vg_lite_float_t		x;
	vg_lite_float_t		y;
//	static	int			bank_no=0;
//	static	int			dir=0;

	x = 480.0f;
	y = 360.0f;

	image.width		= 960;
	image.height	= 720;
	image.stride	= 3840;

	image.format	= VG_LITE_RGBA8888;
	image.address	= D_IP_GPU_HDMI_INPUT;
	image.tiled		= 0;

	buffer.width  = 960;
	buffer.height = 720;
	buffer.stride = 3840;

	if(vg_bank_no==0){
		buffer.address	= D_IP_GPU_OUTPUT_HDMI_BANK0;
	}else{
		buffer.address	= D_IP_GPU_OUTPUT_HDMI_BANK1;
	}
	vg_lite_clear( &buffer, NULL, 0);
    vg_lite_finish();

	vg_lite_identity(&t_matrix);
	vg_lite_translate(x, y, &t_matrix);

	switch(vg_dir){
	case 0:
	 	vg_lite_rotate(0.0f, &t_matrix);
		vg_lite_scale(1.00f, 1.00f, &t_matrix);
		break;
	case 1:
	 	vg_lite_rotate(90.0f, &t_matrix);
		vg_lite_scale(0.75f, 1.333f, &t_matrix);
		break;
	case 2:
	 	vg_lite_rotate(180.0f, &t_matrix);
		vg_lite_scale(1.00f, 1.00f, &t_matrix);
		break;
	case 3:
	 	vg_lite_rotate(-90.0f, &t_matrix);
		vg_lite_scale(0.75f, 1.333f, &t_matrix);
		break;
	}

	vg_lite_translate(-x, -y, &t_matrix);
	vg_lite_blit(&buffer, &image, &t_matrix, VG_LITE_BLEND_NONE, 0, VG_LITE_FILTER_BI_LINEAR);

    vg_lite_finish();

	vg_bank_no = (vg_bank_no==0)? 1:0;
	vg_dir++;
	if(vg_dir==4){
		vg_dir = 0;
	}
}

VOID IP_GPU_rotateLCD()
{
    vg_lite_matrix_t t_matrix;
	vg_lite_buffer_t image;
	vg_lite_float_t		x;
	vg_lite_float_t		y;
//	static	int			bank_no=0;
//	static	int			dir=0;

	x = 160.0f;
	y = 120.0f;

	image.width		= 320;
	image.height	= 240;
	image.stride	= 1280;
	image.format	= VG_LITE_RGBA8888;
	image.address	= D_IP_GPU_INPUT;
	image.tiled		= 0;

	buffer.width  = 320;
	buffer.height = 240;
	buffer.stride = 1280;
	buffer.format = VG_LITE_RGBA8888;

	if(vg_bank_no==0){
		buffer.address	= D_IP_GPU_OUTPUT_LCD_BANK0;
	}else{
		buffer.address	= D_IP_GPU_OUTPUT_LCD_BANK1;
	}
	vg_lite_clear( &buffer, NULL, 0);
    vg_lite_finish();

	vg_lite_identity(&t_matrix);
	vg_lite_translate(x, y, &t_matrix);

	switch(vg_dir){
	case 0:
	 	vg_lite_rotate(0.0f, &t_matrix);
		vg_lite_scale(1.00f, 1.00f, &t_matrix);
		break;
	case 1:
	 	vg_lite_rotate(90.0f, &t_matrix);
		vg_lite_scale(0.75f, 1.333f, &t_matrix);
		break;
	case 2:
	 	vg_lite_rotate(180.0f, &t_matrix);
		vg_lite_scale(1.00f, 1.00f, &t_matrix);
		break;
	case 3:
	 	vg_lite_rotate(-90.0f, &t_matrix);
		vg_lite_scale(0.75f, 1.333f, &t_matrix);
		break;
	}

	vg_lite_translate(-x, -y, &t_matrix);
	vg_lite_blit(&buffer, &image, &t_matrix, VG_LITE_BLEND_NONE, 0, VG_LITE_FILTER_BI_LINEAR);

    vg_lite_finish();

	vg_bank_no = (vg_bank_no==0)? 1:0;
	vg_dir++;
	if(vg_dir==4){
		vg_dir = 0;
	}
}

VOID IP_GPU_BufferSetLCD(VOID)
{
	IP_GPU_SoftReset();
	vg_lite_init(32, 32);
	buffer.width  = 320;
	buffer.height = 480;
	buffer.stride = 1280;
	buffer.format = VG_LITE_RGBA8888;
	buffer.address = D_IP_GPU_OUTPUT_LCD_BANK0;
	vg_lite_allocate(&buffer);
	vg_lite_clear( &buffer, NULL, 0x00000000);
    vg_lite_finish();
}

VOID IP_GPU_BufferSetHDMI(VOID)
{
	IP_GPU_SoftReset();
	vg_lite_init(32, 32);
	buffer.width  = 960;
	buffer.height = 1440;
	buffer.stride = 3840;
	buffer.format = VG_LITE_RGBA8888;
	buffer.address = D_IP_GPU_OUTPUT_HDMI_BANK0;
	vg_lite_allocate(&buffer);
	vg_lite_clear( &buffer, NULL, 0x00000000);
    vg_lite_finish();
}


VOID IP_GPU_SoftReset(VOID)
{
	vg_soft_reset();
}
