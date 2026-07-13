/**
 * @file		ct_im_raw.c
 * @brief		ct code for im_raw.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h> 

#include "jdsraw.h"
#include "im_raw.h"
#include "ct_im_raw.h"
#include "driver_common.h"

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
#endif

/*----------------------------------------------------------------------*/
/* Definition	    													*/
/*----------------------------------------------------------------------*/
// #define D_CT_IM_RAW_PT_TEST

#define D_CT_IM_RAW_OK		(0)
#define D_CT_IM_RAW_NG		(1)

#define D_CT_IM_RAW_WIDTH	(1536)
#define D_CT_IM_RAW_LINES	(1152)

#define D_CT_IM_RAW_8_WIDTH	(640)
#define D_CT_IM_RAW_8_LINES	(480)

/*----------------------------------------------------------------------*/
/* Global variables    													*/
/*----------------------------------------------------------------------*/
static T_IM_RAW_CTRL_CODE_TBL	gCt_Im_Raw_CodeTbl;
static T_IM_RAW_CTRL_PARAM		gCt_Im_Raw_Param;
static UCHAR					gCt_Im_Raw_User_Lut[256];


static UINT32					gCt_Im_Raw_Enc_Byte;

/*----------------------------------------------------------------------*/
/* Macro		     													*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function		     													*/
/*----------------------------------------------------------------------*/

static INT32 ct_im_raw_check_retval(INT32 retval, INT32 correct )
{
	INT32 ret;

	if( retval != correct ){
		Ddim_Print(("Return Value Error !!!!!,  retval = 0x%x\n", retval));
		ret = D_CT_IM_RAW_NG;
	}
	else{
//		Ddim_Print(("Return Value OK\n"));
		ret = D_CT_IM_RAW_OK;
	}

	return ret;
}

static VOID ct_im_raw_ret_print( INT32 ret_val )
{
	if( ret_val ){
		Ddim_Print(("Return Value Error\n"));
	}
}

#if 0
#ifndef CO_DEBUG_ON_PC
// Clear
static VOID ct_im_raw_clear( VOID )
{
	INT32 i;
	INT32 byte = 64*1024*1024;
	volatile UINT32 *ptr;

	ptr = (volatile UINT32*)D_CT_IM_RAW_ORG_ADDR;
	for ( i = 0; i < byte; i+=4 ) {
		*ptr++ = 0;
	}

	ptr = (UINT32*)D_CT_IM_RAW_ENC_ADDR;
	for( i = 0; i < byte; i+=4 ){
		*ptr++ = 0;
	}

	ptr = (UINT32*)D_CT_IM_RAW_DEC_ADDR;
	for( i = 0; i < byte; i+=4 ){
		*ptr++ = 0;
	}
}
#endif	// CO_DEBUG_ON_PC
#endif

// Dump register
static VOID ct_im_raw_dump_reg( VOID )
{
	INT32 i;
	volatile UINT32 *pLut = (volatile UINT32*)&IO_RAW.LUT.LUT1.word;
	volatile UINT32 temp;

	Im_RAW_On_Pclk();

	Ddim_Print(("RCTL0    = 0x%8lx\n", IO_RAW.RCTL0.word));
	Ddim_Print(("RCTL1    = 0x%8lx\n", IO_RAW.RCTL1.word));
	Ddim_Print(("RINTE    = 0x%8lx\n", IO_RAW.RINTE.word));
	Ddim_Print(("RINTF    = 0x%8lx\n", IO_RAW.RINTF.word));
	Ddim_Print(("COND     = 0x%8lx\n", IO_RAW.COND.word));
	Ddim_Print(("HSIZE    = 0x%8lx\n", IO_RAW.HSIZE.word));
	Ddim_Print(("VSIZE    = 0x%8lx\n", IO_RAW.VSIZE.word));
	Ddim_Print(("DSIZE    = 0x%8lx\n", IO_RAW.DSIZE.word));
	Ddim_Print(("MSA      = 0x%8lx\n", IO_RAW.MSA.word));
	Ddim_Print(("MDA      = 0x%8lx\n", IO_RAW.MDA.word));
	Ddim_Print(("DEFINIT  = 0x%8lx\n", IO_RAW.DEFINIT.word));
	Ddim_Print(("CLMT     = 0x%8lx\n", IO_RAW.CLMT.word));
	Ddim_Print(("CCNT     = 0x%8lx\n", IO_RAW.CCNT.word));
	Ddim_Print(("MWAXCTL  = 0x%8lx\n", IO_RAW.MRAXCTL.word));
	Ddim_Print(("MWAXCTL  = 0x%8lx\n", IO_RAW.MWAXCTL.word));
	Ddim_Print(("MRAXSTS  = 0x%8lx\n", IO_RAW.MRAXSTS.word));
	Ddim_Print(("MWAXSTS  = 0x%8lx\n", IO_RAW.MWAXSTS.word));

	Ddim_Print(("CODE0 = 0x%04X\n", IO_RAW.CODE.CODE1.bit.CODE0));
	Ddim_Print(("CODE1 = 0x%04X\n", IO_RAW.CODE.CODE1.bit.CODE1));
	Ddim_Print(("CODE2 = 0x%04X\n", IO_RAW.CODE.CODE2.bit.CODE2));
	Ddim_Print(("CODE3 = 0x%04X\n", IO_RAW.CODE.CODE2.bit.CODE3));
	Ddim_Print(("CODE4 = 0x%04X\n", IO_RAW.CODE.CODE3.bit.CODE4));
	Ddim_Print(("CODE5 = 0x%04X\n", IO_RAW.CODE.CODE3.bit.CODE5));
	Ddim_Print(("CODE6 = 0x%04X\n", IO_RAW.CODE.CODE4.bit.CODE6));
	Ddim_Print(("CODE7 = 0x%04X\n", IO_RAW.CODE.CODE4.bit.CODE7));
	Ddim_Print(("CODE8 = 0x%04X\n", IO_RAW.CODE.CODE5.bit.CODE8));
	Ddim_Print(("CODE9 = 0x%04X\n", IO_RAW.CODE.CODE5.bit.CODE9));
	Ddim_Print(("CODEA = 0x%04X\n", IO_RAW.CODE.CODE6.bit.CODEA));
	Ddim_Print(("CODEB = 0x%04X\n", IO_RAW.CODE.CODE6.bit.CODEB));
	Ddim_Print(("CODEC = 0x%04X\n", IO_RAW.CODE.CODE7.bit.CODEC));
	Ddim_Print(("CODED = 0x%04X\n", IO_RAW.CODE.CODE7.bit.CODED));
	Ddim_Print(("CODEE = 0x%04X\n", IO_RAW.CODE.CODE8.bit.CODEE));

	Ddim_Print(("LEN0 = 0x%04X\n", IO_RAW.LEN.LEN1.bit.LEN0));
	Ddim_Print(("LEN1 = 0x%04X\n", IO_RAW.LEN.LEN1.bit.LEN1));
	Ddim_Print(("LEN2 = 0x%04X\n", IO_RAW.LEN.LEN2.bit.LEN2));
	Ddim_Print(("LEN3 = 0x%04X\n", IO_RAW.LEN.LEN2.bit.LEN3));
	Ddim_Print(("LEN4 = 0x%04X\n", IO_RAW.LEN.LEN3.bit.LEN4));
	Ddim_Print(("LEN5 = 0x%04X\n", IO_RAW.LEN.LEN3.bit.LEN5));
	Ddim_Print(("LEN6 = 0x%04X\n", IO_RAW.LEN.LEN4.bit.LEN6));
	Ddim_Print(("LEN7 = 0x%04X\n", IO_RAW.LEN.LEN4.bit.LEN7));
	Ddim_Print(("LEN8 = 0x%04X\n", IO_RAW.LEN.LEN5.bit.LEN8));
	Ddim_Print(("LEN9 = 0x%04X\n", IO_RAW.LEN.LEN5.bit.LEN9));
	Ddim_Print(("LENA = 0x%04X\n", IO_RAW.LEN.LEN6.bit.LENA));
	Ddim_Print(("LENB = 0x%04X\n", IO_RAW.LEN.LEN6.bit.LENB));
	Ddim_Print(("LENC = 0x%04X\n", IO_RAW.LEN.LEN7.bit.LENC));
	Ddim_Print(("LEND = 0x%04X\n", IO_RAW.LEN.LEN7.bit.LEND));
	Ddim_Print(("LENE = 0x%04X\n", IO_RAW.LEN.LEN8.bit.LENE));

	Ddim_Print(("CLMT = 0x%08lX\n", IO_RAW.CLMT.word));
	Ddim_Print(("CCNT = 0x%08lX\n", IO_RAW.CCNT.word));

	Ddim_Print(("LUT ADDR = 0x%08X\n", (UINT32)pLut));

	for( i = 0; i < 256; i+=4 ){
		temp = *pLut++;
		Ddim_Print(("0x%08X ", (UINT32)temp));
		Ddim_Print(("LUT[%3d-%3d] | 0x%2X | 0x%2X | 0x%2X | 0x%2X |\n", i, (i+3),
					(UCHAR)(temp >> 0), (UCHAR)(temp >> 8), (UCHAR)(temp >> 16), (UCHAR)(temp >> 24)));
	}

	Im_RAW_Off_Pclk();
}

// callback
VOID sample_raw_callback( UINT32 cond )
{
	Ddim_Print(("Int : COND = %d\n", cond));
}

