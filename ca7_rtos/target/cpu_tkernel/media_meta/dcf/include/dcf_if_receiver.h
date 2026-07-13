/**
 * @file		dcf_if_receiver.h
 * @brief		APIs of received request depend on DCF(header)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _DCF_IF_RECV_H_
#define _DCF_IF_RECV_H_

#include "fs_user_custom.h"

#include "fs_sender.h"
#include "dcf_sender.h"


/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/**
 * @breif asyncronous processing continue/stop
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_RECIVER_ASYNC_CONTINUE = 0x00,
	E_BF_DCF_IF_RECIVER_ASYNC_STOP = 0x01
} E_BF_DCF_IF_RECIVER_ASYNC;

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
#if 1	// for DCF-task
FW_INT32 BF_Dcf_If_Receiver_End_Task_Mdf(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Set_Cur_Drive(T_DCF_SENDER_MSG* req_msg);
#endif

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
FW_INT32 BF_Dcf_If_Receiver_Save_Ext(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Exif_Info_Ext(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Ext_Exif_Thumbnail(T_DCF_SENDER_MSG* req_msg);
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

FW_INT32 BF_Dcf_If_Receiver_Format(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Create_DataBase(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Start_Media(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_End_Media(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Save_Jpeg(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Save_Raw(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Save_Thm(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Exif_Info(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Exif_Info_Name(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Load_Vga(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Exif_Thumbnail(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Start_Div_Load(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Result_Div_Load(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Async_Stop(T_DCF_SENDER_MSG* req_msg);
E_BF_DCF_IF_RECIVER_ASYNC BF_Dcf_If_Receiver_Is_Async_Stop(FW_VOID);
FW_INT32 BF_Dcf_If_Receiver_Save_Resize(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Update_Rotate(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Del_Single_Obj(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Del_All_Obj_In_Current_Dir(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Del_All_Obj(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Set_Single_Obj_Attr(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Set_All_Obj_Attr_In_Current_Dir(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Set_All_Obj_Attrl(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Fstat(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Entry(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_File_Info(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_File_Id(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_File_Type(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Make_New_File_Path(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Make_New_Thm_File_Path(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Make_Latest_File_Path(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Make_Index_File_Path(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Index_File_Path(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Updata_File(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Updata_Option_File(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Set_File_Name(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Jpeg_Quant_Table(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Set_Free_File_Name(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_File_Name(T_DCF_SENDER_MSG* req_msg);
//FW_INT32 BF_Dcf_If_Receiver_Get_Duplicate_File_No(T_DCF_SENDER_MSG *req_msg);

// NEW API ===================================================================
FW_INT32 BF_Dcf_If_Receiver_Reset_Db(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Set_Dir_Db(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Read_Dir_Total_Cnt(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Idx_By_Dir_Name(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Read_Dir_Name(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Read_Dir_Duplication_Status(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Read_Dir_Date_Time(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Max_File_Num_As_Dir_Num(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Set_Current_Dir(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Dir_Idx(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Read_Obj_Cnt(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Read_Obj_Name(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Read_Dir_Obj_Name(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Read_Obj_Inf(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Write_Obj_Attr(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Idx_By_Obj_Name(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Set_Allow_Ext_List(T_DCF_SENDER_MSG* req_msg);
//FW_INT32 BF_Dcf_If_Receiver_Read_Allowed_Ext_Cnt(T_DCF_SENDER_MSG *req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Typ_By_Ext_Name(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Ext_Name_By_Typ(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Full_Path_By_File_Index(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Is_File_Type(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Obj_Info(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Create_Dir(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_First_Free_File_No(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_New_Copy_Dir_No(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Last_File_Number(T_DCF_SENDER_MSG* req_msg);

FW_INT32 BF_Dcf_If_Receiver_Fstat(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Dir_File_Exist(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Del_Single_Dcf_Dir(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Del_All_Dcf_Dir(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Dcf_Key(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Make_Dir_Name(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Make_File_Name(T_DCF_SENDER_MSG* req_msg);

#if 1   /* for Multi-Delete */
FW_INT32 BF_Dcf_If_Receiver_Init_Select_List(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Add_File_To_Select_List(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Remove_File_From_Select_List(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Execute_Multi_Delete(T_DCF_SENDER_MSG* req_msg);
#endif  /* for Multi-Delete */

FW_INT32 BF_Dcf_If_Receiver_Check_Date_Db_File(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Load_Date_Db(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Create_Date_Db_File(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Date_Index_By_Month(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Create_Day_Table(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Month_Top_Date_Index(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Month_Latest_Date_Index(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Day_Obj_Num(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Sort_Date_Db(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Save_Date_Db_File(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Date_Index_By_File_Num(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Date_Index_By_Date(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Obj_Info_By_Date_Index(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Date_Info(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Obj_Date_Info(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Date_Db_Obj_Num(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Month_Obj_Num(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Check_Date_Db_Comp_File(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Update_Date_Db(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Day_Cnt_By_Index(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Update_Date_Db_Obj_Stat(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Init_Date_Db_buffer(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Date_Db_File_Status(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Get_Date_Db_File_Size(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Set_Date_Db_Filter_Type(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Write_Date_Db_Obj_Filter_Attr(T_DCF_SENDER_MSG* req_msg);
FW_INT32 BF_Dcf_If_Receiver_Update_Date_Db_Obj_Filter_Attr(T_DCF_SENDER_MSG* req_msg);
#endif
