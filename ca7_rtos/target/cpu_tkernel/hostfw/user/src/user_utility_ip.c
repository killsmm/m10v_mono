/**
 * @file		user_utility_ip.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")
#include "user.h"
#include "user_utility_ip.h"
#include "ip_ahip.h"
#include "ip_fd.h"
#include "ip_pd.h"
#include "gtypes.h"
#include "sdram_map_define.h"
#include "im_iip.h"
#include "os_if.h"
#include "category_parameter.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#ifndef _MIN
#define _MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

#define ROUNDUP_8(val)	(((UINT32)(val) +7) & 0xFFFFFFF8)
#define PD_INPUT_WIDTH 1920
/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_SHARE_FD_DATA		g_fd_info[D_IP_FD_FACE_MAX];
static T_SHARE_PD_PERSON	g_pd_info[D_IP_PD_MAX_PERSONS];

static BOOL					g_fd_task_start = FALSE;

static ULONG g_pd_image_buf;
static void *g_mem_fd = NULL;
static void *g_mem_pd = NULL;
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static int conv_y_image(const T_SHARE_LAYOUT *yuv_src, const ULONG out_addr);
static ULONG user_utility_fd_stop_callback(ULONG param);

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
INT32 User_Utility_FD_Start(const T_SHARE_LAYOUT *yuv)
{
	T_SHARE_SYSTEM_PARAM param;
	T_IP_FD_DETECTOR_CFG cfg;
	INT32 rval = 0;
	T_OS_USER_RTSK	pk_rtsk;

	if(g_fd_task_start == TRUE){
		UPRINTF("FD task already start, wait for stop!\n");
		// wait for terminate
		OS_User_Ref_Tsk( TID_AHIP_FD, &pk_rtsk );
		while( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ) {
			OS_User_Dly_Tsk( 1 );
			OS_User_Ref_Tsk( TID_AHIP_FD, &pk_rtsk );
		}
		g_fd_task_start = FALSE;
	}
	
	g_mem_fd = User_Utility_MM_alloc((D_IP_FD_MEM_HEAP_BUF_SIZE+D_IP_FD_MEM_PRE_BUF_SIZE+D_IP_FD_MEM_BUF_SIZE), 0);
	if (g_mem_fd == NULL) {
		rval = -1;
		UPRINTF_ERR(rval);
		return rval;
	}
	UPRINTF("FD heap addr:0x%lX, size:%ld\n", (ULONG)g_mem_fd, (ULONG)(D_IP_FD_MEM_HEAP_BUF_SIZE+D_IP_FD_MEM_PRE_BUF_SIZE+D_IP_FD_MEM_BUF_SIZE));

	if( yuv->h_size == 0 || yuv->v_size == 0) {
		UPRINTF_ERR(-1);
		return -1;
	}

	rval = IP_AHIP_OpenPlayback(0);
	if( rval ) {
		UPRINTF_ERR(rval);
		return rval;
	}

	rval = IP_FD_Open(0);
	if( rval ) {
		UPRINTF_ERR(rval);
		return rval;
	}

	Get_Share_System_Param(&param);

	cfg.frame.format			= yuv->format == E_SHARE_YUV_FORMAT_420_SEMI ? E_IP_FD_FMT_YUV_420SP : E_IP_FD_FMT_YUV_422SP;
	cfg.frame.width				= yuv->h_size;
	cfg.frame.height			= yuv->v_size;
	cfg.flags					= (USER_UTILITY_FD_TRACKING_MODE | USER_UTILITY_FD_DETECT_EYES);
	cfg.workRes.width			= 640;
	cfg.workRes.height			= 640 * yuv->v_size / yuv->h_size;
	cfg.lockAngles				= NULL;
	cfg.smoothing				= 50;
	cfg.keepFacesOverMoreFrames	= -1;
	cfg.numStaticSearches		= 0;
	cfg.maxFaceCountForFeatures	= 5;
	cfg.maxFaceCount			= D_IP_FD_FACE_MAX;
	cfg.boostFDvsFP				= param.fd_param.boostDR;

	cfg.FdHeapBuffer			= g_mem_fd;
	cfg.FdHeapBufferSize		= D_IP_FD_MEM_HEAP_BUF_SIZE;
	cfg.PreBuffer				= (void*)(cfg.FdHeapBuffer + cfg.FdHeapBufferSize);
	cfg.PreBufferSize			= D_IP_FD_MEM_PRE_BUF_SIZE;
	cfg.FdBuffer				= (void*)(cfg.PreBuffer + cfg.PreBufferSize);
	cfg.FdBufferSize			= D_IP_FD_MEM_BUF_SIZE;
	cfg.minFaceSize				= 20;

	rval = IP_FD_Init(0, &cfg, E_IP_FD_PLAYBACK);
	if( rval ) {
		UPRINTF_ERR(rval);
		return rval;
	}

	Get_Share_System_Param(&param);
	param.fd_info = (T_SHARE_FD_DATA*)&g_fd_info[0];
	Cate_Get_System_Param( &param, D_CONFIG_MODE_VIEW );
	Set_Share_System_Param(&param);

	IP_FD_Set_Param(0, (T_IP_FD_PARAM *)&param.fd_param);
	IP_FD_Task_Start(0);
	g_fd_task_start = TRUE;

	return rval;
}

INT32 User_Utility_FD_Stop(const FJ_DISP_TYPE disp_type)
{
	BOOL sync = FALSE;
	T_SHARE_SYSTEM_PARAM param;
	INT32 rval = 0;

	User_Utility_MM_free(g_mem_fd, 0);

	rval = FJ_OSD_Set_Face_Frame_Enable(disp_type, 0, 0);
	UPRINTF_ERR(rval);

	if(sync == TRUE){
		IP_FD_Task_End(0, NULL);

		rval = IP_FD_Close(0);
		UPRINTF_ERR(rval);

		Get_Share_System_Param(&param);
		param.fd_info = NULL;
		param.num_of_face = 0;
		Cate_Get_System_Param( &param, D_CONFIG_MODE_VIEW );
		Set_Share_System_Param(&param);
		g_fd_task_start = FALSE;
	}
	else{
		IP_FD_Task_End(0, user_utility_fd_stop_callback);
	}

	return rval;
}


VOID User_Utility_FD_Detect_One_Frame(T_SHARE_LAYOUT *yuv)
{
	T_SHARE_SYSTEM_PARAM param = {0};
	T_IP_FD_PLANE_DATA img;
	USHORT width = 0, height = 0;
	UCHAR num_of_faces = 0;

	Get_Share_System_Param(&param);
	img.plane[0].stride = yuv->h_byte;
	img.plane[0].data	= (void*)yuv->addr_y;
	img.plane[1].stride = yuv->h_byte;
	img.plane[1].data	= (void*)yuv->addr_c;
	// YUV420 semi, the following plane is no needed
	img.plane[2].stride = 0;
	img.plane[2].data	= (void*)0;
	IP_FD_Set_Param(0, (T_IP_FD_PARAM *)&param.fd_param);
	IP_FD_Task_Detect_Start(0, img);

	if( param.fd_info ) {
		memset(param.fd_info, 0, sizeof(T_SHARE_FD_DATA) * D_IP_FD_FACE_MAX);
		IP_FD_Get_Face_List(0, &width, &height, &num_of_faces, (T_IP_FD_DATA*)param.fd_info);
		param.num_of_face = num_of_faces;
		Set_Share_System_Param(&param);
	}
}

INT32 User_Utility_FD_Draw(T_SHARE_YUV *yuv, T_SHARE_LAYOUT *yuv_layout, FJ_DISP_TYPE disp_type, const T_SHARE_FD_DATA *fd_info, UCHAR num_of_face)
{
	T_FACE_FRAME_EACH_SET face_pos[OSD_DRAW_FACE_NUM];
	T_FACE_FRAME_SET face_set;
	UINT32 fd_draw_en = 0;
	UINT32 num_face_confidence = 0;
	int rval = 0;
	int i;
	float x_ratio, y_ratio;

	if( yuv->layout[E_SHARE_YUV_OUT_USER_1].h_size == 0 || yuv->layout[E_SHARE_YUV_OUT_USER_1].v_size == 0 ||
		yuv_layout->h_size == 0 || yuv_layout->v_size == 0) {
		UPRINTF_ERR(-1);
		return -1;
	}
	x_ratio = (float)yuv->layout[E_SHARE_YUV_OUT_USER_1].h_size / (float)yuv_layout->h_size;
	y_ratio = (float)yuv->layout[E_SHARE_YUV_OUT_USER_1].v_size / (float)yuv_layout->v_size;

	num_of_face = num_of_face > OSD_DRAW_FACE_NUM ? OSD_DRAW_FACE_NUM : num_of_face;
	for(i = 0 ; i < num_of_face; i++) {
		if( fd_info[i].confidence >= 10 ) {
			fd_draw_en |= 1 << num_face_confidence;
			face_pos[num_face_confidence].start_pos.Position.StartX	= (UINT32)((float)fd_info[i].rect.left * x_ratio);
			face_pos[num_face_confidence].start_pos.Position.StartY	= (UINT32)((float)fd_info[i].rect.top * y_ratio);
			face_pos[num_face_confidence].start_pos.Width			= (UINT32)((float)fd_info[i].rect.width * x_ratio);
			face_pos[num_face_confidence].start_pos.Height			= (UINT32)((float)fd_info[i].rect.height * y_ratio);
			face_pos[num_face_confidence].v_thickness				= 10;
			face_pos[num_face_confidence].h_thickness				= 10;
			face_pos[num_face_confidence].rgba.RGBA.A				= 2;
			face_pos[num_face_confidence].rgba.RGBA.R				= 0x50;
			face_pos[num_face_confidence].rgba.RGBA.G				= 0x25;
			face_pos[num_face_confidence].rgba.RGBA.B				= 0x10;
			face_pos[num_face_confidence].start_pos.Position.StartX = _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].h_size, face_pos[num_face_confidence].start_pos.Position.StartX);
			face_pos[num_face_confidence].start_pos.Position.StartY = _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].v_size, face_pos[num_face_confidence].start_pos.Position.StartY);
			face_pos[num_face_confidence].start_pos.Width  			= _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].h_size - face_pos[num_face_confidence].start_pos.Position.StartX - (face_pos[num_face_confidence].h_thickness * 2), face_pos[num_face_confidence].start_pos.Width );
			face_pos[num_face_confidence].start_pos.Height 			= _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].v_size - face_pos[num_face_confidence].start_pos.Position.StartY - (face_pos[num_face_confidence].v_thickness * 2), face_pos[num_face_confidence].start_pos.Height);
			num_face_confidence++;

			if(num_face_confidence >= OSD_DRAW_FACE_NUM)
				break;

			if((fd_info[i].eyeL.width != 0) && (fd_info[i].eyeL.height != 0)){
				fd_draw_en |= 1 << num_face_confidence;
				face_pos[num_face_confidence].start_pos.Position.StartX = (UINT32)((float)fd_info[i].eyeL.left * x_ratio);
				face_pos[num_face_confidence].start_pos.Position.StartY = (UINT32)((float)fd_info[i].eyeL.top * y_ratio);
				face_pos[num_face_confidence].start_pos.Width			= (UINT32)((float)fd_info[i].eyeL.width * x_ratio);
				face_pos[num_face_confidence].start_pos.Height			= (UINT32)((float)fd_info[i].eyeL.height * y_ratio);
				face_pos[num_face_confidence].v_thickness				= 10;
				face_pos[num_face_confidence].h_thickness				= 10;
				face_pos[num_face_confidence].rgba.RGBA.A				= 2;
				face_pos[num_face_confidence].rgba.RGBA.R				= 0x25;
				face_pos[num_face_confidence].rgba.RGBA.G				= 0x50;
				face_pos[num_face_confidence].rgba.RGBA.B				= 0x10;
				face_pos[num_face_confidence].start_pos.Position.StartX = _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].h_size, face_pos[num_face_confidence].start_pos.Position.StartX);
				face_pos[num_face_confidence].start_pos.Position.StartY = _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].v_size, face_pos[num_face_confidence].start_pos.Position.StartY);
				face_pos[num_face_confidence].start_pos.Width			= _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].h_size - face_pos[num_face_confidence].start_pos.Position.StartX - (face_pos[num_face_confidence].h_thickness * 2), face_pos[num_face_confidence].start_pos.Width );
				face_pos[num_face_confidence].start_pos.Height			= _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].v_size - face_pos[num_face_confidence].start_pos.Position.StartY - (face_pos[num_face_confidence].v_thickness * 2), face_pos[num_face_confidence].start_pos.Height);
				num_face_confidence++;
			}

			if(num_face_confidence >= OSD_DRAW_FACE_NUM)
				break;

			if((fd_info[i].eyeR.width != 0) && (fd_info[i].eyeR.height != 0)){
				fd_draw_en |= 1 << num_face_confidence;
				face_pos[num_face_confidence].start_pos.Position.StartX = (UINT32)((float)fd_info[i].eyeR.left * x_ratio);
				face_pos[num_face_confidence].start_pos.Position.StartY = (UINT32)((float)fd_info[i].eyeR.top * y_ratio);
				face_pos[num_face_confidence].start_pos.Width			= (UINT32)((float)fd_info[i].eyeR.width * x_ratio);
				face_pos[num_face_confidence].start_pos.Height			= (UINT32)((float)fd_info[i].eyeR.height * y_ratio);
				face_pos[num_face_confidence].v_thickness				= 10;
				face_pos[num_face_confidence].h_thickness				= 10;
				face_pos[num_face_confidence].rgba.RGBA.A				= 2;
				face_pos[num_face_confidence].rgba.RGBA.R				= 0x25;
				face_pos[num_face_confidence].rgba.RGBA.G				= 0x50;
				face_pos[num_face_confidence].rgba.RGBA.B				= 0x10;
				face_pos[num_face_confidence].start_pos.Position.StartX = _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].h_size, face_pos[num_face_confidence].start_pos.Position.StartX);
				face_pos[num_face_confidence].start_pos.Position.StartY = _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].v_size, face_pos[num_face_confidence].start_pos.Position.StartY);
				face_pos[num_face_confidence].start_pos.Width			= _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].h_size - face_pos[num_face_confidence].start_pos.Position.StartX - (face_pos[num_face_confidence].h_thickness * 2), face_pos[num_face_confidence].start_pos.Width );
				face_pos[num_face_confidence].start_pos.Height			= _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].v_size - face_pos[num_face_confidence].start_pos.Position.StartY - (face_pos[num_face_confidence].v_thickness * 2), face_pos[num_face_confidence].start_pos.Height);
				num_face_confidence++;
			}

			if(num_face_confidence >= OSD_DRAW_FACE_NUM)
				break;
		}
	}

	face_set.face_count = num_face_confidence;
	face_set.ih_size = yuv->layout[E_SHARE_YUV_OUT_USER_1].h_size;
	face_set.iv_size = yuv->layout[E_SHARE_YUV_OUT_USER_1].v_size;
	rval = FJ_OSD_Draw_Face_Frame(disp_type, face_pos, &face_set);
	UPRINTF_ERR(rval);
	rval = FJ_OSD_Set_Face_Frame_Enable(disp_type, fd_draw_en, num_face_confidence);
	UPRINTF_ERR(rval);

	return rval;
}

BOOL User_Utility_Is_FD_Running(void)
{
	return g_fd_task_start;
}

INT32 User_Utility_PD_Start(const T_SHARE_LAYOUT *yuv)
{
	T_SHARE_SYSTEM_PARAM param;
	T_IP_PD_DETECTOR_CFG cfg;
	ULONG heap_buf;
	INT32 rval = 0;
	USHORT width, height;
	ULONG img_buf;

	if( yuv->h_size == 0 || yuv->v_size == 0) {
		UPRINTF_ERR(-1);
		return -1;
	}

	rval = IP_AHIP_OpenPlayback(0);
	if( rval ) {
		UPRINTF_ERR(rval);
		return rval;
	}

	rval = IP_PD_Open();
	if( rval ) {
		UPRINTF_ERR(rval);
		return rval;
	}

	if( yuv->h_size > PD_INPUT_WIDTH ) {
		width = PD_INPUT_WIDTH;
		height = PD_INPUT_WIDTH * yuv->v_size / yuv->h_size;
	} else {
		width = yuv->h_size;
		height = yuv->v_size;
	}

	memset(&cfg, 0, sizeof(cfg));
	cfg.flags			= E_IP_PD_AHOF_PERSON_FLAGS_TRACKING_MODE;							// Configuration flags
	cfg.minPersonSize	= D_IP_PD_MIN_PERSON_SIZE;											// Minimum person size in pixels the library should try to detect.
	cfg.maxPersonSize	= width > height ? height : width;									// Maximum person size in pixels the library should try to detect.
	cfg.maxPersonCount	= D_IP_PD_MAX_PERSONS;												// Maximum number of persons to be detected and tracked.
	cfg.boostDRvsFP		= -2;																// Trade-off between detection rate and false positives.
	cfg.boostDRvsSPEED	= 0;																// Trade-off between detection rate and execution speed.
	cfg.width			= width;						// image width
	cfg.width2			= width / 2;					// crop width <-- no used in playback mode
	cfg.height			= height;						// image height
	cfg.planes[0].stride	= width;
	cfg.planes[1].stride	= 0;
	cfg.planes[1].data		= 0;

	if( yuv->h_size > PD_INPUT_WIDTH ) {
		g_mem_pd = User_Utility_MM_alloc((PD_INPUT_WIDTH * (PD_INPUT_WIDTH * yuv->v_size / yuv->h_size)) + IP_PD_GetLibHeapSize(&cfg), 0);
		if (g_mem_pd == NULL) {
			rval = -1;
			UPRINTF_ERR(rval);
			return rval;
		}
		heap_buf = (UINT32)g_mem_pd + PD_INPUT_WIDTH * (PD_INPUT_WIDTH * yuv->v_size / yuv->h_size);
		g_pd_image_buf = img_buf = (UINT32)g_mem_pd;
		rval = conv_y_image(yuv, img_buf);
		if( rval ) {
			UPRINTF_ERR(rval);
			return rval;
		}
	} else {
		g_mem_pd = User_Utility_MM_alloc(IP_PD_GetLibHeapSize(&cfg), 0);
		heap_buf = (UINT32)g_mem_pd;
		img_buf = yuv->addr_y;
	}

	cfg.heapBuffer		= (void*)heap_buf;
	cfg.planes[0].data	= (VOID*)img_buf;
	rval = IP_PD_Init(&cfg);
	if( rval ) {
		UPRINTF_ERR(rval);
		return rval;
	}
	UPRINTF("PD heap addr:0x%lX, size:%ld\n", heap_buf, (ULONG)cfg.heapBuffer - heap_buf);

	Get_Share_System_Param(&param);
	param.pd_info = &g_pd_info[0];
	Cate_Get_System_Param( &param, D_CONFIG_MODE_VIEW );
	Set_Share_System_Param(&param);

	rval = IP_PD_Task_Start();
	UPRINTF_ERR(rval);

	return rval;
}

INT32 User_Utility_PD_Stop(const FJ_DISP_TYPE disp_type)
{
	T_SHARE_SYSTEM_PARAM param;
	INT32 rval = 0;

	User_Utility_MM_free(g_mem_pd, 0);

	IP_PD_Task_End();

	rval = IP_PD_Close();
	UPRINTF_ERR(rval);

	rval = FJ_OSD_Set_Face_Frame_Enable(disp_type, 0, 0);
	UPRINTF_ERR(rval);

	Get_Share_System_Param(&param);
	param.pd_info = NULL;
	Cate_Get_System_Param( &param, D_CONFIG_MODE_VIEW );
	Set_Share_System_Param(&param);

	printf( "IP_PD_Close\n");

	return rval;
}

VOID User_Utility_PD_Detect_One_Frame(T_SHARE_LAYOUT *yuv)
{
	T_SHARE_SYSTEM_PARAM param;
	T_IP_PD_PLANE_DATA img;
	UINT32 num_of_person = 0;
	INT32 rval = 0;
	UINT32 h_byte;
	UINT32 img_buf;

	if( yuv->h_size > PD_INPUT_WIDTH ) {
		rval = conv_y_image(yuv, g_pd_image_buf);
		if( rval ) {
			UPRINTF_ERR(rval);
			return;
		}
		h_byte = PD_INPUT_WIDTH;
		img_buf = g_pd_image_buf;
	} else {
		h_byte = yuv->h_byte;
		img_buf = yuv->addr_y;
	}

	img.plane[0].stride = h_byte;
	img.plane[0].data	= (void*)img_buf;
	img.plane[1].stride = 0;
	img.plane[1].data	= (void*)0;
	// YUV420 semi, the following plane is no needed
	img.plane[2].stride = 0;
	img.plane[2].data	= (void*)0;
	rval = IP_PD_Task_Detect_Start(img);
	if( rval ) {
		UPRINTF_ERR(rval);
		return;
	}

	Get_Share_System_Param(&param);
	memset(param.pd_info, 0, sizeof(T_SHARE_PD_PERSON) * D_IP_PD_MAX_PERSONS);
	rval = IP_PD_Get_Person_List( (T_IP_PD_PERSON*)param.pd_info, &num_of_person );
	param.num_of_person = num_of_person;
	if( rval ) {
		UPRINTF_ERR(rval);
		return;
	}
	Set_Share_System_Param(&param);
}

INT32 User_Utility_PD_Draw(T_SHARE_YUV *yuv, T_SHARE_LAYOUT *yuv_layout, FJ_DISP_TYPE disp_type, const T_SHARE_PD_PERSON *pd_info, UCHAR num_of_person)
{
	T_FACE_FRAME_EACH_SET pd_pos[OSD_DRAW_FACE_NUM];
	T_FACE_FRAME_SET pd_set;
	USHORT pd_draw_en = 0;
	UINT32 num_person_confidence = 0;
	int rval = 0;
	int i;
	float x_ratio, y_ratio;
	USHORT d_width, d_height;

	if( yuv->layout[E_SHARE_YUV_OUT_USER_1].h_size == 0 || yuv->layout[E_SHARE_YUV_OUT_USER_1].v_size == 0 ||
		yuv_layout->h_size == 0 || yuv_layout->v_size == 0) {
		UPRINTF_ERR(-1);
		return -1;
	}

	if( yuv_layout->h_size > PD_INPUT_WIDTH ) {
		d_width		= PD_INPUT_WIDTH;
		d_height	= PD_INPUT_WIDTH * yuv_layout->v_size / yuv_layout->h_size;
	} else {
		d_width		= yuv_layout->h_size;
		d_height	= yuv_layout->v_size;
	}

	x_ratio = (float)yuv->layout[E_SHARE_YUV_OUT_USER_1].h_size / (float)d_width;
	y_ratio = (float)yuv->layout[E_SHARE_YUV_OUT_USER_1].v_size / (float)d_height;

	num_of_person = num_of_person > OSD_DRAW_FACE_NUM ? OSD_DRAW_FACE_NUM : num_of_person;
	for(i = 0 ; i < num_of_person; i++) {
		if( pd_info[i].confidence >= 200 ) {
			pd_draw_en |= 1 << num_person_confidence;
			pd_pos[num_person_confidence].start_pos.Position.StartX	= (UINT32)((float)pd_info[i].rect.left * x_ratio);
			pd_pos[num_person_confidence].start_pos.Position.StartY	= (UINT32)((float)pd_info[i].rect.top * y_ratio);
			pd_pos[num_person_confidence].start_pos.Width			= (UINT32)((float)pd_info[i].rect.width * x_ratio);
			pd_pos[num_person_confidence].start_pos.Height			= (UINT32)((float)pd_info[i].rect.height * y_ratio);
			pd_pos[num_person_confidence].v_thickness				= 10;
			pd_pos[num_person_confidence].h_thickness				= 10;
			pd_pos[num_person_confidence].rgba.RGBA.A				= 2;
			pd_pos[num_person_confidence].rgba.RGBA.R				= 0x50;
			pd_pos[num_person_confidence].rgba.RGBA.G				= 0x25;
			pd_pos[num_person_confidence].rgba.RGBA.B				= 0x10;
			
			pd_pos[num_person_confidence].start_pos.Position.StartX = _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].h_size, pd_pos[num_person_confidence].start_pos.Position.StartX);
			pd_pos[num_person_confidence].start_pos.Position.StartY = _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].v_size, pd_pos[num_person_confidence].start_pos.Position.StartY);
			pd_pos[num_person_confidence].start_pos.Width  			= _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].h_size - pd_pos[num_person_confidence].start_pos.Position.StartX - (pd_pos[num_person_confidence].h_thickness * 2), pd_pos[num_person_confidence].start_pos.Width );
			pd_pos[num_person_confidence].start_pos.Height 			= _MIN(yuv->layout[E_SHARE_YUV_OUT_USER_1].v_size - pd_pos[num_person_confidence].start_pos.Position.StartY - (pd_pos[num_person_confidence].v_thickness * 2), pd_pos[num_person_confidence].start_pos.Height);
			num_person_confidence++;
		}
	}

	pd_set.face_count = num_person_confidence;
	pd_set.ih_size = yuv->layout[E_SHARE_YUV_OUT_USER_1].h_size;
	pd_set.iv_size = yuv->layout[E_SHARE_YUV_OUT_USER_1].v_size;
	rval = FJ_OSD_Draw_Face_Frame(disp_type, pd_pos, &pd_set);
	UPRINTF_ERR(rval);
	if ( rval == 2 ) {
		for(i = 0 ; i < num_person_confidence; i++) {
			printf("x:%d, y:%d, w:%d, h:%d\n", pd_pos[i].start_pos.Position.StartX, pd_pos[i].start_pos.Position.StartY, pd_pos[i].start_pos.Width, pd_pos[i].start_pos.Height);
		}
	}
	rval = FJ_OSD_Set_Face_Frame_Enable(disp_type, pd_draw_en, num_person_confidence);
	UPRINTF_ERR(rval);

	return rval;
}

INT32 User_Utility_PD_Calc_Coodination(UINT32 in_h, UINT32 in_v, UINT32 out_h, UINT32 out_v, T_SHARE_PD_PERSON *pd_info, UCHAR num_of_person)
{
	int i;
	float x_ratio, y_ratio;
	USHORT d_width, d_height;
	if( out_h == 0 || out_v == 0 ||
		in_h == 0 || in_v == 0) {
		UPRINTF_ERR(-1);
		return -1;
	}

	if( in_h > PD_INPUT_WIDTH ) {
		d_width		= PD_INPUT_WIDTH;
		d_height	= PD_INPUT_WIDTH * in_v / in_h;
	} else {
		d_width		= in_h;
		d_height	= in_v;
	}

	x_ratio = (float)out_h / (float)d_width;
	y_ratio = (float)out_v / (float)d_height;

	for(i = 0 ; i < num_of_person; i++) {
		pd_info[i].rect.left = (UINT32)((float)pd_info[i].rect.left * x_ratio);
		pd_info[i].rect.top = (UINT32)((float)pd_info[i].rect.top * y_ratio);
		pd_info[i].rect.width = (UINT32)((float)pd_info[i].rect.width * x_ratio);
		pd_info[i].rect.height = (UINT32)((float)pd_info[i].rect.height * y_ratio);
		// Make sure the box won't over the output width & height
		pd_info[i].rect.width = (pd_info[i].rect.left + pd_info[i].rect.width >  out_h) ? out_h - pd_info[i].rect.left : pd_info[i].rect.width;
		pd_info[i].rect.height = (pd_info[i].rect.top + pd_info[i].rect.height >  out_v) ? out_v - pd_info[i].rect.top : pd_info[i].rect.height;

#ifdef ENABLE_IV_PD
#if !defined (DEBUG_PD_BBOX) && defined (HEIGHT_CLIP)
		pd_info[i].rect.height -= pd_info[i].rect.height * 0.3;;
#endif

#if !defined (DEBUG_PD_BBOX) && defined (FIXED_WIDTH)
		pd_info[i].rect.left += (pd_info[i].rect.width - FIXED_PD_BBOX_WIDTH) / 2;
		pd_info[i].rect.width = FIXED_PD_BBOX_WIDTH;
#endif

#if !defined (DEBUG_PD_BBOX) && defined (FIXED_HEIGHT)
		pd_info[i].rect.top += (pd_info[i].rect.height - FIXED_PD_BBOX_HEIGHT) / 4;
		pd_info[i].rect.height = FIXED_PD_BBOX_HEIGHT;
#endif
#endif
	}

	return 0;
}

INT32 User_Utility_BBox_Draw(UINT32 out_width, UINT32 out_height, UINT32 in_width, UINT32 in_height, FJ_DISP_TYPE disp_type, T_FACE_FRAME_EACH_SET *bbox, UCHAR num_of_box)
{
	T_FACE_FRAME_SET face_set;
	USHORT fd_draw_en = 0;
	int rval = 0;
	int i;
	float x_ratio, y_ratio;

	x_ratio = (float)out_width / in_width;
	y_ratio = (float)out_height / in_height;

	for(i = 0 ; i < num_of_box; i++) {
		fd_draw_en |= 1 << i;
		bbox[i].start_pos.Position.StartX *= x_ratio;
		bbox[i].start_pos.Position.StartY *= y_ratio;
		bbox[i].start_pos.Width *= x_ratio;
		bbox[i].start_pos.Height *= y_ratio;
		bbox[i].start_pos.Position.StartX = _MIN(out_width, bbox[i].start_pos.Position.StartX);
		bbox[i].start_pos.Position.StartY = _MIN(out_height, bbox[i].start_pos.Position.StartY);
		bbox[i].start_pos.Width  			= _MIN(out_width - bbox[i].start_pos.Position.StartX - (bbox[i].h_thickness * 2), bbox[i].start_pos.Width );
		bbox[i].start_pos.Height 			= _MIN(out_height - bbox[i].start_pos.Position.StartY - (bbox[i].v_thickness * 2), bbox[i].start_pos.Height);
	}

	face_set.face_count = num_of_box;
	face_set.ih_size = out_width;
	face_set.iv_size = out_height;
	rval = FJ_OSD_Draw_Face_Frame(disp_type, bbox, &face_set);
	UPRINTF_ERR(rval);
	rval = FJ_OSD_Set_Face_Frame_Enable(disp_type, fd_draw_en, num_of_box);
	UPRINTF_ERR(rval);

	return rval;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static int conv_y_image(const T_SHARE_LAYOUT *yuv_src, const ULONG out_addr)
{
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_RR_BUF_BYTES];
	ULONG param_buffer_addr = ROUNDUP_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 1000;
	T_IM_IIP_UTIL_RR iip_util_rr0;
	int rval = 0;

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_07, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "CONV YUV"));

	memset(&iip_util_rr0, 0, sizeof(T_IM_IIP_UTIL_RR));

	iip_util_rr0.afn_unitid = E_IM_IIP_UNIT_ID_AFN0;
	iip_util_rr0.sl_unitid = E_IM_IIP_UNIT_ID_SL2;
	iip_util_rr0.src_pixid = E_IM_IIP_PIXID_4;
	iip_util_rr0.dst_pixid = E_IM_IIP_PIXID_5;

	iip_util_rr0.src.gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_rr0.src.gbl.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	iip_util_rr0.src.gbl.pix_format = yuv_src->format == E_SHARE_YUV_FORMAT_422_SEMI ? E_IM_IIP_PFMT_YCC422 : E_IM_IIP_PFMT_YCC420;
	iip_util_rr0.src.gbl.chunky_color.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G;
	iip_util_rr0.src.gbl.chunky_color.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B;
	iip_util_rr0.src.gbl.chunky_color.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R;
	iip_util_rr0.src.gbl.chunky_color.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A;
	iip_util_rr0.src.gbl.sign_Y_G = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.src.gbl.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.src.gbl.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.src.gbl.sign_D3 = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.src.gbl.masterIF_Y_G = D_IM_IIP_MASTER_IF0;
	iip_util_rr0.src.gbl.masterIF_Cb_B = D_IM_IIP_MASTER_IF0;
	iip_util_rr0.src.gbl.masterIF_Cr_R = D_IM_IIP_MASTER_IF0;
	iip_util_rr0.src.gbl.masterIF_Alpha = D_IM_IIP_MASTER_IF0;

	iip_util_rr0.dst.gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_rr0.dst.gbl.frame_type = E_IM_IIP_FTYPE_PLANE;
	iip_util_rr0.dst.gbl.pix_format = E_IM_IIP_PFMT_PLANE;
	iip_util_rr0.dst.gbl.chunky_color.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G; // YUYV
	iip_util_rr0.dst.gbl.chunky_color.component1 = D_IM_IIP_CHUNKY_COLOR_Y0_G; // YUYV
	iip_util_rr0.dst.gbl.chunky_color.component2 = D_IM_IIP_CHUNKY_COLOR_Y0_G; // YUYV
	iip_util_rr0.dst.gbl.chunky_color.component3 = D_IM_IIP_CHUNKY_COLOR_Y0_G; // YUYV
	iip_util_rr0.dst.gbl.sign_Y_G = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.dst.gbl.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.dst.gbl.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.dst.gbl.sign_D3 = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.dst.gbl.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.dst.gbl.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.dst.gbl.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.dst.gbl.masterIF_Alpha = D_IM_IIP_MASTER_IF1;

	iip_util_rr0.afn_cache_select = E_IM_IIP_PARAM_CSEL_0;
	iip_util_rr0.pix_depth = D_IM_IIP_PDEPTH_8BITS;
	iip_util_rr0.rotation = E_IM_IIP_UTIL_ROTATE_000;
	iip_util_rr0.flip = E_IM_IIP_UTIL_FLIP_OFF;
	iip_util_rr0.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;

	iip_util_rr0.src.rect.width				= yuv_src->h_size;
	iip_util_rr0.src.rect.lines				= yuv_src->v_size;
	iip_util_rr0.src.gbl.pix_format			= yuv_src->format == E_SHARE_YUV_FORMAT_422_SEMI ? E_IM_IIP_PFMT_YCC422 : E_IM_IIP_PFMT_YCC420;
	iip_util_rr0.src.gbl.width				= yuv_src->h_size;
	iip_util_rr0.src.gbl.lines				= yuv_src->v_size;
	iip_util_rr0.src.gbl.line_bytes.Y_G		= yuv_src->h_byte;
	iip_util_rr0.src.gbl.line_bytes.Cb_B	= yuv_src->h_byte;
	iip_util_rr0.src.gbl.line_bytes.Cr_R	= 0;
	iip_util_rr0.src.gbl.line_bytes.Alpha	= 0;
	iip_util_rr0.src.gbl.addr.Y_G			= yuv_src->addr_y;
	iip_util_rr0.src.gbl.addr.Cb_B			= yuv_src->addr_c;
	iip_util_rr0.src.gbl.addr.Cr_R			= 0;
	iip_util_rr0.src.gbl.addr.Alpha			= 0;

	iip_util_rr0.dst.rect.width				= PD_INPUT_WIDTH;
	iip_util_rr0.dst.rect.lines				= PD_INPUT_WIDTH * yuv_src->v_size / yuv_src->h_size;
	iip_util_rr0.dst.gbl.pix_format			= E_IM_IIP_PFMT_PLANE;
	iip_util_rr0.dst.gbl.width				= PD_INPUT_WIDTH;
	iip_util_rr0.dst.gbl.lines				= PD_INPUT_WIDTH * yuv_src->v_size / yuv_src->h_size;
	iip_util_rr0.dst.gbl.line_bytes.Y_G		= PD_INPUT_WIDTH;
	iip_util_rr0.dst.gbl.line_bytes.Cb_B	= 0;
	iip_util_rr0.dst.gbl.line_bytes.Cr_R	= 0;
	iip_util_rr0.dst.gbl.line_bytes.Alpha	= 0;
	iip_util_rr0.dst.gbl.addr.Y_G			= out_addr;
	iip_util_rr0.dst.gbl.addr.Cb_B			= 0;
	iip_util_rr0.dst.gbl.addr.Cr_R			= 0;
	iip_util_rr0.dst.gbl.addr.Alpha			= 0;

	rval = IM_IIP_Utility_ResizeRotate( &iip_util_rr0,
										NULL,
										param_master_IF,
										param_buffer_addr,
										wait_time );
	UPRINTF_ERR(rval);

	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_07, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "CONV YUV"));

	return rval;
}

static ULONG user_utility_fd_stop_callback(ULONG param)
{
	T_SHARE_SYSTEM_PARAM system_param;
	INT32 rval = 0;

	UPRINTF("Execution %s(%d)\n", __func__, (UINT32)param);

	rval = IP_FD_Close(0);
	UPRINTF_ERR(rval);

	Get_Share_System_Param(&system_param);
	system_param.fd_info = NULL;
	system_param.num_of_face = 0;
	Cate_Get_System_Param( &system_param, D_CONFIG_MODE_VIEW );
	Set_Share_System_Param(&system_param);
	g_fd_task_start = FALSE;

	return rval;
}

