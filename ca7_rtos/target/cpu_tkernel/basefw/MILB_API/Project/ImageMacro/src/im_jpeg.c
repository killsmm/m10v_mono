/**
 * @file		im_jpeg.h
 * @brief		Jpeg macro driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include "im_jpeg.h"
#include "dd_top.h"
#include "ddim_user_custom.h"
#include "jdsjp7.h"
#include "jdsimg.h"
#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// for debug
//#define CO_IM_JPEG_DEBUG

#ifdef CO_ACT_CLOCK
#define CO_ACT_JPEG_CLOCK
#endif //CO_ACT_CLOCK
#ifdef CO_ACT_HCLOCK
#define CO_ACT_JPEG_HCLOCK
#endif //CO_ACT_HCLOCK
#ifdef CO_ACT_ICLOCK
#define CO_ACT_JPEG_ICLOCK
#endif //CO_ACT_ICLOCK

#if 1	// Clock control force ON.
#ifndef CO_ACT_CLOCK
#define CO_ACT_JPEG_CLOCK
#endif //CO_ACT_CLOCK
#ifndef CO_ACT_HCLOCK
#define CO_ACT_JPEG_HCLOCK
#endif //CO_ACT_HCLOCK
#ifndef CO_ACT_ICLOCK
#define CO_ACT_JPEG_ICLOCK
#endif //CO_ACT_ICLOCK
#endif

#define D_IM_JPEG_CLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP3())
#define D_IM_JPEG_HCLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP3())
#define D_IM_JPEG_ICLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP3())

#define D_IM_JPEG_CLK_BIT			D_DD_TOP_JPGCK_BIT

#define D_IM_JPEG_HCLK_BIT			D_DD_TOP_JPGAH_BIT

#define D_IM_JPEG_ICLK_BIT			D_DD_TOP_JPGAX_BIT

#define D_IM_JPEG_SECT_CNT			512					// 1sector= 512byte

// difinition macro
#define Im_JPEG_Limit_Min(A, B)		(((A) > (B)) ? (A) : (B))								// < A = A >=  B
#define Im_JPEG_Limit_Max(A, B)		(((A) < (B)) ? (A) : (B))								// < A = A <=  B
#define Im_JPEG_Limit(A, B, C)		(Im_JPEG_Limit_Max(Im_JPEG_Limit_Min((A), (B)), (C)))	// < A = B <= A <= C

#define Im_JPEG_Dsb()				Dd_ARM_Dsb_Pou()

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// quantization table for the next frame
static T_IM_JPEG_QUAT_TBL_PACK	gIm_Jpeg_Next_Frame_Quant_Tbl;
static volatile	UCHAR			gIm_Jpeg_Update_Quant_Flg = 0;

static T_IM_JPEG_QUAT_TBL		gIm_Jpeg_Quant_Tbl_0;
static T_IM_JPEG_QUAT_TBL		gIm_Jpeg_Quant_Tbl_1;
static T_IM_JPEG_QUAT_TBL		gIm_Jpeg_Quant_Tbl_2;
static T_IM_JPEG_QUAT_TBL		gIm_Jpeg_Quant_Tbl_3;

// counter
static volatile	USHORT			gIm_Jpeg_Line_Cnt = 0;
static volatile	ULONG			gIm_Jpeg_Sect_Cnt = 0;

// encode setting table
static T_IM_JPEG_ENC_MNG		gIm_Jpeg_Enc_Mng;
static T_IM_JPEG_ENC_FRAME_MNG	gIm_Jpeg_Enc_Frame_Mng;

// decode setting table
static T_IM_JPEG_DEC_MNG		gIm_Jpeg_Dec_Mng;
static T_IM_JPEG_DEC_FRAME_MNG	gIm_Jpeg_Dec_Frame_Mng;

// Clock control counter (0~255)
#ifdef CO_ACT_JPEG_CLOCK
volatile UCHAR gIM_Jpg_Clk_Ctrl_Cnt = 0;
#endif // CO_ACT_JPEG_CLOCK
#ifdef CO_ACT_JPEG_HCLOCK
volatile UCHAR gIM_Jpg_Hclk_Ctrl_Cnt = 0;
#endif // CO_ACT_JPEG_HCLOCK
#ifdef CO_ACT_JPEG_ICLOCK
volatile UCHAR gIM_Jpg_Iclk_Ctrl_Cnt = 0;
#endif // CO_ACT_JPEG_ICLOCK

static INT32					gIm_Jpeg_Result_Judge=0;

// Spin Lock
static ULONG gIm_Jpg_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64))) = 0;

/********************************************************/
/* Default Quantization Table							*/
/********************************************************/
// for Y
static const T_IM_JPEG_QUAT_TBL gDefault_Quantize_Tbl_Luma = {
	{	0x10, 0x0B, 0x0A, 0x10, 0x18, 0x28, 0x33, 0x3D,
		0x0C, 0x0C, 0x0E, 0x13, 0x1A, 0x3A, 0x3C, 0x37,
		0x0E, 0x0D, 0x10, 0x18, 0x28, 0x39, 0x45, 0x38,
		0x0E, 0x11, 0x16, 0x1D, 0x33, 0x57, 0x50, 0x3E,
		0x12, 0x16, 0x25, 0x38, 0x44, 0x6D, 0x67, 0x4D,
		0x18, 0x23, 0x37, 0x40, 0x51, 0x68, 0x71, 0x5C,
		0x31, 0x40, 0x4E, 0x57, 0x67, 0x79, 0x78, 0x65,
		0x48, 0x5C, 0x5F, 0x62, 0x70, 0x64, 0x67, 0x63	}
};

// for Cb/Cr
static const T_IM_JPEG_QUAT_TBL gDefault_Quantize_Tbl_Chroma = {
	{	0x11, 0x12, 0x18, 0x2F, 0x63, 0x63, 0x63, 0x63,
		0x12, 0x15, 0x1A, 0x42, 0x63, 0x63, 0x63, 0x63,
		0x18, 0x1A, 0x38, 0x63, 0x63, 0x63, 0x63, 0x63,
		0x2F, 0x42, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
		0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
		0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
		0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
		0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63	}
};

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static VOID im_jpeg_on_clk( VOID );
static VOID im_jpeg_off_clk( VOID );
static VOID im_jpeg_on_hclk( VOID );
static VOID im_jpeg_off_hclk( VOID );
static VOID im_jpeg_on_iclk( VOID );
static VOID im_jpeg_off_iclk( VOID );
static VOID im_jpeg_set_quant_tbl( T_IM_JPEG_QUAT_TBL_PACK* p_quant_tbl );

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief		the Jpeg clock on.
 * @param[in]	None
 * @param[out]	None
 * @return		None
 */
static VOID im_jpeg_on_clk( VOID )
{
#ifdef CO_ACT_JPEG_CLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_Jpg_Clk_Ctrl_Cnt, D_IM_JPEG_CLK_REG_ADDR, ~D_IM_JPEG_CLK_BIT );
#endif	// CO_ACT_JPEG_CLOCK
}

/**
 * @brief		the Jpeg clock off.
 * @param[in]	None
 * @param[out]	None
 * @return		None
 */
static VOID im_jpeg_off_clk( VOID )
{
#ifdef CO_ACT_JPEG_CLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_Jpg_Clk_Ctrl_Cnt, D_IM_JPEG_CLK_REG_ADDR, D_IM_JPEG_CLK_BIT );
#endif	// CO_ACT_JPEG_CLOCK
}

/**
 * @brief		the AHB bus clock off.
 * @param[in]	None
 * @param[out]	None
 * @return		None
 */
static VOID im_jpeg_on_hclk( VOID )
{
#ifdef CO_ACT_JPEG_HCLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_Jpg_Hclk_Ctrl_Cnt, D_IM_JPEG_HCLK_REG_ADDR, ~D_IM_JPEG_HCLK_BIT );
#endif	// CO_ACT_JPEG_HCLOCK
}

/**
 * @brief		the AHB bus clock on.
 * @param[in]	None
 * @param[out]	None
 * @return		None
 */
static VOID im_jpeg_off_hclk( VOID )
{
#ifdef CO_ACT_JPEG_HCLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_Jpg_Hclk_Ctrl_Cnt, D_IM_JPEG_HCLK_REG_ADDR, D_IM_JPEG_HCLK_BIT );
#endif	// CO_ACT_JPEG_HCLOCK
}

/**
 * @brief  the AXI bus clock on.
 * @param[in]	None
 * @param[out]	None
 * @return None
 */
static VOID im_jpeg_on_iclk( VOID )
{
#ifdef CO_ACT_JPEG_ICLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_Jpg_Iclk_Ctrl_Cnt, D_IM_JPEG_ICLK_REG_ADDR, ~D_IM_JPEG_ICLK_BIT );
#endif	// CO_ACT_JPEG_ICLOCK
}

/**
 * @brief  the AXI bus clock off.
 * @param[in]	None
 * @param[out]	None
 * @return None
 */
static VOID im_jpeg_off_iclk( VOID )
{
#ifdef CO_ACT_JPEG_ICLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_Jpg_Iclk_Ctrl_Cnt, D_IM_JPEG_ICLK_REG_ADDR, D_IM_JPEG_ICLK_BIT );
#endif	// CO_ACT_JPEG_ICLOCK
}

/**
 * @brief  Jpeg macro reset for stop.
 * @param[in]	None
 * @param[out]	None
 * @return None
 */
static VOID im_jpeg_reset( VOID )
{
	// trriger off
	IO_JPG7.JICTL.bit.JITRG = 0;
	IO_JPG7.JCCTL.bit.JCTRG = 0;

	// reset
	IO_JPG7.JPCMD.bit.JRESET = 1;

	Im_JPEG_Dsb();

#ifdef CO_IM_JPEG_DEBUG
	Ddim_Print(("IO_JPG7.JSTATE.bit.JALLRSTP =[0x%X]\n", IO_JPG7.JSTATE.bit.JALLRSTP));
#endif

	while ( IO_JPG7.JSTATE.bit.JALLRSTP == 1 ) {
		;	// during reset
#ifdef CO_DEBUG_ON_PC
		IO_JPG7.JSTATE.bit.JALLRSTP = 0;
#endif
	}
}

/**
 * @brief  Set JPEG Quantization table.
 * @param  T_IM_JPEG_QUAT_TBL_PACK* p_quant_tbl :Pointer to Quantization Table
 * @return NONE
 */
static VOID im_jpeg_set_quant_tbl( T_IM_JPEG_QUAT_TBL_PACK* p_quant_tbl )
{
	INT32 i, index;
	im_jpeg_on_hclk();	// HCLK on

	IO_IMG.SRAMPD.bit.JPEGPD &= ~0x8;	// QTTBL SRAM power down off.

	Im_JPEG_Dsb();

	// QTTBL SRAM power down control wait time.
	Dd_ARM_Wait_ns( 1000 );

	if ( p_quant_tbl == NULL ) {
		// Quantization table No.0 for Y
		for ( i = 0, index = 0; i < 16; i++ ) {
			IO_JPG7.JPQT0.word[i] = ((gDefault_Quantize_Tbl_Luma.quant_value[index] << 24) | (gDefault_Quantize_Tbl_Luma.quant_value[index+1] << 16) | (gDefault_Quantize_Tbl_Luma.quant_value[index+2] << 8) | (gDefault_Quantize_Tbl_Luma.quant_value[index+3]));
			index = index + 4;
		}
		// Quantization table No.1 for CbCr
		for ( i = 0, index = 0; i < 16; i++ ) {
			IO_JPG7.JPQT1.word[i] = ((gDefault_Quantize_Tbl_Chroma.quant_value[index] << 24) | (gDefault_Quantize_Tbl_Chroma.quant_value[index+1] << 16) | (gDefault_Quantize_Tbl_Chroma.quant_value[index+2] << 8) | (gDefault_Quantize_Tbl_Chroma.quant_value[index+3]));
			index = index + 4;
		}
		// Quantization table No.2 for Y
		for ( i = 0, index = 0; i < 16; i++ ) {
			IO_JPG7.JPQT2.word[i] = ((gDefault_Quantize_Tbl_Luma.quant_value[index] << 24) | (gDefault_Quantize_Tbl_Luma.quant_value[index+1] << 16) | (gDefault_Quantize_Tbl_Luma.quant_value[index+2] << 8) | (gDefault_Quantize_Tbl_Luma.quant_value[index+3]));
			index = index + 4;
		}
		// Quantization table No.3 for CbCr
		for ( i = 0, index = 0; i < 16; i++ ) {
			IO_JPG7.JPQT3.word[i] = ((gDefault_Quantize_Tbl_Chroma.quant_value[index] << 24) | (gDefault_Quantize_Tbl_Chroma.quant_value[index+1] << 16) | (gDefault_Quantize_Tbl_Chroma.quant_value[index+2] << 8) | (gDefault_Quantize_Tbl_Chroma.quant_value[index+3]));
			index = index + 4;
		}
	}
	else {

		if ( p_quant_tbl->quantization_tbl_0 != NULL ) {
			for ( i = 0, index = 0; i < 16; i++ ) {
				IO_JPG7.JPQT0.word[i] = ((p_quant_tbl->quantization_tbl_0->quant_value[index] << 24) | (p_quant_tbl->quantization_tbl_0->quant_value[index+1] << 16) | (p_quant_tbl->quantization_tbl_0->quant_value[index+2] << 8) | (p_quant_tbl->quantization_tbl_0->quant_value[index+3]));
				index = index + 4;
			}
		}
		else {
			// Quantization table No.0 for Y
			for ( i = 0, index = 0; i < 16; i++ ) {
				IO_JPG7.JPQT0.word[i] = ((gDefault_Quantize_Tbl_Luma.quant_value[index] << 24) | (gDefault_Quantize_Tbl_Luma.quant_value[index+1] << 16) | (gDefault_Quantize_Tbl_Luma.quant_value[index+2] << 8) | (gDefault_Quantize_Tbl_Luma.quant_value[index+3]));
				index = index + 4;
			}
		}

		if ( p_quant_tbl->quantization_tbl_1 != NULL ) {
			for ( i = 0, index = 0; i < 16; i++ ) {
				IO_JPG7.JPQT1.word[i] = ((p_quant_tbl->quantization_tbl_1->quant_value[index] << 24) | (p_quant_tbl->quantization_tbl_1->quant_value[index+1] << 16) | (p_quant_tbl->quantization_tbl_1->quant_value[index+2] << 8) | (p_quant_tbl->quantization_tbl_1->quant_value[index+3]));
				index = index + 4;
			}
		}
		else {
			// Quantization table No.1 for CbCr
			for ( i = 0, index = 0; i < 16; i++ ) {
				IO_JPG7.JPQT1.word[i] = ((gDefault_Quantize_Tbl_Chroma.quant_value[index] << 24) | (gDefault_Quantize_Tbl_Chroma.quant_value[index+1] << 16) | (gDefault_Quantize_Tbl_Chroma.quant_value[index+2] << 8) | (gDefault_Quantize_Tbl_Chroma.quant_value[index+3]));
				index = index + 4;
			}
		}

		if ( p_quant_tbl->quantization_tbl_2 != NULL ) {
			for ( i = 0, index = 0; i < 16; i++ ) {
				IO_JPG7.JPQT2.word[i] = ((p_quant_tbl->quantization_tbl_2->quant_value[index] << 24) | (p_quant_tbl->quantization_tbl_2->quant_value[index+1] << 16) | (p_quant_tbl->quantization_tbl_2->quant_value[index+2] << 8) | (p_quant_tbl->quantization_tbl_2->quant_value[index+3]));
				index = index + 4;
			}
		}
		else {
			// Quantization table No.2 for Y
			for ( i = 0, index = 0; i < 16; i++ ) {
				IO_JPG7.JPQT2.word[i] = ((gDefault_Quantize_Tbl_Luma.quant_value[index] << 24) | (gDefault_Quantize_Tbl_Luma.quant_value[index+1] << 16) | (gDefault_Quantize_Tbl_Luma.quant_value[index+2] << 8) | (gDefault_Quantize_Tbl_Luma.quant_value[index+3]));
				index = index + 4;
			}
		}

		if ( p_quant_tbl->quantization_tbl_3 != NULL ) {
			for ( i = 0, index = 0; i < 16; i++ ) {
				IO_JPG7.JPQT3.word[i] = ((p_quant_tbl->quantization_tbl_3->quant_value[index] << 24) | (p_quant_tbl->quantization_tbl_3->quant_value[index+1] << 16) | (p_quant_tbl->quantization_tbl_3->quant_value[index+2] << 8) | (p_quant_tbl->quantization_tbl_3->quant_value[index+3]));
				index = index + 4;
			}
		}
		else {
			// Quantization table No.3 for CbCr
			for ( i = 0, index = 0; i < 16; i++ ) {
				IO_JPG7.JPQT3.word[i] = ((gDefault_Quantize_Tbl_Chroma.quant_value[index] << 24) | (gDefault_Quantize_Tbl_Chroma.quant_value[index+1] << 16) | (gDefault_Quantize_Tbl_Chroma.quant_value[index+2] << 8) | (gDefault_Quantize_Tbl_Chroma.quant_value[index+3]));
				index = index + 4;
			}
		}
	}

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	return;
}

