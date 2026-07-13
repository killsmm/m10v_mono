/**
 * @file		af_task.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

//#pragma GCC optimize ("O0")

#include "share.h"
#include "share_data.h"
#include "user.h"
#include "af_task.h"
#include "camera_config.h"
#include "dd_tmr32.h"
#include "fj_memory.h"
#include "PDAF_ctrl.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
//#define D_FOCUS_DEBUG_PRINT_ON
//#define D_FOCUS_DEBUG_SAVE_PD_DATA
#ifdef D_FOCUS_DEBUG_SAVE_PD_DATA
//#define D_FOCUS_DEBUG_SAVE_PD_CENTER_FROM_SEPARATE	// Cut from separate LR data
//#define D_FOCUS_DEBUG_SAVE_PD_CENTER_FROM_MIX			// Cut from mixed LR data
#endif
#define D_CAF_TIMER_CH		(8)
#define D_CAF_INTERVAL_US	(5000000) // 5 sec interval time



#define SUB_RELATIVE(a,b) ( (a)>(b) ? ( (a)-(b) ): ( (b)-(a) ) )
/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	struct {
		USHORT	af_position[2];	// [1]=the max position, [0]=previous of the max position
		USHORT	af_step_max;
		USHORT	af_step_cnt;
	}af_info[E_CAMERA_ID_MAX];
} T_FOCUS_PARAM;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_SHARE_AF_PARAM g_share_af_param;
static T_SHARE_AF_DATA  g_share_af_data;
static T_SHARE_AF_FILTER g_share_af_filter;

// sample for IIR Filter Value
static const INT16 gAF_IIR_Filter_Value[45][3] = {
	// k        b1      b2              CutOff Freq
	{0x00F5,	0x7E17,	0x00EA},		//0.01
	{0x00EA,	0x7E2D,	0x00D6},		//0.02
	{0x00E0,	0x7E44,	0x00C4},		//0.03
	{0x00D6,	0x7E5A,	0x00B3},		//0.04
	{0x00CD,	0x7E70,	0x00A4},		//0.05
	{0x00C4,	0x7E86,	0x0096},		//0.06
	{0x00BB,	0x7E9C,	0x008A},		//0.07
	{0x00B3,	0x7EB1,	0x007E},		//0.08
	{0x00AB,	0x7EC6,	0x0073},		//0.09
	{0x00A4,	0x7EDB,	0x006A},		//0.1
	{0x009C,	0x7EF0,	0x0061},		//0.11
	{0x0095,	0x7F05,	0x0059},		//0.12
	{0x008E,	0x7F19,	0x0052},		//0.13
	{0x0088,	0x7F2D,	0x004B},		//0.14
	{0x0081,	0x7F41,	0x0046},		//0.15
	{0x007B,	0x7F54,	0x0041},		//0.16
	{0x0075,	0x7F68,	0x003C},		//0.17
	{0x006F,	0x7F7B,	0x0038},		//0.18
	{0x006A,	0x7F8E,	0x0035},		//0.19
	{0x0064,	0x7FA1,	0x0032},		//0.2
	{0x005F,	0x7FB4,	0x0030},		//0.21
	{0x005A,	0x7FC7,	0x002E},		//0.22
	{0x0055,	0x7FDA,	0x002D},		//0.23
	{0x0050,	0x7FED,	0x002C},		//0.24
	{0x004B,	0x0000,	0x002C},		//0.25
	{0x0046,	0x0013,	0x002C},		//0.26
	{0x0042,	0x0026,	0x002D},		//0.27
	{0x003D,	0x0039,	0x002E},		//0.28
	{0x0039,	0x004C,	0x0030},		//0.29
	{0x0035,	0x005F,	0x0032},		//0.3
	{0x0031,	0x0072,	0x0035},		//0.31
	{0x002D,	0x0085,	0x0038},		//0.32
	{0x0029,	0x0098,	0x003C},		//0.33
	{0x0025,	0x00AC,	0x0041},		//0.34
	{0x0022,	0x00BF,	0x0046},		//0.35
	{0x001E,	0x00D3,	0x004B},		//0.36
	{0x001B,	0x00E7,	0x0052},		//0.37
	{0x0017,	0x00FB,	0x0059},		//0.38
	{0x0014,	0x0110,	0x0061},		//0.39
	{0x0011,	0x0125,	0x006A},		//0.4
	{0x000E,	0x013A,	0x0073},		//0.41
	{0x000C,	0x014F,	0x007E},		//0.42
	{0x0009,	0x0164,	0x008A},		//0.43
	{0x0007,	0x017A,	0x0096},		//0.44
	{0x0005,	0x0190,	0x00A4},		//0.45
};
#ifdef D_FOCUS_DEBUG_SAVE_PD_DATA
static UCHAR gPDimage[1141056];
#ifdef D_FOCUS_DEBUG_SAVE_PD_CENTER_FROM_SEPARATE
static UCHAR gPdL[11648];
static UCHAR gPdR[11648];
#endif
#endif

static E_AF_STATE			g_af_state = E_AF_STATE_WAIT;
static E_AF_STATE			g_caf_state[E_CAMERA_ID_MAX] = {E_AF_STATE_WAIT, E_AF_STATE_WAIT, E_AF_STATE_WAIT, E_AF_STATE_WAIT};
static UCHAR				g_af_mode = AF_MODE_SINGLE;
//static E_CAMERA_FOCUS_MOVE_DIR	g_af_dir[E_CAMERA_ID_MAX] = {E_CAMERA_FOCUS_MOVE_DIR_FORWARD, E_CAMERA_FOCUS_MOVE_DIR_FORWARD, E_CAMERA_FOCUS_MOVE_DIR_FORWARD, E_CAMERA_FOCUS_MOVE_DIR_FORWARD};

static E_CAMERA_FOCUS_MOVE_DIR	g_af_dir[E_CAMERA_ID_MAX] = {E_CAMERA_FOCUS_MOVE_DIR_BACKWARD, E_CAMERA_FOCUS_MOVE_DIR_BACKWARD, E_CAMERA_FOCUS_MOVE_DIR_BACKWARD, E_CAMERA_FOCUS_MOVE_DIR_BACKWARD};
static BOOL		g_coarse_search[E_CAMERA_ID_MAX] = {FALSE, TRUE, TRUE, TRUE};

static T_AF_COUNTER	g_cnt[E_CAMERA_ID_MAX];
static T_AF_WORK	g_work[E_CAMERA_ID_MAX];
static E_CAMERA_FOCUS_DOF_DISTANCE	g_dof_macro_max = E_CAMERA_FOCUS_DOF_MAX;
static T_CAMERA_FOCUS_PARAM		g_in_lens[E_CAMERA_ID_MAX];
static unsigned int			g_af_wddata0_history[E_CAMERA_ID_MAX][15];
static unsigned int			g_af_wddata0_ave[E_CAMERA_ID_MAX];
static unsigned int			g_af_wddatav_history[E_CAMERA_ID_MAX][15];
static unsigned int			g_af_wddatav_ave[E_CAMERA_ID_MAX];
static unsigned int			g_af_afdatas_history[E_CAMERA_ID_MAX][15];
static unsigned int			g_af_afdatas_ave[E_CAMERA_ID_MAX];
static T_AF_THRESHOLD		g_thrs[E_CAMERA_ID_MAX];
static T_AF_DIRSEARCH		g_dir_search[E_CAMERA_ID_MAX];

//static E_CAMERA_FOCUS_MOVE_DIR	g_caf_dir[E_CAMERA_ID_MAX] = {E_CAMERA_FOCUS_MOVE_DIR_FORWARD, E_CAMERA_FOCUS_MOVE_DIR_FORWARD, E_CAMERA_FOCUS_MOVE_DIR_FORWARD, E_CAMERA_FOCUS_MOVE_DIR_FORWARD};
static E_CAMERA_FOCUS_MOVE_DIR	g_caf_dir[E_CAMERA_ID_MAX] = {E_CAMERA_FOCUS_MOVE_DIR_BACKWARD, E_CAMERA_FOCUS_MOVE_DIR_BACKWARD, E_CAMERA_FOCUS_MOVE_DIR_BACKWARD, E_CAMERA_FOCUS_MOVE_DIR_BACKWARD};

static BOOL		g_caf_start_trig = TRUE;
static ULLONG	g_caf_max_curr[E_CAMERA_ID_MAX][AF_DETECT_NUM_MAX] = {{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0}};
static UCHAR	g_caf_max_cnt[E_CAMERA_ID_MAX][AF_DETECT_NUM_MAX] = {{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0}};
static ULLONG	g_caf_max_curr_y[E_CAMERA_ID_MAX] = {0, 0, 0, 0};
static UCHAR	g_caf_max_cnt_y[E_CAMERA_ID_MAX] = {0, 0, 0, 0};

static T_FOCUS_PARAM	g_focus_param;
static BOOL		g_focus_bw_init[E_CAMERA_ID_MAX] = {FALSE, FALSE, FALSE, FALSE};
static BOOL		g_focus_fw_init[E_CAMERA_ID_MAX] = {FALSE, FALSE, FALSE, FALSE};
static USHORT	g_focus_value[E_CAMERA_ID_MAX] = {0, 0, 0, 0};


float g_af_continues_focus_sensitivity = 0.95f; /* 调焦调节系数 */
float g_af_continues_focus_sensitivity_near= 0.9f; /* 调焦调节系数 �? */
float g_af_continues_focus_sensitivity_far= 1.1f; /* 调焦调节系数 �? */

int g_af_max_cnt = 5;
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static INT32 AF_Rcv_Msg( T_AF_MSG* msg );
static void set_filter( const T_AF_PARAM* param );
static void set_mode( const T_AF_PARAM* param );
static void debug_printout( ULLONG frame_no );
static void single_af_init(void);
static void single_pdaf_proc( void );
static void single_af_proc( ULLONG frame_no );
static void simple_af( ULLONG frame_no, BOOL inited );
static void caf_init(E_CAMERA_ID id, E_SHARE_AF_WINDOW win, FLOAT start_um, CHAR dir);
static void af_search(E_CAMERA_ID id, E_SHARE_AF_WINDOW win);
static void caf_proc( ULLONG frame_no );
static void continues_af( ULLONG frame_no );
static void caf_pd_proc( void );
static void get_cafdata_by_win(E_CAMERA_ID id, E_SHARE_AF_WINDOW win);
static void get_afdata_by_win(E_CAMERA_ID id, E_SHARE_AF_WINDOW win, ULLONG* af_data);
static void get_afdata_sum(E_CAMERA_ID id, ULLONG* af_data);
static void caf_interval_timer_handler( void );
static void caf_interval_timer_start(ULONG usec);
static signed int lsm_set_inv_matrix( FLOAT mtx[][4] );
static void lsm_set_matrix( FLOAT mtx[][4], FLOAT* x, FLOAT* y, INT32 max_x );
static signed int Least_Square_Method( float* x, float* y, int max_x, float* result_para );
static int af_least_sq_method(E_CAMERA_ID focus_id, unsigned char startx, unsigned char peakx, unsigned char use_num, unsigned char use_wd );
static float next_lens_um_from_DoF(E_CAMERA_ID focus_id, float now_um, char dir);
static int af_data_calc_average(E_CAMERA_ID id, int target_cnt);
static int judge_stable(E_CAMERA_ID id, E_SHARE_AF_WINDOW win);
static void caf_set_threshold(E_CAMERA_ID id, E_SHARE_AF_WINDOW win);
static int my_abs(int x);

