/**
 * @file		user_utility_calib.c
 * @brief		None
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

//#pragma GCC optimize ("O0")
#include "user_utility_calib.h"
#include "category_parameter.h"
#include "fj_filesystem.h"
#include "fj_still.h"
#include "iq_dpc_detect_sw_if.h"
#include "iq_dpc_file.h"
#include "iq_dpc_multipipe.h"
#include "iq_dpc_multipipe_tbl_mng.h"
#include "iq_dpc_tbl_sel.h"
#include "l1l2cache.h"
#include "sdram_map_define.h"
#include "share_data.h"
#include "shd_bin.h"
#include "stdlib.h"
#include "user.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define G_THRES_L_PERCENT  45
#define G_THRES_H_PERCENT  65
#define INTEREST_AREA_DIV  9 // Make it odd
#define INTEREST_AREA_HALF (INTEREST_AREA_DIV / 2)

#define CLR_NUM              4
#define DETAIL_RANGE         1
#define DETAIL_SHIFT         2
#define INIT_CENTER_ARROUND  1
#define INIT_ROUGH_DIVIDE    28
#define INIT_DETAIL_DIVIDE   6
#define INIT_CENT_ALLOW_HIGH 14080 // 14bits RAW
#define INIT_CENT_ALLOW_LOW  11420 // 14bits RAW
#define MAX_BLK_W_NUM        500
#define MAX_BLK_H_NUM        500

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define FIX_VAL(n) ((USHORT)((FLOAT)((n)*256.0)))

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	int width;
	int height;
} T_SHD_IMGSIZE;

typedef struct {
	int x;
	int y;
} T_SHD_POINT;

typedef FJ_PRO_CTRL_FIRST_PIX_COLOR (*shd_get_color)(const UCHAR colorindex);
typedef UCHAR (*shd_get_color_index)(const FJ_PRO_CTRL_FIRST_PIX_COLOR color);
typedef void (*shd_set_center)(void);

typedef struct {
	int width;
	int height;
	int x;
	int y;
	T_SHD_POINT location;
} T_BLK_RECT;

typedef struct {
	T_SHD_IMGSIZE size;
	FJ_PRO_CTRL_FIRST_PIX_COLOR s_pix;
	USHORT pix_max;
	USHORT pix_min;

	shd_get_color get_pixel_color;
	shd_get_color_index get_color_index;

	T_SHD_POINT center;
	shd_set_center set_center;

} T_SHD_IMGDATA;

typedef struct {
	int sum;
	int num;
	int max;
	int maxX;
	int maxY;
} T_SHD_RAWDATA;

typedef struct {
	T_SHD_RAWDATA rawdata[CLR_NUM];
	T_BLK_RECT blockrect;
} T_SHD_BLKDATA;

typedef struct {
	T_SHD_BLKDATA blkdata[MAX_BLK_H_NUM][MAX_BLK_W_NUM];
	T_SHD_IMGSIZE offset;
	double brightness[CLR_NUM];
	USHORT w_blk_num;
	USHORT h_blk_num;
} T_SHD_SHDDATA;

typedef struct {
	T_SHD_IMGSIZE Div;
	int RoughDivide;
	int DetailDivide;
	UCHAR shd_comp_rate;
	UCHAR center_pos_rate;
	UCHAR shd_corner_br_thre;
} T_SHD_CONFIG;

typedef struct {
	T_CALIB_BADPIX_PARAM user_param;
	INT32 exec_cnt;
	INT32 dbg_cnt;
} T_BADPIX_MNG;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/

static T_SHD_CONFIG g_config;
static T_CALIB_SET g_calib_set;
static T_SHD_IMGDATA g_imgdata;
static FJ_PRO_CTRL_FIRST_PIX_COLOR g_s_pix_order[4][4] = {
    {FJ_PRO_CTRL_FIRST_PIX_COLOR_R, FJ_PRO_CTRL_FIRST_PIX_COLOR_GR, FJ_PRO_CTRL_FIRST_PIX_COLOR_GB,
     FJ_PRO_CTRL_FIRST_PIX_COLOR_B},
    {FJ_PRO_CTRL_FIRST_PIX_COLOR_B, FJ_PRO_CTRL_FIRST_PIX_COLOR_GB, FJ_PRO_CTRL_FIRST_PIX_COLOR_GR,
     FJ_PRO_CTRL_FIRST_PIX_COLOR_R},
    {FJ_PRO_CTRL_FIRST_PIX_COLOR_GR, FJ_PRO_CTRL_FIRST_PIX_COLOR_R, FJ_PRO_CTRL_FIRST_PIX_COLOR_B,
     FJ_PRO_CTRL_FIRST_PIX_COLOR_GB},
    {FJ_PRO_CTRL_FIRST_PIX_COLOR_GB, FJ_PRO_CTRL_FIRST_PIX_COLOR_B, FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
     FJ_PRO_CTRL_FIRST_PIX_COLOR_GR}};

static T_SHD_SHDDATA* g_main_shd   = NULL;
static T_SHD_SHDDATA* g_rough_shd  = NULL;
static T_SHD_SHDDATA* g_detail_shd = NULL;

static T_BADPIX_MNG g_badpix_mng = {.exec_cnt = 0, .dbg_cnt = 0};

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static FJ_PRO_CTRL_FIRST_PIX_COLOR get_color(const UCHAR index);
static UCHAR get_color_index(const FJ_PRO_CTRL_FIRST_PIX_COLOR color);
static void set_center(void);

static void set_rawdata(T_SHD_SHDDATA* shddata, const UCHAR c_index, const USHORT blk_x,
                        const USHORT blk_y, const int x, const int y, const USHORT val);
static USHORT getbrightrate4096(T_SHD_SHDDATA* shddata, const UCHAR c_index, const USHORT blk_x,
                                const USHORT blk_y, const double centerbright);
static double getbrightrate100(T_SHD_SHDDATA* shddata, const UCHAR c_index, const USHORT blk_x,
                               const USHORT blk_y, const double centerbright);
static double getaverage(T_SHD_SHDDATA* shddata, const UCHAR c_index, const USHORT blk_x,
                         const USHORT blk_y);
#if defined(SHD_DEBUG)
static int getsum(T_SHD_SHDDATA* shddata, const UCHAR c_index, const USHORT blk_x,
                  const USHORT blk_y);
static int getnum(T_SHD_SHDDATA* shddata, const UCHAR c_index, const USHORT blk_x,
                  const USHORT blk_y);
#endif
static T_SHD_POINT getmax_pos_in_blk(T_SHD_SHDDATA* shddata, const UCHAR c_index,
                                     const USHORT blk_x, const USHORT blk_y);

static void Set_MaxBrightness(T_SHD_SHDDATA* shddata);
static double Get_MaxAverage(T_SHD_SHDDATA* shddata, const UCHAR c_index);
static T_SHD_POINT get_maxAveBlockPos(T_SHD_SHDDATA* shddata, const UCHAR c_index);
static T_SHD_POINT get_maxValImgPos(T_SHD_SHDDATA* shddata, const UCHAR c_index, const USHORT blk_x,
                                    const USHORT blk_y);

static void SHD_Init(USHORT width, USHORT height, FJ_PRO_CTRL_FIRST_PIX_COLOR s_pix);
static int SHD_Adj_Check(const USHORT* const in);
static void ADJ_AnalyzeBlockData(T_SHD_SHDDATA* shddata, const USHORT* const in,
                                 const T_SHD_IMGSIZE aDivBlockSize);
static void ADJ_GetRawData(T_SHD_SHDDATA* shddata, const USHORT* const in, T_BLK_RECT aBlockRect,
                           T_SHD_BLKDATA* aShdData, const int blk_x, const int blk_y);
static void ADJ_StartPixel(const T_BLK_RECT aBlockRect, int* clrIndex);
static USHORT ADJ_GetRawPixel(const USHORT* const in, int aX, int aY);
static int ADJ_CheckCenterValue(T_SHD_SHDDATA* shddata);
static T_SHD_POINT ADJ_SearchCenter(T_SHD_SHDDATA* shddata, const USHORT* const in);
static T_BLK_RECT ADJ_GetMaxAveAroundRect(T_SHD_SHDDATA* shddata, int aArroundShift);
static T_SHD_SHDDATA* ADJ_AnalyzeAboutCenter(const USHORT* const in, T_BLK_RECT aSearchArea);
static T_SHD_SHDDATA* ADJ_AnalyzeShiftBlockColor(const USHORT* const in, T_BLK_RECT aSearchArea,
                                                 int aDivSize, int aColor);
static void ADJ_GetRawDataColor(T_SHD_SHDDATA* shddata, const USHORT* const in,
                                T_BLK_RECT aBlockRect, int aColor, USHORT blk_x, USHORT blk_y);

static T_SHD_POINT ADJ_GetSarchedCenterPosMixColor(T_SHD_POINT* aFindCenters);
static int ADJ_CheckCenter(T_SHD_POINT aCenterPoint);
static int ADJ_CheckCornerRate(T_SHD_SHDDATA* aShdData);

static T_SHD_SHDDATA* SHD_AnalyzeMain(const USHORT* const in);
static void SHD_GetOutData(T_SHD_SHDDATA* aImgData, USHORT* outData);
/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void User_Utility_Calib_Set(T_CALIB_SET set_info, T_CALIB_SHD_PARAM param)
{
	g_calib_set                 = set_info;
	g_config.shd_comp_rate      = param.shd_gain_comp_rate > 80 ? 80 : param.shd_gain_comp_rate;
	g_config.Div.width          = param.blk_num.bits.blk_num_horizontal;
	g_config.Div.height         = param.blk_num.bits.blk_num_vertical;
	g_config.center_pos_rate    = param.shd_thre_cen_pos;
	g_config.shd_corner_br_thre = param.shd_corner_br_rate;
}

void User_Utility_Calib_Get(T_CALIB_SET* const calib_set)
{
	*calib_set = g_calib_set;
}

INT32 User_Utility_Calib_RGB(const ULONG raw_addr, const ULLONG frame_no, const UCHAR sensor_mode)
{
	T_SHARE_RAW raw;
	USHORT raw_w, raw_h;
	FJ_PRO_CTRL_FIRST_PIX_COLOR s_pix;
	USHORT rgb_calc_w, rgb_calc_h;
	USHORT s_pos_h, s_pos_v;
	USHORT* p_pix;
	UINT32 pix_sum[4] = {0};
	UINT32 r_sum = 0, gr_sum = 0, gb_sum = 0, b_sum = 0;
	UINT32 r_avg, g_avg, b_avg;
	int h_cnt, v_cnt;
	float r_g_ratio, b_g_ratio;
	USHORT fix_rg, fix_bg;
	UINT32 rg_bg;
	INT32 rval = 0;
	ULONG fid;
	ULONG ret_wr_size;

	Get_Frame_Share_Sensor_Raw(&raw, frame_no);
	raw_w      = raw.sensor[0].h_size;
	raw_h      = raw.rect[0].v_size;
	s_pix      = CAMERA.sensor[raw.sensor[0].sensor_id].mode[sensor_mode].first_active_pix_color;
	s_pos_h    = (raw_w / INTEREST_AREA_DIV) * INTEREST_AREA_HALF;
	s_pos_v    = (raw_h / INTEREST_AREA_DIV) * INTEREST_AREA_HALF;
	rgb_calc_w = (raw_w / INTEREST_AREA_DIV) & 0xFFFE;
	rgb_calc_h = (raw_h / INTEREST_AREA_DIV) & 0xFFFE;

	for (v_cnt = 0; v_cnt < rgb_calc_h; v_cnt++) {
		// shift p_pix to start of pixel for RGB sum
		p_pix = (USHORT*)raw_addr;
		p_pix += (s_pos_v + v_cnt) * raw_w + s_pos_h;
		for (h_cnt = 0; h_cnt < rgb_calc_w; h_cnt += 2) {
			if ((v_cnt & 0x0001) == 0) {
				pix_sum[0] += *p_pix++;
				pix_sum[1] += *p_pix++;
			} else {
				pix_sum[2] += *p_pix++;
				pix_sum[3] += *p_pix++;
			}
		}
	}

	switch (s_pix) {
	case FJ_PRO_CTRL_FIRST_PIX_COLOR_R:
		r_sum  = pix_sum[0];
		gr_sum = pix_sum[1];
		gb_sum = pix_sum[2];
		b_sum  = pix_sum[3];
		break;
	case FJ_PRO_CTRL_FIRST_PIX_COLOR_B:
		b_sum  = pix_sum[0];
		gb_sum = pix_sum[1];
		gr_sum = pix_sum[2];
		r_sum  = pix_sum[3];
		break;
	case FJ_PRO_CTRL_FIRST_PIX_COLOR_GR:
		gr_sum = pix_sum[0];
		gb_sum = pix_sum[3];
		r_sum  = pix_sum[1];
		b_sum  = pix_sum[2];
		break;
	case FJ_PRO_CTRL_FIRST_PIX_COLOR_GB:
		gb_sum = pix_sum[0];
		gr_sum = pix_sum[3];
		b_sum  = pix_sum[1];
		r_sum  = pix_sum[2];
		break;
	default: break;
	}

	UPRINTF("r_sum:%d, gr_sum:%d, gb_sum:%d, b_sum:%d\n", r_sum, gr_sum, gb_sum, b_sum);
	UPRINTF("rgb_calc_w:%d, rgb_calc_h:%d\n", rgb_calc_w, rgb_calc_h);

	r_avg = r_sum / (rgb_calc_w * rgb_calc_h / 4);
	g_avg = (gr_sum + gb_sum) / (rgb_calc_w * rgb_calc_h / 2);
	b_avg = b_sum / (rgb_calc_w * rgb_calc_h / 4);

	if (g_avg < (0x3fff * G_THRES_L_PERCENT / 100) || g_avg > (0x3fff * G_THRES_H_PERCENT / 100)) {
		UPRINTF("G avg:%Xh [max:3FFFh] small or big than 40 percentage min or 60 percentage max, "
		        "please adjust exposure value\n",
		        g_avg);
		rval = -2;
		UPRINTF_ERR(rval);
		return rval;
	}
	r_g_ratio = (float)r_avg / (float)g_avg;
	b_g_ratio = (float)b_avg / (float)g_avg;

	UPRINTF("rg raio:%f, bg ratio:%f\n", r_g_ratio, b_g_ratio);

	fix_rg = FIX_VAL(r_g_ratio);
	fix_bg = FIX_VAL(b_g_ratio);

	rg_bg = fix_bg << 16 | fix_rg;
	rval  = Cate_Set(0x0F, 0x01, 1, (ULONG*)&rg_bg);
	UPRINTF_ERR(rval);

	fid =
	    FJ_Fs_Open((CHAR*)"I:\\RGB.bin", (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC));
	FJ_Fs_Write(fid, (UINT32*)&rg_bg, sizeof(ULONG), &ret_wr_size);
	FJ_Fs_Close(fid);

	return rval;
}

#define ROUNDUPN(val, n) ((((val) + (n)-1) / (n)) * (n))

INT32 User_Utility_Calib_SHD(const ULONG raw_addr, const ULLONG frame_no, const UCHAR sensor_mode,
                             E_CALIB_SHD_CT ct)
{
	T_SHD_SHDDATA* shdData = NULL;
	T_SHARE_RAW raw;
	USHORT* out = NULL;
	T_FSHD_TBL_HEADER* head;
	CHAR filepath[32];
	ULONG fid;
	ULONG ret_wr_size;
	ULONG tbl_len      = 0;
	ULONG shd_clac_len = 0;
	int rval           = 0;

	Get_Frame_Share_Sensor_Raw(&raw, frame_no);

	shd_clac_len = sizeof(T_SHD_SHDDATA) * 3;
	tbl_len = g_config.Div.width * g_config.Div.height * CLR_NUM * 2;
	head        = (T_FSHD_TBL_HEADER*)User_Utility_MM_alloc(
	           sizeof(T_FSHD_TBL_HEADER) + tbl_len + shd_clac_len, 0);
	if (head == NULL) {
		rval = -1;
		UPRINTF_ERR(rval);
		return rval;
	}
	out = (USHORT*)((unsigned int)head + sizeof(T_FSHD_TBL_HEADER));

	g_main_shd   = (T_SHD_SHDDATA*)(((UINT32)out) + tbl_len);
	g_rough_shd  = (T_SHD_SHDDATA*)(((UINT32)g_main_shd) + sizeof(T_SHD_SHDDATA));
	g_detail_shd = (T_SHD_SHDDATA*)(((UINT32)g_rough_shd) + sizeof(T_SHD_SHDDATA));

	SHD_Init(raw.sensor[0].h_size, raw.rect[0].v_size,
	         CAMERA.sensor[raw.sensor[0].sensor_id].mode[sensor_mode].first_active_pix_color);

	rval = SHD_Adj_Check((USHORT*)raw_addr);
	if (rval) {
		User_Utility_MM_free(head, 0);
		return rval;
	}

	shdData = SHD_AnalyzeMain((USHORT*)raw_addr);

	SHD_GetOutData(shdData, out);
	memset(head, 0, sizeof(T_FSHD_TBL_HEADER));
	head->version.tbl_columns           = g_config.Div.width;
	head->version.tbl_rows              = g_config.Div.height;
	head->version.tbl_global_col_bytes  = g_config.Div.width * 2;
	head->version.tbl_frame_fld_bytelen = g_config.Div.width * g_config.Div.height * 2;

	if (ct == E_SHD_LIGHT_0) {
		strcpy(filepath, "I:\\FSHD_0.bin");
		if (User_Get_Resource()->FSHD_BIN_size[0] == (sizeof(T_FSHD_TBL_HEADER) + tbl_len)) {
			memcpy((void*)User_Get_Resource()->FSHD_BIN_addr[0], (void*)head,
			       sizeof(T_FSHD_TBL_HEADER) + tbl_len);
		}
	} else if (ct == E_SHD_LIGHT_1) {
		strcpy(filepath, "I:\\FSHD_1.bin");
		if (User_Get_Resource()->FSHD_BIN_size[1] == (sizeof(T_FSHD_TBL_HEADER) + tbl_len)) {
			memcpy((void*)User_Get_Resource()->FSHD_BIN_addr[1], (void*)head,
			       sizeof(T_FSHD_TBL_HEADER) + tbl_len);
		}
	} else if (ct == E_SHD_LIGHT_2) {
		strcpy(filepath, "I:\\FSHD_2.bin");
		if (User_Get_Resource()->FSHD_BIN_size[2] == (sizeof(T_FSHD_TBL_HEADER) + tbl_len)) {
			memcpy((void*)User_Get_Resource()->FSHD_BIN_addr[2], (void*)head,
			       sizeof(T_FSHD_TBL_HEADER) + tbl_len);
		}
	} else {
	}

	fid = FJ_Fs_Open((CHAR*)filepath, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC));
	FJ_Fs_Write(fid, (UINT32*)head, sizeof(T_FSHD_TBL_HEADER) + tbl_len, &ret_wr_size);
	FJ_Fs_Close(fid);

	User_Utility_MM_free(head, 0);

	return rval;
}

void User_Utility_Calib_Badpix_Change_Config(T_SHARE_SYSTEM_CONFIG* config,
                                             T_SHARE_AE_PARAM* ae_param)
{
	T_CAMERA_CALC_EXP_IN calc_exp_in = {0};
	UCHAR sid                        = 0; // fix

	// Get Param
	Cate_Get_Calib_Badpix_Param(&g_badpix_mng.user_param);

	// Error check
	if (g_badpix_mng.user_param.detect_algo >= E_IQ_DETECT_ALGO_MAX) {
		g_badpix_mng.user_param.detect_algo = E_IQ_DETECT_ALGO_0;
		UPRINTF_ERR(1);
	}
	if (g_badpix_mng.user_param.sensor_mode >= CAMERA.sensor[0].mode_num) {
		g_badpix_mng.user_param.sensor_mode = 0;
		UPRINTF_ERR(1);
	}

	// Change Sensor Setting
	config->sensor_num       = 1;
	config->sensor_mode[sid] = g_badpix_mng.user_param.sensor_mode;
	calc_exp_in.exp_msec     = g_badpix_mng.user_param.sensor_exp_msec;
	calc_exp_in.gain_times   = g_badpix_mng.user_param.sensor_gain_times;
	// Chagne AE Setting
	ae_param->isp_gain.sensor[sid].b2b_end[E_PGAIN_R]  = 1.0;
	ae_param->isp_gain.sensor[sid].b2b_end[E_PGAIN_GR] = 1.0;
	ae_param->isp_gain.sensor[sid].b2b_end[E_PGAIN_GB] = 1.0;
	ae_param->isp_gain.sensor[sid].b2b_end[E_PGAIN_B]  = 1.0;

	if (CAMERA.sensor[sid].Func_Calc_Exp) {
		CAMERA.sensor[sid].Func_Calc_Exp(sid, config->sensor_mode[sid], &calc_exp_in,
		                                 &ae_param->exp.sensor[sid].exp,
		                                 &ae_param->reg.sensor[sid].reg);
	}
	// Chagne Other
	config->sensor_yuv[E_SHARE_YUV_OUT_USER_0].h_size =
	    CAMERA.sensor[sid].mode[config->sensor_mode[sid]].active_pix_width;
	config->sensor_yuv[E_SHARE_YUV_OUT_USER_0].v_size =
	    CAMERA.sensor[sid].mode[config->sensor_mode[sid]].active_pix_lines;
	config->sensor_yuv[E_SHARE_YUV_OUT_USER_0].v_size -=
	    (D_SHARE_PIPE_V_OFFSET - D_SHARE_PIPE_V_LOST);

	if (((g_badpix_mng.user_param.iq_setting >> 0) & 1)) { config->disable_ob = TRUE; }
	if (((g_badpix_mng.user_param.iq_setting >> 1) & 1)) { config->disable_sat = TRUE; }

	// Debug Mode to Check SDC Effect ( currently capture raw 2times )
	if (g_badpix_mng.user_param.mode == 0xFF) {
		config->disable_ob  = TRUE;
		config->disable_sat = TRUE;
		config->disable_dpc = TRUE;
		config->sdc_en      = TRUE;
		// config->sensor_div_forced    = 0; // capture set 4
		User_Utility_Calib_Badpix_Addr(sid, config->sensor_mode[sid], &config->sdc_tbl_addr[sid]);

		if (g_badpix_mng.dbg_cnt == 0) {
			config->raw_out_point_forced = RAW_EXTRACT_POINT_SEN; // before
			g_badpix_mng.dbg_cnt         = 1;
		} else if (g_badpix_mng.dbg_cnt == 1) {
			config->raw_out_point_forced = RAW_EXTRACT_POINT_SRO; // after
			g_badpix_mng.dbg_cnt         = 2;
		} else {
			g_badpix_mng.dbg_cnt = 0;
		}
	}
}

INT32 User_Utility_Calib_Badpix(const ULONG raw_addr, const ULLONG frame_no,
                                const UCHAR sensor_mode)
{
	// for bad pix calc param
	T_BF_IQ_DPC_DETECT_INFO detect_info                      = {0};
	FJ_IQ_CORNER_THRESHOLD corner_thres                      = {{0}};
	T_FJ_IQ_DPC_LINE_ORDER line_order                        = {0};
	T_FJ_IQ_DPC_LINE_ORDER_IN_FIELDS line_order_in_fields[2] = {{0}};
	USHORT line_number_tbl0                                  = 0;
	USHORT line_number_tbl1                                  = 0;
	T_BF_IQ_DPC_TBL_INFO* tbl_info;
	E_IQ_CALIB_TGT e_calib_tgt = E_IQ_CALIB_TGT_0;
	UINT32 tbl_idx             = 0;
	BOOL is_merge              = FALSE;
	BOOL is_save               = FALSE;
	// for bayer value
	T_SHARE_RAW raw;
	UINT32 bay_hsta, bay_vsta;           // saved bayer start pos from sensor output
	UINT32 bay_hsiz, bay_vsiz;           // saved bayer size
	UINT32 bay_calc_hsta, bay_calc_vsta; // calc start pos from saved bayer
	UINT32 bay_calc_hsiz, bay_calc_vsiz; // calc size in saved bayer
	// other
	CHAR fpath[64];
	INT32 ercd;
	static INT32 file_save_cnt = 0;

	// ------------------------------
	// Debug Mode to Check SDC Effect ( currently capture raw 2times )
	// ------------------------------
	if (g_badpix_mng.user_param.mode == 0xFF) {
		if (g_badpix_mng.dbg_cnt == 1) {
			UPRINTF(" The saved bayer is before SDC ------------------------------\n");
			Snd_Event_to_User(E_USER_EVENT_CAPTURE_START_JPG_RAW_YUV, 1);
			return 1; // not OK
		} else {
			UPRINTF(" The saved bayer is after SDC ------------------------------\n");
			return OK;
		}
	}
	// ------------------------------
	// Param
	// ------------------------------
	if (g_badpix_mng.user_param.mode == 0) {
		is_save  = TRUE;
		is_merge = FALSE;

	} else if (g_badpix_mng.user_param.mode == 1) {
		if ((g_badpix_mng.exec_cnt & 1) == 1) { // 2n times (n = 1, 2 ... )
			is_merge = TRUE;
			is_save  = TRUE;
		} else {
			is_merge = FALSE;
			is_save  = FALSE;
		}
	}

	// get raw
	Get_Frame_Share_Sensor_Raw(&raw, frame_no);
	if (raw.sensor[0].raw_wo_mergin == 0) {
		// need to combine, no support now.
		UPRINTF_ERR(1);
	}
	// set bayer info
	bay_hsta      = 0;                                      // the raw is saved from beggingin
	bay_vsta      = 0;                                      // the raw is saved from beggingin
	bay_hsiz      = raw.rect[0].h_byte / 2;                 // raw is saved with 8byte align
	bay_vsiz      = raw.rect[0].v_ofs + raw.rect[0].v_size; // all rect[] v_ofs and v_size are same
	bay_calc_hsta = CAMERA.sensor[0].mode[sensor_mode].active_pix_x_pos;
	bay_calc_vsta = CAMERA.sensor[0].mode[sensor_mode].active_pix_y_pos;
	bay_calc_hsiz = CAMERA.sensor[0].mode[sensor_mode].active_pix_width;
	bay_calc_vsiz = CAMERA.sensor[0].mode[sensor_mode].active_pix_lines;
	if (bay_calc_vsta + bay_calc_vsiz == bay_vsiz + D_SHARE_PIPE_V_LOST_BAYER) {
		bay_calc_vsiz -= D_SHARE_PIPE_V_LOST_BAYER; // ISP lost line
	}
	if (bay_calc_hsta + bay_calc_hsiz > bay_hsiz) {
		UPRINTF_ERR(1);
		UPRINTF("bay_calc_hsta(%d) + bay_calc_hsiz(%d) > bay_hsiz(%d)\n", bay_calc_hsta,
		        bay_calc_hsiz, bay_hsiz);
	}
	if (bay_calc_vsta + bay_calc_vsiz > bay_vsiz) {
		UPRINTF_ERR(1);
		UPRINTF("bay_calc_vsta(%d) + bay_calc_vsiz(%d) > bay_vsiz(%d)\n", bay_calc_vsta,
		        bay_calc_vsiz, bay_vsiz);
	}
	// ------------------------------
	// Param for Create
	// ------------------------------
	// corner_thres ( optional )

	// line_number_tbl ( depends on vsta )
	if ((bay_vsta & 1) == 0) { // even
		line_number_tbl0 = 0;
		line_number_tbl1 = 1;
	} else { // odd
		line_number_tbl0 = 1;
		line_number_tbl1 = 0;
	}
	// line_order_in_fields ( fix )
	line_order_in_fields[0].ui_table_num                = 1; // fix
	line_order_in_fields[0].pus_line_order_in_field_tbl = (const USHORT*)&line_number_tbl0;
	line_order_in_fields[1].ui_table_num                = 1; // fix
	line_order_in_fields[1].pus_line_order_in_field_tbl = (const USHORT*)&line_number_tbl1;
	// line_order ( fix )
	line_order.us_field_separate_row_cycle = 2; // fix
	line_order.us_line_order_tbl_num       = 2; // fix
	line_order.pt_line_order_in_fields_tbl =
	    (const T_FJ_IQ_DPC_LINE_ORDER_IN_FIELDS*)&line_order_in_fields;
	// detect_info ( other value is 0 )
	detect_info.pus_bayer16_addr     = (const USHORT*)raw.rect[0].addr;
	detect_info.ui_bayer_byte        = bay_hsiz * bay_vsiz * 2; // 2 is 16bit
	detect_info.us_global_w_pixels   = bay_hsiz;
	detect_info.us_global_h_pixels   = bay_vsiz;
	detect_info.us_hd_ofs            = bay_hsta;
	detect_info.us_vd_ofs            = bay_vsta;
	detect_info.us_left_trim         = bay_calc_hsta;
	detect_info.us_upper_trim        = bay_calc_vsta;
	detect_info.us_w_pixels          = bay_calc_hsiz;
	detect_info.us_h_pixels          = bay_calc_vsiz;
	detect_info.ui_field_num         = 2; // fix
	detect_info.pt_corner_thresh     = &corner_thres;
	detect_info.pt_line_order_table  = &line_order;
	detect_info.us_threshold_high    = g_badpix_mng.user_param.detect_algo_param0;
	detect_info.us_threshold_low     = g_badpix_mng.user_param.detect_algo_param1;
	detect_info.e_detect_mode        = g_badpix_mng.user_param.detect_algo;
	detect_info.ui_max_badpix_thresh = g_badpix_mng.user_param.detect_max_count;

	// ------------------------------
	// Create Bad Pix Table
	// ------------------------------
	ercd = BF_IQ_Dpc_Create_Bad_Pix_Table(e_calib_tgt, tbl_idx, is_merge, &detect_info);
	UPRINTF_ERR(ercd);
	if (detect_info.ui_over4succession_count) {}
	UPRINTF("over4-consecutive    = %d\n", detect_info.ui_over4succession_count);
	UPRINTF("overflow             = %d\n", detect_info.ui_detect_info_tbl_overflow);

	// Get table
	tbl_info = BF_IQ_Dpc_Tbl_Get_Info(e_calib_tgt, tbl_idx);
	UPRINTF("bad pixel_num        = %d\n", tbl_info->pixel_num);

	// save file
	if (is_save) {
		sprintf(fpath, "I:\\%d_BAD_senosr%d_mode%d.bin", file_save_cnt++, 0, sensor_mode);
		ercd = fj_iq_calibrationsavebadpixtable(e_calib_tgt, tbl_idx, fpath);
		UPRINTF_ERR(!ercd);
	}

	g_badpix_mng.exec_cnt++;

	return OK;
}

INT32 User_Utility_Calib_Badpix_Addr(UCHAR sensor_id, UCHAR sensor_mode, ULONG* addr)
{
	const T_RESOURCE* rsc;
	INT32 ret = -1;

	rsc = User_Get_Resource();

	if (addr == NULL) { return -1; }

	// Sample
	switch (sensor_id) {
	case 0:
		switch (sensor_mode) {
		case 0:
		case 1:
		case 2:
		case 3:
			*addr = (ULONG)rsc->BADPIX_addr[sensor_mode];
			ret   = 0;
			break;
		default: *addr = 0; break;
		}
		break;

	case 1:
	case 2:
	case 3:
	default: *addr = (ULONG)rsc->BADPIX_addr[0]; break;
	}
	return ret;
}

void User_Utility_Calib_Create_Badpix_Debug(BOOL merge_en)
{
	// input to  SD Card
	// - bp_white.raw ( 16bit )
	// - bp_black.raw ( 16bit )
	// out to SD Card
	// - bp_black_even.bin, bp_black_odd.bin, bp_white_even.bin, bp_white_odd.bin ( if !merge_en )
	// - bp_v400.bin, bp_merge_even.bin, bp_merge_odd.bin ( if merge_en )

	// DPC
	T_BF_IQ_DPC_DETECT_INFO detect_info                      = {0};
	FJ_IQ_CORNER_THRESHOLD corner_thres                      = {{0}};
	T_FJ_IQ_DPC_LINE_ORDER line_order                        = {0};
	T_FJ_IQ_DPC_LINE_ORDER_IN_FIELDS line_order_in_fields[2] = {{0}};
	USHORT line_number_tbl0                                  = 0;
	USHORT line_number_tbl1                                  = 0;
	T_BF_IQ_DPC_TBL_INFO* tbl_info;
	INT32 black_white;
	INT32 even_odd;
	BOOL merge                 = 0;
	E_IQ_CALIB_TGT e_calib_tgt = E_IQ_CALIB_TGT_0;
	UINT32 tbl_idx             = 0;

	// bayer
	UINT32 bay_width     = 3840; // bp_white.raw/black.raw
	UINT32 bay_lines     = 2160; // bp_white.raw/black.raw
	UINT32 bay_bytelen   = bay_width * bay_lines * 2;
	USHORT* bay_buf      = (USHORT*)SDRAM_ADR_CH1_RTOS_IMAGE_WORK;
	CHAR bay_name[2][64] = {"bp_white.raw", "bp_black.raw"};

	// file
	ULONG fid;
	CHAR fpath[64];
	ULONG fret_length;
	CHAR fname[2][64] = {"bp_white", "bp_black"};

	// other
	INT32 ercd = 0;

	// DPC Corner
	corner_thres.t_left_top.us_h                  = 0;
	corner_thres.t_left_top.us_v                  = 0;
	corner_thres.t_left_top.us_threshold_high     = 0;
	corner_thres.t_left_top.us_threshold_low      = 0;
	corner_thres.t_right_top.us_h                 = 0;
	corner_thres.t_right_top.us_v                 = 0;
	corner_thres.t_right_top.us_threshold_high    = 0;
	corner_thres.t_right_top.us_threshold_low     = 0;
	corner_thres.t_left_bottom.us_h               = 0;
	corner_thres.t_left_bottom.us_v               = 0;
	corner_thres.t_left_bottom.us_threshold_high  = 0;
	corner_thres.t_left_bottom.us_threshold_low   = 0;
	corner_thres.t_right_bottom.us_h              = 0;
	corner_thres.t_right_bottom.us_v              = 0;
	corner_thres.t_right_bottom.us_threshold_high = 0;
	corner_thres.t_right_bottom.us_threshold_low  = 0;
	// DPC line_number
	if (1) { // ofst is even
		line_number_tbl0 = 0;
		line_number_tbl1 = 1;
	} else {
		line_number_tbl0 = 1;
		line_number_tbl1 = 0;
	}
	// DPC line_order_in_fileds
	line_order_in_fields[0].ui_table_num                = 1; // fix
	line_order_in_fields[0].pus_line_order_in_field_tbl = (const USHORT*)&line_number_tbl0;
	line_order_in_fields[1].ui_table_num                = 1; // fix
	line_order_in_fields[1].pus_line_order_in_field_tbl = (const USHORT*)&line_number_tbl1;
	// DPC line_order
	line_order.us_field_separate_row_cycle = 2; // fix
	line_order.us_line_order_tbl_num       = 2; // fix
	line_order.pt_line_order_in_fields_tbl =
	    (const T_FJ_IQ_DPC_LINE_ORDER_IN_FIELDS*)&line_order_in_fields;
	// DPC detect_info
	detect_info.us_threshold_high        = 1024;
	detect_info.us_threshold_low         = 1024;
	detect_info.pus_bayer16_addr         = bay_buf;
	detect_info.ui_bayer_byte            = bay_width * bay_lines * 2;
	detect_info.us_global_w_pixels       = bay_width;
	detect_info.us_global_h_pixels       = bay_lines;
	detect_info.us_hd_ofs                = 0;
	detect_info.us_vd_ofs                = 0;
	detect_info.uc_trans_unit            = 0; // fix
	detect_info.uc_hd_edge               = 0; // fix
	detect_info.uc_vd_edge               = 0; // fix
	detect_info.us_left_trim             = 0;
	detect_info.us_upper_trim            = 0;
	detect_info.us_w_pixels              = bay_width;
	detect_info.us_h_pixels              = bay_lines;
	detect_info.ui_field_num             = 2; // fix
	detect_info.e_detect_mode            = 0;
	detect_info.pt_corner_thresh         = &corner_thres;
	detect_info.pt_line_order_table      = &line_order;
	detect_info.ull_trans_limit_byte     = 0; // fix
	detect_info.b_hwDetectOnOff          = 0; // fix
	detect_info.ui_max_badpix_thresh     = 1000;
	detect_info.pt_defect_info_tbl_addr  = 0; // fix ( set in BF_IQ_Dpc_Create_Bad_Pix_Table )
	detect_info.ull_defect_info_tbl_byte = 0; // fix ( set in BF_IQ_Dpc_Create_Bad_Pix_Table )
	// output
	detect_info.ull_defect_info_tbl_bytelen[0] = 0;
	detect_info.ull_defect_info_tbl_bytelen[1] = 0;
	detect_info.ui_detect_info_tbl_overflow    = 0;
	detect_info.ui_detect_count                = 0;
	detect_info.ui_over4succession_count       = 0;

	for (black_white = 0; black_white < 2; black_white++) {

		// Read Bayer
		sprintf(fpath, "I:\\%s", bay_name[black_white]);
		fid  = FJ_Fs_Open((CHAR*)fpath, FJ_FS_OPEN_RDONLY);
		ercd = FJ_Fs_Read(fid, (UINT32*)bay_buf, bay_bytelen, &fret_length);
		UPRINTF_ERR(ercd);
		FJ_Fs_Close(fid);
		BF_L1l2cache_Flush_Addr((UINT32)bay_buf, fret_length); // for safety

		// Detect
		if (black_white != 0) {
			merge = merge_en;
		} else {
			merge = 0;
		}
		ercd = BF_IQ_Dpc_Create_Bad_Pix_Table(e_calib_tgt, tbl_idx, merge, &detect_info);
		UPRINTF_ERR(ercd);

		// Print Result
		printf("over4pix    = %d\n", detect_info.ui_over4succession_count);
		printf("overflow    = %d\n", detect_info.ui_detect_info_tbl_overflow);

		// Get table
		tbl_info = BF_IQ_Dpc_Tbl_Get_Info(e_calib_tgt, tbl_idx);
		printf("pixel_num   = %d\n", tbl_info->pixel_num);

		// Save
		if (merge_en) {
			if (merge) {
				sprintf(fname[black_white], "bp_merge"); // change name

				// save by fj func with header
				sprintf(fpath, "I:\\bp_v400.bin");
				ercd = fj_iq_calibrationsavebadpixtable(e_calib_tgt, tbl_idx, fpath);
				UPRINTF_ERR(!ercd);
			} else {
				continue; // skip
			}
		}

		// save each table by manual
		for (even_odd = 0; even_odd < 2; even_odd++) { // even odd
			if (even_odd == 0) {
				sprintf(fpath, "I:\\%s_even.bin", fname[black_white]);

			} else {
				sprintf(fpath, "I:\\%s_odd.bin", fname[black_white]);
			}

			BF_L1l2cache_Flush_Addr((UINT32)tbl_info->addr_by_fields[even_odd],
			                        (UINT32)tbl_info->bytelen_by_fields[even_odd]); // for safety
			fid  = FJ_Fs_Open((CHAR*)fpath, FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC);
			ercd = FJ_Fs_Write(fid, (UINT32*)tbl_info->addr_by_fields[even_odd],
			                   (UINT32)tbl_info->bytelen_by_fields[even_odd], &fret_length);
			UPRINTF_ERR(ercd);
			FJ_Fs_Close(fid);
		}
	}
}

void User_Utility_Calib_Load_Badpix_Debug(UCHAR div_num)
{
	E_IQ_CALIB_TGT e_calib_tgt = E_IQ_CALIB_TGT_0;
	UINT32 tbl_idx             = 0;
	T_BF_IQ_DPC_TBL_INFO* tbl_info;
	T_BF_IQ_DPC_MULTIPIPE_CALC_PARAM calc_param;
	T_BF_IQ_DPC_MULTIPIPE_TBL* multipipe_tbl;

	ULONG fid;
	CHAR fpath[64];
	ULONG fret_length;
	INT32 x, y;
	INT32 ercd;
	INT32 even_odd;

	// Load Table
	sprintf(fpath, "I:\\bp_v400.bin");
	ercd = fj_iq_calibrationloadbadpixtable(e_calib_tgt, tbl_idx, fpath);
	UPRINTF_ERR(!ercd);

	tbl_info = BF_IQ_Dpc_Tbl_Get_Info(e_calib_tgt, tbl_idx);
	printf("tbl_info->pixel_num = %d\n", (INT32)tbl_info->pixel_num);
	printf("tbl_info->sta_x     = %d\n", (INT32)tbl_info->sta_x);
	printf("tbl_info->sta_y     = %d\n", (INT32)tbl_info->sta_y);
	printf("tbl_info->width     = %d\n", (INT32)tbl_info->width);
	printf("tbl_info->lines     = %d\n", (INT32)tbl_info->lines);

	ercd = BF_IQ_Dpc_Tbl_Sel_Set(0, e_calib_tgt, tbl_idx);
	UPRINTF_ERR(ercd);

	// Make Div Information
	if (div_num != 4 && div_num != 2 && div_num != 1) { div_num = 1; }
	calc_param.x_num = div_num;
	calc_param.y_num = 1;
	y                = 0;

	for (x = 0; x < div_num; x++) {
		calc_param.crop_info[x][y].pos_x = (tbl_info->width / div_num) * x;
		calc_param.crop_info[x][y].pos_y = 0;
		calc_param.crop_info[x][y].width = tbl_info->width / div_num;
		calc_param.crop_info[x][y].lines = tbl_info->lines;
	}
	// Create Table
	calc_param.x_decrease_crop_offset = TRUE;
	calc_param.y_decrease_crop_offset = FALSE;
	ercd                              = BF_IQ_Dpc_MultiPipe_Calc(0, &calc_param);
	UPRINTF_ERR(ercd);

	multipipe_tbl = BF_IQ_Dpc_MultiPipeTblMng_Get_Info(0, TRUE);

	// Save Table
	for (x = 0; x < calc_param.x_num; x++) {
		printf("multipipe_tbl->x_num = %d\n", multipipe_tbl->x_num);
		printf("multipipe_tbl->y_num = %d\n", multipipe_tbl->y_num);
		printf("multipipe_tbl->tbl_info[%d][%d].addr_by_fields[0]    = 0x%x\n", x, y,
		       (INT32)multipipe_tbl->tbl_info[x][y].addr_by_fields[0]);
		printf("multipipe_tbl->tbl_info[%d][%d].addr_by_fields[1]    = 0x%x\n", x, y,
		       (INT32)multipipe_tbl->tbl_info[x][y].addr_by_fields[1]);
		printf("multipipe_tbl->tbl_info[%d][%d].bytelen_by_fields[0] = 0x%x\n", x, y,
		       (INT32)multipipe_tbl->tbl_info[x][y].bytelen_by_fields[0]);
		printf("multipipe_tbl->tbl_info[%d][%d].bytelen_by_fields[1] = 0x%x\n", x, y,
		       (INT32)multipipe_tbl->tbl_info[x][y].bytelen_by_fields[1]);
		printf("multipipe_tbl->tbl_info[%d][%d].pixel_num            = %d\n", x, y,
		       multipipe_tbl->tbl_info[x][y].pixel_num);
		printf("multipipe_tbl->tbl_info[%d][%d].sta_x                = %d\n", x, y,
		       multipipe_tbl->tbl_info[x][y].sta_x);
		printf("multipipe_tbl->tbl_info[%d][%d].sta_y                = %d\n", x, y,
		       multipipe_tbl->tbl_info[x][y].sta_y);
		printf("multipipe_tbl->tbl_info[%d][%d].width                = %d\n", x, y,
		       multipipe_tbl->tbl_info[x][y].width);
		printf("multipipe_tbl->tbl_info[%d][%d].lines                = %d\n", x, y,
		       multipipe_tbl->tbl_info[x][y].lines);

		for (even_odd = 0; even_odd < 2; even_odd++) {
			if (even_odd == 0) {
				sprintf(fpath, "I:\\bp_v400_div%drect%d_even.bin", calc_param.x_num, x);
			} else {
				sprintf(fpath, "I:\\bp_v400_div%drect%d_odd.bin", calc_param.x_num, x);
			}
			BF_L1l2cache_Flush_Addr(
			    (UINT32)multipipe_tbl->tbl_info[x][y].addr_by_fields[even_odd],
			    (UINT32)multipipe_tbl->tbl_info[x][y].bytelen_by_fields[even_odd]); // for safety
			fid  = FJ_Fs_Open((CHAR*)fpath, FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC);
			ercd = FJ_Fs_Write(fid, (UINT32*)multipipe_tbl->tbl_info[x][y].addr_by_fields[even_odd],
			                   (UINT32)multipipe_tbl->tbl_info[x][y].bytelen_by_fields[even_odd],
			                   &fret_length);
			UPRINTF_ERR(ercd);
			FJ_Fs_Close(fid);
		}
	}
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static void SHD_Init(USHORT width, USHORT height, FJ_PRO_CTRL_FIRST_PIX_COLOR s_pix)
{
	g_imgdata.get_pixel_color = get_color;
	g_imgdata.get_color_index = get_color_index;
	g_imgdata.set_center      = set_center;

	g_imgdata.size.width  = width;
	g_imgdata.size.height = height;

	g_imgdata.pix_max = 0x3FFF; // 14bits bitdepth
	g_imgdata.pix_min = 0;

	g_imgdata.s_pix = s_pix;
	g_imgdata.set_center();

	g_config.DetailDivide = INIT_DETAIL_DIVIDE;
	g_config.RoughDivide  = INIT_ROUGH_DIVIDE;
}

static int SHD_Adj_Check(const USHORT* const in)
{
	int rectW, rectH;
	T_SHD_IMGSIZE divBlockSize;
	int rval = 0;
	T_SHD_POINT point;
	int i;

	rectW = (int)((double)g_imgdata.size.width / (double)g_config.Div.width);
	rectH = (int)((double)g_imgdata.size.height / (double)g_config.Div.height);

	divBlockSize.width  = rectW;
	divBlockSize.height = rectH;

	if (g_config.Div.width > MAX_BLK_W_NUM || g_config.Div.height > MAX_BLK_H_NUM) {
		UPRINTF_ERR(-200);
		return -200;
	}

	memset(g_main_shd, 0, sizeof(T_SHD_SHDDATA));

	g_main_shd->w_blk_num = g_config.Div.width;
	g_main_shd->h_blk_num = g_config.Div.height;

	g_main_shd->offset.width  = (rectW >> 1) - 1;
	g_main_shd->offset.height = (rectH >> 1) - 1;

	ADJ_AnalyzeBlockData(g_main_shd, in, divBlockSize);
	rval = ADJ_CheckCenterValue(g_main_shd);

	for (i = 0; i < CLR_NUM; i++) {
		point = get_maxAveBlockPos(g_main_shd, i);
		UPRINTF("Max average color index(%d) block position (x:%d, y:%d)\n", i, point.x, point.y);
	}

	if (rval) return rval;

	point = ADJ_SearchCenter(g_main_shd, in);

	rval = ADJ_CheckCenter(point);
	rval |= ADJ_CheckCornerRate(g_main_shd);

	return rval;
}

static T_SHD_SHDDATA* SHD_AnalyzeMain(const USHORT* const in)
{
	int rectW, rectH;
	T_SHD_IMGSIZE divBlockSize;

	rectW = (int)((double)g_imgdata.size.width / (double)g_config.Div.width);
	rectH = (int)((double)g_imgdata.size.height / (double)g_config.Div.height);

	divBlockSize.width  = rectW;
	divBlockSize.height = rectH;

	if (g_config.Div.width > MAX_BLK_W_NUM || g_config.Div.height > MAX_BLK_H_NUM) {
		UPRINTF_ERR(-200);
		return NULL;
	}

	memset(g_main_shd, 0, sizeof(T_SHD_SHDDATA));

	g_main_shd->w_blk_num = g_config.Div.width;
	g_main_shd->h_blk_num = g_config.Div.height;

	g_main_shd->offset.width  = (rectW >> 1) - 1;
	g_main_shd->offset.height = (rectH >> 1) - 1;

	ADJ_AnalyzeBlockData(g_main_shd, in, divBlockSize);

	return g_main_shd;
}

static void SHD_GetOutData(T_SHD_SHDDATA* aImgData, USHORT* outData)
{
	int w, h;

	for (h = 0; h < g_config.Div.height; h++) {
		for (w = 0; w < g_config.Div.width; w++) {
			*outData++ = getbrightrate4096(
				    aImgData, g_imgdata.get_color_index(FJ_PRO_CTRL_FIRST_PIX_COLOR_R), w, h,
				    aImgData->brightness[g_imgdata.get_color_index(FJ_PRO_CTRL_FIRST_PIX_COLOR_R)]);
		}
	}

	for (h = 0; h < g_config.Div.height; h++) {
		for (w = 0; w < g_config.Div.width; w++) {
			*outData++ = getbrightrate4096(
				    aImgData, g_imgdata.get_color_index(FJ_PRO_CTRL_FIRST_PIX_COLOR_GR), w, h,
				    aImgData->brightness[g_imgdata.get_color_index(FJ_PRO_CTRL_FIRST_PIX_COLOR_GR)]);
		}
	}

	for (h = 0; h < g_config.Div.height; h++) {
		for (w = 0; w < g_config.Div.width; w++) {
				*outData++ = getbrightrate4096(
				    aImgData, g_imgdata.get_color_index(FJ_PRO_CTRL_FIRST_PIX_COLOR_GB), w, h,
				    aImgData->brightness[g_imgdata.get_color_index(FJ_PRO_CTRL_FIRST_PIX_COLOR_GB)]);
		}
	}

	for (h = 0; h < g_config.Div.height; h++) {
		for (w = 0; w < g_config.Div.width; w++) {
				*outData++ = getbrightrate4096(
				    aImgData, g_imgdata.get_color_index(FJ_PRO_CTRL_FIRST_PIX_COLOR_B), w, h,
				    aImgData->brightness[g_imgdata.get_color_index(FJ_PRO_CTRL_FIRST_PIX_COLOR_B)]);
		}
	}
}
static void ADJ_AnalyzeBlockData(T_SHD_SHDDATA* shddata, const USHORT* const in,
                                 const T_SHD_IMGSIZE aDivBlockSize)
{
	int w, h;
	int startX, startY, xWidth, yHeight;
	T_BLK_RECT blockRect;

	for (h = 0; h < shddata->h_blk_num; h++) {
		for (w = 0; w < shddata->w_blk_num; w++) {
			startX  = w * aDivBlockSize.width;
			startY  = h * aDivBlockSize.height;
			xWidth  = (w + 1 == shddata->w_blk_num) ? (g_imgdata.size.width - startX)
			                                        : aDivBlockSize.width;
			yHeight = (h + 1 == shddata->h_blk_num) ? (g_imgdata.size.height - startY)
			                                        : aDivBlockSize.height;

			startX -= shddata->offset.width;
			startY -= shddata->offset.height;

			blockRect.width      = xWidth;
			blockRect.height     = yHeight;
			blockRect.x          = startX;
			blockRect.y          = startY;
			blockRect.location.x = startX;
			blockRect.location.y = startY;

			ADJ_GetRawData(shddata, in, blockRect, &shddata->blkdata[h][w], w, h);
		}
	}
	Set_MaxBrightness(shddata);
}

static void ADJ_GetRawData(T_SHD_SHDDATA* shddata, const USHORT* const in,
                           const T_BLK_RECT aBlockRect, T_SHD_BLKDATA* aShdData, const int blk_x,
                           const int blk_y)
{
	int clrIndex[4];
	int x, y;

	aShdData->blockrect = aBlockRect;

	ADJ_StartPixel(aBlockRect, clrIndex);

	for (y = 0; y < (aBlockRect.height >> 1); y++) {
		for (x = 0; x < (aBlockRect.width >> 1); x++) {
			if (aBlockRect.x + (x << 1) >= 0 && aBlockRect.y + (y << 1) >= 0) {
				set_rawdata(shddata, clrIndex[0], blk_x, blk_y, x, y,
				            ADJ_GetRawPixel(in, aBlockRect.x + (x << 1), aBlockRect.y + (y << 1)));
			}

			if (aBlockRect.x + ((x << 1) | 1) >= 0 && aBlockRect.y + (y << 1) >= 0) {
				set_rawdata(
				    shddata, clrIndex[1], blk_x, blk_y, x, y,
				    ADJ_GetRawPixel(in, aBlockRect.x + ((x << 1) | 1), aBlockRect.y + (y << 1)));
			}

			if (aBlockRect.x + (x << 1) >= 0 && aBlockRect.y + ((y << 1) | 1) >= 0) {
				set_rawdata(
				    shddata, clrIndex[2], blk_x, blk_y, x, y,
				    ADJ_GetRawPixel(in, aBlockRect.x + (x << 1), aBlockRect.y + ((y << 1) | 1)));
			}

			if (aBlockRect.x + ((x << 1) | 1) >= 0 && aBlockRect.y + ((y << 1) | 1) >= 0) {
				set_rawdata(shddata, clrIndex[3], blk_x, blk_y, x, y,
				            ADJ_GetRawPixel(in, aBlockRect.x + ((x << 1) | 1),
				                            aBlockRect.y + ((y << 1) | 1)));
			}
		}
	}
}

static void ADJ_StartPixel(const T_BLK_RECT aBlockRect, int* clrIndex)
{
	if ((abs(aBlockRect.x) & 0x1) != 0) {
		if ((abs(aBlockRect.y) & 0x1) != 0) {
			clrIndex[0] = 3;
			clrIndex[1] = 2;
			clrIndex[2] = 1;
			clrIndex[3] = 0;
		} else {
			clrIndex[0] = 1;
			clrIndex[1] = 0;
			clrIndex[2] = 3;
			clrIndex[3] = 2;
		}
	} else {
		if ((abs(aBlockRect.y) & 0x1) != 0) {
			clrIndex[0] = 2;
			clrIndex[1] = 3;
			clrIndex[2] = 0;
			clrIndex[3] = 1;
		} else {
			clrIndex[0] = 0;
			clrIndex[1] = 1;
			clrIndex[2] = 2;
			clrIndex[3] = 3;
		}
	}
}

static USHORT ADJ_GetRawPixel(const USHORT* const in, int aX, int aY)
{
	int xy = (g_imgdata.size.width * aY) + aX;
	return in[xy];
}

static int ADJ_CheckCenterValue(T_SHD_SHDDATA* shddata)
{
	int xPos, yPos;
	double average;
	int rval = 0;

	xPos = (shddata->w_blk_num + 1) / 2 - 1;
	yPos = (shddata->h_blk_num + 1) / 2 - 1;

	average =
	    getaverage(shddata, g_imgdata.get_color_index(FJ_PRO_CTRL_FIRST_PIX_COLOR_GR), xPos, yPos);
	if (average < INIT_CENT_ALLOW_LOW || INIT_CENT_ALLOW_HIGH < average) {
		UPRINTF("The center block brightness (GR): %f is out of range %d ~ %d\n", average,
		        INIT_CENT_ALLOW_LOW, INIT_CENT_ALLOW_HIGH);
		UPRINTF_ERR(-5);
		rval = -5;
	}

	average =
	    getaverage(shddata, g_imgdata.get_color_index(FJ_PRO_CTRL_FIRST_PIX_COLOR_GB), xPos, yPos);
	if (average < INIT_CENT_ALLOW_LOW || INIT_CENT_ALLOW_HIGH < average) {
		UPRINTF("The center block brightness (GB): %f is out of range %d ~ %d\n", average,
		        INIT_CENT_ALLOW_LOW, INIT_CENT_ALLOW_HIGH);
		UPRINTF_ERR(-5);
		rval = -5;
	}

	return rval;
}

static T_SHD_POINT ADJ_SearchCenter(T_SHD_SHDDATA* shddata, const USHORT* const in)
{
	T_BLK_RECT arroundRect;
	T_SHD_POINT findCenters[CLR_NUM];
	T_SHD_POINT tempDbg;
	T_BLK_RECT trect;
	T_SHD_POINT point            = {0};
	T_SHD_SHDDATA* imgDataDetail = NULL;
	T_SHD_SHDDATA* imgDataRough  = NULL;
	int i;

	arroundRect = ADJ_GetMaxAveAroundRect(shddata, INIT_CENTER_ARROUND);

	imgDataRough = ADJ_AnalyzeAboutCenter(in, arroundRect);

	for (i = 0; i < CLR_NUM; i++) {
		UPRINTF("fix size divide start (ConfigData.RoughDivide:%d)\n", g_config.RoughDivide);
		tempDbg = get_maxAveBlockPos(imgDataRough, i);
		UPRINTF("Max div pos color:%d = x:%d, y:%d\n", g_imgdata.get_pixel_color(i), tempDbg.x,
		        tempDbg.y);
		trect = imgDataRough->blkdata[tempDbg.y][tempDbg.x].blockrect;
		UPRINTF("Max Img pos color:%d = x:%d, y:%d\n", g_imgdata.get_pixel_color(i), trect.x,
		        trect.y);

		imgDataDetail = ADJ_AnalyzeShiftBlockColor(in, ADJ_GetMaxAveAroundRect(imgDataRough, 1),
		                                           g_config.DetailDivide, i);

		point          = get_maxAveBlockPos(imgDataDetail, i);
		findCenters[i] = get_maxValImgPos(imgDataDetail, i, point.x, point.y);
	}

	return ADJ_GetSarchedCenterPosMixColor(findCenters);
}

static T_BLK_RECT ADJ_GetMaxAveAroundRect(T_SHD_SHDDATA* shddata, int aArroundShift)
{
	T_SHD_POINT minPos = {0};
	T_SHD_POINT maxPos = {0};
	T_SHD_POINT blockPos;
	T_BLK_RECT locate;
	T_BLK_RECT tempRect;
	T_BLK_RECT trect;
	T_BLK_RECT result;

	int leftLocateX;
	int leftLocateY;
	int rectX;
	int rectY;
	int i;

	for (i = 0; i < CLR_NUM; i++) {
		blockPos = get_maxAveBlockPos(shddata, i);

		UPRINTF("Max div pos color index:%d (x:%d,y:%d)\n", i, blockPos.x, blockPos.y);
		trect = shddata->blkdata[blockPos.y][blockPos.x].blockrect;
		UPRINTF("Max Img Locate pos color index:%d (x:%d,y:%d)\n", i, trect.x, trect.y);

		if (i == 0) {
			minPos = blockPos;
			maxPos = blockPos;
		} else {
			minPos.x = (minPos.x > blockPos.x) ? blockPos.x : minPos.x;
			minPos.y = (minPos.y > blockPos.y) ? blockPos.y : minPos.y;
			maxPos.x = (maxPos.x < blockPos.x) ? blockPos.x : maxPos.x;
			maxPos.y = (maxPos.y < blockPos.y) ? blockPos.y : maxPos.y;
		}
	}

	leftLocateX = (minPos.x - aArroundShift < 0) ? 0 : minPos.x - aArroundShift;
	leftLocateY = (minPos.y - aArroundShift < 0) ? 0 : minPos.y - aArroundShift;
	rectX       = (maxPos.x + aArroundShift >= shddata->w_blk_num) ? (shddata->w_blk_num - 1)
	                                                               : maxPos.x + aArroundShift;
	rectY       = (maxPos.y + aArroundShift >= shddata->h_blk_num) ? (shddata->h_blk_num - 1)
	                                                               : maxPos.y + aArroundShift;

	locate   = shddata->blkdata[leftLocateY][leftLocateX].blockrect;
	tempRect = shddata->blkdata[rectY][rectX].blockrect;

	result.height     = tempRect.y + tempRect.height - locate.location.y;
	result.width      = tempRect.x + tempRect.width - locate.location.x;
	result.x          = locate.location.x;
	result.y          = locate.location.y;
	result.location.x = locate.location.x;
	result.location.y = locate.location.y;

	UPRINTF("Avg around rect, x:%d, width:%d, y:%d, height:%d\n", result.x, result.width, result.y,
	        result.height);
	return result;
}

static T_SHD_SHDDATA* ADJ_AnalyzeAboutCenter(const USHORT* const in, T_BLK_RECT aSearchArea)
{
	int divMaxX;
	int divMaxY;
	int w, h;
	int startX, startY;
	T_BLK_RECT blockRect;
#if defined(SHD_DEBUG)
	FJ_PRO_CTRL_FIRST_PIX_COLOR color;
#endif

	aSearchArea.height =
	    (aSearchArea.height > g_imgdata.size.height) ? g_imgdata.size.height : aSearchArea.height;
	aSearchArea.width =
	    (aSearchArea.width > g_imgdata.size.width) ? g_imgdata.size.width : aSearchArea.width;

	g_config.RoughDivide =
	    (aSearchArea.width < INIT_ROUGH_DIVIDE) ? aSearchArea.width : INIT_ROUGH_DIVIDE;
	g_config.RoughDivide =
	    (aSearchArea.height < INIT_ROUGH_DIVIDE) ? aSearchArea.height : INIT_ROUGH_DIVIDE;

	divMaxX = aSearchArea.width / g_config.RoughDivide;
	divMaxY = aSearchArea.height / g_config.RoughDivide;

	if (divMaxX > MAX_BLK_W_NUM || divMaxY > MAX_BLK_H_NUM) {
		UPRINTF("The block number w:%d h:%d is out of MAX w:%d h:%d\n", divMaxX, divMaxY,
		        MAX_BLK_W_NUM, MAX_BLK_H_NUM);
		UPRINTF_ERR(-200);
		return NULL;
	}

	memset(g_rough_shd, 0, sizeof(T_SHD_SHDDATA));

	g_rough_shd->w_blk_num = divMaxX;
	g_rough_shd->h_blk_num = divMaxY;

	for (h = 0; h < divMaxY; h++) {
		for (w = 0; w < divMaxX; w++) {
			startX = (w * g_config.RoughDivide) + aSearchArea.x;
			startY = (h * g_config.RoughDivide) + aSearchArea.y;

			blockRect.x          = startX;
			blockRect.y          = startY;
			blockRect.width      = g_config.RoughDivide;
			blockRect.height     = g_config.RoughDivide;
			blockRect.location.x = startX;
			blockRect.location.y = startY;

			ADJ_GetRawData(g_rough_shd, in, blockRect, &g_rough_shd->blkdata[h][w], w, h);
#if defined(SHD_DEBUG)
			UPRINTF("divide Block = w:%d, h:%d\n", w, h);
			for (int i = 0; i < CLR_NUM; i++) {
				color = g_imgdata.get_pixel_color(i);
				UPRINTF("Average col:%d = average:%f\n", color, getaverage(g_rough_shd, i, w, h));
				UPRINTF("Sum col:%d, sum:%d\n", color, getsum(g_rough_shd, i, w, h));
				UPRINTF("Num col:%d, num:%d\n", color, getnum(g_rough_shd, i, w, h));
			}
#endif
		}
	}

	return g_rough_shd;
}

static T_SHD_SHDDATA* ADJ_AnalyzeShiftBlockColor(const USHORT* const in, T_BLK_RECT aSearchArea,
                                                 int aDivSize, int aColor)
{
	int divMaxY;
	int divMaxX;
	int w, h;
	int startX, startY;
	T_BLK_RECT blockRect;

	divMaxY = (aSearchArea.height - aDivSize) / DETAIL_SHIFT;
	divMaxX = (aSearchArea.width - aDivSize) / DETAIL_SHIFT;

	if (divMaxX > MAX_BLK_W_NUM || divMaxY > MAX_BLK_H_NUM) {
		UPRINTF("The block number w:%d h:%d is out of MAX w:%d h:%d\n", divMaxX, divMaxY,
		        MAX_BLK_W_NUM, MAX_BLK_H_NUM);
		UPRINTF_ERR(-200);
		return NULL;
	}

	memset(g_detail_shd, 0, sizeof(T_SHD_SHDDATA));

	g_detail_shd->w_blk_num = divMaxX;
	g_detail_shd->h_blk_num = divMaxY;

	for (h = 0; h < divMaxY; h++) {
		for (w = 0; w < divMaxX; w++) {
			startX = (w * DETAIL_SHIFT) + aSearchArea.x;
			startY = (h * DETAIL_SHIFT) + aSearchArea.y;

			blockRect.x          = startX;
			blockRect.y          = startY;
			blockRect.location.x = startX;
			blockRect.location.y = startY;
			blockRect.width      = aDivSize;
			blockRect.height     = aDivSize;

			ADJ_GetRawDataColor(g_detail_shd, in, blockRect, aColor, w, h);
		}
	}

	return g_detail_shd;
}

static void ADJ_GetRawDataColor(T_SHD_SHDDATA* shddata, const USHORT* const in,
                                T_BLK_RECT aBlockRect, int aColor, USHORT blk_x, USHORT blk_y)
{
	int x, y;

	shddata->blkdata[blk_y][blk_x].blockrect = aBlockRect;

	for (y = 0; y < (aBlockRect.height >> 1); y++) {
		for (x = 0; x < (aBlockRect.width >> 1); x++) {
			if (aColor == 0) {
				set_rawdata(shddata, 0, blk_x, blk_y, x, y,
				            ADJ_GetRawPixel(in, aBlockRect.x + (x << 1), aBlockRect.y + (y << 1)));
			} else if (aColor == 1) {
				set_rawdata(
				    shddata, 1, blk_x, blk_y, x, y,
				    ADJ_GetRawPixel(in, aBlockRect.x + ((x << 1) | 1), aBlockRect.y + (y << 1)));
			} else if (aColor == 2) {
				set_rawdata(
				    shddata, 2, blk_x, blk_y, x, y,
				    ADJ_GetRawPixel(in, aBlockRect.x + (x << 1), aBlockRect.y + ((y << 1) | 1)));
			} else if (aColor == 3) {
				set_rawdata(shddata, 3, blk_x, blk_y, x, y,
				            ADJ_GetRawPixel(in, aBlockRect.x + ((x << 1) | 1),
				                            aBlockRect.y + ((y << 1) | 1)));
			} else {
				set_rawdata(shddata, 0, blk_x, blk_y, x, y,
				            ADJ_GetRawPixel(in, aBlockRect.x + (x << 1), aBlockRect.y + (y << 1)));
				set_rawdata(
				    shddata, 1, blk_x, blk_y, x, y,
				    ADJ_GetRawPixel(in, aBlockRect.x + ((x << 1) | 1), aBlockRect.y + (y << 1)));
				set_rawdata(
				    shddata, 2, blk_x, blk_y, x, y,
				    ADJ_GetRawPixel(in, aBlockRect.x + (x << 1), aBlockRect.y + ((y << 1) | 1)));
				set_rawdata(shddata, 3, blk_x, blk_y, x, y,
				            ADJ_GetRawPixel(in, aBlockRect.x + ((x << 1) | 1),
				                            aBlockRect.y + ((y << 1) | 1)));
			}
		}
	}
}

static T_SHD_POINT ADJ_GetSarchedCenterPosMixColor(T_SHD_POINT* aFindCenters)
{
	int sumX = 0;
	int sumY = 0;
	int centerX;
	int centerY;
	T_SHD_POINT searchedCenter;
	int i;

	for (i = 0; i < 4; i++) {
		sumX += aFindCenters[i].x;
		sumY += aFindCenters[i].y;
		UPRINTF("aFindCenters[%d].x:%d, aFindCenters[%d].y:%d\n", i, aFindCenters[i].x, i,
		        aFindCenters[i].y);
	}

	centerX = (int)((double)sumX / (double)4.0);
	centerY = (int)((double)sumY / (double)4.0);

	searchedCenter.x = centerX;
	searchedCenter.y = centerY;

	return searchedCenter;
}

static int ADJ_CheckCenter(T_SHD_POINT aCenterPoint)
{
	int diffX, diffY;
	double rateX, rateY;
	int rval = 0;

	diffX = abs(g_imgdata.center.x - aCenterPoint.x);
	diffY = abs(g_imgdata.center.y - aCenterPoint.y);
	rateX = (double)diffX / (double)g_imgdata.size.width * 100.0;
	rateY = (double)diffY / (double)g_imgdata.size.height * 100.0;

	if (rateX > (double)g_config.center_pos_rate || rateY > (double)g_config.center_pos_rate) {
		UPRINTF("The optics center poition is out of range. center pos x:%d, y:%d, "
		        "img_cen_pos_x:%d, img_cen_pos_y:%d, cen_pos_rate:%d\n",
		        aCenterPoint.x, aCenterPoint.y, g_imgdata.center.x, g_imgdata.center.y,
		        g_config.center_pos_rate);
		rval = -10;
	}

	return rval;
}

static int ADJ_CheckCornerRate(T_SHD_SHDDATA* aShdData)
{
	int i;
	int rval = 0;

	for (i = 0; i < CLR_NUM; i++) {
		if (getbrightrate100(aShdData, i, 0, 0, aShdData->brightness[i]) <
		    g_config.shd_corner_br_thre) {
			UPRINTF("The corner block brightness rate is out of range. rate = %f\n",
			        getbrightrate100(aShdData, i, 0, 0, aShdData->brightness[i]));
			rval = -11;
		} else if (getbrightrate100(aShdData, i, 0, g_config.Div.height - 1,
		                            aShdData->brightness[i]) < g_config.shd_corner_br_thre) {
			UPRINTF(
			    "The corner block brightness rate is out of range. rate = %f\n",
			    getbrightrate100(aShdData, i, 0, g_config.Div.height - 1, aShdData->brightness[i]));
			rval = -11;
		} else if (getbrightrate100(aShdData, i, g_config.Div.width - 1, 0,
		                            aShdData->brightness[i]) < g_config.shd_corner_br_thre) {
			UPRINTF(
			    "The corner block brightness rate is out of range. rate = %f\n",
			    getbrightrate100(aShdData, i, g_config.Div.width - 1, 0, aShdData->brightness[i]));
			rval = -11;
		} else if (getbrightrate100(aShdData, i, g_config.Div.width - 1, g_config.Div.height - 1,
		                            aShdData->brightness[i]) < g_config.shd_corner_br_thre) {
			UPRINTF("The corner block brightness rate is out of range. rate = %f\n",
			        getbrightrate100(aShdData, i, g_config.Div.width - 1, g_config.Div.height - 1,
			                         aShdData->brightness[i]));
			rval = -11;
		}
	}

	return rval;
}

static FJ_PRO_CTRL_FIRST_PIX_COLOR get_color(const UCHAR index)
{
	return g_s_pix_order[g_imgdata.s_pix][index];
}

static UCHAR get_color_index(const FJ_PRO_CTRL_FIRST_PIX_COLOR color)
{
	UCHAR i;

	for (i = 0; i < 4; i++) {
		if (g_s_pix_order[g_imgdata.s_pix][i] == color) break;
	}
	return i;
}

static void set_center(void)
{
	g_imgdata.center.x = (USHORT)((float)g_imgdata.size.width / (float)2.0);
	g_imgdata.center.y = (USHORT)((float)g_imgdata.size.height / (float)2.0);
}

static void set_rawdata(T_SHD_SHDDATA* shddata, const UCHAR c_index, const USHORT blk_x,
                        const USHORT blk_y, const int x, const int y, const USHORT val)
{
	shddata->blkdata[blk_y][blk_x].rawdata[c_index].sum += val;
	shddata->blkdata[blk_y][blk_x].rawdata[c_index].num++;
	shddata->blkdata[blk_y][blk_x].rawdata[c_index].max =
	    shddata->blkdata[blk_y][blk_x].rawdata[c_index].max < val
	        ? val
	        : shddata->blkdata[blk_y][blk_x].rawdata[c_index].max;

	if (shddata->blkdata[blk_y][blk_x].rawdata[c_index].max == val) {
		shddata->blkdata[blk_y][blk_x].rawdata[c_index].maxX = x;
		shddata->blkdata[blk_y][blk_x].rawdata[c_index].maxY = y;
	}
}

static USHORT getbrightrate4096(T_SHD_SHDDATA* shddata, const UCHAR c_index, const USHORT blk_x,
                                const USHORT blk_y, const double centerbright)
{
	double temp;

	temp = (double)shddata->blkdata[blk_y][blk_x].rawdata[c_index].sum /
	       (double)shddata->blkdata[blk_y][blk_x].rawdata[c_index].num;
	temp = centerbright / temp;

	if (temp >= 1.0) {
		temp -= 1.0;
	} else {
		UPRINTF("color:%d, blkx:%d, blky:%d, BR:%lf, centerBR:%lf\n", c_index, blk_x, blk_y, temp,
		        centerbright);
		UPRINTF_ERR(-5);
	}
	temp = temp / 100.0 * g_config.shd_comp_rate;
	temp += 1.0;
	temp = temp * 4096.0;

	return (USHORT)temp;
}

static double getbrightrate100(T_SHD_SHDDATA* shddata, const UCHAR c_index, const USHORT blk_x,
                               const USHORT blk_y, const double centerbright)
{
	double temp;

	temp = (double)shddata->blkdata[blk_y][blk_x].rawdata[c_index].sum /
	       (double)shddata->blkdata[blk_y][blk_x].rawdata[c_index].num;
	UPRINTF("Color:%d, blk_x:%d, blk_y:%d, brightness:%lf, most BR:%lf\n", c_index, blk_x, blk_y,
	        temp, centerbright);
	temp = temp / centerbright * 100.0;

	return temp;
}

static double getaverage(T_SHD_SHDDATA* shddata, const UCHAR c_index, const USHORT blk_x,
                         const USHORT blk_y)
{
	return (double)shddata->blkdata[blk_y][blk_x].rawdata[c_index].sum /
	       (double)shddata->blkdata[blk_y][blk_x].rawdata[c_index].num;
}

#if defined(SHD_DEBUG)
static int getsum(T_SHD_SHDDATA* shddata, const UCHAR c_index, const USHORT blk_x,
                  const USHORT blk_y)
{
	return shddata->blkdata[blk_y][blk_x].rawdata[c_index].sum;
}

static int getnum(T_SHD_SHDDATA* shddata, const UCHAR c_index, const USHORT blk_x,
                  const USHORT blk_y)
{
	return shddata->blkdata[blk_y][blk_x].rawdata[c_index].num;
}
#endif

static T_SHD_POINT getmax_pos_in_blk(T_SHD_SHDDATA* shddata, const UCHAR c_index,
                                     const USHORT blk_x, const USHORT blk_y)
{
	T_SHD_POINT point;

	point.x = shddata->blkdata[blk_y][blk_x].rawdata[c_index].maxX;
	point.y = shddata->blkdata[blk_y][blk_x].rawdata[c_index].maxY;

	return point;
}

static void Set_MaxBrightness(T_SHD_SHDDATA* shddata)
{
	int i;

	for (i = 0; i < CLR_NUM; i++) { shddata->brightness[i] = Get_MaxAverage(shddata, (UCHAR)i); }
}

static double Get_MaxAverage(T_SHD_SHDDATA* shddata, const UCHAR c_index)
{
	int x, y;
	double tempAve = 0.0;

	for (y = 0; y < shddata->h_blk_num; y++) {
		for (x = 0; x < shddata->w_blk_num; x++) {
			if (tempAve < getaverage(shddata, c_index, x, y)) {
				tempAve = getaverage(shddata, c_index, x, y);
			}
		}
	}

	return tempAve;
}

static T_SHD_POINT get_maxAveBlockPos(T_SHD_SHDDATA* shddata, const UCHAR c_index)
{
	int x, y;
	double tempAve = 0.0;
	T_SHD_POINT point;

	for (y = 0; y < shddata->h_blk_num; y++) {
		for (x = 0; x < shddata->w_blk_num; x++) {
			if (tempAve < getaverage(shddata, c_index, x, y)) {
				tempAve = getaverage(shddata, c_index, x, y);
				point.x = x;
				point.y = y;
			}
		}
	}

	return point;
}

static T_SHD_POINT get_maxValImgPos(T_SHD_SHDDATA* shddata, const UCHAR c_index, const USHORT blk_x,
                                    const USHORT blk_y)
{
	T_SHD_POINT point;

	point = getmax_pos_in_blk(shddata, c_index, blk_x, blk_y);
	point.x += shddata->blkdata[blk_y][blk_x].blockrect.x;
	point.y += shddata->blkdata[blk_y][blk_x].blockrect.y;

	return point;
}
