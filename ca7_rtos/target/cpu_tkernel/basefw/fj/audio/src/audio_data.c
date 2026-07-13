/**
 * @file		audio_data.c
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "fj_audio.h"
#include "audio_data.h"
#include "dd_arm.h"
#include "l1l2cache.h"
#include "sdram_map_cache_define.h"
#include "debug.h"

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
static const volatile UINT32 gBF_Audio_Data_NullData[4096] __attribute__((aligned(64))) = {0};

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function                                                             */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/

/**
 *	@brief		Clean and Flush Data Cache
 *	@param		VOID*			address
 *	@param		UINT32			size
 *	@return		None
 *	@attention	None
 */
VOID BF_Audio_Data_Clean_Flush_Dcache( const VOID* const addr, const UINT32 bytes )
{
	if (bytes == 0){
		BF_Debug_Print_Error(("BF_Audio_Data_Clean_Flush_Dcache() Size=0, Addr=%lx\n", (ULONG)addr));
	}
	else {
		if (addr == NULL){
			BF_Debug_Print_Error(("BF_Audio_Data_Clean_Flush_Dcache() Addr=NULL, size=%lx\n", (ULONG)bytes));
		}
		else {
			BF_L1l2cache_Clean_Flush_Addr((UINT32)M_SDRAM_MAP_COMMON_ADR_PHY2LOG((UINT32)addr), bytes);
		}
	}
}

