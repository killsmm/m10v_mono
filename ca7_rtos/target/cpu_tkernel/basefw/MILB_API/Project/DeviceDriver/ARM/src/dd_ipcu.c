/**
 * @file		dd_ipcu.c
 * @brief		IPCU (Inter-Processor Communications Unit) driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "dd_ipcu.h"	
#include "dd_arm.h"
#include "ddim_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
#define D_DD_IPCU_MB_NOT_USE		(0xFF)								/* IPCU Mailbox not used 				*/
#define D_DD_IPCU_UNIT_TOTAL_NUM	(3)									/* Total number of IPCU Unit 			*/
#define D_DD_IPCU_UNIT_MAX			(D_DD_IPCU_UNIT_TOTAL_NUM - 1)		/* IPCU Unit Max Value					*/
#define D_DD_IPCU_ID_TOTAL_NUM		(16)								/* Total number of IPCU ID				*/
#define D_DD_IPCU_ID_MAX			(D_DD_IPCU_ID_TOTAL_NUM - 1)		/* IPCU ID Max Value 					*/

#define D_DD_IPCU_MAGIC_NUMBER		(0xBEEFCAFEul)

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special

#endif //CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special

#endif //CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/* IPCU Send information table */
typedef struct {
	UCHAR	state;				/* State of IPCU process	*/
	UCHAR	async;				/* Async  			*/
} T_DD_IPCU_SEND_INFO;

/* IPCU Request data */
typedef struct {
	ULONG	mb_data[D_DD_IPCU_MB_DATA_TOTAL_NUM];	/* Mailbox data */
} T_DD_IPCU_MB_DATA;

/* IPCU Callback function for Request */
typedef VOID (*T_DD_IPCU_REQ_FUNC)( UCHAR );

/* IPCU Callback function for Acknowledge */
typedef VOID (*T_DD_IPCU_ACK_FUNC)( UCHAR );

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special

#endif //CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
ULONG gDD_IPCU_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special

#endif //CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special

#endif //CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special

