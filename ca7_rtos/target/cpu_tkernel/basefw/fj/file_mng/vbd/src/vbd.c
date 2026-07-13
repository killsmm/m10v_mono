/**
 * @file		vbd.c
 * @brief		Virtual Block Device function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include <string.h>
#include "ddim_typedef.h"
#include "fj_filesystem.h"
#include "fs_if.h"
#include "fs_controller.h"
#include "fj_media.h"
#include "ipcu.h"
#include "l1l2cache.h"
#include "mwnf_core.h"
#include "dd_top.h"
#include "dd_exs.h"
#include "../include/vbd.h"
#include "ipcufs.h"
// --- REMOVE_ES3_HARDWARE BEGIN ---
#include "em_int_cmn.h"
#include "em_if_ext.h"
// --- REMOVE_ES3_HARDWARE END --

/*----------------------------------------------------------------*/
/* Definition													  */
/*----------------------------------------------------------------*/
/* IPCU event flag pattern */
#define FLG_FS_VBD_RECEIVE_CB			(0x80000000)

/* NF partition table Linux rootfs area num */
#define LINUX_ROOTFS_PARTITION_NUM		(6)

/*----------------------------------------------------------------*/
/* Enumeration													  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Structure													  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Data													  */
/*----------------------------------------------------------------*/
static volatile UCHAR* g_bf_vbd_info_table;
static volatile UINT32 g_bf_vbd_info_length;

static volatile UCHAR  g_media_init_flag = 0;

static volatile T_NF_PARTITION g_NF_linux_rootfs_partition_table;

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
/* IPCU Request Callback */
FW_VOID vbd_ipcu_event_request_cb( UCHAR id, UCHAR *data, UINT32 length, UINT32 cont, UINT32 total_length )
{
	g_bf_vbd_info_table  = data;
	g_bf_vbd_info_length = length;

//	BF_Fs_User_Custom_Info_Printer(("[VBD]: Send IPCU request on VBD_receiver task.\n"));

	OS_User_Set_Flg( FID_FS_VBD_RECEIVE, FLG_FS_VBD_RECEIVE_CB );
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/**
 * @brief VBD receiver task [TASK]
 * @param FW_VOID
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_VOID VBD_receiver(INT32 stacd)
{
	T_FS_VBD_INFO_TABLE_COMM* info_tbl_comm;
	OS_USER_FLGPTN	flgptn;
	FW_UCHAR ipcu_id;
	FW_INT32 ercd;

	BF_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_9, &ipcu_id);
	BF_IPCU_SetReqCB(ipcu_id, vbd_ipcu_event_request_cb);

	OS_User_Clr_Flg(FID_FS_VBD_RECEIVE, ~FLG_FS_VBD_RECEIVE_CB);

	// make partition table
	memcpy((void*)&g_NF_linux_rootfs_partition_table,
		   (void*)&gNF_partition_table[LINUX_ROOTFS_PARTITION_NUM], sizeof(T_NF_PARTITION));

	/* Wait for a IPCU response from Linux */
	while( 1 ) {
		OS_User_Twai_Flg( FID_FS_VBD_RECEIVE, FLG_FS_VBD_RECEIVE_CB, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );
		OS_User_Clr_Flg(FID_FS_VBD_RECEIVE, ~FLG_FS_VBD_RECEIVE_CB);

		info_tbl_comm = (T_FS_VBD_INFO_TABLE_COMM*)g_bf_vbd_info_table;

//		BF_Fs_User_Custom_Info_Printer(("[VBD]: VBD_receiver start. ope_cmd=0x%x\n", info_tbl_comm->ope_cmd));

		switch(info_tbl_comm->ope_cmd) {
			case D_BF_FS_VBD_NANDBLK_GET_SIZE:
				ercd = BF_Vbd_Get_Sector_Size((T_FS_VBD_INFO_TABLE_GET_SIZE*)g_bf_vbd_info_table);
				break;

			case D_BF_FS_VBD_NANDBLK_WRITE :
				ercd = BF_Vbd_Sector_Write((T_FS_VBD_INFO_TABLE_WRITE*)g_bf_vbd_info_table);
				break;

			case D_BF_FS_VBD_NANDBLK_READ :
				ercd = BF_Vbd_Sector_Read((T_FS_VBD_INFO_TABLE_READ*)g_bf_vbd_info_table);
				break;

			default :
				g_bf_vbd_info_length = 0;
				ercd = 1;
				break;
		}

		if (g_bf_vbd_info_length != 0) {
			// IPCU complete send
			BF_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_8, &ipcu_id);
			BF_L1l2cache_Clean_Flush_Addr((UINT32)g_bf_vbd_info_table, g_bf_vbd_info_length);
			ercd = BF_IPCU_Send(ipcu_id, (UCHAR*)g_bf_vbd_info_table, g_bf_vbd_info_length, 1);
			while( ercd == FJ_ERR_NG_COMMUNICATON) {
				ercd = BF_IPCU_Send(ipcu_id, (UCHAR*)g_bf_vbd_info_table, g_bf_vbd_info_length, 1);
				OS_User_Dly_Tsk(10);
			}
			BF_IPCU_Close(ipcu_id);
		}
//		BF_Fs_User_Custom_Info_Printer(("[VBD]: VBD_receiver end.(%d)\n", ercd ));
	}
}

