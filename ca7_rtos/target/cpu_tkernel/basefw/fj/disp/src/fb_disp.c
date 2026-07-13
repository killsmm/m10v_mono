
/**
 * @file		fb_disp.c
 * @brief		Set Disp's GR by request from IPCU
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2017 Socionext Inc.</I></B>
 */

#include <string.h>
#include "fb_disp.h"
#include "l1l2cache.h"
#include "im_disp.h"
#include "ipcu.h"

/*----------------------------------------------------------------*/
/* Definition													  */
/*----------------------------------------------------------------*/

/* IPCU event flag pattern */
#define FLG_IPCU_SET_OSD_CONFIG_CB	(0x00000001)

/*----------------------------------------------------------------*/
/* Enumeration													  */
/*----------------------------------------------------------------*/
typedef enum {
	E_SNI_FB_DISP_GRTRG_WRITE_NO_ACT = 0x00000000,	// Set TRG to off at once
	E_SNI_FB_DISP_GRTRG_WRITE_ACT,					// Set TRG to on
	E_SNI_FB_DISP_GRTRG_READ_NO_ACT,					// TRG is not on or set to off by frame stop
	E_SNI_FB_DISP_GRTRG_READ_ACT						// TRG is on
} E_SNI_FB_DISP_GRTRG;


/*----------------------------------------------------------------*/
/* Structure													  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Data													  */
/*----------------------------------------------------------------*/

static t_ipcu_mail_box g_bf_ipcu_info_table;

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
/* IPCU Request Callback */

