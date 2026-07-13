/**
 * @file		fs_if_receiver.h
 * @brief		APIs of received request depend on FS(header)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FS_IF_RECV_H_
#define _FS_IF_RECV_H_

#include "fs_user_custom.h"
#include "fs_if.h"
#include "fs_sender.h"

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
/* Function														  */
/*----------------------------------------------------------------*/
// romarea_boot
extern FW_INT32 BF_Fs_If_Receiver_End_Task_Mdf(T_BF_FS_SENDER_MSG* req_msg);    // for DCF-task
extern FW_INT32 BF_Fs_If_Receiver_Start_Media(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_End_Media(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_QuickFormat(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Statvfs(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Set_VolLabel(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Get_VolLabel(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Stat(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Get_Attribute(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Open(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Close(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Read(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Write(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Write_As(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Write_Result(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Lseek(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Lseek64(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Create(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Copy(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Remove(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Remove_Multi(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Fstat(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Mkdir(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Rmdir(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Opendir(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Readdir(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Closedir(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Chdir(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Get_Cur_Dir(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Set_Protection_Mode(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Get_Media_State_By_Poll(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Set_Media_State(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Get_Media_State(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Check_Media_State(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Get_Cur_Media(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Get_FreeSize(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Check_Mbr(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Make_Mbr(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Set_Media_DMA(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Set_Vfs_Write_Err_Cb(T_BF_FS_SENDER_MSG* req_msg);

// romarea_comm
extern FW_INT32 BF_Fs_Receiver_Copy_File_To_Drive(T_BF_FS_SENDER_MSG* req_msg);

// romarea_usb
extern FW_INT32 BF_Fs_If_Receiver_End_Drv(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Init_Drv(T_BF_FS_SENDER_MSG* req_msg);

// romarea_last
extern FW_INT32 BF_Fs_If_Receiver_Get_Cur_Drv(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Sync(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Fsync(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Rename(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Set_Attribute(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Set_Attribute_Multi(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Get_Media_Protect(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Change_Media(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Init_SD0_For_Copy(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Init_SD1_For_Copy(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_End_SD_For_Copy(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Drive_To_Drive_Copy(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Format(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Date_Time_Sync(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Get_Media_Format_Type(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Set_Time(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Get_User_Setting(T_BF_FS_SENDER_MSG* req_msg);
extern FW_INT32 BF_Fs_If_Receiver_Fsfirst( T_BF_FS_SENDER_MSG* req_msg );
extern FW_INT32 BF_Fs_If_Receiver_Fsnext( T_BF_FS_SENDER_MSG* req_msg );

#endif