FW_INT32 BF_Vbd_Get_Sector_Size(T_FS_VBD_INFO_TABLE_GET_SIZE* info_table)
{
	info_table->sector_size = (g_NF_linux_rootfs_partition_table.area_size / 1024);
	info_table->error_code = D_BF_FS_VBD_ERR_OK;

	return info_table->error_code;
}

FW_INT32 BF_Vbd_Sector_Write(T_FS_VBD_INFO_TABLE_WRITE* info_table)
{
	FW_INT32 ercd = D_BF_FS_VBD_ERR_OK;

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	CHAR	mdr = (*(( unsigned long * )0x1DFF4000) & 0x7);
	INT32 	selector = g_NF_linux_rootfs_partition_table.selector;

	if (mdr == 1) {
		if((BF_Ipcufs_Get_Unmount_Request_Flg()) && (g_media_init_flag == 0)){
			FJ_EMMC_Init();
			FJ_EMMC_Device_Init();
			g_media_init_flag = 1;
		}

		// eMMC
		ercd = MWEM_DirectWrite(g_NF_linux_rootfs_partition_table.area[selector].sector_num + info_table->start_sec,
								info_table->write_sec, info_table->data_addr);
		while(ercd == EM_INVAL){
			OS_User_Dly_Tsk(100);
			ercd = MWEM_DirectWrite(g_NF_linux_rootfs_partition_table.area[selector].sector_num + info_table->start_sec,
									info_table->write_sec, info_table->data_addr);
		}
	}
	else {
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		// NF
		// if unmount request exist, enable NF interrupt
		if((BF_Ipcufs_Get_Unmount_Request_Flg()) && (g_media_init_flag == 0)){
			ercd = Media_Custom_NF_GIC_Ctrl();
			if (ercd != 0) {
				BF_Fs_User_Custom_Err_Printer(("%s(): Media_Custom_NF_GIC_Ctrl() failed: ercd=%x\n", __func__, ercd ));
				info_table->error_code = 1;
				return info_table->error_code;
			}
			g_media_init_flag = 1;
		}
#if 0 // not supported
		ercd = FJ_NF_Write_Physical_Data(g_NF_linux_rootfs_partition_table.area[selector].sector_num,
										 (UINT32*)info_table->data_addr);
		while(ercd == D_MWNF_UNINITIALIZE_ERR){
			OS_User_Dly_Tsk(100);
			ercd = FJ_NF_Write_Physical_Data(g_NF_linux_rootfs_partition_table.area[selector].sector_num,
											 (UINT32*)info_table->data_addr);
		}
#endif
		ercd = FJ_ERR_OK;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	}
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	if (ercd == FJ_ERR_OK) {
		info_table->error_code = D_BF_FS_VBD_ERR_OK;
	}
	else {
		info_table->error_code = D_BF_FS_VBD_ERR_NG;
	}

	return info_table->error_code;
}

