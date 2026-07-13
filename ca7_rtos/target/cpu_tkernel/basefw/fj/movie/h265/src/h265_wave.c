/**
 * @file		h265_hevc.c
 * @brief		H.265 HEVC function.
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; FUJITSU SEMICONDUCTOR LIMITED 2014</I></B>
 */

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#include "vpuapi.h"
#include "vpuapifunc.h"
#include "vdi_userfunc.h"
#include "h265_wave.h"
#include "sdram_map_define.h"

#ifdef CO_H265_SECTION_ON
#pragma arm section code="H265_CODE"
#pragma arm section rodata="H265_CONST"
#pragma arm section rwdata="H265_INIT"
#endif


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */
#define localStack (*((volatile struct ls *)0x11000000))

//#define CO_SIRIUS_FIRM_DOWNLOAD_SKIP
#define CO_PMU_ON

#if 0
#define CO_DEBUG_CODE_SIZE_CHECK
/*
ex.)The required code size is obtained by the following calculation.

ch0 : (7c800000 + 1ff000) - 7c000000 = 9FF000 ¨ 9.99609375MByte
ch1 : (df614000 + 89000)  - dee00000 = 89D000 ¨ 8.61328125MByte


I:1:T01000046: code.addr_ch0 = 7c000000  -- Start address of ch 0
I:1:T01000046: code.addr_ch1 = dee00000  -- Start address of ch 1
I:1:T01000046: code.size_ch0 = 4000000
I:1:T01000046: code.size_ch1 = 1000000
I:1:T01000046: vdi_allocate_dma_memory : ddr_ch = (0)
I:1:T01000046: vdi_allocate_dma_memory : vb->size      = (100000)
I:1:T01000046: vdi_allocate_dma_memory : vb->phys_addr = (7c700000)
I:1:T01000046: vdi_allocate_dma_memory : vb->base      = (700000)
I:1:T01000046: vdi_allocate_dma_memory : vb->virt_addr = (7c700000)
I:1:T01000046: vdi_allocate_dma_memory : vb->block     = (4)
I:1:T01000046: vdi_allocate_dma_memory : ddr_ch = (1)
I:1:T01000046: vdi_allocate_dma_memory : vb->size      = (111000)
I:1:T01000046: vdi_allocate_dma_memory : vb->phys_addr = (df500000)
I:1:T01000046: vdi_allocate_dma_memory : vb->base      = (700000)
I:1:T01000046: vdi_allocate_dma_memory : vb->virt_addr = (df500000)
I:1:T01000046: vdi_allocate_dma_memory : vb->block     = (3)
I:1:T01000046: vdi_allocate_dma_memory : ddr_ch = (1)
I:1:T01000046: vdi_allocate_dma_memory : vb->size      = (89000)    -+
I:1:T01000046: vdi_allocate_dma_memory : vb->phys_addr = (df614000) -+-- addr + size is last address used for ch 1
I:1:T01000046: vdi_allocate_dma_memory : vb->base      = (814000)
I:1:T01000046: vdi_allocate_dma_memory : vb->virt_addr = (df614000)
I:1:T01000046: vdi_allocate_dma_memory : vb->block     = (3)
I:1:T01000046: vdi_allocate_dma_memory : ddr_ch = (0)
I:1:T01000046: vdi_allocate_dma_memory : vb->size      = (1ff000)   -+
I:1:T01000046: vdi_allocate_dma_memory : vb->phys_addr = (7c800000) -+-- addr + size is last address used for ch 0
I:1:T01000046: vdi_allocate_dma_memory : vb->base      = (800000)
I:1:T01000046: vdi_allocate_dma_memory : vb->virt_addr = (7c800000)
I:1:T01000046: vdi_allocate_dma_memory : vb->block     = (4)
*/
#endif

#define NAL_BUF_SIZE			512
#define SEI_NAL_DATA_BUF_SIZE	0x4000
#define VUI_HRD_RBSP_BUF_SIZE	0x4000
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
enum
{
    SPP_IN,
    SPP_OUT_RBSP,
    SPP_OUT_NAL,
    SPP_UNALIGN
};

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct
{
	// stream buffer status
	unsigned char*  rd_ptr;
	unsigned char*  wr_ptr;

	unsigned char*  bb_start;
	unsigned char*  bb_end;

	int     explicit_end_flag;
	int     stream_end_flag;

	// instance handler ... for interrupt handling
	void*   handle;

} gbu_if_t;

typedef struct
{
	// for interfacing
	gbu_if_t    *param;
	
	// internal read pointer
	UINT8* rd_ptr;

	// nal buffer things
	UINT8  nal_buf[2][NAL_BUF_SIZE];

	int      nal_buf_idx;
	int      nal_buf_cnt;

	int      nal_ptr;
	UINT32 nal_cnt;

	int      last_nal_byte;

	// rbsp buffer things
	INT32  wbuf[2];
	int8_t   wbuf_emul_info;

	int8_t   bptr;
	int8_t   rbsp_bit_cnt;

	int      rbsp_init; 

	// rbsp consumed bit count
	int      tcnt;

	int      tc;

	// trailing zero counter
	int      zero_cnt;

	// eos
	int      eos;

	
	UINT32 est_nal_byte_cnt;

	int epbRequired;

} gbu_t;

typedef void* spp_enc_context;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static VOID h265_wave_sdramc_clock_enable( E_H265_SDRAMC_CLOCK_EN enable );

static void h265_wave_hrdEncode(spp_enc_context spp, hrd_t *hrd, UINT32 vps_max_sub_layers_minus1);

static void enc_flush_rbsp(spp_enc_context context);
static void enc_flush_nal_buf(spp_enc_context context);
static void put_bits(spp_enc_context context, UINT32 var, int n);
//static void put_ue(spp_enc_context context, UINT32 var);

static spp_enc_context spp_enc_init(UINT8 *buffer, int buffer_size, int enableEPB);
static void spp_enc_deinit(spp_enc_context context);
//static void spp_enc_init_rbsp(spp_enc_context context);
static void spp_enc_put_nal_byte(spp_enc_context context, UINT32 var, int n);
static void spp_enc_put_bits(spp_enc_context context, UINT32 var, int n);
static void spp_enc_flush(spp_enc_context context);
static void spp_enc_put_ue(spp_enc_context context, UINT32 var);
static INT32 spp_enc_get_ue_bit_size(UINT32 var);
//static void spp_enc_put_se(spp_enc_context context, INT32 var);
static void spp_enc_put_byte_align(spp_enc_context context, int has_stop_bit);
//static UINT32 spp_enc_get_wbuf_remain(spp_enc_context context);
static UINT32 spp_enc_get_rbsp_bit(spp_enc_context context);
static UINT32 spp_enc_get_nal_cnt(spp_enc_context context);
//static UINT8* spp_enc_get_wr_ptr(spp_enc_context context);
//static UINT8* spp_enc_get_wr_ptr_only(spp_enc_context context);
//static UINT32 spp_enc_get_est_nal_cnt(spp_enc_context context);

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
static VOID h265_wave_sdramc_clock_enable( E_H265_SDRAMC_CLOCK_EN enable )
{
	if( enable == E_H265_SDRAMC_CLOCK_EN_ENABLE ){
		// SDRAM ch0
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VCOREW, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VCORER0, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VCORER1, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VCPU, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VCOREM, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VCPU, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VCOREWR0, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VCORER1, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VCOREM, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		// SDRAM ch1
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VCOREW, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VCORER0, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VCORER1, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VCPU, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VCOREM, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VCPU, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VCOREWR0, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VCORER1, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VCOREM, E_H265_SDRAMC_CLOCK_EN_ENABLE );
	}
	else{
		// SDRAM ch0
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VCOREW, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VCORER0, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VCORER1, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VCPU, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VCOREM, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VCPU, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VCOREWR0, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VCORER1, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VCOREM, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		// SDRAM ch1
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VCOREW, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VCORER0, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VCORER1, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VCPU, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VCOREM, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VCPU, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VCOREWR0, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VCORER1, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VCOREM, E_H265_SDRAMC_CLOCK_EN_DISABLE );
	}
	
	return;
}

/**
 *	@brief		WAVE init Process.
 *	@param		IN :mode
 *				   :clock
 *				OUT:None
 *	@return		H265_OK	Normal End
 *	@return		H265_NG	Abnormal End
 *	@note		None
 *	@attention	None
 */
INT32 H265_Wave_Initialize( UINT32 mode, UINT32 clock, T_H265_WAVE_CODE code )
{
	INT32			ret;
	
	H265_Info_Print(( "H265_Wave_Initialize\n" ));
	
	H265_Movie_PMU_Setting( E_H265_PMU_BLOCK_WAVE, H265_ON );
	
	// clock start!!
	H265_Wave_Clock_Start( clock );
	
	// set vpu sdram addr/size
	vdi_set_vpu_dram_ch0_physical_base( (INT32)code.addr_ch0 );
	vdi_set_vpu_dram_ch1_physical_base( (INT32)code.addr_ch1 );
	vdi_set_vpu_dram_ch0_size( (INT32)code.size_ch0 );
	vdi_set_vpu_dram_ch1_size( (INT32)code.size_ch1 );
	
#ifdef CO_DEBUG_CODE_SIZE_CHECK
	H265_Info_Print(( "\n" ));
	H265_Info_Print(( "code.addr_ch0 = %#x\n", (INT32)code.addr_ch0 ));
	H265_Info_Print(( "code.addr_ch1 = %#x\n", (INT32)code.addr_ch1 ));
	H265_Info_Print(( "code.size_ch0 = %#x\n", (INT32)code.size_ch0 ));
	H265_Info_Print(( "code.size_ch1 = %#x\n", (INT32)code.size_ch1 ));
	H265_Info_Print(( "\n" ));
#endif
	
	ret = VPU_Init( 0 );
	if( ret != RETCODE_SUCCESS ){
		H265_Err_Print(( "H265_Wave_Initialize : VPU_Init error(%d)\n", ret ));
		return H265_NG;
	}
	
	H265_Info_Print(( "H265_Wave_Initialize End\n" ));
	return H265_OK;
}

