/**
 * @file		ipc_stc.c
 * @brief		API for Stitching
 * @author		Hattori
 * @since		2016/12/02
 * @date		2017/11/10
 * @date		2017/11/14
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT 2017</I></B>
 */

// ****************************************************************************
//                          Include
// ****************************************************************************
#include <string.h>
#include "ddim_typedef.h"
#include "l1l2cache.h"
#include "ipc_iip.h"
#include "ipc_blend.h"
#include "ip_stc.h"
#include "im_xch.h"
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
static	T_IPC_IMAGE		alp0Img;
static	T_IPC_IMAGE		alp1Img;
static	IPC_INT			gIpAwidth;
static	IPC_INT			gIpOffset;
static	IPC_INT			gIpLofs;
static	IPC_UINT		gIpStcMode = 0;
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
 * @brief	Stiting Initialize
 * @param	IPC_INT			mode
 * @retval	none
 */

VOID IP_STC_Init( IPC_UINT mode )
{
	gIpStcMode = mode;
}

/******************************************************************************
 * @brief	Stiting Main
 * @param	T_IPC_IMAGE*	img0
 * @param	T_IPC_IMAGE*	img1
 * @param	IPC_INT			offsetx
 * @retval	none
 */

VOID IP_STC_Stitch(
	T_IPC_IMAGE* img0,			// 全体画像
	T_IPC_IMAGE* img1,			// バックアップ画像1(Right)
	T_IPC_IMAGE* img2,			// バックアップ画像2(Left)
	IPC_INT		 offsetx,		// 重ね描画オフセット
	IPC_INT		 offsetx2		// 正面画像先頭オフセット
)
{
	T_IPC_IMAGE		in0img;
	T_IPC_IMAGE		in1img;
	T_IPC_IMAGE		outimg;
	IPC_INT			ofs;

	switch( gIpStcMode ){
	case D_IP_STC_MODE_HORIZONTAL:
		offsetx = offsetx & ~0x1f;
		in0img.imgForm				= img0->imgForm;
		in0img.bitDepth				= img0->bitDepth;
		in0img.dataForm				= img0->dataForm;
		in0img.imgInfo[0].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[0].pImgAdrs + offsetx);
		in0img.imgInfo[0].gImgSize	= (img0->imgInfo[0].gImgSize + 31) & ~0x1f;
		in0img.imgInfo[0].wImgSize	= (img0->imgInfo[0].wImgSize + 31) & ~0x1f;
		in0img.imgInfo[0].hImgSize	= img0->imgInfo[0].hImgSize;
		in0img.imgInfo[1].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[1].pImgAdrs + offsetx);
		in0img.imgInfo[1].gImgSize	= (img0->imgInfo[1].gImgSize + 31) & ~0x1f;
		in0img.imgInfo[1].wImgSize	= (img0->imgInfo[1].wImgSize + 15) & ~0xf;
		in0img.imgInfo[1].hImgSize	= img0->imgInfo[1].hImgSize;

		outimg.imgForm				= img0->imgForm;
		outimg.bitDepth				= img0->bitDepth;
		outimg.dataForm				= img0->dataForm;
		outimg.imgInfo[0].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[0].pImgAdrs + offsetx);
		outimg.imgInfo[0].gImgSize	= (img0->imgInfo[0].gImgSize + 31) & ~0x1f;
		outimg.imgInfo[0].wImgSize	= (img0->imgInfo[0].wImgSize + 31) & ~0x1f;
		outimg.imgInfo[0].hImgSize	= img0->imgInfo[0].hImgSize;
		outimg.imgInfo[1].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[1].pImgAdrs + offsetx);
		outimg.imgInfo[1].gImgSize	= (img0->imgInfo[1].gImgSize + 31) & ~0x1f;
		outimg.imgInfo[1].wImgSize	= (img0->imgInfo[1].wImgSize + 15) & ~0xf;
		outimg.imgInfo[1].hImgSize	= img0->imgInfo[1].hImgSize;

		in1img.imgForm				= img1->imgForm;
		in1img.bitDepth				= img1->bitDepth;
		in1img.dataForm				= img1->dataForm;
		in1img.imgInfo[0].pImgAdrs	= (IPC_VOID*)(img1->imgInfo[0].pImgAdrs);
		in1img.imgInfo[0].gImgSize	= (img1->imgInfo[0].gImgSize + 31) & ~0x1f;
		in1img.imgInfo[0].wImgSize	= (img1->imgInfo[0].wImgSize + 31) & ~0x1f;
		in1img.imgInfo[0].hImgSize	= img1->imgInfo[0].hImgSize;
		in1img.imgInfo[1].pImgAdrs	= (IPC_VOID*)(img1->imgInfo[1].pImgAdrs);
		in1img.imgInfo[1].gImgSize	= (img1->imgInfo[1].gImgSize + 31) & ~0x1f;
		in1img.imgInfo[1].wImgSize	= (img1->imgInfo[1].wImgSize + 15) & ~0xf;
		in1img.imgInfo[1].hImgSize	= img1->imgInfo[1].hImgSize;

		IPC_BLEND_Blend(&outimg, &in0img, &in1img, &alp1Img);

		in0img.imgForm				= img0->imgForm;
		in0img.bitDepth				= img0->bitDepth;
		in0img.dataForm				= img0->dataForm;
		in0img.imgInfo[0].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[0].pImgAdrs);
		in0img.imgInfo[0].gImgSize	= img0->imgInfo[0].gImgSize;
		in0img.imgInfo[0].wImgSize	= (gIpAwidth + 31) & ~0x1f;
		in0img.imgInfo[0].hImgSize	= img0->imgInfo[0].hImgSize;
		in0img.imgInfo[1].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[1].pImgAdrs);
		in0img.imgInfo[1].gImgSize	= img0->imgInfo[1].gImgSize;
		in0img.imgInfo[1].wImgSize	= (gIpAwidth/2 + 15) & ~0xf;
		in0img.imgInfo[1].hImgSize	= img0->imgInfo[1].hImgSize;

		ofs = img0->imgInfo[0].gImgSize - img1->imgInfo[0].wImgSize;

		in1img.imgForm				= img0->imgForm;
		in1img.bitDepth				= img0->bitDepth;
		in1img.dataForm				= img0->dataForm;
		in1img.imgInfo[0].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[0].pImgAdrs + ofs);
		in1img.imgInfo[0].gImgSize	= img0->imgInfo[0].gImgSize;
		in1img.imgInfo[0].wImgSize	= (gIpAwidth + 31) & ~0x1f;
		in1img.imgInfo[0].hImgSize	= img0->imgInfo[0].hImgSize;
		in1img.imgInfo[1].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[1].pImgAdrs + ofs);
		in1img.imgInfo[1].gImgSize	= img0->imgInfo[1].gImgSize;
		in1img.imgInfo[1].wImgSize	= (gIpAwidth/2 + 15) & ~0xf;
		in1img.imgInfo[1].hImgSize	= img0->imgInfo[1].hImgSize;

		outimg.imgForm				= img0->imgForm;
		outimg.bitDepth				= img0->bitDepth;
		outimg.dataForm				= img0->dataForm;
		outimg.imgInfo[0].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[0].pImgAdrs);
		outimg.imgInfo[0].gImgSize	= img0->imgInfo[0].gImgSize;
		outimg.imgInfo[0].wImgSize	= (gIpAwidth + 31) & ~0x1f;
		outimg.imgInfo[0].hImgSize	= img0->imgInfo[0].hImgSize;
		outimg.imgInfo[1].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[1].pImgAdrs);
		outimg.imgInfo[1].gImgSize	= img0->imgInfo[1].gImgSize;
		outimg.imgInfo[1].wImgSize	= (gIpAwidth/2 + 15) & ~0xf;
		outimg.imgInfo[1].hImgSize	= img0->imgInfo[1].hImgSize;

		IPC_BLEND_Blend(&outimg, &in0img, &in1img, &alp0Img);
		break;
	case D_IP_STC_MODE_HORIZONTAL_CENTER:
		offsetx = offsetx & ~0x1f;
		in0img.imgForm				= img0->imgForm;
		in0img.bitDepth				= img0->bitDepth;
		in0img.dataForm				= img0->dataForm;
		in0img.imgInfo[0].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[0].pImgAdrs + offsetx + offsetx2);
		in0img.imgInfo[0].gImgSize	= (img0->imgInfo[0].gImgSize + 31) & ~0x1f;
		in0img.imgInfo[0].wImgSize	= (img0->imgInfo[0].wImgSize + 31) & ~0x1f;
		in0img.imgInfo[0].hImgSize	= img0->imgInfo[0].hImgSize;
		in0img.imgInfo[1].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[1].pImgAdrs + offsetx + offsetx2);
		in0img.imgInfo[1].gImgSize	= (img0->imgInfo[1].gImgSize + 31) & ~0x1f;
		in0img.imgInfo[1].wImgSize	= (img0->imgInfo[1].wImgSize + 15) & ~0xf;
		in0img.imgInfo[1].hImgSize	= img0->imgInfo[1].hImgSize;

		outimg.imgForm				= img0->imgForm;
		outimg.bitDepth				= img0->bitDepth;
		outimg.dataForm				= img0->dataForm;
		outimg.imgInfo[0].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[0].pImgAdrs + offsetx + offsetx2);
		outimg.imgInfo[0].gImgSize	= (img0->imgInfo[0].gImgSize + 31) & ~0x1f;
		outimg.imgInfo[0].wImgSize	= (img0->imgInfo[0].wImgSize + 31) & ~0x1f;
		outimg.imgInfo[0].hImgSize	= img0->imgInfo[0].hImgSize;
		outimg.imgInfo[1].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[1].pImgAdrs + offsetx + offsetx2);
		outimg.imgInfo[1].gImgSize	= (img0->imgInfo[1].gImgSize + 31) & ~0x1f;
		outimg.imgInfo[1].wImgSize	= (img0->imgInfo[1].wImgSize + 15) & ~0xf;
		outimg.imgInfo[1].hImgSize	= img0->imgInfo[1].hImgSize;

		in1img.imgForm				= img1->imgForm;
		in1img.bitDepth				= img1->bitDepth;
		in1img.dataForm				= img1->dataForm;
		in1img.imgInfo[0].pImgAdrs	= (IPC_VOID*)(img1->imgInfo[0].pImgAdrs);
		in1img.imgInfo[0].gImgSize	= (img1->imgInfo[0].gImgSize + 31) & ~0x1f;
		in1img.imgInfo[0].wImgSize	= (img1->imgInfo[0].wImgSize + 31) & ~0x1f;
		in1img.imgInfo[0].hImgSize	= img1->imgInfo[0].hImgSize;
		in1img.imgInfo[1].pImgAdrs	= (IPC_VOID*)(img1->imgInfo[1].pImgAdrs);
		in1img.imgInfo[1].gImgSize	= (img1->imgInfo[1].gImgSize + 31) & ~0x1f;
		in1img.imgInfo[1].wImgSize	= (img1->imgInfo[1].wImgSize + 15) & ~0xf;
		in1img.imgInfo[1].hImgSize	= img1->imgInfo[1].hImgSize;

		IPC_BLEND_Blend(&outimg, &in0img, &in1img, &alp1Img);

		in0img.imgForm				= img2->imgForm;
		in0img.bitDepth				= img2->bitDepth;
		in0img.dataForm				= img2->dataForm;
		in0img.imgInfo[0].pImgAdrs	= (IPC_VOID*)(img2->imgInfo[0].pImgAdrs);
		in0img.imgInfo[0].gImgSize	= (img2->imgInfo[0].gImgSize + 31) & ~0x1f;
		in0img.imgInfo[0].wImgSize	= (img2->imgInfo[0].gImgSize + 31) & ~0x1f;
		in0img.imgInfo[0].hImgSize	= img2->imgInfo[0].hImgSize;
		in0img.imgInfo[1].pImgAdrs	= (IPC_VOID*)(img2->imgInfo[1].pImgAdrs);
		in0img.imgInfo[1].gImgSize	= (img2->imgInfo[0].gImgSize + 31) & ~0x1f;
		in0img.imgInfo[1].wImgSize	= (img2->imgInfo[0].gImgSize/2 + 15) & ~0xf;
		in0img.imgInfo[1].hImgSize	= img2->imgInfo[1].hImgSize;

		in1img.imgForm				= img0->imgForm;
		in1img.bitDepth				= img0->bitDepth;
		in1img.dataForm				= img0->dataForm;
		in1img.imgInfo[0].pImgAdrs	= (IPC_VOID*)(((IPC_UINT)img0->imgInfo[0].pImgAdrs + offsetx2) & ~0x1F);
		in1img.imgInfo[0].gImgSize	= img0->imgInfo[0].gImgSize;
		in1img.imgInfo[0].wImgSize	= (img2->imgInfo[0].gImgSize + 31) & ~0x1f;
		in1img.imgInfo[0].hImgSize	= img2->imgInfo[0].hImgSize;
		in1img.imgInfo[1].pImgAdrs	= (IPC_VOID*)(((IPC_UINT)img0->imgInfo[1].pImgAdrs + offsetx2) & ~0xF);
		in1img.imgInfo[1].gImgSize	= img0->imgInfo[1].gImgSize;
		in1img.imgInfo[1].wImgSize	= (img2->imgInfo[0].gImgSize/2 + 15) & ~0xf;
		in1img.imgInfo[1].hImgSize	= img2->imgInfo[1].hImgSize;

		outimg.imgForm				= img0->imgForm;
		outimg.bitDepth				= img0->bitDepth;
		outimg.dataForm				= img0->dataForm;
		outimg.imgInfo[0].pImgAdrs	= (IPC_VOID*)(((IPC_UINT)img0->imgInfo[0].pImgAdrs + offsetx2) & ~0x1F);
		outimg.imgInfo[0].gImgSize	= img0->imgInfo[0].gImgSize;
		outimg.imgInfo[0].wImgSize	= (img2->imgInfo[0].gImgSize + 31) & ~0x1f;
		outimg.imgInfo[0].hImgSize	= img2->imgInfo[0].hImgSize;
		outimg.imgInfo[1].pImgAdrs	= (IPC_VOID*)(( (IPC_UINT)img0->imgInfo[1].pImgAdrs + offsetx2) & ~0xF);
		outimg.imgInfo[1].gImgSize	= img0->imgInfo[1].gImgSize;
		outimg.imgInfo[1].wImgSize	= (img2->imgInfo[0].gImgSize/2 + 15) & ~0xf;
		outimg.imgInfo[1].hImgSize	= img2->imgInfo[1].hImgSize;

		IPC_BLEND_Blend(&outimg, &in0img, &in1img, &alp0Img);
		break;
	case D_IP_STC_MODE_VERTICAL:
		offsetx = 0;
		in0img.imgForm				= img0->imgForm;
		in0img.bitDepth				= img0->bitDepth;
		in0img.dataForm				= img0->dataForm;
		in0img.imgInfo[0].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[0].pImgAdrs + offsetx);
		in0img.imgInfo[0].gImgSize	= (img0->imgInfo[0].gImgSize + 31) & ~0x1f;
		in0img.imgInfo[0].wImgSize	= (img0->imgInfo[0].wImgSize + 31) & ~0x1f;
		in0img.imgInfo[0].hImgSize	= img0->imgInfo[0].hImgSize;
		in0img.imgInfo[1].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[1].pImgAdrs + offsetx);
		in0img.imgInfo[1].gImgSize	= (img0->imgInfo[1].gImgSize + 31) & ~0x1f;
		in0img.imgInfo[1].wImgSize	= (img0->imgInfo[1].wImgSize + 15) & ~0xf;
		in0img.imgInfo[1].hImgSize	= img0->imgInfo[1].hImgSize;

		outimg.imgForm				= img0->imgForm;
		outimg.bitDepth				= img0->bitDepth;
		outimg.dataForm				= img0->dataForm;
		outimg.imgInfo[0].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[0].pImgAdrs + offsetx);
		outimg.imgInfo[0].gImgSize	= (img0->imgInfo[0].gImgSize + 31) & ~0x1f;
		outimg.imgInfo[0].wImgSize	= (img0->imgInfo[0].wImgSize + 31) & ~0x1f;
		outimg.imgInfo[0].hImgSize	= img0->imgInfo[0].hImgSize;
		outimg.imgInfo[1].pImgAdrs	= (IPC_VOID*)(img0->imgInfo[1].pImgAdrs + offsetx);
		outimg.imgInfo[1].gImgSize	= (img0->imgInfo[1].gImgSize + 31) & ~0x1f;
		outimg.imgInfo[1].wImgSize	= (img0->imgInfo[1].wImgSize + 15) & ~0xf;
		outimg.imgInfo[1].hImgSize	= img0->imgInfo[1].hImgSize;

		in1img.imgForm				= img1->imgForm;
		in1img.bitDepth				= img1->bitDepth;
		in1img.dataForm				= img1->dataForm;
		in1img.imgInfo[0].pImgAdrs	= (IPC_VOID*)(img1->imgInfo[0].pImgAdrs);
		in1img.imgInfo[0].gImgSize	= (img1->imgInfo[0].gImgSize + 31) & ~0x1f;
		in1img.imgInfo[0].wImgSize	= (img1->imgInfo[0].wImgSize + 31) & ~0x1f;
		in1img.imgInfo[0].hImgSize	= img1->imgInfo[0].hImgSize;
		in1img.imgInfo[1].pImgAdrs	= (IPC_VOID*)(img1->imgInfo[1].pImgAdrs);
		in1img.imgInfo[1].gImgSize	= (img1->imgInfo[1].gImgSize + 31) & ~0x1f;
		in1img.imgInfo[1].wImgSize	= (img1->imgInfo[1].wImgSize + 15) & ~0xf;
		in1img.imgInfo[1].hImgSize	= img1->imgInfo[1].hImgSize;

		IPC_BLEND_Blend(&outimg, &in0img, &in1img, &alp0Img);
		break;
	default:
		break;
	}
}