#ifdef D_FOCUS_DEBUG_SAVE_PD_DATA
static void debug_save_file( CHAR* fname, ULONG addr, ULONG size );
#endif

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void AF_Task( INT32 stacd )
{
	E_CAMERA_ID		focus_id = E_CAMERA_ID_0;
	T_AF_MSG		af;
	OS_USER_ER		ercd = 0;
	
	memset( (void*)&g_share_af_param, 0, sizeof(g_share_af_param) );
	memset( (void*)&g_focus_param, 0, sizeof(g_focus_param) );

	for(focus_id = E_CAMERA_ID_0; focus_id < E_CAMERA_ID_MAX; focus_id++){
		if(CAMERA.focus[focus_id].basic){
			if( CAMERA.focus[ focus_id ].Func_VCM_Init ){
				CAMERA.focus[ focus_id ].Func_VCM_Init( focus_id, &g_in_lens[focus_id] );
				// Initial Lens Position setting(1m)
				g_in_lens[focus_id].now_um = CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_100CM].lens_um;
				if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
					if( CAMERA.focus[ focus_id ].Func_VCM_CalcDAC ){
						CAMERA.focus[ focus_id ].Func_VCM_CalcDAC( focus_id, &g_in_lens[focus_id], D_FOCUS_LENS2DAC );
					}
					if( CAMERA.focus[ focus_id ].Func_VCM_SetDAC ){
						CAMERA.focus[ focus_id ].Func_VCM_SetDAC( focus_id, g_in_lens[focus_id].now_dac );
					}
				}
			}
		}
	}
	
	while (1) {
		ercd = AF_Rcv_Msg( &af );
		UPRINTF_ERR(ercd);
	/* zxl debug
		UPRINTF("af.msg = %d\ng_share_af_param = %f, %f, %f, %f\n", 
			af.msg, g_share_af_param.sensor[0].window[0].h_size_ratio,
			g_share_af_param.sensor[0].window[0].v_size_ratio,
			g_share_af_param.sensor[0].window[0].h_ofst_ratio,
			g_share_af_param.sensor[0].window[0].v_ofst_ratio);
	*/
		switch (af.msg) {
		case E_AF_MSG_MODE_CHG:
			set_mode( &af.param );
			Set_Share_AF_Param( &g_share_af_param );
			Send_Share_System_Event( E_SHARE_SYSTEM_EVENT_AF_UPDATE, 0 );
		break;
			
		case E_AF_MSG_CTRL_CHG:
			if(af.param.af_ctrl == E_SHARE_AF_CTRL_STOP){
				g_af_state = E_AF_STATE_WAIT;
			}else if((af.param.af_ctrl == E_SHARE_AF_CTRL_SINGLE_START) && (g_af_state == E_AF_STATE_WAIT)){
				g_af_state = E_AF_STATE_START;
				g_af_mode = AF_MODE_SINGLE;
			}else if((af.param.af_ctrl == E_SHARE_AF_CTRL_SIMPLE_START) && (g_af_state == E_AF_STATE_WAIT)){
				g_af_state = E_AF_STATE_START;
				g_af_mode = AF_MODE_SIMPLE;
			}else if((af.param.af_ctrl == E_SHARE_AF_CTRL_CAF_START) && (g_af_state == E_AF_STATE_WAIT)){
				g_af_state = E_AF_STATE_START;
				g_af_mode = AF_MODE_CAF;
			}else if((af.param.af_ctrl == E_SHARE_AF_CTRL_CONTI_START) && (g_af_state == E_AF_STATE_WAIT)){
				g_af_state = E_AF_STATE_START;
				g_af_mode = AF_MODE_CONTI;
				g_caf_start_trig = TRUE;
				memcpy((void*)&g_caf_dir, (void*)&g_af_dir, sizeof(g_af_dir));
				memset( (void*)&g_caf_max_curr, 0, sizeof(g_caf_max_curr) );
			}
		break;
				
		case E_AF_MSG_RANGE_CHG:
			if(g_af_state == E_AF_STATE_WAIT){
				for(focus_id = E_CAMERA_ID_0; focus_id < E_CAMERA_ID_MAX; focus_id++){
					if(CAMERA.focus[focus_id].basic){
						if(af.param.af_range == E_SHARE_AF_RANGE_MACRO){
							g_af_dir[focus_id] = E_CAMERA_FOCUS_MOVE_DIR_FORWARD;
							g_coarse_search[focus_id] = TRUE;
							if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
								if( CAMERA.focus[ focus_id ].Func_VCM_Move ){
									//printf("==========move start , E_SHARE_AF_RANGE_MACRO g_coarse_search[focus_id] =%d ===========\n", g_coarse_search[focus_id] );
									CAMERA.focus[ focus_id ].Func_VCM_Move( focus_id, E_CAMERA_FOCUS_MOVE_DIR_BACKWARD_INIT, TRUE, &g_focus_value[focus_id] );
									g_focus_fw_init[ focus_id ] = TRUE;
									//printf("==========move end===========\n");
								}
							}
						}else if(af.param.af_range == E_SHARE_AF_RANGE_INF){
								g_af_dir[focus_id] = E_CAMERA_FOCUS_MOVE_DIR_BACKWARD;
								g_coarse_search[focus_id] = TRUE;
								if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
									if( CAMERA.focus[ focus_id ].Func_VCM_Move ){
										//printf("==========move start , E_CAMERA_FOCUS_MOVE_DIR_BACKWARD g_coarse_search[focus_id] =%d ===========\n", g_coarse_search[focus_id] );
										CAMERA.focus[ focus_id ].Func_VCM_Move( focus_id, E_CAMERA_FOCUS_MOVE_DIR_FORWARD_INIT, TRUE, &g_focus_value[focus_id] );
										g_focus_bw_init[ focus_id ] = TRUE;
										//printf("==========move end===========\n");
									}
								}
						}else if(af.param.af_range == E_SHARE_AF_RANGE_FOCUS_INIT){
							if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
								if( CAMERA.focus[ focus_id ].Func_VCM_Init ){
									CAMERA.focus[ focus_id ].Func_VCM_Init( focus_id, &g_in_lens[focus_id] );
								}
							}
						}
					}
				}
			}
		break;
					
		case E_AF_MSG_STAT_RDY:
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AF, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "af stat"));
			
			Get_Frame_Share_AF_Data( &g_share_af_data, af.frame_no );

			if(g_af_state == E_AF_STATE_START){
				memset( (void*)&g_focus_param, 0, sizeof(g_focus_param) );
				memset(&g_share_af_data, 0 , sizeof(g_share_af_data));
				if( g_af_mode == AF_MODE_SINGLE){
					if(g_share_af_data.af_en == 0x01) single_af_init();
					g_af_state = E_AF_STATE_WAIT_WD;
				}else if(g_af_mode == AF_MODE_SIMPLE){
					
					simple_af( af.frame_no ,TRUE);
					for(focus_id = E_CAMERA_ID_0; focus_id < E_CAMERA_ID_MAX; focus_id++){
						if(CAMERA.focus[focus_id].basic){
							/* 获取当前步进位置， 通过当前位置对比左边 和右边 的距离， 找到最近的距离 做为初始化点 */
							//g_focus_value[focus_id] = CAMERA.focus[focus_id].basic->cur_focus_value;
							if ( CAMERA.focus[ focus_id ].Func_VCM_Get_Cur_Focus_Value )
							{
								CAMERA.focus[ focus_id ].Func_VCM_Get_Cur_Focus_Value(&g_focus_value[focus_id]);
							}
							if (SUB_RELATIVE( (int)CAMERA.focus[focus_id].basic->max_value, (int)g_focus_value[focus_id] ) >  SUB_RELATIVE( (int)CAMERA.focus[focus_id].basic->mini_value, (int)g_focus_value[focus_id] ))
							{
								g_af_dir[focus_id] = E_CAMERA_FOCUS_MOVE_DIR_FORWARD;
							}
							else
							{
								g_af_dir[focus_id] = E_CAMERA_FOCUS_MOVE_DIR_BACKWARD;
							}
							if(g_af_dir[focus_id] == E_CAMERA_FOCUS_MOVE_DIR_FORWARD){
								g_coarse_search[focus_id] = TRUE;
								g_focus_param.af_info[ focus_id ].af_step_max = CAMERA.focus[focus_id].basic->coarse_fw_step_num;
								//if(g_focus_fw_init[ focus_id ] == FALSE)
								
								{
									if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
										if( CAMERA.focus[ focus_id ].Func_VCM_Move ){
											//printf("==========move start , E_CAMERA_FOCUS_MOVE_DIR_FORWARD g_coarse_search[focus_id] =%d ===========\n", g_coarse_search[focus_id] );
											CAMERA.focus[ focus_id ].Func_VCM_Move( focus_id, E_CAMERA_FOCUS_MOVE_DIR_FORWARD_INIT, TRUE, &g_focus_value[focus_id] );
											g_focus_fw_init[ focus_id ] = TRUE;
										}
									}
								}
							}else if(g_af_dir[focus_id] == E_CAMERA_FOCUS_MOVE_DIR_BACKWARD){
								g_coarse_search[focus_id] = TRUE;
								g_focus_param.af_info[ focus_id ].af_step_max = CAMERA.focus[focus_id].basic->coarse_bw_step_num;
								//if(g_focus_bw_init[ focus_id ] == FALSE)
								{
									if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
										if( CAMERA.focus[ focus_id ].Func_VCM_Move ){
											//printf("==========move start , E_CAMERA_FOCUS_MOVE_DIR_BACKWARD g_coarse_search[focus_id] =%d ===========\n", g_coarse_search[focus_id] );
											CAMERA.focus[ focus_id ].Func_VCM_Move( focus_id, E_CAMERA_FOCUS_MOVE_DIR_BACKWARD_INIT, TRUE, &g_focus_value[focus_id] );
											g_focus_bw_init[ focus_id ] = TRUE;
										}
									}
								}
							}
						}
					}
					g_af_state = E_AF_STATE_WAIT_WD;
				}else	if(g_af_mode == AF_MODE_CAF){
					for(focus_id = E_CAMERA_ID_0; focus_id < E_CAMERA_ID_MAX; focus_id++){
						memset( (void*)&g_cnt[focus_id], 0, sizeof(T_AF_COUNTER) );
						memset( (void*)&g_thrs[focus_id], 0, sizeof(T_AF_THRESHOLD) );
						memset( (void*)&g_dir_search[focus_id], 0, sizeof(T_AF_DIRSEARCH) );
						if(CAMERA.focus[focus_id].dof){
							if( g_af_dir[focus_id] == E_CAMERA_FOCUS_MOVE_DIR_FORWARD){
								g_dir_search[focus_id].dir = DIR_INF2MAC;
								caf_init(focus_id, 0, CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_1000CM].lens_um, DIR_INF2MAC);
							}else{
								g_dir_search[focus_id].dir = DIR_MAC2INF;
								caf_init(focus_id, 0, CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_8CM].lens_um, DIR_MAC2INF);
							}
							g_cnt[focus_id].wd++;
							if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
								if( CAMERA.focus[ focus_id ].Func_VCM_CalcDAC ){
									g_in_lens[focus_id].now_um = g_work[focus_id].target_pos_um;
									CAMERA.focus[ focus_id ].Func_VCM_CalcDAC( focus_id, &g_in_lens[focus_id], D_FOCUS_LENS2DAC );
								}
								if( CAMERA.focus[ focus_id ].Func_VCM_SetDAC ){
									CAMERA.focus[ focus_id ].Func_VCM_SetDAC( focus_id, g_in_lens[focus_id].now_dac );
								}
							}
							g_caf_state[focus_id] = E_CAF_STATE_DETECTING_TRG;
							g_work[focus_id].ope  = AF_OPE_SEARCH;
						}
					}
				
					g_af_state = E_CAF_STATE_DETECTING_TRG;
				}else if(g_af_mode == AF_MODE_CONTI){
					memset( (void*)&g_focus_param, 0, sizeof(g_focus_param) );
					for(focus_id = E_CAMERA_ID_0; focus_id < E_CAMERA_ID_MAX; focus_id++){
						if(CAMERA.focus[focus_id].basic){
							if(g_af_dir[focus_id] == E_CAMERA_FOCUS_MOVE_DIR_FORWARD){
								g_coarse_search[focus_id] = TRUE;
								g_focus_param.af_info[ focus_id ].af_step_max = CAMERA.focus[focus_id].basic->coarse_fw_step_num;
							}else if(g_af_dir[focus_id] == E_CAMERA_FOCUS_MOVE_DIR_BACKWARD){
								g_coarse_search[focus_id] = TRUE;
								g_focus_param.af_info[ focus_id ].af_step_max = CAMERA.focus[focus_id].basic->coarse_bw_step_num;
							}
						}
					}
					g_af_state = E_CAF_STATE_DETECTING_TRG;
				}
			}else if(g_af_state == E_AF_STATE_WAIT_WD){
				if(g_af_mode == AF_MODE_SINGLE){
					// single AF
					switch(g_share_af_data.af_en & 0x03)
					{
						case AF_PROC_PD:
							single_pdaf_proc();
						break;
						case AF_PROC_HYBRID:
						break;
						case AF_PROC_CONTRAST:
							single_af_proc( af.frame_no );
						default:
						break;
					}
				}else if(g_af_mode == AF_MODE_SIMPLE){
					// simple AF
					switch(g_share_af_data.af_en & 0x03)
					{
						case AF_PROC_PD:
						break;
						case AF_PROC_HYBRID:
						break;
						case AF_PROC_CONTRAST:
						default:
							simple_af( af.frame_no ,FALSE);
						break;
					}
				}

				if(g_af_state == E_AF_STATE_WAIT){
					Snd_Event_to_User( E_USER_EVENT_AF_RELEASE, 0 );
				}
			}else if(g_af_state == E_CAF_STATE_DETECTING_TRG){
				if(g_af_mode == AF_MODE_CAF){
					// CAF
					switch(g_share_af_data.af_en & 0x03)
					{
						case AF_PROC_PD:
							caf_pd_proc();
						break;
						case AF_PROC_HYBRID:
						break;
						case AF_PROC_CONTRAST:
						default:
							caf_proc( af.frame_no );
						break;
					}
				}else if(g_af_mode == AF_MODE_CONTI){
					// Continues AF
					switch(g_share_af_data.af_en & 0x03)
					{
						case AF_PROC_PD:
						break;
						case AF_PROC_HYBRID:
						break;
						case AF_PROC_CONTRAST:
						default:
							continues_af( af.frame_no );
						break;
					}
				}
			}else{
				debug_printout( af.frame_no );
			}

			////////////////////Testing_to_Save_PD_data Start///////////////////////////////////////
#ifdef D_FOCUS_DEBUG_SAVE_PD_DATA
			if(g_share_af_data.af_en & 0x02) {
				static UCHAR d_frame_cnt = 0;
				UINT32 hcnt, vcnt;
				USHORT *src;
#if defined(D_FOCUS_DEBUG_SAVE_PD_CENTER_FROM_SEPARATE)
				UCHAR *src_cut;
				UINT32 cut_x, cut_y, offset_x, offset_y;
#elif defined(D_FOCUS_DEBUG_SAVE_PD_CENTER_FROM_MIX)
				UINT32 cut_x, cut_y, offset_x, offset_y;
#endif
				UCHAR *dst;
				CHAR fname[32];
				ULONG size = 0;
				
			
				if(d_frame_cnt < 60){
					d_frame_cnt++;
				}
				else if( d_frame_cnt == 60){
					d_frame_cnt++;
					printf( "-------- AF Func: PDAF(Dual PD) -------- \n");
					printf( "  IMAGE_DATA:\n");
					printf( "  global width(%d) width(%d) height(%d)\n",
									(UINT32)(g_share_af_data.pdaf.IMAGE_DATA.g_width),
									(UINT32)(g_share_af_data.pdaf.IMAGE_DATA.width),
									(UINT32)(g_share_af_data.pdaf.IMAGE_DATA.height)
									);
					printf( "  DUAL_PD_DATA:\n");
					printf( "  global width(%d) width(%d) height(%d) address(0x%x)\n",
									(UINT32)(g_share_af_data.pdaf.DUAL_PD_DATA.g_width),
									(UINT32)(g_share_af_data.pdaf.DUAL_PD_DATA.width),
									(UINT32)(g_share_af_data.pdaf.DUAL_PD_DATA.height),
									(UINT32)(g_share_af_data.pdaf.DUAL_PD_DATA.addr)
									);
					FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_FLUSH,
						  (UINT32)(g_share_af_data.pdaf.DUAL_PD_DATA.addr), 4582336);
					
					size = g_share_af_data.pdaf.DUAL_PD_DATA.g_width * g_share_af_data.pdaf.DUAL_PD_DATA.height;
					sprintf( fname, "I:\\PD_DATA_%d.raw", 0 );
					debug_save_file(fname, g_share_af_data.pdaf.DUAL_PD_DATA.addr, size);
					UPRINTF("Size: %d x %d (Y Only)\n", g_share_af_data.pdaf.DUAL_PD_DATA.width, g_share_af_data.pdaf.DUAL_PD_DATA.height);

					src = (USHORT*)g_share_af_data.pdaf.DUAL_PD_DATA.addr;
					dst = gPDimage;
					for( vcnt=0; vcnt<g_share_af_data.pdaf.DUAL_PD_DATA.height; vcnt++ ){
						for( hcnt=0; hcnt<(g_share_af_data.pdaf.DUAL_PD_DATA.width>>1); hcnt++ ){
							*dst = (UCHAR)(*src>>2);
							src+=2;
							dst++;
						}
						src += ( g_share_af_data.pdaf.DUAL_PD_DATA.g_width - g_share_af_data.pdaf.DUAL_PD_DATA.width*2 );
					}
					debug_save_file((CHAR *)"I:\\PD_DATA_L.raw", (ULONG)gPDimage, g_share_af_data.pdaf.DUAL_PD_DATA.width*g_share_af_data.pdaf.DUAL_PD_DATA.height>>1 );
					UPRINTF("Size: %d x %d (Ldata)\n", g_share_af_data.pdaf.DUAL_PD_DATA.width>>1, g_share_af_data.pdaf.DUAL_PD_DATA.height);

#ifdef D_FOCUS_DEBUG_SAVE_PD_CENTER_FROM_SEPARATE
					cut_x = 208;
					cut_y = 56;
					offset_x = (( (g_share_af_data.pdaf.DUAL_PD_DATA.width>>1) - cut_x ) >> 1)&0xFFFFFFE;
					offset_y = ( g_share_af_data.pdaf.DUAL_PD_DATA.height - cut_y ) >> 1;
					
					src_cut = (UCHAR*)gPDimage + (g_share_af_data.pdaf.DUAL_PD_DATA.width>>1) * offset_y + offset_x;
					dst = gPdL;
					
					for( vcnt=0; vcnt<cut_y; vcnt++ ){
						for( hcnt=0; hcnt<cut_x; hcnt++ ){
							*dst = *src_cut;
							src_cut++;
							dst++;
						}
						src_cut += ( (g_share_af_data.pdaf.DUAL_PD_DATA.width>>1) - cut_x );
					}
					
					debug_save_file((CHAR *)"I:\\PD_DATA_L_center.raw", (ULONG)gPdL, cut_x*cut_y );
					UPRINTF("Size: %d x %d (Ldata)\n", cut_x, cut_y);				
#endif
					src = (USHORT*)g_share_af_data.pdaf.DUAL_PD_DATA.addr + 1;
					dst = gPDimage;
					for( vcnt=0; vcnt<g_share_af_data.pdaf.DUAL_PD_DATA.height; vcnt++ ){
						for( hcnt=0; hcnt<(g_share_af_data.pdaf.DUAL_PD_DATA.width>>1); hcnt++ ){
							*dst = (UCHAR)(*src>>2);
							src+=2;
							dst++;
						}
						src += ( g_share_af_data.pdaf.DUAL_PD_DATA.g_width - g_share_af_data.pdaf.DUAL_PD_DATA.width*2 );
					}
					debug_save_file((CHAR *)"I:\\PD_DATA_R.raw", (ULONG)gPDimage, g_share_af_data.pdaf.DUAL_PD_DATA.width*g_share_af_data.pdaf.DUAL_PD_DATA.height>>1 );
					UPRINTF("Size: %d x %d (Rdata)\n", g_share_af_data.pdaf.DUAL_PD_DATA.width>>1, g_share_af_data.pdaf.DUAL_PD_DATA.height);
					
#ifdef D_FOCUS_DEBUG_SAVE_PD_CENTER_FROM_SEPARATE
					src_cut = (UCHAR*)gPDimage + (g_share_af_data.pdaf.DUAL_PD_DATA.width>>1) * offset_y + offset_x;
					dst = gPdR;
					
					for( vcnt=0; vcnt<cut_y; vcnt++ ){
						for( hcnt=0; hcnt<cut_x; hcnt++ ){
							*dst = *src_cut;
							src_cut++;
							dst++;
						}
						src_cut += ( (g_share_af_data.pdaf.DUAL_PD_DATA.width>>1) - cut_x );
					}
					
					debug_save_file((CHAR *)"I:\\PD_DATA_R_center.raw", (ULONG)gPdR, cut_x*cut_y );
					UPRINTF("Size: %d x %d (Ldata)\n", cut_x, cut_y);
#endif

#ifdef D_FOCUS_DEBUG_SAVE_PD_CENTER_FROM_MIX
					cut_x = 208;
					cut_y = 56;
					offset_x = ( (g_share_af_data.pdaf.DUAL_PD_DATA.width>>1) - cut_x ) >> 1;
					offset_y = ( g_share_af_data.pdaf.DUAL_PD_DATA.height - cut_y ) >> 1;
					
					src = (USHORT*)g_share_af_data.pdaf.DUAL_PD_DATA.addr + g_share_af_data.pdaf.DUAL_PD_DATA.width * offset_y + offset_x*2;
					dst = gPDimage;
					
					for( vcnt=0; vcnt<cut_y; vcnt++ ){
						for( hcnt=0; hcnt<cut_x; hcnt++ ){
							*dst = (UCHAR)(*src>>2);
							src+=2;
							dst++;
						}
						src += ( g_share_af_data.pdaf.DUAL_PD_DATA.width - cut_x*2 );
					}
					
					debug_save_file((CHAR *)"I:\\PD_DATA_L_center.raw", (ULONG)gPDimage, cut_x*cut_y );
					UPRINTF("Size: %d x %d (Ldata)\n", cut_x, cut_y);
				
					src = (USHORT*)g_share_af_data.pdaf.DUAL_PD_DATA.addr + g_share_af_data.pdaf.DUAL_PD_DATA.width * offset_y + offset_x*2 + 1;
					dst = gPDimage;
					
					for( vcnt=0; vcnt<cut_y; vcnt++ ){
						for( hcnt=0; hcnt<cut_x; hcnt++ ){
							*dst = (UCHAR)(*src>>2);
							src+=2;
							dst++;
						}
						src += ( g_share_af_data.pdaf.DUAL_PD_DATA.width - cut_x*2 );
					}
					
					debug_save_file((CHAR *)"I:\\PD_DATA_R_center.raw", (ULONG)gPDimage, cut_x*cut_y );
					UPRINTF("Size: %d x %d (Ldata)\n", cut_x, cut_y);
#endif
				}
			}