/**
 *	@brief		WAVE Fainaraiz Process.
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Wave_Finalize( VOID )
{
	H265_Info_Print(( "H265_Wave_Finalize Start\n" ));
	
	VPU_DeInit( 0 );
	
	H265_Wave_Clock_Stop();
	
	H265_Movie_PMU_Setting( E_H265_PMU_BLOCK_WAVE, H265_OFF );
	
	H265_Info_Print(( "H265_Wave_Finalize End\n"   ));
}

/**
 *	@brief		Start MPCKST and execute Soft Reset(H265SR).\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Wave_Clock_Start( INT32 clock )
{
	h265_wave_sdramc_clock_enable( E_H265_SDRAMC_CLOCK_EN_ENABLE );
	
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VCOREW,  0x00000000 );
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VCORER0, 0x00000000 );
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VCORER1, 0x00000000 );
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VCPU,    0x00000000 );
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VCOREM,  0x00000000 );
	
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VCOREW,  0x00000000 );
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VCORER0, 0x00000000 );
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VCORER1, 0x00000000 );
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VCPU,    0x00000000 );
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VCOREM,  0x00000000 );
	
	H265_Movie_HEVENC_Clock_Stop( E_H265_TOP_CLOCK_STOP_OFF );
	
	OS_User_Tslp_Tsk(1);
	H265_Movie_HEVENC_Reset( E_H265_TOP_RESET_OFF );
}

/**
 *	@brief		Stop MPCKST and execute Soft Reset(H265SR).\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Wave_Clock_Stop( VOID )
{
	OS_User_Tslp_Tsk(1);
	H265_Movie_HEVENC_Reset( E_H265_TOP_RESET_ON );
	
	h265_wave_sdramc_clock_enable( E_H265_SDRAMC_CLOCK_EN_DISABLE );
}

static DDRChannelParam ddrCh_save;

//----------------------
//	Encode Function
//----------------------
INT32 H265_Wave_SetDDRChannel( DDRChannelParam ddrCh )
{
	INT32	ret;
	
	ret = VPU_SetDDRChannel( 0, ddrCh );
	if( ret != RETCODE_SUCCESS ){
		H265_Err_Print(( "VPU_SetDDRChannel error = (%d)\n", ret ));
	}
	ddrCh_save = ddrCh;
	
	return ret;
}

INT32 H265_Wave_EncOpen( UINT32 ch, EncHandle* handle, EncOpenParam* encOpParam )
{
	INT32	ret;
	
	if( encOpParam->bitstreamBuffer >= SDRAM_ADR_CH1_TOP ){
		encOpParam->bitstreamBuffer -= SDRAM_ADR_CH1_TOP;
	}
	else{
		encOpParam->bitstreamBuffer -= SDRAM_ADR_CH0_TOP;
	}
	if( encOpParam->vuiRbspDataAddr >= SDRAM_ADR_CH1_TOP ){
		encOpParam->vuiRbspDataAddr -= SDRAM_ADR_CH1_TOP;
	}
	else{
		encOpParam->vuiRbspDataAddr -= SDRAM_ADR_CH0_TOP;
	}
	if( encOpParam->nalSizeReportAddr >= SDRAM_ADR_CH1_TOP ){
		encOpParam->nalSizeReportAddr -= SDRAM_ADR_CH1_TOP;
	}
	else{
		encOpParam->nalSizeReportAddr -= SDRAM_ADR_CH0_TOP;
	}
	ret = VPU_EncOpen( handle, encOpParam );
	if( ret != RETCODE_SUCCESS ){
		H265_Err_Print(( "VPU_EncOpen(%d) error = (%d)\n", ch, ret ));
	}
	return ret;
}

INT32 H265_Wave_EncClose( UINT32 ch, EncHandle handle )
{
	INT32	ret;
	
	ret = VPU_EncClose( handle );
	if( ret != RETCODE_SUCCESS ){
		H265_Err_Print(( "VPU_EncClose(%d) error = (%d)\n", ch, ret ));
	}
	return ret;
}

INT32 H265_Wave_EncGetInitialInfo( UINT32 ch, EncHandle handle, EncInitialInfo* encInitInfo )
{
	INT32	ret;
	
	ret = VPU_EncGetInitialInfo( handle, encInitInfo );
	if( ret != RETCODE_SUCCESS ){
		H265_Err_Print(( "VPU_EncGetInitialInfo(%d) error = (%d)\n", ch, ret ));
	}
#if 1
	H265_Info_Print(( "VPU_EncGetInitialInfo(%d) : encInitInfo->minFrameBufferCount = (%d)\n", ch, encInitInfo->minFrameBufferCount ));
	H265_Info_Print(( "VPU_EncGetInitialInfo(%d) : encInitInfo->minSrcFrameCount    = (%d)\n", ch, encInitInfo->minSrcFrameCount    ));
#endif
	return ret;
}

INT32 H265_Wave_EncRegisterFrameBuffer( UINT32 ch, EncHandle handle, FrameBuffer* bufArray, int num, int stride, int height, int mapType )
{
	INT32	ret;
	
#if 0
	H265_Info_Print(( "H265_Wave_EncRegisterFrameBuffer(%d) : num     = (%x)\n", ch, num     ));
	H265_Info_Print(( "H265_Wave_EncRegisterFrameBuffer(%d) : stride  = (%x)\n", ch, stride  ));
	H265_Info_Print(( "H265_Wave_EncRegisterFrameBuffer(%d) : height  = (%x)\n", ch, height  ));
	H265_Info_Print(( "H265_Wave_EncRegisterFrameBuffer(%d) : mapType = (%x)\n", ch, mapType ));
	{
		INT32	i;
		for( i=0;i < num;i++ ){
			H265_Info_Print(( "H265_Wave_EncRegisterFrameBuffer(%d) : bufArray[%d].bufY         = (%x)\n", ch, i, bufArray[i].bufY         ));
			H265_Info_Print(( "H265_Wave_EncRegisterFrameBuffer(%d) : bufArray[%d].bufCb        = (%x)\n", ch, i, bufArray[i].bufCb        ));
			H265_Info_Print(( "H265_Wave_EncRegisterFrameBuffer(%d) : bufArray[%d].bufCr        = (%x)\n", ch, i, bufArray[i].bufCr        ));
			H265_Info_Print(( "H265_Wave_EncRegisterFrameBuffer(%d) : bufArray[%d].size         = (%x)\n", ch, i, bufArray[i].size         ));
			H265_Info_Print(( "H265_Wave_EncRegisterFrameBuffer(%d) : bufArray[%d].updateFbInfo = (%x)\n", ch, i, bufArray[i].updateFbInfo ));
		}
	}
#endif
	{
		INT32 i;
		for( i=0;i < num;i++ ){
			if( bufArray[i].bufY >= SDRAM_ADR_CH1_TOP ){
				bufArray[i].bufY -= SDRAM_ADR_CH1_TOP;
			}
			else{
				bufArray[i].bufY -= SDRAM_ADR_CH0_TOP;
			}
		}
		
	}
	
	ret = VPU_EncRegisterFrameBuffer( handle, bufArray, num, stride, height, mapType );
	if( ret != RETCODE_SUCCESS ){
		H265_Err_Print(( "VPU_EncRegisterFrameBuffer(%d) error = (%d)\n", ch, ret ));
	}
	return ret;
}

INT32 H265_Wave_EncAllocateFrameBuffer( UINT32 ch, EncHandle handle, FrameBufferAllocInfo info, FrameBuffer *frameBuffer )
{
	INT32	ret;
#if 0
	H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : info.mapType        = (%x)\n", ch, info.mapType ));
	H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : info.cbcrInterleave = (%x)\n", ch, info.cbcrInterleave ));
	H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : info.nv21           = (%x)\n", ch, info.nv21 ));
	H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : info.format         = (%x)\n", ch, info.format ));
	H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : info.stride         = (%x)\n", ch, info.stride ));
	H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : info.height         = (%x)\n", ch, info.height ));
	H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : info.size           = (%x)\n", ch, info.size ));
	H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : info.lumaBitDepth   = (%x)\n", ch, info.lumaBitDepth ));
	H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : info.chromaBitDepth = (%x)\n", ch, info.chromaBitDepth ));
	H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : info.endian         = (%x)\n", ch, info.endian ));
	H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : info.num            = (%x)\n", ch, info.num ));
	H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : info.type           = (%x)\n", ch, info.type ));
	{
		INT32	i;
		for( i=0;i < info.num;i++ ){
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].bufY           = (%x)\n", ch, i, frameBuffer[i].bufY ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].bufCb          = (%x)\n", ch, i, frameBuffer[i].bufCb ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].bufCr          = (%x)\n", ch, i, frameBuffer[i].bufCr ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].bufYBot        = (%x)\n", ch, i, frameBuffer[i].bufYBot ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].bufCbBot       = (%x)\n", ch, i, frameBuffer[i].bufCbBot ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].bufCrBot       = (%x)\n", ch, i, frameBuffer[i].bufCrBot ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].cbcrInterleave = (%x)\n", ch, i, frameBuffer[i].cbcrInterleave ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].nv21           = (%x)\n", ch, i, frameBuffer[i].nv21 ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].endian         = (%x)\n", ch, i, frameBuffer[i].endian ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].myIndex        = (%x)\n", ch, i, frameBuffer[i].myIndex ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].stride         = (%x)\n", ch, i, frameBuffer[i].stride ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].width          = (%x)\n", ch, i, frameBuffer[i].width ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].height         = (%x)\n", ch, i, frameBuffer[i].height ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].lumaBitDepth   = (%x)\n", ch, i, frameBuffer[i].lumaBitDepth ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].chromaBitDepth = (%x)\n", ch, i, frameBuffer[i].chromaBitDepth ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].format         = (%x)\n", ch, i, frameBuffer[i].format ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].sourceLBurstEn = (%x)\n", ch, i, frameBuffer[i].sourceLBurstEn ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].srcBufState    = (%x)\n", ch, i, frameBuffer[i].srcBufState ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].allocNum       = (%x)\n", ch, i, frameBuffer[i].allocNum ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].srcEndFlag     = (%x)\n", ch, i, frameBuffer[i].srcEndFlag ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].sequenceNo     = (%x)\n", ch, i, frameBuffer[i].sequenceNo ));
			H265_Info_Print(( "H265_Wave_EncAllocateFrameBuffer(%d) : frameBuffer[%d].updateFbInfo   = (%x)\n", ch, i, frameBuffer[i].updateFbInfo ));
		}
	}
#endif
	{
		INT32 i;
		for( i=0;i < info.num;i++ ){
			if( frameBuffer[i].bufY >= SDRAM_ADR_CH1_TOP ){
				frameBuffer[i].bufY -= SDRAM_ADR_CH1_TOP;
			}
			else{
				frameBuffer[i].bufY -= SDRAM_ADR_CH0_TOP;
			}
		}
		
	}
	
	ret = VPU_EncAllocateFrameBuffer( handle, info, frameBuffer );
	if( ret != RETCODE_SUCCESS ){
		H265_Err_Print(( "VPU_EncAllocateFrameBuffer(%d) error = (%d)\n", ch, ret ));
	}
	return ret;
}

INT32 H265_Wave_EncGiveCommand( UINT32 ch, EncHandle handle, CodecCommand cmd, void * parameter )
{
	INT32	ret;
	
	if( cmd == ENC_PUT_VIDEO_HEADER ){
		EncHeaderParam* encHeaderParamp;
		encHeaderParamp = (EncHeaderParam*)parameter;
		if( encHeaderParamp->buf >= SDRAM_ADR_CH1_TOP ){
			encHeaderParamp->buf -= SDRAM_ADR_CH1_TOP;
		}
		else{
			encHeaderParamp->buf -= SDRAM_ADR_CH0_TOP;
		}
	}
	else if( cmd == SET_ADDR_ENC_RC_REPORT_BASE ){
		UINT32 *val;
		val = (UINT32*)parameter;
		if( *val >= SDRAM_ADR_CH1_TOP ){
			*val -= SDRAM_ADR_CH1_TOP;
		}
		else{
			*val -= SDRAM_ADR_CH0_TOP;
		}
	}
	else if( cmd == ENC_SET_SEI_NAL_DATA ){
		HevcSEIDataEnc* seiDataEnc;
		seiDataEnc = (HevcSEIDataEnc*)parameter;
		if( seiDataEnc->prefixSeiNalAddr >= SDRAM_ADR_CH1_TOP ){
			seiDataEnc->prefixSeiNalAddr -= SDRAM_ADR_CH1_TOP;
		}
		else{
			seiDataEnc->prefixSeiNalAddr -= SDRAM_ADR_CH0_TOP;
		}
	}
	
	ret = VPU_EncGiveCommand( handle, cmd, parameter );
	if( ret != RETCODE_SUCCESS ){
		H265_Err_Print(( "VPU_EncGiveCommand(%d) error = (%d)\n", ch, ret ));
	}
	
	if( cmd == ENC_PUT_VIDEO_HEADER ){
		EncHeaderParam* encHeaderParamp;
		encHeaderParamp = (EncHeaderParam*)parameter;
		encHeaderParamp->buf += (ddrCh_save.vCoreCh) ? SDRAM_ADR_CH1_TOP:SDRAM_ADR_CH0_TOP;
	}
	else if( cmd == SET_ADDR_ENC_RC_REPORT_BASE ){
		UINT32 *val;
		val = (UINT32*)parameter;
		*val += (ddrCh_save.vCoreCh) ? SDRAM_ADR_CH1_TOP:SDRAM_ADR_CH0_TOP;
	}
	else if( cmd == ENC_SET_SEI_NAL_DATA ){
		HevcSEIDataEnc* seiDataEnc;
		seiDataEnc = (HevcSEIDataEnc*)parameter;
		seiDataEnc->prefixSeiNalAddr += (ddrCh_save.vCoreCh) ? SDRAM_ADR_CH1_TOP:SDRAM_ADR_CH0_TOP;
	}
	
	return ret;
}

INT32 H265_Wave_EncGetBitstreamBuffer(
EncHandle handle,			/**<  [Input] A encoder handle obtained from VPU_EncOpen()	*/
UINT32 *prdPrt,	/**<  [Output] A stream buffer read pointer for the current encoder instance */
UINT32 *pwrPtr,	/**<  [Output] A stream buffer write pointer for the current encoder instance */
UINT32 * size					/**<  [Output] A variable specifying the available space in bitstream buffer for the current encoder instance */
)
{
	INT32	ret;
	ret = VPU_EncGetBitstreamBuffer(handle, (PhysicalAddress *)prdPrt, (PhysicalAddress *)pwrPtr, (int *)size);

	return ret;
}