// User setting Huffman table
static VOID ct_im_raw_set_huff_tbl(VOID)
{
	gCt_Im_Raw_CodeTbl.code[0]  = 0xD000;
	gCt_Im_Raw_CodeTbl.code[1]  = 0xD800;
	gCt_Im_Raw_CodeTbl.code[2]  = 0xF000;
	gCt_Im_Raw_CodeTbl.code[3]  = 0xC000;
	gCt_Im_Raw_CodeTbl.code[4]  = 0x0000;
	gCt_Im_Raw_CodeTbl.code[5]  = 0x8000;
	gCt_Im_Raw_CodeTbl.code[6]  = 0x4000;
	gCt_Im_Raw_CodeTbl.code[7]  = 0x2000;
	gCt_Im_Raw_CodeTbl.code[8]  = 0xE000;
	gCt_Im_Raw_CodeTbl.code[9]  = 0xD400;
	gCt_Im_Raw_CodeTbl.code[10] = 0xD600;
	gCt_Im_Raw_CodeTbl.code[11] = 0xD700;
	gCt_Im_Raw_CodeTbl.code[12] = 0xD780;
	gCt_Im_Raw_CodeTbl.code[13] = 0xD7E0;
	gCt_Im_Raw_CodeTbl.code[14] = 0xD7C0;

	gCt_Im_Raw_CodeTbl.code_length[0]  = 6;
	gCt_Im_Raw_CodeTbl.code_length[1]  = 5;
	gCt_Im_Raw_CodeTbl.code_length[2]  = 4;
	gCt_Im_Raw_CodeTbl.code_length[3]  = 4;
	gCt_Im_Raw_CodeTbl.code_length[4]  = 3;
	gCt_Im_Raw_CodeTbl.code_length[5]  = 2;
	gCt_Im_Raw_CodeTbl.code_length[6]  = 2;
	gCt_Im_Raw_CodeTbl.code_length[7]  = 3;
	gCt_Im_Raw_CodeTbl.code_length[8]  = 4;
	gCt_Im_Raw_CodeTbl.code_length[9]  = 7;
	gCt_Im_Raw_CodeTbl.code_length[10] = 8;
	gCt_Im_Raw_CodeTbl.code_length[11] = 9;
	gCt_Im_Raw_CodeTbl.code_length[12] = 10;
	gCt_Im_Raw_CodeTbl.code_length[13] = 11;
	gCt_Im_Raw_CodeTbl.code_length[14] = 11;
}

// init global param
static VOID ct_im_raw_init_param( VOID )
{
	// init gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ORG_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.diff_fixed_value = 0;
	gCt_Im_Raw_Param.byte_stuffing = D_IM_RAW_OFF;
	gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_14_OR_16_BIT;
	gCt_Im_Raw_Param.callback = sample_raw_callback;
	gCt_Im_Raw_Param.mcu_size = D_IM_RAW_DEFFC_MCU_2x2;
	gCt_Im_Raw_Param.bit_select = D_IM_RAW_BITSEL_EXCEPT_16_BIT;
	gCt_Im_Raw_Param.width = 0;
	gCt_Im_Raw_Param.lines = 0;
	gCt_Im_Raw_Param.diff_mode = D_IM_RAW_DEFOP_DIFF_PREV_VALUE;
	gCt_Im_Raw_Param.lut = NULL;
	gCt_Im_Raw_Param.data_size = 0;
	gCt_Im_Raw_Param.axi_param.r_cache_type      = D_IM_RAW_ON_CACHE_RW_WRITE_BACK;		// MRAXCTL
	gCt_Im_Raw_Param.axi_param.r_protection_type = D_IM_RAW_D_SEC_NORMAL_ACCESS;		// MRAXCTL
	gCt_Im_Raw_Param.axi_param.w_cache_type      = D_IM_RAW_ON_CACHE_RW_WRITE_BACK;		// MWAXCTL
	gCt_Im_Raw_Param.axi_param.w_protection_type = D_IM_RAW_D_SEC_NORMAL_ACCESS;		// MWAXCTL
	// init gCt_Im_Raw_CodeTbl.code
	gCt_Im_Raw_CodeTbl.code[0]  = 0x6000;
	gCt_Im_Raw_CodeTbl.code[1]  = 0x7000;
	gCt_Im_Raw_CodeTbl.code[2]  = 0x0000;
	gCt_Im_Raw_CodeTbl.code[3]  = 0x2000;
	gCt_Im_Raw_CodeTbl.code[4]  = 0x4000;
	gCt_Im_Raw_CodeTbl.code[5]  = 0x8000;
	gCt_Im_Raw_CodeTbl.code[6]  = 0x9000;
	gCt_Im_Raw_CodeTbl.code[7]  = 0xA000;
	gCt_Im_Raw_CodeTbl.code[8]  = 0xB000;
	gCt_Im_Raw_CodeTbl.code[9]  = 0xC000;
	gCt_Im_Raw_CodeTbl.code[10] = 0xD000;
	gCt_Im_Raw_CodeTbl.code[11] = 0xE000;
	gCt_Im_Raw_CodeTbl.code[12] = 0xE800;
	gCt_Im_Raw_CodeTbl.code[13] = 0xF000;
	gCt_Im_Raw_CodeTbl.code[14] = 0xF800;
	// init gCt_Im_Raw_CodeTbl.code_length
	gCt_Im_Raw_CodeTbl.code_length[0]  = 4;
	gCt_Im_Raw_CodeTbl.code_length[1]  = 4;
	gCt_Im_Raw_CodeTbl.code_length[2]  = 3;
	gCt_Im_Raw_CodeTbl.code_length[3]  = 3;
	gCt_Im_Raw_CodeTbl.code_length[4]  = 3;
	gCt_Im_Raw_CodeTbl.code_length[5]  = 4;
	gCt_Im_Raw_CodeTbl.code_length[6]  = 4;
	gCt_Im_Raw_CodeTbl.code_length[7]  = 4;
	gCt_Im_Raw_CodeTbl.code_length[8]  = 4;
	gCt_Im_Raw_CodeTbl.code_length[9]  = 4;
	gCt_Im_Raw_CodeTbl.code_length[10] = 4;
	gCt_Im_Raw_CodeTbl.code_length[11] = 5;
	gCt_Im_Raw_CodeTbl.code_length[12] = 5;
	gCt_Im_Raw_CodeTbl.code_length[13] = 5;
	gCt_Im_Raw_CodeTbl.code_length[14] = 5;
}

// setup invalid code
static VOID ct_im_raw_invalid_code( VOID )
{
	gCt_Im_Raw_CodeTbl.code[0]  = 0xFFFC;
	gCt_Im_Raw_CodeTbl.code[1]  = 0xFFFC;
	gCt_Im_Raw_CodeTbl.code[2]  = 0xFFFC;
	gCt_Im_Raw_CodeTbl.code[3]  = 0xFFFC;
	gCt_Im_Raw_CodeTbl.code[4]  = 0xFFFC;
	gCt_Im_Raw_CodeTbl.code[5]  = 0xFFFC;
	gCt_Im_Raw_CodeTbl.code[6]  = 0xFFFC;
	gCt_Im_Raw_CodeTbl.code[7]  = 0xFFFC;
	gCt_Im_Raw_CodeTbl.code[8]  = 0xFFFC;
	gCt_Im_Raw_CodeTbl.code[9]  = 0xFFFC;
	gCt_Im_Raw_CodeTbl.code[10] = 0xFFFC;
	gCt_Im_Raw_CodeTbl.code[11] = 0xFFFC;
	gCt_Im_Raw_CodeTbl.code[12] = 0xFFFC;
	gCt_Im_Raw_CodeTbl.code[13] = 0xFFFC;
	gCt_Im_Raw_CodeTbl.code[14] = 0xFFFC;

	gCt_Im_Raw_CodeTbl.code_length[0]  = 14;
	gCt_Im_Raw_CodeTbl.code_length[1]  = 14;
	gCt_Im_Raw_CodeTbl.code_length[2]  = 14;
	gCt_Im_Raw_CodeTbl.code_length[3]  = 14;
	gCt_Im_Raw_CodeTbl.code_length[4]  = 14;
	gCt_Im_Raw_CodeTbl.code_length[5]  = 14;
	gCt_Im_Raw_CodeTbl.code_length[6]  = 14;
	gCt_Im_Raw_CodeTbl.code_length[7]  = 14;
	gCt_Im_Raw_CodeTbl.code_length[8]  = 14;
	gCt_Im_Raw_CodeTbl.code_length[9]  = 14;
	gCt_Im_Raw_CodeTbl.code_length[10] = 14;
	gCt_Im_Raw_CodeTbl.code_length[11] = 14;
	gCt_Im_Raw_CodeTbl.code_length[12] = 14;
	gCt_Im_Raw_CodeTbl.code_length[13] = 14;
	gCt_Im_Raw_CodeTbl.code_length[14] = 14;
}

// setup 8bit code table
static VOID ct_im_raw_8bit_code_table( VOID )
{
	gCt_Im_Raw_CodeTbl.code[0]  = 0x6000;
	gCt_Im_Raw_CodeTbl.code[1]  = 0x4000;
	gCt_Im_Raw_CodeTbl.code[2]  = 0x0000;
	gCt_Im_Raw_CodeTbl.code[3]  = 0x8000;
	gCt_Im_Raw_CodeTbl.code[4]  = 0xc000;
	gCt_Im_Raw_CodeTbl.code[5]  = 0x2000;
	gCt_Im_Raw_CodeTbl.code[6]  = 0x3000;
	gCt_Im_Raw_CodeTbl.code[7]  = 0x3800;
	gCt_Im_Raw_CodeTbl.code[8]  = 0x3c00;
	gCt_Im_Raw_CodeTbl.code[9]  = 0x3ec0;
	gCt_Im_Raw_CodeTbl.code[10] = 0x3e80;
	gCt_Im_Raw_CodeTbl.code[11] = 0x3e40;
	gCt_Im_Raw_CodeTbl.code[12] = 0x3e00;
	gCt_Im_Raw_CodeTbl.code[13] = 0x3f80;
	gCt_Im_Raw_CodeTbl.code[14] = 0x3f00;

	gCt_Im_Raw_CodeTbl.code_length[0]  = 3;
	gCt_Im_Raw_CodeTbl.code_length[1]  = 3;
	gCt_Im_Raw_CodeTbl.code_length[2]  = 3;
	gCt_Im_Raw_CodeTbl.code_length[3]  = 2;
	gCt_Im_Raw_CodeTbl.code_length[4]  = 2;
	gCt_Im_Raw_CodeTbl.code_length[5]  = 4;
	gCt_Im_Raw_CodeTbl.code_length[6]  = 5;
	gCt_Im_Raw_CodeTbl.code_length[7]  = 6;
	gCt_Im_Raw_CodeTbl.code_length[8]  = 7;
	gCt_Im_Raw_CodeTbl.code_length[9]  = 10;
	gCt_Im_Raw_CodeTbl.code_length[10] = 10;
	gCt_Im_Raw_CodeTbl.code_length[11] = 10;
	gCt_Im_Raw_CodeTbl.code_length[12] = 10;
	gCt_Im_Raw_CodeTbl.code_length[13] = 9;
	gCt_Im_Raw_CodeTbl.code_length[14] = 9;
}

