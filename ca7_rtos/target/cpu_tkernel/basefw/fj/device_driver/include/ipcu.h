/**
 * @file		ipcu.h
 * @brief		IPCU (Inter-Processor Communications Unit) process api header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IPCU_H_
#define _IPCU_H_

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define IPCU_SEND_BUFFER_SIZE		(32 * 1024)						// 32kByte size per resource
#define IPCU_ID_MAX					(16)							// Total number of IPCU ID

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Command code for CM0 */
typedef enum {
	E_BF_IPCU_CMD_CM0_GO = 0x01,	/* Go to sleep state. No parameter */
	E_BF_IPCU_CMD_CM0_WRITE,		/* Write to memory and memory-mapped register by a data */
	E_BF_IPCU_CMD_CM0_READ,			/* Read a data from memory and memory-mapped register */
	E_BF_IPCU_CMD_CM0_RMW,			/* Read-modify-write a data from/to memory and memory-mapped register */
	E_BF_IPCU_CMD_CM0_SET,			/* Set parameter */
	E_BF_IPCU_CMD_CM0_GET,			/* Get parameter */
	E_BF_IPCU_CMD_CM0_START,		/* Function start */
	E_BF_IPCU_CMD_CM0_STOP,			/* Function stop */
// --- REMOVE_SECURE BEGIN ---
#if 1
	E_BF_IPCU_CMD_CM0_CRYPTO,		/* Check Secure Code */
#else
// --- REMOVE_SECURE END ---
	E_BF_IPCU_CMD_CM0_DUMMY,		/* No function */
// --- REMOVE_SECURE BEGIN ---
#endif
// --- REMOVE_SECURE END ---
	E_BF_IPCU_CMD_CM0_STATE_AFTER_STANDBY,	/* State ater standby state */
	E_BF_IPCU_CMD_CM0_DELAY,				/* Time delay by RTOS */
	E_BF_IPCU_CMD_CM0_WAIT_LOOP,			/* Time wait by loop program */
	E_BF_IPCU_CMD_CM0_END,					/* End of sequence */
	E_BF_IPCU_CMD_CM0_MAX
} E_BF_IPCU_CMD_CM0;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern UINT8		bf_ipcu_buffer[IPCU_ID_MAX][IPCU_SEND_BUFFER_SIZE];
extern UINT32		bf_ipcu_sync[IPCU_ID_MAX];

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

FJ_ERR_CODE BF_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE ipctype, UCHAR *id);
FJ_ERR_CODE BF_IPCU_Close(UCHAR id);
FJ_ERR_CODE BF_IPCU_SetReqCB(UCHAR id, VOID (*req_fn_ptr)(UCHAR,UCHAR*,UINT32,UINT32,UINT32));
VOID BF_IPCU_SetResCB( UCHAR id, VOID (*res_fn_ptr)(UCHAR, UCHAR*, UINT32) );
FJ_ERR_CODE BF_IPCU_Send(UCHAR id, UCHAR *data, UINT32 length, UCHAR sync );
FJ_ERR_CODE BF_IPCU_DISP_Send(UCHAR id, UCHAR *data, UINT32 length, UCHAR sync );
FJ_ERR_CODE BF_IPCU_Send_CM0(UCHAR id, E_BF_IPCU_CMD_CM0 cmd, ULONG *data );
VOID BF_IPCU_Int_Handler_Req( UINT32 int_ch_num );
VOID BF_IPCU_Int_Handler_Req_CM0( UINT32 int_ch_num );
VOID BF_IPCU_Int_Handler_Ack( UINT32 int_ch_num );
VOID BF_IPCU_Int_Handler_Ack_CM0( UINT32 int_ch_num );
VOID BF_IPCU_Get_Response_Commnad_CM0( ULONG* resp_command );
VOID BF_IPCU_Set_Sync_CM0( VOID );