INT32 H265_Wave_EncStartOneFrame( UINT32 ch, EncHandle handle, EncParam * param )
{
	INT32	ret;
	
#if 1
#if 0
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->forceIPicture       = (%x)\n", ch, param->forceIPicture ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->skipPicture         = (%x)\n", ch, param->skipPicture ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->quantParam          = (%x)\n", ch, param->quantParam ));
//#endif
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->picStreamBufferAddr = (%x)\n", ch, param->picStreamBufferAddr ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->picStreamBufferSize = (%x)\n", ch, param->picStreamBufferSize ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->sourceFrame.bufY = (%x)\n", ch, param->sourceFrame->bufY ));
//	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->sourceFrame.updateFbInfo = (%x)\n", ch, param->sourceFrame->updateFbInfo ));
//#if 0
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->fieldRun            = (%x)\n", ch, param->fieldRun ));
	
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->setROI.mode   = (%x)\n", ch, param->setROI.mode  ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->setROI.number = (%x)\n", ch, param->setROI.number  ));
	{
		INT32	i;
		for( i=0;i < param->setROI.number;i++ ){
			H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->setROI.region[%d].left   = (%x)\n", ch, i, param->setROI.region[i].left ));
			H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->setROI.region[%d].top    = (%x)\n", ch, i, param->setROI.region[i].top ));
			H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->setROI.region[%d].right  = (%x)\n", ch, i, param->setROI.region[i].right ));
			H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->setROI.region[%d].bottom = (%x)\n", ch, i, param->setROI.region[i].bottom ));
			H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->setROI.qp[%d]            = (%x)\n", ch, i, param->setROI.qp[i] ));
		}
		
	}
	
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->ctuOptParam.roiEnable     = (%x)\n", ch, param->ctuOptParam.roiEnable  ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->ctuOptParam.roiDeltaQp    = (%x)\n", ch, param->ctuOptParam.roiDeltaQp  ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->ctuOptParam.mapEndian     = (%x)\n", ch, param->ctuOptParam.mapEndian  ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->ctuOptParam.mapStride     = (%x)\n", ch, param->ctuOptParam.mapStride  ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->ctuOptParam.addrRoiCtuMap = (%x)\n", ch, param->ctuOptParam.addrRoiCtuMap  ));
	
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->forcePicQpEnable   = (%x)\n", ch, param->forcePicQpEnable ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->forcePicQpI        = (%x)\n", ch, param->forcePicQpI ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->forcePicQpP        = (%x)\n", ch, param->forcePicQpP ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->forcePicQpB        = (%x)\n", ch, param->forcePicQpB ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->forcePicTypeEnable = (%x)\n", ch, param->forcePicTypeEnable ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->forcePicTypeEnable = (%x)\n", ch, param->forcePicTypeEnable ));
	
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->codeOption.implicitHeaderEncode = (%x)\n", ch, param->codeOption.implicitHeaderEncode ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->codeOption.encodeVCL            = (%x)\n", ch, param->codeOption.encodeVCL ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->codeOption.encodeVPS            = (%x)\n", ch, param->codeOption.encodeVPS ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->codeOption.encodeSPS            = (%x)\n", ch, param->codeOption.encodeSPS ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->codeOption.encodePPS            = (%x)\n", ch, param->codeOption.encodePPS ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->codeOption.encodeAUD            = (%x)\n", ch, param->codeOption.encodeAUD ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->codeOption.encodeEOS            = (%x)\n", ch, param->codeOption.encodeEOS ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->codeOption.encodeEOB            = (%x)\n", ch, param->codeOption.encodeEOB ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->codeOption.encodeVUI            = (%x)\n", ch, param->codeOption.encodeVUI ));
	
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->w4EncReportBaseAddr    = (%x)\n", ch, param->w4EncReportBaseAddr ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->useCurSrcAsLongtermPic = (%x)\n", ch, param->useCurSrcAsLongtermPic ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->useLongtermRef         = (%x)\n", ch, param->useLongtermRef ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->pts                    = (%x)\n", ch, param->pts ));
//#endif	
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->fcParam.ddrCh0StartAddrUmc  = (%x)\n", ch, param->fcParam.ddrCh0StartAddrUmc ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->fcParam.endianMode          = (%x)\n", ch, param->fcParam.endianMode ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->fcParam.pxtDdrChSel         = (%x)\n", ch, param->fcParam.pxtDdrChSel ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->fcParam.ddrReadMasterSelTdc = (%x)\n", ch, param->fcParam.ddrReadMasterSelTdc ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->fcParam.ddrReadTdcCh        = (%x)\n", ch, param->fcParam.ddrReadTdcCh ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->fcParam.modeSelTdc0         = (%x)\n", ch, param->fcParam.modeSelTdc0 ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->fcParam.modeSelTdc1         = (%x)\n", ch, param->fcParam.modeSelTdc1 ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->fcParam.thModeTdc0          = (%x)\n", ch, param->fcParam.thModeTdc0 ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->fcParam.thModeTdc1          = (%x)\n", ch, param->fcParam.thModeTdc1 ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->fcParam.ddrCh1StartAddrUmc  = (%x)\n", ch, param->fcParam.ddrCh1StartAddrUmc ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->fcParam.powerDownMode0      = (%x)\n", ch, param->fcParam.powerDownMode0 ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->fcParam.powerDownMode1      = (%x)\n", ch, param->fcParam.powerDownMode1 ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->fcParam.tdcTableAddrY       = (%x)\n", ch, param->fcParam.tdcTableAddrY ));
	H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->fcParam.tdcTableAddrC       = (%x)\n", ch, param->fcParam.tdcTableAddrC ));
//#if 0
	{
		UINT64	src_h;
		UINT64	src_l;
		UINT32	h;
		UINT32	l;
		src_h = (param->serialNum & 0xffffffff00000000);
		src_h = src_h >>32;
		h = (UINT32)src_h;
		src_l = (param->serialNum & 0x00000000ffffffff);
		l = (UINT32)src_l;
		H265_Info_Print(( "H265_Wave_EncStartOneFrame(%d) : param->serialNum = (%x)h,(%x)l\n", ch, h, l ));
		(void)regi_dump( (unsigned char *)&param->serialNum, 8, 1 );
	}
#endif
#endif
	
	if( param->picStreamBufferAddr >= SDRAM_ADR_CH1_TOP ){
		param->picStreamBufferAddr -= SDRAM_ADR_CH1_TOP;
	}
	else{
		param->picStreamBufferAddr -= SDRAM_ADR_CH0_TOP;
	}
	if( param->w4EncReportBaseAddr >= SDRAM_ADR_CH1_TOP ){
		param->w4EncReportBaseAddr -= SDRAM_ADR_CH1_TOP;
	}
	else{
		param->w4EncReportBaseAddr -= SDRAM_ADR_CH0_TOP;
	}
	
	if( param->fcParam.tdcTableAddrY >= SDRAM_ADR_CH1_TOP ){
		param->fcParam.tdcTableAddrY -= SDRAM_ADR_CH1_TOP;
	}
	else{
		param->fcParam.tdcTableAddrY -= SDRAM_ADR_CH0_TOP;
	}
	if( param->fcParam.tdcTableAddrC >= SDRAM_ADR_CH1_TOP ){
		param->fcParam.tdcTableAddrC -= SDRAM_ADR_CH1_TOP;
	}
	else{
		param->fcParam.tdcTableAddrC -= SDRAM_ADR_CH0_TOP;
	}
	if( param->ctuOptParam.roiEnable ){
		if( param->ctuOptParam.addrRoiCtuMap >= SDRAM_ADR_CH1_TOP ){
			param->ctuOptParam.addrRoiCtuMap -= SDRAM_ADR_CH1_TOP;
		}
		else{
			param->ctuOptParam.addrRoiCtuMap -= SDRAM_ADR_CH0_TOP;
		}
	}
	
	ret = VPU_EncStartOneFrame( handle, param );
	if( ret != RETCODE_SUCCESS ){
		H265_Err_Print(( "VPU_EncStartOneFrame(%d) error = (%d)\n", ch, ret ));
	}
	return ret;
}

INT32 H265_Wave_WaitInterrupt( UINT32 ch, Uint32 coreIdx, int timeout )
{
	INT32	int_reason;
	
	int_reason = VPU_WaitInterrupt( coreIdx, timeout );
//	H265_Info_Print(( "VPU_WaitInterrupt(%d) int_reason = (%d)\n", ch, int_reason ));
	return int_reason;
}

VOID H265_Wave_ClearInterrupt( UINT32 ch, Uint32 coreIdx )
{
	VPU_ClearInterrupt( coreIdx );
}