// setup User LUT
static VOID ct_im_raw_setup_user_lut0( VOID )
{
	INT32 i;

	for( i = 0; i < 256; i++ ){
		gCt_Im_Raw_User_Lut[i] = 255 - i;
	}
}

// setup User LUT
static VOID ct_im_raw_setup_user_lut1( VOID )
{
	INT32 i;

	for( i = 0; i < 256; i++ ){
		gCt_Im_Raw_User_Lut[i] = i;
	}
}

// print out enc
static VOID ct_im_raw_enc_result( INT32 retval, UINT32 cond, UINT32 byte, USHORT width, USHORT lines, USHORT bit_mode)
{
	if( ( retval == D_IM_RAW_RETVAL_OK ) && ( cond == D_IM_RAW_COND_OK ) ){
		if( bit_mode == 0 ){
			Ddim_Print(("Org data = %ld byte\n", (ULONG)((ULONG)width * 2 * (ULONG)lines)));
		}
		else{
			Ddim_Print(("Org data = %ld byte\n", (ULONG)((ULONG)width * 1.5 * (ULONG)lines)));
		}
		Ddim_Print(("Enc data = %d byte\n", byte));
	}
}

// verify raw
static INT32 ct_im_raw_verify_raw( USHORT* addr1, USHORT* addr2, UINT32 byte, UCHAR bitsel )
{
	INT32 	i;
	USHORT 	mask = (bitsel == D_IM_RAW_BITSEL_EXCEPT_16_BIT) ? 0x3FFF : 0xFFFC;

	for( i = 0; i < byte; i+=2 ){
		if( (mask & (*addr1)) != (mask & (*addr2)) ){
			return D_CT_IM_RAW_NG;
		}
		addr1++;
		addr2++;
	}

	return D_CT_IM_RAW_OK;
}

static INT32 ct_im_raw_verify_raw_12bitpack( USHORT* addr1, BYTE* addr2, UINT32 byte)
{
	INT32 	i;
	BYTE	tmp1;
	BYTE	tmp2;
	USHORT	dec_val;
	USHORT	org_val;

	for( i=0; i<byte; i+=3 ){
		org_val = (*addr1);
		tmp1 = ((*(addr2+1)) & 0x0F);
		tmp2 = (*addr2);
		dec_val = (USHORT)tmp2 + ((USHORT)tmp1 << 8);

		if( (0x3FFF & org_val) != (0x3FFF & dec_val) ){
			Ddim_Print(("Org = %d, Dec = %d\n", org_val, dec_val));
			return D_CT_IM_RAW_NG;
		}

		org_val = (*(addr1+1));
		tmp1 = ((*(addr2+1)) >> 4);
		tmp2 = (*(addr2+2));
		dec_val = ((USHORT)tmp2 << 4) + (USHORT)tmp1;

		if( (0x3FFF & org_val) != (0x3FFF & dec_val) ){
			Ddim_Print(("Org = %d, Dec = %d\n", org_val, dec_val));
			return D_CT_IM_RAW_NG;
		}

		addr1 += 2;		// +2 Byte
		addr2 += 3;		// +3 Byte
	}

	return D_CT_IM_RAW_OK;
}



// verify 8bit data
static INT32 ct_im_raw_verify_data_LUT( UCHAR* addr1, UCHAR* addr2, UINT32 byte )
{
	INT32 	i;

	for( i = 0; i < byte; i++ ){
		if( (gCt_Im_Raw_User_Lut[*addr1]) != (*addr2) ){
			return D_CT_IM_RAW_NG;
		}
		addr1++;
		addr2++;
	}

	return D_CT_IM_RAW_OK;
}

// verify 8bit data with LUT
static INT32 ct_im_raw_verify_data( UCHAR* addr1, UCHAR* addr2, UINT32 byte )
{
	INT32 	i;

	for( i = 0; i < byte; i++ ){
		if( (*addr1) != (*addr2) ){
			return D_CT_IM_RAW_NG;
		}
		addr1++;
		addr2++;
	}

	return D_CT_IM_RAW_OK;
}

/*----------------------------------------------------------------------*/
/* Test code	     													*/
/*----------------------------------------------------------------------*/
// Init
static INT32 ct_im_raw_test_init( VOID )
{
	INT32	retval;
	INT32	ret;

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_PCLOCK)
	Dd_Top_Set_CLKSTOP7_RAWCK(0);
	Dd_Top_Set_CLKSTOP7_RAWAX(0);
	Dd_Top_Set_CLKSTOP7_RAWAP(0);
#endif

	DDIM_User_Dly_Tsk(1);

	IO_RAW.RCTL0.word = 1;	// Reset

	DDIM_User_Dly_Tsk(1);

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_PCLOCK)
	Dd_Top_Set_CLKSTOP7_RAWAP(1);
	Dd_Top_Set_CLKSTOP7_RAWAX(1);
	Dd_Top_Set_CLKSTOP7_RAWCK(1);
#endif

#ifdef CO_DEBUG_ON_PC
	IO_RAW.RCTL1.bit.RTRG = 2;
#endif	//CO_DEBUG_ON_PC

	retval = Im_RAW_Init();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		Ddim_Print(("Im_RAW_Init() Error !!!!!,  retval = 0x%x\n", retval));
		return D_CT_IM_RAW_NG;
	}

	// Dd_GIC_Ctrl( E_DD_GIC_INTID_RAW_INT, 1, D_DD_GIC_PRI30, 0 );

	return D_CT_IM_RAW_OK;
}

// Open( pol_sem )
static INT32 ct_im_raw_test1_0( VOID )
{
	INT32 	retval;
	INT32	ret;

	Ddim_Print(("raw test1_0 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_0 porocess 2\n"));
	retval = Im_RAW_Close();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return ret;
}