#endif //CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
VOID dd_ipcu_memcpy_ulong( ULONG* const dst, ULONG* const src, UINT32 cp_size )
{
	UINT32	i;
	ULONG	words;

	words = ( cp_size + sizeof(ULONG) - 1 ) / sizeof(ULONG);

	for ( i = 0; i < words; i++ ){
		dst[i] = src[i];
	}
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special

#endif //CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Local Data                                                           */
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
static volatile T_DD_IPCU_SEND_INFO	gDD_IPCU_Send_Info_Tbl[D_DD_IPCU_MB_TOTAL_NUM] = {{0}};
static volatile T_DD_IPCU_REQ_FUNC	gDD_IPCU_Req_Func[D_DD_IPCU_UNIT_TOTAL_NUM][D_DD_IPCU_INT_TOTAL_NUM] = {{0}};
static volatile T_DD_IPCU_ACK_FUNC	gDD_IPCU_Ack_Func[D_DD_IPCU_MB_TOTAL_NUM] = {0};
static volatile T_DD_IPCU_MB_DATA	gDD_IPCU_Req_Data[D_DD_IPCU_MB_TOTAL_NUM] = {{{0}}};

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special

#endif //CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/**
 * @brief	Execute exclusive control for designated IPCU unit and Mailbox.
 * @param	UCHAR 	mb_num	Mailbox No.(0-15)
 * @param	INT32	tmout
 * @param	UCHAR* 	id		IPCU ID(0-15)
 * @return	D_DDIM_OK/D_DD_IPCU_INPUT_PARAM_ERR/D_DD_IPCU_MB_BUSY
 */
INT32 Dd_IPCU_Open( UCHAR mb_num, INT32 tmout, UCHAR *id )
{
	DDIM_USER_ER	ercd;
	
#ifdef CO_PARAM_CHECK
	if( mb_num > D_DD_IPCU_MB_MAX ) {
		Ddim_Assertion(( "Dd_IPCU_Open() - IPCU Mailbox No. exceeds the maximum. : MB No. = %d\n", mb_num ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( NULL == id ) {
		Ddim_Assertion(( "Dd_IPCU_Open() - IPCU ID address is NULL.\n" ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	if( tmout == D_DDIM_USER_SEM_WAIT_POL ) {
		ercd = DDIM_User_Pol_Sem( SID_DD_IPCU( mb_num ));
	}
	else{
		ercd = DDIM_User_Twai_Sem( SID_DD_IPCU( mb_num ), (DDIM_USER_TMO)tmout );
	}

	if( D_DDIM_USER_E_OK != ercd ) {
		if( D_DDIM_USER_E_TMOUT == ercd ) {
			return D_DD_IPCU_SEM_TIMEOUT;
		}
		return D_DD_IPCU_SEM_NG;
	}

	/* Get Mailbox */
	if( gDD_IPCU_Send_Info_Tbl[mb_num].state == D_DD_IPCU_STATE_NONE ) {
		*id = mb_num;
	}
	else {
		Ddim_Assertion(( "Dd_IPCU_Open(%d) - Selected Mailbox is busy.\n", mb_num ));
		return D_DD_IPCU_MB_BUSY;
	}
	
	/* Set Send info table */
	gDD_IPCU_Send_Info_Tbl[mb_num].state = D_DD_IPCU_STATE_KEEP;
	
	return D_DDIM_OK;
}

/**
 * @brief	Cancel exclusive control for designated IPCU unit and Mailbox.
 * @param	UCHAR id	IPCU ID(0-15)
 * @return	D_DDIM_OK/D_DD_IPCU_INPUT_PARAM_ERR/D_DD_IPCU_STATUS_ABNORMAL
 */
INT32 Dd_IPCU_Close( UCHAR id )
{
	UCHAR mb_idx;
	DDIM_USER_ER	ercd;
	
#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ) {
		Ddim_Assertion(( "Dd_IPCU_Close() - IPCU ID No. exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	ercd = DDIM_User_Sig_Sem( SID_DD_IPCU( id ));

	if( D_DDIM_USER_E_OK != ercd ) {
		return D_DD_IPCU_SEM_NG;
	}

	if( gDD_IPCU_Send_Info_Tbl[id].state != D_DD_IPCU_STATE_KEEP ) {
		Ddim_Assertion(( "Dd_IPCU_Close() - IPCU state Error! : ID = %d, Status = %d\n", id, gDD_IPCU_Send_Info_Tbl[id].state ));
		return D_DD_IPCU_STATUS_ABNORMAL;
	}
	
	/* Release Malibox */
	if( id < 8 ) {
		mb_idx = id;
		IO_IPCU0.MAILBOX[mb_idx].SRC.word = 0;		/* unit 0 */
	}
	else{
		mb_idx = id - 8;
		IO_IPCU1.MAILBOX[mb_idx].SRC.word = 0;		/* unit 1 */
	}
	
	/* Clear Send info table */
	gDD_IPCU_Send_Info_Tbl[id].state = D_DD_IPCU_STATE_NONE;
	
	/* Clear callback function */
	gDD_IPCU_Ack_Func[id] = NULL;
	
	return D_DDIM_OK;
}

/**
 * @brief	Set IPCU operation condition and callback function for designated IPCU unit and Mailbox.
 * @param	UCHAR id	IPCU ID(0-15)
 * @param	T_DD_IPCU_MNG ipcu_mng
 * @return	D_DDIM_OK/D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Ctrl( UCHAR id, T_DD_IPCU_MNG const* const ipcu_mng )
{
	ULONG src_num;
	ULONG dst_ch_bit;
	ULONG mask_ch_bit;
	UCHAR mb_idx;
	
	src_num = ipcu_mng -> src_ch_num;
	dst_ch_bit = ipcu_mng -> dst_ch_bit;
	mask_ch_bit = ipcu_mng -> mask_ch_bit;
	
#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ) {
		Ddim_Assertion(( "Dd_IPCU_Ctrl() - IPCU ID No. exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ipcu_mng == NULL ){
		Ddim_Assertion(( "Dd_IPCU_Ctrl() - IPCU MNG Tbl address is NULL. ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ipcu_mng -> data_size > D_DD_IPCU_SEND_DATA_MAX ) {
		Ddim_Assertion(( "Dd_IPCU_Ctrl() - IPCU data size exceeds the maximum. : ID = %d, size = %lu\n", id, ipcu_mng -> data_size ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Level1 & Level2 D-Cache Clean by Addressing & size */
	DDIM_User_L1l2cache_Clean_Flush_Addr( ( UINT32 )( ipcu_mng->send_data ), ipcu_mng->data_size );
	
	/* Clear local data */
	memset((VOID*)&gDD_IPCU_Ack_Func[id], 0, sizeof(T_DD_IPCU_SEND_INFO));
	
	if( id < 8 ){
		mb_idx = id;
		/* Set source */
		IO_IPCU0.MAILBOX[mb_idx].SRC.word = ( 1 << src_num );

		if( IO_IPCU0.MAILBOX[mb_idx].SRC.word != ( 1 << src_num )) {
			return D_DD_IPCU_MB_BUSY;
		}
		/* Set destination */
		IO_IPCU0.MAILBOX[mb_idx].DSTSET.word = dst_ch_bit;
		/* Set mask */
		IO_IPCU0.MAILBOX[mb_idx].MASKSET.word = mask_ch_bit;
		/* Set mode */
		IO_IPCU0.MAILBOX[mb_idx].MODE.bit.MODE = ipcu_mng -> mode;
		/* Set data */
		dd_ipcu_memcpy_ulong((ULONG*)&( IO_IPCU0.MAILBOX[mb_idx].DATA ), (ULONG*)&( ipcu_mng -> send_data ), ipcu_mng -> data_size );
	}
	else {
		mb_idx = id - 8;
		/* Set Mailbox */
		IO_IPCU1.MAILBOX[mb_idx].SRC.word = ( 1 << src_num );

		if( IO_IPCU1.MAILBOX[mb_idx].SRC.word != ( 1 << src_num )) {
			return D_DD_IPCU_MB_BUSY;
		}
		/* Set destination */
		IO_IPCU1.MAILBOX[mb_idx].DSTSET.word = dst_ch_bit;
		/* Set mask */
		IO_IPCU1.MAILBOX[mb_idx].MASKSET.word = mask_ch_bit;
		/* Set Mode */
		IO_IPCU1.MAILBOX[mb_idx].MODE.bit.MODE = ipcu_mng -> mode;
		/* Set data */
		dd_ipcu_memcpy_ulong((ULONG*)&( IO_IPCU1.MAILBOX[mb_idx].DATA ), (ULONG*)&( ipcu_mng -> send_data ), ipcu_mng -> data_size );
	}
	
	Dd_ARM_Dsb_Pou();

	return D_DDIM_OK;
}

/**
 * @brief	Get IPCU operation condition and callback function for designated IPCU unit and Mailbox.
 * @param	UCHAR id	IPCU ID(0-15)
 * @param	T_DD_IPCU_MNG *ipcu_mng
 * @return	D_DDIM_OK/D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_Ctrl( UCHAR id, T_DD_IPCU_MNG *const ipcu_mng )
{
	UCHAR src_cnt;
	UCHAR mb_idx;
	
#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ) {
		Ddim_Assertion(( "Dd_IPCU_Get_Ctrl() - IPCU ID No. exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ipcu_mng == NULL ){
		Ddim_Assertion(( "Dd_IPCU_Get_Ctrl() - IPCU MNG Tbl address is NULL. ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	if( id < 8 ){
		mb_idx = id;
		/* Get source */
		for( src_cnt = 0; src_cnt < D_DD_IPCU_ID_MAX; src_cnt++ ){
			if(1 & (IO_IPCU0.MAILBOX[mb_idx].SRC.word >> ( src_cnt ))) {
				ipcu_mng -> src_ch_num = src_cnt;
			}
		}
		/* Get destination */
		ipcu_mng -> dst_ch_bit = IO_IPCU0.MAILBOX[mb_idx].DSTSTAT.word;
		/* Get mask */
		ipcu_mng -> mask_ch_bit = IO_IPCU0.MAILBOX[mb_idx].MASKSTAT.word;
		/* Get mode */
		ipcu_mng -> mode = IO_IPCU0.MAILBOX[mb_idx].MODE.bit.MODE;
		/* Get data */
		dd_ipcu_memcpy_ulong( (ULONG*)&( ipcu_mng -> send_data ), (ULONG*)&( IO_IPCU0.MAILBOX[mb_idx].DATA ), ipcu_mng -> data_size );
	}
	else {
		mb_idx = id - 8;
		/* Get source */
		for( src_cnt = 0; src_cnt < D_DD_IPCU_ID_MAX; src_cnt++ ){
			if(1 & (IO_IPCU1.MAILBOX[mb_idx].SRC.word >> ( src_cnt ))) {
				ipcu_mng -> src_ch_num = src_cnt;
			}
		}
		/* Get destination */
		ipcu_mng -> dst_ch_bit = IO_IPCU1.MAILBOX[mb_idx].DSTSTAT.word;
		/* Get mask */
		ipcu_mng -> mask_ch_bit = IO_IPCU1.MAILBOX[mb_idx].MASKSTAT.word;
		/* Get mode */
		ipcu_mng -> mode = IO_IPCU1.MAILBOX[mb_idx].MODE.bit.MODE;
		/* Get data */
		dd_ipcu_memcpy_ulong( (ULONG*)&( ipcu_mng -> send_data ), (ULONG*)&( IO_IPCU1.MAILBOX[mb_idx].DATA ), ipcu_mng -> data_size );
	}
	
	return D_DDIM_OK;
}
/**
 * @brief	Start sending data.
 * @param	UCHAR id	IPCU ID(0-15)
 * @return	D_DDIM_OK/D_DD_IPCU_INPUT_PARAM_ERR/D_DD_IPCU_WAIT_TIMEOUT
 */
INT32 Dd_IPCU_Start( UCHAR id )
{
	DDIM_USER_FLGPTN flgptn;
	DDIM_USER_ER ercd;
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ) {
		Ddim_Assertion(( "Dd_IPCU_Start() - IPCU ID No. exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	if( id < 8 ){
		mb_idx = id;
		/* Set source register */
		IO_IPCU0.MAILBOX[mb_idx].SEND.bit.SEND = 1;	/* unit 0 */
	}
	else {
		mb_idx = id - 8;
		/* Set source register */
		IO_IPCU1.MAILBOX[mb_idx].SEND.bit.SEND = 1;	/* unit 1 */
	}
	/* Check Sync or Async */
	if( gDD_IPCU_Send_Info_Tbl[id].async == 0 ) {
		/* Sync process */
		ercd = DDIM_User_Twai_Flg( FID_DD_IPCU_ACK, (1 << id), D_DDIM_USER_TWF_ORW, &flgptn, D_DDIM_WAIT_END_TIME );
		DDIM_User_Clr_Flg( FID_DD_IPCU_ACK, ~(1 << id) );
		if( ercd == D_DDIM_USER_E_TMOUT ) {
			Ddim_Print(("Dd_IPCU_Start() : time out. ercd = %d\n", ercd));
			return D_DD_IPCU_WAIT_TIMEOUT;
		}
	}
	
	Dd_ARM_Dsb_Pou();

	return D_DDIM_OK;
}

/**
 * @brief	Acknowledge Interrupt Handler of IPCU request or acknowledge.
 * @param	UINT32 int_ch	Interrupt ch of IPCU(0-15)
 * @param	ULONG  *unit	IPCU unit(0-1)
 * @param	ULONG  *mb_idx	Mailbox number(0-15)
 */
VOID Dd_IPCU_Int_Handler_Ack( UINT32 int_ch, ULONG *mb_num )
{
	ULONG unit;
	ULONG mb_idx;
	ULONG ack_reg;
	
	/* Get Mail Box Number */
	if( 16 > int_ch ) {
		unit = 0;
		mb_idx = ( IO_IPCU0.MBADR[int_ch].bit.MBA - 0x100 ) >> 7;
		*mb_num = mb_idx;
	}
	else{
		unit = 1;
		int_ch -= 16;
		mb_idx = ( IO_IPCU1.MBADR[int_ch].bit.MBA - 0x100 ) >> 7;
		*mb_num = mb_idx + 8;
	}

	if( unit == 0 ) {
		/* Read acknowlegde status */
		ack_reg = IO_IPCU0.MAILBOX[mb_idx].ACKSTAT.word;
		/* Clear acknowlegde */
		IO_IPCU0.MAILBOX[mb_idx].ACKCLR.word = ack_reg;
		/* Clear source register */
		IO_IPCU0.MAILBOX[mb_idx].SRC.word = 0;
	}
	else {
		/* Read acknowlegde status */
		ack_reg = IO_IPCU1.MAILBOX[mb_idx].ACKSTAT.word;
		/* Clear acknowlegde */
		IO_IPCU1.MAILBOX[mb_idx].ACKCLR.word = ack_reg;
		/* Clear source register */
		IO_IPCU1.MAILBOX[mb_idx].SRC.word = 0;
	}
	
	Dd_ARM_Dsb_Pou();
	
#if 0
	/* Check Sync or Async */
	if( gDD_IPCU_Send_Info_Tbl[mb_idx].async == 0 ) {
		/* Sync process */
		ercd = DDIM_User_Set_Flg( FID_DD_IPCU_ACK, (1 << mb_idx));
		
		if( ercd == D_DDIM_USER_E_TMOUT ) {
			Ddim_Print(("Dd_IPCU_Int_Handler(ACK) : Set flag is timeout. ercd = %d\n", ercd));
		}
	}
	else {
		/* Async process */
		/* Callback check */
		if( gDD_IPCU_Ack_Func[mb_num] ) {
			/* Callback function */
			( *gDD_IPCU_Ack_Func[mb_num] )( (ULONG)int_ch );
		}
	}
#endif
}

/**
 * @brief	Request Interrupt Handler of IPCU request or acknowledge.
 * @param	UINT32 int_ch	Interrupt ch of IPCU(0-15)
 */
VOID Dd_IPCU_Int_Handler_Req( UINT32 int_ch )
{
	ULONG unit;
	ULONG mb_num;
	ULONG id;
	DDIM_USER_ER ercd;
	UCHAR loop_cnt;
	
	Dd_ARM_Critical_Section_Start( gDD_IPCU_Spin_Lock );
	
	/* Get Mail Box Number */
	if( 16 > int_ch ) {
		unit = 0;
		mb_num = ( IO_IPCU0.MBADR[int_ch].bit.MBA - 0x100 ) >> 7;
	}
	else{
		unit = 1;
		int_ch -= 16;
		mb_num = (( IO_IPCU1.MBADR[int_ch].bit.MBA - 0x100 ) >> 7 ) + 8;
	}

	if( unit == 0 ) {
		/* Get Data */
		id = mb_num;
		for( loop_cnt = 0; loop_cnt <= D_DD_IPCU_MB_DATA_MAX; loop_cnt++ ) {
			gDD_IPCU_Req_Data[mb_num].mb_data[loop_cnt] = IO_IPCU0.MAILBOX[id].DATA[loop_cnt];
		}
		/* clear Destination Register */
		if( IO_IPCU0.MBADR[int_ch].bit.REQ != 0 ) {
			IO_IPCU0.MAILBOX[id].DSTCLR.word = (1U << ( int_ch ));
		}
	}
	else {	/* unit 1 */
		/* Get Data */
		id = mb_num - 8;
		for( loop_cnt = 0; loop_cnt <= D_DD_IPCU_MB_DATA_MAX; loop_cnt++ ) {
			gDD_IPCU_Req_Data[mb_num].mb_data[loop_cnt] = IO_IPCU1.MAILBOX[id].DATA[loop_cnt];
		}
		/* clear Destination Register */
		if( IO_IPCU1.MBADR[int_ch].bit.REQ != 0 ) {
			IO_IPCU1.MAILBOX[id].DSTCLR.word = (1U << ( int_ch ));
		}
	}
	
	Dd_ARM_Dsb_Pou();
	
	/* Check Sync or Async */
	if( gDD_IPCU_Req_Func[unit][int_ch] ) {
		/* Async process */
		/* Callback function */
		(*gDD_IPCU_Req_Func[unit][int_ch])(( UCHAR )mb_num );
	}
	else {
		/* Sync process */
		ercd = DDIM_User_Set_Flg( FID_DD_IPCU_REQ, (1 << mb_num));
		
		if( ercd == D_DDIM_USER_E_TMOUT ) {
			Ddim_Print(("Dd_IPCU_Int_Handler_Req() : Set flag is timeout. ercd = %d\n", ercd));
		}
	}
	
	Dd_ARM_Critical_Section_End( gDD_IPCU_Spin_Lock );
}

/**
 * @brief	Start Ack process.
 * @param	ULONG  mb_num	Mailbox index(0-7)
 * @param	UINT32 int_ch	Interrupt ch of IPCU(0-15)
 * @return	D_DDIM_OK/D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Start_Ack_Proc( ULONG mb_num, ULONG int_ch )
{
	DDIM_USER_ER ercd;
	ULONG id;
	
#ifdef CO_PARAM_CHECK
	if( mb_num > D_DD_IPCU_MB_MAX ) {
		Ddim_Assertion(( "Dd_IPCU_Receive_Ack() - IPCU Mailbox No. exceeds the maximum. : MB No. = %lu\n", mb_num ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	id = mb_num;

	/* Check Sync or Async */
	if( gDD_IPCU_Send_Info_Tbl[id].async == 0 ) {
		/* Sync process */
		ercd = DDIM_User_Set_Flg( FID_DD_IPCU_ACK, (1 << id));
		
		if( ercd == D_DDIM_USER_E_TMOUT ) {
			Ddim_Print(("Dd_IPCU_Int_Handler(ACK) : Set flag is timeout. ercd = %d\n", ercd));
		}
	}
	else {
		/* Async process */
		/* Callback check */
		if( gDD_IPCU_Ack_Func[id] ) {
			/* Callback function */
			( *gDD_IPCU_Ack_Func[id] )( (ULONG)int_ch );
		}
	}
	return D_DDIM_OK;
}

/**
 * @brief	Wait IPCU Request.
 * @param	ULONG id		IPCU ID(0-15)
 * @return	D_DDIM_OK/D_DD_IPCU_INPUT_PARAM_ERR/D_DD_IPCU_WAIT_TIMEOUT
 */
INT32 Dd_IPCU_Wait_Req( ULONG id )
{
	DDIM_USER_FLGPTN waiptn, flgptn;
	DDIM_USER_ER ercd;
	
#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ) {
		Ddim_Print(( "Dd_IPCU_Wait_Req() - IPCU ID No. exceeds the maximum. : ID = %lu\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	waiptn = ( 1 << id );
	
	ercd = DDIM_User_Twai_Flg( FID_DD_IPCU_REQ, waiptn, D_DDIM_USER_TWF_ORW, &flgptn, D_DDIM_WAIT_END_TIME );
	DDIM_User_Clr_Flg( FID_DD_IPCU_REQ, ~waiptn );
	if( ercd == D_DDIM_USER_E_TMOUT ) {
		Ddim_Print(("Dd_IPCU_Wait_Req() : time out. ercd = %d\n", ercd));
		return D_DD_IPCU_WAIT_TIMEOUT;
	}
	
	/* Level1 & Level2 D-Cache Clean by Addressing & size */
	DDIM_User_L1l2cache_Clean_Flush_Addr(( UINT32 )( gDD_IPCU_Req_Data[id].mb_data ), D_DD_IPCU_DATA_LENGTH );
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Request data.
 * @param	ULONG id		IPCU ID(0-15)
 * @param	ULONG* data		Top address of request data.
 * @param	UCHAR  length	Size of data.
 * @return	D_DDIM_OK/D_DD_IPCU_WAIT_TIMEOUT
 */
INT32 Dd_IPCU_Get_Req_Data( ULONG id, ULONG *data, UCHAR length )
{
//	DDIM_USER_FLGPTN waiptn, flgptn;
//	DDIM_USER_ER ercd;
	
#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ) {
		Ddim_Print(( "Dd_IPCU_Get_Req_Data() - IPCU ID No. exceeds the maximum. : ID = %lu\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( length > D_DD_IPCU_SEND_DATA_MAX ) {
		Ddim_Assertion(( "Dd_IPCU_Get_Req_Data() - IPCU data length exceeds the maximum size. : ID = %lu, data = 0x%p, length = %d\n", id, data, length ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	dd_ipcu_memcpy_ulong( data, ( ULONG* )&( gDD_IPCU_Req_Data[id].mb_data ), length );
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Acknowledge callback function.
 * @param	UCHAR id		IPCU ID(0-15)
 * @param	T_DD_IPCU_ACK_FUNC	ack_fn_ptr Callback function of Acknowledge
 * @return	D_DDIM_OK/D_DD_IPCU_INPUT_PARAM_ERR/D_DD_IPCU_STATUS_ABNORMAL
 */
INT32 Dd_IPCU_Set_Ack_CB( UCHAR id, VOID (* ack_fn_ptr )( UCHAR ))
{
#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ) {
		Ddim_Print(( "Dd_IPCU_Set_Ack_CB() - IPCU ID No. exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ack_fn_ptr == NULL ) {
		Ddim_Print(( "Dd_IPCU_Set_Ack_CB() - Callback function is nothing. : ID = %d\n", id ));
		gDD_IPCU_Send_Info_Tbl[id].async = 0;
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	if( gDD_IPCU_Send_Info_Tbl[id].state != D_DD_IPCU_STATE_KEEP ) {
		Ddim_Print(( "Dd_IPCU_Set_Ack_CB() - state Error! : id=%d\n", id ));
		return D_DD_IPCU_STATUS_ABNORMAL;
	}
	
	gDD_IPCU_Ack_Func[id] = *ack_fn_ptr;
	gDD_IPCU_Send_Info_Tbl[id].async = 1;
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Request callback function.
 * @param	ULONG unit	IPCU unit(0-1)
 * @param	ULONG ch	IPCU interrupt ch(0-15)
 * @param	T_DD_IPCU_REQ_FUNC	p_callreq_fn_ptrback	Callback function of Request
 * @return	D_DDIM_OK/D_DD_IPCU_INPUT_PARAM_ERR/D_DD_IPCU_STATUS_ABNORMAL
 */
INT32 Dd_IPCU_Set_Req_CB( ULONG unit, ULONG ch, VOID (* req_fn_ptr )( UCHAR ))
{
#ifdef CO_PARAM_CHECK
	if( unit > D_DD_IPCU_UNIT_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_Req_CB() - Unit No. exceeds the maximum. :  Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_Req_CB() - INT Ch No. exceeds the maximum. :  Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	gDD_IPCU_Req_Func[unit][ch] = *req_fn_ptr;
	return D_DDIM_OK;
}

/*##################################
  ##   Interrupt Status Register  ##
  ##################################*/

/**
 * @brief	Get Interrupt Status
 * @param	ULONG unit		IPCU Unit(0-1)
 * @param	ULONG ch		Interrupt ch of IPCU(0-15)
 * @param	ULONG *state	Interrupt Status of IPCU
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_Int_Stat( ULONG unit, ULONG ch, ULONG *state )
{
#ifdef CO_PARAM_CHECK
	if( unit > D_DD_IPCU_UNIT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_Int_Stat() - Unit No. exceeds the maximum. :  Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_Int_Stat() - INT Ch No. exceeds the maximum. :  Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( state == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_Int_Stat() - State address is NULL. : Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	if( unit == 0 ){
		*state = IO_IPCU0.ISR[ch].word;		/* unit 0 */
	}
	else {
		*state = IO_IPCU1.ISR[ch].word;		/* unit 1 */
	}
	return D_DDIM_OK;
}

/**
 * @brief	Get Interrupt Status indicated by Mailbox number
 * @param	ULONG unit		IPCU Unit(0-1)
 * @param	ULONG ch		Interrupt ch of IPCU(0-15)
 * @param	UCHAR mb_num 	Mailbox number
 * @param	ULONG *state	Interrupt Status of IPCU
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_Int_Stat_MB( ULONG unit, ULONG ch, UCHAR mb_num, ULONG *state )
{
#ifdef CO_PARAM_CHECK
	if( unit > D_DD_IPCU_UNIT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_Int_Stat_MB() - Unit No. exceeds the maximum. : Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_Int_Stat_MB() - INT Ch No. exceeds the maximum. : Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( mb_num > D_DD_IPCU_MB_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_Int_Stat_MB() - IPCU Mailbox No. exceeds the maximum. : Unit = %lu, Ch = %lu Bit = %u\n", unit, ch, mb_num));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( state == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_Int_Stat_MB() - State address is NULL.\n" ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	if( unit == 0 ){
		*state = ( 1 & (IO_IPCU0.ISR[ch].word >> ( mb_num )));		/* unit 0 */
	}
	else {
		*state = ( 1 & (IO_IPCU1.ISR[ch].word >> ( mb_num )));		/* unit 1 */
	}
	return D_DDIM_OK;
}

/*##################################
  ##   Mailbox Address Register   ##
  ##################################*/
/**
 * @brief	Get Acknowledge status
 * @param	ULONG unit	IPCU Unit(0-1)
 * @param	ULONG ch	Interrupt ch of IPCU(0-15)
 * @param	ULONG *ack  Acknowledge(0:not Ack, 1:Ack)
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Ack( ULONG unit, ULONG ch, ULONG *ack )
{
#ifdef CO_PARAM_CHECK
	if( unit > D_DD_IPCU_UNIT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Ack() - Unit No. exceeds the maximum. : Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Ack() - INT Ch No. exceeds the maximum. : Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ack == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Ack() - Ack address is NULL. : Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	if( unit == 0 ){
		*ack = IO_IPCU0.MBADR[ch].bit.ACK;		/* unit 0 */
	}
	else {
		*ack = IO_IPCU1.MBADR[ch].bit.ACK;		/* unit 1 */
	}
	return D_DDIM_OK;
}

/**
 * @brief	Get Request status
 * @param	ULONG unit	IPCU Unit(0-1)
 * @param	ULONG ch	Interrupt ch of IPCU(0-15)
 * @param	ULONG *req  Request(0:not Req, 1:Req)
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Req( ULONG unit, ULONG ch, ULONG *req )
{
#ifdef CO_PARAM_CHECK
	if( unit > D_DD_IPCU_UNIT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Req() - Unit No. exceeds the maximum. : Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Req() - INT Ch No. exceeds the maximum. : Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( req == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Req() - Req address is NULL. : Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	if( unit == 0 ){
		*req = IO_IPCU0.MBADR[ch].bit.REQ;		/* unit 0 */
	}
	else {
		*req = IO_IPCU1.MBADR[ch].bit.REQ;		/* unit 1 */
	}
	return D_DDIM_OK;
}

/**
 * @brief	Get Mailbox Address
 * @param	ULONG unit	IPCU Unit(0-1)
 * @param	ULONG ch	Interrupt ch of IPCU(0-15)
 * @param	ULONG *mba  Mailbox Address
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Adr( ULONG unit, ULONG ch, ULONG *mba )
{
#ifdef CO_PARAM_CHECK
	if( unit > D_DD_IPCU_UNIT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Adr() - Unit No. exceeds the maximum. : Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Adr() - INT Ch No. exceeds the maximum. : Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( mba == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Adr() - Mailbox address is NULL. Unit = %lu, Ch = %lu\n", unit, ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	if( unit == 0 ){
		*mba = IO_IPCU0.MBADR[ch].bit.MBA;		/* unit 0 */
	}
	else {
		*mba = IO_IPCU1.MBADR[ch].bit.MBA;		/* unit 1 */
	}
	return D_DDIM_OK;
}

/*##################################
  ##   	    Source Register       ##
  ##################################*/
/**
 * @brief	Get Source information
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG *src  Request transmit source
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Src( UCHAR id, ULONG *src )
{
		UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Src() - Mailbox number exceeds the maximum. : ID = %u\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( src == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Src() - Mailbox address is NULL. : ID = %u\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	/* Get unit No. */
	if( id < 8 ) {
		mb_idx = id;
		*src = IO_IPCU0.MAILBOX[mb_idx].SRC.word;		/* unit 0 */
	}
	else {
		mb_idx = id - 8;
		*src = IO_IPCU1.MAILBOX[mb_idx].SRC.word;		/* unit 1 */
	}
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Source information indicated by IPCU ch
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG src_ch	Interrupt ch of IPCU(0-15)
 * @param	ULONG *src  	Request transmit source
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Src_Unit( UCHAR id, ULONG src_ch, ULONG *src )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Src_Unit() - Mailbox number exceeds the maximum. : ID = %d, Ch = %lu\n", id, src_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( src_ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Src_Unit() - INT Ch No. exceeds the maximum. : ID = %d Ch = %lu\n", id, src_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( src == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Src_Unit() - Mailbox address is NULL. : ID = %d, Ch = %lu\n", id, src_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {					/* unit 0 */
		mb_idx = id;
		*src = (1 & (IO_IPCU0.MAILBOX[mb_idx].SRC.word >> ( src_ch )));
	}
	else {							/* unit 2 */
		mb_idx = id - 8;
		*src = (1 & (IO_IPCU1.MAILBOX[mb_idx].SRC.word >> ( src_ch )));
	}
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Source information
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG src   Request transmit source
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Set_MB_Src( UCHAR id, ULONG src )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_MB_Src() - Mailbox number exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( src == 0 ){
		Ddim_Print(( "Dd_IPCU_Set_MB_Src() - It's not registered. : ID = %d, SRC = %lu\n", id, src));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	/* Get unit No. */
	if( id < 8 ) {					/* unit 0 */
		mb_idx = id;
		IO_IPCU0.MAILBOX[mb_idx].SRC.word = src;
	}
	else {							/* unit 1 */
		mb_idx = id - 8;
		IO_IPCU1.MAILBOX[mb_idx].SRC.word = src;
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/*##################################
  ##        Mode Register         ##
  ##################################*/
/**
 * @brief	Get Mode information
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG *mode Operation mode
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Mode( UCHAR id, ULONG *mode )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Mode() - Mailbox number exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( mode == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Mode() - Mode address is NULL. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {					/* unit 0 */
		mb_idx = id;
		*mode = IO_IPCU0.MAILBOX[mb_idx].MODE.bit.MODE;
	}
	else {							/* unit 1 */
		mb_idx = id - 8;
		*mode = IO_IPCU1.MAILBOX[mb_idx].MODE.bit.MODE;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Mode information
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	E_DD_IPCU_MB_MODE mode  Operation mode
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Set_MB_Mode( UCHAR id, E_DD_IPCU_MB_MODE mode )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_MB_Mode() - Mailbox number exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( mode >= E_DD_IPCU_MB_MODE_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_MB_Mode() - Mode address is NULL. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {					/* unit 0 */
		mb_idx = id;
		IO_IPCU0.MAILBOX[mb_idx].MODE.bit.MODE = mode;
	}
	else {							/* unit 1 */
		mb_idx = id - 8;
		IO_IPCU1.MAILBOX[mb_idx].MODE.bit.MODE = mode;
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/*##################################
  ##        Send Register         ##
  ##################################*/
/**
 * @brief	Get Send trigger
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG *send Send trigger
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Send( UCHAR id, ULONG *send )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Send() - Mailbox number exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( send == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Send() - Send address is NULL. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		*send = IO_IPCU0.MAILBOX[mb_idx].SEND.bit.SEND;	
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		*send = IO_IPCU1.MAILBOX[mb_idx].SEND.bit.SEND;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Send trigger
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Set_MB_Send( UCHAR id )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_MB_Send() - Mailbox number exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		if( IO_IPCU0.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Send() - Request transmit source is not set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}

		IO_IPCU0.MAILBOX[mb_idx].SEND.bit.SEND = 1;
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		if( IO_IPCU1.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Send() - Request transmit source is not set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}

		IO_IPCU1.MAILBOX[mb_idx].SEND.bit.SEND = 1;
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/*##################################
  ##  Destination Set Register    ##
  ##################################*/
/**
 * @brief	Set Destination information
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG dst 	Request transmit destination
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Set_MB_Dst( UCHAR id, ULONG dst )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Src() - Mailbox number exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		/* Check Sourece register */
		if( IO_IPCU0.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst() - Request transmit source is not set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU0.MAILBOX[mb_idx].SEND.bit.SEND != 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst() - Send trigger is set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU0.MAILBOX[mb_idx].DSTSET.word = dst;
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		/* Check Sourece register */
		if( IO_IPCU1.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst() - Request transmit source is not set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU1.MAILBOX[mb_idx].SEND.bit.SEND != 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst() - Send trigger is set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU1.MAILBOX[mb_idx].DSTSET.word = dst;
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Destination information indicated by IPCU ch
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG dst_ch 	Request transmit destination ch(0-15)
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Set_MB_Dst_Unit( UCHAR id, ULONG dst_ch )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_MB_Dst_Unit() - Mailbox number exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( dst_ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_MB_Dst_Unit() - INT Ch No. exceeds the maximum. : ID = %d Ch = %lu\n", id, dst_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		/* Check Sourece register */
		if( IO_IPCU0.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst_Unit() - Request transmit source is not set. : ID = %d Ch = %lu\n", id, dst_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU0.MAILBOX[mb_idx].SEND.bit.SEND != 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst_Unit() - Send trigger is set. : ID = %d Ch = %lu\n", id, dst_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU0.MAILBOX[mb_idx].DSTSET.word = (1U << (dst_ch));
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		/* Check Sourece register */
		if( IO_IPCU1.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst_Unit() - Request transmit source is not set. : ID = %d Ch = %lu\n", id, dst_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU1.MAILBOX[mb_idx].SEND.bit.SEND != 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst_Unit() - Send trigger is set. : ID = %d Ch = %lu\n", id, dst_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU1.MAILBOX[mb_idx].DSTSET.word = (1U << (dst_ch));
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/*##################################
  ##  Destination Clear Register  ##
  ##################################*/
/**
 * @brief	Clear Destination information
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG dst 	Request transmit destination by bit data
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Clr_MB_Dst( UCHAR id, ULONG dst )
{
	UCHAR loop_cnt;
	UCHAR bit_cnt = 0;
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Clr_MB_Dst() - Mailbox number exceeds the maximum. : ID = %d, Dst = %lu\n", id, dst ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	
	for( loop_cnt = 0; loop_cnt <= D_DD_IPCU_INT_MAX; loop_cnt++ ) {
		if( 1 & ( dst >> loop_cnt )) {
			bit_cnt++;
		}
	}
	if( bit_cnt > 1 ){
		Ddim_Print(( "Dd_IPCU_Clr_MB_Dst() - Too many setting bits. : ID = %d, Dst = %lu\n", id, dst ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		/* Check Sourece register */
		if( IO_IPCU0.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst() - Request transmit source is not set. : ID = %d, Dst = %lu\n", id, dst ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Destination Status register */
		if( dst != ( IO_IPCU0.MAILBOX[mb_idx].DSTSTAT.word & dst )) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst() - Request for designated ch is nothing. : ID = %d, Dst = %lu\n", id, dst ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU0.MAILBOX[mb_idx].DSTCLR.word = dst;
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		/* Check Sourece register */
		if( IO_IPCU1.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst() - Request transmit source is not set. : ID = %d, Dst = %lu\n", id, dst ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Destination Status register */
		if( dst != ( IO_IPCU1.MAILBOX[mb_idx].DSTSTAT.word & dst )) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst() - Request for designated ch is nothing. : ID = %d, Dst = %lu\n", id, dst ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU1.MAILBOX[mb_idx].DSTCLR.word = dst;
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/**
 * @brief	Clear Destination information indicated by IPCU ch
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG dst_ch 	Request transmit destination ch(0-15)
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Clr_MB_Dst_Unit( UCHAR id, ULONG dst_ch )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Clr_MB_Dst_Unit() - Mailbox number exceeds the maximum. : ID = %d Ch = %lu\n", id, dst_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( dst_ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Clr_MB_Dst_Unit() - INT Ch No. exceeds the maximum. : ID = %d Ch = %lu\n", id, dst_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		/* Check Sourece register */
		if( IO_IPCU0.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst() - Request transmit source is not set. : ID = %d Ch = %lu\n", id, dst_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Destination Status register */
		if(( 1 & (IO_IPCU0.MAILBOX[mb_idx].DSTSTAT.word >> dst_ch)) == 0 ) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst() - Request for designated ch is nothing. : ID = %d Ch = %lu\n", id, dst_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		IO_IPCU0.MAILBOX[mb_idx].DSTCLR.word = (1U << (dst_ch));
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		/* Check Sourece register */
		if( IO_IPCU1.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst() - Request transmit source is not set. : ID = %d Ch = %lu\n", id, dst_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Destination Status register */
		if(( 1 & (IO_IPCU1.MAILBOX[mb_idx].DSTSTAT.word >> dst_ch)) == 0 ) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst() - Request for designated ch is nothing. : ID = %d Ch = %lu\n", id, dst_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		IO_IPCU1.MAILBOX[mb_idx].DSTCLR.word = (1U << (dst_ch));
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/*##################################
  ##  Destination Status Register ##
  ##################################*/
/**
 * @brief	Get Destination information
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG *dst 	Request transmit destination
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Dst_Stat( UCHAR id, ULONG *dst )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Dst_Stat() - Mailbox number exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( dst == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Dst_Stat() - Dst address is NULL. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		*dst = IO_IPCU0.MAILBOX[mb_idx].DSTSTAT.word;
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		*dst = IO_IPCU1.MAILBOX[mb_idx].DSTSTAT.word;
	}

	return D_DDIM_OK;
}

/**
 * @brief	Get Destination information indicated by IPCU ch
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG ch	Interrupt ch of IPCU(0-15)
 * @param	ULONG *dst 	Request transmit destination
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Dst_Stat_Unit( UCHAR id, ULONG dst_ch, ULONG *dst )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Dst_Stat_Unit() - Mailbox number exceeds the maximum. : ID = %d Ch = %lu\n", id, dst_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( dst_ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Dst_Stat_Unit() - INT Ch No. exceeds the maximum. : ID = %d Ch = %lu\n", id, dst_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( dst == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Dst_Stat_Unit() - Dst address is NULL. : ID = %d Ch = %lu\n", id, dst_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		*dst = (1 & (IO_IPCU0.MAILBOX[mb_idx].DSTSTAT.word >> ( dst_ch )));
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		*dst = (1 & (IO_IPCU1.MAILBOX[mb_idx].DSTSTAT.word >> ( dst_ch )));
	}
	
	return D_DDIM_OK;
}

/*##################################
  ##       Mask Set Register      ##
  ##################################*/
/**
 * @brief	Set Mask request
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG msk 	Mask request
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Set_MB_Msk( UCHAR id, ULONG msk )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_MB_Msk() - Mailbox number exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		/* Check Sourece register */
		if( IO_IPCU0.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Msk() - Request transmit source is not set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU0.MAILBOX[mb_idx].SEND.bit.SEND != 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Msk() - Send trigger is set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU0.MAILBOX[mb_idx].MASKSET.word = msk;
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		/* Check Sourece register */
		if( IO_IPCU1.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Msk() - Request transmit source is not set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU1.MAILBOX[mb_idx].SEND.bit.SEND != 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Msk() - Send trigger is set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU1.MAILBOX[mb_idx].MASKSET.word = msk;
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Mask request indicated by IPCU ch
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG msk_ch 	Request mask ch(0-15)
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Set_MB_Msk_Unit( UCHAR id, ULONG msk_ch )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_MB_Msk_Unit() - Mailbox number exceeds the maximum. : ID = %d Ch = %lu\n", id, msk_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( msk_ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_MB_Msk_Unit() - INT Ch No. exceeds the maximum. : ID = %d Ch = %lu\n", id, msk_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		/* Check Sourece register */
		if( IO_IPCU0.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Msk_Unit() - Request transmit source is not set. : ID = %d Ch = %lu\n", id, msk_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU0.MAILBOX[mb_idx].SEND.bit.SEND != 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Msk_Unit() - Send trigger is set. : ID = %d Ch = %lu\n", id, msk_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU0.MAILBOX[mb_idx].MASKSET.word = (1U << (msk_ch));
	}
	else {						/* unit 2 */
		mb_idx = id - 8;
		/* Check Sourece register */
		if( IO_IPCU1.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Msk_Unit() - Request transmit source is not set. : ID = %d Ch = %lu\n", id, msk_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU1.MAILBOX[mb_idx].SEND.bit.SEND != 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Msk_Unit() - Send trigger is set. : ID = %d Ch = %lu\n", id, msk_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}

		IO_IPCU1.MAILBOX[mb_idx].MASKSET.word = (1U << (msk_ch));
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/*##################################
  ##      Mask Clear Register     ##
  ##################################*/
/**
 * @brief	Clear Mask request indicated by IPCU ch
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG msk 	Mask request by bit data
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Clr_MB_Msk( UCHAR id, ULONG msk )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Clr_MB_Dst() - Mailbox number exceeds the maximum. : ID = %d, Msk = %lu\n", id, msk ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		/* Check Sourece register */
		if( IO_IPCU0.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst() - Request transmit source is not set. : ID = %d, Msk = %lu\n", id, msk ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU0.MAILBOX[mb_idx].MASKCLR.word = msk;
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		/* Check Sourece register */
		if( IO_IPCU1.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Dst() - Request transmit source is not set. : ID = %d, Msk = %lu\n", id, msk ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU1.MAILBOX[mb_idx].MASKCLR.word = msk;
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/**
 * @brief	Clear Mask request
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG msk_ch 	Request mask ch(0-15)
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Clr_MB_Msk_Unit( UCHAR id, ULONG msk_ch )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Clr_MB_Msk_Unit() - Mailbox number exceeds the maximum. : ID = %d Ch = %lu\n", id, msk_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( msk_ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Clr_MB_Msk_Unit() - INT Ch No. exceeds the maximum. : ID = %d Ch = %lu\n", id, msk_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		/* Check Sourece register */
		if( IO_IPCU0.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Clr_MB_Msk_Unit() - Request transmit source is not set. : ID = %d Ch = %lu\n", id, msk_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU0.MAILBOX[mb_idx].MASKCLR.word = (1U << (msk_ch));
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		/* Check Sourece register */
		if( IO_IPCU1.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Clr_MB_Msk_Unit() - Request transmit source is not set. : ID = %d Ch = %lu\n", id, msk_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU1.MAILBOX[mb_idx].MASKCLR.word = (1U << (msk_ch));
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/*##################################
  ##     Mask Status Register     ##
  ##################################*/
/**
 * @brief	Get Mask request status
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG *msk 	Mask request
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Msk_Stat( UCHAR id, ULONG *msk )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Msk_Stat() - Mailbox number exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( msk == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Msk_Stat() - Msk address is NULL. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		*msk = IO_IPCU0.MAILBOX[mb_idx].MASKSTAT.word;
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		*msk = IO_IPCU1.MAILBOX[mb_idx].MASKSTAT.word;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Mask request indicated by IPCU ch
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	UCHAR msk_ch	Interrupt ch of IPCU(0-15)
 * @param	ULONG *msk	 	Mask request
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Msk_Stat_Unit( UCHAR id, ULONG msk_ch, ULONG *msk )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Src_Unit() - Mailbox number exceeds the maximum. : ID = %d, Ch = %lu\n", id, msk_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( msk_ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Src_Unit() - INT Ch No. exceeds the maximum. : ID = %d Ch = %lu\n", id, msk_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( msk == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Src_Unit() - Msk address is NULL. : ID = %d, Ch = %lu\n", id, msk_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		*msk = (1 & (IO_IPCU0.MAILBOX[mb_idx].MASKSTAT.word >> ( msk_ch )));
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		*msk = (1 & (IO_IPCU1.MAILBOX[mb_idx].MASKSTAT.word >> ( msk_ch )));
	}
	
	return D_DDIM_OK;
}

/*##################################
  ##    Acknowledge Set Register  ##
  ##################################*/
/**
 * @brief	Set Acknowledge
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG ack 	Acknowledge
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Set_MB_Ack( UCHAR id, ULONG ack )
{
	UCHAR loop_cnt;
	UCHAR bit_cnt = 0;
	UCHAR unit;
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_MB_Ack() - Unit No. exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	/* Get unit No. */
	if( id < 8 ) {
		mb_idx = id;
		unit = 0;
	}
	else if( id < 16 ) {
		mb_idx = id - 8;
		unit = 1;
	}
	else {
		mb_idx = id - 16;
		unit = 2;
	}
	
#ifdef CO_PARAM_CHECK
	if( unit == 0 ){
		if( IO_IPCU0.MAILBOX[mb_idx].MODE.bit.MODE == E_DD_IPCU_MB_MODE_MANUAL_2 ) {
			/* Check too many setting */
			for( loop_cnt = 0; loop_cnt <= D_DD_IPCU_INT_MAX; loop_cnt++ ) {
				if( 1 & ( ack >> loop_cnt )) {
					bit_cnt++;
				}
			}
			if( bit_cnt > 1 ) {
				Ddim_Print(( "Dd_IPCU_Set_MB_Ack() - Too many setting bits. : ID = %d, Ack = %lu\n", id, ack ));
				return D_DD_IPCU_INPUT_PARAM_ERR;
			}
		}
	}
	else {
		if( IO_IPCU1.MAILBOX[mb_idx].MODE.bit.MODE == E_DD_IPCU_MB_MODE_MANUAL_2 ) {
			/* Check too many setting */
			for( loop_cnt = 0; loop_cnt <= D_DD_IPCU_INT_MAX; loop_cnt++ ) {
				if( 1 & ( ack >> loop_cnt )) {
					bit_cnt++;
				}
			}
			if( bit_cnt > 1 ) {
				Ddim_Print(( "Dd_IPCU_Set_MB_Ack() - Too many setting bits. : ID = %d, Ack = %lu\n", id, ack ));
				return D_DD_IPCU_INPUT_PARAM_ERR;
			}
		}
	}
#endif // CO_PARAM_CHECK
	
	if( unit == 0 ){
		/* Check Sourece register */
		if( IO_IPCU0.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Ack() - Request transmit source is not set. : ID = %d, Ack = %lu\n", id, ack ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU0.MAILBOX[mb_idx].SEND.bit.SEND == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Ack() - Send trigger is not set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check mode type */
		if( IO_IPCU0.MAILBOX[mb_idx].MODE.bit.MODE > E_DD_IPCU_MB_MODE_MANUAL_2 ) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Ack() - Mode is not correct. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check status with mode 2 */
		if( IO_IPCU0.MAILBOX[mb_idx].MODE.bit.MODE == E_DD_IPCU_MB_MODE_MANUAL_2 ) {
			if(( IO_IPCU0.MAILBOX[mb_idx].ACKSET.word & ack ) != 0 ) {
				Ddim_Print(( "Dd_IPCU_Set_MB_Ack() - ACK is sending. : ID = %d\n", id ));
				return D_DD_IPCU_INPUT_PARAM_ERR;
			}
		}
		
		IO_IPCU0.MAILBOX[mb_idx].ACKSET.word = ack;
	}
	else {
		/* Check Sourece register */
		if( IO_IPCU1.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Ack() - Request transmit source is not set. : ID = %d, Ack = %lu\n", id, ack ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU1.MAILBOX[mb_idx].SEND.bit.SEND == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Ack() - Send trigger is not set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check mode type */
		if( IO_IPCU1.MAILBOX[mb_idx].MODE.bit.MODE > E_DD_IPCU_MB_MODE_MANUAL_2 ) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Ack() - Mode is not correct. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		/* Check status with mode 2 */
		if( IO_IPCU1.MAILBOX[mb_idx].MODE.bit.MODE == E_DD_IPCU_MB_MODE_MANUAL_2 ) {
			if(( IO_IPCU1.MAILBOX[mb_idx].ACKSET.word & ack ) != 0 ) {
				Ddim_Print(( "Dd_IPCU_Set_MB_Ack() - ACK is sending. : ID = %d\n", id ));
				return D_DD_IPCU_INPUT_PARAM_ERR;
			}
		}
		
		IO_IPCU1.MAILBOX[mb_idx].ACKSET.word = ack;
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Acknowledge indicated by IPCU ch
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG ack_ch 	Acknowledge ch(0-15)
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Set_MB_Ack_Unit( UCHAR id, ULONG ack_ch )
{
	UCHAR unit;
	UCHAR mb_idx;
	
#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_MB_Ack_Unit() - Mailbox number exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	/* Get unit No. */
	if( id < 8 ) {
		mb_idx = id;
		unit = 0;
	}
	else if( id < 16 ) {
		mb_idx = id - 8;
		unit = 1;
	}
	else {
		mb_idx = id - 16;
		unit = 2;
	}
	
	if( ack_ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_MB_Ack_Unit() - INT Ch No. exceeds the maximum. : ID = %d Ch = %lu\n", id, ack_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	
#endif // CO_PARAM_CHECK
	
	if( unit == 0 ){
		/* Check Sourece register */
		if( IO_IPCU0.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Ack_Unit() - Request transmit source is not set. : ID = %d, Ch = %lu\n", id, ack_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU0.MAILBOX[mb_idx].SEND.bit.SEND == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Ack_Unit() - Send trigger is not set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check mode type */
		if( IO_IPCU0.MAILBOX[mb_idx].MODE.bit.MODE > E_DD_IPCU_MB_MODE_MANUAL_2 ) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Ack_Unit() - Mode is not correct. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check status with mode 2 */
		if( IO_IPCU0.MAILBOX[mb_idx].MODE.bit.MODE == E_DD_IPCU_MB_MODE_MANUAL_2 ) {
			if( 1 & (IO_IPCU0.MAILBOX[mb_idx].ACKSET.word >> ( ack_ch ))) {
				Ddim_Print(( "Dd_IPCU_Set_MB_Ack_Unit() - ACK is sending. : ID = %d Ch = %lu\n", id, ack_ch ));
				return D_DD_IPCU_INPUT_PARAM_ERR;
			}
		}
		
		IO_IPCU0.MAILBOX[mb_idx].ACKSET.word = (1U << (ack_ch));
	}
	else {
		/* Check Sourece register */
		if( IO_IPCU1.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Ack_Unit() - Request transmit source is not set. : ID = %d, Ack = %lu\n", id, ack_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU1.MAILBOX[mb_idx].SEND.bit.SEND == 0) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Ack_Unit() - Send trigger is not set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check mode type */
		if( IO_IPCU1.MAILBOX[mb_idx].MODE.bit.MODE > E_DD_IPCU_MB_MODE_MANUAL_2 ) {
			Ddim_Print(( "Dd_IPCU_Set_MB_Ack_Unit() - Mode is not correct. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check status with mode 2 */
		if( IO_IPCU1.MAILBOX[mb_idx].MODE.bit.MODE == E_DD_IPCU_MB_MODE_MANUAL_2 ) {
			if( 1 & (IO_IPCU1.MAILBOX[mb_idx].ACKSET.word >> ( ack_ch ))) {
				Ddim_Print(( "Dd_IPCU_Set_MB_Ack_Unit() - ACK is sending. : ID = %d Ch = %lu\n", id, ack_ch ));
				return D_DD_IPCU_INPUT_PARAM_ERR;
			}
		}
		
		IO_IPCU1.MAILBOX[mb_idx].ACKSET.word = (1U << (ack_ch));
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;

}

/*##################################
  ##  Acknowledge Clear Register  ##
  ##################################*/
/**
 * @brief	Clear Acknowledge indicated by IPCU ch
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG msk 	Acknowledge by bit data
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Clr_MB_Ack( UCHAR id, ULONG ack )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Clr_MB_Ack() - Mailbox number exceeds the maximum. : ID = %d, Ack = %lu\n", id, ack ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		/* Check Sourece register */
		if( IO_IPCU0.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Clr_MB_Ack() - Request transmit source is not set. : ID = %d, Clr = %lu\n", id, ack ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU0.MAILBOX[mb_idx].SEND.bit.SEND == 0) {
			Ddim_Print(( "Dd_IPCU_Clr_MB_Ack() - Send trigger is not set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check mode type */
		if( IO_IPCU0.MAILBOX[mb_idx].MODE.bit.MODE == E_DD_IPCU_MB_MODE_AUTO_CLEAR ) {
			Ddim_Print(( "Dd_IPCU_Clr_MB_Ack() - Mode is Auto clear mode. : ID = %d, Mode = %d\n", id, E_DD_IPCU_MB_MODE_AUTO_CLEAR ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU0.MAILBOX[mb_idx].ACKCLR.word = ack;
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		/* Check Sourece register */
		if( IO_IPCU1.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Clr_MB_Ack() - Request transmit source is not set. : ID = %d, Clr = %lu\n", id, ack ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU1.MAILBOX[mb_idx].SEND.bit.SEND == 0) {
			Ddim_Print(( "Dd_IPCU_Clr_MB_Ack() - Send trigger is not set. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check mode type */
		if( IO_IPCU1.MAILBOX[mb_idx].MODE.bit.MODE == E_DD_IPCU_MB_MODE_AUTO_CLEAR ) {
			Ddim_Print(( "Dd_IPCU_Clr_MB_Ack() - Mode is Auto clear mode. : ID = %d, Mode = %d\n", id, E_DD_IPCU_MB_MODE_AUTO_CLEAR ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU1.MAILBOX[mb_idx].ACKCLR.word = ack;
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/**
 * @brief	Clear Acknowledge indicated by IPCU ch
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG ack_ch 	Acknowledge by bit data
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Clr_MB_Ack_Unit( UCHAR id, ULONG ack_ch )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Clr_MB_Ack() - Mailbox number exceeds the maximum. : ID = %d, Ch = %lu\n", id, ack_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ack_ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Clr_MB_Ack() - INT Ch No. exceeds the maximum. : ID = %d Ch = %lu\n", id, ack_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		/* Check Sourece register */
		if( IO_IPCU0.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Clr_MB_Ack() - Request transmit source is not set. : ID = %d, Ch = %lu\n", id, ack_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU0.MAILBOX[mb_idx].SEND.bit.SEND == 0) {
			Ddim_Print(( "Dd_IPCU_Clr_MB_Ack() - Send trigger is not set. : ID = %d, Ch = %lu\n", id, ack_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check mode type */
		if( IO_IPCU0.MAILBOX[mb_idx].MODE.bit.MODE == E_DD_IPCU_MB_MODE_AUTO_CLEAR ) {
			Ddim_Print(( "Dd_IPCU_Clr_MB_Ack() - Mode is Auto clear mode. : ID = %d, Ch = %lu\n", id, ack_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU0.MAILBOX[mb_idx].ACKCLR.word = (1U << (ack_ch));
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		/* Check Sourece register */
		if( IO_IPCU1.MAILBOX[mb_idx].SRC.word == 0) {
			Ddim_Print(( "Dd_IPCU_Clr_MB_Ack() - Request transmit source is not set. : ID = %d, Ch = %lu\n", id, ack_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check Send register */
		if( IO_IPCU1.MAILBOX[mb_idx].SEND.bit.SEND == 0) {
			Ddim_Print(( "Dd_IPCU_Clr_MB_Ack() - Send trigger is not set. : ID = %d, Ch = %lu\n", id, ack_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		/* Check mode type */
		if( IO_IPCU1.MAILBOX[mb_idx].MODE.bit.MODE == E_DD_IPCU_MB_MODE_AUTO_CLEAR ) {
			Ddim_Print(( "Dd_IPCU_Clr_MB_Ack() - Mode is Auto clear mode. : ID = %d, Ch = %lu\n", id, ack_ch ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		
		IO_IPCU1.MAILBOX[mb_idx].ACKCLR.word = (1U << (ack_ch));
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/*##################################
  ##  Acknowledge Status Register ##
  ##################################*/
/**
 * @brief	Get Acknowledge status
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG *ack 	Acknowledge status
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Ack_Stat( UCHAR id, ULONG *ack )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Stat() - Mailbox number exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ack == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Stat() - Ack address is NULL. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		/* Check mode type */
		if( IO_IPCU0.MAILBOX[mb_idx].MODE.bit.MODE == E_DD_IPCU_MB_MODE_AUTO_CLEAR ) {
			Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Stat() - Mode is not correct. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		*ack = IO_IPCU0.MAILBOX[mb_idx].ACKSTAT.word;
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		/* Check mode type */
		if( IO_IPCU1.MAILBOX[mb_idx].MODE.bit.MODE == E_DD_IPCU_MB_MODE_AUTO_CLEAR ) {
			Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Stat() - Mode is not correct. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		*ack = IO_IPCU1.MAILBOX[mb_idx].ACKSTAT.word;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Acknowledge indicated by IPCU ch
 * @param	UCHAR id		IPCU ID(0 to 17)
 * @param	UCHAR ack_ch	Interrupt ch of IPCU(0-15)
 * @param	ULONG *ack	 	Acknowledge
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Ack_Stat_Unit( UCHAR id, ULONG ack_ch, ULONG *ack )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Stat_Unit() - Mailbox number exceeds the maximum. : ID = %d, Ch = %lu\n", id, ack_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ack_ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Stat_Unit() - INT Ch No. exceeds the maximum. : ID = %d Ch = %lu\n", id, ack_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ack == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Stat_Unit() - Ack address is NULL. : ID = %d, Ch = %lu\n", id, ack_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		/* Check mode type */
		if( IO_IPCU0.MAILBOX[mb_idx].MODE.bit.MODE == E_DD_IPCU_MB_MODE_AUTO_CLEAR ) {
			Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Stat_Unit() - Mode is not correct. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		*ack = (1 & (IO_IPCU0.MAILBOX[mb_idx].ACKSTAT.word >> ( ack_ch )));
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		/* Check mode type */
		if( IO_IPCU1.MAILBOX[mb_idx].MODE.bit.MODE == E_DD_IPCU_MB_MODE_AUTO_CLEAR ) {
			Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Stat_Unit() - Mode is not correct. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		*ack = (1 & (IO_IPCU1.MAILBOX[mb_idx].ACKSTAT.word >> ( ack_ch )));
	}
	
	return D_DDIM_OK;
}

/*##################################
  ##  Acknowledge Source Register ##
  ##################################*/
/**
 * @brief	Get Acknowledge source information
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	ULONG *ack 	Acknowledge source
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Ack_Src( UCHAR id, ULONG *ack )
{
	ULONG mode;
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Src() - Mailbox number exceeds the maximum. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ack == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Src() - Ack address is NULL. : ID = %d\n", id ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		/* Check mode type */
		mode = IO_IPCU0.MAILBOX[mb_idx].MODE.bit.MODE;
		if(( mode != E_DD_IPCU_MB_MODE_MANUAL_2 ) && ( mode != E_DD_IPCU_MB_MODE_AUTO_ACK_2 )) {
			Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Src() - Mode is not correct. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		*ack = IO_IPCU0.MAILBOX[mb_idx].ACKSRC.word;
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		/* Check mode type */
		mode = IO_IPCU1.MAILBOX[mb_idx].MODE.bit.MODE;
		if(( mode != E_DD_IPCU_MB_MODE_MANUAL_2 ) && ( mode != E_DD_IPCU_MB_MODE_AUTO_ACK_2 )) {
			Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Src() - Mode is not correct. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		*ack = IO_IPCU1.MAILBOX[mb_idx].ACKSRC.word;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Source information indicated by IPCU ch
 * @param	UCHAR id		IPCU ID(0 to 17)
 * @param	UCHAR ack_ch	Interrupt ch of IPCU(0-15)
 * @param	ULONG *ack  	Acknowledge source
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Ack_Src_Unit( UCHAR id, ULONG ack_ch, ULONG *ack )
{
	ULONG mode;
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_MB_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Src_Unit() - Mailbox number exceeds the maximum. : ID = %d, Ch = %lu\n", id, ack_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ack_ch > D_DD_IPCU_INT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Src_Unit() - INT Ch No. exceeds the maximum. : ID = %d Ch = %lu\n", id, ack_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( ack == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Src_Unit() - Mailbox address is NULL. : ID = %d, Ch = %lu\n", id, ack_ch ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		/* Check mode type */
		mode = IO_IPCU0.MAILBOX[mb_idx].MODE.bit.MODE;
		if(( mode != E_DD_IPCU_MB_MODE_MANUAL_2 ) && ( mode != E_DD_IPCU_MB_MODE_AUTO_ACK_2 )) {
			Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Src_Unit() - Mode is not correct. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		*ack = (1 & (IO_IPCU0.MAILBOX[mb_idx].ACKSRC.word >> ( ack_ch )));
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		/* Check mode type */
		mode = IO_IPCU1.MAILBOX[mb_idx].MODE.bit.MODE;
		if(( mode != E_DD_IPCU_MB_MODE_MANUAL_2 ) && ( mode != E_DD_IPCU_MB_MODE_AUTO_ACK_2 )) {
			Ddim_Print(( "Dd_IPCU_Get_MB_Ack_Src_Unit() - Mode is not correct. : ID = %d\n", id ));
			return D_DD_IPCU_STATUS_ABNORMAL;
		}
		*ack = (1 & (IO_IPCU1.MAILBOX[mb_idx].ACKSRC.word >> ( ack_ch )));
	}
	
	return D_DDIM_OK;
}

/*##################################
  ##         Data Register        ##
  ##################################*/
/**
 * @brief	Get Transfer data
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	UCHAR dat_num	Transfer data No.
 * @param	ULONG *data 	Transfer data
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Data( UCHAR id, UCHAR dat_num, ULONG *data )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Data() - Mailbox number exceeds the maximum. : ID = %d, Data No = %d\n", id, dat_num ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( dat_num > D_DD_IPCU_MB_DATA_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Data() - Transfer data No. exceeds the maximum. : ID = %d, Data No = %d\n", id, dat_num ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( data == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Data() - Data address is NULL. : ID = %d, Data No = %d\n", id, dat_num ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		*data = IO_IPCU0.MAILBOX[mb_idx].DATA[dat_num];
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		*data = IO_IPCU1.MAILBOX[mb_idx].DATA[dat_num];
	}
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Transfer data
 * @param	UCHAR id	IPCU ID(0 to 17)
 * @param	UCHAR dat_num	Transfer data No.
 * @param	ULONG data 		Transfer data
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Set_MB_Data( UCHAR id, UCHAR dat_num, ULONG data )
{
	UCHAR mb_idx;

#ifdef CO_PARAM_CHECK
	if( id > D_DD_IPCU_ID_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_MB_Data() - Mailbox number exceeds the maximum. : ID = %d, Data No = %d\n", id, dat_num ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( dat_num > D_DD_IPCU_MB_DATA_MAX ){
		Ddim_Print(( "Dd_IPCU_Set_MB_Data() - Transfer data No. exceeds the maximum. : ID = %d, Data No = %d\n", id, dat_num ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	/* Get unit No. */
	if( id < 8 ) {				/* unit 0 */
		mb_idx = id;
		IO_IPCU0.MAILBOX[mb_idx].DATA[dat_num] = data;
	}
	else {						/* unit 1 */
		mb_idx = id - 8;
		IO_IPCU1.MAILBOX[mb_idx].DATA[dat_num] = data;
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/*##################################
  ##    MailBox Status Register   ##
  ##################################*/
/**
 * @brief	Get MailBox status
 * @param	UCHAR unit		IPCU Unit(0-1)
 * @param	ULONG *mb_stat 	MailBox status
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Stat( UCHAR unit, ULONG *mb_stat )
{
#ifdef CO_PARAM_CHECK
	if( unit > D_DD_IPCU_UNIT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Stat() - Unit No. exceeds the maximum. : Unit = %d\n", unit ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( mb_stat == NULL ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Stat() - MB status address is NULL. : Unit = %d\n", unit ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	if( unit == 0 ){
		*mb_stat = IO_IPCU0.MBSTAT.word;
	}
	else {
		*mb_stat = IO_IPCU1.MBSTAT.word;
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/**
 * @brief	Get MailBox status
 * @param	UCHAR unit		IPCU Unit(0-1)
 * @param	ULONG num		Mailbox number(0-7)
 * @param	ULONG *mb_stat 	MailBox status
 * @return  D_DDIM_OK / D_DD_IPCU_INPUT_PARAM_ERR
 */
INT32 Dd_IPCU_Get_MB_Stat_MB( UCHAR unit, ULONG num, ULONG *mb_stat )
{
#ifdef CO_PARAM_CHECK
	if( unit > D_DD_IPCU_UNIT_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Stat_MB() - Unit No. exceeds the maximum. : Unit = %d, Num = %lu\n", unit, num ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
	if( num > D_DD_IPCU_MB_MAX ){
		Ddim_Print(( "Dd_IPCU_Get_MB_Stat_MB() - Mailbox number exceeds the maximum. : Unit = %d, Num = %lu\n", unit, num ));
		return D_DD_IPCU_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK
	
	if( unit == 0 ){
		*mb_stat = ( 1 & (IO_IPCU0.MBSTAT.word >> ( num )));
	}
	else {
		*mb_stat = ( 1 & (IO_IPCU1.MBSTAT.word >> ( num )));
	}
	
	return D_DDIM_OK;
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special

#endif //CO_DDIM_UTILITY_USE