/**
 * @brief  Set JPEG Next Quantization table.
 * @param  T_IM_JPEG_QUAT_TBL_PACK* p_quant_tbl :Pointer to Next Quantization Table
 * @return NONE
 */
static VOID im_jpeg_set_next_quant_tbl( T_IM_JPEG_QUAT_TBL_PACK* p_quant_tbl )
{
	if ( p_quant_tbl == NULL ) {
	// Set default table
		gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_0 = (T_IM_JPEG_QUAT_TBL*)&gDefault_Quantize_Tbl_Luma;
		gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_1 = (T_IM_JPEG_QUAT_TBL*)&gDefault_Quantize_Tbl_Chroma;
		gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_2 = (T_IM_JPEG_QUAT_TBL*)&gDefault_Quantize_Tbl_Luma;
		gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_3 = (T_IM_JPEG_QUAT_TBL*)&gDefault_Quantize_Tbl_Chroma;
	}
	else {
		if ( p_quant_tbl->quantization_tbl_0 != NULL ) {
			gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_0 = p_quant_tbl->quantization_tbl_0;
		}
		else {
			gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_0 = (T_IM_JPEG_QUAT_TBL*)&gDefault_Quantize_Tbl_Luma;
		}
		if ( p_quant_tbl->quantization_tbl_1 != NULL ) {
			gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_1 = p_quant_tbl->quantization_tbl_1;
		}
		else {
			gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_1 = (T_IM_JPEG_QUAT_TBL*)&gDefault_Quantize_Tbl_Chroma;
		}
		if ( p_quant_tbl->quantization_tbl_2 != NULL ) {
			gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_2 = p_quant_tbl->quantization_tbl_2;
		}
		else {
			gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_2 = (T_IM_JPEG_QUAT_TBL*)&gDefault_Quantize_Tbl_Luma;
		}
		if ( p_quant_tbl->quantization_tbl_3 != NULL ) {
			gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_3 = p_quant_tbl->quantization_tbl_3;
		}
		else {
			gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_3 = (T_IM_JPEG_QUAT_TBL*)&gDefault_Quantize_Tbl_Chroma;
		}
	}

#ifdef CO_IM_JPEG_DEBUG
	UCHAR i;
	for ( i=0; i < 64; i++ ) {
		Ddim_Print(("gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_0.quant_value[%d]=0x%X\n", i, gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_0->quant_value[i]));
	}
	for ( i=0; i < 64; i++ ) {
		Ddim_Print(("gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_1.quant_value[%d]=0x%X\n", i, gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_1->quant_value[i]));
	}
	for ( i=0; i < 64; i++ ) {
		Ddim_Print(("gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_2.quant_value[%d]=0x%X\n", i, gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_2->quant_value[i]));
	}
	for ( i=0; i < 64; i++ ) {
		Ddim_Print(("gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_3.quant_value[%d]=0x%X\n", i, gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_3->quant_value[i]));
	}
#endif

	return;
}

/**
 * @brief  interrupt handler for QTWINT.
 * @param[in]	None
 * @param[out]	INT32* result
 * @return None
 */
static VOID im_jpeg_sub_int_handler_enc_core_qtwint( INT32* result )
{
	// Check update enable
	if ( IO_JPG7.JPINTSTA.bit.QTWINT == 1 ) {
		if (gIm_Jpeg_Update_Quant_Flg) {
			// Set quantize table
			im_jpeg_set_quant_tbl( &gIm_Jpeg_Next_Frame_Quant_Tbl );
			// Flag clear
			gIm_Jpeg_Update_Quant_Flg = 0;
		}
		// QTWINT bit clear
		IO_JPG7.JPINTSTA.bit.QTWINT = 0;

		// Set result
		*result |= D_IM_JPEG_QTWINT;

	}

	return;
}

/**
 * @brief  Encode normal end handler.
 * @param[in]	None
 * @param[out]	INT32* result
 * @return None
 */
static VOID im_jpeg_sub_int_handler_enc_core_end( INT32* result )
{
	// Normal end
	if ( IO_JPG7.JPINTSTA.bit.INTFLG == 1 ) {
		// INTFLG bit clear
		IO_JPG7.JPINTSTA.bit.INTFLG = 0;
	}

	// Write code size
	gIm_Jpeg_Enc_Mng.code_size = IO_JPG7.JPBYTCNT.word;	// Count value of compression volume of data

	// Set result
	*result |= D_IM_JPEG_NML_END;
	gIm_Jpeg_Result_Judge |= D_IM_JPEG_NML_END;

	return;
}

/**
 * @brief  Decode err handler.
 * @param[in]	None
 * @param[out]	INT32* result
 * @return None
 */
static VOID im_jpeg_sub_int_handler_dec_core_errint( INT32* result )
{
	INT32 res = 0;

	// get error code
	gIm_Jpeg_Dec_Mng.err_code = IO_JPG7.JPERRCOD.bit.JPERRCOD;

	if ( IO_JPG7.JPINTSTA.bit.ERRFLG != 0 ) {
		res |= D_IM_JPEG_ERR;
	}

	// Status clear
	IO_JPG7.JPINTSTA.word = 0x00000000;

	// Set result
	*result |= res;

	// Error reset
	im_jpeg_reset();

	return;
}

/**
 * @brief  interrupt handler for REGRDINT.
 * @param[in]	None
 * @param[out]	INT32* result
 * @return	INT32	D_IM_JPEG_OK / D_IM_JPEG_DECODE_ERR
 */
static INT32 im_jpeg_sub_int_handler_dec_core_regrdint( INT32* result )
{
	USHORT	width;
	USHORT	lines;

	if ( IO_JPG7.JPINTSTA.bit.REGRDINT == 1 ) {
		// REGRDINT bit clear
		IO_JPG7.JPINTSTA.bit.REGRDINT = 0;
	}

	// Set result
	*result |= D_IM_JPEG_REGRDINT;

	// Get the target image size
	gIm_Jpeg_Dec_Mng.org_width = IO_JPG7.JPWIDTH.bit.JPWIDTH;

	if ( IO_JPG7.JPDNLSET.bit.JPDNLSET == 0 ) {
		gIm_Jpeg_Dec_Mng.org_lines = IO_JPG7.JPHEIGHT.bit.JPHEIGHT;
	}
	else {
		// set DNL line value
		gIm_Jpeg_Dec_Mng.org_lines = IO_JPG7.JPDNLSET.bit.JPDNLSET;
	}

	// Check sampling type
	if ( gIm_Jpeg_Dec_Mng.smpl_type != (E_IM_JPEG_SMPL_TYPE)IO_JPG7.JPMODE.bit.MMODE ) {
		gIm_Jpeg_Dec_Mng.smpl_type = (E_IM_JPEG_SMPL_TYPE)IO_JPG7.JPMODE.bit.MMODE;
		// Set result
		*result |= D_IM_JPEG_SMPL_TYPE_NG;
		// Error reset
		im_jpeg_reset();

		return D_IM_JPEG_DECODE_ERR;

	}

	// Cutout enable check
	if ( gIm_Jpeg_Dec_Frame_Mng.cutout_flg ) {
		if ( gIm_Jpeg_Dec_Mng.smpl_type == E_IM_JPEG_SMPL_TYPE_YCC420 ) {
			// Set result
			*result |= D_IM_JPEG_CUTOUT_ERR;
			// Error reset
			im_jpeg_reset();

			return D_IM_JPEG_DECODE_ERR;
		}
	}

#ifdef CO_IM_JPEG_DEBUG
	Ddim_Print(("IO_JPG7.JPWIDTH.bit.JPWIDTH.   =[%d]\n", IO_JPG7.JPWIDTH.bit.JPWIDTH));
	Ddim_Print(("IO_JPG7.JPHEIGHT.bit.JPHEIGHT. =[%d]\n", IO_JPG7.JPHEIGHT.bit.JPHEIGHT));
	Ddim_Print(("IO_JPG7.JPDNLSET.bit.JPDNLSET. =[%d]\n", IO_JPG7.JPDNLSET.bit.JPDNLSET));
	Ddim_Print(("IO_JPG7.JPMODE.bit.MMODE.      =[0x%X]\n", IO_JPG7.JPMODE.bit.MMODE));
#endif

	width = gIm_Jpeg_Dec_Mng.org_width;

	if ( (width & 0x01) != 0 ) {
		if ( gIm_Jpeg_Dec_Mng.smpl_type == E_IM_JPEG_SMPL_TYPE_YCC422
			|| gIm_Jpeg_Dec_Mng.smpl_type == E_IM_JPEG_SMPL_TYPE_YCC420 ) {
		// 2pix unit
			width = (USHORT)(((width >> 1) << 1 ) + 2);
		}
	}

	lines = gIm_Jpeg_Dec_Mng.org_lines;

	if ( (lines & 0x01) != 0 ) {
		if ( gIm_Jpeg_Dec_Mng.smpl_type == E_IM_JPEG_SMPL_TYPE_YCC420 ) {
		// 2lines unit
			lines = (USHORT)(((lines >> 1) << 1 ) + 2);
		}
	}

	if ( gIm_Jpeg_Dec_Frame_Mng.cutout_flg ) {
		IO_JPG7.JIMGSHDG.bit.JIMGSHDG = width;
		IO_JPG7.JIMGSH.bit.JIMGSH = gIm_Jpeg_Dec_Frame_Mng.cutout_width;
		IO_JPG7.JIMGSV.bit.JIMGSV = gIm_Jpeg_Dec_Frame_Mng.cutout_lines;
		IO_JPG7.JHOFFSET.bit.JHOFFSET = gIm_Jpeg_Dec_Frame_Mng.cutout_offset_h;
		IO_JPG7.JVOFFSET.bit.JVOFFSET = gIm_Jpeg_Dec_Frame_Mng.cutout_offset_v;
	}
	else {
		IO_JPG7.JIMGSH.bit.JIMGSH = width;
		IO_JPG7.JIMGSV.bit.JIMGSV = lines;
		IO_JPG7.JIMGSHDG.bit.JIMGSHDG = 0;
		IO_JPG7.JHOFFSET.bit.JHOFFSET = 0;
		IO_JPG7.JVOFFSET.bit.JVOFFSET = 0;
	}

	// Set the frame memory
	IO_JPG7.JIMGYGH.bit.JIMGYGH = gIm_Jpeg_Dec_Frame_Mng.global_y_width;
	IO_JPG7.JIMGCGH.bit.JIMGCGH = gIm_Jpeg_Dec_Frame_Mng.global_c_width;

#ifdef CO_IM_JPEG_DEBUG
	Ddim_Print(("IO_JPG7.JIMGYGH  =[%ld]\n", IO_JPG7.JIMGYGH.word));
	Ddim_Print(("IO_JPG7.JIMGCGH  =[%ld]\n", IO_JPG7.JIMGCGH.word));
	Ddim_Print(("IO_JPG7.JIMGSHDG =[%ld]\n", IO_JPG7.JIMGSHDG.word));
	Ddim_Print(("IO_JPG7.JIMGSH   =[%ld]\n", IO_JPG7.JIMGSH.word));
	Ddim_Print(("IO_JPG7.JIMGSV   =[%ld]\n", IO_JPG7.JIMGSV.word));
	Ddim_Print(("IO_JPG7.JHOFFSET =[%ld]\n", IO_JPG7.JHOFFSET.word));
	Ddim_Print(("IO_JPG7.JVOFFSET =[%ld]\n", IO_JPG7.JVOFFSET.word));
#endif

	// PBUF start
	IO_JPG7.JICTL.bit.JITRG = 1;

	if (IO_JPG7.JPSUSPEND.bit.JPSUSPEND) {
	// during suspend
		// Core restart
		IO_JPG7.JPCMD.bit.JRSTART = 1;
	}

	Im_JPEG_Dsb();

	return D_IM_JPEG_OK;
}

/**
 * @brief  Decode normal end.
 * @param[in]	None
 * @param[out]	INT32* result
 * @return	None
 */
static VOID im_jpeg_sub_int_handler_dec_core_end( INT32* result )
{
	INT32 res = 0;

	if ( IO_JPG7.JPINTSTA.bit.COREND == 1 ) {	// correction process end
		// bit clear
		IO_JPG7.JPINTSTA.bit.COREND = 0;

		if (IO_JPG7.JPCORSTA.bit.CORSIZINFO == 1) {
			// Set result
			res |= D_IM_JPEG_CORSIZINFO;
		}
		if (IO_JPG7.JPCORSTA.bit.CORDRIINFO == 1) {
			// Set result
			res |= D_IM_JPEG_CORDRIINFO;
		}

		if (IO_JPG7.JPINTSTA.bit.INTFLG == 1) {
			// INTFLG bit clear
			IO_JPG7.JPINTSTA.bit.INTFLG = 0;
		}

		// Set result
		res |= D_IM_JPEG_CORRECT_END;
	}
	else {
	// Normal end
		if (IO_JPG7.JPINTSTA.bit.INTFLG == 1) {
			// INTFLG bit clear
			IO_JPG7.JPINTSTA.bit.INTFLG = 0;
		}
		// Set result
		res |= D_IM_JPEG_NML_END;
	}

	// Set result
	*result |= res;
	gIm_Jpeg_Result_Judge |= D_IM_JPEG_NML_END;

	return;
}