FW_INT32 BF_Vbd_Sector_Read(T_FS_VBD_INFO_TABLE_READ* info_table)
{
	FW_INT32 ercd = D_BF_FS_VBD_ERR_OK;
	INT32 selector = g_NF_linux_rootfs_partition_table.selector;
	INT32 i;
	ULONG sec_addr;
	UINT32 read_sec, block_num, start_blk;
	FW_CHAR* data_addr;

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	CHAR	mdr = (*(( unsigned long * )0x1DFF4000) & 0x7);

	if (mdr == 1) {
		// if unmount request exist, eMMC re-init
		if((BF_Ipcufs_Get_Unmount_Request_Flg()) && (g_media_init_flag == 0)){
			FJ_EMMC_Init();
			FJ_EMMC_Device_Init();
			g_media_init_flag = 1;
		}

		// eMMC
		ercd = MWEM_DirectRead(g_NF_linux_rootfs_partition_table.area[selector].sector_num + info_table->start_sec,
							   info_table->read_sec, info_table->data_addr);
		while(ercd == EM_INVAL){
			OS_User_Dly_Tsk(100);
			ercd = MWEM_DirectRead(g_NF_linux_rootfs_partition_table.area[selector].sector_num + info_table->start_sec,
								   info_table->read_sec, info_table->data_addr);
		}
	}
	else {
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		// NF
		// if unmount request exist, enable NF interrupt
		if((BF_Ipcufs_Get_Unmount_Request_Flg()) && (g_media_init_flag == 0)){
			ercd = Media_Custom_NF_GIC_Ctrl();
			if (ercd != 0) {
				BF_Fs_User_Custom_Err_Printer(("%s(): Media_Custom_NF_GIC_Ctrl() failed: ercd=%x\n", __func__, ercd ));
				info_table->error_code = 1;
				return info_table->error_code;
			}
			g_media_init_flag = 1;
		}
		if (info_table->read_sec >= D_BF_FS_VBD_SEC_PAR_BLOCK) {
			read_sec = D_BF_FS_VBD_SEC_PAR_BLOCK;
			if (info_table->read_sec % D_BF_FS_VBD_SEC_PAR_BLOCK == 0) {
				block_num = info_table->read_sec / D_BF_FS_VBD_SEC_PAR_BLOCK;
			}
			else{
				block_num = (info_table->read_sec / D_BF_FS_VBD_SEC_PAR_BLOCK) + 1;
			}
		}
		else{
			block_num = 1;
			read_sec = info_table->read_sec % D_BF_FS_VBD_SEC_PAR_BLOCK;
		}

		data_addr = info_table->data_addr;
		start_blk = info_table->start_sec / D_BF_FS_VBD_SEC_PAR_BLOCK;
		for (i=0; i<block_num; i++) {
			sec_addr = (info_table->start_sec - start_blk * D_BF_FS_VBD_SEC_PAR_BLOCK) +
					(g_NF_linux_rootfs_partition_table.area[selector].lut[start_blk+i]) * D_BF_FS_VBD_SEC_PAR_BLOCK;
			ercd = MWNF_Direct_BRead_DataDMA(sec_addr,
											 read_sec, data_addr);
		while(ercd == D_MWNF_UNINITIALIZE_ERR){
			OS_User_Dly_Tsk(100);
				ercd = MWNF_Direct_BRead_DataDMA(sec_addr,
												 read_sec, data_addr);
			}
			data_addr += D_BF_FS_VBD_SECTOR_SIZE * read_sec;
			if (i < block_num) {
				read_sec =  D_BF_FS_VBD_SEC_PAR_BLOCK;
			}
			else{
				read_sec = read_sec % D_BF_FS_VBD_SEC_PAR_BLOCK;
			}
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	}
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	if (ercd == FJ_ERR_OK) {
		info_table->error_code = D_BF_FS_VBD_ERR_OK;
	}
	else {
		info_table->error_code = D_BF_FS_VBD_ERR_NG;
	}

	return info_table->error_code;
}