// Open/Close( twai_sem )
static INT32 ct_im_raw_test1_1( VOID )
{
	INT32 	retval;
	INT32	ret;

	Ddim_Print(("raw test1_1 porocess 1\n"));
	retval = Im_RAW_Open( 100 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_1 porocess 2\n"));
	retval = Im_RAW_Close();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return D_CT_IM_RAW_OK;
}

// Open( parameter error )
static INT32 ct_im_raw_test1_2( VOID )
{
	INT32 	retval;
	INT32	ret;

	Ddim_Print(("raw test1_2 porocess 1\n"));
	retval = Im_RAW_Open( -2 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	return D_CT_IM_RAW_OK;
}

// Ctrl_CodeTbl( normal end )
static INT32 ct_im_raw_test1_3( VOID )
{
	INT32 	retval;
	INT32	ret;

	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return ret;
}

// Ctrl_CodeTbl( parameter error )
static INT32 ct_im_raw_test1_4( VOID )
{
	INT32 	retval;
	INT32	ret;

	retval = Im_RAW_Ctrl_CodeTbl( NULL, gCt_Im_Raw_Param.data_format );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, 3 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	return D_CT_IM_RAW_OK;
}

// Ctrl_CodeTbl( macro busy )
static INT32 ct_im_raw_test1_5( VOID )
{
	INT32 	retval;
	INT32	ret;

	IO_RAW.RCTL1.bit.RTRG = 1;
#ifdef CO_DEBUG_ON_PC
	IO_RAW.RCTL1.bit.RTRG = 3;
#endif

	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	return D_CT_IM_RAW_OK;
}

// Ctrl_Enc( normal end )
static INT32 ct_im_raw_test1_6( VOID )
{
	INT32 	retval;
	INT32	ret;

	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return ret;
}

// Ctrl_Enc( parameter error )
static INT32 ct_im_raw_test1_7( VOID )
{
	INT32 	retval;
	INT32	ret;

	retval = Im_RAW_Ctrl_Enc( NULL, 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	return D_CT_IM_RAW_OK;
}

// Ctrl_Enc( macro busy )
static INT32 ct_im_raw_test1_8( VOID )
{
	INT32 	retval;
	INT32	ret;

	IO_RAW.RCTL1.bit.RTRG = 1;
#ifdef CO_DEBUG_ON_PC
	IO_RAW.RCTL1.bit.RTRG = 3;
#endif

	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	return D_CT_IM_RAW_OK;
}

// Ctrl_Dec( normal end )
static INT32 ct_im_raw_test1_9( VOID )
{
	INT32 	retval;
	INT32	ret;

	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return ret;
}

// Ctrl_Dec( parameter error )
static INT32 ct_im_raw_test1_10( VOID )
{
	INT32 	retval;
	INT32	ret;

	retval = Im_RAW_Ctrl_Dec( NULL );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	return D_CT_IM_RAW_OK;
}

// Ctrl_Dec( macro busy )
static INT32 ct_im_raw_test1_11( VOID )
{
	INT32 	retval;
	INT32	ret;

	IO_RAW.RCTL1.bit.RTRG = 1;
#ifdef CO_DEBUG_ON_PC
	IO_RAW.RCTL1.bit.RTRG = 3;
#endif

	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	return D_CT_IM_RAW_OK;
}

// Start( normal end )
static INT32 ct_im_raw_test1_12( VOID )
{
	INT32 	retval;
	INT32	ret;
	UINT32	condition, byte;

	Ddim_Print(("raw test1_12 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_12 porocess 2\n"));
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_12 porocess 3\n"));
	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_12 porocess 4\n"));
	retval = Im_RAW_Start();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

#ifdef CO_DEBUG_ON_PC
	// normal end
	DDIM_User_Set_Flg( FID_IM_RAW, 0x00000001 );
#endif

	Ddim_Print(("raw test1_12 porocess 5\n"));
	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_12 porocess 6\n"));
	retval = Im_RAW_Close();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return ret;
}

// Start( system error )
static INT32 ct_im_raw_test1_13( VOID )
{
	INT32 	retval;
	INT32	ret;
#if 0	// because not start
	UINT32	condition, byte;
#endif	// because not start
	UCHAR	tmp_fid;

	Ddim_Print(("raw test1_13 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_13 porocess 2\n"));
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_13 porocess 3\n"));
	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	tmp_fid = FID_IM_RAW;
	FID_IM_RAW = 0xFF;

	Ddim_Print(("raw test1_13 porocess 4\n"));
	retval = Im_RAW_Start();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	FID_IM_RAW = tmp_fid;

#if 0	// because not start
	Ddim_Print(("raw test1_13 porocess 5\n"));
	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}
#endif	// because not start

	Ddim_Print(("raw test1_13 porocess 6\n"));
	retval = Im_RAW_Close();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return ret;
}

// Start( macro busy )
static INT32 ct_im_raw_test1_14( VOID )
{
	INT32 	retval;
	INT32	ret;
	UINT32	condition, byte;

	Ddim_Print(("raw test1_14 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_14 porocess 2\n"));
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_14 porocess 3\n"));
	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_14 porocess 4\n"));
	retval = Im_RAW_Start();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

#ifdef CO_DEBUG_ON_PC
	IO_RAW.RCTL1.bit.RTRG = 3;
#endif

	Ddim_Print(("raw test1_14 porocess 5\n"));
	retval = Im_RAW_Start();	// Im_RAW_Start() retry
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

#ifdef CO_DEBUG_ON_PC
	// normal end
	DDIM_User_Set_Flg( FID_IM_RAW, 0x00000001 );
#endif

	Ddim_Print(("raw test1_14 porocess 6\n"));
	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_14 porocess 7\n"));
	retval = Im_RAW_Close();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return ret;
}

// Wait_End_Enc( normal end )
static INT32 ct_im_raw_test1_15( VOID )
{
	INT32 	retval;
	INT32	ret;
	UINT32	condition, byte;

	Ddim_Print(("raw test1_15 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_15 porocess 2\n"));
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_15 porocess 3\n"));
	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_15 porocess 4\n"));
	retval = Im_RAW_Start();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

#ifdef CO_DEBUG_ON_PC
	// normal end
	DDIM_User_Set_Flg( FID_IM_RAW, 0x00000001 );
#endif

	Ddim_Print(("raw test1_15 porocess 5\n"));
	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_15 porocess 6\n"));
	retval = Im_RAW_Close();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return ret;
}

// Wait_End_Enc( parameter error )
static INT32 ct_im_raw_test1_16( VOID )
{
	INT32 	retval;
	INT32	ret;
	UINT32	condition, byte;

	retval = Im_RAW_Wait_End_Enc( NULL, &byte, 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	retval = Im_RAW_Wait_End_Enc( &condition, NULL, 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	retval = Im_RAW_Wait_End_Enc( &condition, &byte, -3 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	return D_CT_IM_RAW_OK;
}

// Wait_End_Enc( system error )
static INT32 ct_im_raw_test1_17( VOID )
{
	INT32 	retval;
	INT32	ret;
	UINT32	condition, byte;
	UCHAR	tmp_fid;

	Ddim_Print(("raw test1_17 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_17 porocess 2\n"));
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_17 porocess 3\n"));
	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_17 porocess 4\n"));
	retval = Im_RAW_Start();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	tmp_fid = FID_IM_RAW;
	FID_IM_RAW = 0xFF;

	Ddim_Print(("raw test1_17 porocess 5\n"));
	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	FID_IM_RAW = tmp_fid;

	Ddim_Print(("raw test1_17 porocess 6\n"));
	retval = Im_RAW_Close();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return ret;
}

// Wait_End_Enc( R_AXI error )
static INT32 ct_im_raw_test1_18( VOID )
{
	INT32 	retval;
	INT32	ret;
	UINT32	condition, byte;

	DDIM_User_Set_Flg( FID_IM_RAW, 0x00000004 );
	Ddim_Print(( "Set_Flg : R_AXI_ERR\n" ));

	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	return D_CT_IM_RAW_OK;
}

// Wait_End_Enc( RW_AXI error )
static INT32 ct_im_raw_test1_19( VOID )
{
	INT32 	retval;
	INT32	ret;
	UINT32	condition, byte;

	DDIM_User_Set_Flg( FID_IM_RAW, 0x00000008 );
	Ddim_Print(( "Set_Flg : W_AXI_ERR\n" ));

	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	return D_CT_IM_RAW_OK;
}


// Wait_End_Enc( force stop )
static INT32 ct_im_raw_test1_20( VOID )
{
	INT32 	retval;
	INT32	ret;

	Ddim_Print(("raw test1_20 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_20 porocess 2\n"));
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_20 porocess 3\n"));
	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_20 porocess 4\n"));
	retval = Im_RAW_Start();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

#ifdef CO_DEBUG_ON_PC
	IO_RAW.RCTL1.bit.RTRG = 3;
#endif

	Ddim_Print(("raw test1_20 porocess 5\n"));
	retval = Im_RAW_Stop();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_20 porocess 6\n"));
	retval = Im_RAW_Close();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return ret;
}

// Wait_End_Dec( normal end )
static INT32 ct_im_raw_test1_21( VOID )
{
	INT32 	retval;
	INT32	ret;
	UINT32	condition;

	Ddim_Print(("raw test1_21 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_21 porocess 2\n"));
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_21 porocess 3\n"));
	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_21 porocess 4\n"));
	retval = Im_RAW_Start();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

#ifdef CO_DEBUG_ON_PC
	// normal end
	DDIM_User_Set_Flg( FID_IM_RAW, 0x00000001 );
#endif

	Ddim_Print(("raw test1_21 porocess 5\n"));
	retval = Im_RAW_Wait_End_Dec( &condition, 1000 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_21 porocess 6\n"));
	retval = Im_RAW_Close();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return ret;
}

// Wait_End_Dec( parameter error )
static INT32 ct_im_raw_test1_22( VOID )
{
	INT32 	retval;
	INT32	ret;
	UINT32	condition;

	retval = Im_RAW_Wait_End_Dec( NULL, 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	retval = Im_RAW_Wait_End_Dec( &condition, -100 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	return D_CT_IM_RAW_OK;
}

// Wait_End_Dec( system error )
static INT32 ct_im_raw_test1_23( VOID )
{
	INT32 	retval;
	INT32	ret;
	UINT32	condition;
	UCHAR	tmp_fid;

	Ddim_Print(("raw test1_23 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_23 porocess 2\n"));
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_23 porocess 3\n"));
	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_23 porocess 4\n"));
	retval = Im_RAW_Start();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	tmp_fid = FID_IM_RAW;
	FID_IM_RAW = 0xFF;

	Ddim_Print(("raw test1_23 porocess 5\n"));
	retval = Im_RAW_Wait_End_Dec( &condition, 1000 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	FID_IM_RAW = tmp_fid;

	Ddim_Print(("raw test1_23 porocess 6\n"));
	retval = Im_RAW_Close();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return ret;
}

// Wait_End_Dec( R_AXI error )
static INT32 ct_im_raw_test1_24( VOID )
{
	INT32 	retval;
	INT32	ret;
	UINT32	condition;

	DDIM_User_Set_Flg( FID_IM_RAW, 0x00000004 );
	Ddim_Print(( "Set_Flg : R_AXI_ERR\n" ));

	retval = Im_RAW_Wait_End_Dec( &condition, 1000 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	return D_CT_IM_RAW_OK;
}

// Wait_End_Dec( W_AXI error )
static INT32 ct_im_raw_test1_25( VOID )
{
	INT32 	retval;
	INT32	ret;
	UINT32	condition;

	DDIM_User_Set_Flg( FID_IM_RAW, 0x00000008 );
	Ddim_Print(( "Set_Flg : W_AXI_ERR\n" ));

	retval = Im_RAW_Wait_End_Dec( &condition, 1000 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	return D_CT_IM_RAW_OK;
}

// Wait_End_Dec( force stop )
static INT32 ct_im_raw_test1_26( VOID )
{
	INT32 	retval;
	INT32	ret;

	Ddim_Print(("raw test1_26 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_26 porocess 2\n"));
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_26 porocess 3\n"));
	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_26 porocess 4\n"));
	retval = Im_RAW_Start();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

#ifdef CO_DEBUG_ON_PC
	IO_RAW.RCTL1.bit.RTRG = 3;
#endif

	Ddim_Print(("raw test1_26 porocess 5\n"));
	retval = Im_RAW_Stop();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test1_26 porocess 6\n"));
	retval = Im_RAW_Close();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return ret;
}

// Stop( system error )
static INT32 ct_im_raw_test1_28( VOID )
{
	INT32 	retval;
	INT32	ret;
	UCHAR	tmp_fid;

	tmp_fid = FID_IM_RAW;
	FID_IM_RAW = 0xFF;

#ifdef CO_DEBUG_ON_PC
	IO_RAW.RCTL1.bit.RTRG = 3;
#endif

	retval = Im_RAW_Stop();
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	FID_IM_RAW = tmp_fid;

	return D_CT_IM_RAW_OK;
}

// Get_AXI_Status( normal end )
static INT32 ct_im_raw_test1_30( VOID )
{
	INT32 				retval;
	INT32				ret;
	T_IM_RAW_AXI_STATUS	axi_state;

	retval = Im_RAW_Get_AXI_Status( &axi_state );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("AXI state\n"));
	Ddim_Print(("Read ch  response state : %d\n", axi_state.r_ch_resp));
	Ddim_Print(("Write ch response state : %d\n", axi_state.w_ch_resp));

	return D_CT_IM_RAW_OK;
}

// Get_AXI_Status( parameter error )
static INT32 ct_im_raw_test1_31( VOID )
{
	INT32 	retval;
	INT32	ret;

	retval = Im_RAW_Get_AXI_Status( NULL );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_NG ){
		// NG is correct
		return D_CT_IM_RAW_NG;
	}

	return D_CT_IM_RAW_OK;
}

// Int_Handler( normal end )
static INT32 ct_im_raw_test1_32( VOID )
{
	INT32 	retval;
	INT32	ret;

	// set decode for callback setting
	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	IO_RAW.RINTF.bit.__RF	= 1;
	IO_RAW.RINTE.bit.RE		= 1;

	Im_RAW_Int_Handler();
	Ddim_Print(( "check : Int_Handler called\n" ));

	return D_CT_IM_RAW_OK;
}

// Int_Handler( R_AXI error )
static INT32 ct_im_raw_test1_33( VOID )
{
	INT32 	retval;
	INT32	ret;

	// set decode for callback setting
	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	IO_RAW.RINTF.bit.__RAXERF	= 1;
	IO_RAW.RINTE.bit.RAXERE		= 1;

	Im_RAW_Int_Handler();
	Ddim_Print(( "check : Int_Handler called\n" ));

	return D_CT_IM_RAW_OK;
}

// Int_Handler( W_AXI error )
static INT32 ct_im_raw_test1_34( VOID )
{
	INT32 	retval;
	INT32	ret;

	// set decode for callback setting
	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ret = ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	if( ret != D_CT_IM_RAW_OK ){
		return D_CT_IM_RAW_NG;
	}

	IO_RAW.RINTF.bit.__WAXERF	= 1;
	IO_RAW.RINTE.bit.WAXERE		= 1;

	Im_RAW_Int_Handler();
	Ddim_Print(( "check : Int_Handler called\n" ));

	return D_CT_IM_RAW_OK;
}

// Abnormal argument test
static INT32 ct_im_raw_test_2_0( VOID )
{
	T_IM_RAW_CTRL_CODE_TBL 	tbl;
	INT32 					retval;
	UINT32					condition, byte;

	Ddim_Print(("raw param error check. 11test all OK?\n"));
	retval = Im_RAW_Open( -2 );
	ct_im_raw_check_retval( retval, (INT32)D_IM_RAW_RETVAL_PARAM_ERROR );

	retval = Im_RAW_Ctrl_CodeTbl( NULL, D_IM_RAW_RFMT_14_OR_16_BIT );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_PARAM_ERROR );

	retval = Im_RAW_Ctrl_CodeTbl( &tbl, 10 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_PARAM_ERROR );

	retval = Im_RAW_Ctrl_Enc( NULL, 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_PARAM_ERROR );

	retval = Im_RAW_Ctrl_Dec( NULL );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_PARAM_ERROR );

	retval = Im_RAW_Wait_End_Enc( NULL, &byte, 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_PARAM_ERROR );

	retval = Im_RAW_Wait_End_Enc( &condition, NULL, 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_PARAM_ERROR );

	retval = Im_RAW_Wait_End_Enc( &condition, &byte, -3 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_PARAM_ERROR );

	retval = Im_RAW_Wait_End_Dec( NULL, 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_PARAM_ERROR );

	retval = Im_RAW_Wait_End_Dec( &condition, -100 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_PARAM_ERROR );

	retval = Im_RAW_Get_AXI_Status( NULL );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_PARAM_ERROR );

	return D_CT_IM_RAW_OK;
}

static INT32 ct_im_raw_test_2_1( VOID )
{
	return ct_im_raw_test_init();
}

// 14 bit RAW
static INT32 ct_im_raw_test_2_2( INT32 test_type )
{
	INT32 	retval;
	UINT32	condition, byte;
	T_IM_RAW_AXI_STATUS	axi_state;

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ORG_ADDR;

	if( test_type == 1 ){
		gCt_Im_Raw_Param.dst_addr = 0x00000000;			// Can't Access Address
	}
	else{
		gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_ENC_ADDR;
	}
	gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_14_OR_16_BIT;
	gCt_Im_Raw_Param.mcu_size = D_IM_RAW_DEFFC_MCU_2x2;
	gCt_Im_Raw_Param.bit_select = D_IM_RAW_BITSEL_EXCEPT_16_BIT;
	gCt_Im_Raw_Param.width = D_CT_IM_RAW_WIDTH;
	gCt_Im_Raw_Param.lines = D_CT_IM_RAW_LINES;

	Ddim_Print(("raw test2_2 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_2 porocess 2\n"));
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_2 porocess 3\n"));
	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_2 porocess 4\n"));
	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_2 porocess 5\n"));
	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );

	gCt_Im_Raw_Enc_Byte = byte;

	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	ct_im_raw_enc_result( retval, condition, byte, gCt_Im_Raw_Param.width, gCt_Im_Raw_Param.lines, 0 );

	retval = Im_RAW_Get_AXI_Status( &axi_state );
	Ddim_Print(("AXI state\n"));
	Ddim_Print(("Read ch  response state : %d\n", axi_state.r_ch_resp));
	Ddim_Print(("Write ch response state : %d\n", axi_state.w_ch_resp));

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_DEC_ADDR;

	Ddim_Print(("raw test2_2 porocess 6\n"));
	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_2 porocess 7\n"));
	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_2 porocess 8\n"));
	retval = Im_RAW_Wait_End_Dec( &condition, 1000 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	retval = ct_im_raw_verify_raw( (USHORT*)D_CT_IM_RAW_DEC_ADDR, (USHORT*)D_CT_IM_RAW_ORG_ADDR,
						           (gCt_Im_Raw_Param.width * 2 * gCt_Im_Raw_Param.lines),
						           gCt_Im_Raw_Param.bit_select );
	ct_im_raw_check_retval( retval, D_CT_IM_RAW_OK );

	Ddim_Print(("raw test2_2 porocess 9\n"));
	retval = Im_RAW_Close();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_2 porocess 10\n"));

	return D_CT_IM_RAW_OK;
}

// 16 bit RAW
static INT32 ct_im_raw_test_2_3( VOID )
{
	INT32 	retval;
	UINT32	condition, byte;

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ORG_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_14_OR_16_BIT;
	gCt_Im_Raw_Param.mcu_size = D_IM_RAW_DEFFC_MCU_2x2;
	gCt_Im_Raw_Param.bit_select = D_IM_RAW_BITSEL_16_BIT;
	gCt_Im_Raw_Param.width = D_CT_IM_RAW_WIDTH;
	gCt_Im_Raw_Param.lines = D_CT_IM_RAW_LINES;

	Ddim_Print(("raw test2_3 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_3 porocess 2\n"));

	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_3 porocess 3\n"));
	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_3 porocess 4\n"));
	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_3 porocess 5\n"));
	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );

	gCt_Im_Raw_Enc_Byte = byte;

	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	ct_im_raw_enc_result( retval, condition, byte, gCt_Im_Raw_Param.width, gCt_Im_Raw_Param.lines, 0 );

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_DEC_ADDR;
	gCt_Im_Raw_Param.bit_select = D_IM_RAW_BITSEL_16_BIT;		// 16 bit decode

	Ddim_Print(("raw test2_3 porocess 6\n"));
	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_3 porocess 7\n"));
	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_3 porocess 8\n"));
	retval = Im_RAW_Wait_End_Dec( &condition, 1000 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	retval = ct_im_raw_verify_raw( (USHORT*)D_CT_IM_RAW_ORG_ADDR, (USHORT*)D_CT_IM_RAW_DEC_ADDR,
						           (gCt_Im_Raw_Param.width * 2 * gCt_Im_Raw_Param.lines),
						           gCt_Im_Raw_Param.bit_select );
	ct_im_raw_check_retval( retval, D_CT_IM_RAW_OK );

	Ddim_Print(("raw test2_3 porocess 9\n"));
	retval = Im_RAW_Close();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_3 porocess 10\n"));

	return D_CT_IM_RAW_OK;
}

// 8 bit data w/o LUT
static INT32 ct_im_raw_test_2_4( VOID )
{
	INT32 	retval;
	UINT32	condition, byte;

	ct_im_raw_8bit_code_table();

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ORG_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_8_BIT;
	gCt_Im_Raw_Param.diff_mode = D_IM_RAW_DEFOP_DIFF_PREV_VALUE;
	gCt_Im_Raw_Param.lut = NULL;
	gCt_Im_Raw_Param.data_size = D_CT_IM_RAW_8_WIDTH * D_CT_IM_RAW_8_LINES;

	Ddim_Print(("raw test2_4 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_4 porocess 2\n"));
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_4 porocess 3\n"));
	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_4 porocess 4\n"));
	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_4 porocess 5\n"));
	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );

	gCt_Im_Raw_Enc_Byte = byte;

	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	ct_im_raw_enc_result( retval, condition, byte, D_CT_IM_RAW_8_WIDTH, D_CT_IM_RAW_8_LINES, 0 );

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_DEC_ADDR;

	Ddim_Print(("raw test2_4 porocess 6\n"));
	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_4 porocess 7\n"));
	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_4 porocess 8\n"));
	retval = Im_RAW_Wait_End_Dec( &condition, 1000 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	retval = ct_im_raw_verify_data( (UCHAR*)D_CT_IM_RAW_DEC_ADDR, (UCHAR*)D_CT_IM_RAW_ORG_ADDR,
						            gCt_Im_Raw_Param.data_size );
	ct_im_raw_check_retval( retval, D_CT_IM_RAW_OK );

	Ddim_Print(("raw test2_4 porocess 9\n"));
	retval = Im_RAW_Close();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_4 porocess 10\n"));

	return D_CT_IM_RAW_OK;
}

// 8 bit data with LUT
static INT32 ct_im_raw_test_2_5( VOID )
{
	INT32 	retval;
	UINT32	condition, byte;

	ct_im_raw_8bit_code_table();

	// LUT
	ct_im_raw_setup_user_lut0();

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ORG_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_8_BIT;
	gCt_Im_Raw_Param.diff_mode = D_IM_RAW_DEFOP_DIFF_PREV_VALUE;
	gCt_Im_Raw_Param.lut = gCt_Im_Raw_User_Lut;
	gCt_Im_Raw_Param.data_size = D_CT_IM_RAW_8_WIDTH * D_CT_IM_RAW_8_LINES;

	Ddim_Print(("raw test2_5 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_5 porocess 2\n"));
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_5 porocess 3\n"));
	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_5 porocess 4\n"));
	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_5 porocess 5\n"));
	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );

	gCt_Im_Raw_Enc_Byte = byte;

	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	ct_im_raw_enc_result( retval, condition, byte, D_CT_IM_RAW_8_WIDTH, D_CT_IM_RAW_8_LINES, 0 );

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_DEC_ADDR;

	// LUT
	ct_im_raw_setup_user_lut1();

	Ddim_Print(("raw test2_5 porocess 6\n"));
	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_5 porocess 7\n"));
	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_5 porocess 8\n"));
	retval = Im_RAW_Wait_End_Dec( &condition, 1000 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	// LUT
	ct_im_raw_setup_user_lut0();

	retval = ct_im_raw_verify_data_LUT( (UCHAR*)D_CT_IM_RAW_ORG_ADDR, (UCHAR*)D_CT_IM_RAW_DEC_ADDR,
						            	gCt_Im_Raw_Param.data_size );
	ct_im_raw_check_retval( retval, D_CT_IM_RAW_OK );

	Ddim_Print(("raw test2_5 porocess 9\n"));
	retval = Im_RAW_Close();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_5 porocess 10\n"));

	return D_CT_IM_RAW_OK;
}

// Force Stop
// Init at running
// Ctrl at running
// Start at running
static INT32 ct_im_raw_test_2_6( VOID )
{
	INT32 	retval;
	UINT32	condition, byte;

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ORG_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_14_OR_16_BIT;
	gCt_Im_Raw_Param.mcu_size = D_IM_RAW_DEFFC_MCU_2x2;
	gCt_Im_Raw_Param.bit_select = D_IM_RAW_BITSEL_EXCEPT_16_BIT;
	gCt_Im_Raw_Param.width = D_CT_IM_RAW_WIDTH;
	gCt_Im_Raw_Param.lines = D_CT_IM_RAW_LINES;

	retval = Im_RAW_Open( 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );


	Ddim_Print(("raw encode force stop test\n"));
	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Stop();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );

	gCt_Im_Raw_Enc_Byte = byte;

	Ddim_Print(("encode force stop OK?\n"));
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_FORCE_STOP );

	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw encode macro busy error test\n"));
	Ddim_Print(("4test macro busy OK?\n"));
	retval = Im_RAW_Init();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_MACRO_BUSY );
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_MACRO_BUSY );
	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_MACRO_BUSY );
	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_MACRO_BUSY );

	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	Ddim_Print(("encode result\n"));
	ct_im_raw_enc_result( retval, condition, byte, gCt_Im_Raw_Param.width, gCt_Im_Raw_Param.lines, 0 );

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_DEC_ADDR;

	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Stop();
	Ddim_Print(("raw decode force stop test\n"));
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Wait_End_Dec( &condition, 1000 );
	Ddim_Print(("decode force stop OK?\n"));
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_FORCE_STOP );

	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw encode macro busy error test\n"));
	Ddim_Print(("4test macro busy OK?\n"));
	retval = Im_RAW_Init();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_MACRO_BUSY );
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_MACRO_BUSY );
	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_MACRO_BUSY );
	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_MACRO_BUSY );

	retval = Im_RAW_Wait_End_Dec( &condition, 1000 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	retval = Im_RAW_Close();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return D_CT_IM_RAW_OK;
}