/**
 * @brief  Detection segment interrupt handler.
 * @param[in]	None
 * @param[out]	INT32* result
 * @return	None
 */
static void im_jpeg_sub_int_handler_dec_core_segint( INT32* result )
{
	INT32 res = 0;

	// Segment error
	if ( IO_JPG7.JPINTSTA.bit.SIZERR != 0 ) {
		IO_JPG7.JPINTSTA.bit.SIZERR = 0;
		res |= D_IM_JPEG_SIZERR;
	}
	else if ( IO_JPG7.JPINTSTA.bit.RSTERR != 0 ) {
		IO_JPG7.JPINTSTA.bit.RSTERR = 0;
		res |= D_IM_JPEG_RSTERR;
	}
	else {
		// Do Nothing
	}

	// get error code
	gIm_Jpeg_Dec_Mng.err_code = IO_JPG7.JPERRCOD.bit.JPERRCOD;

	// Set result
	*result |= res;

	// Error reset
	im_jpeg_reset();

	return;
}

/**
 * @brief  Detection marker interrupt handler.
 * @param[in]	None
 * @param[out]	INT32* result
 * @return	None
 */
static VOID im_jpeg_sub_int_handler_dec_core_mkint( INT32* result )
{
	INT32 res = 0;

	// Marker detection interrupt
	if ( IO_JPG7.JPINTSTA.bit.UMKSINFO == 1 ) {
		IO_JPG7.JPINTSTA.bit.UMKSINFO = 0;
		// Set result
		res |= D_IM_JPEG_UMKSINFO;
	}
	else if ( IO_JPG7.JPINTSTA.bit.COMMKINFO == 1 ) {
		IO_JPG7.JPINTSTA.bit.COMMKINFO = 0;
		// Set result
		res |= D_IM_JPEG_COMMKINFO;
	}
	else if ( IO_JPG7.JPINTSTA.bit.APPMKINFO == 1 ) {
		IO_JPG7.JPINTSTA.bit.APPMKINFO = 0;
		// Set result
		res |= D_IM_JPEG_APPMKINFO;
	}
	else {
		// Fail Safe
		// Status clear
		IO_JPG7.JPINTSTA.word = 0x00000000;
		res |= D_IM_JPEG_ERR;
	}

	if (IO_JPG7.JPSUSPEND.bit.JPSUSPEND) {
		// during suspend
		// Core restart
		IO_JPG7.JPCMD.bit.JRSTART = 1;
	}

	// Set result
	*result |= res;

	return;
}

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief		Initialization of the JPEG macro.
 * @param[in]	None
 * @param[out]	None
 * @return		None
 */
VOID Im_JPEG_Init( VOID )
{
	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_clk();
	im_jpeg_on_iclk();
	im_jpeg_on_hclk();
	Im_JPEG_Dsb();

	im_jpeg_reset();

	im_jpeg_off_hclk();
	im_jpeg_off_iclk();
	im_jpeg_off_clk();
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();
}

/**
 * @brief		The semaphore of JPEG is acquired
 * @param[in]	INT32	tmout	:Timeout time ( -1:Forever, 0:Polling, Other(ms))
 * @param[out]	None
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_SEM_TIMEOUT / D_IM_JPEG_SEM_NG / D_IM_JPEG_PARAM_ERROR
 */
INT32 Im_JPEG_Open( INT32 tmout )
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if ( tmout < D_DDIM_USER_SEM_WAIT_FEVR ) {
		Ddim_Assertion(("I:Im_JPEG_Open: input param error. tmout = %x\n", tmout));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif

	if ( D_DDIM_USER_SEM_WAIT_POL == tmout ) {
		// pol_sem()
		ercd = DDIM_User_Pol_Sem( SID_IM_JPEG );
	}
	else {
		// twai_sem()
		ercd = DDIM_User_Twai_Sem( SID_IM_JPEG, (DDIM_USER_TMO)tmout );
	}

	if( D_DDIM_USER_E_OK != ercd ) {
		if( D_DDIM_USER_E_TMOUT == ercd ) {
			Ddim_Print(("I:Im_JPEG_Open() Error : Semaphore Get Time Out. ercd = %d\n", ercd));
			return D_IM_JPEG_SEM_TIMEOUT;
		}

		Ddim_Print(("I:Im_JPEG_Open() Error : Semaphore Get Error. ercd = %d\n", ercd));
		return D_IM_JPEG_SEM_NG;
	}

	im_jpeg_on_clk();	// CLK on
	im_jpeg_on_iclk();	// ICLK on
	Im_JPEG_Dsb();

	// Initialize
	memset ( &gIm_Jpeg_Enc_Mng, 0, sizeof(T_IM_JPEG_ENC_MNG));
	memset ( &gIm_Jpeg_Enc_Frame_Mng, 0, sizeof(T_IM_JPEG_ENC_FRAME_MNG));
	memset ( &gIm_Jpeg_Dec_Mng, 0, sizeof(T_IM_JPEG_DEC_MNG));
	memset ( &gIm_Jpeg_Dec_Frame_Mng, 0, sizeof(T_IM_JPEG_DEC_FRAME_MNG));
	memset ( &gIm_Jpeg_Next_Frame_Quant_Tbl, 0, sizeof(T_IM_JPEG_QUAT_TBL_PACK));

	return D_IM_JPEG_OK;
}

/**
 * @brief		The semaphore of JPEG is returned
 * @param[in]	None
 * @param[out]	None
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_SEM_NG
 */
INT32 Im_JPEG_Close( VOID )
{
	DDIM_USER_ER	ercd;

	im_jpeg_off_iclk();	// ICLK off
	im_jpeg_off_clk();	// CLK off
	Im_JPEG_Dsb();

	gIm_Jpeg_Update_Quant_Flg = 0;

	ercd = DDIM_User_Sig_Sem( SID_IM_JPEG );
	if ( ercd != D_DDIM_USER_E_OK ) {
		Ddim_Print(("I:Im_JPEG_Close() Error : ercd = %d\n", ercd));
		return D_IM_JPEG_SEM_NG;
	}

	return D_IM_JPEG_OK;
}

/**
 * @brief		Set quantization table
 * @param[in]	T_IM_JPEG_QUAT_TBL_PACK*	p_quant_tbl		: Pointer to quantization table
 * @param[in]	UCHAR						next_frm_flg	: Flag to determine whether a quantization table for the next frame
 * @param[out]	None
 * @return 		None
 * @note		"p_quant_tbl" is NULL, use the default table.
 */
VOID Im_JPEG_Set_QTbl( T_IM_JPEG_QUAT_TBL_PACK* p_quant_tbl, UCHAR next_frm_flg )
{
	// Set flag for next frame 
	gIm_Jpeg_Update_Quant_Flg = next_frm_flg;

	if ( next_frm_flg ) {
	// For next frame 
		im_jpeg_set_next_quant_tbl( p_quant_tbl );
	}
	else {
		// Set quantization table
		DDIM_User_AhbReg_SpinLock();
		im_jpeg_set_quant_tbl( p_quant_tbl );
		DDIM_User_AhbReg_SpinUnLock();
	}

	return;
}

/**
 * @brief		Set the quantization table value by considering the quality value.
 * @param[in]	INT32	qua_value		: Quality value ( format: S7.10 )
 * @param[in]	UCHAR	next_frm_flg	: Flag to determine whether a quantization table for the next frame
 * @param[out]	None
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_PARAM_ERROR
 */