INT32 H265_Wave_EncGetOutputInfo( UINT32 ch, EncHandle handle, EncOutputInfo * info )
{
	INT32	ret;
	
	ret = VPU_EncGetOutputInfo( handle, info );
	if( ret != RETCODE_SUCCESS ){
//		H265_Err_Print(( "VPU_EncGetOutputInfo(%d) error = (%d), reason = (%x)\n", ch, ret, info->errorReason ));
	}
#if 0
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->bitstreamBuffer     = (%x)\n", ch, info->bitstreamBuffer ));
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->bitstreamSize       = (%x)\n", ch, info->bitstreamSize ));
//	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->bitstreamWrapAround = (%x)\n", ch, info->bitstreamWrapAround ));
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->picType             = (%d)\n", ch, info->picType ));
//	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->numOfSlices         = (%d)\n", ch, info->numOfSlices ));
//	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->reconFrameIndex     = (%d)\n", ch, info->reconFrameIndex ));
#if 0
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->rdPtr               = (%d)\n", ch, info->rdPtr ));
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->wrPtr               = (%d)\n", ch, info->wrPtr ));
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->picSkipped          = (%d)\n", ch, info->picSkipped ));
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->numOfIntra          = (%d)\n", ch, info->numOfIntra ));
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->numOfMerge          = (%d)\n", ch, info->numOfMerge ));
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->numOfSkipBlock      = (%d)\n", ch, info->numOfSkipBlock ));
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->avgCtuQp            = (%d)\n", ch, info->avgCtuQp ));
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->encPicByte          = (%d)\n", ch, info->encPicByte ));
#endif
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->encGopPicIdx        = (%d)\n", ch, info->encGopPicIdx ));
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->encPicPoc           = (%d)\n", ch, info->encPicPoc ));
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->encSrcIdx           = (%d)\n", ch, info->encSrcIdx ));
//	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->encVclNal           = (%d)\n", ch, info->encVclNal ));
//	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->encPicCnt           = (%d)\n", ch, info->encPicCnt ));
//	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->frameCycle          = (%d)\n", ch, info->frameCycle ));
//	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->feedingSignal       = (%d)\n", ch, info->feedingSignal ));
#endif
#if 0
	{
		UINT64	src_h;
		UINT64	src_l;
		UINT32	h;
		UINT32	l;
		src_h = (info->pts & 0xffffffff00000000);
		src_h = (src_h >> 32);
		h = (UINT32)src_h;
		src_l = (info->pts & 0x00000000ffffffff);
		l = (UINT32)src_l;
		H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->pts                 = h(%x),l(%x)\n", ch, h, l ));
		(void)regi_dump( (unsigned char *)&info->pts, 8, 1 );
	}
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->encodedCoreIdx      = (%d)\n", ch, info->encodedCoreIdx ));
	H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->encInstIdx          = (%d)\n", ch, info->encInstIdx ));
	{
		UINT64	src_h;
		UINT64	src_l;
		UINT32	h;
		UINT32	l;
		src_h = (info->serialNum & 0xffffffff00000000);
		src_h = (src_h >> 32);
		h = (UINT32)src_h;
		src_l = (info->serialNum & 0x00000000ffffffff);
		l = (UINT32)src_l;
		H265_Info_Print(( "VPU_EncGetOutputInfo(%d) : info->serialNum           = h(%x),l(%x)\n", ch, h, l ));
		(void)regi_dump( (unsigned char *)&info->serialNum, 8, 1 );
	}
#endif
	info->bitstreamBuffer += (ddrCh_save.vCoreCh) ? SDRAM_ADR_CH1_TOP:SDRAM_ADR_CH0_TOP;
	if( info->bitstreamWrapAround == 1){
		info->bitstreamWrapAroundPtr += (ddrCh_save.vCoreCh) ? SDRAM_ADR_CH1_TOP:SDRAM_ADR_CH0_TOP;
	}
	info->nalSizeReportAddr += (ddrCh_save.vCoreCh) ? SDRAM_ADR_CH1_TOP:SDRAM_ADR_CH0_TOP;
	return ret;
}

T_H265_WAVE_QUEUE* H265_Wave_Queue_Create( UINT32 itemCount, UINT32 itemSize )
{
	T_H265_WAVE_QUEUE*		queue = NULL; 
	OS_USER_ER	ercd;
	OS_USER_VP	blk = (OS_USER_VP)0;
	
	ercd = OS_User_Pget_Mpl( MPKID_WAVE_QUEUE, (INT32)sizeof(T_H265_WAVE_QUEUE), &blk );
	if( ercd != D_OS_USER_E_OK ){
		H265_Info_Print(( "H265_Wave_Queue_Create : OS_User_Pget_Mpl err ercd = (%x), size = (%x)\n", ercd, sizeof(T_H265_WAVE_QUEUE) ));
		return NULL;
	}
	queue = (T_H265_WAVE_QUEUE*)blk;
//	H265_Info_Print(( "que_malloc : addr = (%x), size = (%x)\n", blk, sizeof(T_H265_WAVE_QUEUE) ));
	
//	if ((queue=(Queue *)osal_malloc(sizeof(Queue))) == NULL)
//		return NULL;
	queue->size   = itemCount;
	queue->itemSize = itemSize;
	queue->count  = 0;
	queue->front  = 0;
	queue->rear   = 0;
	
	ercd = OS_User_Pget_Mpl( MPKID_WAVE_QUEUE, (INT32)(itemCount*itemSize), &blk );
	if( ercd != D_OS_USER_E_OK ){
		H265_Info_Print(( "H265_Wave_Queue_Create : OS_User_Pget_Mpl err ercd = (%x), size = (%x)\n", ercd, (itemCount*itemSize) ));
		blk = 0;
		return NULL;
	}
	queue->buffer = (VOID *)blk;
//	queue->buffer = (UINT8*)osal_malloc(itemCount*itemSize);
//	queue->lock = NULL;
	queue->lock = 0;
//	H265_Info_Print(( "que_malloc : addr = (%x), size = (%x)\n", blk, (itemCount*itemSize) ));

	
    return queue;
}

VOID H265_Wave_Queue_Destroy( T_H265_WAVE_QUEUE* queue )
{
	if( queue == NULL ){
		return;
	}
	
	if( queue->buffer ){
//		osal_free(queue->buffer);
		(VOID)OS_User_Rel_Mpl( MPKID_WAVE_QUEUE, (OS_USER_VP)(queue->buffer) );
//		H265_Info_Print(( "vdi_user_free : addr = (%x)\n", queue->buffer ));
	}
	if( queue->lock ){
//		VpuMutex_Destroy(queue->lock);
		queue->lock = 0;
	}
//	osal_free(queue);
	(VOID)OS_User_Rel_Mpl( MPKID_WAVE_QUEUE, (OS_USER_VP)(queue) );
//	H265_Info_Print(( "vdi_user_free : addr = (%x)\n", queue ));
}

BOOL H265_Wave_Queue_Enqueue( T_H265_WAVE_QUEUE* queue, VOID* data )
{
	UCHAR*    ptr;
	UINT32    offset;
	
	if( queue == NULL ){
		return FALSE;
	}
	
    /* Queue is full */
	if( queue->count == queue->size ){
		return -1;
	}
	
	if( queue->lock ){
//		VpuMutex_Lock(queue->lock);
		OS_User_Wai_Sem( queue->lock );
	}
	offset = queue->rear * queue->itemSize;
	
	ptr = &queue->buffer[offset];
//	osal_memcpy(ptr, data, queue->itemSize);
	memcpy(ptr, data, queue->itemSize);
	queue->rear++;
	queue->rear %= queue->size;
	queue->count++;
	if( queue->lock ){
//		VpuMutex_Unlock(queue->lock);
		OS_User_Sig_Sem( queue->lock );
	}
	
	return TRUE;
}

VOID* H265_Wave_Queue_Dequeue( T_H265_WAVE_QUEUE* queue )
{
	VOID*  data;
	UINT32 offset;
	
	if( queue == NULL ){
		return NULL;
	}
	
	/* Queue is empty */
	if( queue->count == 0 ){
		return NULL;
	}
	
	if( queue->lock ){
//		VpuMutex_Lock(queue->lock);
		OS_User_Wai_Sem( queue->lock );
	}
	offset = queue->front * queue->itemSize;
	data   = (void*)&queue->buffer[offset];
	queue->front++;
	queue->front %= queue->size;
	queue->count--;
	if( queue->lock ){
//		VpuMutex_Unlock(queue->lock);
		OS_User_Sig_Sem( queue->lock );
	}
	return data;
}

UINT32 H265_Wave_Queue_Get_Cnt( T_H265_WAVE_QUEUE* queue )
{
	UINT32    cnt;
	
	if (queue == NULL){
		return 0;
	}
	
	if( queue->lock ){
//		VpuMutex_Lock(queue->lock);
		OS_User_Wai_Sem( queue->lock );
	}
	cnt = queue->count;
	if( queue->lock ){
//		VpuMutex_Unlock(queue->lock);
		OS_User_Sig_Sem( queue->lock );
	}
	return cnt;
}

INT32 H265_Wave_activeParameterSEIEncode(sei_active_parameter_t *sap, hrd_t *hrd, UINT8 *pBuffer)
{
	spp_enc_context spp;
	UINT32 code;
	const INT32 layer_id = 0;
	const INT32 temporal_id_plus_one = 1;
	const INT32 payload_type = (INT32)ACTIVE_PARAMETER_SETS;

	INT32 put_bit_byte_size;
	INT32 payload_bit_size;
	INT32 payload_byte_size;
	int i;

	payload_bit_size = 0;
	payload_byte_size = 0;
		

	sap->active_video_parameter_set_id = 0; // vps_video_parameter_set_id of the VPS. wave420 is 0
	sap->self_contained_cvs_flag = 0;
	sap->no_parameter_set_update_flag = 0;
	sap->num_sps_ids_minus1 = 0;
	sap->active_seq_parameter_set_id[0] = 0;// sps_seq_parameter_set_id of the SPS. wave420 is 0

	spp = spp_enc_init(pBuffer, SEI_NAL_DATA_BUF_SIZE, 1);

	// put start code
	spp_enc_put_nal_byte(spp, 1, 4);

	// put nal header
	code = (SNT_PREFIX_SEI << 9) | (layer_id << 3) | temporal_id_plus_one;
	spp_enc_put_nal_byte(spp, code, 2);

	// put payload type
	spp_enc_put_nal_byte(spp, payload_type, 1);

	// put payload size
	payload_bit_size = 4/*active_video_parameter_set_id*/ + 1/*self_contained_cvs_flag*/ + 1/*no_parameter_set_update_flag*/;
	payload_bit_size += spp_enc_get_ue_bit_size(sap->num_sps_ids_minus1);
	for (i=0; i <= sap->num_sps_ids_minus1; i++)
		payload_bit_size+= spp_enc_get_ue_bit_size(sap->active_seq_parameter_set_id[i]);

	payload_byte_size = ((payload_bit_size+7)/8);
	spp_enc_put_nal_byte(spp, payload_byte_size, 1); 

	
	spp_enc_put_bits(spp, sap->active_video_parameter_set_id, 4); 
	spp_enc_put_bits(spp, sap->self_contained_cvs_flag, 1); 
	spp_enc_put_bits(spp, sap->no_parameter_set_update_flag, 1); 
	spp_enc_put_ue(spp, sap->num_sps_ids_minus1); 		
	for (i=0; i <= sap->num_sps_ids_minus1; i++)
	{
		spp_enc_put_ue(spp, sap->active_seq_parameter_set_id[i]); 		
	}
	
	if (payload_bit_size & 7)
	{
		spp_enc_put_bits(spp, 1/*payload_bit_equal_to_one*/, 1);
		spp_enc_put_bits(spp, 0/*payload_bit_equal_to_zero*/,  (8 - (payload_bit_size & 7)-1));
	}
	spp_enc_put_byte_align(spp, 1);
	spp_enc_flush(spp);

	put_bit_byte_size = spp_enc_get_nal_cnt(spp);

	spp_enc_deinit(spp);
	return put_bit_byte_size;
}

INT32 H265_Wave_picTimingSEIEncode(sei_pic_timing_t *spt, hrd_t *hrd, UINT8 *pBuffer)
{
	spp_enc_context spp;
	UINT32 code;
	const INT32 layer_id = 0;
	const INT32 temporal_id_plus_one = 1;
	const INT32 payload_type = (INT32)PICTURE_TIMING;

	INT32 put_bit_byte_size;
	INT32 payload_bit_size;
	INT32 payload_byte_size;
	int i;


	payload_bit_size = 0;
	payload_byte_size = 0;
	if (hrd->nal_hrd_parameters_present_flag || hrd->vcl_hrd_parameters_present_flag)
		spt->CpbDpbDelaysPresentFlag = 1;

	spp = spp_enc_init(pBuffer, SEI_NAL_DATA_BUF_SIZE, 1);

	// put start code
	spp_enc_put_nal_byte(spp, 1, 4);

	// put nal header
	code = (SNT_PREFIX_SEI << 9) | (layer_id << 3) | temporal_id_plus_one;
	spp_enc_put_nal_byte(spp, code, 2);

	// put payload type
	spp_enc_put_nal_byte(spp, payload_type, 1);

	// put payload size
	payload_bit_size = 4/*pic_struct*/ + 2/*source_scan_type*/ + 1/*duplicate_flag*/;
	if (spt->CpbDpbDelaysPresentFlag)
	{
		payload_bit_size += (hrd->au_cpb_removal_delay_length_minus1 + 1); //spt->au_cpb_removal_delay_minus1
		payload_bit_size += (hrd->dpb_output_delay_length_minus1+ 1);		// spt->pic_dpb_output_delay
		if (hrd->sub_pic_hrd_params_present_flag)
			payload_bit_size += (hrd->dpb_output_delay_du_length_minus1 + 1); //spt->pic_dpb_output_du_delay
		if (hrd->sub_pic_hrd_params_present_flag &&
			hrd->sub_pic_cpb_params_in_pic_timing_sei_flag)
		{
			payload_bit_size += spp_enc_get_ue_bit_size(spt->num_decoding_units_minus1);	//spt->num_decoding_units_minus1
			payload_bit_size += 1;	// spt->du_common_cpb_removal_delay_flag
			if (spt->du_common_cpb_removal_delay_flag)
				payload_bit_size +=  (hrd->du_cpb_removal_delay_increment_length_minus1 + 1);	//spt->du_common_cpb_removal_delay_increment_minus1
			for (i=0; i <= spt->num_decoding_units_minus1; i++)
			{
				payload_bit_size += spp_enc_get_ue_bit_size(spt->num_nalus_in_du_minus1[i]);	//spt->num_nalus_in_du_minus1[i]
				if (!spt->du_common_cpb_removal_delay_flag && i < spt->num_decoding_units_minus1)
				{
					payload_bit_size += (hrd->du_cpb_removal_delay_increment_length_minus1 + 1);	// spt->du_cpb_removal_delay_increment_minus1[i]
				}
			}
		}
	}
	payload_byte_size = ((payload_bit_size+7)/8);
	spp_enc_put_nal_byte(spp, payload_byte_size, 1); 

	// put sei_pic_timing
	spt->pic_struct = 0;
	spt->source_scan_type = 1;
	spt->duplicate_flag = 0;

	spp_enc_put_bits(spp, spt->pic_struct, 4); 
	spp_enc_put_bits(spp, spt->source_scan_type, 2); 
	spp_enc_put_bits(spp, spt->duplicate_flag, 1); 		

	if (spt->CpbDpbDelaysPresentFlag)
	{
		spp_enc_put_bits(spp, spt->au_cpb_removal_delay_minus1, (hrd->au_cpb_removal_delay_length_minus1 + 1));
		spp_enc_put_bits(spp, spt->pic_dpb_output_delay, (hrd->dpb_output_delay_length_minus1+ 1));
		if (hrd->sub_pic_hrd_params_present_flag)
			spp_enc_put_bits(spp, spt->pic_dpb_output_du_delay, (hrd->dpb_output_delay_du_length_minus1 + 1));
		if (hrd->sub_pic_hrd_params_present_flag &&
			hrd->sub_pic_cpb_params_in_pic_timing_sei_flag)
		{
			spp_enc_put_ue(spp, spt->num_decoding_units_minus1);
			spp_enc_put_bits(spp, spt->du_common_cpb_removal_delay_flag, 1);
			if (spt->du_common_cpb_removal_delay_flag)
				spp_enc_put_bits(spp, spt->du_common_cpb_removal_delay_increment_minus1, (hrd->du_cpb_removal_delay_increment_length_minus1 + 1));
			for (i=0; i <= spt->num_decoding_units_minus1; i++)
			{
				spp_enc_put_ue(spp, spt->num_nalus_in_du_minus1[i]);
				if (!spt->du_common_cpb_removal_delay_flag && i < spt->num_decoding_units_minus1)
				{
					spp_enc_put_bits(spp, spt->du_cpb_removal_delay_increment_minus1[i], (hrd->du_cpb_removal_delay_increment_length_minus1 + 1));
				}
			}
				
		}

	}
	if (payload_bit_size & 7)
	{
		spp_enc_put_bits(spp, 1/*payload_bit_equal_to_one*/, 1);
		spp_enc_put_bits(spp, 0/*payload_bit_equal_to_zero*/,  (8 - (payload_bit_size & 7)-1));
	}
	spp_enc_put_byte_align(spp, 1);
	spp_enc_flush(spp);

	put_bit_byte_size = spp_enc_get_nal_cnt(spp);

	spp_enc_deinit(spp);
	return put_bit_byte_size;
}

INT32 H265_Wave_bufferingPeriodSeiEncode(sei_buffering_period_t *sbp, hrd_t *hrd, UINT8 *pBuffer)
{
	spp_enc_context spp;
	UINT32 code;
	const INT32 layer_id = 0;
    const INT32 temporal_id_plus_one = 1;
	const INT32 payload_type = (INT32)BUFFERING_PERIOD;
	int i;
	INT32 put_bit_byte_size;
	INT32 payload_bit_size;
	INT32 payload_byte_size;

	payload_bit_size = 0;
	payload_byte_size = 0;


	sbp->nal_initial_cpb_removal_delay[0] = 2229;
	sbp->nal_initial_cpb_removal_offset[0] = 0;

	sbp->sub_pic_hrd_params_present_flag = hrd->sub_pic_hrd_params_present_flag;
	sbp->au_cpb_removal_delay_length_minus1 =  hrd->au_cpb_removal_delay_length_minus1;
	sbp->initial_cpb_removal_delay_length_minus1 =  hrd->initial_cpb_removal_delay_length_minus1;
	sbp->CpbCnt =  hrd->cpb_cnt_minus1[0]+1;
	sbp->NalHrdBpPresentFlag =  hrd->nal_hrd_parameters_present_flag;
	sbp->VclHrdBpPresentFlag =  hrd->vcl_hrd_parameters_present_flag;

	payload_bit_size += spp_enc_get_ue_bit_size(sbp->bp_seq_parameter_set_id);
	if (!sbp->sub_pic_hrd_params_present_flag)
		payload_bit_size += 1;	//irap_cpb_params_present_flag
	if (sbp->irap_cpb_params_present_flag)
	{
		payload_bit_size += (sbp->au_cpb_removal_delay_length_minus1+1);	//cpb_delay_offset
		payload_bit_size += (sbp->au_cpb_removal_delay_length_minus1+1);	//dpb_delay_offset
	}	
	payload_bit_size += 1;	//concatenation_flag
	payload_bit_size += (sbp->au_cpb_removal_delay_length_minus1+1); //au_cpb_removal_delay_delta_minus1
	if (sbp->NalHrdBpPresentFlag)
	{
		for (i=0; i < (int)sbp->CpbCnt; i++)
		{
			payload_bit_size += (sbp->initial_cpb_removal_delay_length_minus1+1);	//nal_initial_cpb_removal_delay
			payload_bit_size += (sbp->initial_cpb_removal_delay_length_minus1+1);	//nal_initial_cpb_removal_offset

			if (sbp->sub_pic_hrd_params_present_flag || sbp->irap_cpb_params_present_flag)
			{
				payload_bit_size += (sbp->initial_cpb_removal_delay_length_minus1+1);	// nal_initial_alt_cpb_removal_delay
				payload_bit_size += (sbp->initial_cpb_removal_delay_length_minus1+1);	//nal_initial_alt_cpb_removal_offset
			}
		}
	}
	if (sbp->VclHrdBpPresentFlag )
	{
		for (i=0; i < (int)sbp->CpbCnt; i++)
		{
			payload_bit_size += (sbp->initial_cpb_removal_delay_length_minus1+1);	//vcl_initial_cpb_removal_delay
			payload_bit_size += (sbp->initial_cpb_removal_delay_length_minus1+1);	//vcl_initial_cpb_removal_offset

			if (sbp->sub_pic_hrd_params_present_flag || sbp->irap_cpb_params_present_flag)
			{
				payload_bit_size += (sbp->initial_cpb_removal_delay_length_minus1+1);	//vcl_initial_alt_cpb_removal_delay
				payload_bit_size += (sbp->initial_cpb_removal_delay_length_minus1+1);	//vcl_initial_alt_cpb_removal_offset
			}
		}
	}

	spp = spp_enc_init(pBuffer, SEI_NAL_DATA_BUF_SIZE, 1);

	// put start code
	spp_enc_put_nal_byte(spp, 1, 4);

	// put nal header
	code = (SNT_PREFIX_SEI << 9) | (layer_id << 3) | temporal_id_plus_one;
	spp_enc_put_nal_byte(spp, code, 2);

	// put payload type
	spp_enc_put_nal_byte(spp, payload_type, 1);

	// put payload size
	payload_byte_size = ((payload_bit_size+7)/8);
	spp_enc_put_nal_byte(spp, payload_byte_size, 1); 

	// put buffering period
	spp_enc_put_ue(spp, sbp->bp_seq_parameter_set_id);
	if (!sbp->sub_pic_hrd_params_present_flag)
		spp_enc_put_bits(spp, sbp->irap_cpb_params_present_flag, 1);
	if (sbp->irap_cpb_params_present_flag)
	{
		spp_enc_put_bits(spp, sbp->dpb_delay_offset, (sbp->au_cpb_removal_delay_length_minus1+1));
		spp_enc_put_bits(spp, sbp->dpb_delay_offset, (sbp->au_cpb_removal_delay_length_minus1+1));
	}
	spp_enc_put_bits(spp, sbp->concatenation_flag, 1); 
	spp_enc_put_bits(spp, sbp->au_cpb_removal_delay_delta_minus1, (sbp->au_cpb_removal_delay_length_minus1+1)); 
	if (sbp->NalHrdBpPresentFlag)
	{
		for (i=0; i < (int)sbp->CpbCnt; i++)
		{
			spp_enc_put_bits(spp, sbp->nal_initial_cpb_removal_delay[i], (sbp->initial_cpb_removal_delay_length_minus1+1));
			spp_enc_put_bits(spp, sbp->nal_initial_cpb_removal_offset[i], (sbp->initial_cpb_removal_delay_length_minus1+1));
			if (sbp->sub_pic_hrd_params_present_flag || sbp->irap_cpb_params_present_flag)
			{
				spp_enc_put_bits(spp, sbp->nal_initial_alt_cpb_removal_delay[i], (sbp->initial_cpb_removal_delay_length_minus1+1));
				spp_enc_put_bits(spp, sbp->nal_initial_alt_cpb_removal_offset[i], (sbp->initial_cpb_removal_delay_length_minus1+1));
			}
		}
	}
	if (sbp->VclHrdBpPresentFlag)
	{
		for (i=0; i < (int)sbp->CpbCnt; i++)
		{
			spp_enc_put_bits(spp, sbp->vcl_initial_cpb_removal_delay[i], (sbp->initial_cpb_removal_delay_length_minus1+1));
			spp_enc_put_bits(spp, sbp->vcl_initial_cpb_removal_offset[i], (sbp->initial_cpb_removal_delay_length_minus1+1));
			if (sbp->sub_pic_hrd_params_present_flag || sbp->irap_cpb_params_present_flag)
			{
				spp_enc_put_bits(spp, sbp->vcl_initial_alt_cpb_removal_delay[i], (sbp->initial_cpb_removal_delay_length_minus1+1));
				spp_enc_put_bits(spp, sbp->vcl_initial_alt_cpb_removal_offset[i], (sbp->initial_cpb_removal_delay_length_minus1+1));
			}
		}
	}		

	if (payload_bit_size & 7)
	{
		spp_enc_put_bits(spp, 1/*payload_bit_equal_to_one*/, 1);
		spp_enc_put_bits(spp, 0/*payload_bit_equal_to_zero*/,  (8 - (payload_bit_size & 7)-1));
	}


	spp_enc_put_byte_align(spp, 1);
	spp_enc_flush(spp);

	put_bit_byte_size = spp_enc_get_nal_cnt(spp);

	spp_enc_deinit(spp);

	return put_bit_byte_size;
}

INT32 H265_Wave_EncodePrefixSEI(BOOL on_pictstruct_flag, T_H265_WAVE_SEI_ACTIVE_PARAMETER *sap_info, T_H265_WAVE_SEI_PIC_TIMING *spt_info, BOOL on_buffer_ctrl_flag, T_H265_WAVE_SEI_BUFFERING_PERIOD *sbp_info, hrd_t *hrd, UINT8 *pBuffer)
{
	UINT8 *ptrBuffer;
	INT32 byteSize;
	
	byteSize = 0;
	sap_info->size = 0;
	sap_info->addr = 0;
	spt_info->size = 0;
	spt_info->addr = 0;
	sbp_info->size = 0;
	sbp_info->addr = 0;
	
	if (on_pictstruct_flag == TRUE || on_buffer_ctrl_flag == TRUE)
	{
 		ptrBuffer = (UINT8 *)(pBuffer + byteSize);
		sap_info->size = H265_Wave_activeParameterSEIEncode(&sap_info->sap, hrd, ptrBuffer);
		sap_info->addr = (UINT32)ptrBuffer;
		byteSize += sap_info->size;
		
		ptrBuffer = (UINT8 *)(pBuffer + byteSize);
		spt_info->size = H265_Wave_picTimingSEIEncode(&spt_info->spt, hrd, ptrBuffer);
		spt_info->addr = (UINT32)ptrBuffer;
		byteSize += spt_info->size;
	}
	
	if (on_buffer_ctrl_flag == TRUE)
	{
		ptrBuffer = (UINT8 *)(pBuffer + byteSize);
		sbp_info->size = H265_Wave_bufferingPeriodSeiEncode(&sbp_info->sbp, hrd, ptrBuffer);
		sbp_info->addr = (UINT32)ptrBuffer;
		byteSize += sbp_info->size;
	}
	
	return byteSize;
}

static void h265_wave_hrdEncode(spp_enc_context spp, hrd_t *hrd, UINT32 vps_max_sub_layers_minus1)
{
	INT32 layer;
	INT32 cpb;

	hrd->commonInfPresentFlag = 1;
	hrd->vps_max_sub_layers_minus1 = vps_max_sub_layers_minus1;

	// sample of configuration 
	hrd->nal_hrd_parameters_present_flag = 1;
	hrd->vcl_hrd_parameters_present_flag = 0;	
	hrd->initial_cpb_removal_delay_length_minus1 = 31;
	hrd->au_cpb_removal_delay_length_minus1 = 23;
	hrd->dpb_output_delay_length_minus1 = 23;

	// layer 0
	hrd->bit_rate_value_minus1[0][0] = 21033;
	hrd->cpb_size_value_minus1[0][0] = 63520;


		
	// the other parameters can be configured according to the target application.
	if (hrd->commonInfPresentFlag)
	{
		spp_enc_put_bits(spp, hrd->nal_hrd_parameters_present_flag, 1);
		spp_enc_put_bits(spp, hrd->vcl_hrd_parameters_present_flag, 1);

		if (hrd->nal_hrd_parameters_present_flag || hrd->vcl_hrd_parameters_present_flag)
		{
			spp_enc_put_bits(spp, hrd->sub_pic_hrd_params_present_flag, 1);
			if (hrd->sub_pic_hrd_params_present_flag)
			{
				spp_enc_put_bits(spp, hrd->tick_divisor_minus2, 8);
				spp_enc_put_bits(spp, hrd->du_cpb_removal_delay_increment_length_minus1, 5);
				spp_enc_put_bits(spp, hrd->sub_pic_cpb_params_in_pic_timing_sei_flag, 1);
				spp_enc_put_bits(spp, hrd->dpb_output_delay_du_length_minus1, 5);
			}

			spp_enc_put_bits(spp, hrd->bit_rate_scale, 4);
						
			spp_enc_put_bits(spp, hrd->cpb_size_scale, 4);

			if (hrd->sub_pic_hrd_params_present_flag)
			{
				spp_enc_put_bits(spp, hrd->cpb_size_du_scale, 4);
			}

			spp_enc_put_bits(spp, hrd->initial_cpb_removal_delay_length_minus1, 5);
			spp_enc_put_bits(spp, hrd->au_cpb_removal_delay_length_minus1, 5);
			spp_enc_put_bits(spp, hrd->dpb_output_delay_length_minus1, 5);

		
		}
	}

	for (layer=0; layer <= (INT32)hrd->vps_max_sub_layers_minus1; layer++)
	{
		spp_enc_put_bits(spp, hrd->fixed_pic_rate_general_flag[layer], 1);

		if (!hrd->fixed_pic_rate_general_flag[layer])
			spp_enc_put_bits(spp, hrd->fixed_pic_rate_within_cvs_flag[layer], 1);
		
		if (hrd->fixed_pic_rate_within_cvs_flag[layer])
			spp_enc_put_ue(spp, hrd->elemental_duration_in_tc_minus1[layer]);
		else
			spp_enc_put_bits(spp, hrd->low_delay_hrd_flag[layer], 1);
		
		if (!hrd->low_delay_hrd_flag[layer])
			spp_enc_put_ue(spp, hrd->cpb_cnt_minus1[layer]);
		
		if ((layer==0 && hrd->nal_hrd_parameters_present_flag) || (layer==1 && hrd->vcl_hrd_parameters_present_flag))
		{
			for(cpb=0; cpb <= (INT32)hrd->cpb_cnt_minus1[layer]; cpb++ )
			{
				spp_enc_put_ue(spp, hrd->bit_rate_value_minus1[cpb][layer]);

				spp_enc_put_ue(spp, hrd->cpb_size_value_minus1[cpb][layer]);

				if (hrd->sub_pic_hrd_params_present_flag)
				{
					spp_enc_put_ue(spp, hrd->cpb_size_du_value_minus1[cpb][layer]);

					spp_enc_put_ue(spp, hrd->bit_rate_du_value_minus1[cpb][layer]);
				}
				spp_enc_put_bits(spp, hrd->cbr_flag[cpb][layer], 1);					
			}
		}
	}
}



BOOL H265_Wave_EncodeHRDInVUI(hrd_t *hrd, vui_t *vui, UINT8 *pBuffer, INT32 *pByteSize, INT32 *pBitSize, double dframeRate)
{
	spp_enc_context spp;
	INT32 put_bit_byte_size;
	INT32 put_bit_bit_size;

	// the other parameters can be configured according to the target application.
	spp = spp_enc_init(pBuffer, VUI_HRD_RBSP_BUF_SIZE, 0);

	spp_enc_put_bits(spp, vui->aspect_ratio_info_present_flag, 1); 
	if (vui->aspect_ratio_info_present_flag)
	{
		spp_enc_put_bits(spp, vui->aspect_ratio_idc, 8); 

		if (vui->aspect_ratio_idc == EXTENDED_SAR)
		{
			spp_enc_put_bits(spp, vui->sar_width, 16); 
			spp_enc_put_bits(spp, vui->sar_height, 16); 
		}
	}

	spp_enc_put_bits(spp, vui->overscan_info_present_flag, 1); 
	if (vui->overscan_info_present_flag)
	{
		spp_enc_put_bits(spp, vui->overscan_appropriate_flag, 1); 
	}

	spp_enc_put_bits(spp, vui->video_signal_type_present_flag, 1); 
	if (vui->video_signal_type_present_flag)
	{
		spp_enc_put_bits(spp, vui->video_format, 3); 
		spp_enc_put_bits(spp, vui->video_full_range_flag, 1); 
		spp_enc_put_bits(spp, vui->colour_description_present_flag, 1); 

		if (vui->colour_description_present_flag)
		{
			spp_enc_put_bits(spp, vui->colour_primaries, 8); 
			spp_enc_put_bits(spp, vui->transfer_characteristics, 8); 
			spp_enc_put_bits(spp, vui->matrix_coeffs, 8); 
		}
	}

	spp_enc_put_bits(spp, vui->chroma_loc_info_present_flag, 1); 
	if (vui->chroma_loc_info_present_flag)
	{
		spp_enc_put_ue(spp, vui->chroma_sample_loc_type_top_field);
		spp_enc_put_ue(spp, vui->chroma_sample_loc_type_bottom_field);
	}

	spp_enc_put_bits(spp, vui->neutral_chroma_indication_flag, 1); 
	spp_enc_put_bits(spp, vui->field_seq_flag, 1); 
	spp_enc_put_bits(spp, vui->frame_field_info_present_flag, 1); 
	spp_enc_put_bits(spp, vui->default_display_window_flag, 1); 
	if (vui->default_display_window_flag)
	{
		spp_enc_put_ue(spp, vui->def_disp_win_left_offset);
		spp_enc_put_ue(spp, vui->def_disp_win_right_offset);
		spp_enc_put_ue(spp, vui->def_disp_win_top_offset);
		spp_enc_put_ue(spp, vui->def_disp_win_bottom_offset);
	}
	spp_enc_put_bits(spp, vui->vui_timing_info_present_flag, 1); 
	if (vui->vui_timing_info_present_flag)
	{
		spp_enc_put_bits(spp, vui->vui_num_units_in_tick, 32); 
		spp_enc_put_bits(spp, vui->vui_time_scale, 32); 
		spp_enc_put_bits(spp, vui->vui_poc_proportional_to_timing_flag, 1); 
		if (vui->vui_poc_proportional_to_timing_flag)
		{
			spp_enc_put_ue(spp, vui->vui_num_ticks_poc_diff_one_minus1); 
		}

		spp_enc_put_bits(spp, vui->vui_hrd_parameters_present_flag, 1); 
#define SAMPLE_SPS_MAX_SUB_LAYERS_MINUS1 0
		if (vui->vui_hrd_parameters_present_flag)
			h265_wave_hrdEncode(spp, hrd, SAMPLE_SPS_MAX_SUB_LAYERS_MINUS1);	
	}

	spp_enc_put_bits(spp, vui->bitstream_restriction_flag, 1); 
	if (vui->bitstream_restriction_flag)
	{
		spp_enc_put_bits(spp, vui->tiles_fixed_structure_flag, 1);
		spp_enc_put_bits(spp, vui->motion_vectors_over_pic_boundaries_flag, 1);
		spp_enc_put_bits(spp, vui->restricted_ref_pic_lists_flag, 1);
		spp_enc_put_ue(spp, vui->min_spatial_segmentation_idc);
		spp_enc_put_ue(spp, vui->max_bytes_per_pic_denom);
		spp_enc_put_ue(spp, vui->max_bits_per_min_cu_denom);
		spp_enc_put_ue(spp, vui->log2_max_mv_length_horizontal);
		spp_enc_put_ue(spp, vui->log2_max_mv_length_vertical);
	}



	spp_enc_flush(spp);		


	put_bit_byte_size = spp_enc_get_nal_cnt(spp);
	put_bit_bit_size = spp_enc_get_rbsp_bit(spp);

	if (pByteSize)
		*pByteSize = put_bit_byte_size;

	if (pBitSize)
		*pBitSize = put_bit_bit_size;

	spp_enc_deinit(spp);
	return 1;
}

#if 1
// group_a_ch		= 0;	// Current Luma              read  SNI FC (PRP)
// 							// Current Chroma            read  SNI FC (PRP)
// group_b_ch		= 0;	// Comp table                read  SNI FC
// group_c_ch		= 0;	// Sub-sampled ref.          read  ME
// 							// Sub-sampled ref.          write Down-scaler
// 							// Coded bitstream           Write EntropyCoder
// group_d_ch		= 0;	// Compressed Luma ref.      read  FBD/Decompressor
// 							// Compressed Chroma ref.    read  FBD/Decompressor
// 							// Compressed Luma ref.      Write FBC/Compressor
// 							// Compressed Chroma ref.    Write FBC/Compressor
// group_e_ch		= 0;	// Offset table for Luma ref.    Read  FBD/Decompressor
// 							// Offset table for Chroma ref.  Read  FBD/Decompressor
// 							// Offset table for Luma ref.    Write FBD/Compressor
// 							// Offset table for Chroma ref.  Write FBD/Compressor
// group_g_ch		= 0;	// VPU                           R/W   VPU
//
//
// VBB_SNI_FC_PRP(fcPrpCh) : group_a_ch, group_b_ch
// VBB_FBC(fbcCh)          : group_d_ch, group_e_ch
// VBB_VCORE(vCoreCh)      : group_c_ch
// VBB_VCPU(vCPUCh)        : group_g_ch
//
// mvColSize      : group_c_ch
// fbcYTblSize    : group_e_ch
// fbcCTblSize    : group_e_ch
// subSampledSize : group_c_ch

#define WAVE4_SUBSAMPLED_ONE_SIZE(_w, _h)   ((((_w/4)+15)&~15)*(((_h/4)+7)&~7))

VOID H265_Wave_VmemSize_Calculation( UINT32 ch, EncHandle handle, EncInitialInfo* encInitInfo )
{
	INT32	bufWidth, bufHeight, count;
	INT32	mvColSize;
	INT32	fbcYTblSize, fbcCTblSize;
	INT32	subSampledSize;
	INT32	ch0_total_size = 0;
	INT32	ch1_total_size = 0;
	EncInfo*		pEncInfo = &handle->CodecInfo->encInfo;
	EncOpenParam*	pOpenParam = &pEncInfo->openParam;
	
	bufWidth  = VPU_ALIGN8( pOpenParam->picWidth );
	bufHeight = VPU_ALIGN8( pOpenParam->picHeight );
	count     = encInitInfo->minFrameBufferCount;
	
	mvColSize = WAVE4_ENC_HEVC_MVCOL_BUF_SIZE( bufWidth, bufHeight );
	mvColSize = VPU_ALIGN16( mvColSize );
	mvColSize = ((mvColSize*count+4095)&~4095)+4096;   /* 4096 is a margin */
	mvColSize = VPU_ALIGN16384( mvColSize );
	if( ddrCh_save.vCoreCh == 0 ){
		ch0_total_size += mvColSize;
	}
	else{
		// ch1
		ch1_total_size += mvColSize;
	}
	
	fbcYTblSize = WAVE420SN_FBC_LUMA_TABLE_SIZE( bufWidth, bufHeight );
	fbcYTblSize = ((fbcYTblSize*count+4095)&~4095)+4096;
	fbcYTblSize = VPU_ALIGN16384( fbcYTblSize );
	if( ddrCh_save.fbcCh == 0 ){
		ch0_total_size += fbcYTblSize;
	}
	else{
		// ch1
		ch1_total_size += fbcYTblSize;
	}
	
	fbcCTblSize = WAVE420SN_FBC_CHROMA_TABLE_SIZE( bufWidth, bufHeight );
	fbcCTblSize = ((fbcCTblSize*count+4095)&~4095)+4096;
	fbcCTblSize = VPU_ALIGN16384( fbcCTblSize );
	if( ddrCh_save.fbcCh == 0 ){
		ch0_total_size += fbcCTblSize;
	}
	else{
		// ch1
		ch1_total_size += fbcCTblSize;
	}
	
	subSampledSize = WAVE4_SUBSAMPLED_ONE_SIZE( bufWidth, bufHeight );
	subSampledSize = ((subSampledSize*count+4095)&~4095)+4096;
	subSampledSize = VPU_ALIGN16384( subSampledSize );
	if( ddrCh_save.vCoreCh == 0 ){
		ch0_total_size += subSampledSize;
	}
	else{
		// ch1
		ch1_total_size += subSampledSize;
	}
/*
 The required code size is calculated as the sum of the sizes output in the debug log below.
 
 # The code area is required for each channel of the SDRAM, and the code size requires 64 Kbyte alignment.
 # In addition, a common memory area (7 Mbytes) is required for each SDRAM ch.
 
 ex.
 H265_Wave_VmemSize_Calculation(0) : size ch0(0x0) ch1(0x37c000)
 H265_Wave_VmemSize_Calculation(1) : size ch0(0x0) ch1(0xe8000)
 H265_Wave_VmemSize_Calculation(2) : size ch0(0x0) ch1(0x78000)
 
 ch0 code size : 0x700000
                 ~~~~~~~~
 ch1 code size : 0x700000 + 0x37c000 + 0xe8000 + 0x78000 = 0xBDC000 -> [64Kbyte alignment] -> 0xBE0000
                                                                                              ~~~~~~~~
*/
	H265_Info_Print(( "H265_Wave_VmemSize_Calculation(%d) : size ch0(0x%x) ch1(0x%x)\n", ch, ch0_total_size, ch1_total_size ));
	
	return;
}
#endif


void  vdi_user_lock( void ){
	OS_User_Wai_Sem(SID_WAVE1);
}

void  vdi_user_unlock( void ){
	OS_User_Sig_Sem(SID_WAVE1);
}

void  vdi_user_vmem_lock( void ){
	OS_User_Wai_Sem(SID_WAVE2);
}

void  vdi_user_vmem_unlock( void ){
	OS_User_Sig_Sem(SID_WAVE2);
}

void  vdi_user_irq_flag_clear( void ){
	OS_User_Clr_Flg( FID_WAVE, 0 );
}

void  vdi_user_irq_flag_set( int interrupt_reason ){
	OS_User_Set_Flg( FID_WAVE, interrupt_reason );
}

void  vdi_user_wait_flag( int *interrupt_reason, int timeout ){
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
	
	ercd = OS_User_Twai_Flg( FID_WAVE, 0xFFFFFFFF, D_OS_USER_TWF_ORW, &flgptn, timeout );
	if( ercd == D_OS_USER_E_TMOUT ){
		flgptn = -1;
	}
	*interrupt_reason = (int)flgptn;
}

void  vdi_user_clean_cache( unsigned int dst, unsigned int count ){
	H265_Movie_Cache_Clean( (ULONG)dst,  (ULONG)count );
}

void  vdi_user_logprintf( char *fmt, ... ){
#ifdef CO_DEBUG_CODE_SIZE_CHECK
	H265_Info_Print( (fmt) );
//	H265_PRINTF( fmt );
#endif
}

void *vdi_user_malloc( int size ){
	OS_USER_ER		ercd;
	OS_USER_VP		blk = (OS_USER_VP)0;
	
	ercd = OS_User_Pget_Mpl( MPKID_WAVE, (INT32)size, &blk );
	if( ercd != D_OS_USER_E_OK ){
		H265_Info_Print(( "osal_malloc err : ercd = (%x), size = (%x)\n", ercd, size ));
		return(0);
	}
//	H265_Info_Print(( "vdi_user_malloc : addr = (%x), size = (%x)\n", blk, size ));
	return (void*)blk;
}

void  vdi_user_free( void *p){
	INT32	err;
	
	err = OS_User_Rel_Mpl( MPKID_WAVE, (OS_USER_VP)p );
	if( err != D_OS_USER_E_OK ){
		err = err;
	}
//	H265_Info_Print(( "vdi_user_free : addr = (%x)\n", p ));
	return;	
}

// pbu.c
spp_enc_context spp_enc_init(UINT8 *buffer, int buffer_size, int enableEPB)
{
	gbu_t* gbu;
	
	gbu = (gbu_t*)osal_malloc(sizeof(gbu_t));
	if (!gbu)
		return NULL;
	gbu->param = (gbu_if_t *)osal_malloc(sizeof(gbu_if_t));
	if (!gbu->param) {
		osal_free(gbu);
		return NULL;
	}

    // connect interface structure
	gbu->param->bb_start = buffer;
	gbu->param->bb_end = buffer + buffer_size;
	gbu->param->rd_ptr = buffer;
	gbu->param->wr_ptr = buffer;
	
    // reset write pointer
    gbu->rd_ptr = gbu->param->wr_ptr;

    // reset nal memory index/cnt
    gbu->nal_buf_idx = 0;
    gbu->nal_buf_cnt = 0;

    gbu->nal_ptr = 0;
    gbu->nal_cnt = 0;
    gbu->est_nal_byte_cnt = 0;

    // reset trailing zero count
    gbu->zero_cnt = 0;

    // reset rbsp consumed count
    gbu->bptr = 32;

    gbu->rbsp_bit_cnt = 0;
    gbu->tcnt = 0;

    // reset wbuf
    gbu->wbuf[0] = gbu->wbuf[1] = 0x00;

	gbu->epbRequired = enableEPB;

	return (spp_enc_context)gbu;
    
}

