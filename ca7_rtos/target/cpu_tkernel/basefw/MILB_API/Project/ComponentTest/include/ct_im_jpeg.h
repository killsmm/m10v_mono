/**
 * @file		ct_im_jpeg.h
 * @brief		JPEG ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef _CT_IM_JPEG_H_
#define _CT_IM_JPEG_H_

#include "driver_common.h"
#include "im_jpeg.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_CT_JPEG_YCC_ADDR				(0xB8000000)	// YCC address (edit to Y address)
#define D_CT_JPEG_CODE_ADDR				(0xB0000000)	// JPEG code address
#define D_CT_JPEG_CODE_ADDR2			(0xB4000000)	// JPEG code address
//#define D_CT_JPEG_YCC_ADDR				(0x49000000)	// YCC address (edit to Y address)
//#define D_CT_JPEG_CODE_ADDR				(0x51000000)	// JPEG code address
//#define D_CT_JPEG_CODE_ADDR2			(0x53000000)	// JPEG code address



#define D_CT_JPEG_YCC_ADDR_ERR			(0xC1000000)	// YCC address (edit to Y address)
#define D_CT_JPEG_CODE_ADDR_ERR			(0xC3000000)	// JPEG code address
//#define D_CT_JPEG_YCC_ADDR_ERR			(0x69000000)	// YCC address (edit to Y address)
//#define D_CT_JPEG_CODE_ADDR_ERR			(0x71000000)	// JPEG code address


#define D_CT_JPEG_SIZE_VGA_H			(640)
#define D_CT_JPEG_SIZE_VGA_V			(480)
#define D_CT_JPEG_CODE_SIZE_VGA			(0x0001733B)	// (edit with test JPEG file)
#define D_CT_JPEG_SIZE_VGA_LIMIT		(0)

#define D_CT_JPEG_SIZE_3M_H				(2048)
#define D_CT_JPEG_SIZE_3M_V				(1536)
#define D_CT_JPEG_CODE_SIZE_3M			(0x197A90)		// (edit with test JPEG file)
#define D_CT_JPEG_SIZE_3M_LIMIT			(0)

#define D_CT_JPEG_SIZE_12M_H			(4000)
#define D_CT_JPEG_SIZE_12M_V			(3000)
#define D_CT_JPEG_CODE_SIZE_12M			(0x4632A0)		// (edit with test JPEG file)
#define D_CT_JPEG_CODE_SIZE_12M_GRAY	(0x2FA350)	// (edit with test JPEG file)
#define D_CT_JPEG_SIZE_12M_LIMIT		(0)

#define D_CT_JPEG_SIZE_16M_H			(4608)
#define D_CT_JPEG_SIZE_16M_V			(3456)
#define D_CT_JPEG_CODE_SIZE_16M			(0x99DE60)		// (edit with test JPEG file)
#define D_CT_JPEG_CODE_SIZE_16M_GRAY	(0x2FA350)	// (edit with test JPEG file)
#define D_CT_JPEG_SIZE_16M_LIMIT		(0)


#define D_CT_JPEG_LIMIT_MAX				(0)
#define D_CT_JPEG_LIMIT_SIZE			(D_CT_JPEG_SIZE_16M_H * D_CT_JPEG_SIZE_16M_V)

#define D_CT_JPEG_TEST_SIZE_H			D_CT_JPEG_SIZE_16M_H
#define D_CT_JPEG_TEST_SIZE_V			D_CT_JPEG_SIZE_16M_V
#define D_CT_JPEG_TEST_CODE				D_CT_JPEG_CODE_SIZE_16M
#define D_CT_JPEG_TEST_CODE_GRAY		D_CT_JPEG_CODE_SIZE_16M_GRAY
#define D_CT_JPEG_TEST_SIZE_LIMIT		D_CT_JPEG_SIZE_16M_LIMIT


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
extern VOID		Ct_Im_Jpeg_Run_1( INT32 ct_param_1, INT32 ct_param_2 );
extern VOID		Ct_Im_Jpeg_Run_2( INT32 ct_param_1, INT32 ct_param_2 );
extern VOID		Ct_Im_Jpeg_Main( INT32 argc, CHAR** argv );
extern ULONG	Ct_Im_Jpeg_Get_Enc_Size( VOID );
extern ULONG	Ct_Im_Jpeg_Get_OutputBytes( VOID );

#endif	// _CT_IM_JPEG_H_
