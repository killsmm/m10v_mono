/**
 * @file		fj_arm.c
 * @brief		ARM process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_arm.h"
#include "interrupt.h"
#include "ipcu.h"
#include "bf_arm.h"
#include "linux.h"

/*--------------------------------------------------------------*/
/* Definition													*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Global Data													*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Extern  Function												*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Local Function												*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Global Function												*/
/*--------------------------------------------------------------*/
FJ_ERR_CODE FJ_GIC_Ctrl(UINT32 intid, ULONG set_enable, LONG intrpt_pri, E_FJ_GIC_TARGET_CPU cpu_target)
{
	return fj_gic_ctrl(intid, set_enable, intrpt_pri, cpu_target);
}

FJ_ERR_CODE FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE ipctype, UCHAR *id)
{
	return fj_ipcu_open( ipctype, id );
}

FJ_ERR_CODE FJ_IPCU_Close(UCHAR id)
{
	return fj_ipcu_close( id );
}

FJ_ERR_CODE FJ_IPCU_SetReceiverCB(UCHAR id, VOID (*req_fn_ptr)(UCHAR,UINT32,UINT32,UINT32,UINT32))
{
	return fj_ipcu_setreceivercb( id, req_fn_ptr );
}

FJ_ERR_CODE FJ_IPCU_Send(UCHAR id, UINT32 pdata, UINT32 length, UCHAR sync)
{
	return fj_ipcu_send( id, pdata, length, sync );
}

FJ_ERR_CODE FJ_IPCU_SetStateAfterStandby_CM0( const E_FJ_IPCU_STATE_AFTER_STANDY next_state, const UINT32 wait_time, const UINT32 mode )
{
	return fj_ipcu_setstateafterstandby_cm0( next_state, wait_time, mode );
}

FJ_ERR_CODE FJ_IPCU_Sleep_CM0( VOID )
{
	return fj_ipcu_sleep_cm0();
}

FJ_ERR_CODE FJ_IPCU_Write_CM0( ULONG type, ULONG address, ULONG data )
{
	return fj_ipcu_write_cm0( type, address, data );
}

FJ_ERR_CODE FJ_IPCU_Read_CM0( ULONG type, ULONG address, ULONG *read_data )
{
	return fj_ipcu_read_cm0( type, address, read_data );
}

FJ_ERR_CODE FJ_IPCU_Read_Modify_Write_CM0( ULONG type, ULONG address, ULONG data, ULONG mask )
{
	return fj_ipcu_read_modify_write_cm0( type, address, data, mask );
}

FJ_ERR_CODE FJ_IPCU_Set_Tempera_Buff_CM0( T_FJ_ARM_IPCU_TEMPERA tempera )
{
	return fj_ipcu_set_tempera_buff_cm0( tempera );
}

FJ_ERR_CODE FJ_IPCU_Set_Send_Buff_CM0( E_FJ_IPCU_BUFF type, ULONG address, ULONG size )
{
	return fj_ipcu_set_send_buff_cm0( type, address, size );
}

FJ_ERR_CODE FJ_IPCU_Set_Receive_Buff_CM0( E_FJ_IPCU_BUFF type, ULONG address, ULONG size, ULONG wpt, ULONG overlap )
{
	return fj_ipcu_set_receive_buff_cm0( type, address, size, wpt, overlap );
}

FJ_ERR_CODE FJ_IPCU_Set_I2C_Send_Buff_CM0( ULONG address, ULONG size, ULONG slave_address )
{
	return fj_ipcu_set_i2c_send_buff_cm0( address, size, slave_address );
}

FJ_ERR_CODE FJ_IPCU_Set_I2C_Receive_Buff_CM0( ULONG address, ULONG size, ULONG wpt, ULONG overlap, ULONG slave_address )
{
	return fj_ipcu_set_i2c_receive_buff_cm0( address, size, wpt, overlap, slave_address );
}

FJ_ERR_CODE FJ_IPCU_Set_Notify_CM0( E_FJ_IPCU_NOTIFY func, ULONG enable )
{
	return fj_ipcu_set_notify_cm0( func, enable );
}

FJ_ERR_CODE FJ_IPCU_Set_Condition_CM0( ULONG enable )
{
	return fj_ipcu_set_condition_cm0( enable );
}

