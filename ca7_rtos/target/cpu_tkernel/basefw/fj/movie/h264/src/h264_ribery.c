/**
 * @file		h264_ribery.c
 * @brief		H.264 RIBERY function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "h264_ribery.h"
#include "h264_user.h"

#ifdef CO_H264_SECTION_ON
//#pragma arm section code="H264_CODE"
//#pragma arm section rodata="H264_CONST"
//#pragma arm section rwdata="H264_INIT"
#endif


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */
#define localStack (*((volatile struct ls *)0x11000000))
//#define CO_RB_MAP_PRINT

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// FIFO control
/* Encode */
typedef struct{
	ULONG	index[D_H264_RIBERY_VRAW_EMPTY_FIFO_MAX_NUM];
	ULONG	idx_get;
	ULONG	idx_set;
	ULONG	full_flg;
	ULONG	emp_flg;
} T_H264_RIBERY_VRAW_EMPTY_FIFO;

/* Decode */
typedef struct{
	ULONG	field[D_H264_RIBERY_VCPB_EMPTY_FIFO_MAX_NUM];
	ULONG	idx_get;
	ULONG	idx_set;
	ULONG	full_flg;
	ULONG	emp_flg;
} T_H264_RIBERY_VCPB_EMPTY_FIFO;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static	volatile	T_H264_RIBERY_VRAW_EMPTY_FIFO	gVraw_Empty_Fifo[RB_MULTI_ENC_CH_NUM];
static	volatile	T_H264_RIBERY_VCPB_EMPTY_FIFO	gVcpb_Empty_Fifo;
static	volatile	UCHAR	gAHclk_Count;

static	volatile	ULONG	gMV0Work_adr = 0;
static	volatile	ULONG	gMV1Work_adr = 0;
static	volatile	ULONG	gNoirq_mode = 0;

//static	volatile	ULONG	gMax_Bank_Num;

volatile	USHORT	gErrorInfo[RB_ERRORINFO_COUNT_NUM];

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static VOID h264_ribery_wdt_intHandler( VOID );
static VOID h264_ribery_enc_intHandler( VOID );
static VOID h264_ribery_dec_intHandler( VOID );
static VOID h264_ribery_trc_intHandler( VOID );
static VOID h264_ribery_clean( ULONG addr, ULONG size );
static VOID h264_ribery_flush( ULONG addr, ULONG size );
static INT32 h264_ribery_init_bridge( ULONG codec, ULONG core_mode, T_H264_RIBERY_BRIDGE_COMMON* bridge_common, T_H264_RIBERY_BRIDGE_PORT* bridge_port );

static VOID h264_ribery_dec_bs_read_scaling_list( T_H264_MOVIE_STREAM_CONTROL* bit_stream, INT32* scalingList, INT32 sizeOfScalingList, INT32* useDefaultScalingMatrixFlag );

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

//----------------------
//	Handler Process 
//----------------------
/**
 *	@brief		H264(RIBERY Macro) Interrupt Handller for WDT(Watch dog timer).\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_ribery_wdt_intHandler( VOID )
{
	// Watch dog timer interrupt
	H264_Info_Print(( "Int ribery_hrwdt_int!!\n" ));
	
#ifdef CO_RB_ERROR_INFO_ON
	// Get Error Info
	RB_Common_ErrorInfo( (USHORT*)gErrorInfo );
#endif
	
	/*
	   this case is critical error -> H.264 system down
	*/
	H264_Movie_Ribery_AHClock_Stop( (UCHAR*)&gAHclk_Count );
	H264_Movie_Ribery_Clock_Stop( E_H264_TOP_RIBERY_CLOCK_STOP_ON );	// rbr clock Stop
	H264_Movie_Ribery_Reset( E_H264_TOP_RIBERY_RESET_ON );				// RIB_SRMP reset
	
	OS_User_Set_Flg( FID_H264, FLG_H264_WDT );
//	OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_WDT );
	return;
}


/**
 *	@brief		H264(RIBERY Macro) Interrupt Handller for Encode.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_ribery_enc_intHandler( VOID )
{
	USHORT	int_status;
	USHORT	err_status;
	
	// get IRQ
	RB_Int_Get_Status( &int_status );
	RB_Int_Get_ErrStatus( &err_status );
	
//	H264_Info_Print(( "int_st  = %#x\n", int_status ));
//	H264_Info_Print(( "err_st  = %#x\n", err_status ));
	
	if( int_status & RB_INT_STATUS_BOOT_END ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_BOOT_COMP );
	}
	if( int_status & RB_INT_STATUS_RESET_END ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_CODEC_STOP_COMP );
	}
	if( int_status & RB_INT_STATUS_START_END ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_CODEC_START_COMP );
	}
	if( int_status & RB_INT_STATUS_FIFO_VRAW_ENCREQ_SET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VRAW_ENCREQ_SET );
	}
	if( int_status & RB_INT_STATUS_FIFO_VSTRM_FREE_SET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VSTRM_FREE_SET );
	}
	if( int_status & RB_INT_STATUS_FIFO_VRAW_FREE_GET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VRAW_FREE_GET );
	}
	if( int_status & RB_INT_STATUS_FIFO_VSTRM_ENCDONE_GET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VSTRM_ENCDONE_GET );
	}
	if( int_status & RB_INT_STATUS_STOP_END )	{	;}
	if( int_status & RB_INT_STATUS_DONE )		{	
//													H264_Info_Print(( "Int!! VSTRM_ENCDONE\n" ));
													OS_User_Set_Flg( FID_H264, FLG_H264_ENC_VSTRM_COMP );
//													OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_ENC_VSTRM_COMP );
	}
	if( int_status & RB_INT_STATUS_FREE )		{	
//													H264_Info_Print(( "Int!! VRAW_FREE\n" ));
//													H264_Ribery_Enc_VRAW_Free();	// task or handler
	}
	
	if( int_status & RB_INT_STATUS_Y_VEC )		{	
//													H264_Info_Print(( "Int!! Y_VEC\n" ));
													OS_User_Set_Flg( FID_H264, FLG_H264_ENC_Y_VEC_COMP );
//													OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_Y_VEC_COMP );
	}
	
	// ERROR!!
	if( err_status & RB_INT_STATUS_ERROR ){
		H264_Err_Print(( "Error Interrupt\n"));
		H264_Err_Print(( "ENC_ERROR_INFO         = %#lx\n", *(ULONG*)( gMV0Work_adr + RB_MAP_ADR_ENC_ERROR_INFO        ) ));
		H264_Err_Print(( "ENC_ERROR_INFO_EX      = %#lx\n", *(ULONG*)( gMV0Work_adr + RB_MAP_ADR_ENC_ERROR_INFO_EX     ) ));
		H264_Err_Print(( "ENC_ERROR_INFO_GLOBAL  = %#lx\n", *(ULONG*)( gMV0Work_adr + RB_MAP_ADR_ENC_ERROR_INFO_GLOBAL ) ));
		
#ifdef CO_RB_ERROR_INFO_ON
		// Get Error Info
		RB_Common_ErrorInfo( (USHORT*)gErrorInfo );
#endif
		
		OS_User_Set_Flg( FID_H264, FLG_H264_FORCE_STOP );
//		OS_User_Set_Flg( FID_H264, FLG_RIBERY_FORCE_STOP );
	}
	
	// clear IRQ
	RB_Int_Clear_Status( int_status );
	RB_Int_Clear_ErrStatus( err_status );
	
	return;
}


/**
 *	@brief		H264(RIBERY Macro) Interrupt Handller for Decode.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_ribery_dec_intHandler( VOID )
{
	USHORT	int_status;
	USHORT	err_status;
	
	// get IRQ
	RB_Int_Get_Status( &int_status );
	RB_Int_Get_ErrStatus( &err_status );
	
//	H264_Info_Print(( "int_st  = %#x\n", int_status ));
//	H264_Info_Print(( "err_st  = %#x\n", err_status ));
	
	if( int_status & RB_INT_STATUS_BOOT_END ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_BOOT_COMP );
	}
	if( int_status & RB_INT_STATUS_RESET_END ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_CODEC_STOP_COMP );
	}
	if( int_status & RB_INT_STATUS_START_END ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_CODEC_START_COMP );
	}
	if( int_status & RB_INT_STATUS_FIFO_VCPB_DECREQ_SET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VCPB_DECREQ_SET );
	}
	if( int_status & RB_INT_STATUS_FIFO_VDPB_FREE_SET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VDPB_FREE_SET );
	}
	if( int_status & RB_INT_STATUS_FIFO_VCPB_FREE_GET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VCPB_FREE_GET );
	}
	if( int_status & RB_INT_STATUS_FIFO_VDPB_DECDONE_GET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VDPB_DECDONE_GET );
	}
	if( int_status & RB_INT_STATUS_STOP_END )	{
													OS_User_Set_Flg( FID_H264, FLG_H264_CODEC_FLUSH_COMP );
//													OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_CODEC_FLUSH_COMP );
	}
	if( int_status & RB_INT_STATUS_DONE )		{	
//													H264_Info_Print(( "Int!! VDPB_DECDONE\n" ));
													OS_User_Set_Flg( FID_H264, FLG_H264_DEC_VDPB_COMP );
//													OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_DEC_VDPB_COMP );
	}
	if( int_status & RB_INT_STATUS_FREE )		{	
//													H264_Info_Print(( "Int!! VCPB_FREE\n" ));
//													H264_Ribery_Dec_VCPB_Free();	// task or handler
	}
	// ERROR!!
	if( err_status & RB_INT_STATUS_ERROR ){
		H264_Info_Print(( "Error Interrupt\n"));
		H264_Err_Print(( "DEC_MVCPU_ERROR_INFO   = %#lx\n", *(ULONG*)( gMV0Work_adr + RB_MAP_ADR_DEC_MVCPU_ERROR_INFO   ) ));
		H264_Err_Print(( "DEC_SVCPU_ERROR_INFO   = %#lx\n", *(ULONG*)( gMV0Work_adr + RB_MAP_ADR_DEC_SVCPU_ERROR_INFO   ) ));
		H264_Err_Print(( "DEC_ERROR_INFO_EX_MV   = %#lx\n", *(ULONG*)( gMV0Work_adr + RB_MAP_ADR_DEC_ERROR_INFO_EX_MV   ) ));
		H264_Err_Print(( "DEC_ERROR_INFO_EX_SV   = %#lx\n", *(ULONG*)( gMV0Work_adr + RB_MAP_ADR_DEC_ERROR_INFO_EX_SV   ) ));
		OS_User_Set_Flg( FID_H264, FLG_H264_DEC_ERROR );
	}
	
	// clear IRQ
	RB_Int_Clear_Status( int_status );
	RB_Int_Clear_ErrStatus( err_status );
	
	return;
}

/**
 *	@brief		H264(RIBERY Macro) Interrupt Handller for Transcoding.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_ribery_trc_intHandler( VOID )
{
	USHORT	int_status;
	USHORT	err_status;
	USHORT	int_status2;
	USHORT	err_status2;
	
	// get IRQ
	RB_Int_Get_Status( &int_status );
	RB_Int_Get_ErrStatus( &err_status );
	RB_Int_Get_Status2( &int_status2 );
	RB_Int_Get_ErrStatus2( &err_status2 );
	
//	H264_Info_Print(( "int_st  = %#x\n", int_status  ));
//	H264_Info_Print(( "err_st  = %#x\n", err_status  ));
//	H264_Info_Print(( "int_st2 = %#x\n", int_status2 ));
//	H264_Info_Print(( "err_st2 = %#x\n", err_status2 ));
	
	if( int_status & RB_INT_STATUS_BOOT_END ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_BOOT_COMP );
	}
	if( int_status & RB_INT_STATUS_RESET_END ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_CODEC_STOP_COMP );
	}
	if( int_status & RB_INT_STATUS_START_END ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_CODEC_START_COMP );
	}
	if( int_status & RB_INT_STATUS_FIFO_VRAW_ENCREQ_SET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VRAW_ENCREQ_SET );
	}
	if( int_status & RB_INT_STATUS_FIFO_VSTRM_FREE_SET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VSTRM_FREE_SET );
	}
	if( int_status & RB_INT_STATUS_FIFO_VRAW_FREE_GET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VRAW_FREE_GET );
	}
	if( int_status & RB_INT_STATUS_FIFO_VSTRM_ENCDONE_GET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VSTRM_ENCDONE_GET );
	}
	if( int_status & RB_INT_STATUS_STOP_END )	{	;}
	if( int_status & RB_INT_STATUS_DONE )		{	
//													H264_Info_Print(( "Int!! VSTRM_ENCDONE\n" ));
													OS_User_Set_Flg( FID_H264, FLG_H264_ENC_VSTRM_COMP );
//													OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_ENC_VSTRM_COMP );
	}
	if( int_status & RB_INT_STATUS_FREE )		{	
//													H264_Info_Print(( "Int!! VRAW_FREE\n" ));
//													H264_Ribery_Enc_VRAW_Free();	// task or handler
	}
	
	if( int_status & RB_INT_STATUS_Y_VEC )		{	
//													H264_Info_Print(( "Int!! Y_VEC\n" ));
													OS_User_Set_Flg( FID_H264, FLG_H264_ENC_Y_VEC_COMP );
//													OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_Y_VEC_COMP );
	}
	
	// ERROR!!
	if( err_status & RB_INT_STATUS_ERROR ){
		H264_Err_Print(( "Error Interrupt(ENC)\n"));
		H264_Err_Print(( "ENC_ERROR_INFO         = %#lx\n", *(ULONG*)( gMV0Work_adr + RB_MAP_ADR_ENC_ERROR_INFO        ) ));
		H264_Err_Print(( "ENC_ERROR_INFO_EX      = %#lx\n", *(ULONG*)( gMV0Work_adr + RB_MAP_ADR_ENC_ERROR_INFO_EX     ) ));
		H264_Err_Print(( "ENC_ERROR_INFO_GLOBAL  = %#lx\n", *(ULONG*)( gMV0Work_adr + RB_MAP_ADR_ENC_ERROR_INFO_GLOBAL ) ));
		
#ifdef CO_RB_ERROR_INFO_ON
		// Get Error Info
		RB_Common_ErrorInfo( (USHORT*)gErrorInfo );
#endif
		
		OS_User_Set_Flg( FID_H264, FLG_H264_FORCE_STOP );
//		OS_User_Set_Flg( FID_H264, FLG_RIBERY_FORCE_STOP );
	}
	
	
	
	if( int_status2 & RB_INT_STATUS_BOOT_END ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_BOOT2_COMP );
	}
	if( int_status2 & RB_INT_STATUS_RESET_END ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_CODEC_STOP2_COMP );
	}
	if( int_status2 & RB_INT_STATUS_START_END ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_CODEC_START2_COMP );
	}
	if( int_status2 & RB_INT_STATUS_FIFO_VCPB_DECREQ_SET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VCPB_DECREQ_SET );
	}
	if( int_status2 & RB_INT_STATUS_FIFO_VDPB_FREE_SET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VDPB_FREE_SET );
	}
	if( int_status2 & RB_INT_STATUS_FIFO_VCPB_FREE_GET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VCPB_FREE_GET );
	}
	if( int_status2 & RB_INT_STATUS_FIFO_VDPB_DECDONE_GET ) {
		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VDPB_DECDONE_GET );
	}
	if( int_status2 & RB_INT_STATUS_STOP_END )	{
													OS_User_Set_Flg( FID_H264, FLG_H264_CODEC_FLUSH_COMP );
//													OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_CODEC_FLUSH_COMP );
	}
	if( int_status2 & RB_INT_STATUS_DONE )		{	
//													H264_Info_Print(( "Int!! VDPB_DECDONE\n" ));
													OS_User_Set_Flg( FID_H264, FLG_H264_DEC_VDPB_COMP );
//													OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_DEC_VDPB_COMP );
	}
	if( int_status2 & RB_INT_STATUS_FREE )		{	
//													H264_Info_Print(( "Int!! VCPB_FREE\n" ));
//													H264_Ribery_Dec_VCPB_Free();	// task or handler
	}
	// ERROR!!
	if( err_status2 & RB_INT_STATUS_ERROR ){
		H264_Info_Print(( "Error Interrupt(DEC)\n"));
		H264_Err_Print(( "DEC_MVCPU_ERROR_INFO   = %#lx\n", *(ULONG*)( gMV1Work_adr + RB_MAP_ADR_DEC_MVCPU_ERROR_INFO   ) ));
		H264_Err_Print(( "DEC_SVCPU_ERROR_INFO   = %#lx\n", *(ULONG*)( gMV1Work_adr + RB_MAP_ADR_DEC_SVCPU_ERROR_INFO   ) ));
		H264_Err_Print(( "DEC_ERROR_INFO_EX      = %#lx\n", *(ULONG*)( gMV1Work_adr + RB_MAP_ADR_DEC_ERROR_INFO_EX_MV   ) ));
		H264_Err_Print(( "DEC_ERROR_INFO_GLOBAL  = %#lx\n", *(ULONG*)( gMV1Work_adr + RB_MAP_ADR_DEC_ERROR_INFO_EX_SV   ) ));
		OS_User_Set_Flg( FID_H264, FLG_H264_STOP );
		OS_User_Set_Flg( FID_H264, FLG_H264_FORCE_STOP );
//		OS_User_Set_Flg( FID_RIBERY, FLG_RIBERY_FORCE_STOP );
	}
	
	
	// clear IRQ
	RB_Int_Clear_Status( int_status );
	RB_Int_Clear_ErrStatus( err_status );
	RB_Int_Clear_Status2( int_status2 );
	RB_Int_Clear_ErrStatus2( err_status2 );
	
	return;
}

//----------------------
//	Common Function
//----------------------
/**
 *	@brief		PERI_INT2_REQ REGISTER SETTING for MVCPU Process.\n
 *	@param		IN :request
 *	@param		OUT:None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Ribery_Int_Request( USHORT request )
{
	USHORT irq;
	
//	H264_Info_Print(( "H264_Ribery_Int_Request START (request = %#x)\n", request ));
	
	switch( request ){
		// Encode
		case E_H264_RIBERY_IRQ_REQUEST_ENC_START:
			irq = RB_INT_REQUEST_START;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_ENC_RESET:
			irq = RB_INT_REQUEST_RESET;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_ENC_VRAW_ENCREQ:
			irq = RB_INT_REQUEST_REQ;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_ENC_VSTRM_FREE:
			irq = RB_INT_REQUEST_FREE;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VRAW_ENCREQ_SET:
			irq = RB_INT_REQUEST_FIFO_VRAW_ENCREQ_SET;
#ifdef CO_DEBUG_ON_PC
			OS_User_Set_Flg( FID_H264, FLG_H264_ENC_VSTRM_COMP );
#endif
			break;
		case E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VSTRM_FREE_SET:
			irq = RB_INT_REQUEST_FIFO_VSTRM_FREE_SET;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VRAW_FREE_GET:
			irq = RB_INT_REQUEST_FIFO_VRAW_FREE_GET;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VSTRM_ENCDONE_GET:
			irq = RB_INT_REQUEST_FIFO_VSTRM_ENCDONE_GET;
			break;
		
		// Decode
		case E_H264_RIBERY_IRQ_REQUEST_DEC_START:
			irq = RB_INT_REQUEST_START;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_RESET:
			irq = RB_INT_REQUEST_RESET;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_STOP:
			irq = RB_INT_REQUEST_STOP;
#ifdef CO_DEBUG_ON_PC
			OS_User_Set_Flg( FID_H264, FLG_H264_CODEC_FLUSH_COMP );
#endif
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_VCPB_DECREQ:
			irq = RB_INT_REQUEST_REQ;
#ifdef CO_DEBUG_ON_PC
			OS_User_Set_Flg( FID_H264, FLG_H264_DEC_VDPB_COMP );
#endif
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_VDPB_FREE:
			irq = RB_INT_REQUEST_FREE;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VCPB_DECREQ_SET:
			irq = RB_INT_REQUEST_FIFO_VCPB_DECREQ_SET;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VDPB_FREE_SET:
			irq = RB_INT_REQUEST_FIFO_VDPB_FREE_SET;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VCPB_FREE_GET:
			irq = RB_INT_REQUEST_FIFO_VCPB_FREE_GET;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VDPB_DECDONE_GET:
			irq = RB_INT_REQUEST_FIFO_VDPB_DECDONE_GET;
			break;
		default:
			return;
	}
//	H264_Info_Print(( "RBIntRq Kind = %#x\n", irq ));
	
	RB_Int_Set_Request( irq );
}

/**
 *	@brief		PERI_INT2_REQ REGISTER SETTING for MVCPU Process2.\n
 *	@param		IN :request
 *	@param		OUT:None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Ribery_Int_Request2( USHORT request )
{
	USHORT irq;
	
//	H264_Info_Print(( "H264_Ribery_Int_Request2 START (request = %#x)\n", request ));
	
	switch( request ){
		// Encode
		case E_H264_RIBERY_IRQ_REQUEST_ENC_START:
			irq = RB_INT_REQUEST_START;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_ENC_RESET:
			irq = RB_INT_REQUEST_RESET;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_ENC_VRAW_ENCREQ:
			irq = RB_INT_REQUEST_REQ;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_ENC_VSTRM_FREE:
			irq = RB_INT_REQUEST_FREE;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VRAW_ENCREQ_SET:
			irq = RB_INT_REQUEST_FIFO_VRAW_ENCREQ_SET;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VSTRM_FREE_SET:
			irq = RB_INT_REQUEST_FIFO_VSTRM_FREE_SET;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VRAW_FREE_GET:
			irq = RB_INT_REQUEST_FIFO_VRAW_FREE_GET;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VSTRM_ENCDONE_GET:
			irq = RB_INT_REQUEST_FIFO_VSTRM_ENCDONE_GET;
			break;
		
		// Decode
		case E_H264_RIBERY_IRQ_REQUEST_DEC_START:
			irq = RB_INT_REQUEST_START;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_RESET:
			irq = RB_INT_REQUEST_RESET;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_STOP:
			irq = RB_INT_REQUEST_STOP;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_VCPB_DECREQ:
			irq = RB_INT_REQUEST_REQ;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_VDPB_FREE:
			irq = RB_INT_REQUEST_FREE;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VCPB_DECREQ_SET:
			irq = RB_INT_REQUEST_FIFO_VCPB_DECREQ_SET;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VDPB_FREE_SET:
			irq = RB_INT_REQUEST_FIFO_VDPB_FREE_SET;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VCPB_FREE_GET:
			irq = RB_INT_REQUEST_FIFO_VCPB_FREE_GET;
			break;
		case E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VDPB_DECDONE_GET:
			irq = RB_INT_REQUEST_FIFO_VDPB_DECDONE_GET;
			break;
		default:
			return;
	}
//	H264_Info_Print(( "RBIntRq2 Kind = %#x\n", irq ));
	
	RB_Int_Set_Request2( irq );
}

/**
 *	@brief		init ribery bridge
 *	@param		IN :codec
 *				   :*param
 *	@return		H264_OK	Normal End
 *	@return		H264_NG	Abnormal End
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 h264_ribery_init_bridge( ULONG codec, ULONG core_mode, T_H264_RIBERY_BRIDGE_COMMON* bridge_common, T_H264_RIBERY_BRIDGE_PORT* bridge_port )
{
	INT32	rb_codec=0;
	INT32	rb_core_mode;
	INT32	ret;
	T_RB_BRIDGE_MAP_COMMON	map_common[2];
	T_RB_BRIDGE_MAP_PORT	map_port;
	
	// work ch/adr save
	gMV0Work_adr = bridge_common[0].common_addr.work0_addr;
	gMV1Work_adr = bridge_common[1].common_addr.work0_addr;
	
	if( codec == H264_MODE_ENCODE )						{	rb_codec = RB_CODEC_ENC;		}
	else if( codec == H264_MODE_DECODE )				{	rb_codec = RB_CODEC_DEC;		}
	else												{	rb_codec = RB_CODEC_TRANSCODE;	}
	
#ifdef CO_RB_MAP_PRINT
	H264_Info_Print(( "RB_Bridge_Set_MemoryMap setting check!!\n" ));
	H264_Info_Print(( "rb_codec                 = %#x\n", rb_codec                ));
	H264_Info_Print(( "core_mode                = %#x\n", core_mode               ));
	H264_Info_Print(( "\n" ));
	H264_Info_Print(( "bridge_common[0]\n" ));
	H264_Info_Print(( "ch0_top_addr             = %#x\n", bridge_common[0].ch0_top_addr            ));
	H264_Info_Print(( "ch1_top_addr             = %#x\n", bridge_common[0].ch1_top_addr            ));
	H264_Info_Print(( "strm_bank                = %#x\n", bridge_common[0].strm_bank               ));
	H264_Info_Print(( "common_ch.work0_ch       = %#x\n", bridge_common[0].common_ch.work0_ch      ));
	H264_Info_Print(( "common_ch.work1_ch       = %#x\n", bridge_common[0].common_ch.work1_ch      ));
	H264_Info_Print(( "common_ch.work2_ch       = %#x\n", bridge_common[0].common_ch.work2_ch      ));
	H264_Info_Print(( "common_ch.stream_ch      = %#x\n", bridge_common[0].common_ch.stream_ch     ));
	H264_Info_Print(( "common_addr.work0_addr   = %#x\n", bridge_common[0].common_addr.work0_addr  ));
	H264_Info_Print(( "common_addr.work1_addr   = %#x\n", bridge_common[0].common_addr.work1_addr  ));
	H264_Info_Print(( "common_addr.work2_addr   = %#x\n", bridge_common[0].common_addr.work2_addr  ));
	H264_Info_Print(( "common_addr.stream_addr  = %#x\n", bridge_common[0].common_addr.stream_addr ));
	if( rb_codec == RB_CODEC_TRANSCODE ){
		H264_Info_Print(( "\n" ));
		H264_Info_Print(( "bridge_common[1]\n" ));
		H264_Info_Print(( "ch0_top_addr             = %#x\n", bridge_common[1].ch0_top_addr            ));
		H264_Info_Print(( "ch1_top_addr             = %#x\n", bridge_common[1].ch1_top_addr            ));
		H264_Info_Print(( "strm_bank                = %#x\n", bridge_common[1].strm_bank               ));
		H264_Info_Print(( "common_ch.work0_ch       = %#x\n", bridge_common[1].common_ch.work0_ch      ));
		H264_Info_Print(( "common_ch.work1_ch       = %#x\n", bridge_common[1].common_ch.work1_ch      ));
		H264_Info_Print(( "common_ch.work2_ch       = %#x\n", bridge_common[1].common_ch.work2_ch      ));
		H264_Info_Print(( "common_ch.stream_ch      = %#x\n", bridge_common[1].common_ch.stream_ch     ));
		H264_Info_Print(( "common_addr.work0_addr   = %#x\n", bridge_common[1].common_addr.work0_addr  ));
		H264_Info_Print(( "common_addr.work1_addr   = %#x\n", bridge_common[1].common_addr.work1_addr  ));
		H264_Info_Print(( "common_addr.work2_addr   = %#x\n", bridge_common[1].common_addr.work2_addr  ));
		H264_Info_Print(( "common_addr.stream_addr  = %#x\n", bridge_common[1].common_addr.stream_addr ));
	}
#endif
	
	map_common[0].ch0_top_addr					= bridge_common[0].ch0_top_addr;
	map_common[0].ch1_top_addr					= bridge_common[0].ch1_top_addr;
	map_common[0].strm_bank						= bridge_common[0].strm_bank;
	map_common[0].map_common_ch.work0_ch		= bridge_common[0].common_ch.work0_ch;
	map_common[0].map_common_ch.work1_ch		= bridge_common[0].common_ch.work1_ch;
	map_common[0].map_common_ch.work2_ch		= bridge_common[0].common_ch.work2_ch;
	map_common[0].map_common_ch.stream_ch		= bridge_common[0].common_ch.stream_ch;
	map_common[0].map_common_addr.work0_addr	= bridge_common[0].common_addr.work0_addr;
	map_common[0].map_common_addr.work1_addr	= bridge_common[0].common_addr.work1_addr;
	map_common[0].map_common_addr.work2_addr	= bridge_common[0].common_addr.work2_addr;
	map_common[0].map_common_addr.stream_addr	= bridge_common[0].common_addr.stream_addr;
	if( rb_codec == RB_CODEC_TRANSCODE ){
		map_common[1].ch0_top_addr					= bridge_common[1].ch0_top_addr;
		map_common[1].ch1_top_addr					= bridge_common[1].ch1_top_addr;
		map_common[1].strm_bank						= bridge_common[1].strm_bank;
		map_common[1].map_common_ch.work0_ch		= bridge_common[1].common_ch.work0_ch;
		map_common[1].map_common_ch.work1_ch		= bridge_common[1].common_ch.work1_ch;
		map_common[1].map_common_ch.work2_ch		= bridge_common[1].common_ch.work2_ch;
		map_common[1].map_common_ch.stream_ch		= bridge_common[1].common_ch.stream_ch;
		map_common[1].map_common_addr.work0_addr	= bridge_common[1].common_addr.work0_addr;
		map_common[1].map_common_addr.work1_addr	= bridge_common[1].common_addr.work1_addr;
		map_common[1].map_common_addr.work2_addr	= bridge_common[1].common_addr.work2_addr;
		map_common[1].map_common_addr.stream_addr	= bridge_common[1].common_addr.stream_addr;
	}
	
	// port set
	map_port.PME_Vector_Read					= bridge_port->PME_Vector_Read;
	map_port.PME_Vector_Write					= bridge_port->PME_Vector_Write;
	map_port.EDGE_Read							= bridge_port->EDGE_Read;
	map_port.EDGE_Write							= bridge_port->EDGE_Write;
	map_port.UBG_Read							= bridge_port->UBG_Read;
	map_port.UBG_Write							= bridge_port->UBG_Write;
	map_port.Activity_Read						= bridge_port->Activity_Read;
	map_port.Activity_Write						= bridge_port->Activity_Write;
	map_port.Stream0_Read_Acore					= bridge_port->Stream0_Read_Acore;
	map_port.Stream0_Write_Acore				= bridge_port->Stream0_Write_Acore;
	map_port.Stream1_Read_Acore					= bridge_port->Stream1_Read_Acore;
	map_port.Stream1_Write_Acore				= bridge_port->Stream1_Write_Acore;
	map_port.CME_Vector_Read_Acore				= bridge_port->CME_Vector_Read_Acore;
	map_port.CME_Vector_Write_Acore				= bridge_port->CME_Vector_Write_Acore;
	map_port.COEF_Read_Acore					= bridge_port->COEF_Read_Acore;
	map_port.COEF_Write_Acore					= bridge_port->COEF_Write_Acore;
	map_port.Stream0_Read_Bcore					= bridge_port->Stream0_Read_Bcore;
	map_port.Stream0_Write_Bcore				= bridge_port->Stream0_Write_Bcore;
	map_port.Stream1_Read_Bcore					= bridge_port->Stream1_Read_Bcore;
	map_port.Stream1_Write_Bcore				= bridge_port->Stream1_Write_Bcore;
	map_port.CME_Vector_Read_Bcore				= bridge_port->CME_Vector_Read_Bcore;
	map_port.CME_Vector_Write_Bcore				= bridge_port->CME_Vector_Write_Bcore;
	map_port.COEF_Read_Bcore					= bridge_port->COEF_Read_Bcore;
	map_port.COEF_Write_Bcore					= bridge_port->COEF_Write_Bcore;
	map_port.SEQ_Read							= bridge_port->SEQ_Read;
	map_port.SEQ_Write							= bridge_port->SEQ_Write;
	map_port.CBUS_Read							= bridge_port->CBUS_Read;
	map_port.CBUS_Write							= bridge_port->CBUS_Write;
	map_port.CC_1_1_Ref_Picture_Y_Read_Acore	= bridge_port->CC_1_1_Ref_Picture_Y_Read_Acore;
	map_port.CC_1_1_Ref_Picture_C_Read_Acore	= bridge_port->CC_1_1_Ref_Picture_C_Read_Acore;
	map_port.CC_1_1_Ref_Picture_Y_Write_Acore	= bridge_port->CC_1_1_Ref_Picture_Y_Write_Acore;
	map_port.CC_1_1_Ref_Picture_C_Write_Acore	= bridge_port->CC_1_1_Ref_Picture_C_Write_Acore;
	map_port.CC_1_1_Org_Picture_Y_Read_Acore	= bridge_port->CC_1_1_Org_Picture_Y_Read_Acore;
	map_port.CC_1_1_Org_Picture_C_Read_Acore	= bridge_port->CC_1_1_Org_Picture_C_Read_Acore;
	map_port.PP_1_1_Org_Picture_Y_Read_Acore	= bridge_port->PP_1_1_Org_Picture_Y_Read_Acore;
	map_port.PP_1_1_Org_Picture_C_Read_Acore	= bridge_port->PP_1_1_Org_Picture_C_Read_Acore;
	map_port.PIPP_Red_Picture_Y_Read_Acore		= bridge_port->PIPP_Red_Picture_Y_Read_Acore;
	map_port.PIPP_Red_Picture_C_Read_Acore		= bridge_port->PIPP_Red_Picture_C_Read_Acore;
	map_port.PIPP_Red_Picture_Y_Write_Acore		= bridge_port->PIPP_Red_Picture_Y_Write_Acore;
	map_port.PIPP_Red_Picture_C_Write_Acore		= bridge_port->PIPP_Red_Picture_C_Write_Acore;
	map_port.PME_1_16_Org_Picture_Y_Read_Acore	= bridge_port->PME_1_16_Org_Picture_Y_Read_Acore;
	map_port.PME_1_16_Org_Picture_C_Read_Acore	= bridge_port->PME_1_16_Org_Picture_C_Read_Acore;
	map_port.PME_1_16_Ref_Picture_Y_Read_Acore	= bridge_port->PME_1_16_Ref_Picture_Y_Read_Acore;
	map_port.PME_1_16_Ref_Picture_C_Read_Acore	= bridge_port->PME_1_16_Ref_Picture_C_Read_Acore;
	map_port.CC_1_1_Ref_Picture_Y_Read_Bcore	= bridge_port->CC_1_1_Ref_Picture_Y_Read_Bcore;
	map_port.CC_1_1_Ref_Picture_C_Read_Bcore	= bridge_port->CC_1_1_Ref_Picture_C_Read_Bcore;
	map_port.CC_1_1_Ref_Picture_Y_Write_Bcore	= bridge_port->CC_1_1_Ref_Picture_Y_Write_Bcore;
	map_port.CC_1_1_Ref_Picture_C_Write_Bcore	= bridge_port->CC_1_1_Ref_Picture_C_Write_Bcore;
	map_port.CC_1_1_Org_Picture_Y_Read_Bcore	= bridge_port->CC_1_1_Org_Picture_Y_Read_Bcore;
	map_port.CC_1_1_Org_Picture_C_Read_Bcore	= bridge_port->CC_1_1_Org_Picture_C_Read_Bcore;
	map_port.PP_1_1_Org_Picture_Y_Read_Bcore	= bridge_port->PP_1_1_Org_Picture_Y_Read_Bcore;
	map_port.PP_1_1_Org_Picture_C_Read_Bcore	= bridge_port->PP_1_1_Org_Picture_C_Read_Bcore;
	map_port.PIPP_Red_Picture_Y_Read_Bcore		= bridge_port->PIPP_Red_Picture_Y_Read_Bcore;
	map_port.PIPP_Red_Picture_C_Read_Bcore		= bridge_port->PIPP_Red_Picture_C_Read_Bcore;
	map_port.PIPP_Red_Picture_Y_Write_Bcore		= bridge_port->PIPP_Red_Picture_Y_Write_Bcore;
	map_port.PIPP_Red_Picture_C_Write_Bcore		= bridge_port->PIPP_Red_Picture_C_Write_Bcore;
	map_port.PME_1_16_Org_Picture_Y_Read_Bcore	= bridge_port->PME_1_16_Org_Picture_Y_Read_Bcore;
	map_port.PME_1_16_Org_Picture_C_Read_Bcore	= bridge_port->PME_1_16_Org_Picture_C_Read_Bcore;
	map_port.PME_1_16_Ref_Picture_Y_Read_Bcore	= bridge_port->PME_1_16_Ref_Picture_Y_Read_Bcore;
	map_port.PME_1_16_Ref_Picture_C_Read_Bcore	= bridge_port->PME_1_16_Ref_Picture_C_Read_Bcore;
	
	if(      core_mode == H264_CORE_MODE_1					)	{	rb_core_mode = RB_CORE_MODE_1;	}
	else														{	rb_core_mode = RB_CORE_MODE_2;	}
	
	// bridge start
	RB_Bridge_Release( rb_codec, rb_core_mode );
	
	// bridge port set
	RB_Bridge_Set_MemoryMap_Port( &map_port );
	
	// Memory Map Common
	if( rb_codec == RB_CODEC_ENC ){
		ret = RB_Bridge_Set_MemoryMap_EncCommon( &map_common[0] );
		if( ret != RB_OK ){
			H264_Err_Print(( "RB_Bridge_Set_MemoryMap_EncCommon error!!\n" ));
			return H264_NG;
		}
	}
	else if( rb_codec == RB_CODEC_DEC ){
		ret = RB_Bridge_Set_MemoryMap_DecCommon( &map_common[0] );
		if( ret != RB_OK ){
			H264_Err_Print(( "RB_Bridge_Set_MemoryMap_DecCommon error!!\n" ));
			return H264_NG;
		}
	}
	else{
		ret = RB_Bridge_Set_MemoryMap_EncCommon( &map_common[0] );
		if( ret != RB_OK ){
			H264_Err_Print(( "RB_Bridge_Set_MemoryMap_EncCommon error!!\n" ));
			return H264_NG;
		}
		
		ret = RB_Bridge_Set_MemoryMap_DecCommon2( &map_common[1] );
		if( ret != RB_OK ){
			H264_Err_Print(( "RB_Bridge_Set_MemoryMap_DecCommon2 error!!\n" ));
			return H264_NG;
		}
	}
	
	return H264_OK;
}

/**
 *	@brief		SDRAMC buffer clean
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_ribery_clean( ULONG addr, ULONG size )
{
	H264_Movie_Cache_Clean( addr, size );
	return;
}

/**
 *	@brief		SDRAMC buffer flush
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_ribery_flush( ULONG addr, ULONG size )
{
	H264_Movie_Cache_Flush( addr, size );
	return;
}

/**
 *	@brief		RIBERY init Process.
 *	@param		IN :codec
 *				   :bridge_param
 *				OUT:None
 *	@return		H264_OK	Normal End
 *	@return		H264_NG	Abnormal End
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Initialize( ULONG codec, ULONG core_mode, T_H264_RIBERY_BRIDGE_COMMON* bridge_common, T_H264_RIBERY_BRIDGE_PORT* bridge_port )
{
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	INT32			ret;
	
	H264_Info_Print(( "H264_Ribery_Initialize Start\n" ));
	
	// work ch/adr clear
	gMV0Work_adr = 0;
	gMV1Work_adr = 0;
	
	// Noirq mode clear
	gNoirq_mode = 0;
	
	// flag clear (Ribery)
	OS_User_Clr_Flg( FID_RIBERY, 0 );
	
	// INT Handler setting
	if( codec == H264_MODE_ENCODE )			{	RB_Int_Set_Callback( h264_ribery_wdt_intHandler, h264_ribery_enc_intHandler );	}
	else if( codec == H264_MODE_DECODE )	{	RB_Int_Set_Callback( h264_ribery_wdt_intHandler, h264_ribery_dec_intHandler );	}
	else									{	RB_Int_Set_Callback( h264_ribery_wdt_intHandler, h264_ribery_trc_intHandler );	}
	
	H264_Movie_Ribery_PMU_Setting( H264_ON );
	
	// clock start!!
	H264_Ribery_Clock_Start();
	
	// init Bridge
	ret = h264_ribery_init_bridge( codec, core_mode, bridge_common, bridge_port );
	if( ret != H264_OK ){
		H264_Ribery_Clock_Stop();
		return ret;
	}
	
	// INT Enable
	RB_Int_Set_Enable( 0xFFFF );		/* PERI_INT2_ENABLE  */
	RB_Int_Set_ErrEnable( 0xFFFF );		/* PERI_INT2_ENABLE2 */
	if( codec == H264_MODE_TRANSCODE ){
		RB_Int_Set_Enable2( 0xFFFF );
		RB_Int_Set_ErrEnable2( 0xFFFF );
	}
	
	// RIBERY Firm Unlock Start
	RB_Cpu_Firm_Unlock_Start();
	
	// RIBERY firmware download
	if( codec == H264_MODE_ENCODE )			{	RB_Cpu_Firm_Download( RB_CODEC_ENC );		}
	else if( codec == H264_MODE_DECODE )	{	RB_Cpu_Firm_Download( RB_CODEC_DEC );		}
	else									{	RB_Cpu_Firm_Download( RB_CODEC_TRANSCODE );	}
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_BOOT_COMP );
	
	// RIBERY CPU reset release
	RB_Cpu_Boot();
	
#ifndef CO_DEBUG_ON_PC
	// wait boot complete
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_BOOT_COMP, D_OS_USER_TWF_ORW, &flgptn, D_H264_RIBERY_BOOT_TIMEOUT );
	if( ercd == D_OS_USER_E_TMOUT ){
		H264_Err_Print(( "Ribery CPU Boot Time Out\n" ));
		H264_Ribery_Clock_Stop();
		return H264_NG;
	}
#endif
	if( codec == H264_MODE_TRANSCODE ){
		OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_BOOT2_COMP );
		
		// RIBERY CPU reset release
		RB_Cpu_Boot2();
		
#ifndef CO_DEBUG_ON_PC
		// wait boot complete
		ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_BOOT2_COMP, D_OS_USER_TWF_ORW, &flgptn, D_H264_RIBERY_BOOT_TIMEOUT );
		if( ercd == D_OS_USER_E_TMOUT ){
			H264_Err_Print(( "Ribery CPU Boot Time Out\n" ));
			H264_Ribery_Clock_Stop();
			return H264_NG;
		}
#endif
	}
	
	H264_Info_Print(( "H264_Ribery_Initialize End\n" ));
	return H264_OK;
}



/**
 *	@brief		RIBERY Fainaraiz Process.
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Finalize( ULONG codec )
{
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
	H264_Info_Print(( "H264_Ribery_Finalize Start\n" ));
	

#ifndef CO_DEBUG_ON_PC
	
	if( codec == H264_MODE_TRANSCODE ){
		OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_CODEC_STOP2_COMP );
		H264_Ribery_Int_Request2( E_H264_RIBERY_IRQ_REQUEST_ENC_RESET );
		ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_CODEC_STOP2_COMP, D_OS_USER_TWF_ORW, &flgptn, D_H264_RIBERY_STOP_TIMEOUT );
		
		/* CPU reset */
		RB_Cpu_Reset2();
	}
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_CODEC_STOP_COMP );
	H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_ENC_RESET );
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_CODEC_STOP_COMP, D_OS_USER_TWF_ORW, &flgptn, D_H264_RIBERY_STOP_TIMEOUT );
	
	/* CPU reset */
	RB_Cpu_Reset();
	
	
	/* Bridge reset */
	RB_Bridge_Reset();
	
	// clock stop!!
	H264_Ribery_Clock_Stop();
	
	H264_Movie_Ribery_PMU_Setting( H264_OFF );
	
	H264_Info_Print(( "H264_Ribery_Finalize End\n" ));
	
	if( ercd != D_OS_USER_E_OK ){
		return H264_NG;
	}
#endif
	
	return H264_OK;
}

/**
 *	@brief		Start MPCKST and execute Soft Reset(H264SR).\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Ribery_Clock_Start( VOID )
{
	H264_Movie_Ribery_Clock_Stop( E_H264_TOP_RIBERY_CLOCK_STOP_ON );			// clock Stop
	OS_User_Tslp_Tsk(1);
#ifdef CO_RB_ACT_CLOCK
	gAHclk_Count = 1;
	H264_Movie_Ribery_AHClock_Stop( (UCHAR*)&gAHclk_Count );
#else
	gAHclk_Count = 0;
	H264_Movie_Ribery_AHClock_Start( (UCHAR*)&gAHclk_Count );
#endif
	
	// SDRAMC clock enable
	H264_Movie_Sdramc_BusIf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_0, E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB0, E_H264_SDRAMC_RIBERY_CLOCK_EN_ENABLE );
	H264_Movie_Sdramc_BusIf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_0, E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB1, E_H264_SDRAMC_RIBERY_CLOCK_EN_ENABLE );
	H264_Movie_Sdramc_BusIf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_0, E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB2, E_H264_SDRAMC_RIBERY_CLOCK_EN_ENABLE );
	H264_Movie_Sdramc_BusIf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_1, E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB0, E_H264_SDRAMC_RIBERY_CLOCK_EN_ENABLE );
	H264_Movie_Sdramc_BusIf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_1, E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB1, E_H264_SDRAMC_RIBERY_CLOCK_EN_ENABLE );
	H264_Movie_Sdramc_BusIf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_1, E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB2, E_H264_SDRAMC_RIBERY_CLOCK_EN_ENABLE );
	
	H264_Movie_Sdramc_DataBuf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_0, E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB0, E_H264_SDRAMC_RIBERY_CLOCK_EN_ENABLE );
	H264_Movie_Sdramc_DataBuf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_0, E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB1, E_H264_SDRAMC_RIBERY_CLOCK_EN_ENABLE );
	H264_Movie_Sdramc_DataBuf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_0, E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB2, E_H264_SDRAMC_RIBERY_CLOCK_EN_ENABLE );
	H264_Movie_Sdramc_DataBuf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_1, E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB0, E_H264_SDRAMC_RIBERY_CLOCK_EN_ENABLE );
	H264_Movie_Sdramc_DataBuf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_1, E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB1, E_H264_SDRAMC_RIBERY_CLOCK_EN_ENABLE );
	H264_Movie_Sdramc_DataBuf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_1, E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB2, E_H264_SDRAMC_RIBERY_CLOCK_EN_ENABLE );
	
	H264_Movie_Ribery_Reset( E_H264_TOP_RIBERY_RESET_ON );						// RIB_SRMP reset
	H264_Movie_Ribery_Clock_Stop( E_H264_TOP_RIBERY_CLOCK_STOP_OFF );			// rbr clock Start
	OS_User_Tslp_Tsk(1);
	H264_Movie_Ribery_Reset( E_H264_TOP_RIBERY_RESET_OFF );						// RIB_SRMP Cancellation
	
	return;
}

/**
 *	@brief		Stop MPCKST and execute Soft Reset(H264SR).\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Ribery_Clock_Stop( VOID )
{
	H264_Movie_Ribery_AHClock_Stop( (UCHAR*)&gAHclk_Count );
	H264_Movie_Ribery_Clock_Stop( E_H264_TOP_RIBERY_CLOCK_STOP_ON );			// rbr clock Stop
	OS_User_Tslp_Tsk(1);
	H264_Movie_Ribery_Reset( E_H264_TOP_RIBERY_RESET_ON );						// RIB_SRMP reset
	
	// SDRAMC clock disable
	H264_Movie_Sdramc_BusIf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_0, E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB0, E_H264_SDRAMC_RIBERY_CLOCK_EN_DISABLE );
	H264_Movie_Sdramc_BusIf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_0, E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB1, E_H264_SDRAMC_RIBERY_CLOCK_EN_DISABLE );
	H264_Movie_Sdramc_BusIf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_0, E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB2, E_H264_SDRAMC_RIBERY_CLOCK_EN_DISABLE );
	H264_Movie_Sdramc_BusIf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_1, E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB0, E_H264_SDRAMC_RIBERY_CLOCK_EN_DISABLE );
	H264_Movie_Sdramc_BusIf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_1, E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB1, E_H264_SDRAMC_RIBERY_CLOCK_EN_DISABLE );
	H264_Movie_Sdramc_BusIf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_1, E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB2, E_H264_SDRAMC_RIBERY_CLOCK_EN_DISABLE );
	
	H264_Movie_Sdramc_DataBuf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_0, E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB0, E_H264_SDRAMC_RIBERY_CLOCK_EN_DISABLE );
	H264_Movie_Sdramc_DataBuf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_0, E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB1, E_H264_SDRAMC_RIBERY_CLOCK_EN_DISABLE );
	H264_Movie_Sdramc_DataBuf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_0, E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB2, E_H264_SDRAMC_RIBERY_CLOCK_EN_DISABLE );
	H264_Movie_Sdramc_DataBuf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_1, E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB0, E_H264_SDRAMC_RIBERY_CLOCK_EN_DISABLE );
	H264_Movie_Sdramc_DataBuf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_1, E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB1, E_H264_SDRAMC_RIBERY_CLOCK_EN_DISABLE );
	H264_Movie_Sdramc_DataBuf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH_1, E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB2, E_H264_SDRAMC_RIBERY_CLOCK_EN_DISABLE );
	
	return;
}

INT32 H264_Ribery_Boot( VOID )
{
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_BOOT_COMP );
	
	H264_Info_Print(( "Ribery CPU0 Boot Start\n" ));
	
	// RIBERY CPU reset release
	RB_Cpu_Boot();
	
#ifndef CO_DEBUG_ON_PC
	// wait boot complete
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_BOOT_COMP, D_OS_USER_TWF_ORW, &flgptn, D_H264_RIBERY_BOOT_TIMEOUT );
	if( ercd == D_OS_USER_E_TMOUT ){
		H264_Err_Print(( "Ribery CPU0 Boot Time Out\n" ));
		H264_Ribery_Clock_Stop();
		return H264_NG;
	}
#endif
	
	H264_Info_Print(( "RB Start0\n" ));	
	return H264_OK;
}

INT32 H264_Ribery_Boot2( VOID )
{
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_BOOT2_COMP );
	
	H264_Info_Print(( "Ribery CPU1 Boot Start\n" ));
	
	// RIBERY CPU reset release
	RB_Cpu_Boot2();
	
	// wait boot complete
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_BOOT2_COMP, D_OS_USER_TWF_ORW, &flgptn, D_H264_RIBERY_BOOT_TIMEOUT );
	if( ercd == D_OS_USER_E_TMOUT ){
		H264_Err_Print(( "Ribery CPU1 Boot Time Out\n" ));
		H264_Ribery_Clock_Stop();
		return H264_NG;
	}
	
	H264_Info_Print(( "RB Start1\n" ));
	return H264_OK;
}

//----------------------
//	Encode Function
//----------------------
/**
 *	@brief		init param setting. (movie define -> Ribery define convert)
 *	@param		IN :*param
 *				   :*vui
 *				OUT:None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Ribery_Enc_InitParam_Set( UINT32 ch, T_H264_RIBERY_ENC_PARAM* param, T_H264_RIBERY_BRIDGE_YC* bridge_yc )
{
	INT32				ret;
	INT32				rb_tb_flag = 0;		// Top/Bottom Separator Flag
	
	T_RB_BRIDGE_MAP_YC	yc;
	T_RB_INITPARAM_ENC	initparam;
	
	if( bridge_yc->tb_flag == H264_ON )	{	rb_tb_flag = RB_ON;		}
	else								{	rb_tb_flag = RB_OFF;	}
	
	// set Bridge YC
	yc.tb_flag						= rb_tb_flag;
	yc.map_yc_size.width			= bridge_yc->map_yc_size.width;
	yc.map_yc_size.y_bank			= bridge_yc->map_yc_size.y_bank;
	yc.map_yc_size.c_bank			= bridge_yc->map_yc_size.c_bank;
	yc.map_yc_size.localdec_width	= bridge_yc->map_yc_size.localdec_width;
	yc.map_yc_size.localdec_y_bank	= bridge_yc->map_yc_size.localdec_y_bank;
	yc.map_yc_size.localdec_c_bank	= bridge_yc->map_yc_size.localdec_c_bank;
	yc.map_yc_size.reduction_width	= bridge_yc->map_yc_size.reduction_width;
	yc.map_yc_size.reduction_y_bank	= bridge_yc->map_yc_size.reduction_y_bank;
	yc.map_yc_size.reduction_c_bank	= bridge_yc->map_yc_size.reduction_c_bank;
	yc.map_yc_ch.y_ch				= bridge_yc->map_yc_ch.y_ch;
	yc.map_yc_ch.c_ch				= bridge_yc->map_yc_ch.c_ch;
	yc.map_yc_ch.localdec_y_ch		= bridge_yc->map_yc_ch.localdec_y_ch;
	yc.map_yc_ch.localdec_c_ch		= bridge_yc->map_yc_ch.localdec_c_ch;
	yc.map_yc_ch.reduction_y_ch		= bridge_yc->map_yc_ch.reduction_y_ch;
	yc.map_yc_ch.reduction_c_ch		= bridge_yc->map_yc_ch.reduction_c_ch;
	yc.map_yc_addr.y_addr			= bridge_yc->map_yc_addr.y_addr;
	yc.map_yc_addr.c_addr			= bridge_yc->map_yc_addr.c_addr;
	yc.map_yc_addr.localdec_y_addr	= bridge_yc->map_yc_addr.localdec_y_addr;
	yc.map_yc_addr.localdec_c_addr	= bridge_yc->map_yc_addr.localdec_c_addr;
	yc.map_yc_addr.reduction_y_addr	= bridge_yc->map_yc_addr.reduction_y_addr;
	yc.map_yc_addr.reduction_c_addr	= bridge_yc->map_yc_addr.reduction_c_addr;
	
	ret = RB_Bridge_Set_MemoryMap_EncYC( ch, &yc );
	if( ret != RB_OK ){
		H264_Err_Print(( "RB_Bridge_Set_MemoryMap_EncYC error!!\n" ));
	}
	
	// size
	switch( param->size ){
		case E_H264_IF_IMAGE_SIZE_4096_2304:
			initparam.size = RB_SIZE_4K2K_16_9;
			break;
		case E_H264_IF_IMAGE_SIZE_4096_2160:
			initparam.size = RB_SIZE_4K2K_1_9_1;
			break;
		case E_H264_IF_IMAGE_SIZE_4096_2048:
			initparam.size = RB_SIZE_4K2K_2_1;
			break;
		case E_H264_IF_IMAGE_SIZE_3840_2160:
			initparam.size = RB_SIZE_4K2K_QFHD;
			break;
		case E_H264_IF_IMAGE_SIZE_3840_1920:
			initparam.size = RB_SIZE_3840x1920;
			break;
		case E_H264_IF_IMAGE_SIZE_3000_3000:
			initparam.size = RB_SIZE_3K_1_1;
			break;			
		case E_H264_IF_IMAGE_SIZE_2704_2028:
			initparam.size = RB_SIZE_2_7K_4_3;
			break;
		case E_H264_IF_IMAGE_SIZE_2704_1520:
			initparam.size = RB_SIZE_2_7K_16_9;
			break;
#if 0
		case E_H264_IF_IMAGE_SIZE_2_7K_1_9_1:
			initparam.size = RB_SIZE_2_7K_1_9_1;
			break;
#endif
		case E_H264_IF_IMAGE_SIZE_2560_1440:
			initparam.size = RB_SIZE_2560x1440;
			break;
		case E_H264_IF_IMAGE_SIZE_1920_1440:
			initparam.size = RB_SIZE_1440P;
			break;
		case E_H264_IF_IMAGE_SIZE_1920_1080:
			initparam.size = RB_SIZE_FHD;
			break;
		case E_H264_IF_IMAGE_SIZE_1920_960:
			initparam.size = RB_SIZE_1920x960;
			break;
		case E_H264_IF_IMAGE_SIZE_1440_1080:
			initparam.size = RB_SIZE_1440;
			break;
		case E_H264_IF_IMAGE_SIZE_1440_720:
			initparam.size = RB_SIZE_1440x720;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_1024:
			initparam.size = RB_SIZE_1280x1024;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_960:
			initparam.size = RB_SIZE_SXVGA;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_768:
			initparam.size = RB_SIZE_1280x768;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_720:
			initparam.size = RB_SIZE_HD;
			break;
		case E_H264_IF_IMAGE_SIZE_1024_768:
			initparam.size = RB_SIZE_1024x768;
			break;
		case E_H264_IF_IMAGE_SIZE_864_480:
			initparam.size = RB_SIZE_864x480;
			break;
		case E_H264_IF_IMAGE_SIZE_800_600:
			initparam.size = RB_SIZE_800x600;
			break;
		case E_H264_IF_IMAGE_SIZE_960_540:
			initparam.size = RB_SIZE_960x540;
			break;
		case E_H264_IF_IMAGE_SIZE_960_480:
			initparam.size = RB_SIZE_960x480;
			break;
		case E_H264_IF_IMAGE_SIZE_848_480:
			initparam.size = RB_SIZE_FWVGA;
			break;
		case E_H264_IF_IMAGE_SIZE_720_576:
			initparam.size = RB_SIZE_SD_PAL;
			break;
		case E_H264_IF_IMAGE_SIZE_720_480:
			initparam.size = RB_SIZE_SD_NTSC;
			break;
		case E_H264_IF_IMAGE_SIZE_640_480:
			initparam.size = RB_SIZE_VGA;
			break;
		case E_H264_IF_IMAGE_SIZE_640_360:
			initparam.size = RB_SIZE_HVGAW;
			break;
		case E_H264_IF_IMAGE_SIZE_480_270:
			initparam.size = RB_SIZE_480x270;
			break;
		case E_H264_IF_IMAGE_SIZE_432_240:
			initparam.size = RB_SIZE_FWQVGA;
			break;
		case E_H264_IF_IMAGE_SIZE_320_240:
			initparam.size = RB_SIZE_QVGA;
			break;
		case E_H264_IF_IMAGE_SIZE_1600_896:
			initparam.size = RB_SIZE_1600x896;
			break;
		case E_H264_IF_IMAGE_SIZE_1024_576:
			initparam.size = RB_SIZE_1024x576;
			break;
		case E_H264_IF_IMAGE_SIZE_800_448:
			initparam.size = RB_SIZE_800x448;
			break;
		case E_H264_IF_IMAGE_SIZE_352_288:
			initparam.size = RB_SIZE_352x288;
			break;
		case E_H264_IF_IMAGE_SIZE_2592_1944:
			initparam.size = RB_SIZE_2592x1944;
			break;
		case E_H264_IF_IMAGE_SIZE_2304_1296:
			initparam.size = RB_SIZE_2304x1296;
			break;
		case E_H264_IF_IMAGE_SIZE_1920_1280:
			initparam.size = RB_SIZE_1920_1280;
			break;
		case E_H264_IF_IMAGE_SIZE_640_512:
			initparam.size = RB_SIZE_640_512;
			break;
		case E_H264_IF_IMAGE_SIZE_384_288:
			initparam.size = RB_SIZE_384_288;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: size error!!\n" ));
			break;
	}
	
	// rate mode
	switch( param->rate_mode ){
		case E_H264_IF_RATE_MODE_CBR:
			initparam.ratemode = RB_RATE_MODE_CBR;
			break;
		case E_H264_IF_RATE_MODE_VBR:
			initparam.ratemode = RB_RATE_MODE_VBR;
			break;
		case E_H264_IF_RATE_MODE_FIXED_QP:
			initparam.ratemode = RB_RATE_MODE_CONSTANT_QP;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: rate mode error!!\n" ));
			break;
	}
	
	// enc mode
	switch( param->enc_mode ){
		case E_H264_IF_STREAM_FORMAT_ES:
			initparam.encmode = RB_ENC_MODE_MP4;
			break;
		case E_H264_IF_STREAM_FORMAT_PES:
			initparam.encmode = RB_ENC_MODE_TS;
			break;
		case E_H264_IF_STREAM_FORMAT_ES_HEADER:
			initparam.encmode = RB_ENC_MODE_TS;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: enc mode error!!\n" ));
			break;
	}
	
	// bit rate
	// ave bit rate -> don't parameter check (because, VBR mode setting "0bps".)
	
	// bps -> Kbps
	initparam.bitrate		= param->bitrate/1000;
	initparam.avebitrate	= param->avebitrate/1000;
	
	// frame rate
	switch( param->frame_rate ){
		case E_H264_IF_FRAME_RATE_239_76:
			initparam.framerate = RB_FR_239_76FPS;		//  239.76fps
			break;
		case E_H264_IF_FRAME_RATE_119_88:
			initparam.framerate = RB_FR_119_88FPS;		//  119.88fps
			break;
		case E_H264_IF_FRAME_RATE_59_94:
			initparam.framerate = RB_FR_59_94FPS;		// 59.94fps
			break;
		case E_H264_IF_FRAME_RATE_47_95:
			initparam.framerate = RB_FR_47_95FPS;		//   47.95fps
			break;
		case E_H264_IF_FRAME_RATE_29_97:
			initparam.framerate = RB_FR_29_97FPS;		// 29.97fps
			break;
		case E_H264_IF_FRAME_RATE_23_97:
			initparam.framerate = RB_FR_23_97FPS;		// 23.97fps
			break;
		case E_H264_IF_FRAME_RATE_14_98:
			initparam.framerate = RB_FR_14_98FPS;		// 14.98fps
			break;
		case E_H264_IF_FRAME_RATE_9_99:
			initparam.framerate = RB_FR_9_99FPS;		//  9.99fps
			break;
		case E_H264_IF_FRAME_RATE_4_99:
			initparam.framerate = RB_FR_4_99FPS;		//  4.99fps
			break;
		case E_H264_IF_FRAME_RATE_2_99:
			initparam.framerate = RB_FR_2_99FPS;		//  2.99fps
			break;
		case E_H264_IF_FRAME_RATE_1_99:
			initparam.framerate = RB_FR_1_99FPS;		//  1.99fps
			break;
		case E_H264_IF_FRAME_RATE_0_99:
			initparam.framerate = RB_FR_0_99FPS;		//  0.99fps
			break;
		
		case E_H264_IF_FRAME_RATE_240_0:
			initparam.framerate = RB_FR_240_0FPS;		//  240.00fps
			break;
		case E_H264_IF_FRAME_RATE_120_0:
			initparam.framerate = RB_FR_120_0FPS;		//  120.00fps
			break;
		case E_H264_IF_FRAME_RATE_60_0:
			initparam.framerate = RB_FR_60_0FPS;		// 60.00fps
			break;
		case E_H264_IF_FRAME_RATE_50_0:
			initparam.framerate = RB_FR_50_0FPS;		// 50.00fps
			break;
		case E_H264_IF_FRAME_RATE_48_0:
			initparam.framerate = RB_FR_48_0FPS;		//   48.00fps
			break;
		case E_H264_IF_FRAME_RATE_30_0:
			initparam.framerate = RB_FR_30_0FPS;		// 30.00fps
			break;
		case E_H264_IF_FRAME_RATE_25_0:
			initparam.framerate = RB_FR_25_0FPS;		// 25.00fps
			break;
		case E_H264_IF_FRAME_RATE_24_0:
			initparam.framerate = RB_FR_24_0FPS;		// 24.00fps
			break;
		case E_H264_IF_FRAME_RATE_15_0:
			initparam.framerate = RB_FR_15_0FPS;		// 15.00fps
			break;
		case E_H264_IF_FRAME_RATE_12_5:
			initparam.framerate = RB_FR_12_5FPS;		// 12.50fps
			break;
		case E_H264_IF_FRAME_RATE_10_0:
			initparam.framerate = RB_FR_10_0FPS;		// 10.00fps
			break;
		case E_H264_IF_FRAME_RATE_5_0:
			initparam.framerate = RB_FR_5_0FPS;			//  5.00fps
			break;
		case E_H264_IF_FRAME_RATE_3_0:
			initparam.framerate = RB_FR_3_0FPS;			//  3.00fps
			break;
		case E_H264_IF_FRAME_RATE_2_0:
			initparam.framerate = RB_FR_2_0FPS;			//  2.00fps
			break;
		case E_H264_IF_FRAME_RATE_1_0:
			initparam.framerate = RB_FR_1_0FPS;			//  1.00fps
			break;
		
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: framerate error!!\n" ));
			break;
	}
	
	switch( param->scan_mode ){
		case E_H264_IF_SCAN_MODE_PROGRESSIVE:
			initparam.scan_mode = RB_SCAN_MODE_PROGRESSIVE;		// Progressive
			break;
		case E_H264_IF_SCAN_MODE_INTERLACE:
			initparam.scan_mode = RB_SCAN_MODE_INTERLACE;		// Interlace
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: scan_mode error!!\n" ));
			break;
	}
	
	// gop_struct
	switch ( param->gop_struct ) {
//		case E_H264_IF_GOP_STR_IIII:
//			initparam.gop_struct = RB_GOP_STR_IIII;
//			break;
		case E_H264_IF_GOP_STR_IBP:
			initparam.gop_struct = RB_GOP_STR_IBPB;
			break;
		case E_H264_IF_GOP_STR_IPPP:
			initparam.gop_struct = RB_GOP_STR_IPPP;
			break;
		default:	// IBBP
			initparam.gop_struct = RB_GOP_STR_IBBP;
			break;
	}	
	
	// Closed GOP
	switch( param->cl_gop ){
		case H264_ON  :
			initparam.cl_gop = RB_CLOSED_GOP_ON;	// set ClosedGOP(0)
			break ;
		case H264_OFF :
			initparam.cl_gop = RB_CLOSED_GOP_OFF;	// set OpenGOP(1)
			break ;
		default :
			initparam.cl_gop = RB_CLOSED_GOP_ON;	// set ClosedGOP(0)
			break ;
	}
	
	// gop size(num)
	initparam.gop_size = BF_H264_User_Record_Get_Gop_Size(ch);
	
	// profile
	switch( param->profile ){
		case E_H264_IF_PROFILE_HIGH:
			initparam.profile = RB_PROFILE_HIGH;
			break;
		case E_H264_IF_PROFILE_MAIN:
			initparam.profile = RB_PROFILE_MAIN;
			break;
		case E_H264_IF_PROFILE_CONSTRAINED_BASELINE:
			initparam.profile = RB_PROFILE_CONSTRAINED_BASELINE;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: profile error!!\n" ));
			break;
	}
	
	// level
	initparam.level = param->level;
	
	// vlc mode
	switch( param->vlcmode ){
		case E_H264_IF_VLC_MODE_CABAC:
			initparam.vlcmode = RB_VLC_MODE_CABAC;
			break;
		case E_H264_IF_VLC_MODE_CAVLC:
			initparam.vlcmode = RB_VLC_MODE_CAVLC;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: VLC mode error!!\n" ));
			break;
	}
	
	// insert_pic_struct
	switch( param->insert_pic_struct ){
		case H264_ON:
			initparam.insert_pic_struct = RB_ON;
			break;
		case H264_OFF:
			initparam.insert_pic_struct = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: insert_pic_struct error!!\n" ));
			break ;
	}
	
	// insert_buf_ctrl_param
	switch( param->insert_buf_ctrl_param ){
		case H264_ON:
			initparam.insert_buf_ctrl_param = RB_ON;
			break;
		case H264_OFF:
			initparam.insert_buf_ctrl_param = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: insert_buf_ctrl_param error!!\n" ));
			break ;
	}
	
	// insert_recovery_point_sei
	switch( param->insert_recovery_point_sei ){
		case H264_ON:
			initparam.insert_recovery_point_sei = RB_ON;
			break;
		case H264_OFF:
			initparam.insert_recovery_point_sei = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: insert_recovery_point_sei error!!\n" ));
			break ;
	}
	
	// end_of_stream_en
	switch( param->end_of_stream_en ){
		case H264_ON:
			initparam.end_of_stream_en = RB_ON;
			break;
		case H264_OFF:
			initparam.end_of_stream_en = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: end_of_stream_en error!!\n" ));
			break ;
	}
	
	// end_of_sequence_en
	switch( param->end_of_sequence_en ){
		case H264_ON:
			initparam.end_of_sequence_en = RB_ON;
			break;
		case H264_OFF:
			initparam.end_of_sequence_en = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: end_of_sequence_en error!!\n" ));
			break ;
	}
	
	// cpb_over_en
	switch( param->cpb_over_en ){
		case H264_ON:
			initparam.cpb_over_en = RB_ON;
			break;
		case H264_OFF:
			initparam.cpb_over_en = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: cpb_over_en error!!\n" ));
			break ;
	}
	
	// ubg_en
	switch( param->ubg_en ){
		case H264_ON:
			initparam.ubg_en = RB_ON;
			break;
		case H264_OFF:
			initparam.ubg_en = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: ubg_en error!!\n" ));
			break ;
	}
	
	// filler_en
	switch( param->filler_en ){
		case H264_ON:
			initparam.filler_en = RB_ON;
			break;
		case H264_OFF:
			initparam.filler_en = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: filler_en error!!\n" ));
			break ;
	}
	
	// max_qp
	initparam.max_qp = param->max_qp;
	
	// min_qp
	initparam.min_qp = param->min_qp;
	
	// initial_cpb_removal_delay
	initparam.initial_cpb_removal_delay = param->initial_cpb_removal_delay;
	
	// vec_info_en
	switch( param->vec_info_en ){
		case H264_ON:
			initparam.vec_info_en = RB_ON;
			break;
		case H264_OFF:
			initparam.vec_info_en = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: vec_info_en error!!\n" ));
			break ;
	}
	
	initparam.frame_crop_top_offset    = param->frame_crop_top_offset;
	initparam.frame_crop_bottom_offset = param->frame_crop_bottom_offset;
	initparam.frame_crop_right_offset  = param->frame_crop_right_offset;
	initparam.frame_crop_left_offset   = param->frame_crop_left_offset;
	
#ifdef CO_RB_SEARCH_RANGE_ON
	// search_range
	switch( param->search_range_sel ){
		case H264_SEARCH_RANGE_SEL_MODE0:
			initparam.search_range_sel = RB_SEARCH_RANGE_SEL_MODE0;
			break;
		case H264_SEARCH_RANGE_SEL_MODE1:
			initparam.search_range_sel = RB_SEARCH_RANGE_SEL_MODE1;
			break;
		case H264_SEARCH_RANGE_SEL_MODE2:
			initparam.search_range_sel = RB_SEARCH_RANGE_SEL_MODE2;
			break;
		case H264_SEARCH_RANGE_SEL_MODE3:
			initparam.search_range_sel = RB_SEARCH_RANGE_SEL_MODE3;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: ENC SEARCH_RANGE_SEL_MODE error!!\n" ));
			break ;
	}
#endif
	
#ifdef CO_RB_SLICE_ON
	// slice
	switch( param->slice ){
		case H264_OFF:
			initparam.slice = RB_SLICE_ENC_NO_DIV;
			initparam.slice_notify = 0;
			break;
		case H264_ON:
			initparam.slice = RB_SLICE_ENC_4_DIV;
			initparam.slice_notify = 1;
			//UPRINTF( "\n RB_SLICE_ENC_4_DIV\n\n" );
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: ENC SLICE error!!\n" ));
			break ;
	}
#endif
	
#ifdef CO_IMAGE_QUALITY_MODE_ON
	switch( param->image_quality_mode ){
		case H264_IMAGE_QUALITY_MODE0:
			initparam.image_quality_mode = RB_IMAGE_QUALITY_MODE0;
			break;
		case H264_IMAGE_QUALITY_MODE1:
			initparam.image_quality_mode = RB_IMAGE_QUALITY_MODE1;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: ENC Image Quality mode error!!\n" ));
			break ;
	}

	initparam.qp_lambda_offset = param->qp_lambda_offset;
	
	switch( param->inter_intra_weight_ctrl ){
		case E_H264_IF_WEIGHT_CTRL_INTRA_3:
			initparam.inter_intra_weight_ctrl = RB_INTRA_WEIGHT_3;
			break;
		case E_H264_IF_WEIGHT_CTRL_INTRA_2:
			initparam.inter_intra_weight_ctrl = RB_INTRA_WEIGHT_2;
			break;
		case E_H264_IF_WEIGHT_CTRL_INTRA_1:
			initparam.inter_intra_weight_ctrl = RB_INTRA_WEIGHT_1;
			break;
		case E_H264_IF_WEIGHT_CTRL_DEFAULT:
			initparam.inter_intra_weight_ctrl = RB_DEFAULT;
			break;
		case E_H264_IF_WEIGHT_CTRL_INTER_1:
			initparam.inter_intra_weight_ctrl = RB_INTER_WEIGHT_1;
			break;
		case E_H264_IF_WEIGHT_CTRL_INTER_2:
			initparam.inter_intra_weight_ctrl = RB_INTER_WEIGHT_2;
			break;
		case E_H264_IF_WEIGHT_CTRL_INTER_3:
			initparam.inter_intra_weight_ctrl = RB_INTER_WEIGHT_3;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: ENC inter_intra_weight_ctrl error!!\n" ));
			break ;
	}
#endif
	
#ifdef CO_MB_LIGHT_MODE
	// mb_light_mode
	switch( param->mb_light_mode ){
		case H264_ON:
			initparam.mb_light_mode = RB_ON;
			break;
		case H264_OFF:
			initparam.mb_light_mode = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: mb_light_mode error!!\n" ));
			break ;
	}
#endif
	
#ifdef CO_FIXQP_EN_ON
	// fixqp_en
	switch( param->fixqp_en ){
		case H264_ON:
			initparam.fixqp_en = RB_ON;
			break;
		case H264_OFF:
			initparam.fixqp_en = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: fixqp_en error!!\n" ));
			break ;
	}
#endif
	
	// ENC REQ NO IRQ mode
	switch( param->noirq_mode ){
		case H264_ON:
			initparam.noirq_mode = RB_ON;
			break;
		case H264_OFF:
			initparam.noirq_mode = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Enc_InitParam_Set: ENC REQ NO IRQ mode error!!\n" ));
			break ;
	}
	// Set noirq mode
	gNoirq_mode = param->noirq_mode;
	
	// VRAW INFO interval
	initparam.vrawinfo_interval = param->vrawinfo_interval;
	
	// VSTRM INFO interval
	initparam.vstrminfo_interval = param->vstrminfo_interval;
	
#ifdef CO_RB_VUI_ON
	// vui
	initparam.vui                = (T_RB_INITPARAM_VUI *)param->vui;
#endif
	
#ifdef CO_RB_SCALINGLIST_ON
	// scalinglist
	initparam.scalinglist        = (T_RB_INITPARAM_SCALINGLIST *)param->scalinglist;
#endif
	
	// InitParam setting
	ret = RB_InitParam_Set_Encode( ch, &initparam );
	if( ret != RB_OK ){
		H264_Err_Print(( "RB_InitParam_Set_Encode error!!\n" ));
	}
	
	return;
}


/**
 *	@brief		Encode Start Request
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Enc_Start( UINT32 ch )
{
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_CODEC_START_COMP );
	
	// Set ch
	RB_InitParam_Set_Encode_Status( ch, RB_ON );
	
	// request & wait response
	H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_ENC_START );
#ifndef CO_DEBUG_ON_PC
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_CODEC_START_COMP, D_OS_USER_TWF_ORW, &flgptn, D_H264_RIBERY_START_TIMEOUT );
	if( ercd == D_OS_USER_E_OK ){
		if( RB_InitParam_Get_Encode_Status( ch ) == RB_ON ){
			H264_Warning_Print(( "Boot Complete(%d)!!\n", ch ));
		}
		else{
			H264_Warning_Print(( "Boot Complete NG(%d)!!\n", ch ));
			return H264_NG;
		}
	}
	else{
		H264_Warning_Print(( "Boot Complete NG(%d)!!\n", ch ));
		return H264_NG;
	}
#endif
	
	return H264_OK;
}


/**
 *	@brief		info_table/FIFO initialize Process.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Ribery_Enc_Vtable_Init( VOID )
{
	// info table init
	RB_InfoTable_Vraw_Init();
	
	// InitParam init
	RB_InitParam_Encode_Init();
	return;
}


/**
 *	@brief		VRAW_info_table data set Process.\n
 *	@param		IN :index
 *				   :vraw_info
 *	@param		OUT:None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Enc_VRAW_Set(T_RB_FIFO_INDEX index, T_RB_INFOTABLE_VRAW* vraw_info )
{
	INT32	ret;
	
	ret = RB_InfoTable_Vraw_Set( index, vraw_info );
	
	if( ret != RB_OK ){
		H264_Err_Print(( " RB_InfoTable_Vraw_Set NG!! ret = %d\n", ret ));
		return H264_NG;
	}
	
//	H264_Info_Print(( "VRAW. serial_num = [%d]\n", vraw_info->serial_num ));
	
	return H264_OK;
}


/**
 *	@brief		Encode Request Process.\n
 *	@param		IN :index
 *	@param		OUT:None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Enc_VRAW_Req( UINT32 count, T_RB_FIFO_INDEX* index )
{
	INT32			ret;
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
	// FIFO setting -> Encode request!!
	ret = RB_Fifo_Vraw_EncReq_Set( count, index );
	if( ret != RB_OK ){
		H264_Err_Print(( " RB_Fifo_Vraw_EncReq_Set NG!!\n" ));
		return H264_NG;
	}
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_FIFO_VRAW_ENCREQ_SET );
	
	// Fifo Push req & wait response
	H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VRAW_ENCREQ_SET );
#ifndef CO_DEBUG_ON_PC
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VRAW_ENCREQ_SET, D_OS_USER_TWF_ORW, (OS_USER_FLGPTN*)&flgptn, D_H264_RIBERY_FIFO_TIMEOUT );
	if( ercd != D_OS_USER_E_OK ){
		return H264_NG;
	}
	else{
//		H264_Info_Print(( "FIFO_VRAW_ENCREQ_SET Complete!!\n" ));
	}
#endif
	
	// check irq mode
	if( gNoirq_mode != H264_ON ){
		H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_ENC_VRAW_ENCREQ );
	}
	
	return H264_OK;
}


/**
 *	@brief		VRAW info table free Process.\n
 *	@param		IN :None
 *				OUT:None
 *	@return		H264_OK	Normal End
 *	@return		H264_NG	Abnormal End
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Enc_VRAW_Free( UINT32* free_count, T_RB_FIFO_INDEX* free_index, UINT32* free_bank )
{
	T_RB_FIFO_INDEX	index[RB_FIFO_MAX_COUNT_NUM];
	UINT32	count;
	UINT32	i;
	INT32	ret, ret2, ret3;
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
	T_RB_INFOTABLE_VRAW	vraw_info[RB_FIFO_MAX_COUNT_NUM];
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_FIFO_VRAW_FREE_GET );
	
#ifndef CO_DEBUG_ON_PC
	// Fifo pop req & wait response
	H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VRAW_FREE_GET );
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VRAW_FREE_GET, D_OS_USER_TWF_ORW, (OS_USER_FLGPTN*)&flgptn, D_H264_RIBERY_FIFO_TIMEOUT );
	if( ercd != D_OS_USER_E_OK ){
		return H264_NG;
	}
	else{
//		H264_Info_Print(( "FIFO_VRAW_FREE_GET Complete!!\n" ));
	}
#endif
	
	/* FIFO Get */
	ret = RB_Fifo_Vraw_Free_Get( &count, index );
	if(( ret == RB_OK ) || ( ret == RB_COMPLETE )){
		for( i=0;count > i;i++ ){
//			H264_Info_Print(( "VRAW free. index = [%d]\n", index[i].index ));
			// Get VRAW_info table
			ret2 = RB_InfoTable_Vraw_Get( index[i], &vraw_info[i] );
			if( ret2 != RB_OK ){
				H264_Err_Print(( "VRAW Get NG. index = [%d]\n", index[i].index ));
				return H264_ERR_SYSTEM;
			}
//			H264_Info_Print(( "VRAW physical_bank_idx = [%d]\n", vraw_info.physical_bank_idx ));
		}
		
		// VRAW Free
		ret2 = RB_InfoTable_Vraw_Free( count, index );
		if( ret2 != RB_OK ){
//			H264_Err_Print(( "VRAW free NG.\n" ));
			return H264_ERR_SYSTEM;
		}
		*free_count = count;
		for( i=0;count > i;i++ ){
			// Empty Setting
			ret3 = H264_Ribery_Enc_Vraw_Empty_Field_Set( index[i].ch, index[i].index );
			if( ret3 != H264_OK ){
				H264_Err_Print(( "Vraw_Empty_Field_Set NG\n" ));	// There is no liberating area of VRAW_info_table. 
				return H264_ERR_SYSTEM;
			}
			free_index[i].ch = index[i].ch;
			free_index[i].index = index[i].index;
			free_bank[i] = vraw_info[i].physical_bank_idx;	/* pgr0000 */
		}
		if( ret == RB_COMPLETE )	return H264_OK_COMPLETE;	// Everything was liberated. 
		else						return H264_OK;				// There is still remainder. 
	}
	return H264_NG;	// over read

}


/**
 *	@brief		VSTRM_info_table data get Process.\n
 *	@param		IN :None
 *				OUT:in_index
 *				   :vstrm_info
 *	@return		H264_OK			OK - Next Exist
 *				H264_OK_COMPLETE	OK - Read Completed
 *				H264_NG			Abnormal End
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Enc_VSTREAM_Get( UINT32* count, T_RB_FIFO_INDEX* index, T_RB_INFOTABLE_VSTRM* vstrm_info )
{
	INT32			ret, ret2;
#ifdef CO_RB_EMU_PRINT
	UINT32			i;
#endif
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_FIFO_VSTRM_ENCDONE_GET );
	
	// Fifo pop req & wait response
	H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VSTRM_ENCDONE_GET );
#ifndef CO_DEBUG_ON_PC
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VSTRM_ENCDONE_GET, D_OS_USER_TWF_ORW, (OS_USER_FLGPTN*)&flgptn, D_H264_RIBERY_FIFO_TIMEOUT );
	if( ercd != D_OS_USER_E_OK ){
		return H264_NG;
	}
	else{
//		H264_Info_Print(( "VSTRM_ENCDONE_GET Complete!!\n" ));
	}
#endif
	
	// index get from FIFO
	ret = RB_Fifo_Vstrm_EncDone_Get( count, index );
	if(( ret == RB_OK ) || ( ret == RB_COMPLETE )){
		// VSTRM_info get
		ret2 = RB_InfoTable_Vstrm_Get( *count, index, vstrm_info );
		if( ret2 == RB_OK ){
			if( ret == RB_COMPLETE )	return H264_OK_COMPLETE;	// It finished reading everything. 
			else						return H264_OK;				// There is still remainder. 
		}
	}
	
	return H264_NG;
}

/**
 *	@brief		VSTRM_free Process.\n
 *	@param		IN :index
 *	@param		OUT:None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Enc_VSTREAM_Free( UINT32 count, T_RB_FIFO_INDEX* index )
{
#ifndef CO_DEBUG_ON_PC
//	INT32			i;
	INT32			ret;
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;

//	for( i=0;count > i;i++ ){
//		H264_Info_Print(( "VSTRM free. index = [%d]\n", index[i].index));
//	}
	
	ret = RB_Fifo_Vstrm_Free_Set( count, index );
	
	if( ret != RB_OK ){
		H264_Err_Print(( "H264_Ribery_Enc_VSTREAM_Free : RB_Fifo_Vstrm_Free_Set NG\n" ));
		return H264_NG;
	}
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_FIFO_VSTRM_FREE_SET );
	
	// Fifo Push req & wait response
	H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VSTRM_FREE_SET );
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VSTRM_FREE_SET, D_OS_USER_TWF_ORW, (OS_USER_FLGPTN*)&flgptn, D_H264_RIBERY_FIFO_TIMEOUT );
	if( ercd != D_OS_USER_E_OK ){
		return H264_NG;
	}
	else{
//		H264_Info_Print(( "FIFO_VSTRM_FREE_SET Complete!!\n" ));
	}
	
	if( gNoirq_mode != H264_ON ){
		H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_ENC_VSTRM_FREE );
	}
#endif
	
	return H264_OK;
}

/**
 *	@brief		Y_VEC_info_table data get Process.\n
 *	@param		IN :None
 *				OUT:in_index
 *				   :vdpb_info
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Enc_Y_VEC_Get( UINT32* index, T_RB_INFOTABLE_Y_VEC* y_vec_info )
{
	INT32			ret, ret2;
	
	*index = 0xFF;
	
	// index get from FIFO
	ret = RB_Fifo_Y_VEC_Get( index );
	if(( ret == RB_OK ) || ( ret == RB_COMPLETE )){
		
		// Y_VEC_Info_Table get
		ret2 = RB_InfoTable_Y_VEC_Get( *index, y_vec_info );
		if( ret2 == RB_OK ){
#if 0
			H264_Info_Print(( "\n" ));
			H264_Info_Print(( "[Y_VEC_info_table_get]\n" ));
			H264_Info_Print(( " index = %d\n", *index));
			H264_Info_Print(( "  ch                 = %d\n",y_vec_info->ch ));
//			H264_Info_Print(( "  vraw_serial_num    = %d\n",y_vec_info->vraw_serial_num ));
			H264_Info_Print(( "  vraw_serial_num_h  = %#x\n", (unsigned long)((0xFFFFFFFF00000000 & y_vec_info->vraw_serial_num)>>32) ));
			H264_Info_Print(( "  vraw_serial_num_l  = %#x\n", (unsigned long)((0x00000000FFFFFFFF & y_vec_info->vraw_serial_num)    ) ));
			H264_Info_Print(( "  pic_type           = %d\n",y_vec_info->pic_type ));
			H264_Info_Print(( "  y_info_bank_idx    = %d\n",y_vec_info->y_info_bank_idx ));
			H264_Info_Print(( "  y_info_bank_addr   = %x\n",y_vec_info->y_info_bank_addr ));
			H264_Info_Print(( "  vec_info_bank_idx  = %d\n",y_vec_info->vec_info_bank_idx ));
			H264_Info_Print(( "  vec_info_bank_addr = %x\n",y_vec_info->vec_info_bank_addr ));
//			H264_Info_Print(( "  PTS                = %#x\n",y_vec_info->PTS ));
			H264_Info_Print(( "  PTS_h              = %#x\n", (unsigned long)((0xFFFFFFFF00000000 & y_vec_info->PTS)>>32) ));
			H264_Info_Print(( "  PTS_l              = %#x\n", (unsigned long)((0x00000000FFFFFFFF & y_vec_info->PTS)    ) ));
//			H264_Info_Print(( "  DTS                = %#x\n",y_vec_info->DTS ));
			H264_Info_Print(( "  DTS_h              = %#x\n", (unsigned long)((0xFFFFFFFF00000000 & y_vec_info->DTS)>>32) ));
			H264_Info_Print(( "  DTS_l              = %#x\n", (unsigned long)((0x00000000FFFFFFFF & y_vec_info->DTS)    ) ));
			H264_Info_Print(( "  bitrate            = %#x\n",y_vec_info->bitrate ));
			H264_Info_Print(( "  avebitrate         = %#x\n",y_vec_info->avebitrate ));
			H264_Info_Print(( "\n" ));
#else
//	H264_Info_Print(( "Y_VEC Done. vraw_serial_num = [%d]\n", y_vec_info->vraw_serial_num ));
//	H264_Info_Print(( "Y_VEC Done. vraw_serial_num_h = %#x\n", (unsigned long)((0xFFFFFFFF00000000 & y_vec_info->vraw_serial_num)>>32) ));
//	H264_Info_Print(( "Y_VEC Done. vraw_serial_num_l = %#x\n", (unsigned long)((0x00000000FFFFFFFF & y_vec_info->vraw_serial_num)    ) ));
#endif
			if( ret == RB_COMPLETE )	return H264_OK_COMPLETE;	// It finished reading everything. 
			else						return H264_OK;				// There is still remainder.
		}
	}
	
	return H264_NG;
}

/**
 *	@brief		VRAW_info_empty_idx_fifo initialize Process.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Ribery_Enc_Vraw_Empty_Init( UINT32 ch, UINT32 table_offset )
{
	UINT32 idx = 0;
	
//	H264_Info_Print(( "H264_Ribery_Enc_Vraw_Empty_Init Start\n" ));
	
	for( idx = 0 ; idx < RB_MAP_BNK_ENC_YCC ; idx++ ){
		gVraw_Empty_Fifo[ch].index[idx] = idx + table_offset;
	}
	gVraw_Empty_Fifo[ch].idx_get  = 0;			/* for FieldGet */
	gVraw_Empty_Fifo[ch].idx_set  = 0;			/* for FieldSet */
	gVraw_Empty_Fifo[ch].full_flg = H264_ON;	/* empty table is full */
	gVraw_Empty_Fifo[ch].emp_flg  = H264_OFF;	/* empty table is full */
	
	return;
}

/**
 *	@brief		VRAW_info_empty_idx_fifo Field Set Process.\n
 *	@param		IN :empty_index
 *				IN :empty_bank
 *				OUT:None
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Enc_Vraw_Empty_Field_Set( UINT32 ch, UINT32 empty_index )
{
	if( gVraw_Empty_Fifo[ch].full_flg == H264_OFF ){
#if 0
		H264_Info_Print(( "VS.i=[%d]\n", empty_index ));
#endif
		/* empty table is Nothing */
		gVraw_Empty_Fifo[ch].index[ gVraw_Empty_Fifo[ch].idx_set ] = empty_index;
		gVraw_Empty_Fifo[ch].idx_set++;
		
		if( gVraw_Empty_Fifo[ch].idx_set >= RB_MAP_BNK_ENC_YCC ){
			gVraw_Empty_Fifo[ch].idx_set = 0;
		}
		
		if( gVraw_Empty_Fifo[ch].idx_set == gVraw_Empty_Fifo[ch].idx_get ){
			/* empty table is full */
			gVraw_Empty_Fifo[ch].full_flg = H264_ON;
		}
		else {
			/* empty table is full */
			gVraw_Empty_Fifo[ch].emp_flg = H264_OFF;
		}
	}
	else{
		/* empty table is full */
		H264_Err_Print(( "Vraw_Empty is Full\n" ));
		
		return H264_NG;
	}
	
	return H264_OK;
}


/**
 *	@brief		VRAW_info_empty_idx_fifo Field Get Process.\n
 *	@param		IN :None
 *				OUT:empty_index
 *				OUT:empty_bank
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Enc_Vraw_Empty_Field_Get( UINT32 ch, UINT32* empty_index )
{
	if( gVraw_Empty_Fifo[ch].emp_flg == H264_OFF ){
		/* empty table is full */
		if( gVraw_Empty_Fifo[ch].idx_get == gVraw_Empty_Fifo[ch].idx_set ){
			/* empty table is Nothing */
			gVraw_Empty_Fifo[ch].full_flg = H264_OFF;
		}
		
		*empty_index = gVraw_Empty_Fifo[ch].index[ gVraw_Empty_Fifo[ch].idx_get ];
		gVraw_Empty_Fifo[ch].idx_get++;
#if 0
		H264_Info_Print(( "VG.i=[%d]\n", *empty_index ));
#endif		
		
		if( gVraw_Empty_Fifo[ch].idx_get >= RB_MAP_BNK_ENC_YCC ){
			gVraw_Empty_Fifo[ch].idx_get = 0;
		}
		
		if( gVraw_Empty_Fifo[ch].idx_get == gVraw_Empty_Fifo[ch].idx_set ){
			/* empty table is Nothing */
			gVraw_Empty_Fifo[ch].emp_flg = H264_ON;
		}
	}
	else{
		/* empty table is Nothing */
		H264_Err_Print(( "Vraw_Empty is Nothing\n" ));
		return H264_NG;
	}
	
	return H264_OK;
}


//----------------------
//	Decode Function
//----------------------
/**
 *	@brief		init param setting.
 *	@param		IN :*param
 *				OUT:None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Ribery_Dec_InitParam_Set( T_H264_RIBERY_DEC_PARAM* param, T_H264_RIBERY_BRIDGE_YC* bridge_yc )
{
	INT32				ret;
	INT32				rb_tb_flag = 0;		// Top/Bottom Separator Flag
	
	T_RB_BRIDGE_MAP_YC	yc;
	T_RB_INITPARAM_DEC	initparam;
	
	if( bridge_yc->tb_flag == H264_ON )	{	rb_tb_flag = RB_ON;		}
	else								{	rb_tb_flag = RB_OFF;	}
	
	// set Bridge YC
	yc.tb_flag						= rb_tb_flag;
	yc.map_yc_size.width			= bridge_yc->map_yc_size.width;
	yc.map_yc_size.y_bank			= bridge_yc->map_yc_size.y_bank;
	yc.map_yc_size.c_bank			= bridge_yc->map_yc_size.c_bank;
	yc.map_yc_size.localdec_width	= 0;
	yc.map_yc_size.localdec_y_bank	= 0;
	yc.map_yc_size.localdec_c_bank	= 0;
	yc.map_yc_size.reduction_width	= 0;
	yc.map_yc_size.reduction_y_bank	= 0;
	yc.map_yc_size.reduction_c_bank	= 0;
	yc.map_yc_ch.y_ch				= bridge_yc->map_yc_ch.y_ch;
	yc.map_yc_ch.c_ch				= bridge_yc->map_yc_ch.c_ch;
	yc.map_yc_ch.localdec_y_ch		= 0;
	yc.map_yc_ch.localdec_c_ch		= 0;
	yc.map_yc_ch.reduction_y_ch		= 0;
	yc.map_yc_ch.reduction_c_ch		= 0;
	yc.map_yc_addr.y_addr			= bridge_yc->map_yc_addr.y_addr;
	yc.map_yc_addr.c_addr			= bridge_yc->map_yc_addr.c_addr;
	yc.map_yc_addr.localdec_y_addr	= 0;
	yc.map_yc_addr.localdec_c_addr	= 0;
	yc.map_yc_addr.reduction_y_addr	= 0;
	yc.map_yc_addr.reduction_c_addr	= 0;
	
	ret = RB_Bridge_Set_MemoryMap_DecYC( &yc );
	if( ret != RB_OK ){
		H264_Err_Print(( "RB_Bridge_Set_MemoryMap_DecYC error!!\n" ));
	}
	
	// size
	switch( param->size ){
		case E_H264_IF_IMAGE_SIZE_4096_2304:
			initparam.size = RB_SIZE_4K2K_16_9;
			break;
		case E_H264_IF_IMAGE_SIZE_4096_2160:
			initparam.size = RB_SIZE_4K2K_1_9_1;
			break;
		case E_H264_IF_IMAGE_SIZE_4096_2048:
			initparam.size = RB_SIZE_4K2K_2_1;
			break;
		case E_H264_IF_IMAGE_SIZE_3840_2160:
			initparam.size = RB_SIZE_4K2K_QFHD;
			break;
        case E_H264_IF_IMAGE_SIZE_3840_1920:
			initparam.size = RB_SIZE_3840x1920;
			break;
		case E_H264_IF_IMAGE_SIZE_3000_3000:
			initparam.size = RB_SIZE_3K_1_1;
			break;			
		case E_H264_IF_IMAGE_SIZE_2704_2028:
			initparam.size = RB_SIZE_2_7K_4_3;
			break;
		case E_H264_IF_IMAGE_SIZE_2704_1520:
			initparam.size = RB_SIZE_2_7K_16_9;
			break;
#if 0
		case E_H264_IF_IMAGE_SIZE_2_7K_1_9_1:
			initparam.size = RB_SIZE_2_7K_1_9_1;
			break;
#endif
		case E_H264_IF_IMAGE_SIZE_2560_1440:
			initparam.size = RB_SIZE_2560x1440;
			break;
		case E_H264_IF_IMAGE_SIZE_1920_1440:
			initparam.size = RB_SIZE_1440P;
			break;
		case E_H264_IF_IMAGE_SIZE_1920_1080:
			initparam.size = RB_SIZE_FHD;
			break;
		case E_H264_IF_IMAGE_SIZE_1920_960:
			initparam.size = RB_SIZE_1920x960;
			break;
		case E_H264_IF_IMAGE_SIZE_1440_1080:
			initparam.size = RB_SIZE_1440;
			break;
		case E_H264_IF_IMAGE_SIZE_1440_720:
			initparam.size = RB_SIZE_1440x720;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_1024:
			initparam.size = RB_SIZE_1280x1024;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_960:
			initparam.size = RB_SIZE_SXVGA;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_768:
			initparam.size = RB_SIZE_1280x768;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_720:
			initparam.size = RB_SIZE_HD;
			break;
		case E_H264_IF_IMAGE_SIZE_1024_768:
			initparam.size = RB_SIZE_1024x768;
			break;
		case E_H264_IF_IMAGE_SIZE_864_480:
			initparam.size = RB_SIZE_864x480;
			break;
		case E_H264_IF_IMAGE_SIZE_800_600:
			initparam.size = RB_SIZE_800x600;
			break;
		case E_H264_IF_IMAGE_SIZE_960_540:
			initparam.size = RB_SIZE_960x540;
			break;
		case E_H264_IF_IMAGE_SIZE_848_480:
			initparam.size = RB_SIZE_FWVGA;
			break;
		case E_H264_IF_IMAGE_SIZE_720_576:
			initparam.size = RB_SIZE_SD_PAL;
			break;
		case E_H264_IF_IMAGE_SIZE_720_480:
			initparam.size = RB_SIZE_SD_NTSC;
			break;
		case E_H264_IF_IMAGE_SIZE_640_480:
			initparam.size = RB_SIZE_VGA;
			break;
		case E_H264_IF_IMAGE_SIZE_640_360:
			initparam.size = RB_SIZE_HVGAW;
			break;
		case E_H264_IF_IMAGE_SIZE_480_270:
			initparam.size = RB_SIZE_480x270;
			break;
		case E_H264_IF_IMAGE_SIZE_432_240:
			initparam.size = RB_SIZE_FWQVGA;
			break;
		case E_H264_IF_IMAGE_SIZE_320_240:
			initparam.size = RB_SIZE_QVGA;
			break;
		case E_H264_IF_IMAGE_SIZE_1600_896:
			initparam.size = RB_SIZE_1600x896;
			break;
		case E_H264_IF_IMAGE_SIZE_1024_576:
			initparam.size = RB_SIZE_1024x576;
			break;
		case E_H264_IF_IMAGE_SIZE_800_448:
			initparam.size = RB_SIZE_800x448;
			break;
		case E_H264_IF_IMAGE_SIZE_352_288:
			initparam.size = RB_SIZE_352x288;
			break;
		case E_H264_IF_IMAGE_SIZE_2592_1944:
			initparam.size = RB_SIZE_2592x1944;
			break;
		case E_H264_IF_IMAGE_SIZE_2304_1296:
			initparam.size = RB_SIZE_2304x1296;
			break;
		case E_H264_IF_IMAGE_SIZE_1920_1280:
			initparam.size = RB_SIZE_1920_1280;
			break;
		case E_H264_IF_IMAGE_SIZE_640_512:
			initparam.size = RB_SIZE_640_512;
			break;
		case E_H264_IF_IMAGE_SIZE_384_288:
			initparam.size = RB_SIZE_384_288;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Dec_InitParam_Set: size error!!\n" ));
			break;
	}
	
	// frame rate
	switch( param->frame_rate ){
		case E_H264_IF_FRAME_RATE_239_76:
			initparam.framerate = RB_FR_239_76FPS;		//  239.76fps
			break;
		case E_H264_IF_FRAME_RATE_119_88:
			initparam.framerate = RB_FR_119_88FPS;		//  119.88fps
			break;
#if 0
		case H264_FR_89_91FPS:
			initparam.framerate = RB_FR_89_91FPS;		//   89.91fps
			break;
		case H264_FR_79_92FPS:
			initparam.framerate = RB_FR_79_92FPS;		//   79.92fps
			break;
#endif
		case E_H264_IF_FRAME_RATE_59_94:
			initparam.framerate = RB_FR_59_94FPS;		// 59.94fps
			break;
		case E_H264_IF_FRAME_RATE_47_95:
			initparam.framerate = RB_FR_47_95FPS;		//   47.95fps
			break;
		case E_H264_IF_FRAME_RATE_29_97:
			initparam.framerate = RB_FR_29_97FPS;		// 29.97fps
			break;
		case E_H264_IF_FRAME_RATE_23_97:
			initparam.framerate = RB_FR_23_97FPS;		// 23.97fps
			break;
		case E_H264_IF_FRAME_RATE_14_98:
			initparam.framerate = RB_FR_14_98FPS;		// 14.98fps
			break;
		case E_H264_IF_FRAME_RATE_9_99:
			initparam.framerate = RB_FR_9_99FPS;		//  9.99fps
			break;
		case E_H264_IF_FRAME_RATE_4_99:
			initparam.framerate = RB_FR_4_99FPS;		//  4.99fps
			break;
		case E_H264_IF_FRAME_RATE_2_99:
			initparam.framerate = RB_FR_2_99FPS;		//  2.99fps
			break;
		case E_H264_IF_FRAME_RATE_1_99:
			initparam.framerate = RB_FR_1_99FPS;		//  1.99fps
			break;
		case E_H264_IF_FRAME_RATE_0_99:
			initparam.framerate = RB_FR_0_99FPS;		//  0.99fps
			break;
		
		case E_H264_IF_FRAME_RATE_240_0:
			initparam.framerate = RB_FR_240_0FPS;		//  240.00fps
			break;
		case E_H264_IF_FRAME_RATE_120_0:
			initparam.framerate = RB_FR_120_0FPS;		//  120.00fps
			break;
#if 0
		case H264_FR_90_0FPS:
			initparam.framerate = RB_FR_90_0FPS;		//   90.00fps
			break;
		case H264_FR_80_0FPS:
			initparam.framerate = RB_FR_80_0FPS;		//   80.00fps
			break;
#endif
		case E_H264_IF_FRAME_RATE_60_0:
			initparam.framerate = RB_FR_60_0FPS;		// 60.00fps
			break;
		case E_H264_IF_FRAME_RATE_50_0:
			initparam.framerate = RB_FR_50_0FPS;		// 50.00fps
			break;
		case E_H264_IF_FRAME_RATE_48_0:
			initparam.framerate = RB_FR_48_0FPS;		//   48.00fps
			break;
		case E_H264_IF_FRAME_RATE_30_0:
			initparam.framerate = RB_FR_30_0FPS;		// 30.00fps
			break;
		case E_H264_IF_FRAME_RATE_25_0:
			initparam.framerate = RB_FR_25_0FPS;		// 25.00fps
			break;
		case E_H264_IF_FRAME_RATE_24_0:
			initparam.framerate = RB_FR_24_0FPS;		// 24.00fps
			break;
		case E_H264_IF_FRAME_RATE_15_0:
			initparam.framerate = RB_FR_15_0FPS;		// 15.00fps
			break;
		case E_H264_IF_FRAME_RATE_12_5:
			initparam.framerate = RB_FR_12_5FPS;		// 12.50fps
			break;
		case E_H264_IF_FRAME_RATE_10_0:
			initparam.framerate = RB_FR_10_0FPS;		// 10.00fps
			break;
		case E_H264_IF_FRAME_RATE_5_0:
			initparam.framerate = RB_FR_5_0FPS;			//  5.00fps
			break;
		case E_H264_IF_FRAME_RATE_3_0:
			initparam.framerate = RB_FR_3_0FPS;			//  3.00fps
			break;
		case E_H264_IF_FRAME_RATE_2_0:
			initparam.framerate = RB_FR_2_0FPS;			//  2.00fps
			break;
		case E_H264_IF_FRAME_RATE_1_0:
			initparam.framerate = RB_FR_1_0FPS;			//  1.00fps
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Dec_InitParam_Set: frame_rate error!!\n" ));
			break;
	}
	
	switch( param->scan_mode ){
		case E_H264_IF_SCAN_MODE_PROGRESSIVE:
			initparam.scan_mode = RB_SCAN_MODE_PROGRESSIVE;		// Progressive
			break;
		case E_H264_IF_SCAN_MODE_INTERLACE:
			initparam.scan_mode = RB_SCAN_MODE_INTERLACE;		// Interlace
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Dec_InitParam_Set: scan_mode error!!\n" ));
			break;
	}
	
	// trick
	switch( param->trick ){
		case H264_ON:
			initparam.trick = RB_ON;
			break;
		case H264_OFF:
			initparam.trick = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Dec_InitParam_Set: trick error!!\n" ));
			break;
	}
	
	// trick: keep_idx
	initparam.keep_idx = param->keep_idx;
	
	// vdpb bank num
	initparam.bank_num = param->bank_num;
	
	// self_rec_play
	switch( param->self_rec_play ){
		case H264_ON:
			initparam.self_rec_play = RB_ON;
			break;
		case H264_OFF:
			initparam.self_rec_play = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Dec_InitParam_Set: self_rec_play error!!\n" ));
			break;
	}
	
#ifdef CO_RB_MAP_PRINT
	H264_Info_Print(( "H264_Ribery_Dec_InitParam_Set setting check!!\n" ));
	H264_Info_Print(( "size          = %#x\n", initparam.size      ));
	H264_Info_Print(( "frame_rate    = %#x\n", initparam.framerate ));
	H264_Info_Print(( "scan_mode     = %#x\n", initparam.scan_mode ));
	H264_Info_Print(( "trick         = %#x\n", initparam.trick     ));
	H264_Info_Print(( "keep_idx      = %#x\n", initparam.keep_idx  ));
	H264_Info_Print(( "bank_num      = %#x\n", initparam.bank_num  ));
	H264_Info_Print(( "self_rec_play = %#x\n", initparam.self_rec_play  ));
#endif
	
	// InitParam setting
	ret = RB_InitParam_Set_Decode( &initparam );
	
	if( ret != RB_OK ){
		H264_Err_Print(( "RB_InitParam_Set_Decode error!!\n" ));
	}
	
	return;
}

/**
 *	@brief		init param setting2.
 *	@param		IN :*param
 *				OUT:None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Ribery_Dec_InitParam_Set2( T_H264_RIBERY_DEC_PARAM* param, T_H264_RIBERY_BRIDGE_YC* bridge_yc )
{
	INT32				ret;
	INT32				rb_tb_flag = 0;		// Top/Bottom Separator Flag
	
	T_RB_BRIDGE_MAP_YC	yc;
	T_RB_INITPARAM_DEC	initparam;
	
	if( bridge_yc->tb_flag == H264_ON )	{	rb_tb_flag = RB_ON;		}
	else								{	rb_tb_flag = RB_OFF;	}
	
	// set Bridge YC
	yc.tb_flag						= rb_tb_flag;
	yc.map_yc_size.width			= bridge_yc->map_yc_size.width;
	yc.map_yc_size.y_bank			= bridge_yc->map_yc_size.y_bank;
	yc.map_yc_size.c_bank			= bridge_yc->map_yc_size.c_bank;
	yc.map_yc_size.localdec_width	= 0;
	yc.map_yc_size.localdec_y_bank	= 0;
	yc.map_yc_size.localdec_c_bank	= 0;
	yc.map_yc_size.reduction_width	= 0;
	yc.map_yc_size.reduction_y_bank	= 0;
	yc.map_yc_size.reduction_c_bank	= 0;
	yc.map_yc_ch.y_ch				= bridge_yc->map_yc_ch.y_ch;
	yc.map_yc_ch.c_ch				= bridge_yc->map_yc_ch.c_ch;
	yc.map_yc_ch.localdec_y_ch		= 0;
	yc.map_yc_ch.localdec_c_ch		= 0;
	yc.map_yc_ch.reduction_y_ch		= 0;
	yc.map_yc_ch.reduction_c_ch		= 0;
	yc.map_yc_addr.y_addr			= bridge_yc->map_yc_addr.y_addr;
	yc.map_yc_addr.c_addr			= bridge_yc->map_yc_addr.c_addr;
	yc.map_yc_addr.localdec_y_addr	= 0;
	yc.map_yc_addr.localdec_c_addr	= 0;
	yc.map_yc_addr.reduction_y_addr	= 0;
	yc.map_yc_addr.reduction_c_addr	= 0;
	
	ret = RB_Bridge_Set_MemoryMap_DecYC2( &yc );
	if( ret != RB_OK ){
		H264_Err_Print(( "RB_Bridge_Set_MemoryMap_DecYC2 error!!\n" ));
	}
	
	// size
	switch( param->size ){
		case E_H264_IF_IMAGE_SIZE_4096_2304:
			initparam.size = RB_SIZE_4K2K_16_9;
			break;
		case E_H264_IF_IMAGE_SIZE_4096_2160:
			initparam.size = RB_SIZE_4K2K_1_9_1;
			break;
		case E_H264_IF_IMAGE_SIZE_4096_2048:
			initparam.size = RB_SIZE_4K2K_2_1;
			break;
		case E_H264_IF_IMAGE_SIZE_3840_2160:
			initparam.size = RB_SIZE_4K2K_QFHD;
			break;
		case E_H264_IF_IMAGE_SIZE_3000_3000:
			initparam.size = RB_SIZE_3K_1_1;
			break;			
		case E_H264_IF_IMAGE_SIZE_2704_2028:
			initparam.size = RB_SIZE_2_7K_4_3;
			break;
		case E_H264_IF_IMAGE_SIZE_2704_1520:
			initparam.size = RB_SIZE_2_7K_16_9;
			break;
#if 0
		case E_H264_IF_IMAGE_SIZE_2_7K_1_9_1:
			initparam.size = RB_SIZE_2_7K_1_9_1;
			break;
#endif
		case E_H264_IF_IMAGE_SIZE_2560_1440:
			initparam.size = RB_SIZE_2560x1440;
			break;
		case E_H264_IF_IMAGE_SIZE_1920_1440:
			initparam.size = RB_SIZE_1440P;
			break;
		case E_H264_IF_IMAGE_SIZE_1920_1080:
			initparam.size = RB_SIZE_FHD;
			break;
		case E_H264_IF_IMAGE_SIZE_1440_1080:
			initparam.size = RB_SIZE_1440;
			break;
		case E_H264_IF_IMAGE_SIZE_1440_720:
			initparam.size = RB_SIZE_1440x720;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_1024:
			initparam.size = RB_SIZE_1280x1024;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_960:
			initparam.size = RB_SIZE_SXVGA;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_768:
			initparam.size = RB_SIZE_1280x768;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_720:
			initparam.size = RB_SIZE_HD;
			break;
		case E_H264_IF_IMAGE_SIZE_1024_768:
			initparam.size = RB_SIZE_1024x768;
			break;
		case E_H264_IF_IMAGE_SIZE_800_600:
			initparam.size = RB_SIZE_800x600;
			break;
		case E_H264_IF_IMAGE_SIZE_960_540:
			initparam.size = RB_SIZE_960x540;
			break;
		case E_H264_IF_IMAGE_SIZE_864_480:
			initparam.size = RB_SIZE_864x480;
			break;
		case E_H264_IF_IMAGE_SIZE_848_480:
			initparam.size = RB_SIZE_FWVGA;
			break;
		case E_H264_IF_IMAGE_SIZE_720_576:
			initparam.size = RB_SIZE_SD_PAL;
			break;
		case E_H264_IF_IMAGE_SIZE_720_480:
			initparam.size = RB_SIZE_SD_NTSC;
			break;
		case E_H264_IF_IMAGE_SIZE_640_480:
			initparam.size = RB_SIZE_VGA;
			break;
		case E_H264_IF_IMAGE_SIZE_640_360:
			initparam.size = RB_SIZE_HVGAW;
			break;
		case E_H264_IF_IMAGE_SIZE_480_270:
			initparam.size = RB_SIZE_480x270;
			break;
		case E_H264_IF_IMAGE_SIZE_432_240:
			initparam.size = RB_SIZE_FWQVGA;
			break;
		case E_H264_IF_IMAGE_SIZE_320_240:
			initparam.size = RB_SIZE_QVGA;
			break;
		case E_H264_IF_IMAGE_SIZE_1600_896:
			initparam.size = RB_SIZE_1600x896;
			break;
		case E_H264_IF_IMAGE_SIZE_1024_576:
			initparam.size = RB_SIZE_1024x576;
			break;
		case E_H264_IF_IMAGE_SIZE_800_448:
			initparam.size = RB_SIZE_800x448;
			break;
		case E_H264_IF_IMAGE_SIZE_352_288:
			initparam.size = RB_SIZE_352x288;
			break;
		case E_H264_IF_IMAGE_SIZE_2592_1944:
			initparam.size = RB_SIZE_2592x1944;
			break;
		case E_H264_IF_IMAGE_SIZE_2304_1296:
			initparam.size = RB_SIZE_2304x1296;
			break;
		case E_H264_IF_IMAGE_SIZE_1920_1280:
			initparam.size = RB_SIZE_1920_1280;
			break;
		case E_H264_IF_IMAGE_SIZE_640_512:
			initparam.size = RB_SIZE_640_512;
			break;
		case E_H264_IF_IMAGE_SIZE_384_288:
			initparam.size = RB_SIZE_384_288;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Dec_InitParam_Set2: size error!!\n" ));
			break;
	}
	
	// frame rate
	switch( param->frame_rate ){
		case E_H264_IF_FRAME_RATE_239_76:
			initparam.framerate = RB_FR_239_76FPS;		//  239.76fps
			break;
		case E_H264_IF_FRAME_RATE_119_88:
			initparam.framerate = RB_FR_119_88FPS;		//  119.88fps
			break;
#if 0
		case H264_FR_89_91FPS:
			initparam.framerate = RB_FR_89_91FPS;		//   89.91fps
			break;
		case H264_FR_79_92FPS:
			initparam.framerate = RB_FR_79_92FPS;		//   79.92fps
			break;
#endif
		case E_H264_IF_FRAME_RATE_59_94:
			initparam.framerate = RB_FR_59_94FPS;		// 59.94fps
			break;
		case E_H264_IF_FRAME_RATE_47_95:
			initparam.framerate = RB_FR_47_95FPS;		//   47.95fps
			break;
		case E_H264_IF_FRAME_RATE_29_97:
			initparam.framerate = RB_FR_29_97FPS;		// 29.97fps
			break;
		case E_H264_IF_FRAME_RATE_23_97:
			initparam.framerate = RB_FR_23_97FPS;		// 23.97fps
			break;
		case E_H264_IF_FRAME_RATE_14_98:
			initparam.framerate = RB_FR_14_98FPS;		// 14.98fps
			break;
		case E_H264_IF_FRAME_RATE_9_99:
			initparam.framerate = RB_FR_9_99FPS;		//  9.99fps
			break;
		case E_H264_IF_FRAME_RATE_4_99:
			initparam.framerate = RB_FR_4_99FPS;		//  4.99fps
			break;
		case E_H264_IF_FRAME_RATE_2_99:
			initparam.framerate = RB_FR_2_99FPS;		//  2.99fps
			break;
		case E_H264_IF_FRAME_RATE_1_99:
			initparam.framerate = RB_FR_1_99FPS;		//  1.99fps
			break;
		case E_H264_IF_FRAME_RATE_0_99:
			initparam.framerate = RB_FR_0_99FPS;		//  0.99fps
			break;
		
		case E_H264_IF_FRAME_RATE_240_0:
			initparam.framerate = RB_FR_240_0FPS;		//  240.00fps
			break;
		case E_H264_IF_FRAME_RATE_120_0:
			initparam.framerate = RB_FR_120_0FPS;		//  120.00fps
			break;
#if 0
		case H264_FR_90_0FPS:
			initparam.framerate = RB_FR_90_0FPS;		//   90.00fps
			break;
		case H264_FR_80_0FPS:
			initparam.framerate = RB_FR_80_0FPS;		//   80.00fps
			break;
#endif
		case E_H264_IF_FRAME_RATE_60_0:
			initparam.framerate = RB_FR_60_0FPS;		// 60.00fps
			break;
		case E_H264_IF_FRAME_RATE_50_0:
			initparam.framerate = RB_FR_50_0FPS;		// 50.00fps
			break;
		case E_H264_IF_FRAME_RATE_48_0:
			initparam.framerate = RB_FR_48_0FPS;		//   48.00fps
			break;
		case E_H264_IF_FRAME_RATE_30_0:
			initparam.framerate = RB_FR_30_0FPS;		// 30.00fps
			break;
		case E_H264_IF_FRAME_RATE_25_0:
			initparam.framerate = RB_FR_25_0FPS;		// 25.00fps
			break;
		case E_H264_IF_FRAME_RATE_24_0:
			initparam.framerate = RB_FR_24_0FPS;		// 24.00fps
			break;
		case E_H264_IF_FRAME_RATE_15_0:
			initparam.framerate = RB_FR_15_0FPS;		// 15.00fps
			break;
		case E_H264_IF_FRAME_RATE_12_5:
			initparam.framerate = RB_FR_12_5FPS;		// 12.50fps
			break;
		case E_H264_IF_FRAME_RATE_10_0:
			initparam.framerate = RB_FR_10_0FPS;		// 10.00fps
			break;
		case E_H264_IF_FRAME_RATE_5_0:
			initparam.framerate = RB_FR_5_0FPS;			//  5.00fps
			break;
		case E_H264_IF_FRAME_RATE_3_0:
			initparam.framerate = RB_FR_3_0FPS;			//  3.00fps
			break;
		case E_H264_IF_FRAME_RATE_2_0:
			initparam.framerate = RB_FR_2_0FPS;			//  2.00fps
			break;
		case E_H264_IF_FRAME_RATE_1_0:
			initparam.framerate = RB_FR_1_0FPS;			//  1.00fps
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Dec_InitParam_Set2: frame_rate error!!\n" ));
			break;
	}
		
	switch( param->scan_mode ){
		case E_H264_IF_SCAN_MODE_PROGRESSIVE:
			initparam.scan_mode = RB_SCAN_MODE_PROGRESSIVE;		// Progressive
			break;
		case E_H264_IF_SCAN_MODE_INTERLACE:
			initparam.scan_mode = RB_SCAN_MODE_INTERLACE;		// Interlace
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Dec_InitParam_Set2: scan_mode error!!\n" ));
			break;
	}
	
	// trick
	switch( param->trick ){
		case H264_ON:
			initparam.trick = RB_ON;
			break;
		case H264_OFF:
			initparam.trick = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Dec_InitParam_Set2: trick error!!\n" ));
			break;
	}
	
	// trick: keep_idx
	initparam.keep_idx = param->keep_idx;
	
	// vdpb bank num
	initparam.bank_num = param->bank_num;
	
	// self_rec_play
	switch( param->self_rec_play ){
		case H264_ON:
			initparam.self_rec_play = RB_ON;
			break;
		case H264_OFF:
			initparam.self_rec_play = RB_OFF;
			break;
		default:
			H264_Err_Print(( "H264_Ribery_Dec_InitParam_Set2: self_rec_play error!!\n" ));
			break;
	}
	
#ifdef CO_RB_MAP_PRINT
	H264_Info_Print(( "H264_Ribery_Dec_InitParam_Set2 setting check!!\n" ));
	H264_Info_Print(( "size          = %#x\n", initparam.size      ));
	H264_Info_Print(( "frame_rate    = %#x\n", initparam.framerate ));
	H264_Info_Print(( "scan_mode     = %#x\n", initparam.scan_mode ));
	H264_Info_Print(( "trick         = %#x\n", initparam.trick     ));
	H264_Info_Print(( "keep_idx      = %#x\n", initparam.keep_idx  ));
	H264_Info_Print(( "bank_num      = %#x\n", initparam.bank_num  ));
	H264_Info_Print(( "self_rec_play = %#x\n", initparam.self_rec_play  ));
#endif
	
	// InitParam setting
	ret = RB_InitParam_Set_Decode2( &initparam );
	
	if( ret != RB_OK ){
		H264_Err_Print(( "RB_InitParam_Set_Decode2 error!!\n" ));
	}
	
	return;
}

/**
 *	@brief		Decode Start Request
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Dec_Start( VOID )
{
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_CODEC_START_COMP );
	
	// request & wait response
	H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_DEC_START );
#ifndef CO_DEBUG_ON_PC
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_CODEC_START_COMP, D_OS_USER_TWF_ORW, (OS_USER_FLGPTN*)&flgptn, D_H264_RIBERY_START_TIMEOUT );
	if( ercd == D_OS_USER_E_OK ){
		H264_Info_Print(( "Boot Complete!!\n" ));
	}
	else{
		return H264_NG;
	}
#endif
	
	return H264_OK;
}

/**
 *	@brief		Decode Start Request2
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Dec_Start2( VOID )
{
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_CODEC_START2_COMP );
	
	// request & wait response
	H264_Ribery_Int_Request2( E_H264_RIBERY_IRQ_REQUEST_DEC_START );
#ifndef CO_DEBUG_ON_PC	
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_CODEC_START2_COMP, D_OS_USER_TWF_ORW, (OS_USER_FLGPTN*)&flgptn, D_H264_RIBERY_START_TIMEOUT );
	if( ercd == D_OS_USER_E_OK ){
		H264_Warning_Print(( "Boot Complete!!\n" ));
	}
	else{
		return H264_NG;
	}
#endif
	
	return H264_OK;
}

/**
 *	@brief		info_table/FIFO initialize Process.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Ribery_Dec_Vtable_Init( VOID )
{
	// info table init
	RB_InfoTable_Vcpb_Init();
	
	// FIFO table init
	H264_Ribery_Dec_Vcpb_Empty_Init();
	
	return;
}

/**
 *	@brief		info_table/FIFO initialize2 Process.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Ribery_Dec_Vtable_Init2( VOID )
{
	// info table init
	RB_InfoTable_Vcpb_Init2();
	
	// FIFO table init
	H264_Ribery_Dec_Vcpb_Empty_Init();
	
	return;
}


/**
 *	@brief		VCPB_info_table data set Process.\n
 *	@param		IN :index
 *				   :vcpb_info
 *	@param		OUT:None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Dec_VCPB_Set( UINT32 index, T_RB_INFOTABLE_VCPB* vcpb_info )
{
	INT32			ret;
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
	ret = RB_InfoTable_Vcpb_Set( index, vcpb_info );
	
	if( ret != RB_OK ){
		H264_Err_Print(( "H264_Ribery_Dec_VCPB_Set: RB_InfoTable_Vcpb_Set NG\n" ));
		return H264_NG;
	}
#if 0
	H264_Info_Print(( "\n" ));
	H264_Info_Print(( "[VCPB_info_table_set]\n" ));
	H264_Info_Print(( " index = %d\n", index));
	H264_Info_Print(( "  serial_num = %d\n", vcpb_info->serial_num ));
	H264_Info_Print(( "  PTS        = %ld\n", (ULONG)vcpb_info->PTS ));
	H264_Info_Print(( "  DTS        = %ld\n", (ULONG)vcpb_info->DTS ));
	H264_Info_Print(( "  pic_size   = %d\n", vcpb_info->pic_size ));
	H264_Info_Print(( "  pic_addr   = %#x\n", vcpb_info->pic_addr ));
	H264_Info_Print(( "\n" ));
#else
//	H264_Info_Print(( "VCPB DecReq. serial_num = [%d]\n", vcpb_info->serial_num ));
#endif
//	H264_Info_Print(( "addr_sta = %#x : addr_end = %#x\n", vcpb_info->pic_addr, (vcpb_info->pic_addr+vcpb_info->pic_size) ));
	
	// FIFO setting
	ret = RB_Fifo_Vcpb_DecReq_Set( index );
	
	if( ret != RB_OK ){
		H264_Err_Print(( "H264_Ribery_Dec_VCPB_Set: RB_Fifo_Vcpb_DecReq_Set NG\n" ));
		return H264_NG;
	}
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_FIFO_VCPB_DECREQ_SET );
	
	// Fifo Push req & wait response
	H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VCPB_DECREQ_SET );
#ifndef CO_DEBUG_ON_PC
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VCPB_DECREQ_SET, D_OS_USER_TWF_ORW, (OS_USER_FLGPTN*)&flgptn, D_H264_RIBERY_FIFO_TIMEOUT );
	if( ercd != D_OS_USER_E_OK ){
		H264_Err_Print(( "H264_Ribery_Dec_VCPB_Set: H264_Ribery_Int_Request NG\n" ));
		return H264_NG;
	}
	else{
//		H264_Info_Print(( "FIFO_VCPB_DECREQ_SET Complete!!\n" ));
	}
#endif
	
	// Decode request!!
	H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_DEC_VCPB_DECREQ );
	
	return H264_OK;
}

/**
 *	@brief		VCPB_info_table data set Process2.\n
 *	@param		IN :index
 *				   :vcpb_info
 *	@param		OUT:None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Dec_VCPB_Set2( UINT32 index, T_RB_INFOTABLE_VCPB* vcpb_info )
{
	INT32			ret;
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
	ret = RB_InfoTable_Vcpb_Set2( index, vcpb_info );
	
	if( ret != RB_OK ){
		H264_Err_Print(( "H264_Ribery_Dec_VCPB_Set2: RB_InfoTable_Vcpb_Set2 NG\n" ));
		return H264_NG;
	}
#if 0
	H264_Info_Print(( "\n" ));
	H264_Info_Print(( "[VCPB_info_table_set2]\n" ));
	H264_Info_Print(( " index = %d\n", index));
	H264_Info_Print(( "  serial_num = %d\n", vcpb_info->serial_num ));
	H264_Info_Print(( "  PTS        = %ld\n", (ULONG)vcpb_info->PTS ));
	H264_Info_Print(( "  DTS        = %ld\n", (ULONG)vcpb_info->DTS ));
	H264_Info_Print(( "  pic_size   = %d\n", vcpb_info->pic_size ));
	H264_Info_Print(( "  pic_addr   = %#x\n", vcpb_info->pic_addr ));
	H264_Info_Print(( "\n" ));
#else
//	H264_Info_Print(( "VCPB DecReq. serial_num = [%d]\n", vcpb_info->serial_num ));
#endif
	// FIFO setting
	ret = RB_Fifo_Vcpb_DecReq_Set2( index );
	
	if( ret != RB_OK ){
		H264_Err_Print(( "H264_Ribery_Dec_VCPB_Set2: RB_Fifo_Vcpb_DecReq_Set2 NG\n" ));
		return H264_NG;
	}
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_FIFO_VCPB_DECREQ_SET );
	
	// Fifo Push req & wait response
	H264_Ribery_Int_Request2( E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VCPB_DECREQ_SET );
#ifndef CO_DEBUG_ON_PC
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VCPB_DECREQ_SET, D_OS_USER_TWF_ORW, (OS_USER_FLGPTN*)&flgptn, D_H264_RIBERY_FIFO_TIMEOUT );
	if( ercd != D_OS_USER_E_OK ){
		H264_Err_Print(( "H264_Ribery_Dec_VCPB_Set2: H264_Ribery_Int_Request2 NG\n" ));
		return H264_NG;
	}
	else{
//		H264_Info_Print(( "FIFO_VCPB_DECREQ_SET Complete!!\n" ));
	}
#endif
	
	// Decode request!!
	H264_Ribery_Int_Request2( E_H264_RIBERY_IRQ_REQUEST_DEC_VCPB_DECREQ );
	
	return H264_OK;
}


/**
 *	@brief		VCPB info table release Process.\n
 *	@param		None
 *	@return		H264_OK	Normal End
 *	@return		H264_NG	Abnormal End
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Dec_VCPB_Free( UINT32 *index )
{
//	UINT32	index = 0;
	INT32	ret, ret2, ret3;
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_FIFO_VCPB_FREE_GET );
	
	// Fifo pop req & wait response
	H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VCPB_FREE_GET );
#ifndef CO_DEBUG_ON_PC
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VCPB_FREE_GET, D_OS_USER_TWF_ORW, (OS_USER_FLGPTN*)&flgptn, D_H264_RIBERY_FIFO_TIMEOUT );
	if( ercd != D_OS_USER_E_OK ){
		H264_Err_Print(( "Fifo pop req NG\n" ));
		return H264_ERR_SYSTEM;
	}
	else{
//		H264_Info_Print(( "FIFO_VCPB_FREE_GET Complete!!\n" ));
	}
#endif
	
	// index get from FIFO
	ret = RB_Fifo_Vcpb_Free_Get( index );
	if(( ret == RB_OK ) || ( ret == RB_COMPLETE )){
		
//		H264_Info_Print(( "VCPB free. index = [%d]\n", *index ));
		
		// VCPB Free
		ret2 = RB_InfoTable_Vcpb_Free( *index );
		if( ret2 != RB_OK ){
			H264_Err_Print(( "VCPB free NG\n" ));
			return H264_ERR_SYSTEM;
		}
		// Empty Set
		ret3 = H264_Ribery_Dec_Vcpb_Empty_Field_Set( *index );
		if( ret3 != H264_OK ){
			H264_Err_Print(( "Vcpb_Empty_Field_Set NG\n" ));	// There is no liberating area of VCPB_info_table
			return H264_ERR_SYSTEM;
		}
		if( ret == RB_COMPLETE )	return H264_OK_COMPLETE;	// Everything was liberated.
		else						return H264_OK;				// There is still remainder.
	}
	return H264_NG;	// over read
}

/**
 *	@brief		VCPB info table release Process2.\n
 *	@param		None
 *	@return		H264_OK	Normal End
 *	@return		H264_NG	Abnormal End
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Dec_VCPB_Free2( UINT32 *index )
{
//	UINT32	index = 0;
	INT32	ret, ret2, ret3;
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_FIFO_VCPB_FREE_GET );
	
	// Fifo pop req & wait response
	H264_Ribery_Int_Request2( E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VCPB_FREE_GET );
#ifndef CO_DEBUG_ON_PC
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VCPB_FREE_GET, D_OS_USER_TWF_ORW, (OS_USER_FLGPTN*)&flgptn, D_H264_RIBERY_FIFO_TIMEOUT );
	if( ercd != D_OS_USER_E_OK ){
		H264_Err_Print(( "H264_Ribery_Dec_VCPB_Free2 : H264_Ribery_Int_Request2 NG\n" ));
		return H264_NG;
	}
	else{
//		H264_Info_Print(( "FIFO_VCPB_FREE_GET Complete!!\n" ));
	}
#endif
	
	// index get from FIFO
	ret = RB_Fifo_Vcpb_Free_Get2( index );
	if(( ret == RB_OK ) || ( ret == RB_COMPLETE )){
		
//		H264_Info_Print(( "VCPB free. index = [%d]\n", *index ));
		
		// VCPB Free
		ret2 = RB_InfoTable_Vcpb_Free2( *index );
		if( ret2 != RB_OK ){
			H264_Err_Print(( "H264_Ribery_Dec_VCPB_Free2 : RB_InfoTable_Vcpb_Free2 NG\n" ));
			return H264_ERR_SYSTEM;
		}
		// Empty Set
		ret3 = H264_Ribery_Dec_Vcpb_Empty_Field_Set( *index );
		if( ret3 != H264_OK ){
			H264_Err_Print(( "Vcpb_Empty_Field_Set NG\n" ));	// There is no liberating area of VCPB_info_table
			return H264_ERR_SYSTEM;
		}
		if( ret == RB_COMPLETE )	return H264_OK_COMPLETE;	// Everything was liberated.
		else						return H264_OK;				// There is still remainder.
	}
	return H264_NG;	// over read
}

/**
 *	@brief		VDPB_info_table data get Process.\n
 *	@param		IN :None
 *				OUT:in_index
 *				   :vdpb_info
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Dec_VDPB_Get( UINT32* index, T_RB_INFOTABLE_VDPB* vdpb_info )
{
	INT32	ret, ret2;
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
	*index = 0xFF;
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_FIFO_VDPB_DECDONE_GET );
	
	// Fifo pop req & wait response
	H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VDPB_DECDONE_GET );
#ifndef CO_DEBUG_ON_PC
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VDPB_DECDONE_GET, D_OS_USER_TWF_ORW, (OS_USER_FLGPTN*)&flgptn, D_H264_RIBERY_FIFO_TIMEOUT );
	if( ercd != D_OS_USER_E_OK ){
		H264_Err_Print(( "H264_Ribery_Dec_VDPB_Get : H264_Ribery_Int_Request\n" ));
		return H264_NG;
	}
	else{
//		H264_Info_Print(( "FIFO_VDPB_DECDONE_GET Complete!!\n" ));
	}
#endif
	
	// index get from FIFO
	ret = RB_Fifo_Vdpb_DecDone_Get( index );
	if(( ret == RB_OK ) || ( ret == RB_COMPLETE )){
		
		// VDPB_Info_Table get
		ret2 = RB_InfoTable_Vdpb_Get( *index, vdpb_info );
		if( ret2 == RB_OK ){
#if 0
			H264_Info_Print(( "\n" ));
			H264_Info_Print(( "[VDPB_info_table_get]\n" ));
			H264_Info_Print(( " index = %d\n", *index));
			H264_Info_Print(( "  serial_num      = %d\n",vdpb_info->serial_num ));
			H264_Info_Print(( "  VCPB_serial_num = %d\n",vdpb_info->VCPB_serial_num ));
			H264_Info_Print(( "  PTS             = %ld\n",(ULONG)vdpb_info->PTS ));
			H264_Info_Print(( "  DTS             = %ld\n",(ULONG)vdpb_info->DTS ));
			H264_Info_Print(( "  err_code        = %d\n",vdpb_info->err_code ));
			H264_Info_Print(( "\n" ));
#else
//	H264_Info_Print(( "VCPB DecDone. serial_num = [%d]\n", vdpb_info->serial_num ));
#endif
			// input stream error check
			if ( vdpb_info->err_code ){
				H264_Err_Print(( "H264_Ribery_Dec_VDPB_Get input stream NG(%lu)\n", vdpb_info->err_code ));
//				return H264_ERR_FILE_STREAM;
			}
			
#ifdef CO_RB_DEBUG
			H264_Err_Print(( "VDPB(%d)\n", vdpb_info->serial_num));
#endif
			if( ret == RB_COMPLETE )	return H264_OK_COMPLETE;	// It finished reading everything. 
			else						return H264_OK;				// There is still remainder.
		}
	}
	
	return H264_NG;
}

/**
 *	@brief		VDPB_info_table data get Process2.\n
 *	@param		IN :None
 *				OUT:in_index
 *				   :vdpb_info
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Dec_VDPB_Get2( UINT32* index, T_RB_INFOTABLE_VDPB* vdpb_info )
{
	INT32	ret, ret2;
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
	*index = 0xFF;
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_FIFO_VDPB_DECDONE_GET );
	
	// Fifo pop req & wait response
	H264_Ribery_Int_Request2( E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VDPB_DECDONE_GET );
#ifndef CO_DEBUG_ON_PC
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VDPB_DECDONE_GET, D_OS_USER_TWF_ORW, (OS_USER_FLGPTN*)&flgptn, D_H264_RIBERY_FIFO_TIMEOUT );
	if( ercd != D_OS_USER_E_OK ){
		return H264_NG;
	}
	else{
//		H264_Info_Print(( "FIFO_VDPB_DECDONE_GET Complete!!\n" ));
	}
#endif
	
	// index get from FIFO
	ret = RB_Fifo_Vdpb_DecDone_Get2( index );
	if(( ret == RB_OK ) || ( ret == RB_COMPLETE )){
		
		// VDPB_Info_Table get
		ret2 = RB_InfoTable_Vdpb_Get2( *index, vdpb_info );
		if( ret2 == RB_OK ){
#if 0
			H264_Info_Print(( "\n" ));
			H264_Info_Print(( "[VDPB_info_table_get2]\n" ));
			H264_Info_Print(( " index = %d\n", *index));
			H264_Info_Print(( "  serial_num      = %d\n",vdpb_info->serial_num ));
			H264_Info_Print(( "  VCPB_serial_num = %d\n",vdpb_info->VCPB_serial_num ));
			H264_Info_Print(( "  PTS             = %ld\n",(ULONG)vdpb_info->PTS ));
			H264_Info_Print(( "  DTS             = %ld\n",(ULONG)vdpb_info->DTS ));
			H264_Info_Print(( "  err_code        = %d\n",vdpb_info->err_code ));
			H264_Info_Print(( "\n" ));
#else
//	H264_Info_Print(( "VCPB DecDone. serial_num = [%d]\n", vdpb_info->serial_num ));
#endif
			// input stream error check
			if ( vdpb_info->err_code ){
				H264_Err_Print(( "H264_Ribery_Dec_VDPB_Get2 input stream NG(%lu)\n", vdpb_info->err_code ));
				return H264_ERR_FILE_STREAM;
			}
			
#ifdef CO_RB_DEBUG
			H264_Err_Print(( "VDPB(%d)\n", vdpb_info->serial_num));
#endif
			if( ret == RB_COMPLETE )	return H264_OK_COMPLETE;	// It finished reading everything. 
			else						return H264_OK;				// There is still remainder.
		}
	}
	return H264_NG;
}


/**
 *	@brief		VDPB_free Process.\n
 *	@param		IN :index
 *	@param		OUT:None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Dec_VDPB_Free( UINT32 index )
{
	INT32 ret;
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
//	H264_Info_Print(( "VDPB free. index = [%d]\n", index ));
	
	ret = RB_Fifo_Vdpb_Free_Set( index );
	if( ret != RB_OK ){
		H264_Err_Print(( "H264_Ribery_Dec_VDPB_Free: RB_Fifo_Vdpb_Free_Set NG\n" ));
		return H264_NG;
	}
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_FIFO_VDPB_FREE_SET );
	
	// Fifo Push req & wait response
	H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VDPB_FREE_SET );
#ifndef CO_DEBUG_ON_PC
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VDPB_FREE_SET, D_OS_USER_TWF_ORW, (OS_USER_FLGPTN*)&flgptn, D_H264_RIBERY_FIFO_TIMEOUT );
	if( ercd != D_OS_USER_E_OK ){
		H264_Err_Print(( "H264_Ribery_Dec_VDPB_Free: H264_Ribery_Int_Request NG\n" ));
		return H264_NG;
	}
	else{
//		H264_Info_Print(( "FIFO_VDPB_FREE_SET Complete!!\n" ));
	}
#endif
	H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_DEC_VDPB_FREE );
	
	return H264_OK;
}

/**
 *	@brief		VDPB_free Process2.\n
 *	@param		IN :index
 *	@param		OUT:None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Dec_VDPB_Free2( UINT32 index )
{
	INT32 ret;
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	
//	H264_Info_Print(( "VDPB free. index = [%d]\n", index ));
	
	ret = RB_Fifo_Vdpb_Free_Set2( index );
	if( ret != RB_OK ){
		H264_Err_Print(( "H264_Ribery_Dec_VDPB_Free2: RB_Fifo_Vdpb_Free_Set2 NG\n" ));
		return H264_NG;
	}
	
	OS_User_Clr_Flg( FID_RIBERY, ~FLG_RIBERY_FIFO_VDPB_FREE_SET );
	
	// Fifo Push req & wait response
	H264_Ribery_Int_Request2( E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VDPB_FREE_SET );
#ifndef CO_DEBUG_ON_PC
	ercd = OS_User_Twai_Flg( FID_RIBERY, FLG_RIBERY_FIFO_VDPB_FREE_SET, D_OS_USER_TWF_ORW, (OS_USER_FLGPTN*)&flgptn, D_H264_RIBERY_FIFO_TIMEOUT );
	if( ercd != D_OS_USER_E_OK ){
		H264_Err_Print(( "H264_Ribery_Dec_VDPB_Free2: E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VDPB_FREE_SET TimeOut NG\n" ));
		return H264_NG;
	}
	else{
//		H264_Info_Print(( "FIFO_VDPB_FREE_SET Complete!!\n" ));
	}
#endif
	H264_Ribery_Int_Request2( E_H264_RIBERY_IRQ_REQUEST_DEC_VDPB_FREE );
	
	return H264_OK ;
}


/**
 *	@brief		VCPB info empty idx fifo initialize Process.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Ribery_Dec_Vcpb_Empty_Init( VOID )
{
	UINT32 idx = 0;
	
	for( idx = 0; idx < D_H264_RIBERY_VCPB_EMPTY_FIFO_MAX_NUM; idx++ ){
		gVcpb_Empty_Fifo.field[idx] = idx;
	}
	gVcpb_Empty_Fifo.idx_get  = 0;			/* for FieldGet */
	gVcpb_Empty_Fifo.idx_set  = 0;			/* for FieldSet */
	gVcpb_Empty_Fifo.full_flg = H264_ON;	/* for table status(In case of "ON". There is a free space.)  */
	gVcpb_Empty_Fifo.emp_flg  = H264_OFF;	/* for table status(In case of "ON". There is no free space.) */
	
	return;
}


/**
 *	@brief		VCPB info empty idx fifo Field Set Process.\n
 *	@param		IN :empty_index
 *				OUT:None
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Dec_Vcpb_Empty_Field_Set( UINT32 empty_index )
{
	if( gVcpb_Empty_Fifo.full_flg == H264_OFF ){
		/* empty table is Nothing */
		gVcpb_Empty_Fifo.field[ gVcpb_Empty_Fifo.idx_set ] = empty_index;
		gVcpb_Empty_Fifo.idx_set++;
		
		if( gVcpb_Empty_Fifo.idx_set >= D_H264_RIBERY_VCPB_EMPTY_FIFO_MAX_NUM ){
			gVcpb_Empty_Fifo.idx_set = 0;
		}
		
		if( gVcpb_Empty_Fifo.idx_set == gVcpb_Empty_Fifo.idx_get ){
			/* empty table is full */
			gVcpb_Empty_Fifo.full_flg = H264_ON;
		}
		else {
			/* empty table is full */
			gVcpb_Empty_Fifo.emp_flg = H264_OFF;
		}
	}
	else{
		/* empty table is full */
		H264_Err_Print(( "Vcpb_Empty is full\n" ));
		return H264_NG;
	}
	
	return H264_OK;
}


/**
 *	@brief		VCPB info empty idx fifo Field Get Process.\n
 *	@param		IN :None
 *				OUT:empty_index*
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
INT32 H264_Ribery_Dec_Vcpb_Empty_Field_Get( UINT32* empty_index )
{
	if( gVcpb_Empty_Fifo.emp_flg == H264_OFF ){
		/* empty table is full */
		if( gVcpb_Empty_Fifo.idx_get == gVcpb_Empty_Fifo.idx_set ){
			/* empty table is Nothing */
			gVcpb_Empty_Fifo.full_flg = H264_OFF;
		}
		
		*empty_index = gVcpb_Empty_Fifo.field[ gVcpb_Empty_Fifo.idx_get ];
		gVcpb_Empty_Fifo.idx_get++;
		
		if( gVcpb_Empty_Fifo.idx_get >= D_H264_RIBERY_VCPB_EMPTY_FIFO_MAX_NUM ){
			gVcpb_Empty_Fifo.idx_get = 0;
		}
		
		if( gVcpb_Empty_Fifo.idx_get == gVcpb_Empty_Fifo.idx_set ){
			/* empty table is Nothing */
			gVcpb_Empty_Fifo.emp_flg = H264_ON;
		}
	}
	else{
		/* empty table is Nothing */
		H264_Err_Print(( "Vcpb_Empty is Nothing\n" ));
		return H264_NG;
	}
	
	return H264_OK;
}

/**
 *	@brief		SPS analyze Process.\n
 *	@param		IN :SPS address
 *	@return		OK/NG
 *	@note		None
 *	@attention	In order for a main purpose to Get "frame_mbs_only_flag",
 *				not all are analyzed for "SPS".
 */
VOID H264_Ribery_Dec_Sps_Analyze( T_H264_RIBERY_DEC_SPS* sps, ULONG sps_addr )
{
	T_H264_MOVIE_STREAM_CONTROL	bit_stream;
	ULONG						loop;
	
	H264_Movie_Bs_Set_Pointer_Address( &bit_stream, (UCHAR*)sps_addr );
	
	// SPS Analyze start
	sps->profile_idc			= H264_Movie_Bs_Read_U_Nbit(&bit_stream, 8);
	sps->constraint_set0_flag	= H264_Movie_Bs_Read_U_1bit(&bit_stream);
	sps->constraint_set1_flag	= H264_Movie_Bs_Read_U_1bit(&bit_stream);
	sps->constraint_set2_flag	= H264_Movie_Bs_Read_U_1bit(&bit_stream);
	sps->constraint_set3_flag	= H264_Movie_Bs_Read_U_1bit(&bit_stream);
	sps->reserved_zero_4bits	= H264_Movie_Bs_Read_U_Nbit(&bit_stream,4);
	sps->level_idc				= H264_Movie_Bs_Read_U_Nbit(&bit_stream, 8);
	sps->seq_parameter_set_id	= H264_Movie_Bs_Read_U_E(&bit_stream);
	
	if( sps->profile_idc == 100 || sps->profile_idc == 110 ||
		sps->profile_idc == 122 || sps->profile_idc == 144 ){
		
		sps->chroma_format_idc	= H264_Movie_Bs_Read_U_E(&bit_stream);
		
		if( sps->chroma_format_idc == 3 ){
			sps->separate_colour_plane_flag	= H264_Movie_Bs_Read_U_1bit(&bit_stream);
		}
		
		sps->bit_depth_luma_minus8					= H264_Movie_Bs_Read_U_E(&bit_stream);
		sps->bit_depth_chroma_minus8				= H264_Movie_Bs_Read_U_E(&bit_stream);
		sps->qpprime_y_zero_transform_bypass_flag	= H264_Movie_Bs_Read_U_1bit(&bit_stream);
		sps->seq_scaling_matrix_present_flag		= H264_Movie_Bs_Read_U_1bit(&bit_stream);
		
		if( sps->seq_scaling_matrix_present_flag ){
			for( loop = 0; loop < 8; loop++ ){
				sps->seq_scaling_list_present_flag[loop] = H264_Movie_Bs_Read_U_1bit(&bit_stream);
				if( sps->seq_scaling_list_present_flag[loop] ){
					if( loop < 6 ){
						h264_ribery_dec_bs_read_scaling_list( &bit_stream, sps->ScalingList4x4[loop],		/* pgr0013 */ /* pgr0862 */
						16, &sps->UseDefaultScalingMatrix4x4Flag[loop]);									/* pgr0013 */ /* pgr0862 */
					}
					else{
						h264_ribery_dec_bs_read_scaling_list( &bit_stream, sps->ScalingList8x8[loop - 6],
						64, &sps->UseDefaultScalingMatrix8x8Flag[loop - 6] );
					}
				}
			}
		}
	}
	
	sps->log2_max_frame_num_minus4	= H264_Movie_Bs_Read_U_E(&bit_stream);
	sps->pic_order_cnt_type			= H264_Movie_Bs_Read_U_E(&bit_stream);
	
	if( sps->pic_order_cnt_type == 0 ){
		sps->log2_max_pic_order_cnt_lsb_minus4	= H264_Movie_Bs_Read_U_E(&bit_stream);
	}
	else if( sps->pic_order_cnt_type == 1 ){
		sps->delta_pic_order_always_zero_flag		= H264_Movie_Bs_Read_U_1bit(&bit_stream);
		sps->offset_for_non_ref_pic					= H264_Movie_Bs_Read_S_E(&bit_stream);
		sps->offset_for_top_to_bottom_field			= H264_Movie_Bs_Read_S_E(&bit_stream);
		sps->num_ref_frames_in_pic_order_cnt_cycle	= H264_Movie_Bs_Read_U_E(&bit_stream);
		
		for( loop = 0; loop < sps->num_ref_frames_in_pic_order_cnt_cycle; loop++ ){
			sps->offset_for_ref_frame[loop]			= H264_Movie_Bs_Read_S_E(&bit_stream);
		}
	}
	
	sps->num_ref_frames							= H264_Movie_Bs_Read_U_E(&bit_stream);
	sps->gaps_in_frame_num_value_allowed_flag	= H264_Movie_Bs_Read_U_1bit(&bit_stream);
	sps->pic_width_in_mbs_minus1				= H264_Movie_Bs_Read_U_E(&bit_stream);
	sps->pic_height_in_map_units_minus1			= H264_Movie_Bs_Read_U_E(&bit_stream);
	sps->frame_mbs_only_flag					= H264_Movie_Bs_Read_U_1bit(&bit_stream);
	
	return;
}

/**
 *	@brief		Bit Stream Read(Scaling List).\n
 *	@param		IN :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
VOID h264_ribery_dec_bs_read_scaling_list( T_H264_MOVIE_STREAM_CONTROL* bit_stream, INT32* scalingList, INT32 sizeOfScalingList, INT32* useDefaultScalingMatrixFlag )
{
	INT32	loop;
	INT32	delta_scale;
	INT32	lastScale = 8;
	INT32	nextScale = 8;
	
	for( loop = 0; loop < sizeOfScalingList; loop++ ){
		if( nextScale != 0 ){
			delta_scale = H264_Movie_Bs_Read_S_E(bit_stream);
			nextScale = ( lastScale + delta_scale + 256 ) % 256;
			*useDefaultScalingMatrixFlag = ( loop == 0 && nextScale == 0 );
		}
		scalingList[loop] = ( nextScale == 0 ) ? lastScale : nextScale;
		lastScale = scalingList[loop];
	}
	
	return;
}


/*----------------------------------------------------------------------*/
/* User Function for Ribery Driver										*/
/*----------------------------------------------------------------------*/
VOID H264_Ribery_AHclock_Set( UCHAR clk )
{
#ifdef CO_RB_ACT_CLOCK
	if( clk == H264_ON ){
		H264_Movie_Ribery_AHClock_Start( (UCHAR*)&gAHclk_Count );	// clock Start
	}
	else {
		H264_Movie_Ribery_AHClock_Stop( (UCHAR*)&gAHclk_Count );	// clock Stop
	}
#endif
	
	return;
}

VOID H264_Ribery_DSB( VOID )
{
	H264_Movie_Arm_Ribery_Dsb_Poc();
	
	return;
}

VOID H264_Ribery_Clean( ULONG addr, ULONG size )
{
#ifdef CO_RB_DEBUG
	H264_Info_Print(( "Clean(%#x,%d)\n", addr, size ));
#endif
	h264_ribery_clean( addr, size );
	
	return;
}

VOID H264_Ribery_Flush( ULONG addr, ULONG size )
{
#ifdef CO_RB_DEBUG
	H264_Info_Print(( "Flush(%#x,%d)\n", addr, size ));
#endif
	h264_ribery_flush( addr, size );
	
	return;
}

INT32 H264_Ribery_DI( VOID ){
	return H264_Movie_Ribery_DI();
}

VOID H264_Ribery_EI( UINT32 status ){
	H264_Movie_Ribery_EI(status);
}