#endif
			////////////////////Testing_to_Save_PD_data End///////////////////////////////////////
			
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AF, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "af stat"));
		break;
			
		default:
		break;
		}
	}
}

USHORT AF_Get_Current_Focus_Value(int focus_id)
{
	return g_focus_value[focus_id];
}

void AF_Set_Before_Config( const T_AF_PARAM* param )
{
	set_filter( param ); // don't change during view
	set_mode( param );
	Set_Share_AF_Param( &g_share_af_param );
	//Send_Share_System_Event( E_SHARE_SYSTEM_EVENT_AF_UPDATE, 0 ); // no need at before config
}


INT32 Snd_Msg_To_AF(const T_AF_MSG* msg)
{
	OS_USER_ER	ercd = 0;
	T_AF_MBOX	*mbox = NULL;

	ercd = OS_User_Get_Mpf( MPFID_AF, (OS_USER_VP*)&mbox );
	UPRINTF_ERR(ercd);

	mbox->blf_addr = mbox;

	memcpy( mbox->msg, msg, sizeof(T_AF_MSG) );

	ercd = OS_User_Snd_Mbx( MID_AF, (T_OS_USER_MSG*)mbox );
	UPRINTF_ERR(ercd);

	return (ercd);
}

VOID AF_Set_Lens_Pos( E_CAMERA_ID focus_id , USHORT um )
{
	if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
		if( CAMERA.focus[ focus_id ].Func_VCM_CalcDAC ){
			g_in_lens[focus_id].now_um = (FLOAT)um;
			CAMERA.focus[ focus_id ].Func_VCM_CalcDAC( focus_id, &g_in_lens[focus_id], D_FOCUS_LENS2DAC );
		}
		if( CAMERA.focus[ focus_id ].Func_VCM_SetDAC ){
			CAMERA.focus[ focus_id ].Func_VCM_SetDAC( focus_id, g_in_lens[focus_id].now_dac );
		}
	}
	printf(" Lens Position:%f(%d)\n",g_in_lens[focus_id].now_um, g_in_lens[focus_id].now_dac );

}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static INT32 AF_Rcv_Msg( T_AF_MSG* msg )
{
	OS_USER_ER	ercd = 0;
	T_AF_MBOX	*mbox = NULL;
	
	/* [mbox] */
	ercd = OS_User_Rcv_Mbx( MID_AF, (T_OS_USER_MSG**)&mbox );
	if( ercd != D_OS_USER_E_OK ){
		UPRINTF_ERR(ercd);
		return ( ercd );
	}

	memcpy( msg, mbox->msg, sizeof(T_AF_MSG) );

	ercd = OS_User_Rel_Mpf( MPFID_AF, (OS_USER_VP)mbox->blf_addr );
	UPRINTF_ERR(ercd);
	return (ercd);
}

static void set_filter( const T_AF_PARAM* param )
{
	INT32 freq = 10;
	
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].r					= 0x134;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].gr				= 0x12c;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].gb				= 0x12c;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].b					= 0x74;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].median_enable		= 0;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_enable		= 1;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_boundary[0]	= 169;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_boundary[1]	= 776;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_boundary[2]	= 1893;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_boundary[3]	= 3566;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_boundary[4]	= 5825;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_boundary[5]	= 8700;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_boundary[6]	= 12213;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_slope[0]		= 776;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_slope[1]		= 216;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_slope[2]		= 117;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_slope[3]		= 78;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_slope[4]		= 58;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_slope[5]		= 46;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_slope[6]		= 37;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_slope[7]		= 31;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_offset[0]	= 0;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_offset[1]	= 513;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_offset[2]	= 1026;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_offset[3]	= 1537;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_offset[4]	= 2047;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_offset[5]	= 2559;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_offset[6]	= 3076;
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0].knee_offset[7]	= 3584;
	// copy
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_1] = g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0];
	g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_2] = g_share_af_filter.eval_calc[E_SHARE_AF_FILTER_UNIT_0];
	
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.iir_enable			= 1;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.fir_enable			= 0;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.input_select		= E_IM_PRO_AF_TAP_0;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.fir_coefficient[0]	= 0x00;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.fir_coefficient[1]	= 0x00;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.fir_coefficient[2]	= 0x40;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.fir_coefficient[3]	= 0x00;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.fir_coefficient[4]	= 0x00;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.fir_level			= 0;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.gain				= 0x40;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.iir_k				= gAF_IIR_Filter_Value[freq][0];
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.iir_a0				= 0x20;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.iir_a1				= 0xC0;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.iir_a2				= 0x20;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.iir_b1				= gAF_IIR_Filter_Value[freq][1];
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.iir_b2				= gAF_IIR_Filter_Value[freq][2];
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.coring_bounder[0]	= 0;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.coring_bounder[1]	= 0;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.coring_bounder[2]	= 0;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.coring_slope[0]		= 0;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.coring_slope[1]		= 0;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.coring_slope[2]		= 0;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.coring_slope[3]		= 0;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.coring_offset[0]	= 0;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.coring_offset[1]	= 0;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.coring_offset[2]	= 0;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.coring_offset[3]	= 0;
	// copy
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_b = g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_1].group_a = g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_1].group_b = g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_2].group_a = g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a;
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_2].group_b = g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a;
	// change
	g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_b.input_select		= E_IM_PRO_AF_TAP_2;
	
	
	g_share_af_filter.vertical.update_cycle		= 0; // not used
	g_share_af_filter.vertical.eval_cal_unit	= E_IM_PRO_AF_TAP_0;
	g_share_af_filter.vertical.start_addr		= 0; // not used
	g_share_af_filter.vertical.horizon_size		= 0; // not used
	g_share_af_filter.vertical.iir_k			= g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.iir_k;
	g_share_af_filter.vertical.iir_a0			= g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.iir_a0;
	g_share_af_filter.vertical.iir_a1			= g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.iir_a1;
	g_share_af_filter.vertical.iir_a2			= g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.iir_a2;
	g_share_af_filter.vertical.iir_b1			= g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.iir_b1;
	g_share_af_filter.vertical.iir_b2			= g_share_af_filter.horizon[E_SHARE_AF_FILTER_UNIT_0].group_a.iir_b2;
	g_share_af_filter.vertical.cor_threshold	= 0;
	g_share_af_filter.vertical.cor_level		= 0;
	g_share_af_filter.vertical.fir_filter[0]	= 0x40;
	g_share_af_filter.vertical.fir_filter[1]	= 0;
	g_share_af_filter.vertical.fir_filter[2]	= 0;
}

static void set_mode( const T_AF_PARAM* param )
{
	//UPRINTF("Change AF Mode\n");
	
	// Enable
	g_share_af_param.sensor[0].window[0].enable = 1;
	// Size
#if 0
	g_share_af_param.sensor[0].window[0].h_size_ratio = 0.4; // 40%
	g_share_af_param.sensor[0].window[0].v_size_ratio = 0.4; // 40%
	g_share_af_param.sensor[0].window[0].h_ofst_ratio = (1.0 - g_share_af_param.sensor[0].window[0].h_size_ratio) / 2; // center
	g_share_af_param.sensor[0].window[0].v_ofst_ratio = (1.0 - g_share_af_param.sensor[0].window[0].v_size_ratio) / 2; // center
	
	g_share_af_param.sensor[0].window[0].h_ofst_ratio += (0.1*param->af_mode); // debug;
	g_share_af_param.sensor[0].window[0].v_ofst_ratio += (0.1*param->af_mode); // debug;
#else
	g_share_af_param.sensor[0].window[0].h_size_ratio = param->roi_af_x_size;
	g_share_af_param.sensor[0].window[0].v_size_ratio = param->roi_af_y_size;
	g_share_af_param.sensor[0].window[0].h_ofst_ratio = param->roi_af_x_sta;
	g_share_af_param.sensor[0].window[0].v_ofst_ratio = param->roi_af_y_sta;
#endif
	// Filter
	g_share_af_param.sensor[0].window[0].h_sel0				= E_IM_PRO_AF_WIN_H0_A;
	g_share_af_param.sensor[0].window[0].h_sel1				= E_IM_PRO_AF_WIN_H0_A;
	g_share_af_param.sensor[0].window[0].h_sel2				= E_IM_PRO_AF_WIN_H0_A;
	g_share_af_param.sensor[0].window[0].eval_cal_unit		= E_IM_PRO_AF_EVAL_CALC_UNIT_0;
	g_share_af_param.sensor[0].window[0].bright_threshold	= 4095;
	g_share_af_param.sensor[0].window[0].dark_threshold		= 0;
	g_share_af_param.sensor[0].window[0].max_mix_select		= E_IM_PRO_AF_WIN_H0_A;
	g_share_af_param.sensor[0].filter = &g_share_af_filter;
	
	g_share_af_param.sensor[1].window[0] = g_share_af_param.sensor[0].window[0];
	g_share_af_param.sensor[2].window[0] = g_share_af_param.sensor[0].window[0];
	g_share_af_param.sensor[3].window[0] = g_share_af_param.sensor[0].window[0];
}

static void debug_printout( ULLONG frame_no )
{
#ifdef D_FOCUS_DEBUG_PRINT_ON
	static int debug_interval = 0;
	int sensor_id, window_id, data_id;
	
	if( debug_interval ++ >= 100 ){
		debug_interval = 0;
		
		printf ("-------- AF STAT frame (%d) -------- \n", (INT32)frame_no );
		
		if(g_share_af_data.af_en & 0x01) {
			printf ("-------- AF Func: Contrast AF -------- \n");
			for( sensor_id = 0; sensor_id < E_SHARE_SENSOR_ID_MAX; sensor_id ++ ){
				
				for( window_id = 0; window_id < E_SHARE_AF_WINDOW_MAX; window_id ++ ){
					
					if( g_share_af_data.sensor[sensor_id].window[window_id].enable ){
						
						printf( " Sensor(%d) Window(%d)\n", sensor_id, window_id );
						
						for( data_id = 0; data_id < E_SHARE_AF_DATA_MAX; data_id ++ ){
							printf( "  data[%02d] 0x%08x%08x\n", data_id,
											(UINT32)(g_share_af_data.sensor[sensor_id].window[window_id].data[data_id]>>32),
											(UINT32)(g_share_af_data.sensor[sensor_id].window[window_id].data[data_id])
											);
						}
						
						printf( "\n" );
					}
				}
				
			}
		}

		if(g_share_af_data.af_en & 0x02) {
			printf( "-------- AF Func: PDAF(Dual PD) -------- \n");
			printf( "  IMAGE_DATA:\n");
			printf( "  global width(%d) width(%d) height(%d)\n",
							(UINT32)(g_share_af_data.pdaf.IMAGE_DATA.g_width),
							(UINT32)(g_share_af_data.pdaf.IMAGE_DATA.width),
							(UINT32)(g_share_af_data.pdaf.IMAGE_DATA.height)
							);
			printf( "  DUAL_PD_DATA:\n");
			printf( "  global width(%d) width(%d) height(%d) address(0x%x)\n",
							(UINT32)(g_share_af_data.pdaf.DUAL_PD_DATA.g_width),
							(UINT32)(g_share_af_data.pdaf.DUAL_PD_DATA.width),
							(UINT32)(g_share_af_data.pdaf.DUAL_PD_DATA.height),
							(UINT32)(g_share_af_data.pdaf.DUAL_PD_DATA.addr)
							);
		}else if(g_share_af_data.af_en & 0x04) {
			printf( "-------- AF Func: PDAF(PD) -------- \n");
			printf( "  IMAGE_DATA:\n");
			printf( "  global width(%d) width(%d) height(%d)\n",
							(UINT32)(g_share_af_data.pdaf.IMAGE_DATA.g_width),
							(UINT32)(g_share_af_data.pdaf.IMAGE_DATA.width),
							(UINT32)(g_share_af_data.pdaf.IMAGE_DATA.height)
							);
			printf( "  PD_DATA:\n");
			printf( "  sensor mode(%d) global cnt(%d) h cnt(%d) v cnt(%d) address(0x%x)\n",
							(UINT32)(g_share_af_data.pdaf.PD_DATA.sensor_mode),
							(UINT32)(g_share_af_data.pdaf.PD_DATA.blk_g_cnt),
							(UINT32)(g_share_af_data.pdaf.PD_DATA.blk_h_cnt),
							(UINT32)(g_share_af_data.pdaf.PD_DATA.blk_v_cnt),
							(UINT32)(g_share_af_data.pdaf.PD_DATA.addr)
							);

			// sample to get pd infomraiton from center block
			{
				USHORT h_cnt, v_cnt;
				USHORT center_sta_h_cnt, center_sta_v_cnt, center_h_cnt, center_v_cnt;
				USHORT pd_num;
				T_CAMERA_PD_DATA pd_info;

				center_h_cnt = g_share_af_data.pdaf.PD_DATA.blk_h_cnt / 4;
				center_v_cnt = g_share_af_data.pdaf.PD_DATA.blk_v_cnt / 4;
				center_sta_h_cnt = (g_share_af_data.pdaf.PD_DATA.blk_h_cnt - center_h_cnt) / 2;
				center_sta_v_cnt = (g_share_af_data.pdaf.PD_DATA.blk_v_cnt - center_v_cnt) / 2;

				for(v_cnt = center_sta_v_cnt; v_cnt < (center_sta_v_cnt + center_v_cnt); v_cnt++){
					for(h_cnt = center_sta_h_cnt; h_cnt < (center_sta_h_cnt + center_h_cnt); h_cnt++){
						if( CAMERA.sensor[0].Func_Get_PdInfo) {
							pd_num = h_cnt + v_cnt * g_share_af_data.pdaf.PD_DATA.blk_g_cnt;
							CAMERA.sensor[0].Func_Get_PdInfo(0, g_share_af_data.pdaf.PD_DATA.sensor_mode, g_share_af_data.pdaf.PD_DATA.addr, pd_num, &pd_info);
							printf( "PD%03d(%02d, %02d): C(%f) P(%f)\n", pd_num, h_cnt, v_cnt, pd_info.level, pd_info.pd);
						}
					}
				}
			}
		}
	}
#endif
}