// CLMT
static INT32 ct_im_raw_test_2_7( VOID )
{
	INT32 	retval;
	UINT32	condition, byte;
	UINT32 	CLMT = 1024;

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ORG_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_14_OR_16_BIT;
	gCt_Im_Raw_Param.mcu_size = D_IM_RAW_DEFFC_MCU_2x2;
	gCt_Im_Raw_Param.bit_select = D_IM_RAW_BITSEL_EXCEPT_16_BIT;
	gCt_Im_Raw_Param.width = D_CT_IM_RAW_WIDTH;
	gCt_Im_Raw_Param.lines = D_CT_IM_RAW_LINES;

	retval = Im_RAW_Open( 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, CLMT );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );

	gCt_Im_Raw_Enc_Byte = byte;

	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	Ddim_Print(("encode limit over test OK?\n"));
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_ENC_LIMIT_OVER );

	retval = Im_RAW_Close();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return D_CT_IM_RAW_OK;
}

// Decode error (decode error)
static INT32 ct_im_raw_test_2_8( VOID )
{
	INT32 	retval;
	UINT32	condition, byte;

	// Modify code table
	gCt_Im_Raw_CodeTbl.code[3]  = 0x2000;
	gCt_Im_Raw_CodeTbl.code[4]  = 0x2000;
	gCt_Im_Raw_CodeTbl.code_length[3]  = 3;
	gCt_Im_Raw_CodeTbl.code_length[4]  = 3;

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ORG_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_14_OR_16_BIT;
	gCt_Im_Raw_Param.mcu_size = D_IM_RAW_DEFFC_MCU_2x2;
	gCt_Im_Raw_Param.bit_select = D_IM_RAW_BITSEL_EXCEPT_16_BIT;
	gCt_Im_Raw_Param.width = D_CT_IM_RAW_WIDTH;
	gCt_Im_Raw_Param.lines = D_CT_IM_RAW_LINES;

	Ddim_Print(("encode process\n"));
	retval = Im_RAW_Open( 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );

	gCt_Im_Raw_Enc_Byte = byte;

	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	ct_im_raw_enc_result( retval, condition, byte, gCt_Im_Raw_Param.width, gCt_Im_Raw_Param.lines, 0 );

	Ddim_Print(("decode process\n"));

	// Modify code table
	gCt_Im_Raw_CodeTbl.code[3] = 0x2000;
	gCt_Im_Raw_CodeTbl.code[4] = 0x2000;
	gCt_Im_Raw_CodeTbl.code_length[3]  = 3;
	gCt_Im_Raw_CodeTbl.code_length[4]  = 3;
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_DEC_ADDR;

	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("decode error test\n"));
	retval = Im_RAW_Wait_End_Dec( &condition, 1000 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	Ddim_Print(("decode error OK?\n"));
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_DEC_ERROR );

	retval = Im_RAW_Close();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return D_CT_IM_RAW_OK;
}

