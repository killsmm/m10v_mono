/**
 * @file		motor_calc_task_0.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2019 Socionext Inc.</I></B>
 */

//#pragma GCC optimize ("O0")

#include "share.h"
#include "share_data.h"
#include "user.h"
#include "motor_calc_task_0.h"
#include "camera_config.h"
#include "dd_tmr32.h"
#include "fj_std.h"

#include <string.h>

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
//#define D_FOCUS_DEBUG_PRINT_ON



/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define IS_FLGPTN_EQUAL(flgptn, FLG)	((((flgptn) & (FLG)) == (FLG)) ? TRUE : FALSE)


#define PWM_DATA_NUM_MAX		(30)


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/



/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static ULONG			g_step_target_usec_ch4_0[PWM_DATA_NUM_MAX][2];	// ["Entry"[0-29]]["cycle"[0] / "duty"[1]]
static FJ_PWM_STEP_DATA g_step_data_ch4[PWM_DATA_NUM_MAX];

static INT32 debug_pwm_convert_zero_duty(UCHAR *step_data_num, ULONG usec_array[][2]);

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void Motor_Calc_0_Task( INT32 stacd )
{
	OS_USER_FLGPTN	flgptn = 0;
	FJ_PERI_CH 		pwm_ch_4;
	FJ_ERR_CODE		err_code;
	static UCHAR	cnt=0;
	UCHAR			array_num = 0;
	
	pwm_ch_4	= FJ_PERI_CH_4; 	// WPPG4, PG6

	while(1)
	{
		// wait event
		OS_User_Twai_Flg( FID_MOTOR_CALC_0, FLG_MOTOR_CALC_0_WAITPTN, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );
		OS_User_Clr_Flg( FID_MOTOR_CALC_0, ~FLG_MOTOR_CALC_0_WAITPTN );
		
		if( IS_FLGPTN_EQUAL( flgptn, FLG_MOTOR_FINISHED_DMA_TRANS_CH4 ) ){
//			printf("\n**TEST**%s(%d)", __func__, __LINE__);
			memset(&g_step_target_usec_ch4_0, 0, sizeof(ULONG)*PWM_DATA_NUM_MAX*2);
			memset(&g_step_data_ch4, 0, sizeof(FJ_PWM_STEP_DATA)*PWM_DATA_NUM_MAX);

			//60fps
			g_step_target_usec_ch4_0[0][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[0][1]	= 4166; 	// usec, duty-0
			g_step_target_usec_ch4_0[1][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[1][1]	= 4168; 	// usec, duty-0
			g_step_target_usec_ch4_0[2][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[2][1]	= 4166; 	// usec, duty-0
			g_step_target_usec_ch4_0[3][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[3][1]	= 4168; 	// usec, duty-0
			g_step_target_usec_ch4_0[4][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[4][1]	= 4166; 	// usec, duty-0
			g_step_target_usec_ch4_0[5][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[5][1]	= 4168; 	// usec, duty-0
			g_step_target_usec_ch4_0[6][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[6][1]	= 4166; 	// usec, duty-0
			g_step_target_usec_ch4_0[7][0]	= 8334; 	// usec, cycle-0
			g_step_target_usec_ch4_0[7][1]	= 4168; 	// usec, duty-0
			g_step_target_usec_ch4_0[8][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[8][1]	= 4166; 	// usec, duty-0
			g_step_target_usec_ch4_0[9][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[9][1]	= 4168; 	// usec, duty-0
			g_step_target_usec_ch4_0[10][0] = 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[10][1] = 4166; 	// usec, duty-0
			g_step_target_usec_ch4_0[11][0] = 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[11][1] = 4168; 	// usec, duty-0
			g_step_target_usec_ch4_0[12][0] = 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[12][1] = 4166; 	// usec, duty-0
			g_step_target_usec_ch4_0[13][0] = 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[13][1] = 4168; 	// usec, duty-0
			g_step_target_usec_ch4_0[14][0] = 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[14][1] = 4166; 	// usec, duty-0
			g_step_target_usec_ch4_0[15][0] = 8334; 	// usec, cycle-0
			g_step_target_usec_ch4_0[15][1] = 4168; 	// usec, duty-0
			g_step_target_usec_ch4_0[16][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[16][1]	= 4166; 	// usec, duty-0
			g_step_target_usec_ch4_0[17][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[17][1]	= 4168; 	// usec, duty-0
			g_step_target_usec_ch4_0[18][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[18][1]	= 4166; 	// usec, duty-0
			g_step_target_usec_ch4_0[19][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[19][1]	= 4168; 	// usec, duty-0
			g_step_target_usec_ch4_0[20][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[20][1]	= 4166; 	// usec, duty-0
			g_step_target_usec_ch4_0[21][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[21][1]	= 4168; 	// usec, duty-0
			g_step_target_usec_ch4_0[22][0]	= 8332; 	// usec, cycle-0
			g_step_target_usec_ch4_0[22][1]	= 4166; 	// usec, duty-0
			g_step_target_usec_ch4_0[23][0]	= 8334; 	// usec, cycle-0
			g_step_target_usec_ch4_0[23][1]	= 4168; 	// usec, duty-0
			
			array_num = 24;
			err_code = debug_pwm_convert_zero_duty(&array_num, g_step_target_usec_ch4_0);
			UPRINTF_ERR(err_code);
			
			cnt++;
			err_code = FJ_PWM_Set_Buffer_Data_Window(pwm_ch_4, g_step_target_usec_ch4_0, &g_step_data_ch4[0], array_num, 0);
			UPRINTF_ERR(err_code);
		}
	}
}


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static INT32 debug_pwm_convert_zero_duty(UCHAR *step_data_num, ULONG usec_array[][2])
{
	INT32			ercd = 0;
	UCHAR			cnt = 0;
	UCHAR			find_zero_duty_flag = 0;
	UCHAR			array_num = 0;
	ULONG			tmp_array[2];	// [0]:cycle, [1]:duty
	static ULONG	tpm_usec_array[PWM_DATA_NUM_MAX][2];

	
	if(step_data_num == NULL){
		UPRINTF_ERR(-1);
		return ercd;
	}
	if(usec_array == NULL){
		UPRINTF_ERR(-1);
		return ercd;
	}

	
	for(cnt=0; cnt<(*step_data_num); cnt++){
		// search zero duty
		if(usec_array[cnt][1] == 0){
			find_zero_duty_flag = 1;
			break;
		}
	}

	if(find_zero_duty_flag == 1){
		memcpy((void*)&tpm_usec_array[0][0], (void*)&usec_array[0][0], sizeof(ULONG) * PWM_DATA_NUM_MAX * 2);
		memset((void*)&usec_array[0][0], 0, sizeof(ULONG) * PWM_DATA_NUM_MAX * 2);
		memset((void*)&tmp_array[0], 0, sizeof(ULONG) * 2);

		for(cnt=0; cnt<(*step_data_num); cnt++){
			tmp_array[0] += tpm_usec_array[cnt][0];	// cycle
			tmp_array[1] += tpm_usec_array[cnt][1];	// duty

			if(tmp_array[1] == 0){
				; //skip
			}
			else{
				usec_array[array_num][0] = tmp_array[0];
				usec_array[array_num][1] = tmp_array[1];
					
				memset((void*)&tmp_array[0], 0, sizeof(ULONG) * 2);
				array_num++;
			}
		}
		
		*step_data_num = array_num;
		
		return 0;
		
	}
	else{
		return 0;
	}
	
}