VOID IPCU_request_OSD_cb( UCHAR id, UCHAR *data, UINT32 length, UINT32 cont, UINT32 total_length )
{
	memcpy(&g_bf_ipcu_info_table, data, sizeof(t_ipcu_mail_box));
	
	OS_User_Set_Flg( FID_IPCU_SET_OSD_CONFIG, FLG_IPCU_SET_OSD_CONFIG_CB );
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/**
 * @brief Set OSD configuration [TASK]
 * @param FW_VOID
 * @return FW_VOID
 * @note none
 * @attention none
 */

VOID IPCU_Set_OSD_config(INT32 stacd)
{
	t_ipcu_command* ipcu_comm;
	t_request* request;
	BYTE* temp;
	OS_USER_FLGPTN	flgptn;
	UCHAR ipcu_id_receive;
#ifndef CO_CMD_CT_OSD_FB_DISP
	UCHAR ipcu_id_send;
#endif//CO_CMD_CT_OSD_FB_DISP
	UINT32 ercd = 0;
	UINT32 command_cnt;
	UINT32 word_cnt;
	t_ipcu_mail_box result;
	E_IM_DISP_SEL_LAYER layer_no;

	BF_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_11, &ipcu_id_receive);
	BF_IPCU_SetReqCB(ipcu_id_receive, IPCU_request_OSD_cb);

	OS_User_Clr_Flg(FID_IPCU_SET_OSD_CONFIG, ~FLG_IPCU_SET_OSD_CONFIG_CB);

	printf(("IPCU_Set_OSD_config wait flg\n"));
	/* Wait for a IPCU response from Linux */
	while( 1 ) {
		OS_User_Twai_Flg( FID_IPCU_SET_OSD_CONFIG, FLG_IPCU_SET_OSD_CONFIG_CB, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );
		OS_User_Clr_Flg(FID_IPCU_SET_OSD_CONFIG, ~FLG_IPCU_SET_OSD_CONFIG_CB);
		
		printf(("IPCU_Set_OSD_config start\n"));

		ipcu_comm = (t_ipcu_command*)g_bf_ipcu_info_table.data;
		result.sequence_no = g_bf_ipcu_info_table.sequence_no;
		
		if(ipcu_comm->head.bit.gr_no == 0){
			layer_no =  E_IM_DISP_SEL_LAYER_OSD_0;
		}
		else{
			layer_no =  E_IM_DISP_SEL_LAYER_OSD_1;
		}
		
		for(command_cnt = 0; command_cnt < ipcu_comm->head.bit.command_num; command_cnt++){
			temp = (BYTE*)ipcu_comm;
			temp += ipcu_comm->command_offset[command_cnt];
			request = (t_request*)temp;
			
			for(word_cnt = 0; word_cnt < request->command_head.bit.size; word_cnt++){
				switch(request->command_head.bit.command_code){
					case eGRTISIZE :{
						U_IM_DISP_SIZE territory_size;
						territory_size.word = (ULONG)request->command[word_cnt];
						ercd = Im_DISP_Set_OSD_Territory_Size((E_IM_DISP_SEL)ipcu_comm->head.bit.block_no, 
															  (E_IM_DISP_SEL_LAYER)layer_no, 
															  territory_size);
						break;
					}

					case eGRTDSTA :{
						U_IM_DISP_DSTA position;
						position.word = (ULONG)request->command[word_cnt];
						ercd = Im_DISP_Set_OSD_Territory_Position((E_IM_DISP_SEL)ipcu_comm->head.bit.block_no, 
																  (E_IM_DISP_SEL_LAYER)layer_no, 
																  position);
						break;
					}

					case eGRSCCTL :{
						ercd = Im_DISP_Set_OSD_Area0_Switch((E_IM_DISP_SEL)ipcu_comm->head.bit.block_no, 
															(E_IM_DISP_SEL_LAYER)layer_no, 
															(E_IM_DISP_INPUT_DATA_AREA)(ULONG)request->command[word_cnt]);
						break;
					}

					case eGRRSZ :{
						T_IM_DISP_RESIZE resize;
						resize.rsz0 = (E_IM_DISP_RSZSL)request->command[0];
						resize.rsz1.word = (ULONG)request->command[1];
						resize.rsz2.word = (ULONG)request->command[2];
						resize.rsz3.word = (ULONG)request->command[3];
						ercd = Im_DISP_Set_OSD_Resize((E_IM_DISP_SEL)ipcu_comm->head.bit.block_no, (E_IM_DISP_SEL_LAYER)layer_no, &resize);
						
						word_cnt = request->command_head.bit.size;
						break;
					}

					case eGRISIZE :{
						U_IM_DISP_SIZE input_size[D_IM_DISP_OSD_DISPLAY_AREA_COUNT];
						memcpy(input_size, request->command, (sizeof(U_IM_DISP_SIZE))*(request->command_head.bit.size));
						ercd = Im_DISP_Set_OSD_Area_Size((E_IM_DISP_SEL)ipcu_comm->head.bit.block_no, 
														 (E_IM_DISP_SEL_LAYER)layer_no, 
														 input_size, (UINT32)request->command_head.bit.size);
														 
						word_cnt = request->command_head.bit.size;
						break;
					}

					case eGRDSTA :{
						U_IM_DISP_DSTA position;
						position.word = (ULONG)request->command[word_cnt];
						ercd = Im_DISP_Set_OSD_Display_Position((E_IM_DISP_SEL)ipcu_comm->head.bit.block_no, 
																(E_IM_DISP_SEL_LAYER)layer_no, 
																word_cnt , position);
						break;
					}

					case eGRHGA :{
						ercd = Im_DISP_Set_OSD_Lfd((E_IM_DISP_SEL)ipcu_comm->head.bit.block_no, 
												   (E_IM_DISP_SEL_LAYER)layer_no, 
												   word_cnt , (ULONG)request->command[word_cnt]);
						break;
					}

					case eGRSA0 :
						ercd = Im_DISP_Set_OSD_Area_Addr((E_IM_DISP_SEL)ipcu_comm->head.bit.block_no, 
														 (E_IM_DISP_SEL_LAYER)layer_no, 
														 (E_IM_DISP_OSD_SA_BANK_NO)word_cnt,
														 (ULONG)request->command[word_cnt]);
						break;

					case eGRSA :
						ercd = Im_DISP_Set_OSD_Area_Addr((E_IM_DISP_SEL)ipcu_comm->head.bit.block_no, 
														 (E_IM_DISP_SEL_LAYER)layer_no, 
														 (E_IM_DISP_OSD_SA_BANK_NO)word_cnt + E_IM_DISP_OSD_SA_BANK_NO_1, 
														 (ULONG)request->command[word_cnt]);
						break;

					case eGRAREN :
						ercd = Im_DISP_Set_OSD_Area_Enable((E_IM_DISP_SEL)ipcu_comm->head.bit.block_no, 
														   (E_IM_DISP_SEL_LAYER)layer_no, (ULONG)request->command[word_cnt]);
						break;

					case eGRTRG :{
						if(request->command[word_cnt] == E_SNI_FB_DISP_GRTRG_WRITE_ACT){
							if(ipcu_comm->head.bit.block_no == E_IM_DISP_HDMI){
								ercd = Im_DISP_Start((UINT32)layer_no, 
													  E_IM_DISP_SEL_LAYER_NONE, (BYTE)request->command[word_cnt]);
								break;
							}
							else{
								ercd = Im_DISP_Start(E_IM_DISP_SEL_LAYER_NONE, 
													 (UINT32)layer_no, (BYTE)request->command[word_cnt]);
								break;
							}
						}
						else{
							if(ipcu_comm->head.bit.block_no == E_IM_DISP_HDMI){
								ercd = Im_DISP_Stop((UINT32)layer_no, 
													E_IM_DISP_SEL_LAYER_NONE, 
													(BYTE)request->command[word_cnt], (E_IM_DISP_STOP_TYPE)request->command[word_cnt]);
								break;
							}
							
							else{
								ercd = Im_DISP_Stop(E_IM_DISP_SEL_LAYER_NONE, 
													(UINT32)layer_no, 
													(BYTE)request->command[word_cnt], (E_IM_DISP_STOP_TYPE)request->command[word_cnt]);
								break;
							}
						}
					}

					case eGRIDT :{
						U_IM_DISP_GRIDT gridt;
						gridt.word = (ULONG)request->command[word_cnt];
						ercd = Im_DISP_Set_OSD_Input_Data_Transfer((E_IM_DISP_SEL)ipcu_comm->head.bit.block_no, 
																   (E_IM_DISP_SEL_LAYER)layer_no, gridt);
						break;
					}

					case eGRRST :{
						UINT32 wait_Vsync;
						UINT32 layer_status;
						
						for(wait_Vsync = 0; wait_Vsync < 33; wait_Vsync++){
							if(ipcu_comm->head.bit.block_no == E_IM_DISP_HDMI){
								Im_DISP_Get_Status(&layer_status, NULL);
							}
							else{
								Im_DISP_Get_Status(NULL, &layer_status);
							}
							
							if((layer_status & layer_no) == E_IM_DISP_SEL_LAYER_NONE){
								if(ipcu_comm->head.bit.block_no == E_IM_DISP_HDMI){
									ercd = Im_DISP_Set_SR(layer_no, E_IM_DISP_SEL_LAYER_NONE, (BYTE)request->command[word_cnt]);
								}
								else{
									ercd = Im_DISP_Set_SR(E_IM_DISP_SEL_LAYER_NONE, layer_no, (BYTE)request->command[word_cnt]);
								}
								break;
							}
							OS_User_Dly_Tsk(0);
							if(wait_Vsync == 32){
								ercd = 1;
								break;
							}	
						}
						break;
					}
					
					case eGRIPO :{
						U_IM_DISP_GRIPO ipo;
						ipo.word = (ULONG)request->command[word_cnt];
						ercd = Im_DISP_Set_OSD_IPO((E_IM_DISP_SEL)ipcu_comm->head.bit.block_no, (E_IM_DISP_SEL_LAYER)layer_no, ipo);
						break;
					}

					default :
						ercd = 1;
						break;
				}
				
				if(ercd != 0){
					break;
				}
			}
			if(ercd != 0){
				break;
			}
		}
		result.data = ercd;
		BF_Debug_Print_Information(("IPCU_Set_OSD_config finish request. ercd = %d\n", ercd));
#ifndef CO_CMD_CT_OSD_FB_DISP
		//  Send a result
		BF_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_10, &ipcu_id_send);
		BF_IPCU_DISP_Send(ipcu_id_send, (UCHAR*)&result, sizeof(result), 1);
		BF_IPCU_Close(ipcu_id_send);
#endif//CO_CMD_CT_OSD_FB_DISP
		BF_Debug_Print_Information(("IPCU_Set_OSD_config send result. ercd = %d sequence_no = %d\n", result.data, result.sequence_no));
	}
	BF_IPCU_Close(ipcu_id_receive);
}