// Decode error (undef code)
static INT32 ct_im_raw_test_2_9( VOID )
{
	INT32 	retval;
	UINT32	condition, byte;

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ORG_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_14_OR_16_BIT;
	gCt_Im_Raw_Param.mcu_size = D_IM_RAW_DEFFC_MCU_2x2;
	gCt_Im_Raw_Param.bit_select = D_IM_RAW_BITSEL_EXCEPT_16_BIT;
	gCt_Im_Raw_Param.width = D_CT_IM_RAW_WIDTH;
	gCt_Im_Raw_Param.lines = D_CT_IM_RAW_LINES;

	Ddim_Print(("encode process\n"));
	retval = Im_RAW_Open( 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );

	gCt_Im_Raw_Enc_Byte = byte;

	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	ct_im_raw_enc_result( retval, condition, byte, gCt_Im_Raw_Param.width, gCt_Im_Raw_Param.lines, 0 );


	Ddim_Print(("decode process\n"));

	// Modify code table
	ct_im_raw_invalid_code();
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_DEC_ADDR;

	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("decode error(undef code) test\n"));
	retval = Im_RAW_Wait_End_Dec( &condition, 1000 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	Ddim_Print(("decode error(undef code) OK?\n"));
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_DEC_UNDEF_CODE );

	retval = Im_RAW_Close();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	return D_CT_IM_RAW_OK;
}

// Semaphone
static INT32 ct_im_raw_test_2_10( VOID )
{
	INT32 retval;

	retval = Im_RAW_Open( 0 );
	Ddim_Print(("raw open 1\n"));
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Open( 1 );
	Ddim_Print(("raw open error OK?\n"));
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_SYSTEM_ERROR );

	retval = Im_RAW_Close();
	Ddim_Print(("raw close 1\n"));
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Close();
	Ddim_Print(("raw close error OK?\n"));
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_SYSTEM_ERROR );

	return D_CT_IM_RAW_OK;
}

// 12 bit pack RAW
static INT32 ct_im_raw_test_2_11( VOID )
{
	INT32 	retval;
	UINT32	condition, byte;

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ORG_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_14_OR_16_BIT;
	gCt_Im_Raw_Param.mcu_size = D_IM_RAW_DEFFC_MCU_2x2;
	gCt_Im_Raw_Param.bit_select = D_IM_RAW_BITSEL_EXCEPT_16_BIT;
	gCt_Im_Raw_Param.width = D_CT_IM_RAW_WIDTH;
	gCt_Im_Raw_Param.lines = D_CT_IM_RAW_LINES;

	Ddim_Print(("raw test2_11 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_11 porocess 2\n"));
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_11 porocess 3\n"));
	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_11 porocess 4\n"));
	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_11 porocess 5\n"));
	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );

	gCt_Im_Raw_Enc_Byte = byte;

	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	ct_im_raw_enc_result( retval, condition, byte, gCt_Im_Raw_Param.width, gCt_Im_Raw_Param.lines, 1 );

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_DEC_ADDR;
	gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_12_BIT_PACK;	// 12bit pack decode


	Ddim_Print(("raw test2_11 porocess 6\n"));
	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_11 porocess 7\n"));
	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_11 porocess 8\n"));
	retval = Im_RAW_Wait_End_Dec( &condition, 1000 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	retval = ct_im_raw_verify_raw_12bitpack( (USHORT*)D_CT_IM_RAW_ORG_ADDR, (BYTE*)D_CT_IM_RAW_DEC_ADDR,
						                     (gCt_Im_Raw_Param.width * gCt_Im_Raw_Param.lines * 1.5));
	ct_im_raw_check_retval( retval, D_CT_IM_RAW_OK );

	Ddim_Print(("raw test2_11 porocess 9\n"));
	retval = Im_RAW_Close();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_11 porocess 10\n"));

	return D_CT_IM_RAW_OK;
}

// Int_Handler Test(PC Only)
static INT32 ct_im_raw_test2_12( VOID )
{
	Im_RAW_Int_Handler();

	return D_CT_IM_RAW_OK;
}