static void single_af_init(void)
{
	UCHAR	detect_num;
	INT32	cnt;
	E_CAMERA_ID		focus_id = E_CAMERA_ID_0;

	for(focus_id = E_CAMERA_ID_0; focus_id < E_CAMERA_ID_MAX; focus_id++){
		if(CAMERA.focus[focus_id].dof){

			for(cnt=0; cnt<g_dof_macro_max; cnt++){
				for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num++){
					g_work[focus_id].roi.af_data[detect_num][cnt] = 0x00000000;
				}
				g_work[focus_id].lens_um[cnt] = 0.0;
			}
			for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num++){
				g_work[focus_id].roi.af_2up2down_data[detect_num]= 0x00000000;
				g_work[focus_id].roi.af_2up1down_data[detect_num]= 0x00000000;
				g_work[focus_id].roi.af_1up2down_data[detect_num]= 0x00000000;
				g_work[focus_id].roi.af_1up1down_data[detect_num]= 0x00000000;
				g_work[focus_id].roi.af_start_down_data[detect_num]= 0x00000000;
				g_work[focus_id].roi.af_last_up_data[detect_num]= 0x00000000;
				
				g_work[focus_id].roi.af_max_data[detect_num] = 0x00000000;
				g_work[focus_id].roi.af_min_data[detect_num] = 0xFFFFFFFF;
				
				g_work[focus_id].roi.af_2up2down_step[detect_num]= 0x00;
				g_work[focus_id].roi.af_2up1down_step[detect_num]= 0x00;
				g_work[focus_id].roi.af_1up2down_step[detect_num]= 0x00;
				g_work[focus_id].roi.af_1up1down_step[detect_num]= 0x00;
				g_work[focus_id].roi.af_start_down_step[detect_num]= 0x00;
				g_work[focus_id].roi.af_last_up_step[detect_num]= 0x00;
				
				g_work[focus_id].roi.af_max_step[detect_num] = 0x00;
				g_work[focus_id].roi.af_min_step[detect_num] = 0x00;
				
				g_work[focus_id].roi.af_find_2up2down[detect_num] = OFF;
				g_work[focus_id].roi.af_find_2up1down[detect_num] = OFF;
				g_work[focus_id].roi.af_find_1up2down[detect_num] = OFF;
				g_work[focus_id].roi.af_find_1up1down[detect_num] = OFF;
				g_work[focus_id].roi.af_find_start_down[detect_num] = OFF;
				g_work[focus_id].roi.af_find_last_up[detect_num] = OFF;	
				
				g_work[focus_id].roi.af_start_down_cnt[detect_num] = 0;
				g_work[focus_id].roi.af_last_up_cnt[detect_num] = 0;
				g_work[focus_id].roi.af_down_cnt[detect_num] = 0;
			}
			g_work[focus_id].roi.af_roi_calc_peak_um = 0;
			g_work[focus_id].find_peak = OFF;
			
			g_cnt[focus_id].vd = 0;
			g_cnt[focus_id].wd = 0;
			g_cnt[focus_id].stable_cnt = 0;

			switch( g_af_dir[focus_id] ){
			case E_CAMERA_FOCUS_MOVE_DIR_FORWARD:
				g_work[focus_id].dir = E_CAMERA_FOCUS_MOVE_DIR_FORWARD;
				g_work[focus_id].start_um = CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_1000CM].lens_um - 20.0;
				g_work[focus_id].last_um = CAMERA.focus[focus_id].dof[g_dof_macro_max-1].lens_um;
				g_work[focus_id].fail_um = CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_100CM].lens_um;
				g_work[focus_id].step_num = 0xFF;
				g_work[focus_id].fixed_step = 10;
				g_work[focus_id].find_peak_rate = 0.90;
				g_work[focus_id].wd_frame = 1;
				g_work[focus_id].conti = OFF;
				break;
			case E_CAMERA_FOCUS_MOVE_DIR_BACKWARD:
				g_work[focus_id].dir = E_CAMERA_FOCUS_MOVE_DIR_BACKWARD;
				g_work[focus_id].start_um = CAMERA.focus[focus_id].dof[g_dof_macro_max-1].lens_um + 20.0;
				g_work[focus_id].last_um = CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_1000CM].lens_um;
				g_work[focus_id].fail_um = CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_1000CM].lens_um;
				g_work[focus_id].step_num = 0;
				g_work[focus_id].find_peak_rate = 0.90;
				g_work[focus_id].wd_frame = 2;
				g_work[focus_id].conti = OFF;
				break;
			default:
				break;
			}

			g_work[focus_id].lens_um[g_cnt[focus_id].wd] = g_work[focus_id].start_um;
			g_work[focus_id].ope = AF_OPE_SEARCH;
			
			if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
				if( CAMERA.focus[ focus_id ].Func_VCM_CalcDAC ){
					g_in_lens[focus_id].now_um = g_work[focus_id].start_um;
					CAMERA.focus[ focus_id ].Func_VCM_CalcDAC( focus_id, &g_in_lens[focus_id], D_FOCUS_LENS2DAC );
				}
				if( CAMERA.focus[ focus_id ].Func_VCM_SetDAC ){
					CAMERA.focus[ focus_id ].Func_VCM_SetDAC( focus_id, g_in_lens[focus_id].now_dac );

					g_focus_bw_init[ focus_id ] = TRUE;
					g_focus_fw_init[ focus_id ] = TRUE;
				}
			}
		}
	}
}
static void single_pdaf_proc( void )
{
	E_CAMERA_ID				focus_id = E_CAMERA_ID_0;
	T_PDAF_RESULT_DETAILS	pd_result;

	for(focus_id = E_CAMERA_ID_0; focus_id < E_CAMERA_ID_MAX; focus_id++){
		if(CAMERA.focus[focus_id].basic){
			PDAF_Ctrl( &g_share_af_data, g_in_lens[focus_id].now_um, &pd_result );
			if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
				if( CAMERA.focus[ focus_id ].Func_VCM_CalcDAC ){
					g_in_lens[focus_id].now_um = pd_result.PdafPos;
					CAMERA.focus[ focus_id ].Func_VCM_CalcDAC( focus_id, &g_in_lens[focus_id], D_FOCUS_LENS2DAC );
				}
				if( CAMERA.focus[ focus_id ].Func_VCM_SetDAC ){
					CAMERA.focus[ focus_id ].Func_VCM_SetDAC( focus_id, g_in_lens[focus_id].now_dac );
				}
			}
//			printf(" Change Lens Position:%f(%d) By PDAF result.\n",g_in_lens[focus_id].now_um, g_in_lens[focus_id].now_dac );
		}
	}
	g_af_state = E_AF_STATE_WAIT;
}
static void single_af_proc( ULLONG frame_no )
{
	E_CAMERA_ID				focus_id = E_CAMERA_ID_0;

	for(focus_id = E_CAMERA_ID_0; focus_id < E_CAMERA_ID_MAX; focus_id++){
		if((g_focus_fw_init[ focus_id ] == TRUE) || (g_focus_bw_init[ focus_id ] == TRUE)){
	
			af_search(focus_id, 0);

			if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
				if( CAMERA.focus[ focus_id ].Func_VCM_CalcDAC ){
					g_in_lens[focus_id].now_um = g_work[focus_id].target_pos_um;
					CAMERA.focus[ focus_id ].Func_VCM_CalcDAC( focus_id, &g_in_lens[focus_id], D_FOCUS_LENS2DAC );
				}
				if( CAMERA.focus[ focus_id ].Func_VCM_SetDAC ){
					CAMERA.focus[ focus_id ].Func_VCM_SetDAC( focus_id, g_in_lens[focus_id].now_dac );
				}
			}

			if(g_work[focus_id].ope != AF_OPE_SEARCH){
				g_focus_bw_init[focus_id] = FALSE;
				g_focus_fw_init[focus_id] = FALSE;
			}else{
				if(CAMERA.focus[focus_id].dof){
					g_cnt[focus_id].wd++;
					if( ( g_work[focus_id].dir == E_CAMERA_FOCUS_MOVE_DIR_FORWARD && g_work[focus_id].lens_um[g_cnt[focus_id].wd - 1] >= g_work[focus_id].last_um) ||
						( g_work[focus_id].dir == E_CAMERA_FOCUS_MOVE_DIR_BACKWARD && g_work[focus_id].lens_um[g_cnt[focus_id].wd - 1] <= g_work[focus_id].last_um) ||
						(g_cnt[focus_id].wd >= g_dof_macro_max)){
						g_focus_bw_init[focus_id] = FALSE;
						g_focus_fw_init[focus_id] = FALSE;
					}
				}
			}
		}
	}
	
	if( (g_focus_fw_init[E_CAMERA_ID_0] == FALSE)&&
		(g_focus_fw_init[E_CAMERA_ID_1] == FALSE)&&
		(g_focus_fw_init[E_CAMERA_ID_2] == FALSE)&&
		(g_focus_fw_init[E_CAMERA_ID_3] == FALSE)&&
		(g_focus_bw_init[E_CAMERA_ID_0] == FALSE)&&
		(g_focus_bw_init[E_CAMERA_ID_1] == FALSE)&&
		(g_focus_bw_init[E_CAMERA_ID_2] == FALSE)&&
		(g_focus_bw_init[E_CAMERA_ID_3] == FALSE)){
		g_af_state = E_AF_STATE_WAIT;
	}
}

static void simple_af( ULLONG frame_no , BOOL inited)
{
	static USHORT	tmp_pos[E_CAMERA_ID_MAX][2] = {{0, 0},{0, 0},{0, 0},{0, 0}};	// [1]=current position, [0]=previous of current position
	static ULLONG	tmp_data[E_CAMERA_ID_MAX][2] = {{0, 0},{0, 0},{0, 0},{0, 0}};	// [1]=current data, [0]=previous of current data
	static ULLONG	max_data[E_CAMERA_ID_MAX] = {0,0,0,0};
	E_CAMERA_ID		focus_id = E_CAMERA_ID_0;
	ULLONG			cur_data[AF_DETECT_NUM_MAX] = {0,0,0};
	if ( inited  )
	{
		tmp_pos[focus_id][0] = 0;
		tmp_pos[focus_id][1] = 0;
		tmp_data[focus_id][0] = 0;
		tmp_data[focus_id][1] = 0;
		max_data[focus_id] = 0;
		return ;
	}
	for(focus_id = E_CAMERA_ID_0; focus_id < E_CAMERA_ID_MAX; focus_id++){
		if((g_focus_fw_init[ focus_id ] == TRUE) || (g_focus_bw_init[ focus_id ] == TRUE))
		{

			get_afdata_sum(focus_id, (ULLONG *)cur_data);

			tmp_pos[focus_id][0] = tmp_pos[focus_id][1];
			tmp_pos[focus_id][1] = g_focus_value[focus_id];
			tmp_data[focus_id][0] = tmp_data[focus_id][1];
			tmp_data[focus_id][1] = cur_data[AF_DETECT_NUM_H0];
			
			if(cur_data[AF_DETECT_NUM_H0] > max_data[focus_id]){
				g_focus_param.af_info[ focus_id ].af_position[0] = tmp_pos[focus_id][0];
				g_focus_param.af_info[ focus_id ].af_position[1] = tmp_pos[focus_id][1];
				max_data[focus_id] = cur_data[AF_DETECT_NUM_H0];
			}

#ifdef D_FOCUS_DEBUG_PRINT_ON
			printf( "\n [%d][%d(%d)]:", focus_id, g_focus_param.af_info[ focus_id ].af_step_cnt, g_coarse_search[ focus_id ]);
			printf( "\n tmp p[%d][%d] buf p[%d][%d]"
				, tmp_pos[focus_id][0]
				, tmp_pos[focus_id][1]
				, g_focus_param.af_info[ focus_id ].af_position[0]
				, g_focus_param.af_info[ focus_id ].af_position[1]);
			printf( "\n tmp d[0x%08x%08x][0x%08x%08x]"
				, (UINT32)(tmp_data[focus_id][0]>>32)
				, (UINT32)(tmp_data[focus_id][0])
				, (UINT32)(tmp_data[focus_id][1]>>32)
				, (UINT32)(tmp_data[focus_id][1]));
#endif

			g_focus_param.af_info[ focus_id ].af_step_cnt++;

			if(g_coarse_search[focus_id] == TRUE)
			{
				if(g_focus_param.af_info[ focus_id ].af_step_cnt >= g_focus_param.af_info[ focus_id ].af_step_max){
					g_coarse_search[focus_id] = FALSE;
					if(g_focus_param.af_info[ focus_id ].af_position[0] > 0)
						g_focus_value[focus_id] = g_focus_param.af_info[ focus_id ].af_position[0];
					else
						g_focus_value[focus_id] = g_focus_param.af_info[ focus_id ].af_position[1];
					if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
						if( CAMERA.focus[ focus_id ].Func_VCM_SetDAC ){
							CAMERA.focus[ focus_id ].Func_VCM_SetDAC( focus_id, g_focus_value[focus_id] );
						}
					}
					memset( (void*)&g_focus_param, 0, sizeof(g_focus_param) );
					if(g_af_dir[focus_id] == E_CAMERA_FOCUS_MOVE_DIR_BACKWARD){
						g_focus_param.af_info[ focus_id ].af_step_max = CAMERA.focus[focus_id].basic->fine_bw_step_num;
					}else if(g_af_dir[focus_id] == E_CAMERA_FOCUS_MOVE_DIR_FORWARD){
						g_focus_param.af_info[ focus_id ].af_step_max = CAMERA.focus[focus_id].basic->fine_fw_step_num;
					}
					tmp_pos[focus_id][0] = 0;
					tmp_pos[focus_id][1] = 0;
					tmp_data[focus_id][0] = 0;
					tmp_data[focus_id][1] = 0;
					max_data[focus_id] = 0;
				}else{
					if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
						if( CAMERA.focus[ focus_id ].Func_VCM_Move ){
							CAMERA.focus[ focus_id ].Func_VCM_Move( focus_id, g_af_dir[focus_id], g_coarse_search[focus_id], &g_focus_value[focus_id] );
						}
					}
				}
			}
		
			else
			{
				if(g_focus_param.af_info[ focus_id ].af_step_cnt >= g_focus_param.af_info[ focus_id ].af_step_max){
					g_coarse_search[focus_id] = TRUE;
					g_focus_value[focus_id] = g_focus_param.af_info[ focus_id ].af_position[1];
					if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
						if( CAMERA.focus[ focus_id ].Func_VCM_SetDAC ){
							CAMERA.focus[ focus_id ].Func_VCM_SetDAC( focus_id, g_focus_value[focus_id] );
						}
					}
					tmp_pos[focus_id][0] = 0;
					tmp_pos[focus_id][1] = 0;
					tmp_data[focus_id][0] = 0;
					tmp_data[focus_id][1] = 0;
					max_data[focus_id] = 0;
					g_focus_bw_init[focus_id] = FALSE;
					g_focus_fw_init[focus_id] = FALSE;
				}else{
					#if 1
					if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
						if( CAMERA.focus[ focus_id ].Func_VCM_Move ){
							CAMERA.focus[ focus_id ].Func_VCM_Move( focus_id, g_af_dir[focus_id], g_coarse_search[focus_id], &g_focus_value[focus_id] );
						}
					}
					#endif
				}
			}

#ifdef D_FOCUS_DEBUG_PRINT_ON
			printf( "\n coarse_search(%d):", g_coarse_search[ focus_id ]);
#endif
		}
	}
	
	if( (g_focus_fw_init[E_CAMERA_ID_0] == FALSE)&&
		(g_focus_fw_init[E_CAMERA_ID_1] == FALSE)&&
		(g_focus_fw_init[E_CAMERA_ID_2] == FALSE)&&
		(g_focus_fw_init[E_CAMERA_ID_3] == FALSE)&&
		(g_focus_bw_init[E_CAMERA_ID_0] == FALSE)&&
		(g_focus_bw_init[E_CAMERA_ID_1] == FALSE)&&
		(g_focus_bw_init[E_CAMERA_ID_2] == FALSE)&&
		(g_focus_bw_init[E_CAMERA_ID_3] == FALSE)){
		g_af_state = E_AF_STATE_WAIT;
	}
}

static void caf_init(E_CAMERA_ID id, E_SHARE_AF_WINDOW win, FLOAT start_um, CHAR dir)
{
	ULLONG	cur_data[AF_DETECT_NUM_MAX] = {0,0,0};
	UCHAR	detect_num;
	INT32	cnt;
	FLOAT	next;

	for(cnt=0; cnt<g_dof_macro_max; cnt++){
		for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num++){
			g_work[id].roi.af_data[detect_num][cnt] = 0x00000000;
		}
		g_work[id].lens_um[cnt] = 0.0;
	}

	get_afdata_by_win(id, win, (ULLONG *)cur_data);
	g_work[id].roi.af_data[AF_DETECT_NUM_H0][0] = cur_data[AF_DETECT_NUM_H0];
	g_work[id].roi.af_data[AF_DETECT_NUM_H1][0] = cur_data[AF_DETECT_NUM_H1];
	g_work[id].roi.af_data[AF_DETECT_NUM_V][0]  = cur_data[AF_DETECT_NUM_V];

	for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num++){
		g_work[id].roi.af_2up2down_data[detect_num]= 0x00000000;
		g_work[id].roi.af_2up1down_data[detect_num]= 0x00000000;
		g_work[id].roi.af_1up2down_data[detect_num]= 0x00000000;
		g_work[id].roi.af_1up1down_data[detect_num]= 0x00000000;
		g_work[id].roi.af_start_down_data[detect_num]= 0x00000000;
		g_work[id].roi.af_last_up_data[detect_num]= 0x00000000;

		g_work[id].roi.af_2up2down_step[detect_num]= 0x00;
		g_work[id].roi.af_2up1down_step[detect_num]= 0x00;
		g_work[id].roi.af_1up2down_step[detect_num]= 0x00;
		g_work[id].roi.af_1up1down_step[detect_num]= 0x00;
		g_work[id].roi.af_start_down_step[detect_num]= 0x00;
		g_work[id].roi.af_last_up_step[detect_num]= 0x00;

		g_work[id].roi.af_max_step[detect_num] = 0x00;
		g_work[id].roi.af_min_step[detect_num] = 0x00;

		g_work[id].roi.af_find_2up2down[detect_num] = OFF;
		g_work[id].roi.af_find_2up1down[detect_num] = OFF;
		g_work[id].roi.af_find_1up2down[detect_num] = OFF;
		g_work[id].roi.af_find_1up1down[detect_num] = OFF;
		g_work[id].roi.af_find_start_down[detect_num] = OFF;
		g_work[id].roi.af_find_last_up[detect_num] = OFF;

		g_work[id].roi.af_start_down_cnt[detect_num] = 0;
		g_work[id].roi.af_last_up_cnt[detect_num] = 0;
		g_work[id].roi.af_down_cnt[detect_num] = 0;

		g_work[id].roi.af_max_data[detect_num]  = g_work[id].roi.af_data[detect_num][0];
		g_work[id].roi.af_min_data[detect_num]  = g_work[id].roi.af_data[detect_num][0];
	}

	g_work[id].roi.af_roi_calc_peak_um = 0;
	g_work[id].find_peak = OFF;

	if(dir == DIR_INF2MAC){
		g_work[id].dir = E_CAMERA_FOCUS_MOVE_DIR_FORWARD;
		g_work[id].start_um = start_um;
		g_work[id].last_um = CAMERA.focus[id].dof[E_CAMERA_FOCUS_DOF_8CM].lens_um;
		g_work[id].fail_um = CAMERA.focus[id].dof[E_CAMERA_FOCUS_DOF_100CM].lens_um;
		g_work[id].step_num = 0xFF;
		g_work[id].fixed_step = 10;
		g_work[id].find_peak_rate = 0.90;
		g_work[id].wd_frame = 1;
		g_work[id].conti = ON;
		g_dir_search[id].dir_search_step0 = 8;
		g_dir_search[id].dir_search_step1 = 4;
	}else{
		g_work[id].dir = E_CAMERA_FOCUS_MOVE_DIR_BACKWARD;
		g_work[id].start_um = start_um;
		g_work[id].last_um = CAMERA.focus[id].dof[E_CAMERA_FOCUS_DOF_1000CM].lens_um;
		g_work[id].fail_um = CAMERA.focus[id].dof[E_CAMERA_FOCUS_DOF_100CM].lens_um;
		g_work[id].step_num = 0xFF;
		g_work[id].fixed_step = 10;
		g_work[id].find_peak_rate = 0.90;
		g_work[id].wd_frame = 1;
		g_work[id].conti = ON;
		g_dir_search[id].dir_search_step0 = 8;
		g_dir_search[id].dir_search_step1 = 4;
	}

	g_work[id].lens_um[0] = g_work[id].start_um;
	if( g_work[id].step_num == 0 ){
		next = next_lens_um_from_DoF(id, g_work[id].lens_um[0], g_work[id].dir);
	}else if( g_work[id].step_num == 0xFF ){
		if((g_work[id].last_um - g_work[id].start_um) > 0 ){
			next = g_work[id].lens_um[0] + g_work[id].fixed_step;
		}else{
			next = g_work[id].lens_um[0] - g_work[id].fixed_step;
		}
	}else{
		next = g_work[id].lens_um[0] + ( (g_work[id].last_um - g_work[id].start_um ) / g_work[id].step_num );
	}
	g_work[id].lens_um[1]   = next;
	g_work[id].target_pos_um = next;
	g_work[id].ope = AF_OPE_SEARCH;
}

static void caf_proc( ULLONG frame_no )
{
	E_CAMERA_ID		focus_id = E_CAMERA_ID_0;
	ULLONG			cur_data[AF_DETECT_NUM_MAX] = {0,0,0};
	int ave_cnt;
	UCHAR trg = OFF;
	
	for(focus_id = E_CAMERA_ID_0; focus_id < E_CAMERA_ID_MAX; focus_id++){
		if(CAMERA.focus[focus_id].basic){
			g_cnt[focus_id].vd++;
			get_cafdata_by_win(focus_id, 0);

			switch(g_caf_state[focus_id])
			{
				case E_CAF_STATE_DETECTING_TRG:
					if(g_in_lens[focus_id].now_um > CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_20CM].lens_um){
						ave_cnt = 15;
					}else{
						ave_cnt = 3;
					}
					if(af_data_calc_average(focus_id, ave_cnt) == OK){

						if((g_thrs[focus_id].wddata_l == 0) || (g_thrs[focus_id].wddatav_l == 0) || (g_thrs[focus_id].afdatas_l == 0)){
							trg = ON;
						}
						
						if( (g_af_wddata0_ave[focus_id] < g_thrs[focus_id].wddata_l) ||  (g_af_wddata0_ave[focus_id] > g_thrs[focus_id].wddata_h)){
							trg = ON;
						}
						if( (g_af_wddatav_ave[focus_id] < g_thrs[focus_id].wddatav_l) || (g_af_wddatav_ave[focus_id] > g_thrs[focus_id].wddatav_h)){
							trg = ON;
						}
						if( (g_af_afdatas_ave[focus_id] < g_thrs[focus_id].afdatas_l) || (g_af_afdatas_ave[focus_id] > g_thrs[focus_id].afdatas_h)){
							trg = ON;
						}
						if(trg){
							g_caf_state[focus_id] = E_CAF_STATE_DETECTING_WAIT;
						}
					}
				break;
				case E_CAF_STATE_DETECTING_WAIT:
					if(judge_stable(focus_id, 0) == OK){
						if( g_in_lens[focus_id].now_um > CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_12CM].lens_um ){
							g_dir_search[focus_id].dir = DIR_MAC2INF;
						}else if( g_in_lens[focus_id].now_um < CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_13CM].lens_um ){
							g_dir_search[focus_id].dir = DIR_INF2MAC;
						}else{
							g_dir_search[focus_id].dir = DIR_MAC2INF;
						}
						get_afdata_by_win(focus_id, 0, (ULLONG *)cur_data);
						g_dir_search[focus_id].base_pos	   = g_in_lens[focus_id].now_um;
						g_dir_search[focus_id].next_pos	   = g_in_lens[focus_id].now_um + g_dir_search[focus_id].dir * g_dir_search[focus_id].dir_search_step0;
						g_dir_search[focus_id].base_wddatah0 = cur_data[AF_DETECT_NUM_H0];
						g_dir_search[focus_id].base_wddatav  = cur_data[AF_DETECT_NUM_V];
						g_dir_search[focus_id].wddatah0	   = 0;
						g_dir_search[focus_id].wddatav	   = 0;
						g_work[focus_id].target_pos_um = g_dir_search[focus_id].next_pos;
						g_caf_state[focus_id] = E_CAF_STATE_DIR_SEARCH;
						g_work[focus_id].ope  = AF_OPE_SEARCH;
					}
				break;
				case E_CAF_STATE_DIR_SEARCH:
					{
						INT32 dir = g_dir_search[focus_id].dir;
						UCHAR skip_flg = 0;
						FLOAT up_thr_rate, down_thr_rate;

						up_thr_rate   = 1.03;
						down_thr_rate = 0.97;
						
						get_afdata_by_win(focus_id, 0, (ULLONG *)cur_data);
						g_dir_search[focus_id].wddatah0 = cur_data[AF_DETECT_NUM_H0];
						g_dir_search[focus_id].wddatav  = cur_data[AF_DETECT_NUM_V];
						if( (g_dir_search[focus_id].base_wddatah0 * up_thr_rate) < g_dir_search[focus_id].wddatah0){
							if(g_dir_search[focus_id].base_wddatav < g_dir_search[focus_id].wddatav){
								dir = g_dir_search[focus_id].dir;
							}else{
								if( ((float)g_dir_search[focus_id].wddatah0 / g_dir_search[focus_id].base_wddatah0) > ((float)g_dir_search[focus_id].wddatav / g_dir_search[focus_id].base_wddatav)){
									dir = g_dir_search[focus_id].dir;
								}else{
									dir = (-1) * g_dir_search[focus_id].dir;
								}
							}
						}else if((g_dir_search[focus_id].base_wddatah0 * down_thr_rate) > g_dir_search[focus_id].wddatah0){
							if(g_dir_search[focus_id].base_wddatav > g_dir_search[focus_id].wddatav){
								dir = (-1) * g_dir_search[focus_id].dir;
							}else{
								if( ((float)g_dir_search[focus_id].wddatah0 / g_dir_search[focus_id].base_wddatah0) > ((float)g_dir_search[focus_id].wddatav / g_dir_search[focus_id].base_wddatav)){
									dir = (-1) * g_dir_search[focus_id].dir;
								}else{
									dir = g_dir_search[focus_id].dir;
								}
							}
						}else{
							if( (g_dir_search[focus_id].base_wddatav * up_thr_rate) < g_dir_search[focus_id].wddatav){
								dir = g_dir_search[focus_id].dir;
							}else if((g_dir_search[focus_id].base_wddatav * down_thr_rate) > g_dir_search[focus_id].wddatav){
								dir = (-1) * g_dir_search[focus_id].dir;
							}else{
								if((g_dir_search[focus_id].base_wddatah0 < g_dir_search[focus_id].wddatah0) && (g_dir_search[focus_id].base_wddatav < g_dir_search[focus_id].wddatav)){
									dir = g_dir_search[focus_id].dir;
								}else if((g_dir_search[focus_id].base_wddatah0 > g_dir_search[focus_id].wddatah0) && (g_dir_search[focus_id].base_wddatav > g_dir_search[focus_id].wddatav)){
									dir = (-1) * g_dir_search[focus_id].dir;
								}else{
									if( g_dir_search[focus_id].dir == DIR_INF2MAC ){
										if( g_in_lens[focus_id].now_um > CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_10CM].lens_um ){
											dir = (-1) * g_dir_search[focus_id].dir;
										}else{
											g_dir_search[focus_id].next_pos += (g_dir_search[focus_id].dir * g_dir_search[focus_id].dir_search_step1);
											g_work[focus_id].target_pos_um = g_dir_search[focus_id].next_pos;
											skip_flg = 1;
										}
									}else{
										if( g_in_lens[focus_id].now_um < CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_30CM].lens_um ){
											dir = (-1) * g_dir_search[focus_id].dir;
										}else{
											g_dir_search[focus_id].next_pos += (g_dir_search[focus_id].dir * g_dir_search[focus_id].dir_search_step1);
											g_work[focus_id].target_pos_um = g_dir_search[focus_id].next_pos;
											skip_flg = 1;
										}
									}
								}
							}
						}
						if(skip_flg != 1){
							g_cnt[focus_id].vd = 0;
							g_cnt[focus_id].wd = 0;
							caf_init(focus_id, 0, g_dir_search[focus_id].next_pos, dir);
							g_cnt[focus_id].wd++;
							g_caf_state[focus_id] = E_CAF_STATE_WAIT_WD;
							af_data_calc_average(focus_id, OFF);
						}
					}
				break;
				case E_CAF_STATE_WAIT_WD:
					if( g_cnt[focus_id].vd >= g_work[focus_id].wd_frame ){
						g_cnt[focus_id].vd = 0;
						g_caf_state[focus_id] = E_CAF_STATE_WAIT_WD;
						af_search(focus_id, 0);
						g_cnt[focus_id].wd++;

						if(g_work[focus_id].ope != AF_OPE_SEARCH){
							caf_set_threshold(focus_id, 0);
							g_caf_state[focus_id] = E_CAF_STATE_MOVE_TO_PEAK;
							g_work[focus_id].ope = AF_OPE_SEARCH;
							g_work[focus_id].last_target_um = g_work[focus_id].target_pos_um;
							if(my_abs(g_in_lens[focus_id].now_um - g_work[focus_id].last_target_um) >= 20){
								g_work[focus_id].target_pos_um = g_in_lens[focus_id].now_um + ( g_work[focus_id].last_target_um - g_in_lens[focus_id].now_um ) / 5;
							}
						}else{
							if(af_data_calc_average(focus_id, 3) == OK){
								if((my_abs(g_af_afdatas_ave[focus_id] - g_share_af_data.sensor[focus_id].window[0].data[E_SHARE_AF_DATAS]) > g_af_afdatas_ave[focus_id] * 0.5)){
									g_caf_state[focus_id] = E_CAF_STATE_DETECTING_TRG;
									af_data_calc_average(focus_id, OFF);
								}
							}
						}
					}
				break;
				case E_CAF_STATE_MOVE_TO_PEAK:
					if(g_in_lens[focus_id].now_um == g_work[focus_id].last_target_um){
						g_caf_state[focus_id] = E_CAF_STATE_DETECTING_TRG;
						af_data_calc_average(focus_id, OFF);
						g_work[focus_id].ope = AF_OPE_CONTI;

						get_afdata_by_win(focus_id, 0, (ULLONG *)cur_data);
						if(cur_data[AF_DETECT_NUM_H0] < 20000){
							g_thrs[focus_id].wddata_h = 25000;
							g_thrs[focus_id].wddata_l = 0;
						}else{
							if(cur_data[AF_DETECT_NUM_H0] > g_work[focus_id].roi.af_max_data[0]){
								if(g_work[focus_id].target_pos_um > CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_20CM].lens_um){
									g_thrs[focus_id].wddata_h = cur_data[AF_DETECT_NUM_H0] * 1.6;
									g_thrs[focus_id].wddata_l = cur_data[AF_DETECT_NUM_H0] * 0.5;
								}else{
									g_thrs[focus_id].wddata_h = cur_data[AF_DETECT_NUM_H0] * 1.24;
									g_thrs[focus_id].wddata_l = cur_data[AF_DETECT_NUM_H0] * 0.76;
								}
							}
						}
						if(cur_data[AF_DETECT_NUM_V] < 20000){
							g_thrs[focus_id].wddatav_h = 25000;
							g_thrs[focus_id].wddatav_l = 0;
						}else{
							if(cur_data[AF_DETECT_NUM_V] > g_work[focus_id].roi.af_max_data[2]){
								if(g_work[focus_id].target_pos_um > CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_20CM].lens_um){
									g_thrs[focus_id].wddatav_h = cur_data[AF_DETECT_NUM_V] * 1.6;
									g_thrs[focus_id].wddatav_l = cur_data[AF_DETECT_NUM_V] * 0.5;
								}else{
									g_thrs[focus_id].wddatav_h = cur_data[AF_DETECT_NUM_V] * 1.24;
									g_thrs[focus_id].wddatav_l = cur_data[AF_DETECT_NUM_V] * 0.76;
								}
							}
						}
					}else{
						if(my_abs(g_in_lens[focus_id].now_um - g_work[focus_id].last_target_um) < 20){
							g_work[focus_id].target_pos_um = g_work[focus_id].last_target_um;
						}else{
							g_work[focus_id].target_pos_um = g_in_lens[focus_id].now_um + ( g_work[focus_id].last_target_um - g_in_lens[focus_id].now_um ) / 5;
						}
					}
				break;
				default:
					g_caf_state[focus_id] = E_CAF_STATE_DETECTING_TRG;
				break;
			}
			
			if(g_in_lens[focus_id].now_um != g_work[focus_id].target_pos_um){
				if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
					if( CAMERA.focus[ focus_id ].Func_VCM_CalcDAC ){
						g_in_lens[focus_id].now_um = g_work[focus_id].target_pos_um;
						CAMERA.focus[ focus_id ].Func_VCM_CalcDAC( focus_id, &g_in_lens[focus_id], D_FOCUS_LENS2DAC );
					}
					if( CAMERA.focus[ focus_id ].Func_VCM_SetDAC ){
						CAMERA.focus[ focus_id ].Func_VCM_SetDAC( focus_id, g_in_lens[focus_id].now_dac );
						// g_in_lens[focus_id].now_um is recalculated in Func_VCM_SetDAC() by actual DAC, so target_pos_um have to initialized by now_um
						if( g_work[focus_id].last_target_um == g_work[focus_id].target_pos_um ){
							g_work[focus_id].last_target_um = g_work[focus_id].target_pos_um = g_in_lens[focus_id].now_um;
						}
						else{
							g_work[focus_id].target_pos_um = g_in_lens[focus_id].now_um;
						}
						g_work[focus_id].target_pos_um = g_in_lens[focus_id].now_um;
					}
				}
			}
		}
	}
}


static void continues_af( ULLONG frame_no )
{
	static USHORT	tmp_pos[E_CAMERA_ID_MAX][2] = {{0, 0},{0, 0},{0, 0},{0, 0}};	// [1]=current position, [0]=previous of current position
	static ULLONG	tmp_data[E_CAMERA_ID_MAX][2] = {{0, 0},{0, 0},{0, 0},{0, 0}};	// [1]=current data, [0]=previous of current data
	static ULLONG	max_data[E_CAMERA_ID_MAX][AF_DETECT_NUM_MAX] = {{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0}};
	static USHORT	max_step[E_CAMERA_ID_MAX] = {0,0,0,0};
	static USHORT	search_cnt[E_CAMERA_ID_MAX] = {0,0,0,0};
	E_CAMERA_ID		focus_id = E_CAMERA_ID_0;
	ULLONG			cur_data[AF_DETECT_NUM_MAX] = {0, 0, 0};

	if( g_caf_start_trig == FALSE){
		for(focus_id = E_CAMERA_ID_0; focus_id < E_CAMERA_ID_MAX; focus_id++){
			if(CAMERA.focus[focus_id].basic){
				get_afdata_sum(focus_id, (ULLONG *)cur_data);
				if(cur_data[AF_DETECT_NUM_H0] < (g_caf_max_curr[focus_id][AF_DETECT_NUM_H0] * g_af_continues_focus_sensitivity/*0.95*/)){
					if(g_caf_max_cnt[focus_id][AF_DETECT_NUM_H0]++ >= g_af_max_cnt/*5*/){
						g_caf_start_trig = TRUE;
					}
				}else{
					g_caf_max_cnt[focus_id][AF_DETECT_NUM_H0] = 0;
				}
				if(cur_data[AF_DETECT_NUM_V] < (g_caf_max_curr[focus_id][AF_DETECT_NUM_V] * g_af_continues_focus_sensitivity/*0.95*/)){
					if(g_caf_max_cnt[focus_id][AF_DETECT_NUM_V]++ >= g_af_max_cnt/*5*/){
						g_caf_start_trig = TRUE;
					}
				}else{
					g_caf_max_cnt[focus_id][AF_DETECT_NUM_V] = 0;
				}
				if((g_share_af_data.sensor[focus_id].window[0].data[E_SHARE_AF_DATAS] < (g_caf_max_curr_y[focus_id] * (g_af_continues_focus_sensitivity_near)/*0.9*/)) ||
					(g_share_af_data.sensor[focus_id].window[0].data[E_SHARE_AF_DATAS] > (g_caf_max_curr_y[focus_id] * (g_af_continues_focus_sensitivity_far)/*1.1*/))){
					if(g_caf_max_cnt_y[focus_id]++ >= g_af_max_cnt/*5*/){
						g_caf_start_trig = TRUE;
					}
				}else{
					g_caf_max_cnt_y[focus_id] = 0;
				}
			}
		}
	}
	
	if( g_caf_start_trig == FALSE){
		return;
	}else if((g_focus_fw_init[ E_CAMERA_ID_0 ] == FALSE) && (g_focus_bw_init[ E_CAMERA_ID_0 ] == FALSE)){ // Refer by Sensor0 status
		for(focus_id = E_CAMERA_ID_0; focus_id < E_CAMERA_ID_MAX; focus_id++){
			if(CAMERA.focus[focus_id].basic){
				if(g_af_dir[focus_id] == g_caf_dir[focus_id])
				{
					//printf("==========move start , g_af_dir[focus_id] =%d ===========\n", g_af_dir[focus_id] );
					if(g_af_dir[focus_id] == E_CAMERA_FOCUS_MOVE_DIR_FORWARD){
						if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
							if( CAMERA.focus[ focus_id ].Func_VCM_Move ){
								CAMERA.focus[ focus_id ].Func_VCM_Move( focus_id, E_CAMERA_FOCUS_MOVE_DIR_FORWARD_INIT, TRUE, &g_focus_value[focus_id] );
								g_focus_fw_init[ focus_id ] = TRUE;
							}
						}
					}else if(g_af_dir[focus_id] == E_CAMERA_FOCUS_MOVE_DIR_BACKWARD){
						if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
							if( CAMERA.focus[ focus_id ].Func_VCM_Move ){
								CAMERA.focus[ focus_id ].Func_VCM_Move( focus_id, E_CAMERA_FOCUS_MOVE_DIR_BACKWARD_INIT, TRUE, &g_focus_value[focus_id] );
								g_focus_bw_init[ focus_id ] = TRUE;
							}
						}
					}
				
#if 1
					//printf("===========mov end===========\n");
				}else{
					//printf("==========caf start , g_af_dir[focus_id] =%d ===========\n", g_af_dir[focus_id] );
					if(g_caf_dir[focus_id] == E_CAMERA_FOCUS_MOVE_DIR_FORWARD){
						if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
							if( CAMERA.focus[ focus_id ].Func_VCM_SetDAC ){
								g_focus_value[focus_id] = CAMERA.focus[focus_id].basic->mini_value;
								CAMERA.focus[ focus_id ].Func_VCM_SetDAC( focus_id, g_focus_value[focus_id] );
								g_focus_fw_init[ focus_id ] = TRUE;
							}
						}
					}else if(g_caf_dir[focus_id] == E_CAMERA_FOCUS_MOVE_DIR_BACKWARD){
						if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
							if( CAMERA.focus[ focus_id ].Func_VCM_SetDAC ){
								g_focus_value[focus_id] = CAMERA.focus[focus_id].basic->max_value;
								CAMERA.focus[ focus_id ].Func_VCM_SetDAC( focus_id, g_focus_value[focus_id] );
								g_focus_bw_init[ focus_id ] = TRUE;
							}
						}
					}
					//printf("===========caf end===========\n");
				}
#endif
			}
			g_caf_max_curr_y[focus_id] = 0;
			g_caf_max_cnt_y[focus_id] = 0;
		}
		return;
	}

	for(focus_id = E_CAMERA_ID_0; focus_id < E_CAMERA_ID_MAX; focus_id++){
		if((g_focus_fw_init[ focus_id ] == TRUE) || (g_focus_bw_init[ focus_id ] == TRUE)){

			get_afdata_sum(focus_id, (ULLONG *)cur_data);

			tmp_pos[focus_id][0] = tmp_pos[focus_id][1];
			tmp_pos[focus_id][1] = g_focus_value[focus_id];
			tmp_data[focus_id][0] = tmp_data[focus_id][1];
			tmp_data[focus_id][1] = cur_data[AF_DETECT_NUM_H0];
			
			if((cur_data[AF_DETECT_NUM_H0] > max_data[focus_id][AF_DETECT_NUM_H0]) || (cur_data[AF_DETECT_NUM_V] > max_data[focus_id][AF_DETECT_NUM_V])){
				g_focus_param.af_info[ focus_id ].af_position[0] = tmp_pos[focus_id][0];
				g_focus_param.af_info[ focus_id ].af_position[1] = tmp_pos[focus_id][1];
				max_data[focus_id][AF_DETECT_NUM_H0] = cur_data[AF_DETECT_NUM_H0];
				max_data[focus_id][AF_DETECT_NUM_V] = cur_data[AF_DETECT_NUM_V];
				g_caf_max_curr_y[focus_id] = g_share_af_data.sensor[focus_id].window[0].data[E_SHARE_AF_DATAS];
				max_step[focus_id] = g_focus_param.af_info[ focus_id ].af_step_cnt;
			}

			if(tmp_data[focus_id][0] > tmp_data[focus_id][1]){
				search_cnt[focus_id]++;
			}else{
				search_cnt[focus_id] = 0;
			}

#ifdef D_FOCUS_DEBUG_PRINT_ON
			printf( "\n [%d][%d(%d)](%d):", focus_id, g_focus_param.af_info[ focus_id ].af_step_cnt, g_coarse_search[ focus_id ], search_cnt[ focus_id ]);
			printf( "\n tmp p[%d][%d] buf p[%d][%d]"
				, tmp_pos[focus_id][0]
				, tmp_pos[focus_id][1]
				, g_focus_param.af_info[ focus_id ].af_position[0]
				, g_focus_param.af_info[ focus_id ].af_position[1]);
			printf( "\n tmp d[0x%08x%08x][0x%08x%08x]"
				, (UINT32)(tmp_data[focus_id][0]>>32)
				, (UINT32)(tmp_data[focus_id][0])
				, (UINT32)(tmp_data[focus_id][1]>>32)
				, (UINT32)(tmp_data[focus_id][1]));
#endif

			g_focus_param.af_info[ focus_id ].af_step_cnt++;
			#if 1
			if((g_focus_param.af_info[ focus_id ].af_step_cnt >= g_focus_param.af_info[ focus_id ].af_step_max) || (search_cnt[focus_id] > 3)){
				
				g_caf_max_curr[focus_id][AF_DETECT_NUM_H0] = max_data[focus_id][AF_DETECT_NUM_H0];
				g_caf_max_curr[focus_id][AF_DETECT_NUM_V] = max_data[focus_id][AF_DETECT_NUM_V];
				g_caf_max_cnt[focus_id][AF_DETECT_NUM_H0] = 0;
				g_caf_max_cnt[focus_id][AF_DETECT_NUM_V] = 0;
				g_caf_max_cnt_y[focus_id] = 0;
				g_focus_value[focus_id] = g_focus_param.af_info[ focus_id ].af_position[1];
				//printf("==========Func_VCM_SetDAC start , g_focus_value[focus_id]  =%d ===========\n", g_focus_value[focus_id] );
				if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
					if( CAMERA.focus[ focus_id ].Func_VCM_SetDAC ){
						CAMERA.focus[ focus_id ].Func_VCM_SetDAC( focus_id, g_focus_value[focus_id] );
					}
				}
				//printf("==========Func_VCM_SetDAC end  ===========\n" );
				if(max_step[focus_id] > (g_focus_param.af_info[ focus_id ].af_step_max / 2)){
					if(g_caf_dir[focus_id] == E_CAMERA_FOCUS_MOVE_DIR_FORWARD){
						g_caf_dir[focus_id] = E_CAMERA_FOCUS_MOVE_DIR_BACKWARD;
					}else if(g_caf_dir[focus_id] == E_CAMERA_FOCUS_MOVE_DIR_BACKWARD){
						g_caf_dir[focus_id] = E_CAMERA_FOCUS_MOVE_DIR_FORWARD;
					}
				}

				tmp_pos[focus_id][0] = 0;
				tmp_pos[focus_id][1] = 0;
				tmp_data[focus_id][0] = 0;
				tmp_data[focus_id][1] = 0;
				max_data[focus_id][AF_DETECT_NUM_H0] = 0;
				max_data[focus_id][AF_DETECT_NUM_V] = 0;
				search_cnt[focus_id] = 0;
				g_focus_bw_init[focus_id] = FALSE;
				g_focus_fw_init[focus_id] = FALSE;
				g_af_state = E_AF_STATE_START;
				if((max_step[focus_id] >= g_focus_param.af_info[ focus_id ].af_step_max) || (max_step[focus_id] == 0)){
					caf_interval_timer_start(D_CAF_INTERVAL_US);
				}else{
					g_caf_start_trig = FALSE;
				}
			}else{
				if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
					if( CAMERA.focus[ focus_id ].Func_VCM_Move ){
							//printf("==========move start , g_coarse_search[focus_id] =%d ===========\n", g_coarse_search[focus_id] );
							CAMERA.focus[ focus_id ].Func_VCM_Move( focus_id, g_caf_dir[focus_id], g_coarse_search[focus_id], &g_focus_value[focus_id] );
							//printf("==========move end ===========\n" );
					}
				}
			}
			#else
			if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
				if( CAMERA.focus[ focus_id ].Func_VCM_Move ){
						//printf("==========move start , g_coarse_search[focus_id] =%d ===========\n", g_coarse_search[focus_id] );
						CAMERA.focus[ focus_id ].Func_VCM_Move( focus_id, g_caf_dir[focus_id], g_coarse_search[focus_id], &g_focus_value[focus_id] );
						//printf("==========move end ===========\n" );
				}
			}
			#endif
		}
	}
}


static void caf_pd_proc( void )
{
	// Temporary solution
	//  - Just Only Adjusting Lens posion by PDAF result.
	//  - This function just waits frame stable in E_CAF_STATE_DETECTING_WAIT.
	// ToDo
	//  1. For MP, you should modify condition of E_CAF_STATE_DETECTING_TRG.
	//     (e.g. Checking scene stability by using Gyro parrameter..)
	//  2. In dark condition, I think PDAF result would be uncertain.
	
	E_CAMERA_ID focus_id;
	T_PDAF_RESULT_DETAILS	pd_result;
	static UINT32 stable_cnt[E_CAMERA_ID_MAX] = {0, 0, 0, 0};
	
	for(focus_id = E_CAMERA_ID_0; focus_id < E_CAMERA_ID_MAX; focus_id++){
		if(CAMERA.focus[focus_id].basic){
			switch(g_caf_state[focus_id]){
				case E_CAF_STATE_DETECTING_TRG:
//					printf(" Detect Triger\n");
					PDAF_Ctrl( &g_share_af_data, g_in_lens[focus_id].now_um, &pd_result );
					if(CAMERA.focus[focus_id].basic->focus_if == E_CAMERA_FOCUS_IF_VCM){
						if( CAMERA.focus[ focus_id ].Func_VCM_CalcDAC ){
							g_in_lens[focus_id].now_um = pd_result.PdafPos;
							CAMERA.focus[ focus_id ].Func_VCM_CalcDAC( focus_id, &g_in_lens[focus_id], D_FOCUS_LENS2DAC );
						}
						if( CAMERA.focus[ focus_id ].Func_VCM_SetDAC ){
							CAMERA.focus[ focus_id ].Func_VCM_SetDAC( focus_id, g_in_lens[focus_id].now_dac );
						}
					}
//					printf(" Change Lens Position:%f(%d) By PDAF result.\n",g_in_lens[focus_id].now_um, g_in_lens[focus_id].now_dac );
					g_caf_state[focus_id] = E_CAF_STATE_DETECTING_WAIT;
					break;
				case E_CAF_STATE_DETECTING_WAIT:
//					printf(" Wait Detect Triger(%d)\n",stable_cnt[focus_id] );
					stable_cnt[focus_id]++;
					if(stable_cnt[focus_id] == 2){
						stable_cnt[focus_id] = 0;
						g_caf_state[focus_id] = E_CAF_STATE_DETECTING_TRG;
					}
					break;
				default:
//				printf(" CAF unknown state(%d)\n",g_caf_state[focus_id] );
				break;
			}
		}
	}
}

//extern float now_um;
//extern USHORT now_dac;