INT32 Im_JPEG_Set_Quality( INT32 qua_value, UCHAR next_frm_flg )
{
	INT32 i, index;
	UCHAR luma_array[64];
	UCHAR chro_array[64];
	INT32 luma_buf;
	INT32 chro_buf;

	// parameter check
#ifdef CO_PARAM_CHECK
	if ((UINT32)qua_value >= (1 << 17)) {	// check for S7.10 format (18bits length)
		Ddim_Assertion(("Im_JPEG_Set_Quality(): Quality value error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if (qua_value == 0) {
		// DC component
		luma_array[0] = gDefault_Quantize_Tbl_Luma.quant_value[0];
		chro_array[0] = gDefault_Quantize_Tbl_Chroma.quant_value[0];
		// AC component = 1
		for ( i = 1; i < 64; i++ ) {
			luma_array[i] = 1;
			chro_array[i] = 1;
		}
	}
	else {
		// DC/AC component
		for ( i = 0; i < 64; i++ ) {
			luma_buf = (((((INT32)(gDefault_Quantize_Tbl_Luma.quant_value[i]  )) << 11) / qua_value) + 1) >> 1;
			chro_buf = (((((INT32)(gDefault_Quantize_Tbl_Chroma.quant_value[i])) << 11) / qua_value) + 1) >> 1;
			luma_array[i] = (UCHAR)Im_JPEG_Limit( luma_buf, 1, 255 );
			chro_array[i] = (UCHAR)Im_JPEG_Limit( chro_buf, 1, 255 );
		}
	}

	// Set flag for next frame 
	gIm_Jpeg_Update_Quant_Flg = next_frm_flg;

	if ( next_frm_flg ) {
		// Set Next Quantization table
		// Quantization table No.0 for Y
		for ( i = 0; i < 64; i++ ) {
			gIm_Jpeg_Quant_Tbl_0.quant_value[i] = luma_array[i];
		}
		// Quantization table No.1 for Cb/Cr
		for ( i = 0; i < 64; i++ ) {
			gIm_Jpeg_Quant_Tbl_1.quant_value[i] = chro_array[i];
		}
		// Quantization table No.2 for Y
		for ( i = 0; i < 64; i++ ) {
			gIm_Jpeg_Quant_Tbl_2.quant_value[i] = luma_array[i];
		}
		// Quantization table No.3 for Cb/Cr
		for ( i = 0; i < 64; i++ ) {
			gIm_Jpeg_Quant_Tbl_3.quant_value[i] = chro_array[i];
		}
		// Set table pack
		gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_0 = &gIm_Jpeg_Quant_Tbl_0;
		gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_1 = &gIm_Jpeg_Quant_Tbl_1;
		gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_2 = &gIm_Jpeg_Quant_Tbl_2;
		gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_3 = &gIm_Jpeg_Quant_Tbl_3;

#ifdef CO_IM_JPEG_DEBUG
		UCHAR i;
		for ( i=0; i < 64; i++ ) {
			Ddim_Print(("gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_0.quant_value[%d]=0x%X\n", i, gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_0->quant_value[i]));
		}
		for ( i=0; i < 64; i++ ) {
			Ddim_Print(("gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_1.quant_value[%d]=0x%X\n", i, gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_1->quant_value[i]));
		}
		for ( i=0; i < 64; i++ ) {
			Ddim_Print(("gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_2.quant_value[%d]=0x%X\n", i, gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_2->quant_value[i]));
		}
		for ( i=0; i < 64; i++ ) {
			Ddim_Print(("gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_3.quant_value[%d]=0x%X\n", i, gIm_Jpeg_Next_Frame_Quant_Tbl.quantization_tbl_3->quant_value[i]));
		}
#endif

	}
	else {
		DDIM_User_AhbReg_SpinLock();

		im_jpeg_on_hclk();	// HCLK on
		Im_JPEG_Dsb();

		IO_IMG.SRAMPD.bit.JPEGPD &= ~0x8;	// QTTBL SRAM power down off.

		Im_JPEG_Dsb();

		// QTTBL SRAM power down control wait time.
		Dd_ARM_Wait_ns( 1000 );

		// Set Quantization table
		// Quantization table No.0 for Y
		for ( i = 0, index = 0; i < 16; i++ ) {
			IO_JPG7.JPQT0.word[i] = ((luma_array[index] << 24) | (luma_array[index+1] << 16) | (luma_array[index+2] << 8) | (luma_array[index+3]));
			index = index + 4;
		}
		// Quantization table No.1 for Cb/Cr
		for ( i = 0, index = 0; i < 16; i++ ) {
			IO_JPG7.JPQT1.word[i] = ((chro_array[index] << 24) | (chro_array[index+1] << 16) | (chro_array[index+2] << 8) | (chro_array[index+3]));
			index = index + 4;
		}
		// Quantization table No.2 for Y
		for ( i = 0, index = 0; i < 16; i++ ) {
			IO_JPG7.JPQT2.word[i] = ((luma_array[index] << 24) | (luma_array[index+1] << 16) | (luma_array[index+2] << 8) | (luma_array[index+3]));
			index = index + 4;
		}
		// Quantization table No.3 for Cb/Cr
		for ( i = 0, index = 0; i < 16; i++ ) {
			IO_JPG7.JPQT3.word[i] = ((chro_array[index] << 24) | (chro_array[index+1] << 16) | (chro_array[index+2] << 8) | (chro_array[index+3]));
			index = index + 4;
		}

#ifdef CO_IM_JPEG_DEBUG
		static INT32 zero_cnt=0;
		ULONG value;
		for ( i = 0; i < 16; i++ ) {
			value = IO_JPG7.JPQT0.word[i];
			if ( (value & 0xFF000000) == 0 ) {
				zero_cnt++;
			}
			if ( (value & 0x00FF0000) == 0 ) {
				zero_cnt++;
			}
			if ( (value & 0x0000FF00) == 0 ) {
				zero_cnt++;
			}
			if ( (value & 0x000000FF) == 0 ) {
				zero_cnt++;
			}
		}

		for ( i = 0; i < 16; i++ ) {
			value = IO_JPG7.JPQT1.word[i];
			if ( (value & 0xFF000000) == 0 ) {
				zero_cnt++;
			}
			if ( (value & 0x00FF0000) == 0 ) {
				zero_cnt++;
			}
			if ( (value & 0x0000FF00) == 0 ) {
				zero_cnt++;
			}
			if ( (value & 0x000000FF) == 0 ) {
				zero_cnt++;
			}
		}

		for ( i = 0; i < 16; i++ ) {
			value = IO_JPG7.JPQT2.word[i];
			if ( (value & 0xFF000000) == 0 ) {
				zero_cnt++;
			}
			if ( (value & 0x00FF0000) == 0 ) {
				zero_cnt++;
			}
			if ( (value & 0x0000FF00) == 0 ) {
				zero_cnt++;
			}
			if ( (value & 0x000000FF) == 0 ) {
				zero_cnt++;
			}
		}

		for ( i = 0; i < 16; i++ ) {
			value = IO_JPG7.JPQT3.word[i];
			if ( (value & 0xFF000000) == 0 ) {
				zero_cnt++;
			}
			if ( (value & 0x00FF0000) == 0 ) {
				zero_cnt++;
			}
			if ( (value & 0x0000FF00) == 0 ) {
				zero_cnt++;
			}
			if ( (value & 0x000000FF) == 0 ) { 
				zero_cnt++;
			}
		}
		if ( zero_cnt != 0 ) {
			while(1){
				;
			}
		}
#endif
		im_jpeg_off_hclk();	// HCLK off
		Im_JPEG_Dsb();

		DDIM_User_AhbReg_SpinUnLock();
	}

	return D_IM_JPEG_OK;
}

/**
 * @brief		Setting the block downsampling (for Pass-1 mode)
 * @param[in]	UCHAR downsp_type : downsampling type  (0:1/1, 1:1/2, 2:1/4, 3:1/8)
 * @param[out]	None
 * @return		USHORT	: Proportion of remainder after adjusting for size when decimated. (format: 8.8)
 */
USHORT Im_JPEG_Set_Down_Sampling_Rate( UCHAR downsp_type )
{
	USHORT	dsp_h_size;
	USHORT	dsp_v_size;
	DOUBLE	org_size;
	DOUBLE	mcr_size;
	USHORT	err_ratio = 0;
	UINT32	round_h;
	UINT32	round_v;

#ifdef CO_PARAM_CHECK
	if ( downsp_type > 3 ){
		Ddim_Assertion(("I:Im_JPEG_Set_Enc_Decim. decim_type unknown ratio error\n"));
		return 0;
	}
#endif

	// Base alignment value
	switch( gIm_Jpeg_Enc_Mng.ycc_smpl ) {
		case E_IM_JPEG_SMPL_TYPE_YCC420:
			round_h = 2;
			round_v = 2;
			break;
		case E_IM_JPEG_SMPL_TYPE_YCC422:
			round_h = 2;
			round_v = 1;
			break;
		case E_IM_JPEG_SMPL_TYPE_YCC444:
		case E_IM_JPEG_SMPL_TYPE_YCC400:
			round_h = 1;
			round_v = 1;
			break;
		default:
			Ddim_Print(("Im_JPEG_Set_Down_Sampling_Rate sampling type error!\n"));
			return 0;
	}

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// Set the down sampling rate
	IO_JPG7.JICTL.bit.JIXP = downsp_type;

	// original size
	org_size = ((DOUBLE)gIm_Jpeg_Enc_Mng.width * gIm_Jpeg_Enc_Mng.lines );

	// alignment value
	if ( downsp_type != D_IM_JPEG_DOWNSP_NONE ) {
		if ( downsp_type == D_IM_JPEG_DOWNSP_1_2 ) {
			round_h *= 16;
		}
		else {
			round_h *= 8;
		}
		round_v *= 8;
	}

	// Horizon
	// adjust align
	dsp_h_size = (USHORT)(round_h * (USHORT)( (gIm_Jpeg_Enc_Mng.width / (0x1 << downsp_type)) / round_h ));

	gIm_Jpeg_Enc_Mng.width = ( dsp_h_size << downsp_type );

	IO_JPG7.JIMGSH.bit.JIMGSH		= gIm_Jpeg_Enc_Mng.width;
	IO_JPG7.JPWIDTH.bit.JPWIDTH		= dsp_h_size;


	// Vertical
	// adjust align
	dsp_v_size = (USHORT)(round_v * (USHORT)( gIm_Jpeg_Enc_Mng.lines / (0x1 << downsp_type) / round_v ));

	gIm_Jpeg_Enc_Mng.lines = (dsp_v_size << downsp_type );

	IO_JPG7.JIMGSV.bit.JIMGSV		= gIm_Jpeg_Enc_Mng.lines;
	IO_JPG7.JPHEIGHT.bit.JPHEIGHT	= dsp_v_size;

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();

	// Ask the error ratio
	mcr_size = ((DOUBLE)gIm_Jpeg_Enc_Mng.width * gIm_Jpeg_Enc_Mng.lines );
	err_ratio = (USHORT)( (org_size/mcr_size) * 256 );

	return err_ratio;
}

/**
 * @brief		Base configuration of the JPEG encoding process
 * @param[in]	T_IM_JPEG_ENC_MNG*	p_jpg_enc_mng	: Pointer to JPEG Encode base management table
 * @param[out]	None
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_RUNNING_NG / D_IM_JPEG_PARAM_ERROR
 */
INT32 Im_JPEG_Ctrl_Enc( T_IM_JPEG_ENC_MNG* p_jpg_enc_mng )
{
#ifdef CO_PARAM_CHECK
	if ( p_jpg_enc_mng == NULL ) {
		Ddim_Assertion(("Im_JPEG_Ctrl_Enc(): Null check error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// F_JPEG7 macro In-use check
	if ( IO_JPG7.JPSTATUS.bit.JPSTATUS != 0 ) {

		im_jpeg_off_hclk();	// HCLK off
		Im_JPEG_Dsb();

		DDIM_User_AhbReg_SpinUnLock();

		// macro running
		return D_IM_JPEG_RUNNING_NG;
	}

	// Set Jpeg base management table to a global table
	gIm_Jpeg_Enc_Mng = *p_jpg_enc_mng;

	// JPMODE
	IO_JPG7.JPMODE.word 				= 0x30;		// S_ENDIAN:1/P_ENDIAN:1/DISPOSAL:0
	IO_JPG7.JPMODE.bit.MMODE			= p_jpg_enc_mng->ycc_smpl;	// sampling type

	// JPQTABLE
	IO_JPG7.JPQTABLE.bit.QT0L			= p_jpg_enc_mng->component[0].quant_tbl_no & 0x01;
	IO_JPG7.JPQTABLE.bit.QT0U			= (p_jpg_enc_mng->component[0].quant_tbl_no >> 1) & 0x01;
	IO_JPG7.JPQTABLE.bit.QT1L			= p_jpg_enc_mng->component[1].quant_tbl_no & 0x01;
	IO_JPG7.JPQTABLE.bit.QT1U			= (p_jpg_enc_mng->component[1].quant_tbl_no >> 1) & 0x01;
	IO_JPG7.JPQTABLE.bit.QT2L			= p_jpg_enc_mng->component[2].quant_tbl_no & 0x01;
	IO_JPG7.JPQTABLE.bit.QT2U			= (p_jpg_enc_mng->component[2].quant_tbl_no >> 1) & 0x01;
	// JPHTABLE
	IO_JPG7.JPHTABLE.bit.HT0DC			= p_jpg_enc_mng->component[0].huf_dc_tbl_no;
	IO_JPG7.JPHTABLE.bit.HT0AC			= p_jpg_enc_mng->component[0].huf_ac_tbl_no;
	IO_JPG7.JPHTABLE.bit.HT1DC			= p_jpg_enc_mng->component[1].huf_dc_tbl_no;
	IO_JPG7.JPHTABLE.bit.HT1AC			= p_jpg_enc_mng->component[1].huf_ac_tbl_no;
	IO_JPG7.JPHTABLE.bit.HT2DC			= p_jpg_enc_mng->component[2].huf_dc_tbl_no;
	IO_JPG7.JPHTABLE.bit.HT2AC			= p_jpg_enc_mng->component[2].huf_ac_tbl_no;
	// JPDRISET
	IO_JPG7.JPDRISET.bit.JPDRISET		= p_jpg_enc_mng->dri_mk_num;
	// JPWIDTH
	IO_JPG7.JPWIDTH.bit.JPWIDTH			= p_jpg_enc_mng->width;
	// JPHEIGHT
	IO_JPG7.JPHEIGHT.bit.JPHEIGHT		= p_jpg_enc_mng->lines;
	// JIMGSHDG
	IO_JPG7.JIMGSHDG.bit.JIMGSHDG		= 0;		// Fixed value
	// JPFORMAT
	IO_JPG7.JPFORMAT.bit.CODECNT		= 0;		// Fixed value
	IO_JPG7.JPFORMAT.bit.MKSKIP			= p_jpg_enc_mng->skip_mk_flg;
	IO_JPG7.JPFORMAT.bit.EXIFMOD		= p_jpg_enc_mng->exif_fmt_flg;
	// JMODE
	IO_JPG7.JMODE.bit.JCSCCCEN			= 0;		// CSC Off
	IO_JPG7.JMODE.bit.JPBDEXT			= 0;		// Not use Ext mode
	IO_JPG7.JMODE.bit.JMEMFORM			= (UCHAR)p_jpg_enc_mng->mem_format;
	IO_JPG7.JMODE.bit.JPBENDIAN			= p_jpg_enc_mng->pbuf_ctrl.endian;
	IO_JPG7.JMODE.bit.JJBENDIAN			= p_jpg_enc_mng->jbuf_ctrl.endian;
	IO_JPG7.JMODE.bit.JBURST_AL_ON		= p_jpg_enc_mng->burst_alignment;
	// JPBSTA
	switch ( p_jpg_enc_mng->pbuf_ctrl.issue_tran_num ) {
		case E_IM_JPEG_ISSUE_TRAN_2:
			IO_JPG7.JPBSTA.bit.JPISSUESET	= 0x1;
			break;
		case E_IM_JPEG_ISSUE_TRAN_4:
			IO_JPG7.JPBSTA.bit.JPISSUESET	= 0x2;
			break;
		default:
			IO_JPG7.JPBSTA.bit.JPISSUESET	= 0x3;
			break;
	}
	IO_JPG7.JPBSTA.bit.JPACACHE			= p_jpg_enc_mng->pbuf_ctrl.cache_type;
	IO_JPG7.JPBSTA.bit.JPAPROT			= p_jpg_enc_mng->pbuf_ctrl.prot_type;
	// JJBSTA
	switch ( p_jpg_enc_mng->jbuf_ctrl.issue_tran_num ) {
		case E_IM_JPEG_ISSUE_TRAN_2:
			IO_JPG7.JJBSTA.bit.JJISSUESET	= 0x1;
			break;
		case E_IM_JPEG_ISSUE_TRAN_4:
			IO_JPG7.JJBSTA.bit.JJISSUESET	= 0x2;
			break;
		default:
			IO_JPG7.JJBSTA.bit.JJISSUESET	= 0x3;
			break;
	}
	IO_JPG7.JJBSTA.bit.JJACACHE			= p_jpg_enc_mng->jbuf_ctrl.cache_type;
	IO_JPG7.JJBSTA.bit.JJAPROT			= p_jpg_enc_mng->jbuf_ctrl.prot_type;

	// JCCTL
	if ( p_jpg_enc_mng->jburst_length == E_IM_JPEG_BURST_INCR_8 ) {
		IO_JPG7.JCCTL.bit.JCRP			= 0x2;
	}
	else {
		IO_JPG7.JCCTL.bit.JCRP			= 0x3;
	}
	// JICTL
	IO_JPG7.JICTL.bit.JIXP				= 0;

	if ( p_jpg_enc_mng->pburst_length == E_IM_JPEG_BURST_INCR_8 ) {
		IO_JPG7.JICTL.bit.JIRP			= 0x2;
	}
	else {
		IO_JPG7.JICTL.bit.JIRP			= 0x3;
	}
	IO_JPG7.JICTL.bit.B12M				= p_jpg_enc_mng->bit_depth;
	
	// Periodic interrupt setting
	// line unit
	if ( p_jpg_enc_mng->pint_line != 0 ) {
		IO_JPG7.JINTEN.bit.JLINEEN		= 1;
		IO_JPG7.JLINEVAL.bit.JLINEVAL	= p_jpg_enc_mng->pint_line;
	}
	else {
		IO_JPG7.JINTEN.bit.JLINEEN		= 0;
		IO_JPG7.JLINEVAL.bit.JLINEVAL	= 0;
	}
	// sector unit
	if (p_jpg_enc_mng->pint_sect != 0) {
		IO_JPG7.JINTEN.bit.JSECTEN		= 1;
		IO_JPG7.JSECTVAL.bit.JSECTVAL	= p_jpg_enc_mng->pint_sect;
	}
	else {
		IO_JPG7.JINTEN.bit.JSECTEN		= 0;
		IO_JPG7.JSECTVAL.bit.JSECTVAL	= 0;
	}

	// IIPSET
	IO_JPG7.IIPSET.bit.PORT_MODE		= p_jpg_enc_mng->port_mode;
	IO_JPG7.IIPSET.bit.RING_ON			= p_jpg_enc_mng->ring_on;
	IO_JPG7.IIPSET.bit.BLK_SIZE			= p_jpg_enc_mng->block_size;
	IO_JPG7.IIPSET.bit.RING_SIZE		= p_jpg_enc_mng->ring_size;

	// SRAM power down control
	if( p_jpg_enc_mng->ycc_smpl == E_IM_JPEG_SMPL_TYPE_YCC400 ) {
		IO_IMG.SRAMPD.bit.JPEGPD = 0x7;
	}
	else {
		IO_IMG.SRAMPD.bit.JPEGPD = 0x5;
	}

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();

	// SRAM power down control wait time.
	Dd_ARM_Wait_ns( 1000 );

	return D_IM_JPEG_OK;
}

/**
 * @brief		Frame configuration of the JPEG encoding process
 * @param[in]	T_IM_JPEG_ENC_FRAME_MNG*	p_jpg_enc_frm_mng	: Pointer to JPEG Encode frame management table
 * @param[out]	None
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_PARAM_ERROR
 */
INT32 Im_JPEG_Ctrl_Enc_Frame( T_IM_JPEG_ENC_FRAME_MNG* p_jpg_enc_frm_mng )
{
#ifdef CO_PARAM_CHECK
	if ( p_jpg_enc_frm_mng == NULL ) {
		Ddim_Assertion(("Im_JPEG_Ctrl_Enc_Frame(): Null check error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	// Set Jpeg frame management table to a global table
	gIm_Jpeg_Enc_Frame_Mng = *p_jpg_enc_frm_mng;

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// JIMGYGH
	IO_JPG7.JIMGYGH.bit.JIMGYGH		= p_jpg_enc_frm_mng->global_y_width;
	// JIMGCGH
	IO_JPG7.JIMGCGH.bit.JIMGCGH		= p_jpg_enc_frm_mng->global_c_width;
	// JIMGSH
	IO_JPG7.JIMGSH.bit.JIMGSH		= gIm_Jpeg_Enc_Mng.width;	// same JWIDTH value
	// JIMGSV
	IO_JPG7.JIMGSV.bit.JIMGSV		= gIm_Jpeg_Enc_Mng.lines;	// same JHEIGHT value
	// JIMGAY
	IO_JPG7.JIMGAY.bit.JIMGAY		= p_jpg_enc_frm_mng->ycc_addr.y;
	// JIMGACB
	IO_JPG7.JIMGACB.bit.JIMGACB		= p_jpg_enc_frm_mng->ycc_addr.c;
	// JCC (pass-1 mode)
	IO_JPG7.JCCTL.bit.JCC			= p_jpg_enc_frm_mng->code_count_flg;
	// JCPS
	IO_JPG7.JCCTL.bit.JCPS			= p_jpg_enc_frm_mng->code_over_count_flg;
	// JCODA
	IO_JPG7.JCODA.bit.JCODA			= p_jpg_enc_frm_mng->code_addr;
	// JCODSV
	if ( p_jpg_enc_frm_mng->limit_size == 0 || p_jpg_enc_frm_mng->code_count_flg ) {
		IO_JPG7.JCODSV.word	= 0;
	}
	else if ( p_jpg_enc_frm_mng->limit_size < D_IM_JPEG_SECT_CNT ) {
		IO_JPG7.JCODSV.word	= 1;
		Ddim_Print(("I:Im_JPEG_Ctrl_Enc_Frame() Warning : Limit size changed to 512 bytes.\n"));
	}
	else {
		IO_JPG7.JCODSV.word	= (ULONG)(p_jpg_enc_frm_mng->limit_size / D_IM_JPEG_SECT_CNT);
	}

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();

	return D_IM_JPEG_OK;
}

/**
 * @brief		get the base settings for Jpeg encode.
 * @param[in]	None
 * @param[out]	T_IM_JPEG_ENC_MNG* p_jpg_enc_mng
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_PARAM_ERROR
 */
INT32	Im_JPEG_Get_Ctrl_Enc( T_IM_JPEG_ENC_MNG* p_jpg_enc_mng )
{
#ifdef CO_PARAM_CHECK
	if ( p_jpg_enc_mng == NULL ) {
		Ddim_Assertion(("Im_JPEG_Get_Ctrl_Enc(): Null check error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	p_jpg_enc_mng->ycc_smpl = (E_IM_JPEG_SMPL_TYPE)IO_JPG7.JPMODE.bit.MMODE;	// sampling type

	// JPQTABLE
	p_jpg_enc_mng->component[0].quant_tbl_no	= (IO_JPG7.JPQTABLE.bit.QT0U << 1) | IO_JPG7.JPQTABLE.bit.QT0L;
	p_jpg_enc_mng->component[1].quant_tbl_no	= (IO_JPG7.JPQTABLE.bit.QT1U << 1) | IO_JPG7.JPQTABLE.bit.QT1L;
	p_jpg_enc_mng->component[2].quant_tbl_no	= (IO_JPG7.JPQTABLE.bit.QT2U << 1) | IO_JPG7.JPQTABLE.bit.QT2L;
	// JPHTABLE
	p_jpg_enc_mng->component[0].huf_dc_tbl_no	= IO_JPG7.JPHTABLE.bit.HT0DC;
	p_jpg_enc_mng->component[0].huf_ac_tbl_no	= IO_JPG7.JPHTABLE.bit.HT0AC;
	p_jpg_enc_mng->component[1].huf_dc_tbl_no	= IO_JPG7.JPHTABLE.bit.HT1DC;
	p_jpg_enc_mng->component[1].huf_ac_tbl_no	= IO_JPG7.JPHTABLE.bit.HT1AC;
	p_jpg_enc_mng->component[2].huf_dc_tbl_no	= IO_JPG7.JPHTABLE.bit.HT2DC;
	p_jpg_enc_mng->component[2].huf_ac_tbl_no	= IO_JPG7.JPHTABLE.bit.HT2AC;
	// JPDRISET
	p_jpg_enc_mng->dri_mk_num					= IO_JPG7.JPDRISET.bit.JPDRISET;
	// JPWIDTH
	p_jpg_enc_mng->width						= IO_JPG7.JPWIDTH.bit.JPWIDTH;
	// JPHEIGHT
	p_jpg_enc_mng->lines						= IO_JPG7.JPHEIGHT.bit.JPHEIGHT;
	// JPFORMAT
	p_jpg_enc_mng->skip_mk_flg					= IO_JPG7.JPFORMAT.bit.MKSKIP;
	p_jpg_enc_mng->exif_fmt_flg					= IO_JPG7.JPFORMAT.bit.EXIFMOD;
	// JMODE
	p_jpg_enc_mng->pbuf_ctrl.endian				= (E_IM_JPEG_ENDIAN)IO_JPG7.JMODE.bit.JPBENDIAN;
	p_jpg_enc_mng->jbuf_ctrl.endian				= (E_IM_JPEG_ENDIAN)IO_JPG7.JMODE.bit.JJBENDIAN;
	p_jpg_enc_mng->burst_alignment				= (E_IM_JPEG_BURST_AL)IO_JPG7.JMODE.bit.JBURST_AL_ON;
	// JPBSTA
	switch ( (UCHAR)IO_JPG7.JPBSTA.bit.JPISSUESET ) {
		case 0x01:
			p_jpg_enc_mng->pbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_2;
			break;
		case 0x02:
			p_jpg_enc_mng->pbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_4;
			break;
		default:
			p_jpg_enc_mng->pbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
			break;
	}
	p_jpg_enc_mng->pbuf_ctrl.cache_type			= IO_JPG7.JPBSTA.bit.JPACACHE;
	p_jpg_enc_mng->pbuf_ctrl.prot_type			= IO_JPG7.JPBSTA.bit.JPAPROT;
	// JJBSTA
	switch ( (UCHAR)IO_JPG7.JJBSTA.bit.JJISSUESET ) {
		case 0x01:
			p_jpg_enc_mng->jbuf_ctrl.issue_tran_num = E_IM_JPEG_ISSUE_TRAN_2;
			break;
		case 0x02:
			p_jpg_enc_mng->jbuf_ctrl.issue_tran_num = E_IM_JPEG_ISSUE_TRAN_4;
			break;
		default:
			p_jpg_enc_mng->jbuf_ctrl.issue_tran_num = E_IM_JPEG_ISSUE_TRAN_8;
			break;
	}
	p_jpg_enc_mng->jbuf_ctrl.cache_type			= IO_JPG7.JJBSTA.bit.JJACACHE;
	p_jpg_enc_mng->jbuf_ctrl.prot_type			= IO_JPG7.JJBSTA.bit.JJAPROT;

	// JCCTL
	if ( (UCHAR)IO_JPG7.JCCTL.bit.JCRP == 0x2 ) {
		p_jpg_enc_mng->jburst_length	= E_IM_JPEG_BURST_INCR_8;
	}
	else {
		p_jpg_enc_mng->jburst_length	= E_IM_JPEG_BURST_INCR_16;
	}
	// JICTL
	if ( (UCHAR)IO_JPG7.JICTL.bit.JIRP == 0x2 ) {
		p_jpg_enc_mng->pburst_length	= E_IM_JPEG_BURST_INCR_8;
	}
	else {
		p_jpg_enc_mng->pburst_length	= E_IM_JPEG_BURST_INCR_16;
	}
	p_jpg_enc_mng->bit_depth					= IO_JPG7.JICTL.bit.B12M;

	// Periodic interrupt setting
	// line unit
	p_jpg_enc_mng->pint_line					= IO_JPG7.JLINEVAL.bit.JLINEVAL;

	// sector unit
	p_jpg_enc_mng->pint_sect					= IO_JPG7.JSECTVAL.bit.JSECTVAL;

	// IIPSET
	p_jpg_enc_mng->port_mode					= (E_IM_JPEG_IIP_PORT_MODE)IO_JPG7.IIPSET.bit.PORT_MODE;
	p_jpg_enc_mng->ring_on						= IO_JPG7.IIPSET.bit.RING_ON;
	p_jpg_enc_mng->block_size					= (E_IM_JPEG_IIP_BLOCK)IO_JPG7.IIPSET.bit.BLK_SIZE;
	p_jpg_enc_mng->ring_size					= IO_JPG7.IIPSET.bit.RING_SIZE;

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();

	return D_IM_JPEG_OK;
}


/**
 * @brief		Get the frame settings for Jpeg encode.
 * @param[in]	None
 * @param[out]	T_IM_JPEG_ENC_FRAME_MNG* p_jpg_enc_frm_mng
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_PARAM_ERROR
 */
INT32	Im_JPEG_Get_Ctrl_Enc_Frame( T_IM_JPEG_ENC_FRAME_MNG* p_jpg_enc_frm_mng )
{
#ifdef CO_PARAM_CHECK
	if ( p_jpg_enc_frm_mng == NULL ) {
		Ddim_Assertion(("Im_JPEG_Get_Ctrl_Enc_Frame(): Null check error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// JIMGYGH
	p_jpg_enc_frm_mng->global_y_width = IO_JPG7.JIMGYGH.bit.JIMGYGH;
	// JIMGCGH
	p_jpg_enc_frm_mng->global_c_width = IO_JPG7.JIMGCGH.bit.JIMGCGH;

	// JIMGAY
	p_jpg_enc_frm_mng->ycc_addr.y = IO_JPG7.JIMGAY.bit.JIMGAY;
	// JIMGACB
	p_jpg_enc_frm_mng->ycc_addr.c = IO_JPG7.JIMGACB.bit.JIMGACB;
	// JCC (pass-1 mode)
	p_jpg_enc_frm_mng->code_count_flg = IO_JPG7.JCCTL.bit.JCC;
	// JCPS
	p_jpg_enc_frm_mng->code_over_count_flg = IO_JPG7.JCCTL.bit.JCPS;
	// JCODA
	p_jpg_enc_frm_mng->code_addr = IO_JPG7.JCODA.bit.JCODA;

	// JCODSV
	p_jpg_enc_frm_mng->limit_size = ((IO_JPG7.JCODSV.word) * (ULLONG)D_IM_JPEG_SECT_CNT);

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();

	return D_IM_JPEG_OK;
}


/**
 * @brief		JPEG Encoding process asynchronous.
 * @param[in]	None
 * @param[out]	None
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_RUNNING_NG / D_IM_JPEG_SYSTEMCALL_ERR
 */
INT32 Im_JPEG_Start_Enc( VOID )
{
	DDIM_USER_ER		ercd;

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// F_JPEG7 macro In-use check
	if ( IO_JPG7.JPSTATUS.bit.JPSTATUS != 0 ) {

		im_jpeg_off_hclk();	// HCLK off
		Im_JPEG_Dsb();

		DDIM_User_AhbReg_SpinUnLock();

		// macro running
		return D_IM_JPEG_RUNNING_NG;
	}

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();


	// Initialize
	gIm_Jpeg_Enc_Mng.code_size	= 0;		// Size of JPEG Compressed data
	gIm_Jpeg_Enc_Mng.result		= 0;		// Processing result
	gIm_Jpeg_Result_Judge		= 0;
	gIm_Jpeg_Line_Cnt = 0;
	gIm_Jpeg_Sect_Cnt = 0;

	ercd = DDIM_User_Clr_Flg( FID_IM_JPEG, ~D_IM_JPEG_FLG_WAIT_END );
	if( D_DDIM_USER_E_OK != ercd ) {

		return D_IM_JPEG_SYSTEMCALL_ERR;
	}

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// interrupt setting
	IO_JPG7.JINTEN.word |= 0xC4D000;

	if ( gIm_Jpeg_Update_Quant_Flg ) {
	// Set quant table for the next frame -> enable
		IO_JPG7.JPINTSET.bit.QTWSET	= 1;
	}
	else {
		IO_JPG7.JPINTSET.bit.QTWSET	= 0;
	}

	// CORE start
	IO_JPG7.JPCMD.bit.JSTART	= 1;
	// PBUF start
	IO_JPG7.JICTL.bit.JITRG 	= 1;
	// JBUF start
	IO_JPG7.JCCTL.bit.JCTRG 	= 1;

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb(); 

	DDIM_User_AhbReg_SpinUnLock();

	return D_IM_JPEG_OK;
}

/**
 * @brief		Waiting to be processed JPEG encoding.
 * @param[out]	T_IM_JPEG_ENC_MNG*			p_jpg_enc_mng		:Result of the JPEG encoding process
 * @param[in]	INT32						time_out			:Time-out period
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_PARAM_ERROR / D_IM_JPEG_TIMEOUT / D_IM_JPEG_ENCODE_ERR / D_IM_JPEG_AXI_ERR
 */
INT32 Im_JPEG_Wait_End_Enc( T_IM_JPEG_ENC_MNG* p_jpg_enc_mng, INT32 time_out )
{
	DDIM_USER_FLGPTN flgptn = 0;
	DDIM_USER_ER ercd;
	INT32 ret;

#ifdef CO_PARAM_CHECK
	if ( p_jpg_enc_mng == NULL ) {
		Ddim_Assertion(("Im_JPEG_Wait_End_Enc(): Null check error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
	if ( time_out < D_DDIM_WAIT_END_FOREVER ) {
		Ddim_Assertion(("Im_JPEG_Wait_End_Enc(): input param error. time_out = %x\n", time_out));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	ercd = DDIM_User_Twai_Flg( FID_IM_JPEG, D_IM_JPEG_FLG_WAIT_END, D_DDIM_USER_TWF_ORW, &flgptn, time_out );

	// Set Result
	p_jpg_enc_mng->code_size	= gIm_Jpeg_Enc_Mng.code_size;
	p_jpg_enc_mng->result		= gIm_Jpeg_Enc_Mng.result;

	if (ercd != D_DDIM_USER_E_OK) {
		ret = D_IM_JPEG_TIMEOUT;
	}
	else {
		// judge the process result
		if (((p_jpg_enc_mng->result & D_IM_JPEG_JJBINT) == D_IM_JPEG_JJBINT)
		  ||((p_jpg_enc_mng->result & D_IM_JPEG_JPBINT) == D_IM_JPEG_JPBINT) ) {
			// bus error
			ret = D_IM_JPEG_AXI_ERR;
		}
		else if ( (p_jpg_enc_mng->result & (D_IM_JPEG_JCLRFMER | D_IM_JPEG_ERR)) != 0 ) {
			ret = D_IM_JPEG_ENCODE_ERR;
		}
		else if ( (p_jpg_enc_mng->result & D_IM_JPEG_PAUSE) == D_IM_JPEG_PAUSE ) {
			// pausing end or code size over
			ret = D_IM_JPEG_ENCODE_PAUSE;
		}
		else {
			// encode process finished
			ret = D_IM_JPEG_OK;
		}
	}

	return ret;
}

/**
 * @brief		JPEG encode restart.(Restart from the state is paused encoding)
 * @param[in]	T_IM_JPEG_ENC_FRAME_MNG*	p_jpg_enc_frm_mng	: Pointer to JPEG Encode frame management table
 * @param[out]	None
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_RUNNING_NG / D_IM_JPEG_PARAM_ERROR / D_IM_JPEG_SYSTEMCALL_ERR
 */
INT32 Im_JPEG_Restart_Enc( T_IM_JPEG_ENC_FRAME_MNG* p_jpg_enc_frm_mng )
{
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if ( p_jpg_enc_frm_mng == NULL ) {
		Ddim_Assertion(("Im_JPEG_Restart_Enc(): Null check error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	if ( IO_JPG7.JCCTL.bit.JCTRG != 2 ) {
		im_jpeg_off_hclk();	// HCLK off
		Im_JPEG_Dsb();
		DDIM_User_AhbReg_SpinUnLock();
		// Coding not stopped
		return D_IM_JPEG_RUNNING_NG;
	}

	// Initialize result
	gIm_Jpeg_Enc_Mng.result		= 0;		// Processing result
	gIm_Jpeg_Result_Judge		= 0;

	// Set destination addr
	IO_JPG7.JCODA.bit.JCODA  = p_jpg_enc_frm_mng->code_addr;

	if ( p_jpg_enc_frm_mng->limit_size == 0 ) {
		IO_JPG7.JINTEN.bit.JCPFEN = 0;
		IO_JPG7.JCODSV.word	= 0;
	}
	else {
		IO_JPG7.JINTEN.bit.JCPFEN = 1;
		IO_JPG7.JCODSV.word	= (ULONG)(p_jpg_enc_frm_mng->limit_size / D_IM_JPEG_SECT_CNT);
	}

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();
	DDIM_User_AhbReg_SpinUnLock();

	ercd = DDIM_User_Clr_Flg( FID_IM_JPEG, ~D_IM_JPEG_FLG_WAIT_END );
	if( D_DDIM_USER_E_OK != ercd ) {
		return D_IM_JPEG_SYSTEMCALL_ERR;
	}

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// JBUF restart
	IO_JPG7.JCCTL.bit.JCTRG 	= 1;

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();

	return D_IM_JPEG_OK;
}

/**
 * @brief		Configuration of the JPEG decoding process for marker skip mode
 * @param[in]	None
 * @param[out]	None
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_RUNNING_NG
 */
INT32 Im_JPEG_Set_Skip_Marker_Dec( VOID )
{
	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// F_JPEG7 macro In-use check
	if ( IO_JPG7.JPSTATUS.bit.JPSTATUS != 0 ) {
		im_jpeg_off_hclk();	// HCLK off
		Im_JPEG_Dsb();
		DDIM_User_AhbReg_SpinUnLock();
		// macro running
		return D_IM_JPEG_RUNNING_NG;
	}
	// Marker Skip Mode ON
	IO_JPG7.JPFORMAT.bit.MKSKIP = 1;
	// JPINTSET
	IO_JPG7.JPINTSET.word = 0;

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();

	return D_IM_JPEG_OK;
}

/**
 * @brief		Base configuration of the JPEG decoding process
 * @param[in]	T_IM_JPEG_DEC_MNG*	p_jpg_dec_mng	:Pointer to JPEG decode base management table
 * @param[out]	None
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_RUNNING_NG / D_IM_JPEG_PARAM_ERROR
 */
INT32 Im_JPEG_Ctrl_Dec( T_IM_JPEG_DEC_MNG* p_jpg_dec_mng )
{
#ifdef CO_PARAM_CHECK
	if ( p_jpg_dec_mng == NULL ) {
		Ddim_Assertion(("Im_JPEG_Ctrl_Dec(): Null check error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// F_JPEG7 macro In-use check
	if ( IO_JPG7.JPSTATUS.bit.JPSTATUS != 0 ) {
		im_jpeg_off_hclk();	// HCLK off
		Im_JPEG_Dsb();
		DDIM_User_AhbReg_SpinUnLock();
		// macro running
		return D_IM_JPEG_RUNNING_NG;
	}

	// soft reset
	im_jpeg_reset();

	// Set Jpeg base management table to a global table
	gIm_Jpeg_Dec_Mng = *p_jpg_dec_mng;

	// JPMODE
	IO_JPG7.JPMODE.word 				= 0x38;		// MMODE:000/S_ENDIAN:1/P_ENDIAN:1/DISPOSAL:1
	// JPFORMAT
	IO_JPG7.JPFORMAT.bit.EXIFMOD		= 0;	// fix
	IO_JPG7.JPFORMAT.bit.MKSKIP			= p_jpg_dec_mng->skip_mk_flg;
	// JMODE
	IO_JPG7.JMODE.bit.JMEMFORM			= p_jpg_dec_mng->mem_format;
	IO_JPG7.JMODE.bit.JPBDEXT			= p_jpg_dec_mng->ext_mode;
	IO_JPG7.JMODE.bit.JPBENDIAN			= p_jpg_dec_mng->pbuf_ctrl.endian;
	IO_JPG7.JMODE.bit.JJBENDIAN			= p_jpg_dec_mng->jbuf_ctrl.endian;
	IO_JPG7.JMODE.bit.JBURST_AL_ON		= p_jpg_dec_mng->burst_alignment;
	// JPBSTA
	switch ( p_jpg_dec_mng->pbuf_ctrl.issue_tran_num ) {
		case E_IM_JPEG_ISSUE_TRAN_2:
			IO_JPG7.JPBSTA.bit.JPISSUESET	= 0x1;
			break;
		case E_IM_JPEG_ISSUE_TRAN_4:
			IO_JPG7.JPBSTA.bit.JPISSUESET	= 0x2;
			break;
		default:
			IO_JPG7.JPBSTA.bit.JPISSUESET	= 0x3;
			break;
	}
	IO_JPG7.JPBSTA.bit.JPACACHE			= p_jpg_dec_mng->pbuf_ctrl.cache_type;
	IO_JPG7.JPBSTA.bit.JPAPROT			= p_jpg_dec_mng->pbuf_ctrl.prot_type;
	// JJBSTA
	switch ( p_jpg_dec_mng->jbuf_ctrl.issue_tran_num ) {
		case E_IM_JPEG_ISSUE_TRAN_2:
			IO_JPG7.JJBSTA.bit.JJISSUESET	= 0x1;
			break;
		case E_IM_JPEG_ISSUE_TRAN_4:
			IO_JPG7.JJBSTA.bit.JJISSUESET	= 0x2;
			break;
		default:
			IO_JPG7.JJBSTA.bit.JJISSUESET	= 0x3;
			break;
	}
	IO_JPG7.JJBSTA.bit.JJACACHE			= p_jpg_dec_mng->jbuf_ctrl.cache_type;
	IO_JPG7.JJBSTA.bit.JJAPROT			= p_jpg_dec_mng->jbuf_ctrl.prot_type;
	// Periodic interrupt setting
	// line unit
	if ( p_jpg_dec_mng->pint_line != 0 ) {
		IO_JPG7.JINTEN.bit.JLINEEN		= 1;
		IO_JPG7.JLINEVAL.bit.JLINEVAL	= p_jpg_dec_mng->pint_line;
	}
	else {
		IO_JPG7.JINTEN.bit.JLINEEN		= 0;
		IO_JPG7.JLINEVAL.bit.JLINEVAL	= 0;
	}
	// sector unit
	if (p_jpg_dec_mng->pint_sect != 0) {
		IO_JPG7.JINTEN.bit.JSECTEN		= 1;
		IO_JPG7.JSECTVAL.bit.JSECTVAL	= p_jpg_dec_mng->pint_sect;
	}
	else {
		IO_JPG7.JINTEN.bit.JSECTEN		= 0;
		IO_JPG7.JSECTVAL.bit.JSECTVAL	= 0;
	}
	// JBANDCOLOR
	IO_JPG7.JBANDCOLOR.bit.JBANDY		= p_jpg_dec_mng->color_band.y_band;
	IO_JPG7.JBANDCOLOR.bit.JBANDCB		= p_jpg_dec_mng->color_band.cb_band;
	IO_JPG7.JBANDCOLOR.bit.JBANDCR		= p_jpg_dec_mng->color_band.cr_band;
	// JCCTL
	if ( p_jpg_dec_mng->jburst_length == E_IM_JPEG_BURST_INCR_8 ) {
		IO_JPG7.JCCTL.bit.JCRP			= 0x2;
	}
	else {
		IO_JPG7.JCCTL.bit.JCRP			= 0x3;
	}
	// JICTL
	if ( p_jpg_dec_mng->pburst_length == E_IM_JPEG_BURST_INCR_8 ) {
		IO_JPG7.JICTL.bit.JIRP			= 0x2;
	}
	else {
		IO_JPG7.JICTL.bit.JIRP			= 0x3;
	}
	IO_JPG7.JICTL.bit.B12M				= E_IM_JPEG_BIT_DEPTH_8BIT;
	// JPINTSET
	if ( p_jpg_dec_mng->corr_mode ) {
		IO_JPG7.JPINTSET.word = 0x40F;
	}
	else {
		IO_JPG7.JPINTSET.word = 0x30F;
	}
	
	// IIPSET
	IO_JPG7.IIPSET.bit.PORT_MODE		= p_jpg_dec_mng->port_mode;
	IO_JPG7.IIPSET.bit.RING_ON			= p_jpg_dec_mng->ring_on;
	IO_JPG7.IIPSET.bit.BLK_SIZE			= p_jpg_dec_mng->block_size;
	IO_JPG7.IIPSET.bit.RING_SIZE		= p_jpg_dec_mng->ring_size;

	// SRAM power down control.
	IO_IMG.SRAMPD.bit.JPEGPD = 0x1;

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();

	// SRAM power down control wait time.
	Dd_ARM_Wait_ns( 1000 );

	return D_IM_JPEG_OK;
}

/**
 * @brief		Frame configuration of the JPEG decoding process
 * @param[in]	T_IM_JPEG_DEC_FRAME_MNG*	p_jpg_dec_frm_mng	:Pointer to JPEG Decode frame management table
 * @param[out]	None
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_PARAM_ERROR
 */
INT32 Im_JPEG_Ctrl_Dec_Frame( T_IM_JPEG_DEC_FRAME_MNG* p_jpg_dec_frm_mng )
{
#ifdef CO_PARAM_CHECK
	if ( p_jpg_dec_frm_mng == NULL ) {
		Ddim_Assertion(("Im_JPEG_Ctrl_Dec_Frame(): Null check error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	// Set Jpeg frame management table to a global table
	gIm_Jpeg_Dec_Frame_Mng = *p_jpg_dec_frm_mng;

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// JICTL
	IO_JPG7.JICTL.bit.JIXP		= 0;	// fix
	// JCCTL
	IO_JPG7.JCCTL.bit.JCC		= 0;	// fix
	IO_JPG7.JCCTL.bit.JCPS		= 0;	// fix
	// JIMGAY
	IO_JPG7.JIMGAY.bit.JIMGAY	= p_jpg_dec_frm_mng->ycc_addr.y;
	// JIMGACB
	IO_JPG7.JIMGACB.bit.JIMGACB	= p_jpg_dec_frm_mng->ycc_addr.c;
	// JCODA
	IO_JPG7.JCODA.bit.JCODA		= p_jpg_dec_frm_mng->code_addr;
	// JCODSV
	if ( p_jpg_dec_frm_mng->code_size == 0 ) {
		IO_JPG7.JCODSV.word	= 0;
	}
	else {
		IO_JPG7.JCODSV.word = (ULONG)(p_jpg_dec_frm_mng->code_size / D_IM_JPEG_SECT_CNT) + 1;
	}

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();

	return D_IM_JPEG_OK;
}

/**
 * @brief		Get the base settings for Jpeg decode.
 * @param[in]	None
 * @param[out]	T_IM_JPEG_DEC_MNG*	p_jpg_dec_mng	:Pointer to JPEG Decode base management table
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_PARAM_ERROR
 */
INT32	Im_JPEG_Get_Ctrl_Dec( T_IM_JPEG_DEC_MNG* p_jpg_dec_mng )
{
#ifdef CO_PARAM_CHECK
	if ( p_jpg_dec_mng == NULL ) {
		Ddim_Assertion(("Im_JPEG_Get_Ctrl_Dec(): Null check error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// JPFORMAT
	p_jpg_dec_mng->skip_mk_flg			= IO_JPG7.JPFORMAT.bit.MKSKIP;
	// JMODE
	p_jpg_dec_mng->ext_mode				= (E_IM_JPEG_RESIZE_EXT)IO_JPG7.JMODE.bit.JPBDEXT;
	p_jpg_dec_mng->pbuf_ctrl.endian		= (E_IM_JPEG_ENDIAN)IO_JPG7.JMODE.bit.JPBENDIAN;
	p_jpg_dec_mng->jbuf_ctrl.endian		= (E_IM_JPEG_ENDIAN)IO_JPG7.JMODE.bit.JJBENDIAN;
	p_jpg_dec_mng->burst_alignment		= (E_IM_JPEG_BURST_AL)IO_JPG7.JMODE.bit.JBURST_AL_ON;
	// JPBSTA
	switch ( (UCHAR)IO_JPG7.JPBSTA.bit.JPISSUESET ) {
		case 0x01:
			p_jpg_dec_mng->pbuf_ctrl.issue_tran_num = E_IM_JPEG_ISSUE_TRAN_2;
			break;
		case 0x02:
			p_jpg_dec_mng->pbuf_ctrl.issue_tran_num = E_IM_JPEG_ISSUE_TRAN_4;
			break;
		default:
			p_jpg_dec_mng->pbuf_ctrl.issue_tran_num = E_IM_JPEG_ISSUE_TRAN_8;
			break;
	}
	p_jpg_dec_mng->pbuf_ctrl.cache_type	= IO_JPG7.JPBSTA.bit.JPACACHE;
	p_jpg_dec_mng->pbuf_ctrl.prot_type	= IO_JPG7.JPBSTA.bit.JPAPROT;
	// JJBSTA
	switch ( (UCHAR)IO_JPG7.JJBSTA.bit.JJISSUESET ) {
		case 0x01:
			p_jpg_dec_mng->jbuf_ctrl.issue_tran_num = E_IM_JPEG_ISSUE_TRAN_2;
			break;
		case 0x02:
			p_jpg_dec_mng->jbuf_ctrl.issue_tran_num = E_IM_JPEG_ISSUE_TRAN_4;
			break;
		default:
			p_jpg_dec_mng->jbuf_ctrl.issue_tran_num = E_IM_JPEG_ISSUE_TRAN_8;
			break;
	}
	p_jpg_dec_mng->jbuf_ctrl.cache_type	= IO_JPG7.JJBSTA.bit.JJACACHE;
	p_jpg_dec_mng->jbuf_ctrl.prot_type	= IO_JPG7.JJBSTA.bit.JJAPROT;
	// Periodic interrupt setting
	// line unit
	p_jpg_dec_mng->pint_line			= IO_JPG7.JLINEVAL.bit.JLINEVAL;

	// sector unit
	p_jpg_dec_mng->pint_sect			= IO_JPG7.JSECTVAL.bit.JSECTVAL;

	// JBANDCOLOR
	p_jpg_dec_mng->color_band.y_band	= IO_JPG7.JBANDCOLOR.bit.JBANDY;
	p_jpg_dec_mng->color_band.cb_band	= IO_JPG7.JBANDCOLOR.bit.JBANDCB;
	p_jpg_dec_mng->color_band.cr_band	= IO_JPG7.JBANDCOLOR.bit.JBANDCR;

	// JCCTL
	if ( IO_JPG7.JCCTL.bit.JCRP == 0x2 ) {
		p_jpg_dec_mng->jburst_length	= E_IM_JPEG_BURST_INCR_8;
	}
	else {
		p_jpg_dec_mng->jburst_length	= E_IM_JPEG_BURST_INCR_16;
	}
	// JICTL
	if ( IO_JPG7.JICTL.bit.JIRP == 0x2 ) {
		p_jpg_dec_mng->pburst_length	= E_IM_JPEG_BURST_INCR_8;
	}
	else {
		p_jpg_dec_mng->pburst_length	= E_IM_JPEG_BURST_INCR_16;
	}
	// JPINTSET
	p_jpg_dec_mng->corr_mode			= IO_JPG7.JPINTSET.bit.CORSET;

	// IIPSET
	p_jpg_dec_mng->port_mode			= (E_IM_JPEG_IIP_PORT_MODE)IO_JPG7.IIPSET.bit.PORT_MODE;
	p_jpg_dec_mng->ring_on				= IO_JPG7.IIPSET.bit.RING_ON;
	p_jpg_dec_mng->block_size			= (E_IM_JPEG_IIP_BLOCK)IO_JPG7.IIPSET.bit.BLK_SIZE;
	p_jpg_dec_mng->ring_size			= IO_JPG7.IIPSET.bit.RING_SIZE;

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();

	return D_IM_JPEG_OK;
}

/**
 * @brief		Get the frame settings for Jpeg decode.
 * @param[in]	None
 * @param[out]	T_IM_JPEG_DEC_FRAME_MNG*	p_jpg_dec_frm_mng	:Pointer to JPEG Decode frame management table
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_PARAM_ERROR
 */
INT32	Im_JPEG_Get_Ctrl_Dec_Frame( T_IM_JPEG_DEC_FRAME_MNG* p_jpg_dec_frm_mng )
{
#ifdef CO_PARAM_CHECK
	if ( p_jpg_dec_frm_mng == NULL ) {
		Ddim_Assertion(("Im_JPEG_Get_Ctrl_Dec_Frame(): Null check error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// JIMGAY
	p_jpg_dec_frm_mng->ycc_addr.y	= IO_JPG7.JIMGAY.bit.JIMGAY;
	// JIMGACB
	p_jpg_dec_frm_mng->ycc_addr.c	= IO_JPG7.JIMGACB.bit.JIMGACB;
	// JCODA
	p_jpg_dec_frm_mng->code_addr	= IO_JPG7.JCODA.bit.JCODA;
	// JCODSV
	p_jpg_dec_frm_mng->code_size	= ((IO_JPG7.JCODSV.word) * (ULLONG)D_IM_JPEG_SECT_CNT);

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();

	return D_IM_JPEG_OK;
}

/**
 * @brief		JPEG Decoding process asynchronous.
 * @param[in]	None
 * @param[out]	None
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_RUNNING_NG / D_IM_JPEG_SYSTEMCALL_ERR
 */
INT32 Im_JPEG_Start_Dec( VOID )
{
	DDIM_USER_ER		ercd;

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// F_JPEG7 macro In-use check
	if ( IO_JPG7.JPSTATUS.bit.JPSTATUS != 0 ) {
		im_jpeg_off_hclk();	// HCLK off
		Im_JPEG_Dsb();
		DDIM_User_AhbReg_SpinUnLock();
		// macro running
		return D_IM_JPEG_RUNNING_NG;
	}

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();
	DDIM_User_AhbReg_SpinUnLock();

	// Initialize
	gIm_Jpeg_Dec_Mng.err_code	= 0;	// Decompression Error Code
	gIm_Jpeg_Dec_Mng.result		= 0;	// Processing result
	gIm_Jpeg_Result_Judge		= 0;
	gIm_Jpeg_Line_Cnt = 0;
	gIm_Jpeg_Sect_Cnt = 0;

	ercd = DDIM_User_Clr_Flg( FID_IM_JPEG, ~D_IM_JPEG_FLG_WAIT_END );
	if( D_DDIM_USER_E_OK != ercd ) {
		return D_IM_JPEG_SYSTEMCALL_ERR;
	}

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// interrupt setting
	IO_JPG7.JINTEN.word |= 0xC4D000;

	// JBUF start
	IO_JPG7.JCCTL.bit.JCTRG 	= 1;

	// CORE start
	IO_JPG7.JPCMD.bit.JSTART	= 1;

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();

	return D_IM_JPEG_OK;

}

/**
 * @brief		JPEG Decoding process asynchronous for marker skip mode.
 * @param[in]	None
 * @param[out]	None
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_RUNNING_NG / D_IM_JPEG_SYSTEMCALL_ERR
 */
INT32 Im_JPEG_Start_Skip_Marker_Dec( VOID )
{
	DDIM_USER_ER		ercd;

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// F_JPEG7 macro In-use check
	if ( IO_JPG7.JPSTATUS.bit.JPSTATUS != 0 ) {
		im_jpeg_off_hclk();	// HCLK off
		Im_JPEG_Dsb();
		DDIM_User_AhbReg_SpinUnLock();
		// macro running
		return D_IM_JPEG_RUNNING_NG;
	}

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();
	DDIM_User_AhbReg_SpinUnLock();

	// Initialize
	gIm_Jpeg_Dec_Mng.err_code	= 0;	// Decompression Error Code
	gIm_Jpeg_Dec_Mng.result		= 0;	// Processing result
	gIm_Jpeg_Result_Judge		= 0;
	gIm_Jpeg_Line_Cnt = 0;
	gIm_Jpeg_Sect_Cnt = 0;

	ercd = DDIM_User_Clr_Flg( FID_IM_JPEG, ~D_IM_JPEG_FLG_WAIT_END );
	if( D_DDIM_USER_E_OK != ercd ) {
		return D_IM_JPEG_SYSTEMCALL_ERR;
	}

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// interrupt setting
	IO_JPG7.JINTEN.word |= 0xC4D000;
	// JBUF start
	IO_JPG7.JCCTL.bit.JCTRG 	= 1;
	// PBUF start
	IO_JPG7.JICTL.bit.JITRG = 1;
	// CORE start
	IO_JPG7.JPCMD.bit.JSTART	= 1;


	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinLock();

	return D_IM_JPEG_OK;

}


/**
 * @brief		Waiting to be processed JPEG decoding.
 * @param[out]	T_IM_JPEG_DEC_MNG*	p_jpg_dec_mng		: Result of the JPEG decoding process
 * @param[in]	INT32				time_out			: Time-out period
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_TIMEOUT / D_IM_JPEG_DECODE_ERR / D_IM_JPEG_PARAM_ERROR / D_IM_JPEG_DECODE_PAUSE
 */
INT32 Im_JPEG_Wait_End_Dec( T_IM_JPEG_DEC_MNG* p_jpg_dec_mng, INT32 time_out )
{
	DDIM_USER_FLGPTN flgptn = 0;
	DDIM_USER_ER ercd;
	INT32 ret;

#ifdef CO_PARAM_CHECK
	if ( p_jpg_dec_mng == NULL ) {
		Ddim_Assertion(("Im_JPEG_Wait_End_Dec(): Null check error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
	if ( time_out < D_DDIM_WAIT_END_FOREVER ) {
		Ddim_Assertion(("Im_JPEG_Wait_End_Dec(): input param error. time_out = %x\n", time_out));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	ercd = DDIM_User_Twai_Flg( FID_IM_JPEG, D_IM_JPEG_FLG_WAIT_END, D_DDIM_USER_TWF_ORW, &flgptn, time_out );

	*p_jpg_dec_mng = gIm_Jpeg_Dec_Mng;

	if (ercd != D_DDIM_USER_E_OK) {
		ret = D_IM_JPEG_TIMEOUT;
	}
	else {
		// judge the result
		if ( ((p_jpg_dec_mng->result & D_IM_JPEG_JJBINT) == D_IM_JPEG_JJBINT)
			||((p_jpg_dec_mng->result & D_IM_JPEG_JPBINT) == D_IM_JPEG_JPBINT) ) {
			// bus error
			ret = D_IM_JPEG_AXI_ERR;
		}
		else if ( (p_jpg_dec_mng->result & ( D_IM_JPEG_JCLRFMER
											| D_IM_JPEG_SIZERR
											| D_IM_JPEG_RSTERR
											| D_IM_JPEG_ERR
											| D_IM_JPEG_SMPL_TYPE_NG
											| D_IM_JPEG_CUTOUT_ERR )) != 0 ) {
			ret = D_IM_JPEG_DECODE_ERR;
		}
		else if ( (p_jpg_dec_mng->result & D_IM_JPEG_PAUSE) == D_IM_JPEG_PAUSE ) {
			// pausing end or code size over
			ret = D_IM_JPEG_DECODE_PAUSE;
		}
		else {
			// encode process finished
			ret = D_IM_JPEG_OK;
		}
	}

	return ret;
}

/**
 * @brief		JPEG decode restart.(Restart from the state is paused decoding)
 * @param[in]	T_IM_JPEG_DEC_FRAME_MNG*	p_jpg_dec_frm_mng	: Pointer to JPEG Decode frame management table
 * @param[out]	None
 * @return		INT32	D_IM_JPEG_OK / D_IM_JPEG_RUNNING_NG / D_IM_JPEG_SYSTEMCALL_ERR / D_IM_JPEG_PARAM_ERROR
 */
INT32 Im_JPEG_Restart_Dec( T_IM_JPEG_DEC_FRAME_MNG* p_jpg_dec_frm_mng )
{
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if ( p_jpg_dec_frm_mng == NULL ) {
		Ddim_Assertion(("Im_JPEG_Restart_Dec(): Null check error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	if ( IO_JPG7.JCCTL.bit.JCTRG != 2) {
		im_jpeg_off_hclk();	// HCLK off
		Im_JPEG_Dsb();
		DDIM_User_AhbReg_SpinUnLock();
		// Coding not stopped
		return D_IM_JPEG_RUNNING_NG;
	}

	// Initialize result
	gIm_Jpeg_Dec_Mng.result		= 0;		// Processing result
	gIm_Jpeg_Result_Judge		= 0;

	// Set destination addr
	IO_JPG7.JCODA.bit.JCODA  = p_jpg_dec_frm_mng->code_addr;

	if ( p_jpg_dec_frm_mng->code_size == 0 ) {
		IO_JPG7.JINTEN.bit.JCPFEN = 0;
		IO_JPG7.JCODSV.word	= 0;
	}
	else {
		IO_JPG7.JINTEN.bit.JCPFEN = 1;
		IO_JPG7.JCODSV.word	= (ULONG)(((p_jpg_dec_frm_mng->code_size) / D_IM_JPEG_SECT_CNT) + 1);
	}

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();
	DDIM_User_AhbReg_SpinUnLock();

	ercd = DDIM_User_Clr_Flg( FID_IM_JPEG, ~D_IM_JPEG_FLG_WAIT_END );
	if( D_DDIM_USER_E_OK != ercd ) {
		return D_IM_JPEG_SYSTEMCALL_ERR;
	}

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	// JBUF restart
	IO_JPG7.JCCTL.bit.JCTRG 	= 1;

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();
	DDIM_User_AhbReg_SpinUnLock();

	return D_IM_JPEG_OK;
}

/**
 * @brief		Stop JPEG process.
 * @param[in]	None
 * @param[out]	None
 * @return		None
 */
VOID Im_JPEG_Stop( VOID )
{
	DDIM_User_AhbReg_SpinLock();
	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();
	im_jpeg_reset();
	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();
	DDIM_User_AhbReg_SpinUnLock();
	return;
}

/**
 * @brief		Get JPEG AXI (JBUF and PBUF) State of frame processing
 * @param[in]	None
 * @param[out]	E_IM_JPEG_AXI_ST*	p_jp_axi_state	:AXI state
 * @return		INT32	D_DDIM_OK / D_IM_JPEG_PARAM_ERROR
 */
INT32 Im_JPEG_Get_Axi_State( E_IM_JPEG_AXI_ST* p_jp_axi_state )
{
#ifdef CO_PARAM_CHECK
	if ( p_jp_axi_state == NULL ) {
		Ddim_Assertion(("Im_JPEG_Get_Axi_State(): Null check error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	if ( IO_JPG7.JSTATE.bit.JALLACTP ) {
		if ( (IO_JPG7.JSTATE.word & 0xC0) == 0xC0 ) {
			*p_jp_axi_state = E_IM_JPEG_AXI_ST_ACTIVE_BOTH;
		}
		else if ( IO_JPG7.JSTATE.bit.JJBACTP ) {
			*p_jp_axi_state = E_IM_JPEG_AXI_ST_ACTIVE_JBUF;
		}
		else {
			*p_jp_axi_state = E_IM_JPEG_AXI_ST_ACTIVE_PBUF;
		}
	}
	else {
		*p_jp_axi_state = E_IM_JPEG_AXI_ST_INACTIVE_BOTH;
	}

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();

	return D_DDIM_OK;
}

/*
 * @brief		Returns the lines of the processed image of the vertical.
 * @param[in]	None
 * @param[out]	None
 * @return		USHOUT	Number of the line counts
 */
USHORT Im_JPEG_Get_Line_Cnt( VOID )
{
	USHORT 	ret_cnt;

	// SpinLock.
	Dd_ARM_Critical_Section_Start(gIm_Jpg_Spin_Lock);
	
	ret_cnt = gIm_Jpeg_Line_Cnt;
	
	// SpinUnLock.
	Dd_ARM_Critical_Section_End(gIm_Jpg_Spin_Lock);

	return ret_cnt;
}

/*
 * @brief		Returns the lines of the processed image of the vertical.
 * @param[in]	None
 * @param[out]	None
 * @return		ULONG	Number of the sect counts
 */
ULONG Im_JPEG_Get_Sect_Cnt( VOID )
{
	ULONG ret_cnt;

	// SpinLock.
	Dd_ARM_Critical_Section_Start(gIm_Jpg_Spin_Lock);
	
	ret_cnt = gIm_Jpeg_Sect_Cnt;
	
	// SpinUnLock.
	Dd_ARM_Critical_Section_End(gIm_Jpg_Spin_Lock);


	return ret_cnt;
}

/**
 * @brief		JPEG interrupt handler.
 * @param[in]	None
 * @param[out]	None
 * @return		None
 */
VOID Im_JPEG_Int_Handler( VOID )
{
	UCHAR		enc_dec_flg		= 0;	// decode
	INT32		result			= 0;
	UCHAR		proc_end		= 0;
	UCHAR		force_end		= 0;
	UCHAR		pbuf_err		= 0;
	UCHAR		jbuf_err		= 0;
	VP_CALLBACK	pcallback		= NULL;
	INT32		ret;
	union io_jpeg7c_jint _jint;

	DDIM_User_AhbReg_SpinLock();

	im_jpeg_on_hclk();	// HCLK on
	Im_JPEG_Dsb();

	_jint.word = IO_JPG7.JINT.word;

#ifdef CO_IM_JPEG_DEBUG
	Ddim_Print(("IO_JPG7.JINT.     =[0x%08lX]\n", _jint.word));
	Ddim_Print(("IO_JPG7.JPINTSTA. =[0x%08lX]\n", IO_JPG7.JPINTSTA.word));
#endif
	if (IO_JPG7.JPMODE.bit.DISPOSAL == 0) {
	// encode
		enc_dec_flg = 1;
	}
	else {
	// decode
		enc_dec_flg = 0;
	}

	while( 1 ) {

		if ( _jint.bit.JERRINT == 1 ) {	/* pgr0872 */
		// Module error
			if ( (IO_JPG7.JINTEN.bit.JCLRFMEREN == 1) && (_jint.bit.__JCLRFMER == 1) ) {	/* pgr0872 */
			// Unsupport Format error
				// JCLRFMER bit clear
				IO_JPG7.JINT.word = 0x00040000;
				result |= D_IM_JPEG_JCLRFMER;
			}
			if ( (IO_JPG7.JINTEN.bit.JJBEN == 1) && (_jint.bit.__JJBINT == 1) ) {	/* pgr0872 */
			// JBUF error
				// Get the error factor
				jbuf_err = IO_JPG7.JJBSTA.bit.JJRES;
				// JJBINT bit clear
				IO_JPG7.JINT.word = 0x00400000;
				result |= D_IM_JPEG_JJBINT;
			}
			if ( (IO_JPG7.JINTEN.bit.JPBEN == 1) && (_jint.bit.__JPBINT == 1) ) {	/* pgr0872 */
			// PBUF error
				// Get the error factor
				pbuf_err = IO_JPG7.JPBSTA.bit.JPRES;
				// JPBINT bit clear
				IO_JPG7.JINT.word = 0x00800000;
				result |= D_IM_JPEG_JPBINT;
			}
			// soft reset
			im_jpeg_reset();
			// end of process
			force_end = 1;

			break;

		}
		if ( _jint.bit.JJIRQ != 0 ) {	/* pgr0872 */
		// Interrupt from core(F_JPEG7)
			if ( enc_dec_flg == 1 ) {
			// endcode
				// check the quant table
				im_jpeg_sub_int_handler_enc_core_qtwint( &result );

				if ( (IO_JPG7.JINT.bit.JJIRQST & 0x04) == 0x04 ) {
					// encode normal end
					im_jpeg_sub_int_handler_enc_core_end( &result );
				}
			}
			else {
				if ( (_jint.bit.JJIRQST & 0x01) == 0x01 ) {	/* pgr0872 */
					// decode error
					im_jpeg_sub_int_handler_dec_core_errint( &result );
					// end of process
					force_end = 1;
				}
				else if ( (_jint.bit.JJIRQST & 0x04) == 0x04 || IO_JPG7.JPINTSTA.bit.COREND == 1 ) {
					// decode normal end
 					im_jpeg_sub_int_handler_dec_core_end( &result );
				}
				else if ( (_jint.bit.JJIRQST & 0x02) == 0x02 ) {
					// read size enable
					ret = im_jpeg_sub_int_handler_dec_core_regrdint( &result );
					if ( ret != D_IM_JPEG_OK ) {
						// end of process
						force_end = 1;
					}
				}
				else {
					if((IO_JPG7.JPINTSTA.bit.SIZERR != 0) ||
					   (IO_JPG7.JPINTSTA.bit.RSTERR != 0)) {
						// check huffman segment error int
						im_jpeg_sub_int_handler_dec_core_segint( &result );
						// end of process
						force_end = 1;
					}
					else {
						// check marker int
						im_jpeg_sub_int_handler_dec_core_mkint( &result );
					}
				}
			}

//			break;

		}
		if ((IO_JPG7.JINTEN.bit.JIFEN == 1) && (_jint.bit.__JIF == 1)) {	/* pgr0872 */
		// Whether forwarding the image of one frame was completed is judged
			// Set result
			result |= D_IM_JPEG_JIF;
			gIm_Jpeg_Result_Judge |= D_IM_JPEG_JIF;

			// JIF bit clear
			IO_JPG7.JINT.word = 0x00008000;

			// SpinLock.
			Dd_ARM_Critical_Section_Start(gIm_Jpg_Spin_Lock);
			
			// Get processed line
			gIm_Jpeg_Line_Cnt = IO_JPG7.JISVMN.bit.JISVMN;
			
			// SpinUnLock.
			Dd_ARM_Critical_Section_End(gIm_Jpg_Spin_Lock);

#ifdef CO_IM_JPEG_DEBUG
			Ddim_Print(("IO_JPG7.JISVMN =[0x%X]\n", gIm_Jpeg_Line_Cnt));
#endif
		}
		if ((IO_JPG7.JINTEN.bit.JCFEN == 1) && (_jint.bit.__JCF == 1)) {	/* pgr0872 */
		// Whether the sign data transfer of one frame was completed is judged
			// Set result
			result |= D_IM_JPEG_JCF;
			gIm_Jpeg_Result_Judge |= D_IM_JPEG_JCF;
			// JCF bit clear
			IO_JPG7.JINT.word = 0x00004000;

			// SpinLock.
			Dd_ARM_Critical_Section_Start(gIm_Jpg_Spin_Lock);
			
			// Get processed sect
			gIm_Jpeg_Sect_Cnt = IO_JPG7.JSCTC.bit.JSCTC;
			
			// SpinUnLock.
			Dd_ARM_Critical_Section_End(gIm_Jpg_Spin_Lock);


#ifdef CO_IM_JPEG_DEBUG
			Ddim_Print(("IO_JPG7.JSCTC =[0x%lX]\n", gIm_Jpeg_Sect_Cnt));
#endif

		}
		if ((IO_JPG7.JINTEN.bit.JCPFEN == 1) && (_jint.bit.__JCPF == 1)) {	/* pgr0872 */
		// Whether the transmission of the number of sectors specified by JCODSV is completed is judged.
			// Set result
			result |= D_IM_JPEG_PAUSE;
			// JCPF bit clear
			IO_JPG7.JINT.word = 0x00001000;
			// Write code size
			gIm_Jpeg_Enc_Mng.code_size = IO_JPG7.JPBYTCNT.word - 8;	// Count value of compression volume of data
			// Process end
			force_end = 1;
		}
		if ((IO_JPG7.JINTEN.bit.JSECTEN == 1) && (_jint.bit.__JSECTINT == 1)) {	/* pgr0872 */
		// Whether the transmission of the number of sectors specified by JSECTVAL is completed is judged.
			// Set result
			result |= D_IM_JPEG_PSECTINT;
			// JSECTINT bit clear
			IO_JPG7.JINT.word = 0x00010000;
		}
		if ((IO_JPG7.JINTEN.bit.JLINEEN == 1) && (_jint.bit.__JLINEINT == 1)) {	/* pgr0872 */
		// Whether the transmission of the number of lines specified by JLINEVAL is completed is judged.
			// Set result
			result |= D_IM_JPEG_PLINEINT;
			// JLINEINT bit clear
			IO_JPG7.JINT.word = 0x00020000;
		}

		break;
	}

	im_jpeg_off_hclk();	// HCLK off
	Im_JPEG_Dsb();

	DDIM_User_AhbReg_SpinUnLock();


	if ( force_end || (gIm_Jpeg_Result_Judge & D_IM_JPEG_PROC_END) == D_IM_JPEG_PROC_END ) {
		proc_end = 1;
	}

	if (enc_dec_flg == 1) {
	//when restart, save previous result value.
		if ( jbuf_err ) {
			gIm_Jpeg_Enc_Mng.jbuf_ctrl.err_state = jbuf_err;
		}
		if ( pbuf_err ) {
			gIm_Jpeg_Enc_Mng.pbuf_ctrl.err_state = pbuf_err;
		}

		gIm_Jpeg_Enc_Mng.result |= result;

		pcallback = gIm_Jpeg_Enc_Mng.pcallback;
		if ( (pcallback != NULL) && (proc_end == 1) ) {
			((VOID(*)())pcallback)(&gIm_Jpeg_Enc_Mng);
		}
	}
	else {
	//when restart, save previous result value.
		if ( jbuf_err ) {
			gIm_Jpeg_Dec_Mng.jbuf_ctrl.err_state = jbuf_err;
		}
		if ( pbuf_err ) {
			gIm_Jpeg_Dec_Mng.pbuf_ctrl.err_state = pbuf_err;
		}

		gIm_Jpeg_Dec_Mng.result |= result;

		pcallback = gIm_Jpeg_Dec_Mng.pcallback;
		if ( (pcallback != NULL) && (proc_end == 1) ) {
			((VOID(*)())pcallback)(&gIm_Jpeg_Dec_Mng);
		}
	}

	if ( proc_end != 0 ) {
		// Flag set
		(VOID)DDIM_User_Set_Flg( FID_IM_JPEG, D_IM_JPEG_FLG_WAIT_END );
	}
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------

/**
 * @brief		The decode of jpeg
 * @param[in]	T_IM_JPEG_DEC_INPUT* input_param
 * @param[out]	T_IM_JPEG_DEC_OUTPUT* output_param
 * @return		INT32 D_DDIM_OK / D_IM_JPEG_DECODE_NG / D_IM_JPEG_PARAM_ERROR / D_IM_JPEG_DECODE_TIMEOUT / D_IM_JPEG_DECODE_HEADER_NG / D_IM_JPEG_DECODE_16BYTE_NG / D_IM_JPEG_DECODE_ENCODE_NG
 * @note		None
 * @attention	None
 */
INT32 Im_JPEG_Decode_Sync(T_IM_JPEG_DEC_INPUT* input_param, T_IM_JPEG_DEC_OUTPUT* output_param)
{
	INT32	ret = 0;
	T_IM_JPEG_DEC_MNG jpgmng;
	T_IM_JPEG_DEC_FRAME_MNG jpgfrmmng;

#ifdef CO_PARAM_CHECK
	if ( input_param == NULL ){
		// input_param is Error because of NULL
		Ddim_Print(("I:Im_JPEG_Decode_Sync. input_param = NULL\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
	if ( output_param == NULL ){
		// output_param is Error because of NULL
		Ddim_Print(("I:Im_JPEG_Decode_Sync. output_param = NULL\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
#endif

	if( (input_param->code_addr & 0x7) != 0) {
		// src address error
		Ddim_Print(("I:Im_JPEG_Decode_Sync. code address error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
	if( (input_param->dst_ycc_addr.y & 0x7) != 0) {
		// dst Y address error
		Ddim_Print(("I:Im_JPEG_Decode_Sync. dst Y address error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}
	if( (input_param->dst_ycc_addr.c & 0x7) != 0) {
		// dst Cb address error
		Ddim_Print(("I:Im_JPEG_Decode_Sync. dst Cb address error.\n"));
		return D_IM_JPEG_PARAM_ERROR;
	}

	// initialize
	memset( &jpgmng, 0, sizeof(T_IM_JPEG_DEC_MNG) );
	memset( &jpgfrmmng, 0, sizeof(T_IM_JPEG_DEC_FRAME_MNG) );
	

	// set control structure
	jpgmng.mem_format			= input_param->mem_format;
	jpgmng.ext_mode				= E_IM_JPEG_RESIZE_EXT_DIRECT;
	jpgmng.corr_mode			= D_IM_JPEG_ENABLE_ON;
	jpgmng.skip_mk_flg			= D_IM_JPEG_ENABLE_OFF;
	jpgmng.smpl_type			= input_param->smpl_type;
	jpgmng.pcallback			= NULL;

	jpgfrmmng.global_y_width	= input_param->global_y_width;
	jpgfrmmng.global_c_width	= input_param->global_c_width;
	jpgfrmmng.code_size			= input_param->code_size;
	jpgfrmmng.ycc_addr.y		= input_param->dst_ycc_addr.y;
	jpgfrmmng.ycc_addr.c		= input_param->dst_ycc_addr.c;
	jpgfrmmng.code_addr			= input_param->code_addr;

	ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
	if (ret != D_IM_JPEG_OK) {
		Ddim_Print(("Im_JPEG_Open error! ercd = %d\n", ret));
		return ret;
	}

	ret = Im_JPEG_Ctrl_Dec( &jpgmng );
	if (ret != D_IM_JPEG_OK) {
		Im_JPEG_Close();
		Ddim_Print(("Im_JPEG_Ctrl_Dec error! ercd = %d\n", ret));
		return ret;
	}

	ret = Im_JPEG_Ctrl_Dec_Frame( &jpgfrmmng );
	if (ret != D_IM_JPEG_OK) {
		Im_JPEG_Close();
		Ddim_Print(("Im_JPEG_Ctrl_Dec_Frame error! ercd = %d\n", ret));
		return ret;
	}

	ret = Im_JPEG_Start_Dec();
	if (ret != D_IM_JPEG_OK) {
		Im_JPEG_Close();
		Ddim_Print(("Im_JPEG_Start_Dec error! ercd = %d\n", ret));
		return ret;
	}

#ifdef CO_DEBUG_ON_PC
	{
		IO_JPG7.JPMODE.bit.MMODE = (UCHAR)input_param->smpl_type;

		IO_JPG7.JSTATE.bit.JALLRSTP = 1;
		IO_JPG7.JINT.word		= 0x0A000000;
		IO_JPG7.JPINTSTA.word	= 0x00000008;
		IO_JPG7.JPSUSPEND.bit.JPSUSPEND = 1;

		Im_JPEG_Int_Handler();
		// JIF
		IO_JPG7.JINT.word		= 0x8000;
		Im_JPEG_Int_Handler();
		// JCF
		IO_JPG7.JINT.word		= 0x4000;
		Im_JPEG_Int_Handler();

		IO_JPG7.JINT.word		= 0x0C000000;
		IO_JPG7.JPINTSTA.word	= 0x80000000;
		IO_JPG7.JPSTATUS.bit.JPSTATUS = 1;
		Im_JPEG_Int_Handler();
	}
#endif // CO_DEBUG_ON_PC

	ret = Im_JPEG_Wait_End_Dec( &jpgmng, D_DDIM_WAIT_END_TIME );

	output_param->smpl_type = jpgmng.smpl_type;
	output_param->err_code = jpgmng.err_code;
	output_param->org_width = jpgmng.org_width;
	output_param->org_lines = jpgmng.org_lines;

	if (ret != D_IM_JPEG_OK) {
		switch ( ret ) {
			case D_IM_JPEG_DECODE_PAUSE:
				Ddim_Print(("Im_JPEG_Start_Dec Code size over! ercd = %d\n", ret));
				break;
			case D_IM_JPEG_AXI_ERR:
				if ( (jpgmng.result & D_IM_JPEG_JJBINT) == D_IM_JPEG_JJBINT ) {
					Ddim_Print(("Im_JPEG_Start_Dec JBUF error! ercd = %d\n", jpgmng.jbuf_ctrl.err_state ));
				}
				else {
					Ddim_Print(("Im_JPEG_Start_Dec PBUF error! ercd = %d\n", jpgmng.pbuf_ctrl.err_state ));
				}
				break;
			case D_IM_JPEG_DECODE_ERR:
				if ( (jpgmng.result & D_IM_JPEG_SMPL_TYPE_NG) == D_IM_JPEG_SMPL_TYPE_NG ) {
					Ddim_Print(("Im_JPEG_Start_Dec sampling type not matching! ercd = %d\n", ret));
				}
				else if ( (jpgmng.result & D_IM_JPEG_JCLRFMER) == D_IM_JPEG_JCLRFMER ) {
					Ddim_Print(("Im_JPEG_Start_Dec unsupported format error! ercd = %d\n", ret));
				}
				else {
					Ddim_Print(("Im_JPEG_Start_Dec encode data error! ercd = %d\n", ret));
				}
				break;
			case D_IM_JPEG_TIMEOUT:
				Ddim_Print(("Im_JPEG_Start_Dec time out error! ercd = %d\n", ret));
				break;
			default:
				Ddim_Print(("Im_JPEG_Start_Dec system error! ercd = %d\n", ret));
				break;
		}
		// macro stop
		Im_JPEG_Stop();
	}

	// close
	Im_JPEG_Close();

	return ret;
}

//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE
