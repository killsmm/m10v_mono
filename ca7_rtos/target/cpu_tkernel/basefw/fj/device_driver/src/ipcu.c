/**
 * @file		ipcu.c
 * @brief		IPCU (Inter-Processor Communications Unit) process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h> 
#include "debug.h"
#include "os_user_custom.h"
#include "dd_arm.h"
#include "dd_ipcu.h"
#include "fj_host.h"
#include "sdram_map_cache_define.h"
#include "sdram_map_share.h"
#include "l1l2cache.h"
#include "ipcu.h"
#include "memory.h"
#include "os_if.h"

#ifdef __GNUC__
extern ULONG gSpin_Lock __attribute__((aligned(32)));
#else
extern __align(32) ULONG gSpin_Lock;
#endif
static ULONG sr = 0;

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
#define IPCU_MB_DATA_MAX			(D_DD_IPCU_MB_DATA_TOTAL_NUM)	// number of data in a Mail Box
#define IPCU_INT_CPU0_BASE			(0)								// CPU0
#define IPCU_INT_CPU1_BASE			(8)								// CPU1
#define IPCU_INT_CPU0_ACK			(IPCU_INT_CPU0_BASE + 0)		// CPU0 Send ch(#0)
#define IPCU_INT_CPU0_REQ			(IPCU_INT_CPU0_BASE + 1)		// CPU0 Recv ch(#1)
#define IPCU_INT_CPU1_ACK			(IPCU_INT_CPU1_BASE + 0)		// CPU1 Send ch(#8)
#define IPCU_INT_CPU1_REQ			(IPCU_INT_CPU1_BASE + 1)		// CPU1 Recv ch(#9)
#define IPCU_MB_SPINLOCK			(D_DD_IPCU_MB_MAX)				// Highest number for SpinLock reserved

#define IPCU_UNIT0_ID_MAX			(7)								// Max number of IPCU0 ID
#define IPCU_UNIT1_ID_MAX			(15)							// Max number of IPCU1 ID
#define IPCU_SEND_CM0_ID			(14)							// Mailbox ID for sending to CM0.
#define IPCU_RCV_CM0_ID				(15)							// Mailbox ID for receiving to CM0.

#define IPCU_BUFF_SIZE				(128*1024/4)					// IPCU buffer size(128kB) for CM0

#define IPCU_RES_CMD_CODE			(0x01000000)					// Command code for response (Request command + 0x01000000)

#define IPCU_STATE_NONE				(0)
#define IPCU_STATE_KEEP				(1)
#define IPCU_STATE_BF_ACT			(2)
#define IPCU_STATE_HOST_ACT			(4)

#define IPCU_MAGIC_NUMBER			(0xBEEFCAFEul)

#define IPCU_LOOP_CNT_MAX			(10)

#define FLG_CM0_IPCU_NOTIFY_UART_SEND		0x00000001
#define FLG_CM0_IPCU_NOTIFY_UART_RECV		0x00000002
#define FLG_CM0_IPCU_NOTIFY_SPI_SEND		0x00000004
#define FLG_CM0_IPCU_NOTIFY_SPI_RECV		0x00000008
#define FLG_CM0_IPCU_NOTIFY_I2C_SEND		0x00000010
#define FLG_CM0_IPCU_NOTIFY_I2C_RECV		0x00000020
/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
typedef struct {
	UINT8	state;
	UINT8	sync;
	UINT8*	orginal_data;
	UINT32	orginal_length;
	UINT8*	rest_data;
	UINT32	rest_length;
	UINT8*	local_data;
	UINT32	local_length;
} T_BF_IPCU_SEND_INFO;

typedef struct {
	UINT8*	buffer;
	UINT8	mailbox_num;
	UINT8	req_int_ch_num;
	UINT8	ack_int_ch_num;
} T_BF_IPCU_RESOURCE_INFO;

typedef VOID (*T_BF_IPCU_REQ_FUNC)(UCHAR, UCHAR*, UINT32, UINT32, UINT32);
typedef VOID (*T_BF_IPCU_RES_FUNC)(UCHAR, UCHAR*, UINT32);

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
// --- REMOVE_CAMERAIF BEGIN ---
// Finish Boot Status from Linux
static ULONG	gbBf_Ipcu_Finish_Boot_Status = 0;
// --- REMOVE_CAMERAIF END ---

/* Critical Section */
ULONG gBF_IPCU_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));
/* Mailbox Data */
ULONG gBF_IPCU_MB_Data[IPCU_ID_MAX][IPCU_MB_DATA_MAX];

/* for only CM0 */
/* IPCU read data */
ULONG gBF_IPCU_Read_Data;

/* IPCU check result */
ULONG gBF_IPCU_Check_Result;

/* Response command data */
ULONG gBF_IPCU_Res_Cmd_Data_CM0[10];

/*----------------------------------------------------------------------*/
/* Local Data                                                          */
/*----------------------------------------------------------------------*/
static T_BF_IPCU_SEND_INFO		bf_ipcu_send_info_tbl[IPCU_ID_MAX] = {{0}};

UINT8		bf_ipcu_buffer[IPCU_ID_MAX][IPCU_SEND_BUFFER_SIZE] __attribute__((section(".LINUX_RTOS_COMMON_BUFF")));
UINT32		bf_ipcu_sync[IPCU_ID_MAX] __attribute__((section(".LINUX_RTOS_COMMON_SYNC")));

static const T_BF_IPCU_RESOURCE_INFO	bf_ipcu_resource_info_tbl[IPCU_ID_MAX] ={
	 {bf_ipcu_buffer[0],   0,	0,	8},			/* Unit0 */
	 {bf_ipcu_buffer[1],   1,	1,	9},
	 {bf_ipcu_buffer[2],   2,	2, 10},
	 {bf_ipcu_buffer[3],   3,	3, 11},
	 {bf_ipcu_buffer[4],   4,	4, 12},
	 {bf_ipcu_buffer[5],   5,	5, 13},
	 {bf_ipcu_buffer[6],   6,	6, 14},
	 {bf_ipcu_buffer[7],   7,	7, 15},
	 {bf_ipcu_buffer[8],   8,	0,	6},			/* Unit1 */
	 {bf_ipcu_buffer[9],   9,	1,	7},
	 {bf_ipcu_buffer[10], 10,	2,  8},
	 {bf_ipcu_buffer[11], 11,	3,  9},
	 {bf_ipcu_buffer[12], 12,	5, 12},
	 {bf_ipcu_buffer[13], 13,  13, 10},
	 {bf_ipcu_buffer[14], 14,  14, 11},
	 {bf_ipcu_buffer[15], 15,	4, 15},
};
	 
static T_BF_IPCU_REQ_FUNC		bf_ipcu_req_func[IPCU_ID_MAX] = {0,};
static T_BF_IPCU_RES_FUNC		bf_ipcu_res_func[IPCU_ID_MAX] = {0,};

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function Prototype Definition                                  */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/
static BOOL bf_is_cache_memory( UINT32 addr )
{
	U_MEMORY_ATTRIBUTE attribute;
	fj_get_memory_attribute( addr, &attribute );
	if( (1==attribute.bit.l1_cache) || (1==attribute.bit.l2_cache) ) {
		return TRUE;
	}
	return FALSE;
}

static BOOL bf_is_shared_memory( UINT32 adrs )
{
	return Sdram_Map_Is_Shared(adrs);
}

// --- REMOVE_CAMERAIF BEGIN ---
static VOID bf_ipcu_set_finish_boot(ULONG boot_status)
{
	gbBf_Ipcu_Finish_Boot_Status = boot_status;
}

static ULONG bf_ipcu_get_finish_boot(VOID)
{
	return gbBf_Ipcu_Finish_Boot_Status;
}
// --- REMOVE_CAMERAIF END ---

static FJ_ERR_CODE bf_ipcu_send_repeat(UCHAR id)
{
	UINT32 src_num;
	UINT32 dst_num;
	UINT32 data_temp;
	INT32  ret;
	T_DD_IPCU_MNG ipcu_mng;
	
	src_num = bf_ipcu_resource_info_tbl[id].ack_int_ch_num;
	dst_num = bf_ipcu_resource_info_tbl[id].req_int_ch_num;

	/* Set IPCU management setting */
	ipcu_mng.src_ch_num = src_num;
	ipcu_mng.dst_ch_bit = ( 1 << dst_num );
	ipcu_mng.mask_ch_bit = 0;
	ipcu_mng.mode = E_DD_IPCU_MB_MODE_MANUAL_2;
	
	data_temp = ( UINT32 )( bf_ipcu_send_info_tbl[id].local_data );
	data_temp = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( data_temp );/* pgr1223 */
	ipcu_mng.data_size = D_DD_IPCU_SEND_DATA_MAX;
	ipcu_mng.send_data[0] = id;
	ipcu_mng.send_data[1] = data_temp;
	ipcu_mng.send_data[2] = bf_ipcu_send_info_tbl[id].local_length;
	
	if( !bf_is_shared_memory( (UINT32)(bf_ipcu_send_info_tbl[id].rest_data) ) ) {
#if 1
		memcpy( bf_ipcu_resource_info_tbl[id].buffer, bf_ipcu_send_info_tbl[id].rest_data, IPCU_SEND_BUFFER_SIZE );
#else
		memcpy( bf_ipcu_resource_info_tbl[id].buffer, bf_ipcu_send_info_tbl[id].rest_data, bf_ipcu_send_info_tbl[id].local_length );
#endif
		bf_ipcu_send_info_tbl[id].rest_data +=  bf_ipcu_send_info_tbl[id].local_length;
	}
	bf_ipcu_send_info_tbl[id].rest_length -= bf_ipcu_send_info_tbl[id].local_length;
	if( 0 == bf_ipcu_send_info_tbl[id].rest_length ) {
		ipcu_mng.send_data[3] = 0;				// no countinue
	} else {
		ipcu_mng.send_data[3] = 1;				// continue
	}
	
	ipcu_mng.send_data[4] = bf_ipcu_send_info_tbl[id].orginal_length;
	
	ret = Dd_IPCU_Ctrl( id, &ipcu_mng );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "bf_ipcu_send_repeat() - Setting error! : id=%d\n", id ));
		return FJ_ERR_NG_CONDITION;
	}
	
	OS_User_Clr_Flg( FID_IPCU, ~(1 << id) );

	// Start sending
	ret = Dd_IPCU_Start( id );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "bf_ipcu_send_repeat() - Send error! : id=%d\n", id ));
		return FJ_ERR_NG_TMOUT;
	}
	
	return FJ_ERR_OK;
}