static void af_search(E_CAMERA_ID focus_id, E_SHARE_AF_WINDOW win)
{
	ULLONG					cur_data[AF_DETECT_NUM_MAX] = {0,0,0};
	
	UCHAR	detect_num;
	UINT32	cnt;
	FLOAT	next;
	UCHAR	af_end_flg = OFF;
	
	g_work[focus_id].ope = AF_OPE_SEARCH;

	get_afdata_by_win(focus_id, win, (ULLONG *)cur_data);
	g_work[focus_id].roi.af_data[AF_DETECT_NUM_H0][g_cnt[focus_id].wd%AF_MAX_WD_NUM] = cur_data[AF_DETECT_NUM_H0];
	g_work[focus_id].roi.af_data[AF_DETECT_NUM_H1][g_cnt[focus_id].wd%AF_MAX_WD_NUM] = cur_data[AF_DETECT_NUM_H1];
	g_work[focus_id].roi.af_data[AF_DETECT_NUM_V][g_cnt[focus_id].wd%AF_MAX_WD_NUM]  = cur_data[AF_DETECT_NUM_V];
	
//	printf("%.1f %d %d\n",now_um, (int)now_dac, (int)g_work[focus_id].roi.af_data[AF_DETECT_NUM_H0][g_cnt[focus_id].wd%AF_MAX_WD_NUM]);

	for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num+=2){
		if( g_work[focus_id].roi.af_max_data[detect_num] < g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd] ){
			g_work[focus_id].roi.af_max_data[detect_num] = g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd];
			g_work[focus_id].roi.af_max_step[detect_num] = g_cnt[focus_id].wd;
		}
		if( g_work[focus_id].roi.af_min_data[detect_num] > g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd] ){
			g_work[focus_id].roi.af_min_data[detect_num] = g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd];
			g_work[focus_id].roi.af_min_step[detect_num] = g_cnt[focus_id].wd;
		}
	}
	
	if( g_cnt[focus_id].wd > 1 ){
		for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num+=2){
			if( g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd] < g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd -1]){
				g_work[focus_id].roi.af_down_cnt[detect_num]++;
			}else{
				g_work[focus_id].roi.af_down_cnt[detect_num] = 0;
			}
		}
	}
	
	if( g_cnt[focus_id].wd >= 4 ){
		for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num+=2){
			if( g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-4] < g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-3]
			&&	g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-3] < g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-2]
			&&	g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-2] > g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1]
			&&	g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1] > g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-0] ){
				if( g_work[focus_id].roi.af_2up2down_data[detect_num] < g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-2] ){
					g_work[focus_id].roi.af_2up2down_data[detect_num] = g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-2];
					g_work[focus_id].roi.af_2up2down_step[detect_num] = g_cnt[focus_id].wd-2;
				}
				g_work[focus_id].roi.af_find_2up2down[detect_num] = ON;

				if( ((g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-4] <= g_work[focus_id].roi.af_max_data[detect_num] * g_work[focus_id].find_peak_rate) ||
					 (g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-3] <= g_work[focus_id].roi.af_max_data[detect_num] * g_work[focus_id].find_peak_rate) )
				&&	((g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1] >= g_work[focus_id].roi.af_max_data[detect_num] * g_work[focus_id].find_peak_rate) ||
					 (g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-0] >= g_work[focus_id].roi.af_max_data[detect_num] * g_work[focus_id].find_peak_rate) )
				){
					g_work[focus_id].find_peak = ON;
				}
			}
		}
	}

	if( g_cnt[focus_id].wd >= 3 ){
		for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num+=2){
			if( g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-3] < g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-2]
			&&	g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-2] < g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1]
			&&	g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1] > g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-0] ){
				if( g_work[focus_id].roi.af_2up1down_data[detect_num] < g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1] ){
					g_work[focus_id].roi.af_2up1down_data[detect_num] = g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1];
					g_work[focus_id].roi.af_2up1down_step[detect_num] = g_cnt[focus_id].wd-1;
				}
				g_work[focus_id].roi.af_find_2up1down[detect_num] = ON;
				if( ((g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-3] <= g_work[focus_id].roi.af_max_data[detect_num] * g_work[focus_id].find_peak_rate) ||
					 (g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-2] <= g_work[focus_id].roi.af_max_data[detect_num] * g_work[focus_id].find_peak_rate) )
				&&	(g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-0] <= g_work[focus_id].roi.af_max_data[detect_num] * g_work[focus_id].find_peak_rate)
				&&	g_work[focus_id].conti == ON
				){
					g_work[focus_id].find_peak = ON;
				}
			}
			if( g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-3] < g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-2]
			&&	g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-2] > g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1]
			&&	g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1] > g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-0] ){
				if( g_work[focus_id].roi.af_1up2down_data[detect_num] < g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-2] ){
					g_work[focus_id].roi.af_1up2down_data[detect_num] = g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-2];
					g_work[focus_id].roi.af_1up2down_step[detect_num] = g_cnt[focus_id].wd-2;
				}
				g_work[focus_id].roi.af_find_1up2down[detect_num] = ON;
				if( (g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-3] <= g_work[focus_id].roi.af_max_data[detect_num] )
				&&	((g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1] >= g_work[focus_id].roi.af_max_data[detect_num] * g_work[focus_id].find_peak_rate) ||
					 (g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-0] >= g_work[focus_id].roi.af_max_data[detect_num] * g_work[focus_id].find_peak_rate) )
				&& g_work[focus_id].conti == ON
				){
					g_work[focus_id].find_peak = ON;
				}
			}
		}
	}

	if( g_cnt[focus_id].wd >= 2 ){
		for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num+=2){
			if( g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-2] < g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1]
			&&	g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1] > g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-0] ){
				if( g_work[focus_id].roi.af_1up1down_data[detect_num] < g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1] ){
					g_work[focus_id].roi.af_1up1down_data[detect_num] = g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1];
					g_work[focus_id].roi.af_1up1down_step[detect_num] = g_cnt[focus_id].wd-1;
				}
				g_work[focus_id].roi.af_find_1up1down[detect_num] = ON;
			}
		}
	}

	if( g_cnt[focus_id].wd == 1 ){
		for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num+=2){
			if( g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1] > g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-0] ){
				g_work[focus_id].roi.af_start_down_data[detect_num] = g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1];
				g_work[focus_id].roi.af_start_down_step[detect_num] = g_cnt[focus_id].wd-1;
				g_work[focus_id].roi.af_find_start_down[detect_num] = ON;
				g_work[focus_id].roi.af_start_down_cnt[detect_num] ++;
			}
		}
	}

	if( g_cnt[focus_id].wd > 1 ){
		for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num+=2){
			if( g_work[focus_id].roi.af_start_down_cnt[detect_num] == g_cnt[focus_id].wd - 1
			&& g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1] > g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-0] ){
				g_work[focus_id].roi.af_start_down_cnt[detect_num] ++;
			}
		}
	}

	#if 0
	for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num+=2){
		if( (g_work[focus_id].roi.af_start_down_cnt[AF_DETECT_NUM_H0] > 2) && (g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd] < g_work[focus_id].roi.af_max_data[detect_num] * 0.95) ){
			if( g_work[focus_id].dir == E_CAMERA_FOCUS_MOVE_DIR_FORWARD){
				caf_init(focus_id, 0, g_work[focus_id].lens_um[g_cnt[focus_id].wd], DIR_MAC2INF);
			}else{
				caf_init(focus_id, 0, g_work[focus_id].lens_um[g_cnt[focus_id].wd], DIR_INF2MAC);
			}
			g_cnt[focus_id].vd = 0;
			g_cnt[focus_id].wd = 1;
			return;
		}
	}
	#endif
			
	if( g_work[focus_id].step_num == 0 ){
		next = next_lens_um_from_DoF(focus_id, g_work[focus_id].lens_um[g_cnt[focus_id].wd], g_work[focus_id].dir);
	}else if( g_work[focus_id].step_num == 0xFF ){
		if((g_work[focus_id].last_um - g_work[focus_id].start_um) > 0 ){
			next = g_work[focus_id].lens_um[g_cnt[focus_id].wd] + g_work[focus_id].fixed_step;
		}else{
			next = g_work[focus_id].lens_um[g_cnt[focus_id].wd] - g_work[focus_id].fixed_step;
		}
	}else{
		next = g_work[focus_id].lens_um[g_cnt[focus_id].wd] + ( (g_work[focus_id].last_um - g_work[focus_id].start_um ) / g_work[focus_id].step_num );
	}
		
	if( g_work[focus_id].find_peak == ON ){
		for(detect_num = 0; detect_num < 1; detect_num+=2){
			if( g_work[focus_id].roi.af_find_2up2down[detect_num] == ON ){
				if( af_least_sq_method(focus_id, g_work[focus_id].roi.af_2up2down_step[detect_num]-2, g_work[focus_id].roi.af_2up2down_step[detect_num], 5, detect_num ) ){
					g_work[focus_id].roi.af_roi_calc_peak_um = g_work[focus_id].lens_um[ g_work[focus_id].roi.af_2up2down_step[detect_num] ];
				}
			}else if( g_work[focus_id].roi.af_find_2up1down[detect_num] == ON ){
				if( af_least_sq_method(focus_id, g_work[focus_id].roi.af_2up1down_step[detect_num]-2, g_work[focus_id].roi.af_2up1down_step[detect_num], 4, detect_num ) ){
					g_work[focus_id].roi.af_roi_calc_peak_um = g_work[focus_id].lens_um[ g_work[focus_id].roi.af_2up1down_step[detect_num] ];
				}
			}else if( g_work[focus_id].roi.af_find_1up2down[detect_num] == ON ){
				if( af_least_sq_method(focus_id, g_work[focus_id].roi.af_1up2down_step[detect_num]-1, g_work[focus_id].roi.af_1up2down_step[detect_num], 4, detect_num ) ){
					g_work[focus_id].roi.af_roi_calc_peak_um = g_work[focus_id].lens_um[ g_work[focus_id].roi.af_1up2down_step[detect_num] ];
				}
			}
		}

		g_work[focus_id].ope = AF_OPE_SUCCESS;
		g_work[focus_id].target_pos_um = g_work[focus_id].roi.af_roi_calc_peak_um;
	}else if( (( g_work[focus_id].dir == E_CAMERA_FOCUS_MOVE_DIR_FORWARD && next <= g_work[focus_id].last_um) || ( g_work[focus_id].dir == E_CAMERA_FOCUS_MOVE_DIR_BACKWARD && next >= g_work[focus_id].last_um)) && (af_end_flg != ON)){
		g_work[focus_id].lens_um[g_cnt[focus_id].wd+1] = next;
		g_work[focus_id].target_pos_um = next;
	}else{
		g_work[focus_id].ope = AF_OPE_FAIL;
		g_work[focus_id].target_pos_um = g_work[focus_id].fail_um;
				
		for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num+=2){
			if( g_work[focus_id].roi.af_find_2up2down[detect_num] == ON
			&&	g_work[focus_id].roi.af_2up2down_data[detect_num] >=g_work[focus_id].roi.af_max_data[detect_num] * 1){
				if( af_least_sq_method(focus_id, g_work[focus_id].roi.af_2up2down_step[detect_num]-2, g_work[focus_id].roi.af_2up2down_step[detect_num], 5, detect_num ) ){
					g_work[focus_id].roi.af_roi_calc_peak_um = g_work[focus_id].lens_um[ g_work[focus_id].roi.af_2up2down_step[detect_num] ];
				}
				g_work[focus_id].ope = AF_OPE_SUCCESS;
				g_work[focus_id].target_pos_um = g_work[focus_id].roi.af_roi_calc_peak_um;
				return;
			}
			if( g_work[focus_id].roi.af_find_2up1down[detect_num] == ON
			&&	g_work[focus_id].roi.af_2up1down_data[detect_num] >=g_work[focus_id].roi.af_max_data[detect_num] * 1){
				if( af_least_sq_method(focus_id, g_work[focus_id].roi.af_2up1down_step[detect_num]-2, g_work[focus_id].roi.af_2up1down_step[detect_num], 4, detect_num ) ){
					g_work[focus_id].roi.af_roi_calc_peak_um = g_work[focus_id].lens_um[ g_work[focus_id].roi.af_2up1down_step[detect_num] ];
				}
				g_work[focus_id].ope = AF_OPE_SUCCESS;
				g_work[focus_id].target_pos_um = g_work[focus_id].roi.af_roi_calc_peak_um;
				return;
			}
			if( g_work[focus_id].roi.af_find_1up2down[detect_num] == ON
			&&	g_work[focus_id].roi.af_1up2down_data[detect_num] >=g_work[focus_id].roi.af_max_data[detect_num] * 1){
				if( af_least_sq_method(focus_id, g_work[focus_id].roi.af_1up2down_step[detect_num]-1, g_work[focus_id].roi.af_1up2down_step[detect_num], 4, detect_num ) ){
					g_work[focus_id].roi.af_roi_calc_peak_um = g_work[focus_id].lens_um[ g_work[focus_id].roi.af_1up2down_step[detect_num] ];
				}
				g_work[focus_id].ope = AF_OPE_SUCCESS;
				g_work[focus_id].target_pos_um = g_work[focus_id].roi.af_roi_calc_peak_um;
				return;
			}
			if( g_work[focus_id].roi.af_find_1up1down[detect_num] == ON
			&&	g_work[focus_id].roi.af_1up1down_data[detect_num] >=g_work[focus_id].roi.af_max_data[detect_num] * 1){
				if( af_least_sq_method(focus_id, g_work[focus_id].roi.af_1up1down_step[detect_num]-1, g_work[focus_id].roi.af_1up1down_step[detect_num], 3, detect_num ) ){
					g_work[focus_id].roi.af_roi_calc_peak_um = g_work[focus_id].lens_um[ g_work[focus_id].roi.af_1up1down_step[detect_num] ];
				}
				g_work[focus_id].ope = AF_OPE_SUCCESS;
				g_work[focus_id].target_pos_um = g_work[focus_id].roi.af_roi_calc_peak_um;
				return;
			}
		}

		if( g_cnt[focus_id].wd >= 2 ){
			for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num++){
				if( g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-2] < g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1]
				&&	g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-1] < g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-0] ){
					g_work[focus_id].roi.af_last_up_data[detect_num] = g_work[focus_id].roi.af_data[detect_num][g_cnt[focus_id].wd-0];
					g_work[focus_id].roi.af_last_up_step[detect_num] = g_cnt[focus_id].wd-0;
					g_work[focus_id].roi.af_find_last_up[detect_num] = ON;
				}
			}
		}
		for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num+=2){
			if( g_work[focus_id].roi.af_find_last_up[detect_num] == ON ){
				for( cnt=g_cnt[focus_id].wd; cnt>0; cnt-- ){
					if( g_work[focus_id].roi.af_data[detect_num][cnt-1] < g_work[focus_id].roi.af_data[detect_num][cnt] ){
						g_work[focus_id].roi.af_last_up_cnt[detect_num] ++;
					}else{
						break;
					}
				}
			}
		}
		
		for(detect_num = 0; detect_num < AF_DETECT_NUM_MAX; detect_num+=2){
			if( g_work[focus_id].roi.af_find_last_up[detect_num] == ON && (g_work[focus_id].roi.af_start_down_cnt <= g_work[focus_id].roi.af_last_up_cnt) ){
				g_work[focus_id].roi.af_roi_calc_peak_um = g_work[focus_id].lens_um[ g_work[focus_id].roi.af_last_up_step[detect_num] ];
				if( g_work[focus_id].dir == E_CAMERA_FOCUS_MOVE_DIR_FORWARD ){
					g_work[focus_id].ope = AF_OPE_EDGE_MAC;
				}else{
					g_work[focus_id].ope = AF_OPE_EDGE_INF;
				}
				g_work[focus_id].target_pos_um = g_work[focus_id].roi.af_roi_calc_peak_um;
				return;
			}
			if( g_work[focus_id].roi.af_find_start_down[detect_num] == ON
			&&	g_work[focus_id].roi.af_start_down_data[detect_num] >=g_work[focus_id].roi.af_max_data[detect_num] * 1){
				g_work[focus_id].roi.af_roi_calc_peak_um = g_work[focus_id].lens_um[ g_work[focus_id].roi.af_start_down_step[detect_num] ];
				if( g_work[focus_id].dir == E_CAMERA_FOCUS_MOVE_DIR_FORWARD ){
					g_work[focus_id].ope = AF_OPE_EDGE_INF;
				}else{
					g_work[focus_id].ope = AF_OPE_EDGE_MAC;
				}
				g_work[focus_id].target_pos_um = g_work[focus_id].roi.af_roi_calc_peak_um;
				return;
			}
			if( g_work[focus_id].roi.af_find_last_up[detect_num] == ON
			&&	g_work[focus_id].roi.af_last_up_data[detect_num] >=g_work[focus_id].roi.af_max_data[detect_num] * 1 ){
				g_work[focus_id].roi.af_roi_calc_peak_um = g_work[focus_id].lens_um[ g_work[focus_id].roi.af_last_up_step[detect_num] ];
				if( g_work[focus_id].dir == E_CAMERA_FOCUS_MOVE_DIR_FORWARD ){
					g_work[focus_id].ope = AF_OPE_EDGE_MAC;
				}else{
					g_work[focus_id].ope = AF_OPE_EDGE_INF;
				}
				g_work[focus_id].target_pos_um = g_work[focus_id].roi.af_roi_calc_peak_um;
				return;
			}
		}
	}
}

static void get_cafdata_by_win(E_CAMERA_ID id, E_SHARE_AF_WINDOW win)
{
	int i;
	for(i=14;i>0;i--){
		g_af_wddata0_history[id][i] = g_af_wddata0_history[id][i-1];
		g_af_wddatav_history[id][i] = g_af_wddatav_history[id][i-1];
		g_af_afdatas_history[id][i] = g_af_afdatas_history[id][i-1];
	}
	if( g_share_af_data.sensor[id].window[win].enable ){
		g_af_wddata0_history[id][0] = g_share_af_data.sensor[id].window[win].data[E_SHARE_AF_DATAH0];
		g_af_wddatav_history[id][0] = g_share_af_data.sensor[id].window[win].data[E_SHARE_AF_DATAV];
		g_af_afdatas_history[id][0] = g_share_af_data.sensor[id].window[win].data[E_SHARE_AF_DATAS];
	}else{
		g_af_wddata0_history[id][0] = 0;
		g_af_wddatav_history[id][0] = 0;
		g_af_afdatas_history[id][0] = 0;
	}
}