#define	FAST
/******************************************************************************
 * @brief	MakeAlpha
 * @param	T_IPC_IMAGE*	alphaImage0
 * @param	T_IPC_IMAGE*	alphaImage1
 * @retval	none
 */
VOID IP_STC_MakeAlpha(
	T_IPC_IMAGE* alphaImage0,
	T_IPC_IMAGE* alphaImage1
)
{
	IPC_UCHAR	*ptr;
	IPC_INT		x,y,dx,dy,ddx,ddy;
	IPC_INT		width,width2,height;
	IPC_INT		start;
	IPC_INT		ofs;

	switch( gIpStcMode ){
	case D_IP_STC_MODE_HORIZONTAL:
	case D_IP_STC_MODE_HORIZONTAL_CENTER:
		memcpy( &alp0Img, alphaImage0, sizeof(T_IPC_IMAGE) );
		memcpy( &alp1Img, alphaImage1, sizeof(T_IPC_IMAGE) );

		alp0Img.imgInfo[0].gImgSize  = (alphaImage0->imgInfo[0].gImgSize + 31) & ~0x1f;
		alp0Img.imgInfo[0].wImgSize  = (alphaImage0->imgInfo[0].wImgSize + 31) & ~0x1f;
		alp1Img.imgInfo[0].gImgSize  = (alphaImage1->imgInfo[0].gImgSize + 31) & ~0x1f;
		alp1Img.imgInfo[0].wImgSize  = (alphaImage1->imgInfo[0].wImgSize + 31) & ~0x1f;

		// left alpha
		ptr		= (IPC_UCHAR *)alphaImage0->imgInfo[0].pImgAdrs;
		width2	= (alphaImage0->imgInfo[0].gImgSize + 31) & ~0x1f;
		width	=  alphaImage0->imgInfo[0].wImgSize;
		start	= (alphaImage0->imgInfo[0].wImgSize & 0x1f) ;
		height	=  alphaImage0->imgInfo[0].hImgSize;
		ddx = 256*256 / width;
		ofs = gIpLofs & 0x1F;

		alp0Img.imgInfo[0].wImgSize  = alp0Img.imgInfo[0].gImgSize;
		alp1Img.imgInfo[0].wImgSize  = alp1Img.imgInfo[0].gImgSize;

#ifdef FAST
		for(y=0;y<height/2;y++){
#else
		for(y=0;y<height;y++){
#endif
			dx = 0;
			for(x=0;x<width2;x++){
				*(ptr+(y*width2)+x) = (dx >> 8);
				if(x>=(ofs-1)){
					dx += ddx;
				}
				if(dx>0xff00)dx=0xff00;
			}
		}
		BF_L1l2cache_Flush_Addr( (UINT32)alphaImage0->imgInfo[0].pImgAdrs, width2*height/2 );
#ifdef FAST
		Im_Xch_Copy( E_IM_XCH_CH_SEL_1, (UINT32)ptr, (USHORT)width2, width2, (UINT32)ptr+width2*height/2, width2, height/2);
#endif
		// right alpha
		ptr		= (IPC_UCHAR *)alphaImage1->imgInfo[0].pImgAdrs;
		width2	= (alphaImage1->imgInfo[0].gImgSize + 31) & ~0x1f;
		width	= (alphaImage1->imgInfo[0].wImgSize);
		height	=  alphaImage1->imgInfo[0].hImgSize;
		ddx = 256*256 / width;

		start = gIpOffset & 0x1f;
#ifdef FAST
		for(y=0;y<height/2;y++){
#else
		for(y=0;y<height;y++){
#endif
			dx = 0;
			for(x=start;x<width2;x++){
				*(ptr+(y*width2)+x) = (dx >> 8);
				if(x>=(start-1)){
					dx += ddx;
				}
				if(dx>0xff00)dx=0xff00;
			}
		}
		BF_L1l2cache_Flush_Addr( (UINT32)alphaImage1->imgInfo[0].pImgAdrs, width2*height/2 );
#ifdef FAST
		Im_Xch_Copy( E_IM_XCH_CH_SEL_1, (UINT32)ptr, (USHORT)width2, width2, (UINT32)ptr+width2*height/2, width2, height/2);
#endif
		break;
	case D_IP_STC_MODE_VERTICAL:
		memcpy( &alp0Img, alphaImage0, sizeof(T_IPC_IMAGE) );

		alp0Img.imgInfo[0].gImgSize  = (alphaImage0->imgInfo[0].gImgSize + 31) & ~0x1f;
		alp0Img.imgInfo[0].wImgSize  = (alphaImage0->imgInfo[0].wImgSize + 31) & ~0x1f;

		// left alpha
		ptr		= (IPC_UCHAR *)alphaImage0->imgInfo[0].pImgAdrs;
		width	= (alphaImage0->imgInfo[0].wImgSize + 31) & ~0x1f;
		height	=  alphaImage0->imgInfo[0].hImgSize;
		ddy = 256*256 / height;

#ifdef FAST
		for(x=0;x<width/2;x++){
#else
		for(x=0;x<width;x++){
#endif
			dy = 0;
			for(y=0;y<height;y++){
				*(ptr+(y*width)+x) = (dy >> 8);
				dy += ddy;
				if(dy>0xff00)dy=0xff00;
			}
		}
#ifdef FAST
		BF_L1l2cache_Flush_Addr( (UINT32)ptr, width * height );
		Im_Xch_Copy( E_IM_XCH_CH_SEL_1, (UINT32)ptr, (USHORT)width, width, (UINT32)ptr+width/2, width/2, height);
#endif
		break;
	}
}

/******************************************************************************
 * @brief	MakeParameter
 * @param	in	IPC_INT			imgSizeX
 * @param	in	IPC_INT			imgSizeY
 * @param	in	IPC_INT			stitch0L
 * @param	in	IPC_INT			stitch1L
 * @param	in	IPC_INT			Fimage-offset
 * @param	out	IPC_INT*		offset
 * @param	out	IPC_INT*		alphaRwidth
 * @param	out	IPC_INT*		alphaLwidth
 * @retval	none
 */
VOID IP_STC_MakeParameter(
	IPC_INT	imgSizeX,
	IPC_INT	imgSizeY,
	IPC_INT	stitch0L,
	IPC_INT	stitch1L,
	IPC_INT	Foffset,
	IPC_INT	*offset,
	IPC_INT	*alphaRwidth,
	IPC_INT	*alphaLwidth
)
{
//	IPC_INT	stitch0R;
//	IPC_INT	stitch1R;
//	IPC_INT	ofs;

	*offset = imgSizeX - stitch1L;
	gIpAwidth = ((Foffset & 0x1F) + stitch0L + 0x1F) & ~0x1F;	// 20170621
	gIpLofs = Foffset ;
	*alphaLwidth = gIpAwidth;
//	*alphaLwidth = ((Foffset & 0x1F) + stitch0L) & ~0x1F;
	*alphaRwidth = (((Foffset + imgSizeX - stitch1L) & 0x1F) + stitch1L + 0x1F) & ~0x1F;
}

/******************************************************************************
 * @brief	Backup
 * @param	in	UINT32			src_adr
 * @param	in	USHORT			src_gw
 * @param	in	USHORT			dst_gw
 * @param	in	UINT32			dst_adr
 * @param	in	USHORT			cpy_width
 * @param	in	USHORT			cpy_height
 * @retval	none
 */
VOID IP_STC_Backup(
	T_IPC_IMAGE*	dstImage,
	T_IPC_IMAGE*	srcImage,
	IPC_INT			offset
)
{
	UINT32		src_adr;
	USHORT		src_gw;
	USHORT		dst_gw;
	UINT32		dst_adr;
	USHORT		cpy_width;
	USHORT		cpy_height;
	UINT32		offset32;

	switch( gIpStcMode ){
	case D_IP_STC_MODE_HORIZONTAL:
	case D_IP_STC_MODE_HORIZONTAL_CENTER:
		offset32 = offset & ~0x1f;
		src_adr		= ((UINT32)(srcImage->imgInfo[0].pImgAdrs) & ~0x1F) + offset32;
		src_gw		= srcImage->imgInfo[0].gImgSize;
		dst_adr		= (UINT32)dstImage->imgInfo[0].pImgAdrs;
		dst_gw		= (dstImage->imgInfo[0].gImgSize + 31) & ~0x1f;
		cpy_width	= (dstImage->imgInfo[0].wImgSize + 31) & ~0x1f;
		cpy_height	= (dstImage->imgInfo[0].hImgSize);

		Im_Xch_Copy( E_IM_XCH_CH_SEL_1, src_adr, src_gw, dst_gw, dst_adr, cpy_width, cpy_height);

		src_adr		= ((UINT32)(srcImage->imgInfo[1].pImgAdrs) & ~0x1F) + offset32;
		src_gw		= srcImage->imgInfo[1].gImgSize;
		dst_adr		= (UINT32)dstImage->imgInfo[1].pImgAdrs;
		dst_gw		= (dstImage->imgInfo[1].gImgSize + 31) & ~0x1f;
		cpy_width	= (dstImage->imgInfo[1].wImgSize * 2 + 31) & ~0x1f;
		cpy_height	= (dstImage->imgInfo[1].hImgSize);

		Im_Xch_Copy( E_IM_XCH_CH_SEL_1, src_adr, src_gw, dst_gw, dst_adr, cpy_width, cpy_height);
		break;
	case D_IP_STC_MODE_VERTICAL:
		src_adr		= (UINT32)srcImage->imgInfo[0].pImgAdrs;
		src_gw		= srcImage->imgInfo[0].gImgSize;
		dst_adr		= (UINT32)dstImage->imgInfo[0].pImgAdrs;
		dst_gw		= (dstImage->imgInfo[0].gImgSize + 31) & ~0x1f;
		cpy_width	= (dstImage->imgInfo[0].wImgSize + 31) & ~0x1f;
		cpy_height	= (dstImage->imgInfo[0].hImgSize);

		Im_Xch_Copy( E_IM_XCH_CH_SEL_1, src_adr, src_gw, dst_gw, dst_adr, cpy_width, cpy_height);

		src_adr		= (UINT32)srcImage->imgInfo[1].pImgAdrs;
		src_gw		= srcImage->imgInfo[1].gImgSize;
		dst_adr		= (UINT32)dstImage->imgInfo[1].pImgAdrs;
		dst_gw		= (dstImage->imgInfo[1].gImgSize + 31) & ~0x1f;
		cpy_width	= (dstImage->imgInfo[1].wImgSize + 31) & ~0x1f;
		cpy_height	= (dstImage->imgInfo[1].hImgSize);

		Im_Xch_Copy( E_IM_XCH_CH_SEL_1, src_adr, src_gw, dst_gw, dst_adr, cpy_width, cpy_height);
		break;
	}
}