/**
 *	@brief		Flush Data Cache
 *	@param		VOID*			address
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Flush_Dcache( const VOID* const addr, const UINT32 bytes )
{
	if (bytes == 0){
		BF_Debug_Print_Error(("BF_Audio_Data_Flush_Dcache() Size=0, Addr=%lx\n", (ULONG)addr));
	}
	else {
		if (addr == NULL){
			BF_Debug_Print_Error(("BF_Audio_Data_Flush_Dcache() Addr=NULL, size=%x\n", bytes));
		}
		else {
			BF_L1l2cache_Flush_Addr((UINT32)M_SDRAM_MAP_COMMON_ADR_PHY2LOG((UINT32)addr), bytes);
		}
	}
}

/**
 *	@brief		Clean Data Cache
 *	@param		VOID*			address
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Clean_Dcache( const VOID* const addr, const UINT32 bytes )
{
	if (bytes == 0){
		BF_Debug_Print_Error(("BF_Audio_Data_Clean_Dcache() Size=0, Addr=%lx\n", (ULONG)addr));
	}
	else {
		if (addr == NULL){
			BF_Debug_Print_Error(("BF_Audio_Data_Clean_Dcache() Addr=NULL, size=%x\n", bytes));
		}
		else {
			BF_L1l2cache_Clean_Addr((UINT32)M_SDRAM_MAP_COMMON_ADR_PHY2LOG((UINT32)addr), bytes);
		}
	}
}

/**
 *	@brief		Reverse 2 byte
 *	@param		UINT16*			address of reverse data
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Reverse2Byte( UINT16* data2 )
{
	union {
		UINT16 uh;
		UINT8 byte[2];
	}dummy;
	
	dummy.uh = *data2;
	*data2 = (UINT16)( (UINT32)(dummy.byte[1] << 8) + dummy.byte[0]);
}

/**
 *	@brief		Change Endian
 *	@param		UINT8*			address of pcm data
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_ChangeEndian( UINT8* pcm, UINT32 data_bytes )
{
	UINT32 pos;
	UINT8 data;
	
	for (pos = 0; pos < data_bytes; pos += 2){
		data = pcm[pos];
		pcm[pos] = pcm[pos + 1];
		pcm[pos + 1] = data;
	}
}

/**
 *	@brief		32bit to 24bit
 *	@param		UINT8*			address of pcm data(source)
 *	@param		UINT8*			address of pcm data(destination)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_4ByteTo3Byte( UINT8* in_4byte, UINT8* out_3byte, UINT32 in_data_size )
{
	UINT32 count;
	UINT8* in_wk;
	UINT8* out_wk;
	
	in_wk = in_4byte;
	out_wk = out_3byte;
	
	count = in_data_size / 4;
	
	for (count = in_data_size / 4; count > 0; count--){
		in_wk++;
		
		*out_wk = *in_wk;
		in_wk++;
		out_wk++;
		
		*out_wk = *in_wk;
		in_wk++;
		out_wk++;
		
		*out_wk = *in_wk;
		in_wk++;
		out_wk++;
	}
}

/**
 *	@brief		24bit to 32bit
 *	@param		UINT8*			address of pcm data(source)
 *	@param		UINT8*			address of pcm data(destination)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_3ByteTo4Byte( UINT8* in_3byte, UINT8* out_4byte, UINT32 in_data_size )
{
	UINT32 count;
	UINT8* addr3;
	UINT8* addr4;
	
	addr3 = in_3byte + in_data_size - 1;
	addr4 = out_4byte + ((in_data_size / 3) * 4) - 1;
	
	for (count = in_data_size / 3; count > 0; count--){
		*addr4 = *addr3;
		addr4--;
		addr3--;
		
		*addr4 = *addr3;
		addr4--;
		addr3--;
		
		*addr4 = *addr3;
		addr4--;
		addr3--;
		
		*addr4 = 0;
		addr4--;
	}
}

/**
 *	@brief		24bit to 16bit
 *	@param		UINT8*			address of pcm data(source)
 *	@param		UINT8*			address of pcm data(destination)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Pcm_24bitTo16bit( UINT8* in_24bit, UINT8* out_16bit, UINT32 in_data_size )
{
	UINT32 count;
	UINT8* in_wk;
	UINT8* out_wk;
	
	in_wk = in_24bit;
	out_wk = out_16bit;
	
	for (count = in_data_size / 3; count > 0; count--){
		in_wk++;
		*out_wk = *in_wk;
		in_wk++;
		out_wk++;
		*out_wk = *in_wk;
		in_wk++;
		out_wk++;
	}
}

/**
 *	@brief		16bit to 24bit
 *	@param		UINT8*			address of pcm data(source)
 *	@param		UINT8*			address of pcm data(destination)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Pcm_16bitTo24bit( UINT8* in_16bit, UINT8* out_24bit, UINT32 in_data_size )
{
	UINT32 count;
	UINT8* addr16;
	UINT8* addr24;
	
	addr16 = in_16bit + in_data_size - 1;
	addr24 = out_24bit + ((in_data_size / 2) * 3) - 1;
	
	for (count = in_data_size / 2; count > 0; count--){
		*addr24 = *addr16;
		addr24--;
		addr16--;
		*addr24 = *addr16;
		addr24--;
		addr16--;
		*addr24 = 0;
		addr24--;
	}
}

/**
 *	@brief		8bit to 16bit
 *	@param		UINT8*			address of pcm data(source)
 *	@param		INT16*			address of pcm data(destination)
 *	@param		UINT32			number of samples
 *	@param		UINT32			number of channels
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Pcm_8to16( INT8* src_addr, INT16* dst_addr, UINT32 smpl_cnt, UINT8 channel)
{
	ULONG index;
	INT8* in_wk;
	INT16* out_wk;
	
	in_wk = src_addr;
	out_wk = dst_addr;
	
	for (index = 0; index < (smpl_cnt * channel); index++) {
		*out_wk = (INT16)*in_wk;
		*out_wk = (*out_wk) * 0x100;
		out_wk++;
		in_wk++;
	}
}

/**
 *	@brief		16bit to 8bit
 *	@param		INT16*			address of pcm data(source)
 *	@param		UINT8*			address of pcm data(destination)
 *	@param		UINT32			number of samples
 *	@param		UINT32			number of channels
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Pcm_16to8( INT16* src_addr, INT8* dst_addr, UINT32 smpl_cnt, UINT8 channel)
{
	UINT32 index;
	INT16* in_wk;
	INT8* out_wk;
	
	in_wk = src_addr;
	out_wk = dst_addr;
	
	for (index = 0; index < (smpl_cnt * channel); index++) {
		*out_wk = (INT8)(*in_wk >> 8);
		out_wk++;
		in_wk++;
	}
}

/**
 *	@brief		Mono to Stereo(24bitPCM)
 *	@param		UINT8*			address of pcm data(source)
 *	@param		UINT8*			address of pcm data(destination)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_24bitPcm_MonoToStereo( UINT8* in_mono, UINT8* out_stereo, UINT32 in_data_size )
{
	UINT32 count;
	UINT8* addr_m;
	UINT8* addr_s;
	
	addr_m = in_mono + in_data_size - 1;
	addr_s = out_stereo + in_data_size * 2 - 1;
	
	for (count = in_data_size / 3; count > 0; count--){
		*addr_s			= *addr_m;
		*(addr_s - 3)	= *addr_m;
		addr_s--;
		addr_m--;
		
		*addr_s			= *addr_m;
		*(addr_s - 3)	= *addr_m;
		addr_s--;
		addr_m--;
		
		*addr_s			= *addr_m;
		*(addr_s - 3)	= *addr_m;
		addr_s--;
		addr_m--;
		addr_s -= 3;
	}
}

/**
 *	@brief		Stereo to Mono(24bitPCM)
 *	@param		UINT8*			address of pcm data(source)
 *	@param		UINT8*			address of pcm data(destination)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_24bitPcm_StereoToMono( UINT8* in_stereo, UINT8* out_mono, UINT32 in_data_size )
{
	UINT32 count;
	UINT8* in_wk;
	UINT8* out_wk;
	
	in_wk = in_stereo;
	out_wk = out_mono;
	
	for (count = in_data_size / 3; count > 0; count--){
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		in_wk += 3;		// Skip Rch
	}
}

/**
 *	@brief		Add two pcm data(16bitPCM)
 *	@param		INT16*			address of pcm data
 *	@param		INT16*			address of pcm data
 *	@param		INT16*			address of pcm data(added pcm)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Add_16bitPcm( INT16* in_data_1, INT16* in_data_2, INT16* out_data, UINT32 data_size )
{
	UINT32 count;
	INT32 sum;
	INT16* in_wk1;
	INT16* in_wk2;
	INT16* out_wk;
	
	in_wk1 = in_data_1;
	in_wk2 = in_data_2;
	out_wk = out_data;
	
	for (count = 0; count < data_size / 2; count++){
		sum = *in_wk1 + *in_wk2;
		
		if (sum > 32767){
			sum = 32767;
		}
		else if (sum < -32768){
			sum = -32768;
		}
		else {
			;
		}
		
		*out_wk = (INT16)sum;
		out_wk++;
		in_wk1++;
		in_wk2++;
	}
}

/**
 *	@brief		Add two pcm data(24bitPCM)
 *	@param		INT16*			address of pcm data
 *	@param		INT16*			address of pcm data
 *	@param		INT16*			address of pcm data(added pcm)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Add_24bitPcm( UINT8* in_data_1, UINT8* in_data_2, UINT8* out_data, UINT32 data_size )
{
	UINT32 count;
	INT32 in_data32_1;
	INT32 in_data32_2;
	UINT32 out_data32;
	INT64 sum;
	UINT8* in_wk1;
	UINT8* in_wk2;
	UINT8* out_wk;
	
	in_wk1 = in_data_1;
	in_wk2 = in_data_2;
	out_wk = out_data;
	
	for (count = 0; count < data_size / 3; count++){
		
		BF_Audio_Data_3ByteTo4Byte(in_wk1, (UINT8*)&in_data32_1, 3);
		BF_Audio_Data_3ByteTo4Byte(in_wk2, (UINT8*)&in_data32_2, 3);
		
		sum = (INT64)in_data32_1 + (INT64)in_data32_2;
		
		if (sum > 2147483392){
			sum = 2147483392;
		}
		else if (sum < -2147483648){
			sum = -2147483648;
		}
		else {
			;
		}
		
		out_data32 = (UINT32)(sum & 0x00000000FFFFFFFF);
		
		BF_Audio_Data_4ByteTo3Byte((UINT8*)&out_data32, out_wk, 4);
		
		in_wk1 += 3;
		in_wk2 += 3;
		out_wk += 3;
	}
}

/**
 *	@brief		swap (2ch + 2ch) data to 4ch data(16bitPCM)
 *	@param		INT16*			address of pcm data(2ch data)
 *	@param		INT16*			address of pcm data(2ch data)
 *	@param		INT16*			address of pcm data(4ch data)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Swp_16bitPcm_2chTo4ch( UINT32* in_data_1, UINT32* in_data_2, UINT32* out_data, UINT32 in_data_size )
{
	UINT32 count;
	UINT32* in_wk1;
	UINT32* in_wk2;
	UINT32* out_wk;
	
	in_wk1 = in_data_1;
	in_wk2 = in_data_2;
	out_wk = out_data;
	
	for (count = 0; count < (in_data_size / 4); count++){
		*out_wk = *in_wk1;
		out_wk++;
		in_wk1++;
		
		*out_wk = *in_wk2;
		out_wk++;
		in_wk2++;
	}
}

/**
 *	@brief		swap (2ch + 2ch)data to 4ch data(24bitPCM)
 *	@param		INT16*			address of pcm data(2ch data)
 *	@param		INT16*			address of pcm data(2ch data)
 *	@param		INT16*			address of pcm data(4ch data)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Swp_32bitPcm_2chTo4ch( UINT32* in_data_1, UINT32* in_data_2, UINT32* out_data, UINT32 in_data_size )
{
	UINT32 count;
	UINT32* in_wk1;
	UINT32* in_wk2;
	UINT32* out_wk;
	
	in_wk1 = in_data_1;
	in_wk2 = in_data_2;
	out_wk = out_data;
	
	for (count = 0; count < (in_data_size / 8); count++){
		// FLch
		*out_wk = *in_wk1;
		out_wk++;
		in_wk1++;
		
		// FRch
		*out_wk = *in_wk1;
		out_wk++;
		in_wk1++;
		
		// RLch
		*out_wk = *in_wk2;
		out_wk++;
		in_wk2++;
		
		// RRch
		*out_wk = *in_wk2;
		out_wk++;
		in_wk2++;
	}
}

/**
 *	@brief		swap (2ch + 2ch) data  to 3ch data(16bitPCM)
 *	@param		INT16*			address of pcm data(2ch data)
 *	@param		INT16*			address of pcm data(2ch data)
 *	@param		INT16*			address of pcm data(4ch data)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Swp_16bitPcm_2chTo3ch( UINT16* in_data_1, UINT16* in_data_2, UINT16* out_data, UINT32 in_data_size )
{
	UINT32 count;
	UINT16* in_wk1;
	UINT16* in_wk2;
	UINT16* out_wk;
	
	in_wk1 = in_data_1;
	in_wk2 = in_data_2;
	out_wk = out_data;
	
	for (count = 0; count < (in_data_size / 4); count++){
		*out_wk = *in_wk1;
		out_wk++;
		in_wk1++;
		*out_wk = *in_wk1;
		out_wk++;
		in_wk1++;
		
		*out_wk = *in_wk2;
		out_wk++;
		in_wk2++;
		in_wk2++;
	}
}

/**
 *	@brief		swap (2ch + 1ch) data  to 3ch data(24bitPCM)
 *	@param		INT16*			address of pcm data(2ch data)
 *	@param		INT16*			address of pcm data(1ch data)
 *	@param		INT16*			address of pcm data(4ch data)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Swp_32bitPcm_2chTo3ch( UINT32* in_data_1, UINT32* in_data_2, UINT32* out_data, UINT32 in_data_size )
{
	UINT32 count;
	UINT32* in_wk1;
	UINT32* in_wk2;
	UINT32* out_wk;
	
	in_wk1 = in_data_1;
	in_wk2 = in_data_2;
	out_wk = out_data;
	
	for (count = 0; count < (in_data_size / 8); count++){
		// FLch
		*out_wk = *in_wk1;
		out_wk++;
		in_wk1++;
		
		// FRch
		*out_wk = *in_wk1;
		out_wk++;
		in_wk1++;
		
		// RLch
		*out_wk = *in_wk2;
		out_wk++;
		in_wk2++;
		in_wk2++;
	}
}

/**
 *	@brief		swap 4ch data to (2ch + 2ch) data(16bitPCM)
 *	@param		INT16*			address of pcm data(2ch data)
 *	@param		INT16*			address of pcm data(2ch data)
 *	@param		INT16*			address of pcm data(4ch data)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Swp_16bitPcm_4chTo2ch( UINT32* in_data, UINT32* out_data_1, UINT32* out_data_2, UINT32 in_data_size )
{
	UINT32 count;
	UINT32* in_wk;
	UINT32* out_wk1;
	UINT32* out_wk2;
	
	in_wk = in_data;
	out_wk1 = out_data_1;
	out_wk2 = out_data_2;
	
	for (count = 0; count < (in_data_size / 8); count++){
		*out_wk1 = *in_wk;
		out_wk1++;
		in_wk++;
		
		*out_wk2 = *in_wk;
		out_wk2++;
		in_wk++;
	}
}

/**
 *	@brief		swap 4ch data to (2ch + 2ch) data(24bitPCM)
 *	@param		INT16*			address of pcm data(2ch data)
 *	@param		INT16*			address of pcm data(2ch data)
 *	@param		INT16*			address of pcm data(4ch data)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Swp_32bitPcm_4chTo2ch( UINT32* in_data, UINT32* out_data_1, UINT32* out_data_2, UINT32 in_data_size )
{
	UINT32 count;
	UINT32* in_wk;
	UINT32* out_wk1;
	UINT32* out_wk2;
	
	in_wk = in_data;
	out_wk1 = out_data_1;
	out_wk2 = out_data_2;
	
	for (count = 0; count < (in_data_size / 16); count++){
		// FLch
		*out_wk1 = *in_wk;
		out_wk1++;
		in_wk++;
		
		// FRch
		*out_wk1 = *in_wk;
		out_wk1++;
		in_wk++;
		
		// RLch
		*out_wk2 = *in_wk;
		out_wk2++;
		in_wk++;
		
		// RRch
		*out_wk2 = *in_wk;
		out_wk2++;
		in_wk++;
	}
}

/**
 *	@brief		swap 3ch data to (2ch + 1ch) data(16bitPCM)
 *	@param		INT16*			address of pcm data(2ch data)
 *	@param		INT16*			address of pcm data(1ch data)
 *	@param		INT16*			address of pcm data(4ch data)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */

VOID BF_Audio_Data_Swp_16bitPcm_3chTo2ch( UINT16* in_data, UINT16* out_data_1, UINT16* out_data_2, UINT32 in_data_size )
{
	UINT32 count;
	UINT16* in_wk;
	UINT16* out_wk1;
	UINT16* out_wk2;
	
	in_wk = in_data;
	out_wk1 = out_data_1;
	out_wk2 = out_data_2;
	
	for (count = 0; count < (in_data_size / 6); count++){
		*out_wk1 = *in_wk;
		out_wk1++;
		in_wk++;
		
		*out_wk1 = *in_wk;
		out_wk1++;
		in_wk++;
		
		*out_wk2 = *in_wk;
		out_wk2++;
		in_wk++;
		
		*out_wk2 = 0x0000;
		out_wk2++;
	}
}

/**
 *	@brief		swap 3ch data to (2ch + 1ch) data(24bitPCM)
 *	@param		UINT32*			address of pcm data(2ch data)
 *	@param		UINT32*			address of pcm data(1ch data)
 *	@param		UINT32*			address of pcm data(4ch data)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_Swp_32bitPcm_3chTo2ch( UINT32* in_data, UINT32* out_data_1, UINT32* out_data_2, UINT32 in_data_size )
{
	UINT32 count;
	UINT32* in_wk;
	UINT32* out_wk1;
	UINT32* out_wk2;
	
	in_wk = in_data;
	out_wk1 = out_data_1;
	out_wk2 = out_data_2;
	
	for (count = 0; count < (in_data_size / 12); count++){
		// FLch
		*out_wk1 = *in_wk;
		out_wk1++;
		in_wk++;
		
		// FRch
		*out_wk1 = *in_wk;
		out_wk1++;
		in_wk++;
		
		// RLch
		*out_wk2 = *in_wk;
		out_wk2++;
		in_wk++;
		
		*out_wk2 = 0x00000000;
		out_wk2++;
	}
}

/**
 *	@brief		Convert 24bit 2ch data to  24bit 2ch+1ch(NULL) datadata
 *	@param		UINT8*			address of pcm data(2ch data)
 *	@param		UINT8*			address of pcm data(3ch data)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_24bitPcm_Cnv2chTo3ch( UINT8* in_data, UINT8* out_data, UINT32 in_data_size )
{
	UINT32 count;
	UINT8* in_wk;
	UINT8* out_wk;
	
	in_wk = in_data;
	out_wk = out_data;
	
	for (count = 0; count < (in_data_size / 6); count++){
		// FLch
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		// FRch
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		// RCch
		*out_wk = 0x00;
		out_wk++;
		
		*out_wk = 0x00;
		out_wk++;
		
		*out_wk = 0x00;
		out_wk++;
	}
}

/**
 *	@brief		Convert 24bit 2ch data to  24bit 2ch+2ch(NULL) datadata
 *	@param		UINT8*			address of pcm data(2ch data)
 *	@param		UINT8*			address of pcm data(4ch data)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_24bitPcm_Cnv2chTo4ch( UINT8* in_data, UINT8* out_data, UINT32 in_data_size )
{
	UINT32 count;
	UINT8* in_wk;
	UINT8* out_wk;
	
	in_wk = in_data;
	out_wk = out_data;
	
	for (count = 0; count < (in_data_size / 6); count++){
		// FLch
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		// FRch
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		// RLch
		*out_wk = 0x00;
		out_wk++;
		
		*out_wk = 0x00;
		out_wk++;
		
		*out_wk = 0x00;
		out_wk++;
		
		// RRch
		*out_wk = 0x00;
		out_wk++;
		
		*out_wk = 0x00;
		out_wk++;
		
		*out_wk = 0x00;
		out_wk++;
	}
}

/**
 *	@brief		Convert 16bit 2ch data to  16bit 2ch+1ch(NULL) datadata
 *	@param		INT16*			address of pcm data(2ch data)
 *	@param		INT16*			address of pcm data(3ch data)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_16bitPcm_Cnv2chTo3ch( UINT16* in_data, UINT16* out_data, UINT32 in_data_size )
{
	UINT32 count;
	UINT16* in_wk;
	UINT16* out_wk;
	
	in_wk = in_data;
	out_wk = out_data;
	
	for (count = 0; count < (in_data_size / 4); count++){
		// FLch
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		// FRch
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		// RCch
		*out_wk = 0x0000;
		out_wk++;
	}
}

/**
 *	@brief		Convert 16bit 2ch data to  16bit 2ch+2ch(NULL) datadata
 *	@param		INT16*			address of pcm data(2ch data)
 *	@param		INT16*			address of pcm data(4ch data)
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_16bitPcm_Cnv2chTo4ch( UINT16* in_data, UINT16* out_data, UINT32 in_data_size )
{
	UINT32 count;
	UINT16* in_wk;
	UINT16* out_wk;
	
	in_wk = in_data;
	out_wk = out_data;
	
	for (count = 0; count < (in_data_size / 4); count++){
		// FLch
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		// FRch
		*out_wk = *in_wk;
		out_wk++;
		in_wk++;
		
		// RLch
		*out_wk = 0x0000;
		out_wk++;
		
		// RRch
		*out_wk = 0x0000;
		out_wk++;
	}
}

/**
 *	@brief		Left Bit Shift for 16bit PCM
 *	@param		INT16*			address of pcm data
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_16bitPcm_BitShift_L( UINT16* pcm, UINT32 data_size )
{
	UINT32 loop;
	UINT16* pcm_wk;
	
	pcm_wk = pcm;
	
	for (loop = 0; loop < (data_size / 2); loop++){
		*pcm_wk = (*pcm_wk) << 1;
		pcm_wk++;
	}
}

/**
 *	@brief		Left Bit Shift for 24bit(32bit) PCM
 *	@param		UINT32*			address of pcm data
 *	@param		UINT32			size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Data_32bitPcm_BitShift_L( UINT32* pcm, UINT32 data_size )
{
	UINT32 loop;
	UINT32* pcm_wk;
	
	pcm_wk = pcm;
	
	for (loop = 0; loop < (data_size / 4); loop++){
		*pcm_wk = (*pcm_wk) << 1;
		pcm_wk++;
	}
}

/**
 *	@brief		Get Address of Null Data
 *	@param		None
 *	@return		None
 *	@note		Data Size:16384 = 1024(sample)*4[ch]*4[BYTE](32bitPCM)
 *	@attention	None
 */
UINT32 BF_Audio_Data_Get_NullData_Addr( VOID )
{
	return (UINT32)gBF_Audio_Data_NullData;
}