static void get_afdata_by_win(E_CAMERA_ID id, E_SHARE_AF_WINDOW win, ULLONG* af_data)
{
	if( g_share_af_data.sensor[id].window[win].enable ){
		af_data[AF_DETECT_NUM_H0] = g_share_af_data.sensor[id].window[win].data[E_SHARE_AF_DATAH0];
		af_data[AF_DETECT_NUM_H1] = g_share_af_data.sensor[id].window[win].data[E_SHARE_AF_DATAH1];
		af_data[AF_DETECT_NUM_V] = g_share_af_data.sensor[id].window[win].data[E_SHARE_AF_DATAV];
	}else{
		af_data[AF_DETECT_NUM_H0] = 0;
		af_data[AF_DETECT_NUM_H1] = 0;
		af_data[AF_DETECT_NUM_V] = 0;
	}
}

static void get_afdata_sum(E_CAMERA_ID id, ULLONG* af_data)
{
	E_SHARE_AF_WINDOW	window_id;

	for( window_id = E_SHARE_AF_WINDOW_0; window_id < E_SHARE_AF_WINDOW_MAX; window_id ++ ){
		if( g_share_af_data.sensor[id].window[window_id].enable ){
			af_data[AF_DETECT_NUM_H0] = af_data[AF_DETECT_NUM_H0] + g_share_af_data.sensor[id].window[window_id].data[E_SHARE_AF_DATAH0];
			af_data[AF_DETECT_NUM_H1] = af_data[AF_DETECT_NUM_H1] + g_share_af_data.sensor[id].window[window_id].data[E_SHARE_AF_DATAH1];
			af_data[AF_DETECT_NUM_V] = af_data[AF_DETECT_NUM_V] + g_share_af_data.sensor[id].window[window_id].data[E_SHARE_AF_DATAV];
		}
	}
}

static void caf_interval_timer_handler( void )
{
	g_caf_start_trig = TRUE;

	Dd_TMR32_Stop(D_CAF_TIMER_CH);
	Dd_TMR32_Close(D_CAF_TIMER_CH);
}

static void caf_interval_timer_start(ULONG usec)
{
	INT32		ret;
	
	g_caf_start_trig = FALSE;

	Dd_TMR32_Stop(D_CAF_TIMER_CH);
	Dd_TMR32_Close(D_CAF_TIMER_CH);
	
	ret = Dd_TMR32_Open( D_CAF_TIMER_CH, D_DDIM_USER_SEM_WAIT_POL );
	if (ret != D_DDIM_OK) {
		UPRINTF("\ntimer(%d) open error : ret=%d", D_CAF_TIMER_CH, ret);
		return;
	}
	
	ret = Dd_TMR32_SetTimer( D_CAF_TIMER_CH, usec, caf_interval_timer_handler );
	if(ret != D_DDIM_OK) {
		Dd_TMR32_Stop(D_CAF_TIMER_CH);
		Dd_TMR32_Close(D_CAF_TIMER_CH);
		UPRINTF("\ntimer(%d) set error : ret=%d", D_CAF_TIMER_CH, ret);
		return;
	}
	
	ret = Dd_TMR32_Start(D_CAF_TIMER_CH);
	if(ret != D_DDIM_OK) {
		UPRINTF("\ntimer(%d) start error : ret=%d", D_CAF_TIMER_CH, ret);
		return;
	}

	return;

}

static signed int lsm_set_inv_matrix( FLOAT mtx[][4] )
{
	INT32 cnt1, cnt2, cnt3;
	DOUBLE tmp1, tmp2;

	for (cnt1 = 0; cnt1 < 3; cnt1++) {
		tmp1 = mtx[cnt1][cnt1];
		if( tmp1 == 0 ){
			return NG;
		}
		for (cnt2 = 0; cnt2 < 3; cnt2++){
			mtx[cnt1][cnt2] /= tmp1;
		}
		mtx[cnt1][cnt1] = 1 / tmp1;
		for (cnt3 = 0; cnt3 < 3; cnt3++){
			if (cnt3 != cnt1) {
				tmp2 = mtx[cnt3][cnt1];
				for (cnt2 = 0; cnt2 < 3; cnt2++){
					if (cnt2 != cnt1){
						mtx[cnt3][cnt2] -= mtx[cnt1][cnt2] * tmp2;
					}
					else{
						mtx[cnt3][cnt2] = -tmp2 / tmp1;
					}
				}
			}
		}
	}
	return OK;
}

static void lsm_set_matrix( FLOAT mtx[][4], FLOAT* x, FLOAT* y, INT32 max_x )
{
	INT32 cnt;
	
	for( cnt = 0; cnt < max_x; cnt++ ){
		mtx[0][0] += *x * *x * *x * *x;
		mtx[0][1] += *x * *x * *x;
		mtx[1][0] += *x * *x * *x;
		mtx[0][2] += *x * *x;
		mtx[1][1] += *x * *x;
		mtx[2][0] += *x * *x;
		mtx[1][2] += *x;
		mtx[2][1] += *x;
		mtx[2][2] ++; 
		
		mtx[0][3] += *y * *x * *x;
		mtx[1][3] += *y * *x;
		mtx[2][3] += *y;
		x++;
		y++;
	}
}

static signed int Least_Square_Method( float* x, float* y, int max_x, float* result_para )
{
	FLOAT mtx[3][4];
	INT32 result;
	
	mtx[0][0] = mtx[0][1] = mtx[0][2] = mtx[0][3] 
	= mtx[1][0] = mtx[1][1] = mtx[1][2] = mtx[1][3]
	= mtx[2][0] = mtx[2][1] = mtx[2][2] = mtx[2][3] = (FLOAT)0;
	
	lsm_set_matrix( mtx, x, y, max_x );
	result = lsm_set_inv_matrix( mtx );
	if( result == NG ){
		result_para[0] = result_para[1] = result_para[2] = (FLOAT)0;
		return NG;
	}
	result_para[0] = mtx[0][0] * mtx[0][3] + mtx[0][1] * mtx[1][3] + mtx[0][2] * mtx[2][3];
	result_para[1] = mtx[1][0] * mtx[0][3] + mtx[1][1] * mtx[1][3] + mtx[1][2] * mtx[2][3];
	result_para[2] = mtx[2][0] * mtx[0][3] + mtx[2][1] * mtx[1][3] + mtx[2][2] * mtx[2][3];
	return OK;
}

static int af_least_sq_method(E_CAMERA_ID focus_id, unsigned char startx, unsigned char peakx, unsigned char use_num, unsigned char use_wd )
{
	float x[5], y[5], abc[3];
	signed int icnt, err;
	
	if( use_num < 3 || 5 < use_num ){
		return NG;
	}
	
	for(icnt=0 ; icnt<use_num ; icnt++){
		x[icnt] = g_work[focus_id].lens_um[startx + icnt];
		y[icnt] = g_work[focus_id].roi.af_data[use_wd][startx + icnt];
	}
	
	err = Least_Square_Method( x, y, use_num,  abc);
	if( err == NG ){
		return NG;
	}
	
	 g_work[focus_id].roi.af_roi_calc_peak_um = -abc[1] / (2 * abc[0]);
#if 1	
	if( g_work[focus_id].dir == E_CAMERA_FOCUS_MOVE_DIR_FORWARD ){
		if( (g_work[focus_id].roi.af_roi_calc_peak_um < g_work[focus_id].lens_um[peakx - 1])
		   || (g_work[focus_id].roi.af_roi_calc_peak_um > g_work[focus_id].lens_um[peakx + 1]) ){
			return NG;
		}
	}else
	{
		if( (g_work[focus_id].roi.af_roi_calc_peak_um > g_work[focus_id].lens_um[peakx - 1])
		   || (g_work[focus_id].roi.af_roi_calc_peak_um < g_work[focus_id].lens_um[peakx + 1]) ){
			return NG;
		}
	}
#endif
	return OK;
}

static float next_lens_um_from_DoF(E_CAMERA_ID focus_id, float now_um, char dir)
{
	int cnt, cnt2;
	int now_idx;
	
	if( dir == E_CAMERA_FOCUS_MOVE_DIR_FORWARD ){
		for(cnt=0; cnt<g_dof_macro_max; cnt++){
			if( CAMERA.focus[focus_id].dof[cnt].lens_um > now_um ){
				if( cnt == 0 ){
					return CAMERA.focus[focus_id].dof[0].lens_um;
				}
				now_idx = cnt-1;
				if( CAMERA.focus[focus_id].dof[now_idx].adj_on == ON ){
					return CAMERA.focus[focus_id].dof[now_idx].adj_lens_um;
				}
				for(cnt2=cnt; cnt2<g_dof_macro_max-1; cnt2++){
					if( CAMERA.focus[focus_id].dof[now_idx].obj_near_m <= CAMERA.focus[focus_id].dof[cnt2].obj_far_m ){
						if( CAMERA.focus[focus_id].dof[now_idx].obj_near_m > CAMERA.focus[focus_id].dof[cnt2+1].obj_far_m ){
							return CAMERA.focus[focus_id].dof[cnt2].lens_um;
						}
					}
				}
				return CAMERA.focus[focus_id].dof[cnt].lens_um;
			}
		}
		return CAMERA.focus[focus_id].dof[g_dof_macro_max-1].lens_um + 1;
	}else{
		for(cnt=g_dof_macro_max-1; cnt>=0; cnt--){
			if( CAMERA.focus[focus_id].dof[cnt].lens_um < now_um ){
				if( cnt == g_dof_macro_max-1 ){
					return CAMERA.focus[focus_id].dof[0].lens_um;
				}
				now_idx = cnt+1;
				if( CAMERA.focus[focus_id].dof[now_idx].adj_on == ON ){
					return CAMERA.focus[focus_id].dof[now_idx].adj_lens_um;
				}
				for(cnt2=cnt; cnt2>=1; cnt2--){
					if( CAMERA.focus[focus_id].dof[now_idx].obj_far_m >= CAMERA.focus[focus_id].dof[cnt2].obj_near_m ){
						if( CAMERA.focus[focus_id].dof[now_idx].obj_far_m < CAMERA.focus[focus_id].dof[cnt2-1].obj_near_m ){
							return CAMERA.focus[focus_id].dof[cnt2].lens_um;
						}
					}
				}
				return CAMERA.focus[focus_id].dof[cnt].lens_um;
			}
		}
		return CAMERA.focus[focus_id].dof[0].lens_um - 1;
	}
}

static int af_data_calc_average(E_CAMERA_ID id, int target_cnt)
{
	int i;
	static int now_ave_cnt[E_CAMERA_ID_MAX];

	if(target_cnt == OFF){
		now_ave_cnt[id] = 0;
		return NG;
	}else if(target_cnt > 15){
		return NG;
	}else if(target_cnt < 2){
		target_cnt = 2;
	}

	g_af_wddata0_ave[id] = 0;
	g_af_wddatav_ave[id] = 0;
	g_af_afdatas_ave[id] = 0;

	for(i=0;i<target_cnt;i++){
		g_af_wddata0_ave[id] += g_af_wddata0_history[id][i];
		g_af_wddatav_ave[id] += g_af_wddatav_history[id][i];
		g_af_afdatas_ave[id] += g_af_afdatas_history[id][i];
	}
	g_af_wddata0_ave[id] /= target_cnt;
	g_af_wddatav_ave[id] /= target_cnt;
	g_af_afdatas_ave[id] /= target_cnt;

	now_ave_cnt[id]++;
	if(now_ave_cnt[id] >= target_cnt){
		return OK;
	}else{
		return NG;
	}
}

static int judge_stable(E_CAMERA_ID id, E_SHARE_AF_WINDOW win)
{
	#define STABLE_CNT  5

	int ret;
	int i;
	static unsigned int stable_afdatah[STABLE_CNT];
	static unsigned int stable_afdatav[STABLE_CNT];
	static INT32		stable_Y[STABLE_CNT];
	static INT32		afh0_stable_cnt;
	static INT32		afv_stable_cnt;
	static INT32		y_stable_cnt;

	unsigned int wddatah0 = g_share_af_data.sensor[id].window[win].data[E_SHARE_AF_DATAH0];
	unsigned int wddatav = g_share_af_data.sensor[id].window[win].data[E_SHARE_AF_DATAV];
	signed int ydata = g_share_af_data.sensor[id].window[win].data[E_SHARE_AF_DATAS];

	// initialize
	if(g_cnt[id].stable_cnt==0){
		afh0_stable_cnt = 0;
		afv_stable_cnt  = 0;
		y_stable_cnt    = 0;
	}

	if(g_cnt[id].stable_cnt < STABLE_CNT){
		stable_afdatah[g_cnt[id].stable_cnt] = wddatah0;
		stable_afdatav[g_cnt[id].stable_cnt] = wddatav;
		stable_Y[g_cnt[id].stable_cnt]		= ydata;
	}else{
		for(i=0;i<STABLE_CNT;i++){
			if((my_abs(stable_afdatah[i] - wddatah0) < (wddatah0 * 0.10)) || (my_abs(stable_afdatah[i] - wddatah0) < (1500)) ){
				afh0_stable_cnt++;
			}else{
				afh0_stable_cnt = 0;
			}
			if(my_abs(stable_afdatav[i] - wddatav) < (wddatav * 0.10) || (my_abs(stable_afdatav[i] - wddatav) < (1500))){
				afv_stable_cnt++;
			}else{
				afv_stable_cnt = 0;
			}
			if(my_abs(stable_Y[i] - ydata) < (ydata * 0.10)){
				y_stable_cnt++;
			}else{
				y_stable_cnt = 0;
			}
		}
		for(i=0;i<(STABLE_CNT-1);i++){
			stable_afdatah[i] = stable_afdatah[i+1];
			stable_afdatav[i] = stable_afdatav[i+1];
			stable_Y[i]      = stable_Y[i+1];
		}
		stable_afdatah[STABLE_CNT-1] = wddatah0;
		stable_afdatav[STABLE_CNT-1] = wddatav;
		stable_Y[STABLE_CNT-1]      = ydata;
	}

	g_cnt[id].stable_cnt++;

	{
		if((afh0_stable_cnt >= STABLE_CNT) && (afv_stable_cnt >= STABLE_CNT) && (y_stable_cnt >= STABLE_CNT)){
			g_cnt[id].stable_cnt = 0;
			ret = OK;
		}else{
			ret = NG;
		}
	}
	return ret;
}

static void caf_set_threshold(E_CAMERA_ID id, E_SHARE_AF_WINDOW win)
{
	g_thrs[id].afdatas_h = g_share_af_data.sensor[id].window[win].data[E_SHARE_AF_DATAS] * 1.24;
	g_thrs[id].afdatas_l = g_share_af_data.sensor[id].window[win].data[E_SHARE_AF_DATAS] * 0.76;

	if(g_work[id].roi.af_max_data[0] < 20000){
		g_thrs[id].wddata_h = 25000;
		g_thrs[id].wddata_l = 0;

	}else{
		if(g_work[id].target_pos_um > CAMERA.focus[id].dof[E_CAMERA_FOCUS_DOF_20CM].lens_um){
			g_thrs[id].wddata_h = g_work[id].roi.af_max_data[0] * 1.6;
			g_thrs[id].wddata_l = g_work[id].roi.af_max_data[0] * 0.5;
		}else{
			g_thrs[id].wddata_h = g_work[id].roi.af_max_data[0] * 1.24;
			g_thrs[id].wddata_l = g_work[id].roi.af_max_data[0] * 0.76;
		}
	}
	if(g_work[id].roi.af_max_data[2] < 20000){
		g_thrs[id].wddatav_h = 25000;
		g_thrs[id].wddatav_l = 0;
	}else{
		if(g_work[id].target_pos_um > CAMERA.focus[id].dof[E_CAMERA_FOCUS_DOF_20CM].lens_um){
			g_thrs[id].wddatav_h = g_work[id].roi.af_max_data[2] * 1.6;
			g_thrs[id].wddatav_l = g_work[id].roi.af_max_data[2] * 0.5;
		}else{
			g_thrs[id].wddatav_h = g_work[id].roi.af_max_data[2] * 1.24;
			g_thrs[id].wddatav_l = g_work[id].roi.af_max_data[2] * 0.76;
		}
	}
}

static int my_abs(int x)
{
  if(x < 0)
    return -x;
  else
    return x;
}

////////////////////Testing_to_Save_PD_data Start///////////////////////////////////////
#ifdef D_FOCUS_DEBUG_SAVE_PD_DATA
#include "fj_filesystem.h"
static void debug_save_file( CHAR* fname, ULONG addr, ULONG size )
{
	ULONG fid;
	ULONG write_size = 0;

	fid = FJ_Fs_Open ( fname, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	FJ_Fs_Write ( fid, (VOID*)addr, size, &write_size );
	FJ_Fs_Close( fid );
}
#endif
////////////////////Testing_to_Save_PD_data Start///////////////////////////////////////

