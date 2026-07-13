/**
 * @file		ip_stc.h
 * @brief		API for stitching
 * @author		Hattori
 * @since		2016/12/05
 * @date		2017/11/10
 * @date		2017/11/14
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT Inc. 2016</I></B>
 */

#ifndef _IP_STC_H_
#define _IP_STC_H_

// ***************************************************************************
//                          Include
// ***************************************************************************
#include "ddim_typedef.h"
#include "ipc_iip.h"

// ***************************************************************************
//                          Define       			                          
// ***************************************************************************
#define		D_IP_STC_MODE_HORIZONTAL		0
#define		D_IP_STC_MODE_HORIZONTAL_CENTER	1
#define		D_IP_STC_MODE_VERTICAL			2

// ***************************************************************************
//                        Structure
// ***************************************************************************

// ***************************************************************************
//                        Global Function
// ***************************************************************************
extern	VOID IP_STC_Init(
	IPC_UINT mode
);

extern	VOID	IP_STC_Stitch(
	T_IPC_IMAGE *img0,
	T_IPC_IMAGE *img1,
	T_IPC_IMAGE *img2,
	IPC_INT offsetx,
	IPC_INT offsetx2
);

extern	VOID IP_STC_MakeAlpha(
	T_IPC_IMAGE* alphaImage0,
	T_IPC_IMAGE* alphaImage1
);

extern	VOID IP_STC_MakeParameter(
	IPC_INT	imgSizeX,
	IPC_INT	imgSizeY,
	IPC_INT	stitch0L,
	IPC_INT	stitch1L,
	IPC_INT	Foffset,
	IPC_INT	*offset,
	IPC_INT	*alphaRwidth,
	IPC_INT	*alphaLwidth
);

extern	VOID	IP_STC_Backup(
	T_IPC_IMAGE*	dstImage,
	T_IPC_IMAGE*	srcImage,
	IPC_INT			offset
);

#endif	// _IP_STC_H_