FJ_ERR_CODE FJ_IPCU_Set_Calendar_CM0( T_FJ_ARM_IPCU_CALENDAR calendar )
{
	return fj_ipcu_set_calendar_cm0( calendar );
}

FJ_ERR_CODE FJ_IPCU_Set_Advfs_CM0( T_FJ_ARM_IPCU_ADVFS advfs )
{
	return fj_ipcu_set_advfs_cm0( advfs );
}
FJ_ERR_CODE FJ_IPCU_Set_Advfs_EN_CM0( ULONG enable )
{
	return fj_ipcu_set_advfs_en_cm0( enable );
}
FJ_ERR_CODE FJ_IPCU_Set_PLL1_Frq_Buf_CM0( ULONG frq_h, ULONG frq_l )
{
	return fj_ipcu_set_PLL1_Frq_buf_cm0( frq_h, frq_l );
}

FJ_ERR_CODE FJ_IPCU_Get_Temperature_CM0( ULONG *temperature )
{
	return fj_ipcu_get_temperature_cm0( temperature );
}

FJ_ERR_CODE FJ_IPCU_Get_Calendar_CM0( T_FJ_ARM_IPCU_CALENDAR *calendar )
{
	return fj_ipcu_get_calendar_cm0( calendar );
}

FJ_ERR_CODE FJ_IPCU_Get_Advfs_CM0( T_FJ_ARM_IPCU_ADVFS *advfs )
{
	return fj_ipcu_get_advfs_cm0( advfs );
}
FJ_ERR_CODE FJ_IPCU_Get_Advfs_EN_CM0( ULONG *enable )
{
	return fj_ipcu_get_advfs_en_cm0( enable );
}
FJ_ERR_CODE FJ_IPCU_Get_PLL1_Frq_Buf_CM0( ULONG *frq_h, ULONG *frq_l )
{
	return fj_ipcu_get_PLL1_Frq_buf_cm0( frq_h, frq_l );
}
FJ_ERR_CODE FJ_IPCU_Get_Tempera_Buf_CM0( T_FJ_ARM_IPCU_TEMPERA *tempera )
{
	return fj_ipcu_get_Tempera_buf_cm0( tempera );
}

FJ_ERR_CODE FJ_IPCU_Get_CM0( ULONG id, ULONG *get_data )
{
	return fj_ipcu_get_cm0( id, get_data );
}	

FJ_ERR_CODE FJ_IPCU_Start_CM0( E_FJ_IPCU_START func )
{
	return fj_ipcu_start_cm0( func );
}

FJ_ERR_CODE FJ_IPCU_Stop_CM0( E_FJ_IPCU_STOP func )
{
	return fj_ipcu_stop_cm0( func );
}

// --- REMOVE_SECURE BEGIN ---
FJ_ERR_CODE FJ_IPCU_Check_Secure_Code_CM0( ULONG digest_addr, ULONG address, ULONG count, ULONG *result )
{
	return fj_ipcu_check_secure_code_cm0( digest_addr, address, count, result );
}

FJ_ERR_CODE FJ_IPCU_Read_E_Fuse_CM0( ULONG address, ULONG offset, ULONG size, ULONG *result )
{
	return fj_ipcu_read_e_fuse_cm0( address, offset, size, result );
}

// --- REMOVE_SECURE END ---

// --- REMOVE_CAMERAIF BEGIN ---
VOID FJ_IPCU_SET_FINISH_BOOT(ULONG boot_status)
{
	fj_ipcu_set_finish_boot( boot_status );
}

ULONG FJ_IPCU_GET_FINISH_BOOT(VOID)
{
	return fj_ipcu_get_finish_boot();
}
// --- REMOVE_CAMERAIF END ---

UINT32 FJ_Critical_Section_Start(UINT32* spin_id)
{
	return fj_critical_section_start(spin_id);
}

UINT32 FJ_Critical_Section_Pol_Start(UINT32* spin_id)
{
	return fj_critical_section_pol_start(spin_id);
}

VOID FJ_Critical_Section_End(UINT32* spin_id, UINT32 sr)
{
	fj_critical_section_end(spin_id, sr);
}

UINT64 FJ_TIMESTAMP_Get_Counter( VOID )
{
	return fj_timestamp_get_counter();
}

UINT32 FJ_Linux_Get_State(VOID)
{
	return fj_linux_get_state();
}