void spp_enc_deinit(spp_enc_context context)
{
	gbu_t* gbu = (gbu_t *)context;

	if (gbu) {
		if (gbu->param) {
			osal_free(gbu->param);
		}
		osal_free(gbu);
	}
}

//void spp_enc_init_rbsp(spp_enc_context context)
//{
//    gbu_t* gbu = (gbu_t *)context;
//
//    gbu->tcnt = 0;
//
//    // reset trailing zero count
//    gbu->zero_cnt = 0;
//
//    // reset rbsp consumed count
//    gbu->bptr = 32;
//
//    // reset wbuf
//    gbu->wbuf[0] = gbu->wbuf[1] = 0x00;
//}


void spp_enc_put_nal_byte(spp_enc_context context, UINT32 var, int n)
{
    int i;
    gbu_t* gbu = (gbu_t *)context;

    for (i = n-1 ; i >= 0 ; i--)
    {
        gbu->nal_buf[gbu->nal_buf_idx][gbu->nal_ptr] = ((var >> (i<<3)) & 0xFF);
        gbu->nal_ptr++;
        gbu->nal_cnt++;
        gbu->est_nal_byte_cnt++;

        
		if (gbu->nal_ptr == NAL_BUF_SIZE)
        {
            enc_flush_nal_buf(context);

            gbu->nal_buf_idx = (gbu->nal_buf_idx+1)&1;
            gbu->nal_ptr     = 0;
        }
    }
}


void spp_enc_put_bits(spp_enc_context context, UINT32 var, int n)
{
    UINT32 mask;
    UINT32 data;
    gbu_t* gbu = (gbu_t *)context;

    if (n == 0)
        return;


    
    // update total rbsp count
    gbu->tcnt += n;

    if (n > gbu->bptr)
    {
        // write data
        mask = (1 << gbu->bptr) - 1;
        data = (var >> (n - gbu->bptr)) & mask;

        gbu->wbuf[0] |= data;
        gbu->rbsp_bit_cnt += gbu->bptr;

        // update input data
        n   -= gbu->bptr;

        // update gbu bptr
        gbu->bptr = 0;

        // write rbsp data to nbuf
        enc_flush_rbsp(context);
    }


    mask = ((uint64_t)1 << n) - 1;
    data = var & mask;

    // write data
    gbu->wbuf[0] |= data << (gbu->bptr - n);
    gbu->rbsp_bit_cnt += n;

    // update bptr
    gbu->bptr -= n;

    if (gbu->bptr == 0)
        enc_flush_rbsp(context);
}



void spp_enc_flush(spp_enc_context context)
{

    enc_flush_rbsp(context);
    
    enc_flush_nal_buf(context);
}



void enc_flush_rbsp(spp_enc_context context)
{
	gbu_t* gbu = (gbu_t *)context;
    
    int     cnt = (32 - gbu->bptr + 7) >> 3;
    int     pos = 4 - cnt;
    UINT8 data;

    gbu->wbuf[1] = gbu->wbuf[0];


    
    for (cnt = cnt-1 ; cnt >= 0 ; cnt--)
    {
        data = (gbu->wbuf[0] >> ((cnt+pos)<<3)) & 0xFF;
        
		if (gbu->epbRequired == 1)
		{
			// insert EPB if needed
			if ((gbu->zero_cnt == 2) && (data <= 0x03))
			{
				spp_enc_put_nal_byte(context, 0x03, 1);
				gbu->zero_cnt = 0;
			}
		}
        spp_enc_put_nal_byte(context, data, 1);

        // update number of trailing zeroes
        if (data == 0x00)
            gbu->zero_cnt++;
        else
            gbu->zero_cnt = 0;
    }

    // reset bit ptr
    gbu->bptr = 32;

    // reset wbuf
    gbu->wbuf[0] = 0;

}

static void enc_flush_nal_buf(spp_enc_context context)
{
    int i, cnt;
    int left, room;
    UINT8* ptr;
	UINT8* align_wr_ptr;
    gbu_t* gbu = (gbu_t *)context;
    gbu_if_t* io = gbu->param;
    ptr = (UINT8 *)io->wr_ptr;

    align_wr_ptr = (UINT8*)(ptr);

    for (cnt = 0 ; cnt < gbu->nal_ptr ; )
    {
        room = left = io->rd_ptr - io->wr_ptr;

        // wraparound case
        if (left <= 0)
        {
            left  = io->bb_end - io->wr_ptr;
            room += io->bb_end - io->bb_start;
        }

        // write stream to CPB
        for (i = 0 ; i < left && cnt < gbu->nal_ptr ; i++, cnt++)
            *(align_wr_ptr + i) = gbu->nal_buf[gbu->nal_buf_idx][cnt];

        gbu->param->wr_ptr = align_wr_ptr + i;

        // handle wraparound case
        if (io->wr_ptr == io->bb_end)
            io->wr_ptr = io->bb_start;

        // if room is small enough, then flush data to external memory
        if ((room - i) < 256)
        {
            //host_isr(gbu->param, INT_BIT_BIT_BUF_FULL, NULL);
            continue;
        }
    }


    
    // clear nal_ptr
    gbu->nal_ptr = 0;
}


// put unsigned exp-golomb code
void spp_enc_put_ue(spp_enc_context context, UINT32 var)
{
    INT32  num;
    INT32  data;
   
    
    num  = -1;
    data = var + 1;

    while (data) 
    {
        data = data >> 1;
        num++;
    }

    // leading zero bits
    put_bits(context, 0, num);              
    put_bits(context, var+1, num+1);
}


INT32 spp_enc_get_ue_bit_size(UINT32 var)
{
	INT32 size;
	INT32  num;
	INT32  data;

	num  = -1;
	data = var + 1;

	while (data) 
	{
		data = data >> 1;
		num++;
	}

	// leading zero bits
	size = num;
	size += (num+1);

	return size;
}


// put signed exp-golomb code
//void spp_enc_put_se(spp_enc_context context, INT32 var)
//{
//    INT32  data;
//  
//    
//	// (-1)^(code_num + 1)
//    if (var > 0)
//        data = (( var)<<1) - 1;                 
//    else
//        data = ((-var)<<1);
//
//    put_ue(context, data);
//}



// put stop one bit & padding zeroes
void spp_enc_put_byte_align(spp_enc_context context, int has_stop_bit)
{
    gbu_t* gbu = (gbu_t *)context;

    // stop one bit
    if (has_stop_bit)
        spp_enc_put_bits(context, 1, 1);                     

    // padding zeroes
    if (gbu->bptr & 7)
        spp_enc_put_bits(context, 0, gbu->bptr & 7);    
}


// number of remain bit in wbuf
//UINT32 spp_enc_get_wbuf_remain(spp_enc_context context)
//{
//    gbu_t* gbu = (gbu_t *)context;
//
//    return (UINT32)gbu->bptr;
//}


// number of total bit after gbu initialization 
UINT32 spp_enc_get_rbsp_bit(spp_enc_context context)
{
    gbu_t* gbu = (gbu_t *)context;

    return (UINT32)gbu->tcnt;
}


// number of nal byte after gbu initialization
UINT32 spp_enc_get_nal_cnt(spp_enc_context context)
{
    gbu_t* gbu = (gbu_t *)context;

    return (UINT32)gbu->nal_cnt;
}



// return wr_ptr of stream buffer
//UINT8* spp_enc_get_wr_ptr(spp_enc_context context)
//{
//    gbu_t* gbu = (gbu_t *)context;
//
//    enc_flush_rbsp(context);
//    enc_flush_nal_buf(context);
//
//    return gbu->param->wr_ptr;
//}


	
//UINT8* spp_enc_get_wr_ptr_only(spp_enc_context context)
//{
//    gbu_t* gbu = (gbu_t *)context;
//
//    enc_flush_rbsp(context);
//
//    return gbu->param->wr_ptr + gbu->nal_ptr;
//}



//void spp_enc_set_wr_ptr(spp_enc_context context, UINT32 wr_ptr)
//{
//    gbu_t* gbu = (gbu_t *)context;
//    gbu_if_t*  io  = gbu->param;
//	intptr_t wr_int_ptr = wr_ptr;
//	intptr_t bb_end_int_ptr = (intptr_t)io->bb_end;
//	intptr_t gbu_param_bb_end_int_ptr = (intptr_t)gbu->param->bb_end;
//	intptr_t gbu_param_bb_start_int_ptr = (intptr_t)gbu->param->bb_start;
//	
//    while(wr_ptr > bb_end_int_ptr)
//        wr_int_ptr = wr_int_ptr + gbu_param_bb_end_int_ptr - gbu_param_bb_start_int_ptr;
//
//    io->wr_ptr = (UINT8 *)wr_int_ptr;
//}



// get a estimated NAL count in byte
//UINT32 spp_enc_get_est_nal_cnt(spp_enc_context context)
//{
//    gbu_t* gbu = (gbu_t *)context;
//    UINT32 est_nal_byte = 0;
//    UINT8  data;
//    //INT32  cnt = (32 - gbu->bptr + 7) >> 3;
//    INT32  cnt = (32 - gbu->bptr ) >> 3;
//    INT32  pos = 4 - cnt;
//    INT32  zero_cnt = gbu->zero_cnt;
//
//    for (cnt = cnt-1 ; cnt >= 0 ; cnt--)
//    {
//        data = (gbu->wbuf[0] >> ((cnt+pos)<<3)) & 0xFF;
//
//        // insert EPB if needed
//        if ((zero_cnt == 2) && (data <= 0x03))
//        {
//            est_nal_byte++;
//            zero_cnt = 0;
//        }
//
//        est_nal_byte++;
//
//        // update number of trailing zeroes
//        if (data == 0x00)
//            zero_cnt++;
//        else
//            zero_cnt = 0;
//    }
//
//    return gbu->est_nal_byte_cnt + est_nal_byte;
//}




static void put_bits(spp_enc_context context, UINT32 var, int n)
{
    UINT32 mask;
    UINT32 data;
    gbu_t* gbu = (gbu_t *)context;

    if (n == 0)
        return;

    // update total rbsp count
    gbu->tcnt += n;

    if (n > gbu->bptr)
    {
        // write data
        mask = (1 << gbu->bptr) - 1;
        data = (var >> (n - gbu->bptr)) & mask;

        gbu->wbuf[0] |= data;
        gbu->rbsp_bit_cnt += gbu->bptr;

        // update input data
        n   -= gbu->bptr;

        // update gbu bptr
        gbu->bptr = 0;

        // write rbsp data to nbuf
        enc_flush_rbsp(context);
    }


    mask = (1 << n) - 1;
    data = var & mask;

    // write data
    gbu->wbuf[0] |= data << (gbu->bptr - n);
    gbu->rbsp_bit_cnt += n;

    // update bptr
    gbu->bptr -= n;

    if (gbu->bptr == 0)
        enc_flush_rbsp(context);
}




//static void put_ue(spp_enc_context context, UINT32 var)
//{
//    INT32  num;
//    INT32  data;
//
//    num  = -1;
//    data = var + 1;
//
//    while (data) 
//    {
//        data = data >> 1;
//        num++;
//    }
//
//    // leading zero bits
//    put_bits(context, 0, num);
//    put_bits(context, var+1, num+1);
//}

// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---