FJ_ERR_CODE fj_ipcu_open(E_FJ_IPCU_MAILBOX_TYPE ipctype, UCHAR *id);
FJ_ERR_CODE fj_ipcu_close(UCHAR id);
FJ_ERR_CODE fj_ipcu_setreceivercb(UCHAR id, VOID (*req_fn_ptr)(UCHAR,UINT32,UINT32,UINT32,UINT32));
FJ_ERR_CODE fj_ipcu_send(UCHAR id, UINT32 pdata, UINT32 length, UCHAR sync);
FJ_ERR_CODE fj_ipcu_setstateafterstandby_cm0( const E_FJ_IPCU_STATE_AFTER_STANDY next_state, const UINT32 wait_time, const UINT32 mode );
FJ_ERR_CODE fj_ipcu_sleep_cm0( VOID );
FJ_ERR_CODE fj_ipcu_write_cm0( ULONG type, ULONG address, ULONG write_data );
FJ_ERR_CODE fj_ipcu_read_cm0( ULONG type, ULONG address, ULONG *read_data );
FJ_ERR_CODE fj_ipcu_read_modify_write_cm0( ULONG type, ULONG address, ULONG data, ULONG mask );
FJ_ERR_CODE fj_ipcu_set_tempera_buff_cm0( T_FJ_ARM_IPCU_TEMPERA tempera );
FJ_ERR_CODE fj_ipcu_set_send_buff_cm0( E_FJ_IPCU_BUFF type, ULONG address, ULONG size );
FJ_ERR_CODE fj_ipcu_set_receive_buff_cm0( E_FJ_IPCU_BUFF type, ULONG address, ULONG size, ULONG wpt, ULONG overlap );
FJ_ERR_CODE fj_ipcu_set_i2c_send_buff_cm0( ULONG address, ULONG size, ULONG slave_address );
FJ_ERR_CODE fj_ipcu_set_i2c_receive_buff_cm0( ULONG address, ULONG size, ULONG wpt, ULONG overlap, ULONG slave_address );
FJ_ERR_CODE fj_ipcu_set_notify_cm0( E_FJ_IPCU_NOTIFY func, ULONG enable );
FJ_ERR_CODE fj_ipcu_set_condition_cm0( ULONG enable );
FJ_ERR_CODE fj_ipcu_set_calendar_cm0( T_FJ_ARM_IPCU_CALENDAR calendar );
FJ_ERR_CODE fj_ipcu_set_advfs_cm0( T_FJ_ARM_IPCU_ADVFS advfs );
FJ_ERR_CODE fj_ipcu_set_advfs_en_cm0( ULONG enable );
FJ_ERR_CODE fj_ipcu_set_PLL1_Frq_buf_cm0( ULONG frq_h, ULONG frq_l );
FJ_ERR_CODE fj_ipcu_get_temperature_cm0( ULONG *temperature );
FJ_ERR_CODE fj_ipcu_get_calendar_cm0( T_FJ_ARM_IPCU_CALENDAR *calendar );
FJ_ERR_CODE fj_ipcu_get_advfs_cm0( T_FJ_ARM_IPCU_ADVFS *advfs );
FJ_ERR_CODE fj_ipcu_get_advfs_en_cm0( ULONG *enable );
FJ_ERR_CODE fj_ipcu_get_PLL1_Frq_buf_cm0( ULONG *frq_h, ULONG *frq_l );
FJ_ERR_CODE fj_ipcu_get_Tempera_buf_cm0( T_FJ_ARM_IPCU_TEMPERA *tempera );
FJ_ERR_CODE fj_ipcu_get_cm0( ULONG id, ULONG *get_data );
FJ_ERR_CODE fj_ipcu_start_cm0( E_FJ_IPCU_START func );
FJ_ERR_CODE fj_ipcu_stop_cm0( E_FJ_IPCU_STOP func );
FJ_ERR_CODE fj_ipcu_set_rtc_time_cm0( T_FJ_ARM_IPCU_CALENDAR calendar );
// --- REMOVE_SECURE BEGIN ---
FJ_ERR_CODE fj_ipcu_check_secure_code_cm0( ULONG digest_addr, ULONG address, ULONG count, ULONG *result );
FJ_ERR_CODE fj_ipcu_read_e_fuse_cm0( ULONG address, ULONG offset, ULONG size, ULONG *result );
// --- REMOVE_SECURE END ---

// --- REMOVE_CAMERAIF BEGIN ---
VOID 		fj_ipcu_set_finish_boot(ULONG boot_status);
ULONG 		fj_ipcu_get_finish_boot(VOID);
// --- REMOVE_CAMERAIF END ---

VOID fj_spin_lock( VOID );
VOID fj_spin_unlock( VOID );

#ifdef __cplusplus
}
#endif

#endif	// _IPCU_H_
