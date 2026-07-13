/**
 * @file		dsp_load.c
 * @brief		DSP program binary load API
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "sdram_map_define.h"
#include "dsp_load.h"
#include "fj_filesystem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
//#define M_DSP_LOAD_DBG_PRINTF( arg )		printf arg
#define M_DSP_LOAD_DBG_PRINTF( arg )

#define M_ROUNDDOWN( v, rnd2 )	( (v) & (~(rnd2 - 1)) )

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_DSP_LOAD_SIZE_ID_S	(0x10000)		// DF_ID_S.bin [byte]
#define D_DSP_LOAD_SIZE_IP		(0x4000)		// DF_IP.bin   [byte]

#define D_DSP_LOAD_ADDR_IP		( M_ROUNDDOWN( (SDRAM_ADR_FS_COPY_BUFFER_AREA - D_DSP_LOAD_SIZE_IP),   64 ) )
#define D_DSP_LOAD_ADDR_ID_S	( D_DSP_LOAD_ADDR_IP - D_DSP_LOAD_SIZE_ID_S )

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
static volatile UCHAR	*g_p_dsp_ex = NULL;
static volatile UCHAR	*g_p_dsp_id_s;
static volatile UCHAR	*g_p_dsp_ip;

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/

// Load DSP bin file
static INT32 dsp_read_file( CHAR* file_name, UCHAR* dst_addr, ULONG read_size )
{
	FJ_ERR_CODE fj_ercd;
	ULONG file_id;
	ULONG ret_read_size = 0;
	
	file_id = FJ_Fs_Open( file_name, FJ_FS_OPEN_RDONLY );
	
	if(!file_id) {
		M_DSP_LOAD_DBG_PRINTF( ("DSP binary open error 0!!:[%s]\n", file_name) );
		return 1;
	}
	
	fj_ercd = FJ_Fs_Read( file_id, (UINT32*)dst_addr, read_size, &ret_read_size );
	
	if((fj_ercd != FJ_ERR_OK) && (fj_ercd != FJ_ERR_READ_EOF)) {
		M_DSP_LOAD_DBG_PRINTF( ("DSP binary read error!!:[%s] errorID:[%ld]\n", file_name, file_id) );
		return 1;
	}
	
	FJ_Fs_Close( file_id );
	
	return 0;
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

// Load DF_EX.bin by section
INT32 DSP_Load_Ex_Section( UCHAR *p_src )
{
	ULONG	i;
	ULONG	num;		// The number of section.
	ULONG	size;		// The size of section.
	UCHAR	*p_dst;
	const ULONG size_bin_header = 16;
	const ULONG size_sec_header = 8;
	
	
	if (NULL == g_p_dsp_ex) {
		g_p_dsp_ex = p_src;
	}
	
	// Parse binary header
	num = (ULONG)((p_src[3] << 24) | (p_src[2] << 16) | (p_src[1] <<  8) | (p_src[0] <<  0));
	p_src += size_bin_header;
	
	M_DSP_LOAD_DBG_PRINTF(("dsp_load:num = %d\n", (int)num));
	
	for (i = 0; i < num; i++) {
		
		// Parse section header
		size  = (ULONG )((p_src[3] << 24) | (p_src[2] << 16) | (p_src[1] <<  8) | (p_src[0] <<  0));
		p_dst = (UCHAR*)((p_src[7] << 24) | (p_src[6] << 16) | (p_src[5] <<  8) | (p_src[4] <<  0));
		
		M_DSP_LOAD_DBG_PRINTF(("dsp_load:[%3d] [dst_addr;0x%8x] [size;0x%8x] [src_offset;0x%8x]\n", 
							   (int)i, (UINT32)p_dst, (UINT32)size, (UINT32)(p_src - g_p_dsp_ex)));
		
		p_src += size_sec_header;
		
		// Copy to execute region
		memcpy(p_dst, p_src, size);
		p_src += size;
	
	}
	
	return 0;
}


// DSP bin file load function
INT32 DSP_Load_Bin_File( VOID )
{
	INT32 ret = 0;
#if CO_DSP_MODE == 1		// SD ch1
	CHAR* dsp_ex   = "I:\\DF_EX.BIN";
	CHAR* dsp_id_s = "I:\\DF_ID_S.BIN";
	CHAR* dsp_ip   = "I:\\DF_IP.BIN";
#else						// NAND
	CHAR* dsp_ex   = "A:\\DF_EX.BIN";
	CHAR* dsp_id_s = "A:\\DF_ID_S.BIN";
	CHAR* dsp_ip   = "A:\\DF_IP.BIN";
#endif
	
	// Load each binary
	// note: DF_EX.BIN is loaded to temporary buffer at first.
	{
		FJ_FS_STAT	sta;
		FJ_ERR_CODE ercd;
		
		
		g_p_dsp_id_s = (volatile UCHAR*)D_DSP_LOAD_ADDR_ID_S;
		g_p_dsp_ip   = (volatile UCHAR*)D_DSP_LOAD_ADDR_IP;
		
		ercd = FJ_Fs_Stat( dsp_ex, &sta );
		if( ercd != FJ_ERR_OK ) {
			M_DSP_LOAD_DBG_PRINTF( ("FJ_FsStat error:[%s]\n", dsp_ex) );
			return 1;
		}

		g_p_dsp_ex = (volatile UCHAR*)malloc(sta.size);
		if( NULL == g_p_dsp_ex ) {
			M_DSP_LOAD_DBG_PRINTF( ("malloc error\n") );
			return 1;
		}

		M_DSP_LOAD_DBG_PRINTF(("size ex     : %d\n",      (int)sta.size));
		M_DSP_LOAD_DBG_PRINTF(("g_p_dsp_ex  : 0x%08lx\n", (unsigned long)g_p_dsp_ex));
		M_DSP_LOAD_DBG_PRINTF(("g_p_dsp_id_s: 0x%08lx\n", (unsigned long)g_p_dsp_id_s));
		M_DSP_LOAD_DBG_PRINTF(("g_p_dsp_ip  : 0x%08lx\n", (unsigned long)g_p_dsp_ip));

		ret  = dsp_read_file( dsp_ex,   (UCHAR*)g_p_dsp_ex,   sta.size);
		ret |= dsp_read_file( dsp_id_s, (UCHAR*)g_p_dsp_id_s, D_DSP_LOAD_SIZE_ID_S);
		ret |= dsp_read_file( dsp_ip,   (UCHAR*)g_p_dsp_ip,   D_DSP_LOAD_SIZE_IP);
		
		if(ret != 0) {
			free((VOID*)g_p_dsp_ex);
			return ret; 
		}
	}

	ret = DSP_Load_Ex_Section( (UCHAR*)g_p_dsp_ex );

	M_DSP_LOAD_DBG_PRINTF(("DSP_Load_Ex_Section[ret]: %d\n", ret));

	free((VOID*)g_p_dsp_ex);

	return ret;
}

