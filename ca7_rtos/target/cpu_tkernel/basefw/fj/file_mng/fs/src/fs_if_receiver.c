/**
 * @file		fs_if_receiver.c
 * @brief		APIs of received request depend on FS
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "fs_if_receiver.h"
#include "fs_controller.h"

/////////////////////////////////////////////////////
// romarea_boot
/////////////////////////////////////////////////////

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

#if 1	// for DCF-task
FW_INT32 BF_Fs_If_Receiver_End_Task_Mdf(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_End_Task_MDF();
}
#endif

/**
 * @brief media re-initialize request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Fs_If_Receiver_Start_Media(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Start_Media(*(FW_INT32*)&req_msg->param);
}

/**
 * @brief media re-initialize request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Fs_If_Receiver_End_Media(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_End_Media(*(FW_INT32*)&req_msg->param);
}

/**
 * @brief media format request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Fs_If_Receiver_QuickFormat(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_QuickFormat((FW_CHAR*)&req_msg->param);
}

/**
 * @brief Statvfs request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Statvfs(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Statvfs((FW_CHAR*)&req_msg->param, ((T_FS_CONTROLLER_RET_STATVFS_DAT*)req_msg->result_addr)->buf);
}

/**
 * @brief SetVolLabel request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Set_VolLabel(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Set_VolLabel(
			   ((T_BF_FS_IF_VOLLABEL_DATA*)&req_msg->param)->fs_drvname,
			   ((T_BF_FS_IF_VOLLABEL_DATA*)&req_msg->param)->fs_label);
}

/**
 * @brief Get VolLabel request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Get_VolLabel(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Get_VolLabel(
				(FW_CHAR*)&req_msg->param, 
				((T_FS_CONTROLLER_RET_GET_VOL_LABEL*)req_msg->result_addr)->vollabel);
}

/**
 * @brief Stat request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Stat(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Stat(
			   (FW_CHAR*)&req_msg->param,
			   ((T_FS_CONTROLLER_RET_STAT_DAT*)req_msg->result_addr)->sbuf);
}

/**
 * @brief GetAttribute request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Get_Attribute(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Get_Attribute(
			   (FW_CHAR*)&req_msg->param,
			   ((T_FS_CONTROLLER_RET_GET_ATTR_DAT*)req_msg->result_addr)->attributes);
}

/**
 * @brief Open request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Open(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Open(
			   ((T_BF_FS_IF_OPEN_DATA*)&req_msg->param)->fs_fname,
			   ((T_BF_FS_IF_OPEN_DATA*)&req_msg->param)->fs_mode,
			   ((T_BF_FS_IF_OPEN_DATA*)&req_msg->param)->fs_amode);
}

/**
 * @brief Close request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Close(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Close((FW_INT32)req_msg->param);
}

/**
 * @brief Read request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Read(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Read(
			   ((T_BF_FS_IF_RDWT_DATA*)&req_msg->param)->fs_fno,
			   (FW_ULONG)((T_BF_FS_IF_RDWT_DATA*)&req_msg->param)->fs_addr,
			   ((T_BF_FS_IF_RDWT_DATA*)&req_msg->param)->fs_count,
			   ((T_FS_CONTROLLER_RET_TYPE_READ_DAT*)req_msg->result_addr)->length);
}

/**
 * @brief Write request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Write(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Write(
			   ((T_BF_FS_IF_RDWT_DATA*)&req_msg->param)->fs_fno,
			   (FW_ULONG)((T_BF_FS_IF_RDWT_DATA*)&req_msg->param)->fs_addr,
			   ((T_BF_FS_IF_RDWT_DATA*)&req_msg->param)->fs_count,
			   ((T_FS_CONTROLLER_RET_TYPE_WRITE_DAT*)req_msg->result_addr)->length);
}

/**
 * @brief Async Write request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Write_As(T_BF_FS_SENDER_MSG* req_msg)
{
	FW_ULONG length;

	return BF_Fs_Controller_Write(
			   ((T_BF_FS_IF_RDWT_DATA*)&req_msg->param)->fs_fno,
			   (FW_ULONG)((T_BF_FS_IF_RDWT_DATA*)&req_msg->param)->fs_addr,
			   ((T_BF_FS_IF_RDWT_DATA*)&req_msg->param)->fs_count,
			   (0 != req_msg->result_addr) ? ((T_FS_CONTROLLER_RET_TYPE_WRITE_DAT*)req_msg->result_addr)->length : &length);
}

FW_INT32 BF_Fs_If_Receiver_Write_Result(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Write_Result(((T_FS_CONTROLLER_RET_TYPE_WRITE_RESULT_DAT*)req_msg->result_addr)->length);
}

/**
 * @brief Create request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Create(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Create(
			   ((T_BF_FS_IF_CREATE_DATA*)&req_msg->param)->fs_fname,
			   ((T_BF_FS_IF_CREATE_DATA*)&req_msg->param)->fs_amode);
}

/**
 * @brief Copy request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Copy(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Copy(
			   ((T_BF_FS_IF_FNAME_DATA*)&req_msg->param)->fs_srcname,
			   ((T_BF_FS_IF_FNAME_DATA*)&req_msg->param)->fs_dstname);
}

/**
 * @brief Remove request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Remove(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Remove((FW_CHAR*)&req_msg->param);
}

/**
 * @brief Remove request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Remove_Multi(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Remove_Multi(
				((T_BF_FS_IF_DEL_MULTI_DATA*)&req_msg->param)->dirname,
				((T_BF_FS_IF_DEL_MULTI_DATA*)&req_msg->param)->filename_list,
				((T_BF_FS_IF_DEL_MULTI_DATA*)&req_msg->param)->fnum,
				((T_BF_FS_IF_DEL_MULTI_DATA*)&req_msg->param)->pcallback);
}

/**
 * @brief Fstat request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Fstat(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Fstat(
			   (FW_INT32)req_msg->param,
			   ((T_FS_CONTROLLER_RET_TYPE_FSTAT_DAT*)req_msg->result_addr)->sbuf);
}

/**
 * @brief Mkdir request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Mkdir(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Mkdir((FW_CHAR*)&req_msg->param);
}

/**
 * @brief Rmdir request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Rmdir(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Rmdir((FW_CHAR*)&req_msg->param);
}

/**
 * @brief Opendir request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Opendir(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Opendir(
			   (FW_CHAR*)&req_msg->param,
			   ((T_FS_CONTROLLER_RET_TYPE_OPENDIR_DAT*)req_msg->result_addr)->dNo);
}

/**
 * @brief Readdir request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Readdir(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Readdir(
			   (FW_INT32)req_msg->param,
			   ((T_FS_CONTROLLER_RET_TYPE_READDIR_DAT*)req_msg->result_addr)->fsdirent);
}

/**
 * @brief Chdir request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Chdir(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Chdir((FW_CHAR*)&req_msg->param);
}

/**
 * @brief GetCurdir request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Get_Cur_Dir(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Get_Cur_Dir(
			   ((T_BF_FS_IF_CURDIR_DATA*)&req_msg->param)->fs_drvname,
			   ((T_BF_FS_IF_CURDIR_DATA*)&req_msg->param)->fs_size,
			   ((T_FS_CONTROLLER_RET_TYPE_GET_CURDIR_DAT*)req_msg->result_addr)->curpath);
}

/**
 * @brief SetProtectionMode request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Set_Protection_Mode(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Set_Protection_Mode(((T_BF_FS_IF_SETPROTMODE_DATA*)&req_msg->param)->fs_drvname, ((T_BF_FS_IF_SETPROTMODE_DATA*)&req_msg->param)->fs_wrt_prot);
}

FW_INT32 BF_Fs_If_Receiver_Get_Media_State_By_Poll(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Get_Media_State_By_Poll(
			   ((T_BF_FS_IF_MEDIA_DETECT_BY_POLL_DATA*)&req_msg->param)->ch,
			   ((T_BF_FS_IF_MEDIA_DETECT_BY_POLL_DATA*)&req_msg->param)->interval_time,
			   ((T_BF_FS_IF_MEDIA_DETECT_BY_POLL_DATA*)&req_msg->param)->check_count);
}

FW_INT32 BF_Fs_If_Receiver_Set_Media_State(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Set_Media_State(
			   ((T_BF_FS_IF_MEDIA_DETECT_BY_POLL_DATA*)&req_msg->param)->ch,
			   ((T_BF_FS_IF_MEDIA_DETECT_BY_POLL_DATA*)&req_msg->param)->interval_time,
			   ((T_BF_FS_IF_MEDIA_DETECT_BY_POLL_DATA*)&req_msg->param)->check_count);
}

FW_INT32 BF_Fs_If_Receiver_Get_Media_State(T_BF_FS_SENDER_MSG* req_msg)
{
	CHAR *param;
	
	param = (CHAR*)&req_msg->param;

	return BF_Fs_Controller_Get_Media_State(*(E_BF_MEDIA_STATE_MEDIA_TYPE*)param);
}

FW_INT32 BF_Fs_If_Receiver_Check_Media_State(T_BF_FS_SENDER_MSG* req_msg)
{
	CHAR *param;
	
	param = (CHAR*)&req_msg->param;

	return BF_Fs_Controller_Check_Media_State(*(E_BF_MEDIA_STATE_MEDIA_TYPE*)param);
}

FW_INT32 BF_Fs_If_Receiver_Get_Cur_Media(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Get_Cur_Media();
}

FW_INT32 BF_Fs_If_Receiver_Get_FreeSize(T_BF_FS_SENDER_MSG* req_msg)
{
	CHAR *param;
	
	param = (CHAR*)&req_msg->param;

	return BF_Fs_Controller_Get_FreeSize(*(E_BF_MEDIA_STATE_MEDIA_TYPE*)param, ((T_FS_CONTROLLER_RET_GET_FREESIZE_DAT*)req_msg->result_addr)->free_size);
}

FW_INT32 BF_Fs_If_Receiver_Check_Mbr(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Check_Mbr();
}

FW_INT32 BF_Fs_If_Receiver_Make_Mbr(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Make_Mbr();
}

FW_INT32 BF_Fs_If_Receiver_Set_Media_DMA(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Set_Media_DMA((FW_UCHAR)(req_msg->param >> 24));
}

FW_INT32 BF_Fs_If_Receiver_Get_Media_Format_Type(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Get_Media_Format_Type(
			   ((T_FS_CONTROLLER_RET_GET_MEDIA_FORMAT_DAT*)req_msg->result_addr)->used_media,
			   ((T_FS_CONTROLLER_RET_GET_MEDIA_FORMAT_DAT*)req_msg->result_addr)->format_type);
}

//#pragma arm section code="BASEFW_COMM_CODE"
/////////////////////////////////////////////////////
// romarea_comm
/////////////////////////////////////////////////////

/**
 * @brief Lseek request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Lseek(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Lseek(
			   ((T_BF_FS_IF_LSEEK_DATA*)&req_msg->param)->fs_fno,
			   ((T_BF_FS_IF_LSEEK_DATA*)&req_msg->param)->fs_offset,
			   ((T_BF_FS_IF_LSEEK_DATA*)&req_msg->param)->fs_whence,
			   ((T_FS_CONTROLLER_RET_TYPE_LSEEK_DAT*)req_msg->result_addr)->length);
}

/**
 * @brief Lseek request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Lseek64(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Lseek64(
			   ((T_BF_FS_IF_LSEEK64_DATA*)&req_msg->param)->fs_fno,
			   ((T_BF_FS_IF_LSEEK64_DATA*)&req_msg->param)->fs_offset,
			   ((T_BF_FS_IF_LSEEK64_DATA*)&req_msg->param)->fs_whence,
			   ((T_FS_CONTROLLER_RET_TYPE_LSEEK64_DAT*)req_msg->result_addr)->length);
}

/**
 * @brief Closedir request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Closedir(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Closedir((FW_INT32)req_msg->param);
}

FW_INT32 BF_Fs_If_Receiver_End_Drv(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_End_Drv();
}

FW_INT32 BF_Fs_If_Receiver_Init_Drv(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Init_Drv((FW_UCHAR)req_msg->param);
}

FW_INT32 BF_Fs_If_Receiver_Get_Media_Protect(T_BF_FS_SENDER_MSG* req_msg)
{
	CHAR *param;
	
	param = (CHAR*)&req_msg->param;
	
	return BF_Fs_Controller_Get_Media_Protect(*(E_BF_MEDIA_STATE_MEDIA_TYPE*)param);
}

/**
 * @brief GetCurDrv request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Get_Cur_Drv(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Get_Cur_Drv(((T_FS_CONTROLLER_RET_GET_CURDRV_DAT*)req_msg->result_addr)->drvname);
}

FW_INT32 BF_Fs_Receiver_Copy_File_To_Drive(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Copy_File_To_Drive(
			   ((T_BF_FS_IF_DRVNAME_DATA*)&req_msg->param)->fs_srcdrv,
			   ((T_BF_FS_IF_DRVNAME_DATA*)&req_msg->param)->fs_dstdrv);
}

//#pragma arm section code="BASEFW_LAST_CODE"
/////////////////////////////////////////////////////
// romarea_last
/////////////////////////////////////////////////////

/**
 * @brief Sync request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Sync(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Sync((FW_CHAR*)&req_msg->param);
}

/**
 * @brief Fsync request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Fsync(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Fsync((FW_INT32)req_msg->param);
}

/**
 * @brief Rename request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Rename(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Rename(
			   ((T_BF_FS_IF_FNAME_DATA*)&req_msg->param)->fs_srcname,
			   ((T_BF_FS_IF_FNAME_DATA*)&req_msg->param)->fs_dstname);
}

/**
 * @brief SetAttribute request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Set_Attribute(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Set_Attribute(((T_BF_FS_IF_SETATTR_DATA*)&req_msg->param)->fs_fname, ((T_BF_FS_IF_SETATTR_DATA*)&req_msg->param)->fs_attr);
}

/**
 * @brief SetAttribute request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Set_Attribute_Multi(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Set_Attribute_Multi(
				((T_BF_FS_IF_SETATTR_MULTI_DATA*)&req_msg->param)->dirname,
				((T_BF_FS_IF_SETATTR_MULTI_DATA*)&req_msg->param)->fs_attr,
				((T_BF_FS_IF_SETATTR_MULTI_DATA*)&req_msg->param)->attr_mode,
				((T_BF_FS_IF_SETATTR_MULTI_DATA*)&req_msg->param)->filename_list,
				((T_BF_FS_IF_SETATTR_MULTI_DATA*)&req_msg->param)->fnum,
				((T_BF_FS_IF_SETATTR_MULTI_DATA*)&req_msg->param)->pcallback);
}

/**
 * @brief change used media
 * @param i:*req_msg pointer of request message
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Change_Media(T_BF_FS_SENDER_MSG* req_msg)
{
	CHAR *param;
	
	param = (CHAR*)&req_msg->param;

	return BF_Fs_Controller_Change_Media(*(E_BF_MEDIA_STATE_MEDIA_TYPE*)param);
}

FW_INT32 BF_Fs_If_Receiver_Init_SD0_For_Copy(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Init_SD0_For_Copy();
}

FW_INT32 BF_Fs_If_Receiver_Init_SD1_For_Copy(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Init_SD1_For_Copy();
}

FW_INT32 BF_Fs_If_Receiver_End_SD_For_Copy(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_End_SD_For_Copy();
}

FW_INT32 BF_Fs_If_Receiver_Drive_To_Drive_Copy(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Drive_To_Drive_Copy(
			   ((T_BF_FS_IF_DRVNAME_DATA*)&req_msg->param)->fs_srcdrv,
			   ((T_BF_FS_IF_DRVNAME_DATA*)&req_msg->param)->fs_dstdrv);
}

FW_INT32 BF_Fs_If_Receiver_Format(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Format(*(FW_INT32*)&req_msg->param);
}

FW_INT32 BF_Fs_If_Receiver_Date_Time_Sync(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Date_Time_Sync(
			   ((T_BF_FS_IF_DATETIME_SYNC_DATA*)&req_msg->param)->org_file_path,
			   ((T_BF_FS_IF_DATETIME_SYNC_DATA*)&req_msg->param)->sync_file_ext,
			   ((T_BF_FS_IF_DATETIME_SYNC_DATA*)&req_msg->param)->fs_utim);
}

/**
 * @brief Stat request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Set_Time(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Set_Time(
			   (FW_CHAR*)&req_msg->param,
			   ((T_FS_CONTROLLER_RET_SET_TIME_DAT*)req_msg->result_addr)->timestamp_type,
			   ((T_FS_CONTROLLER_RET_SET_TIME_DAT*)req_msg->result_addr)->utime);
}

/**
 * @brief Async Write request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Set_Vfs_Write_Err_Cb(T_BF_FS_SENDER_MSG* req_msg)
{
	return BF_Fs_Controller_Set_Vfs_Write_Err_Cb(
			   ((T_BF_FS_IF_SET_WRITE_ERR*)&req_msg->param)->fs_fno,
			   ((T_BF_FS_IF_SET_WRITE_ERR*)&req_msg->param)->pcallback );
}

/**
 * @brief Async Write request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Get_User_Setting( T_BF_FS_SENDER_MSG* req_msg )
{
	return BF_Fs_Controller_Set_User_Setting(
			   ( FW_ULONG )(( T_BF_FS_IF_RDWT_DATA* )&req_msg->param )->fs_addr,
			   ( ( T_BF_FS_IF_RDWT_DATA* )&req_msg->param)->fs_count );
}

/**
 * @brief Fsfirst
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Fsfirst( T_BF_FS_SENDER_MSG* req_msg )
{
	return BF_Fs_Controller_Fsfirst(
			   ((T_BF_FS_IF_ENTRY_DATA*)&req_msg->param)->fs_index,
			   ((T_BF_FS_IF_ENTRY_DATA*)&req_msg->param)->fs_name,
			   ((T_BF_FS_IF_ENTRY_DATA*)&req_msg->param)->fs_attr,
			   ((T_FS_CONTROLLER_RET_ENTRY*)req_msg->result_addr)->buf);
}

/**
 * @brief Fsfirst
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_INT32 BF_Fs_If_Receiver_Fsnext( T_BF_FS_SENDER_MSG* req_msg )
{
	return BF_Fs_Controller_Fsnext(
			   ((T_BF_FS_IF_ENTRY_DATA*)&req_msg->param)->fs_index,
			   ((T_FS_CONTROLLER_RET_ENTRY*)req_msg->result_addr)->buf);
}