// change Huffman Table
static INT32 ct_im_raw_test_2_14( UINT32 test_val_1, UINT32 test_val_2, UINT32 test_val_3, UINT32 test_val_4,
							UINT32 test_val_5, UINT32 test_val_6, UINT32 test_val_7, UINT32 test_val_8 )
{
	INT32 	retval;
	INT32	dec_type;
	UINT32	condition, byte;
	T_IM_RAW_AXI_STATUS	axi_state;

	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ORG_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_ENC_ADDR;

	// Encode Format
	if( test_val_1 == 0 ){		// 14bit Format
		gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_14_OR_16_BIT;
		gCt_Im_Raw_Param.bit_select = D_IM_RAW_BITSEL_EXCEPT_16_BIT;
		Ddim_Print(("Encode Format : 14bit Select\n"));
	}
	else if( test_val_1 == 1 ){	// 12bit Format
		gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_12_BIT_PACK;
		gCt_Im_Raw_Param.bit_select = D_IM_RAW_BITSEL_EXCEPT_16_BIT;
		Ddim_Print(("Encode Format : 12bit pack Select\n"));
	}
	else if( test_val_1 == 2 ){	// 16bit Format
		gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_14_OR_16_BIT;
		gCt_Im_Raw_Param.bit_select = D_IM_RAW_BITSEL_16_BIT;
		Ddim_Print(("Encode Format : 16bit Select\n"));
	}
	else{
		Ddim_Print(("Encode Format select error 0:14bit 1:12bit 2:16bit\n"));
		return D_CT_IM_RAW_NG;
	}

	// Decode Format
	if( test_val_2 == 0 ){		// 14bit Format
		dec_type = 0;
		Ddim_Print(("Decode Format : 14bit Select\n"));
	}
	else if( test_val_2 == 1 ){	// 12bit Format
		dec_type = 1;
		Ddim_Print(("Decode Format : 12bit pack Select\n"));
	}
	else if( test_val_2 == 2 ){	// 16bit Format
		dec_type = 2;
		Ddim_Print(("Decode Format : 16bit Select\n"));
	}
	else{
		Ddim_Print(("Decode Format select error 0:14bit 1:12bit 2:16bit\n"));
		return D_CT_IM_RAW_NG;
	}

	// Width
	if( test_val_3 % 32 != 0 ){
		Ddim_Print(("Width error   not 32 byte align\n"));
		return D_CT_IM_RAW_NG;
	}
	gCt_Im_Raw_Param.width = test_val_3;

	// Line
	if( test_val_4 % 2 != 0 ){
		Ddim_Print(("Line error   not 2 byte align\n"));
		return D_CT_IM_RAW_NG;
	}
	gCt_Im_Raw_Param.lines = test_val_4;

	// MCU
	if( test_val_5 == 0 ){			// MCU 2x2
		gCt_Im_Raw_Param.mcu_size = D_IM_RAW_DEFFC_MCU_2x2;
	}
	else if( test_val_5 == 1 ){	// MCU 2x1
		gCt_Im_Raw_Param.mcu_size = D_IM_RAW_DEFFC_MCU_2x1;
	}
	else{
		Ddim_Print(("MCU select error   0:MCU 2x2 1:MCU 2x1\n"));
		return D_CT_IM_RAW_NG;
	}

	// Byte stuff
	if( test_val_6 == 0 ){			// Byte stuff OFF
		gCt_Im_Raw_Param.byte_stuffing = D_IM_RAW_OFF;
	}
	else if( test_val_6 == 1 ){	// Byte stuff ON
		gCt_Im_Raw_Param.byte_stuffing = D_IM_RAW_ON;
	}
	else{
		Ddim_Print(("Byte stuff select error    0:OFF 1:ON\n"));
		return D_CT_IM_RAW_NG;
	}

	// DEFINIT
	if( test_val_7 > 16383 ){		// DEFINIT
		Ddim_Print(("DEFINIT error   diff_fixed_value 0 to 16383\n"));
		return D_CT_IM_RAW_NG;
	}
	else{
		gCt_Im_Raw_Param.diff_fixed_value = test_val_7;
	}

	// Huff Tbl
	if( test_val_8 == 0 ){			// Default Huffman Table
		Ddim_Print(("Select Default huffman table\n"));
	}
	else if( test_val_8 == 1 ){	// User custom Table
		ct_im_raw_set_huff_tbl();
		Ddim_Print(("Select User set huffman table\n"));
	}
	else{
		Ddim_Print(("Huffman table select error    0:Default 1:User Setting\n"));
		return D_CT_IM_RAW_NG;
	}

	Ddim_Print(("raw test2_14 porocess 1\n"));
	retval = Im_RAW_Open( 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_14 porocess 2\n"));
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_14 porocess 3\n"));
	retval = Im_RAW_Ctrl_Enc( &gCt_Im_Raw_Param, 0 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_14 porocess 4\n"));
	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_14 porocess 5\n"));
	retval = Im_RAW_Wait_End_Enc( &condition, &byte, 1000 );

	gCt_Im_Raw_Enc_Byte = byte;

	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	if( gCt_Im_Raw_Param.data_format == D_IM_RAW_RFMT_12_BIT_PACK ){
		ct_im_raw_enc_result( retval, condition, byte, gCt_Im_Raw_Param.width, gCt_Im_Raw_Param.lines, 1 );
	}
	else{
		ct_im_raw_enc_result( retval, condition, byte, gCt_Im_Raw_Param.width, gCt_Im_Raw_Param.lines, 0 );
	}
	retval = Im_RAW_Get_AXI_Status( &axi_state );
	Ddim_Print(("AXI state\n"));
	Ddim_Print(("Read ch  response state : %d\n", axi_state.r_ch_resp));
	Ddim_Print(("Write ch response state : %d\n", axi_state.w_ch_resp));

	// Modify gCt_Im_Raw_Param
	gCt_Im_Raw_Param.src_addr = D_CT_IM_RAW_ENC_ADDR;
	gCt_Im_Raw_Param.dst_addr = D_CT_IM_RAW_DEC_ADDR;

	if( dec_type == 0 ){		// 14bit Format
		gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_14_OR_16_BIT;
		gCt_Im_Raw_Param.bit_select = D_IM_RAW_BITSEL_EXCEPT_16_BIT;
	}
	else if( dec_type == 1 ){	// 12bit Format
		gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_12_BIT_PACK;
		gCt_Im_Raw_Param.bit_select = D_IM_RAW_BITSEL_EXCEPT_16_BIT;
	}
	else{						// 16bit Format
		gCt_Im_Raw_Param.data_format = D_IM_RAW_RFMT_14_OR_16_BIT;
		gCt_Im_Raw_Param.bit_select = D_IM_RAW_BITSEL_16_BIT;
	}

	Ddim_Print(("raw test2_14 porocess 6\n"));
	retval = Im_RAW_Ctrl_CodeTbl( &gCt_Im_Raw_CodeTbl, gCt_Im_Raw_Param.data_format );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	retval = Im_RAW_Ctrl_Dec( &gCt_Im_Raw_Param );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_14 porocess 7\n"));
	retval = Im_RAW_Start();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_14 porocess 8\n"));
	retval = Im_RAW_Wait_End_Dec( &condition, 1000 );
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );
	ct_im_raw_check_retval( condition, D_IM_RAW_COND_OK );

	Ddim_Print(("raw test2_14 porocess 9\n"));
	retval = Im_RAW_Close();
	ct_im_raw_check_retval( retval, D_IM_RAW_RETVAL_OK );

	Ddim_Print(("raw test2_14 porocess 10\n"));

	return D_CT_IM_RAW_OK;
}


UINT32 Ct_Im_RAW_Get_Enc_Size( VOID )
{
	return gCt_Im_Raw_Enc_Byte;
}




// Test Code Main
void Ct_Im_RAW_Main(int argc, char** argv)
{
	UINT32 	test_num_0;
	UINT32 	test_num_1;
	INT32	retval;

#if 0
#ifndef CO_DEBUG_ON_PC
	// Data area value all crear!!!
	ct_im_raw_clear();
#endif	// CO_DEBUG_ON_PC
#endif

	Ddim_Print(("Ct_Im_RAW_Main Start\n"));

	ct_im_raw_init_param();
//	Ddim_Print(("init param OK\n"));
	retval = ct_im_raw_test_init();

//	Ddim_Print(( "test init OK\n" ));

	test_num_0 = (UINT32)atoi((const char*)argv[1]);
	test_num_1 = (UINT32)atoi((const char*)argv[2]);

	switch( test_num_0 ){
		case 1:		// PC simulator
			switch( test_num_1 ){
				case 0:		// Open/Close( pol_sem )
					Ddim_Print(( "raw test1_0 Start\n" ));
					retval = ct_im_raw_test1_0();
					Ddim_Print(( "raw test1_0 End\n" ));
					break;
				case 1:		// Open/Close( twai_sem )
					Ddim_Print(( "raw test1_1 Start\n" ));
					retval = ct_im_raw_test1_1();
					Ddim_Print(( "raw test1_1 End\n" ));
					break;
				case 2:		// Open/Close( parameter error )
					Ddim_Print(( "raw test1_2 Start\n" ));
					retval = ct_im_raw_test1_2();
					Ddim_Print(( "raw test1_2 End\n" ));
					break;
				case 3:		// Ctrl_CodeTbl( normal end )
					Ddim_Print(( "raw test1_3 Start\n" ));
					retval = ct_im_raw_test1_3();
					Ddim_Print(( "raw test1_3 End\n" ));
					break;
				case 4:		// Ctrl_CodeTbl( parameter error )
					Ddim_Print(( "raw test1_4 Start\n" ));
					retval = ct_im_raw_test1_4();
					Ddim_Print(( "raw test1_4 End\n" ));
					break;
				case 5:		// Ctrl_CodeTbl( macro busy )
					Ddim_Print(( "raw test1_5 Start\n" ));
					retval = ct_im_raw_test1_5();
					Ddim_Print(( "raw test1_5 End\n" ));
					break;
				case 6:		// Ctrl_Enc( normal end )
					Ddim_Print(( "raw test1_6 Start\n" ));
					retval = ct_im_raw_test1_6();
					Ddim_Print(( "raw test1_6 End\n" ));
					break;
				case 7:		// Ctrl_Enc( parameter error )
					Ddim_Print(( "raw test1_7 Start\n" ));
					retval = ct_im_raw_test1_7();
					Ddim_Print(( "raw test1_7 End\n" ));
					break;
				case 8:	// Ctrl_Enc( macro busy )
					Ddim_Print(( "raw test1_8 Start\n" ));
					retval = ct_im_raw_test1_8();
					Ddim_Print(( "raw test1_8 End\n" ));
					break;
				case 9:	// Ctrl_Dec( normal end )
					Ddim_Print(( "raw test1_9 Start\n" ));
					retval = ct_im_raw_test1_9();
					Ddim_Print(( "raw test1_9 End\n" ));
					break;
				case 10:	// Ctrl_Dec( parameter error )
					Ddim_Print(( "raw test1_10 Start\n" ));
					retval = ct_im_raw_test1_10();
					Ddim_Print(( "raw test1_10 End\n" ));
					break;
				case 11:	// Ctrl_Dec( macro busy )
					Ddim_Print(( "raw test1_11 Start\n" ));
					retval = ct_im_raw_test1_11();
					Ddim_Print(( "raw test1_11 End\n" ));
					break;
				case 12:	// _Start( normal end )
					Ddim_Print(( "raw test1_12 Start\n" ));
					retval = ct_im_raw_test1_12();
					Ddim_Print(( "raw test1_12 End\n" ));
					break;
				case 13:	// _Start( system error )
					Ddim_Print(( "raw test1_13 Start\n" ));
					retval = ct_im_raw_test1_13();
					Ddim_Print(( "raw test1_13 End\n" ));
					break;
				case 14:	// _Start( macro busy )
					Ddim_Print(( "raw test1_14 Start\n" ));
					retval = ct_im_raw_test1_14();
					Ddim_Print(( "raw test1_14 End\n" ));
					break;
				case 15:	// Wait_End_Enc( normal end )
					Ddim_Print(( "raw test1_15 Start\n" ));
					retval = ct_im_raw_test1_15();
					Ddim_Print(( "raw test1_15 End\n" ));
					break;
				case 16:	// Wait_End_Enc( parameter error )
					Ddim_Print(( "raw test1_16 Start\n" ));
					retval = ct_im_raw_test1_16();
					Ddim_Print(( "raw test1_16 End\n" ));
					break;
				case 17:	// Wait_End_Enc( system error )
					Ddim_Print(( "raw test1_17 Start\n" ));
					retval = ct_im_raw_test1_17();
					Ddim_Print(( "raw test1_17 End\n" ));
					break;
				case 18:	// Wait_End_Enc( R_AXI error )
					Ddim_Print(( "raw test1_18 Start\n" ));
					retval = ct_im_raw_test1_18();
					Ddim_Print(( "raw test1_18 End\n" ));
					break;
				case 19:	// Wait_End_Enc( W_AXI error )
					Ddim_Print(( "raw test1_19 Start\n" ));
					retval = ct_im_raw_test1_19();
					Ddim_Print(( "raw test1_19 End\n" ));
					break;
				case 20:	// Wait_End_Enc( force stop )
					Ddim_Print(( "raw test1_20 Start\n" ));
					retval = ct_im_raw_test1_20();
					Ddim_Print(( "raw test1_20 End\n" ));
					break;
				case 21:	// Wait_End_Dec( normal end )
					Ddim_Print(( "raw test1_21 Start\n" ));
					retval = ct_im_raw_test1_21();
					Ddim_Print(( "raw test1_21 End\n" ));
					break;
				case 22:	// Wait_End_Dec( parameter error )
					Ddim_Print(( "raw test1_22 Start\n" ));
					retval = ct_im_raw_test1_22();
					Ddim_Print(( "raw test1_22 End\n" ));
					break;
				case 23:	// Wait_End_Dec( system error )
					Ddim_Print(( "raw test1_23 Start\n" ));
					retval = ct_im_raw_test1_23();
					Ddim_Print(( "raw test1_23 End\n" ));
					break;
				case 24:	// Wait_End_Dec( R_AXI error )
					Ddim_Print(( "raw test1_24 Start\n" ));
					retval = ct_im_raw_test1_24();
					Ddim_Print(( "raw test1_24 End\n" ));
					break;
				case 25:	// Wait_End_Dec( W_AXI error )
					Ddim_Print(( "raw test1_25 Start\n" ));
					retval = ct_im_raw_test1_25();
					Ddim_Print(( "raw test1_25 End\n" ));
					break;
				case 26:	// Wait_End_Dec( force stop )
					Ddim_Print(( "raw test1_26 Start\n" ));
					retval = ct_im_raw_test1_26();
					Ddim_Print(( "raw test1_26 End\n" ));
					break;
#if 0	// 1-20(Wait_End_Enc( force stop ))Ç≈é¿é{çœÇ›
				case 27:	// Stop( normal end )
					Ddim_Print(( "raw test1_27 Start\n" ));
					retval = ct_im_raw_test1_27();
					Ddim_Print(( "raw test1_27 End\n" ));
					break;
#endif	// 1-20(Wait_End_Enc( force stop ))Ç≈é¿é{çœÇ›
				case 28:	// Stop( system error )
					Ddim_Print(( "raw test1_28 Start\n" ));
					retval = ct_im_raw_test1_28();
					Ddim_Print(( "raw test1_28 End\n" ));
					break;
#if 0	// ämîFÇÃÇµÇÊÇ§Ç™Ç»Ç¢
				case 29:	// Stop( macro busy )
					Ddim_Print(( "raw test1_29 Start\n" ));
					retval = ct_im_raw_test1_29();
					Ddim_Print(( "raw test1_29 End\n" ));
					break;
#endif	// ämîFÇÃÇµÇÊÇ§Ç™Ç»Ç¢
				case 30:	// Get_AXI_Status( normal end )
					Ddim_Print(( "raw test1_30 Start\n" ));
					retval = ct_im_raw_test1_30();
					Ddim_Print(( "raw test1_30 End\n" ));
					break;
				case 31:	// Get_AXI_Status( parameter error )
					Ddim_Print(( "raw test1_31 Start\n" ));
					retval = ct_im_raw_test1_31();
					Ddim_Print(( "raw test1_31 End\n" ));
					break;
				case 32:	// Int_Handler( normal end )
					Ddim_Print(( "raw test1_32 Start\n" ));
					retval = ct_im_raw_test1_32();
					Ddim_Print(( "raw test1_32 End\n" ));
					break;
				case 33:	// Int_Handler( R_AXI error )
					Ddim_Print(( "raw test1_33 Start\n" ));
					retval = ct_im_raw_test1_33();
					Ddim_Print(( "raw test1_33 End\n" ));
					break;
				case 34:	// Int_Handler( W_AXI error )
					Ddim_Print(( "raw test1_34 Start\n" ));
					retval = ct_im_raw_test1_34();
					Ddim_Print(( "raw test1_34 End\n" ));
					break;
				default:
					Ddim_Print(("IMRAW NO COMMAND\n"));
					return;
			}
			break;
		case 2:		// Palladium
			switch( test_num_1 ){
				case 0:
					Ddim_Print(( "raw test2_0 Start\n" ));
					retval = ct_im_raw_test_2_0();
					Ddim_Print(( "raw test2_0 End\n" ));
					break;
				case 1:
					Ddim_Print(( "raw test2_1 Start\n" ));
					retval = ct_im_raw_test_2_1();
					Ddim_Print(( "raw test2_1 End\n" ));
					ct_im_raw_dump_reg();
					break;
				case 2:
					Ddim_Print(( "raw test2_2 Start\n" ));
					retval = ct_im_raw_test_2_2(0);
					Ddim_Print(( "raw test2_2 End\n" ));

					ct_im_raw_dump_reg();
					break;
				case 3:
					Ddim_Print(( "raw test2_3 Start\n" ));
					retval = ct_im_raw_test_2_3();
					Ddim_Print(( "raw test2_3 End\n" ));

					ct_im_raw_dump_reg();
					break;
				case 4:
		#ifndef D_CT_IM_RAW_PT_TEST
		//			ct_im_raw_load_8bit_image();
		#endif
					Ddim_Print(( "raw test2_4 Start\n" ));
					retval = ct_im_raw_test_2_4();
					Ddim_Print(( "raw test2_4 End\n" ));

					ct_im_raw_dump_reg();
					break;
				case 5:
		#ifndef D_CT_IM_RAW_PT_TEST
		//			ct_im_raw_load_8bit_image();
		#endif
					Ddim_Print(( "raw test2_5 Start\n" ));
					retval = ct_im_raw_test_2_5();
					Ddim_Print(( "raw test2_5 End\n" ));

					ct_im_raw_dump_reg();
					break;
				case 6:
					Ddim_Print(( "raw test2_6 Start\n" ));
					retval = ct_im_raw_test_2_6();
					Ddim_Print(( "raw test2_6 End\n" ));
					break;
				case 7:
					Ddim_Print(( "raw test2_7 Start\n" ));
					retval = ct_im_raw_test_2_7();
					Ddim_Print(( "raw test2_7 End\n" ));
					break;
				case 8:
					Ddim_Print(( "raw test2_8 Start\n" ));
					retval = ct_im_raw_test_2_8();
					Ddim_Print(( "raw test2_8 End\n" ));
					break;
				case 9:
					Ddim_Print(( "raw test2_9 Start\n" ));
					retval = ct_im_raw_test_2_9();
					Ddim_Print(( "raw test2_9 End\n" ));
					break;
				case 10:
					Ddim_Print(( "raw test2_10 Start\n" ));
					retval = ct_im_raw_test_2_10();
					Ddim_Print(( "raw test2_10 End\n" ));
					break;
				case 11:
					Ddim_Print(( "raw test2_11 Start\n" ));
					retval = ct_im_raw_test_2_11();
					Ddim_Print(( "raw test2_11 End\n" ));

					ct_im_raw_dump_reg();
					break;
				case 13:
					Ddim_Print(( "raw test2_13 Start\n" ));
					retval = ct_im_raw_test_2_2(1);
					Ddim_Print(( "raw test2_13 End\n" ));
					break;
				case 14:
					Ddim_Print(( "raw test2_14 Start\n" ));
					retval = ct_im_raw_test_2_14( (UINT32)atoi((const char*)argv[2]),
													(UINT32)atoi((const char*)argv[3]),
													(UINT32)atoi((const char*)argv[4]),
													(UINT32)atoi((const char*)argv[5]),
													(UINT32)atoi((const char*)argv[6]),
													(UINT32)atoi((const char*)argv[7]),
													(UINT32)atoi((const char*)argv[8]),
													(UINT32)atoi((const char*)argv[9]) );
					Ddim_Print(( "raw test2_14 End\n" ));
					break;
				default:
					Ddim_Print(("IMRAW NO COMMAND\n"));
					return;
			}
			break;
		default:
			Ddim_Print(("IMRAW NO COMMAND\n"));
			return;
	}
	
#ifndef D_CT_IM_RAW_PT_TEST
	if( retval ){
		Ddim_Print(("IMRAW COMMAND ERROR\n"));
//		DDIM_User_Dly_Tsk(1000);
//		while(1);
	}
	else{
		Ddim_Print(("IMRAW COMMAND OK\n"));
	}
#else
	if( retval ){
		Ddim_Print(("IMRAW COMMAND ERROR\n"));
	}
	else{
		Ddim_Print(("IMRAW COMMAND OK\n"));
	}
#endif

	return;
}



// Test Code Main
void Ct_Im_RAW_Main_PC(int argc, char** argv)
{
	INT32 	retval;

	Ddim_Print(("RAW Test 2_0\n"));
	retval = ct_im_raw_test_2_0();
	ct_im_raw_ret_print(retval);
	Ddim_Print(("RAW Test 2_1\n"));
	retval = ct_im_raw_test_init();
	ct_im_raw_dump_reg();
	ct_im_raw_ret_print(retval);

	Ddim_Print(("RAW Test 2_10\n"));
	retval = ct_im_raw_test_2_10();
	ct_im_raw_ret_print(retval);
	Ddim_Print(("RAW Test 2_11\n"));
	retval = ct_im_raw_test_2_11();
	ct_im_raw_ret_print(retval);
	Ddim_Print(("RAW Test 2_12\n"));
	retval = ct_im_raw_test2_12();
	ct_im_raw_ret_print(retval);

	return;
}