static FJ_ERR_CODE bf_ipcu_send(UCHAR id, UCHAR *data, UINT32 length, UCHAR sync, UINT8 state)
{
	FJ_ERR_CODE ercd;
	UCHAR loop_cnt;

	if( id >= IPCU_ID_MAX ) {
		BF_Debug_Print_Warning(( "bf_ipcu_send() - id Error! : id=%d\n", id ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( NULL == data ) {
		BF_Debug_Print_Warning(( "bf_ipcu_send() - data address Error! : address=%lu\n", (ULONG)data ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( 0 == length ) {
		BF_Debug_Print_Warning(( "bf_ipcu_send() - length Error! : length=%d\n", length ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( 1 < sync ) {
		BF_Debug_Print_Warning(( "bf_ipcu_send() - sync Error! : sync=%d\n", sync ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	for( loop_cnt = 0; loop_cnt < IPCU_LOOP_CNT_MAX; loop_cnt++ ) {

		if( IPCU_MAGIC_NUMBER == bf_ipcu_sync[id] ) {
			/* IPCU available */
			break;
		}
		if( loop_cnt == IPCU_LOOP_CNT_MAX ) {
			BF_Debug_Print_Warning(( "bf_ipcu_send() - ready Error! : id=%d\n", id ));
			return FJ_ERR_NG_COMMUNICATON;
		}
		/* wait 1ms */
		OS_User_Dly_Tsk(1);
	}
	if( IPCU_STATE_KEEP != bf_ipcu_send_info_tbl[id].state ) {
		BF_Debug_Print_Warning(( "bf_ipcu_send() - state Error! : id=%d\n", id ));
		return FJ_ERR_NG_CONDITION;
	}

	bf_ipcu_send_info_tbl[id].state				= state;
	bf_ipcu_send_info_tbl[id].sync				= sync;
	bf_ipcu_send_info_tbl[id].orginal_data		= data;
	bf_ipcu_send_info_tbl[id].orginal_length	= length;
	bf_ipcu_send_info_tbl[id].rest_data			= data;
	bf_ipcu_send_info_tbl[id].rest_length		= length;

	if( bf_is_shared_memory( (UINT32)(bf_ipcu_send_info_tbl[id].rest_data) ) ) {
		bf_ipcu_send_info_tbl[id].local_data		= data;
		bf_ipcu_send_info_tbl[id].local_length		= length;
	} else {
		UINT32 length_tmp;
		if( IPCU_SEND_BUFFER_SIZE <= length ) {
			length_tmp = IPCU_SEND_BUFFER_SIZE;
		} else {
			length_tmp = length;
		}
		bf_ipcu_send_info_tbl[id].local_data	= bf_ipcu_resource_info_tbl[id].buffer;
		bf_ipcu_send_info_tbl[id].local_length	= length_tmp;
	}
	
	if( bf_is_cache_memory( (UINT32)data ) ) {
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, length );
	}
	
	ercd = bf_ipcu_send_repeat(id);
	if( ercd != FJ_ERR_OK ) {
		return ercd;
	}

	if( 0 != sync ) {
		OS_USER_FLGPTN flgptn;
		OS_User_Wai_Flg( FID_IPCU, (1 << id), D_OS_USER_TWF_ORW, &flgptn);
		OS_User_Clr_Flg( FID_IPCU, ~(1 << id) );
	}
	return FJ_ERR_OK;
}

static VOID bf_ipcu_req_cb( UCHAR id )
{
	INT32 ercd;
	ULONG src;
	
	ercd = Dd_IPCU_Get_Req_Data( id, gBF_IPCU_MB_Data[id], D_DD_IPCU_DATA_LENGTH );
	if( ercd != 0 ) {
		BF_Debug_Print_Error(( "bf_ipcu_req_cb(%d) Get Request data Error! : ercd=%d\n", ( UCHAR )id, ercd ));
	}
	
	ercd = Dd_IPCU_Get_MB_Dst_Stat( id, &src );
	if( ercd != 0 ) {
		BF_Debug_Print_Error(( "bf_ipcu_req_cb(%d) Get Dst status Error! : ercd=%d\n", ( UCHAR )id, ercd ));
	}
	
	ercd = Dd_IPCU_Clr_MB_Dst( id, src );
	if( ercd != 0 ) {
		BF_Debug_Print_Error(( "bf_ipcu_req_cb(%d) Clear Dstination Error! : ercd=%d\n", ( UCHAR )id, ercd ));
	}
	
	if ((id == 14) || (id == 15)) {
		// CM0
		if( NULL != bf_ipcu_req_func[id] ){
			(*bf_ipcu_req_func[id])( id, (UCHAR*)gBF_IPCU_MB_Data[id], 0, 0, 0 );
		}
	}
	else if( (id == 10) || (id == 11) ) {
		if( NULL != bf_ipcu_req_func[id] ){
			(*bf_ipcu_req_func[id])( id, (UCHAR*)gBF_IPCU_MB_Data[id], (IPCU_MB_DATA_MAX * 4), 0, 0 );
		}
		
		// Send acknowledge
		Dd_IPCU_Set_MB_Ack_Unit( id, bf_ipcu_resource_info_tbl[id].req_int_ch_num );
	}
	else {
		OS_User_Set_Flg( FID_IPCU_TASK, 1 << id );
	}
}

static FJ_ERR_CODE bf_ipcu_result_check_cm0( ULONG result )
{
	FJ_ERR_CODE ercd = FJ_ERR_OK;
	
	switch( result ) {
		case 1:
			// parameter
			ercd = FJ_ERR_NG_INPUT_PARAM;
			break;
		case 2:
			ercd = FJ_ERR_NG_CONDITION;
			break;
		case 3:
			ercd = FJ_ERR_NG;
			break;
		default:
			break;
	}
	
	return ercd;
}

/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/
FJ_ERR_CODE BF_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE ipctype, UCHAR *id)
{
	static BOOL ch14_opened = FALSE;
	INT32 ercd;

	if(ipctype == IPCU_SEND_CM0_ID){
		if(ch14_opened == TRUE){
			*id = IPCU_SEND_CM0_ID;
			return FJ_ERR_OK;
		}else{
			ch14_opened = TRUE;
		}
	}
	
	if( NULL == id ) {
		BF_Debug_Print_Error(( "BF_IPCU_Open() - id address Error! : address=%lu\n", (ULONG)id ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( ipctype >= E_FJ_IPCU_MAILBOX_TYPE_MAX ) {
		BF_Debug_Print_Error(( "BF_IPCU_Open() - channel Error! : ipctype=%d\n", ipctype ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( IPCU_STATE_NONE != bf_ipcu_send_info_tbl[ipctype].state ) {
		BF_Debug_Print_Error(( "BF_IPCU_Open() - state Error! : ipctype=%d, state:%d\n", ipctype, bf_ipcu_send_info_tbl[ipctype].state ));
		return FJ_ERR_NG_CONDITION;
	}
	ercd = Dd_IPCU_Open(( UCHAR )ipctype, D_DDIM_WAIT_END_TIME, id );
	if( ercd != 0 ) {
		BF_Debug_Print_Error(( "BF_IPCU_Open(%d) Error! : ercd=%d\n", ( UCHAR )ipctype, ercd ));
		return FJ_ERR_NG;
	}
	
	Dd_ARM_Critical_Section_Start( gBF_IPCU_Spin_Lock );
	memset( &bf_ipcu_send_info_tbl[ipctype], 0, sizeof(T_BF_IPCU_SEND_INFO));
	bf_ipcu_send_info_tbl[ipctype].state = IPCU_STATE_KEEP;
	Dd_ARM_Critical_Section_End( gBF_IPCU_Spin_Lock );
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_IPCU_Close(UCHAR id)
{
	INT32 ercd;

	if(id == IPCU_SEND_CM0_ID){
		return FJ_ERR_OK;
	}
	
	if( id >= IPCU_ID_MAX ) {
		BF_Debug_Print_Warning(( "BF_IPCU_Close() - id Error! : id=%d\n", id ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( IPCU_STATE_KEEP != bf_ipcu_send_info_tbl[id].state ) {
		BF_Debug_Print_Warning(( "BF_IPCU_Close() - state Error! : id=%d\n", id ));
		return FJ_ERR_NG_CONDITION;
	}
	
	ercd = Dd_IPCU_Close( id );
	if( ercd != 0 ) {
		BF_Debug_Print_Error(( "BF_IPCU_Close(%d) Error! : ercd=%d\n", id, ercd ));
		return FJ_ERR_NG;
	}
	
	bf_ipcu_req_func[id] = NULL;
	bf_ipcu_res_func[id] = NULL;

	Dd_ARM_Critical_Section_Start( gBF_IPCU_Spin_Lock );
	bf_ipcu_send_info_tbl[id].state = IPCU_STATE_NONE;
	Dd_ARM_Critical_Section_End( gBF_IPCU_Spin_Lock );

	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_IPCU_SetReqCB(UCHAR id, VOID (*req_fn_ptr)(UCHAR,UCHAR*,UINT32,UINT32,UINT32))
{
	static BOOL ch14_set_req = FALSE;
	INT32 ret = D_DDIM_OK;
	ULONG ch, unit;

	if(id == IPCU_SEND_CM0_ID){
		if(ch14_set_req == TRUE){
			return FJ_ERR_OK;
		}else{
			ch14_set_req = TRUE;
		}
	}
	
	if( id >= IPCU_ID_MAX ) {
		BF_Debug_Print_Warning(( "BF_IPCU_SetReqCB() - id Error! : id=%d\n", id ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( IPCU_STATE_KEEP != bf_ipcu_send_info_tbl[id].state ) {
		BF_Debug_Print_Warning(( "BF_IPCU_SetReqCB() - state Error! : id=%d\n", id ));
		return FJ_ERR_NG_CONDITION;
	}
	bf_ipcu_req_func[id] = req_fn_ptr;
	
	if( id <= IPCU_UNIT0_ID_MAX ) {
		unit = 0;
		ch = id;
	}
	else {
		unit = 1;
		ch = id - ( IPCU_UNIT0_ID_MAX + 1 );
	}
	
	ret = Dd_IPCU_Set_Req_CB( unit, ch, bf_ipcu_req_cb );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "BF_IPCU_SetReqCB(%d) Error! : ercd=%d\n", id, ret ));
		return FJ_ERR_NG;
	}
	
	bf_ipcu_sync[id] = IPCU_MAGIC_NUMBER;

	return FJ_ERR_OK;
}

VOID BF_IPCU_SetResCB( UCHAR id, VOID (*res_fn_ptr)(UCHAR, UCHAR*, UINT32) )
{
	INT32 ret = D_DDIM_OK;

	if( id >= IPCU_ID_MAX ) {
		BF_Debug_Print_Warning(( "BF_IPCU_SetResCB() - id Error! : id=%d\n", id ));
		return;
	}
	if( IPCU_STATE_KEEP != bf_ipcu_send_info_tbl[id].state ) {
		BF_Debug_Print_Warning(( "BF_IPCU_SetResCB() - state Error! : id=%d\n", id ));
		return;
	}
	bf_ipcu_res_func[id] = res_fn_ptr;
	ret = Dd_IPCU_Set_Ack_CB( id, NULL );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "BF_IPCU_SetResCB(%d) Error! : ercd=%d\n", id, ret ));
		return;
	}
}

FJ_ERR_CODE BF_IPCU_Send(UCHAR id, UCHAR *data, UINT32 length, UCHAR sync )
{
	return bf_ipcu_send( id, data, length, sync, IPCU_STATE_BF_ACT );
}

FJ_ERR_CODE BF_IPCU_DISP_Send(UCHAR id, UCHAR *data, UINT32 length, UCHAR sync )
{
	FJ_ERR_CODE ercd;
	T_DD_IPCU_MNG ipcu_mng;
	UINT32 src_num;
	UINT32 dst_num;
	
	if( id >= IPCU_ID_MAX ) {
		BF_Debug_Print_Warning(( "bf_ipcu_send() - id Error! : id=%d\n", id ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( NULL == data ) {
		BF_Debug_Print_Warning(( "bf_ipcu_send() - data address Error! : address=%d\n", *data ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( 0 == length ) {
		BF_Debug_Print_Warning(( "bf_ipcu_send() - length Error! : length=%d\n", length ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( 1 < sync ) {
		BF_Debug_Print_Warning(( "bf_ipcu_send() - sync Error! : sync=%d\n", sync ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	bf_ipcu_send_info_tbl[id].state                = IPCU_STATE_BF_ACT;
	bf_ipcu_send_info_tbl[id].sync                = sync;
	bf_ipcu_send_info_tbl[id].rest_length        = 0;
	
	
	
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, length );
	
	
	src_num = bf_ipcu_resource_info_tbl[id].ack_int_ch_num;
	dst_num = bf_ipcu_resource_info_tbl[id].req_int_ch_num;
	
	/* Set IPCU management setting */
	ipcu_mng.src_ch_num = src_num;
	ipcu_mng.dst_ch_bit = ( 1 << dst_num );
	ipcu_mng.mask_ch_bit = 0;
	ipcu_mng.mode = E_DD_IPCU_MB_MODE_MANUAL_2;
	
	ipcu_mng.data_size = D_DD_IPCU_SEND_DATA_MAX;
	
	memcpy( ipcu_mng.send_data, data, length );
	
	ercd = Dd_IPCU_Ctrl( id, &ipcu_mng );
	if( ercd != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "bf_ipcu_send_repeat() - Setting error! : id=%d\n", id ));
		return FJ_ERR_NG_CONDITION;
	}
	
	OS_User_Clr_Flg( FID_IPCU, ~(1 << id) );
	
	// Start sending
	ercd = Dd_IPCU_Start( id );
	if( ercd != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "bf_ipcu_send_repeat() - Send error! : id=%d\n", id ));
		return FJ_ERR_NG_TMOUT;
	}
	
	if( 0 != sync ) {
		OS_USER_FLGPTN flgptn;
		OS_User_Wai_Flg( FID_IPCU, (1 << id), D_OS_USER_TWF_ORW, &flgptn);
		OS_User_Clr_Flg( FID_IPCU, ~(1 << id) );
	}
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_IPCU_Send_CM0(UCHAR id, E_BF_IPCU_CMD_CM0 cmd, ULONG *data )
{
	INT32  ret;
	UINT32 src_num;
	UINT32 dst_num;
	T_DD_IPCU_MNG ipcu_mng;
	OS_USER_FLGPTN flgptn;
	OS_USER_ER		ercd;
	ULONG request_data[5];
	
	src_num = bf_ipcu_resource_info_tbl[id].ack_int_ch_num;
	dst_num = bf_ipcu_resource_info_tbl[id].req_int_ch_num;
	
	if( id >= IPCU_ID_MAX ) {
		BF_Debug_Print_Warning(( "BF_IPCU_Send_CM0() - id Error! : id = %d\n", id ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( cmd >= E_BF_IPCU_CMD_CM0_MAX ) {
		BF_Debug_Print_Warning(( "BF_IPCU_Send_CM0() - cmd Error! : cmd = %d\n", cmd ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( NULL == data ) {
		BF_Debug_Print_Warning(( "BF_IPCU_Send_CM0() - data address Error! : address = %lu\n", (ULONG)data ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( IPCU_STATE_KEEP != bf_ipcu_send_info_tbl[id].state ) {
		BF_Debug_Print_Warning(( "BF_IPCU_Send_CM0() - state Error! : id=%d\n", id ));
		return FJ_ERR_NG_CONDITION;
	}
	
	// response data clear
	memset( (VOID *)gBF_IPCU_Res_Cmd_Data_CM0, 0, (10 * 4) );
	
	/* Set IPCU management */
	ipcu_mng.src_ch_num = src_num;
	ipcu_mng.dst_ch_bit = ( 1 << dst_num );
	ipcu_mng.mask_ch_bit = 0;
	ipcu_mng.mode = E_DD_IPCU_MB_MODE_MANUAL_2;
	
	ipcu_mng.data_size = 5 * 4;
	
	// Set Command data
	memcpy( bf_ipcu_resource_info_tbl[id].buffer, data, (IPCU_MB_DATA_MAX * 4) );
	
	request_data[0] = id;												// Data#0 : Mailbox Number
	request_data[1] = (ULONG)(bf_ipcu_resource_info_tbl[id].buffer);	// Data#1 : Address
	request_data[2] = (IPCU_MB_DATA_MAX * 4);							// Data#2 : Size
	request_data[3] = 0;												// Data#3 : Continue
	request_data[4] = (IPCU_MB_DATA_MAX * 4);							// Data#4 : Total Size
	
	// Set IPCU Send Data
	memcpy( ipcu_mng.send_data, request_data, (5 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)request_data, ( 5 * 4 ));
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)bf_ipcu_resource_info_tbl[id].buffer, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = Dd_IPCU_Ctrl( id, &ipcu_mng );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "BF_IPCU_Send_CM0() - Setting error! : id=%d\n", id ));
		return FJ_ERR_NG_CONDITION;
	}
	
	// Start sending
	ret = Dd_IPCU_Start( id );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "BF_IPCU_Send_CM0() - Send error! : id=%d\n", id ));
		return FJ_ERR_NG_TMOUT;
	}
	
#ifdef CO_TKERNEL_ONLY
	ercd = OS_User_Twai_Flg( FID_IPCU, (1 << id), D_OS_USER_TWF_ORW, &flgptn, 1);	// wait time : 1ms.
#else
	ercd = OS_User_Twai_Flg( FID_IPCU, (1 << id), D_OS_USER_TWF_ORW, &flgptn, 120000);	// wait time : 120s.
#endif
	if( ercd != D_OS_USER_E_OK ) {
		BF_Debug_Print_Error(( "BF_IPCU_Send_CM0() - Timeout error! id=%d\n", id ));
		return FJ_ERR_NG_TMOUT;
	}
	OS_User_Clr_Flg( FID_IPCU, ~(1 << id) );
	
	return FJ_ERR_OK;
}

/**
 Request Interrupt Handler
*/
VOID BF_IPCU_Int_Handler_Req( UINT32 int_ch_num )
{
	Dd_IPCU_Int_Handler_Req( int_ch_num );
}

/**
 Request Interrupt Handler for CM0
*/
VOID BF_IPCU_Int_Handler_Req_CM0( UINT32 int_ch_num )
{
	ULONG	mb_adr = 0;
	ULONG	mb_num = 0;
	ULONG	mb_mod = 0;
	ULONG	id = 0;
	ULONG	data[5];
	
	/* Get Mail Box Number */
	Dd_IPCU_Get_MB_Adr( 1, (int_ch_num - 16), &mb_adr);
	mb_num = (( mb_adr - 0x100 ) >> 7 );
	id = mb_num + 8;
	
	// Get Data
	for( int loop = 0; loop < 5; loop++ ) {
		Dd_IPCU_Get_MB_Data( id, loop, &(data[loop]) );
	}
	
	// Clear destination.
	Dd_IPCU_Clr_MB_Dst_Unit(id, (int_ch_num - 16));
	
	// Get Mode.
	Dd_IPCU_Get_MB_Mode(id, &mb_mod);
	if ((mb_mod == E_DD_IPCU_MB_MODE_MANUAL_1) || (mb_mod == E_DD_IPCU_MB_MODE_MANUAL_2)) {
		// Manual Mode ---> Set Ack.
		Dd_IPCU_Set_MB_Ack_Unit(id, (int_ch_num - 16));
	}
	
	// Set Response command data
	memcpy( gBF_IPCU_Res_Cmd_Data_CM0, (VOID*)data[1], data[2] );
	
	// response command
	if( (gBF_IPCU_Res_Cmd_Data_CM0[0] & 0x01000000) == 0x01000000 ) {
		// Set event flag
		OS_User_Set_Flg( FID_IPCU, (1 << IPCU_SEND_CM0_ID) );
	}
	// notify command
	if( (gBF_IPCU_Res_Cmd_Data_CM0[0] & 0x02000000) == 0x02000000 ) {
//		printf("Receive Notification: 0x%08X\n", (UINT32)gBF_IPCU_Res_Cmd_Data_CM0[0]);
		switch(gBF_IPCU_Res_Cmd_Data_CM0[0]){
			case 0x02000003:
				OS_User_Set_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_UART_SEND );
			break;
			case 0x02000004:
				OS_User_Set_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_UART_RECV );
			break;
			case 0x02000005:
				OS_User_Set_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_SPI_SEND );
			break;
			case 0x02000006:
				OS_User_Set_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_SPI_RECV );
			break;
			case 0x02000007:
				OS_User_Set_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_I2C_SEND );
			break;
			case 0x02000008:
				OS_User_Set_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_I2C_RECV );
			break;
			default:
			break;
		}
	}
}

/**
 Acknowledge Interrupt Handler
*/
VOID BF_IPCU_Int_Handler_Ack( UINT32 int_ch_num )
{
	UCHAR id;
	ULONG mb_num;
	UINT8 state;
	INT32 ret;
	
	Dd_IPCU_Int_Handler_Ack( int_ch_num, &mb_num );
	id = mb_num;
	
	state = bf_ipcu_send_info_tbl[id].state;
	
	if( !bf_is_shared_memory( (UINT32)(bf_ipcu_send_info_tbl[id].rest_data) ) ) {
		UINT32 length_tmp;
		length_tmp =  bf_ipcu_send_info_tbl[id].rest_length;
		if( 0 != length_tmp ) {
			if( IPCU_SEND_BUFFER_SIZE <= length_tmp ) {
				length_tmp = IPCU_SEND_BUFFER_SIZE;
			}
			bf_ipcu_send_info_tbl[id].local_length	= length_tmp;
			bf_ipcu_send_repeat(id);
			return;
		}
	}
	
	bf_ipcu_send_info_tbl[id].state = IPCU_STATE_KEEP;
	
	if( 0 == bf_ipcu_send_info_tbl[id].sync ) {
		UCHAR *data;
		UINT32 length;
		data = bf_ipcu_send_info_tbl[id].orginal_data;
		length = bf_ipcu_send_info_tbl[id].orginal_length;
		if( IPCU_STATE_HOST_ACT == state ) {
			FJ_HOST_IPCUFinishCB( id, data, length );
		}
		else {
			ret = Dd_IPCU_Start_Ack_Proc( mb_num, int_ch_num );
			if( ret != D_DDIM_OK ) {
				BF_Debug_Print_Error(( "BF_IPCU_Int_Handler_Ack() Callback function Error! : mb_num = %lu, ch = %u, ercd=%d\n", mb_num, int_ch_num, ret ));
				return;
			}
		}
	}
	else {
		OS_User_Set_Flg( FID_IPCU, (1 << id) );
		ret = Dd_IPCU_Start_Ack_Proc( mb_num, int_ch_num );
		if( ret != D_DDIM_OK ) {
			BF_Debug_Print_Error(( "BF_IPCU_Int_Handler_Ack() Flag setting Error! : mb_num = %lu, ch = %u, ercd=%d\n", mb_num, int_ch_num, ret ));
			return;
		}
	}
}

/**
 Acknowledge Interrupt Handler for CM0
*/
VOID BF_IPCU_Int_Handler_Ack_CM0( UINT32 int_ch_num )
{
	UCHAR id;
	ULONG mb_num;
	ULONG mb_addr;
	INT32 ret;

	// Mailbox number
	ret = Dd_IPCU_Get_MB_Adr(1, int_ch_num - 16, &mb_addr);
	if ((mb_addr == 0) || (ret != D_DDIM_OK)) {
		BF_Debug_Print_Error(( "BF_IPCU_Int_Handler_Ack() Mailbox address not get : ch = %u, ercd=%d\n", int_ch_num, ret));
		return;
	}
	id = ((mb_addr - 0x100 ) >> 7) + 8;
	
	Dd_IPCU_Int_Handler_Ack( int_ch_num, &mb_num );
	
	bf_ipcu_send_info_tbl[id].state = IPCU_STATE_KEEP;
	
	ret = Dd_IPCU_Start_Ack_Proc( mb_num, int_ch_num );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "BF_IPCU_Int_Handler_Ack() Flag setting Error! : mb_num = %lu, ch = %u, ercd=%d\n", mb_num, int_ch_num, ret ));
		return;
	}
}

void IPCU_Task(int stacd)
{
	OS_USER_ID tskid;
	ULONG id;
	ULONG req_id;
	OS_USER_FLGPTN waiptn;

	OS_User_Get_Tid(&tskid);
	id = tskid - TID_IPCU_TASK0;
	waiptn = (1 << id);
	
	for(;;) {
		ULONG mb_num;
		UCHAR *data;
		UINT32 length;
		OS_USER_FLGPTN flgptn;
		
		OS_User_Wai_Flg( FID_IPCU_TASK, waiptn, D_OS_USER_TWF_ORW, &flgptn );
		OS_User_Clr_Flg( FID_IPCU_TASK, ~waiptn );
		mb_num = bf_ipcu_resource_info_tbl[id].mailbox_num;
		
		req_id 	= gBF_IPCU_MB_Data[id][0];
		data 	= ( UCHAR * )gBF_IPCU_MB_Data[id][1];
		length 	= gBF_IPCU_MB_Data[id][2];
		
		if( NULL != bf_ipcu_req_func[req_id] ){
			UINT32 cont;
			UINT32 total_length;
			UINT32 data_temp;
			data_temp = (UINT32)data;
			data_temp = M_SDRAM_MAP_COMMON_ADR_PHY2LOG(data_temp);/* pgr1223 */
			cont 	= gBF_IPCU_MB_Data[id][3];
			total_length 	= gBF_IPCU_MB_Data[id][4];
			
			(*bf_ipcu_req_func[req_id])( id, (UCHAR*)data_temp, length, cont, total_length );
		}
		
		// Send acknowledge
		Dd_IPCU_Set_MB_Ack_Unit( mb_num, bf_ipcu_resource_info_tbl[id].req_int_ch_num );
	}
}

VOID BF_IPCU_Get_Response_Commnad_CM0( ULONG* resp_command )
{
	// paramter check
	if (resp_command == NULL) {
		// NULL
		return;
	}
	// Set Response command data
	memcpy( resp_command, gBF_IPCU_Res_Cmd_Data_CM0, (10 * 4) );
}

VOID BF_IPCU_Set_Sync_CM0( VOID ) {
	// set Magic number : Mailbox13(Linux->CM0)
	bf_ipcu_sync[13] = IPCU_MAGIC_NUMBER;
}

FJ_ERR_CODE fj_ipcu_open(E_FJ_IPCU_MAILBOX_TYPE ipctype, UCHAR *id)
{
	return BF_IPCU_Open( ipctype, id );
}

FJ_ERR_CODE fj_ipcu_close(UCHAR id)
{
	return BF_IPCU_Close( id );
}

FJ_ERR_CODE fj_ipcu_setreceivercb(UCHAR id, VOID (*req_fn_ptr)(UCHAR,UINT32,UINT32,UINT32,UINT32))
{
	return BF_IPCU_SetReqCB( id, (VOID (*)(UCHAR,UCHAR*,UINT32,UINT32,UINT32))req_fn_ptr );
}

FJ_ERR_CODE fj_ipcu_send(UCHAR id, UINT32 pdata, UINT32 length, UCHAR sync)
{
	return bf_ipcu_send( id, (UCHAR*)pdata, length, sync, IPCU_STATE_HOST_ACT );
}

FJ_ERR_CODE fj_ipcu_setstateafterstandby_cm0( const E_FJ_IPCU_STATE_AFTER_STANDY next_state, const UINT32 wait_time, const UINT32 mode )
{
	ULONG data[9];
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_STATE_AFTER_STANDBY;
	data[1] = (ULONG)next_state;
	data[2] = (ULONG)wait_time;
	data[3] = (ULONG)mode;
	memset( (VOID *)&data[4], 0, (5 * 4) );
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_STATE_AFTER_STANDBY, ( ULONG* )data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_setstateafterstandby_cm0() - Setting error! : id=%d\n", IPCU_SEND_CM0_ID ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_sleep_cm0( VOID )
{
	ULONG data[9];
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_GO;
	memset( (VOID *)&data[1], 0, (8 * 4) );
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_GO, ( ULONG* )data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_sleep_cm0() - Setting error! : id=%d\n", IPCU_SEND_CM0_ID ));
	}
	
	return ret;
}

FJ_ERR_CODE fj_ipcu_write_cm0( ULONG type, ULONG address, ULONG write_data )
{
	ULONG data[9];
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_WRITE;
	data[1] = type;
	data[2] = address;
	data[3] = write_data;
	
	memset( (VOID *)&data[4], 0, (5 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_WRITE, data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_write_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_read_cm0( ULONG type, ULONG address, ULONG *read_data )
{
	ULONG data[9];
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_READ;
	data[1] = type;
	data[2] = address;
	
	memset( (VOID *)&data[3], 0, (6 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = FJ_ERR_OK;
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_READ, data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_read_cm0() - Setting error! : ret = %d\n", ret ));
		return ret;
	}
	
	*read_data = gBF_IPCU_Res_Cmd_Data_CM0[3];
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_read_modify_write_cm0( ULONG type, ULONG address, ULONG write_data, ULONG mask )
{
	ULONG data[9];
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_RMW;
	data[1] = type;
	data[2] = address;
	data[3] = write_data;
	data[4] = mask;
	
	memset( (VOID *)&data[5], 0, (4 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_RMW, data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_read_modify_write_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_set_tempera_buff_cm0( T_FJ_ARM_IPCU_TEMPERA tempera )
{
	ULONG data[9] ={0};
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_SET;
	data[1] = E_FJ_IPCU_BUFF_TEMPERA;
	data[2] = tempera.ipcu_high_tempera;
	data[3] = tempera.ipcu_near_by_high;
	data[4] = tempera.ipcu_near_by_low;
	data[5] = tempera.ipcu_low_tempera;
	memset( (VOID *)&data[6], 0, (3 * 4) );

	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 )); //IPCU_MB_DATA_MAX=9
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_set_tempera_buff_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_set_send_buff_cm0( E_FJ_IPCU_BUFF type, ULONG address, ULONG size )
{
	ULONG data[9];
	FJ_ERR_CODE ret;

	if( (type != E_FJ_IPCU_BUFF_UART_SEND) && (type != E_FJ_IPCU_BUFF_SPI_SEND) ) {
		BF_Debug_Print_Error(( "fj_ipcu_set_send_buff_cm0() - parameter error! : type = 0x%X\n", type ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	data[0] = E_BF_IPCU_CMD_CM0_SET;
	data[1] = type;
	data[2] = address;
	data[3] = size;
	
	memset( (VOID *)&data[4], 0, (5 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_set_send_buff_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_set_receive_buff_cm0( E_FJ_IPCU_BUFF type, ULONG address, ULONG size, ULONG wpt, ULONG overlap )
{
	static BOOL ch14_set_buff = FALSE;
	ULONG data[9];
	FJ_ERR_CODE ret;

	if(type == E_FJ_IPCU_BUFF_ADC){
		if(ch14_set_buff == TRUE){
			return FJ_ERR_OK;
		}else{
			ch14_set_buff = TRUE;
		}
	}

	if( (type != E_FJ_IPCU_BUFF_UART_RECEIVE) && (type != E_FJ_IPCU_BUFF_SPI_RECEIVE) && (type != E_FJ_IPCU_BUFF_ADC) ) {
		BF_Debug_Print_Error(( "fj_ipcu_set_receive_buff_cm0() - parameter error! : type = 0x%X\n", type ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	data[0] = E_BF_IPCU_CMD_CM0_SET;
	data[1] = type;
	data[2] = address;
	data[3] = size;
	data[4] = wpt;
	data[5] = overlap;
	
	memset( (VOID *)&data[6], 0, (3 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_set_receive_buff_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_set_i2c_send_buff_cm0( ULONG address, ULONG size, ULONG slave_address )
{
	ULONG data[9];
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_SET;
	data[1] = E_FJ_IPCU_BUFF_I2C_SEND;
	data[2] = address;
	data[3] = size;
	data[4] = slave_address;
	
	memset( (VOID *)&data[5], 0, (4 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)address, ( size * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_set_i2c_send_buff_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_set_i2c_receive_buff_cm0( ULONG address, ULONG size, ULONG wpt, ULONG overlap, ULONG slave_address )
{
	ULONG data[9];
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_SET;
	data[1] = E_FJ_IPCU_BUFF_I2C_RECEIVE;
	data[2] = address;
	data[3] = size;
	data[4] = wpt;
	data[5] = overlap;
	data[6] = slave_address;
	
	memset( (VOID *)&data[7], 0, (2 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)address, ( size * 4 ));
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)wpt, 4);
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_set_i2c_receive_buff_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_set_notify_cm0( E_FJ_IPCU_NOTIFY func, ULONG enable )
{
	static BOOL ch14_set_notify = FALSE;
	ULONG data[9];
	FJ_ERR_CODE ret;

	if(func == E_FJ_IPCU_NOTIFY_ADC){
		if(ch14_set_notify == TRUE){
			return FJ_ERR_OK;
		}else{
			ch14_set_notify = TRUE;
		}
	}

	switch(func){
		case E_FJ_IPCU_NOTIFY_UART_SEND:
			if(enable == 1) OS_User_Clr_Flg( FID_IPCU_CM0_NOTIFY, ~FLG_CM0_IPCU_NOTIFY_UART_SEND );
			else OS_User_Set_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_UART_SEND );
		break;
		case E_FJ_IPCU_NOTIFY_UART_RECEIVE:
			if(enable == 1) OS_User_Clr_Flg( FID_IPCU_CM0_NOTIFY, ~FLG_CM0_IPCU_NOTIFY_UART_RECV );
			else OS_User_Set_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_UART_RECV );
		break;
		case E_FJ_IPCU_NOTIFY_SPI_SEND:
			if(enable == 1) OS_User_Clr_Flg( FID_IPCU_CM0_NOTIFY, ~FLG_CM0_IPCU_NOTIFY_SPI_SEND );
			else OS_User_Set_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_SPI_SEND );
		break;
		case E_FJ_IPCU_NOTIFY_SPI_RECEIVE:
			if(enable == 1) OS_User_Clr_Flg( FID_IPCU_CM0_NOTIFY, ~FLG_CM0_IPCU_NOTIFY_SPI_RECV );
			else OS_User_Set_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_SPI_RECV );
		break;
		case E_FJ_IPCU_NOTIFY_I2C_SEND:
			if(enable == 1) OS_User_Clr_Flg( FID_IPCU_CM0_NOTIFY, ~FLG_CM0_IPCU_NOTIFY_I2C_SEND );
			else OS_User_Set_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_I2C_SEND );
		break;
		case E_FJ_IPCU_NOTIFY_I2C_RECEIVE:
			if(enable == 1) OS_User_Clr_Flg( FID_IPCU_CM0_NOTIFY, ~FLG_CM0_IPCU_NOTIFY_I2C_RECV );
			else OS_User_Set_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_I2C_RECV );
		break;
		default:
		break;
	}

	data[0] = E_BF_IPCU_CMD_CM0_SET;
	data[1] = func;
	data[2] = enable;
	
	memset( (VOID *)&data[3], 0, (6 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_set_notify_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_set_condition_cm0( ULONG enable )
{
	ULONG data[9];
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_SET;
	data[1] = E_FJ_IPCU_SET_WAKE_UP;
	data[2] = enable;
	
	memset( (VOID *)&data[3], 0, (6 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_set_condition_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_set_calendar_cm0( T_FJ_ARM_IPCU_CALENDAR calendar )
{
	ULONG data[9];
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_SET;
	data[1] = E_FJ_IPCU_SET_CALENDAR;
	data[2] = calendar.ipcu_second;
	data[3] = calendar.ipcu_minute;
	data[4] = calendar.ipcu_hour;
	data[5] = calendar.ipcu_day;
	data[6] = calendar.ipcu_day_of_week;
	data[7] = calendar.ipcu_month;
	data[8] = calendar.ipcu_year;
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_set_calendar_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_set_advfs_cm0( T_FJ_ARM_IPCU_ADVFS advfs )
{
	ULONG data[9] ={0};
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_SET;
	data[1] = E_FJ_IPCU_SET_ADVFS;
	data[2] = advfs.AVS; //AVS not allow to set
	data[3] = advfs.DVFS; //only can set DVFS, temporarily
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_set_advfs_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}
FJ_ERR_CODE fj_ipcu_set_advfs_en_cm0( ULONG enable )
{
	ULONG data[9] ={0};
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_SET;
	data[1] = E_FJ_IPCU_SET_ADVFS_EN;
	data[2] = enable; //set aDvfs-EN/Dis
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_set_advfs_en_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}
FJ_ERR_CODE fj_ipcu_set_PLL1_Frq_buf_cm0( ULONG frq_h, ULONG frq_l )
{
	ULONG data[9] ={0};
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_SET;
	data[1] = E_FJ_IPCU_SET_PLL1_FRQ_BUF;
	data[2] = frq_h;
	data[3] = frq_l;

	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 )); //IPCU_MB_DATA_MAX=9

	if( D_DDIM_OK != (ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data )) ) {
		BF_Debug_Print_Error(( "fj_ipcu_set_tempera_buff_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );	
}

FJ_ERR_CODE fj_ipcu_get_temperature_cm0( ULONG *temperature )
{
	ULONG data[9];
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_GET;
	data[1] = E_FJ_IPCU_GET_TEMPERA;
	
	memset( (VOID *)&data[2], 0, (7 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_get_temperature_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	*temperature = gBF_IPCU_Res_Cmd_Data_CM0[3];
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}
FJ_ERR_CODE fj_ipcu_get_advfs_cm0( T_FJ_ARM_IPCU_ADVFS *advfs )
{
	ULONG data[9] = {0};
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_GET;
	data[1] = E_FJ_IPCU_GET_ADVFS;

	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4));

	if( D_DDIM_OK != (ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data )) ) {
		BF_Debug_Print_Error(( "fj_ipcu_get_advfs_cm0() - Setting error! : ret = %d\n", ret ));
	}

	advfs->AVS = gBF_IPCU_Res_Cmd_Data_CM0[3];
	advfs->DVFS	= gBF_IPCU_Res_Cmd_Data_CM0[4];

	BF_L1l2cache_Clean_Flush_Addr( (UINT32)advfs, ((IPCU_MB_DATA_MAX - 2) * 4));
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}
FJ_ERR_CODE fj_ipcu_get_advfs_en_cm0( ULONG *enable )
{
	ULONG data[9] = {0};
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_GET;
	data[1] = E_FJ_IPCU_GET_ADVFS_EN;

	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4));

	if( D_DDIM_OK != (ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data )) ) {
		BF_Debug_Print_Error(( "fj_ipcu_get_advfs_en_cm0() - Setting error! : ret = %d\n", ret ));
	}

	*enable = gBF_IPCU_Res_Cmd_Data_CM0[3];
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}
FJ_ERR_CODE fj_ipcu_get_PLL1_Frq_buf_cm0( ULONG *frq_h, ULONG *frq_l )
{
	ULONG data[9] = {0};
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_GET;
	data[1] = E_FJ_IPCU_GET_PLL1_FRQ_BUF;

	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4));

	if( D_DDIM_OK != (ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data )) ) {
		BF_Debug_Print_Error(( "fj_ipcu_get_Frequency_buff_cm0() - Setting error! : ret = %d\n", ret ));
	}

	*frq_h = gBF_IPCU_Res_Cmd_Data_CM0[3];
	*frq_l = gBF_IPCU_Res_Cmd_Data_CM0[4];
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}
FJ_ERR_CODE fj_ipcu_get_Tempera_buf_cm0( T_FJ_ARM_IPCU_TEMPERA *tempera )
{
	ULONG data[9] ={0};
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_GET;
	data[1] = E_FJ_IPCU_GET_TEMPERA_BUF;
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 )); //IPCU_MB_DATA_MAX=9
	
	if( D_DDIM_OK != (ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data )) ) {
		BF_Debug_Print_Error(( "fj_ipcu_get_Tempera_buf_cm0() - Setting error! : ret = %d\n", ret ));
	}

	tempera->ipcu_high_tempera = gBF_IPCU_Res_Cmd_Data_CM0[3];
	tempera->ipcu_near_by_high = gBF_IPCU_Res_Cmd_Data_CM0[4];
	tempera->ipcu_near_by_low = gBF_IPCU_Res_Cmd_Data_CM0[5];
	tempera->ipcu_low_tempera = gBF_IPCU_Res_Cmd_Data_CM0[6];
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_get_calendar_cm0( T_FJ_ARM_IPCU_CALENDAR *calendar )
{
	ULONG data[9];
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_GET;
	data[1] = E_FJ_IPCU_GET_CALENDAR;
	
	memset( (VOID *)&data[2], 0, (7 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_get_calendar_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	calendar->ipcu_second		= gBF_IPCU_Res_Cmd_Data_CM0[3];
	calendar->ipcu_minute 		= gBF_IPCU_Res_Cmd_Data_CM0[4];
	calendar->ipcu_hour			= gBF_IPCU_Res_Cmd_Data_CM0[5];
	calendar->ipcu_day			= gBF_IPCU_Res_Cmd_Data_CM0[6];
	calendar->ipcu_day_of_week 	= gBF_IPCU_Res_Cmd_Data_CM0[7];
	calendar->ipcu_month 		= gBF_IPCU_Res_Cmd_Data_CM0[8];
	calendar->ipcu_year 		= gBF_IPCU_Res_Cmd_Data_CM0[9];
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)calendar, ((IPCU_MB_DATA_MAX - 2) * 4 ));
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_get_cm0( ULONG id, ULONG *get_data )
{
	ULONG data[9];
	FJ_ERR_CODE ret;

	data[0] = E_BF_IPCU_CMD_CM0_GET;
	data[1] = id;
	
	memset( (VOID *)&data[2], 0, (7 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_get_temperature_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	*get_data	= gBF_IPCU_Res_Cmd_Data_CM0[3];
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_start_cm0( E_FJ_IPCU_START func )
{
	OS_USER_FLGPTN flgptn;
	static BOOL ch14_start = FALSE;
	ULONG data[9];
	FJ_ERR_CODE ret;

	if(func == E_FJ_IPCU_START_ADC){
		if(ch14_start == TRUE){
			return FJ_ERR_OK;
		}else{
			ch14_start = TRUE;
		}
	}

	data[0] = E_BF_IPCU_CMD_CM0_START;
	data[1] = func;
	
	memset( (VOID *)&data[2], 0, (7 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_START, data );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_start_cm0() - Setting error! : ret = %d\n", ret ));
	}

	switch(func){
		case E_FJ_IPCU_START_UART_SEND:
			OS_User_Wai_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_UART_SEND, D_OS_USER_TWF_ORW, &flgptn);
			OS_User_Clr_Flg( FID_IPCU_CM0_NOTIFY, ~FLG_CM0_IPCU_NOTIFY_UART_SEND );
		break;
		case E_FJ_IPCU_START_UART_RECEIVE:
			OS_User_Wai_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_UART_RECV, D_OS_USER_TWF_ORW, &flgptn);
			OS_User_Clr_Flg( FID_IPCU_CM0_NOTIFY, ~FLG_CM0_IPCU_NOTIFY_UART_RECV );
		break;
		case E_FJ_IPCU_START_SPI_SEND:
			OS_User_Wai_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_SPI_SEND, D_OS_USER_TWF_ORW, &flgptn);
			OS_User_Clr_Flg( FID_IPCU_CM0_NOTIFY, ~FLG_CM0_IPCU_NOTIFY_SPI_SEND );
		break;
		case E_FJ_IPCU_START_SPI_RECEIVE:
			OS_User_Wai_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_SPI_RECV, D_OS_USER_TWF_ORW, &flgptn);
			OS_User_Clr_Flg( FID_IPCU_CM0_NOTIFY, ~FLG_CM0_IPCU_NOTIFY_SPI_RECV );
		break;
		case E_FJ_IPCU_START_I2C_SEND:
			OS_User_Wai_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_I2C_SEND, D_OS_USER_TWF_ORW, &flgptn);
			OS_User_Clr_Flg( FID_IPCU_CM0_NOTIFY, ~FLG_CM0_IPCU_NOTIFY_I2C_SEND );
		break;
		case E_FJ_IPCU_START_I2C_RECEIVE:
			OS_User_Wai_Flg( FID_IPCU_CM0_NOTIFY, FLG_CM0_IPCU_NOTIFY_I2C_RECV, D_OS_USER_TWF_ORW, &flgptn);
			OS_User_Clr_Flg( FID_IPCU_CM0_NOTIFY, ~FLG_CM0_IPCU_NOTIFY_I2C_RECV );
		break;
		default:
		break;
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_stop_cm0( E_FJ_IPCU_STOP func )
{
	static BOOL ch14_stop = FALSE;
	ULONG data[9];
	FJ_ERR_CODE ret;

	if(func == E_FJ_IPCU_STOP_ADC){
		if(ch14_stop == TRUE){
			return FJ_ERR_OK;
		}else{
			ch14_stop = TRUE;
		}
	}

	data[0] = E_BF_IPCU_CMD_CM0_STOP;
	data[1] = func;
	
	memset( (VOID *)&data[2], 0, (7 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_STOP, data );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_stop_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_set_rtc_time_cm0( T_FJ_ARM_IPCU_CALENDAR calendar )
{
	ULONG data[9];
	FJ_ERR_CODE ret;
	
	data[0] = E_BF_IPCU_CMD_CM0_SET;
	data[1] = E_FJ_IPCU_SET_RTC_TIME;
	data[2] = calendar.ipcu_second;
	data[3] = calendar.ipcu_minute;
	data[4] = calendar.ipcu_hour;
	data[5] = calendar.ipcu_day;
	data[6] = calendar.ipcu_day_of_week;
	data[7] = calendar.ipcu_month;
	data[8] = calendar.ipcu_year;
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_SET, data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_set_rtc_time_cm0() - Setting error! : ret = %d\n", ret ));
	}
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

// --- REMOVE_SECURE BEGIN ---
FJ_ERR_CODE fj_ipcu_check_secure_code_cm0( ULONG digest_addr, ULONG address, ULONG count, ULONG *result )
{
	ULONG data[9];
	FJ_ERR_CODE ret;
	ULONG secure_address;
	ULONG secure_size;
	ULONG loop_cnt;
	
	data[0] = E_BF_IPCU_CMD_CM0_CRYPTO;
	data[1] = digest_addr;
	data[2] = address;
	data[3] = count;
	
	memset( (VOID *)&data[4], 0, (5 * 4) );
	
	for (loop_cnt = 0; loop_cnt < count; loop_cnt++) {
		secure_address	= *((ULONG*)(data[2] + (loop_cnt * 8) - 0x20000000)) - 0x20000000;
		secure_size		= *((ULONG*)(data[2] + (loop_cnt * 8) + 4 - 0x20000000));
		BF_L1l2cache_Clean_Flush_Addr( secure_address, secure_size );
	}
	
	BF_L1l2cache_Clean_Flush_Addr( (data[2] - 0x20000000), ( count * 8 ));
	
	BF_L1l2cache_Clean_Flush_Addr( (data[1] - 0x20000000), ( 8 * 4 ));
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_CRYPTO, data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_check_secure_code_cm0() - Setting error! : ret = %d\n", ret ));
		return ret;
	}
	
	*result = gBF_IPCU_Res_Cmd_Data_CM0[3];
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}

FJ_ERR_CODE fj_ipcu_read_e_fuse_cm0( ULONG address, ULONG offset, ULONG size, ULONG *result )
{
	ULONG data[9];
	FJ_ERR_CODE ret;
	
	data[0] = E_BF_IPCU_CMD_CM0_GET;
	data[1] = E_FJ_IPCU_GET_E_FUSE;
	data[2] = address;
	data[3] = offset;
	data[4] = size;
	
	memset( (VOID *)&data[5], 0, (4 * 4) );
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)data, ( IPCU_MB_DATA_MAX * 4 ));
	
	ret = BF_IPCU_Send_CM0( IPCU_SEND_CM0_ID, E_BF_IPCU_CMD_CM0_GET, data );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(( "fj_ipcu_read_e_fuse_cm0() - Setting error! : ret = %d\n", ret ));
		return ret;
	}
	
	*result = gBF_IPCU_Res_Cmd_Data_CM0[2];
	
	return bf_ipcu_result_check_cm0( gBF_IPCU_Res_Cmd_Data_CM0[2] );
}
// --- REMOVE_SECURE END ---

// --- REMOVE_CAMERAIF BEGIN ---
VOID fj_ipcu_set_finish_boot(ULONG boot_status)
{
	bf_ipcu_set_finish_boot( boot_status );
}

ULONG fj_ipcu_get_finish_boot(VOID)
{
	return bf_ipcu_get_finish_boot();
}
// --- REMOVE_CAMERAIF END ---

VOID fj_spin_lock( VOID )
{
	sr = Dd_ARM_DI();	// Interrupt Disable
	Dd_ARM_Spin_Lock(&gSpin_Lock);
}

VOID fj_spin_unlock( VOID )
{
	Dd_ARM_Spin_Unlock(&gSpin_Lock);
	Dd_ARM_EI(sr);		// Interrupt Enable
}
