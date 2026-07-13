/**
 * @file		awb_task.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

//#pragma GCC optimize ("O0")

#include "share.h"
#include "share_data.h"
#include "awb_task.h"
#include "l1l2cache.h"
#include "AlTypStd.h"
#include "AlAwblib.h"
#include "AlAwbSetupProduct.h"
#include "AlHscWrap.h"
#include "category_parameter.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define AWB_MAX_BLK_H	D_SHARE_AEAWB_MAX_BLK_H
#define AWB_MAX_BLK_V	D_SHARE_AEAWB_MAX_BLK_V
#define AWB_MAX_LVL 	8

#define AL_AWB_STARTUP_VD_NUM (4)

#define AL_AWB_DEFAULT_X  AL_FIX_VAL(0.332)
#define AL_AWB_DEFAULT_Y  AL_FIX_VAL(0.347)

#define AWB_ALL_OUT_OF_RANGE_STABI
#define INVALID_DETECT_XY (0xA5A5A5A5) //Do not edit this value .
#define AWB_UNSTABLE_WHITECOUNT	(16)

#define AL_AWB_FAIL_X  AL_FIX_VAL(0.332)
#define AL_AWB_FAIL_Y  AL_FIX_VAL(0.347)

#define AL_AWB_ADJ_CC_STARTUP_CNT	(10)
#define AL_AWB_ADJ_CC_NORMAL_CNT	(30)

#define AWB_NO_BIAS	0x10000			// 1.0
#define AWB_BIAS_R	0x0
#define AWB_BIAS_G	0x1
#define AWB_BIAS_B	0x2

#define AWB_ALL_OUT_OF_RANGE_STABI

#define AWB_RESIZE_BLOCK_H 16
#define AWB_RESIZE_BLOCK_V 16

#define MANUAL_SCENE_MAX 8
#define CATE_MIM_COLOR_TEMP		(25)  // 2500K
#define CATE_MAX_COLOR_TEMP		(125) // 12500K
#define CATE_RANGE_COLOR_TEMP	(CATE_MAX_COLOR_TEMP - CATE_MIM_COLOR_TEMP + 1)

#define RN_FIX_VAL(n) ( (float)( (n) / 65536.0 ) )

#define D_FORCED_INDOOR_BV AL_FIX_VAL(3)
#define D_FORCED_OUTDOOR_BV AL_FIX_VAL(9)
/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
typedef enum {
	E_AWB_AUTO = 0,
	E_AWB_MANUAL
} E_AWB_MODE;

typedef enum {
	E_AWB_CLOUDY = 1,
	E_AWB_DAYLIGHT,
	E_AWB_FLASH,
	E_AWB_CW,
	E_AWB_TUNGSTEN,
	E_AWB_CANDLELIGHT,
	E_AWB_HORIZON,
	E_AWB_CUSTOM_CT,
	E_AWB_CUSTOM_XY,
	E_AWB_CUSTOM_GAIN_CC,
	E_AWB_MODE_MAX,
} E_AWB_MANUL_MODE;

typedef enum {
	eAlAwbWrapXcwMode_Hi = 0,
	eAlAwbWrapXcwMode_NorHi,
	eAlAwbWrapXcwMode_NorLo,
	eAlAwbWrapXcwMode_Lo,

	eAlAwbWrapXcwMode_inMax
} TE_AlAwbWrapXcwMode;

//x coordinate table Ctemp Threshold
typedef enum {
	eAlAwbWrapCtmpSts1BvH = 0,
	eAlAwbWrapCtmpSts2BvH,
	eAlAwbWrapCtmpSts3BvH,
	eAlAwbWrapCtmpSts4BvH,
	eAlAwbWrapCtmpSts5BvH,
	eAlAwbWrapCtmpSts6BvH,
	eAlAwbWrapCtmpSts7BvH,
	eAlAwbWrapCtmpSts1BvL,
	eAlAwbWrapCtmpSts2BvL,
	eAlAwbWrapCtmpSts3BvL,
	eAlAwbWrapCtmpSts4BvL,
	eAlAwbWrapCtmpSts5BvL,
	eAlAwbWrapCtmpSts6BvL,
	eAlAwbWrapCtmpSts7BvL,

	eAlAwbWrapCtmpSts_inMax
} TE_AlAwbWrapCTempStatusIndoor;
/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	ULONG cnt;
	ULONG r;
	ULONG g;
	ULONG b;
} T_AWB_STAT;

typedef struct {
	ULONG cnt;
	ULONG r;
	ULONG g;
	ULONG b;
} T_AWB_STAT_LVL;

typedef struct {
	ULONG r;
	ULONG gr;
	ULONG gb;
	ULONG b;
} T_AWB_DATA;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_AWB_STAT_LVL	g_awblvl_blk[E_SHARE_SENSOR_NUM_MAX][AWB_MAX_BLK_V][AWB_MAX_BLK_H][AWB_MAX_LVL];
static T_AWB_STAT_LVL	g_awblvl_sum[AWB_MAX_BLK_V][AWB_MAX_BLK_H][AWB_MAX_LVL];
static T_AWB_STAT		g_awbblk[E_SHARE_SENSOR_NUM_MAX][AWB_MAX_BLK_V][AWB_MAX_BLK_H];
static T_AWB_STAT		g_awbblk_sum[AWB_MAX_BLK_V][AWB_MAX_BLK_H];

static ULONG			g_awbcntblk[E_SHARE_SENSOR_NUM_MAX][AWB_MAX_BLK_V][AWB_MAX_BLK_H];
static T_AWB_DATA		g_awbdatablk[E_SHARE_SENSOR_NUM_MAX][AWB_MAX_BLK_V][AWB_MAX_BLK_H];
static BOOL 			g_print_on = FALSE;

static T_SHARE_AWB_PARAM g_awb_result;

static UI_32 g_fsiParam[E_SHARE_SENSOR_NUM_MAX][(AL_AWB_PARAM_SZ) / sizeof(ULONG)];
static UI_32 g_fsiShare[E_SHARE_SENSOR_NUM_MAX][(AL_AWB_SHARE_SZ) / sizeof(ULONG)];
static UI_32 g_fsiStat[E_SHARE_SENSOR_NUM_MAX][(AL_AWB_STATISTICS_SZ) / sizeof(ULONG)];

static const TT_AwbWorkMem *g_attWorkMem[E_SHARE_SENSOR_NUM_MAX];
static TT_AwbWorkMem g_attWorkMemArray[E_SHARE_SENSOR_NUM_MAX] = {
	{g_fsiParam[0], g_fsiShare[0], g_fsiStat[0]},
	{g_fsiParam[1], g_fsiShare[1], g_fsiStat[1]},
	{g_fsiParam[2], g_fsiShare[2], g_fsiStat[2]},
	{g_fsiParam[3], g_fsiShare[3], g_fsiStat[3]},
};

static const TT_AlAwbSetupProduct* fpSetup[E_SHARE_SENSOR_NUM_MAX] ={(const TT_AlAwbSetupProduct*)0};
static const TT_AlAwbSetupProduct*	fptSetupModule[E_SHARE_SENSOR_NUM_MAX] = {
	&gttAlAwbSetupProduct0,
	&gttAlAwbSetupProduct1,
	&gttAlAwbSetupProduct2,
	&gttAlAwbSetupProduct3,
};

static UI_32 fuiAwbStartupCounter = 0;
static UI_08 fuiXCTblModeOutdoor[E_SHARE_SENSOR_NUM_MAX] = {OUTDOOR_XC_MODE_BV_LOW, OUTDOOR_XC_MODE_BV_LOW, OUTDOOR_XC_MODE_BV_LOW, OUTDOOR_XC_MODE_BV_LOW};
static UI_08 fuiXCTblModeIndoor[E_SHARE_SENSOR_NUM_MAX] = {(UI_08)eAlAwbWrapXcwMode_NorHi, (UI_08)eAlAwbWrapXcwMode_NorHi, (UI_08)eAlAwbWrapXcwMode_NorHi, (UI_08)eAlAwbWrapXcwMode_NorHi};

static const UI_08 fuiXCStatusTblIndoor[eAlAwbWrapCtmpSts_inMax][eAlAwbWrapXcwMode_inMax] = 
{
	//XcTblModeHi				XcTblModeNorHi				XcTblModeNorLo				XcTblModeLo
	{eAlAwbWrapXcwMode_Hi		,eAlAwbWrapXcwMode_Hi		,eAlAwbWrapXcwMode_Hi		,eAlAwbWrapXcwMode_Hi		},	// CtmpSts1BvH
	{eAlAwbWrapXcwMode_Hi		,eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	},	// CtmpSts2BvH
	{eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	},	// CtmpSts3BvH
	{eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorLo	,eAlAwbWrapXcwMode_NorLo	},	// CtmpSts4BvH
	{eAlAwbWrapXcwMode_NorLo	,eAlAwbWrapXcwMode_NorLo	,eAlAwbWrapXcwMode_NorLo	,eAlAwbWrapXcwMode_NorLo	},	// CtmpSts5BvH
	{eAlAwbWrapXcwMode_NorLo	,eAlAwbWrapXcwMode_NorLo	,eAlAwbWrapXcwMode_Lo		,eAlAwbWrapXcwMode_Lo		},	// CtmpSts6BvH
	{eAlAwbWrapXcwMode_Lo		,eAlAwbWrapXcwMode_Lo		,eAlAwbWrapXcwMode_Lo		,eAlAwbWrapXcwMode_Lo		},	// CtmpSts7BvH
	
	{eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	},	// CtmpSts1BvL
	{eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	},	// CtmpSts2BvL
	{eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	},	// CtmpSts3BvL
	{eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorHi	,eAlAwbWrapXcwMode_NorLo	,eAlAwbWrapXcwMode_NorLo	},	// CtmpSts4BvL
	{eAlAwbWrapXcwMode_NorLo	,eAlAwbWrapXcwMode_NorLo	,eAlAwbWrapXcwMode_NorLo	,eAlAwbWrapXcwMode_NorLo	},	// CtmpSts5BvL
	{eAlAwbWrapXcwMode_NorLo	,eAlAwbWrapXcwMode_NorLo	,eAlAwbWrapXcwMode_NorLo	,eAlAwbWrapXcwMode_Lo		},	// CtmpSts6BvL
	{eAlAwbWrapXcwMode_Lo		,eAlAwbWrapXcwMode_Lo		,eAlAwbWrapXcwMode_Lo		,eAlAwbWrapXcwMode_Lo		},	// CtmpSts7BvL
};

static UI_08 fuiColorOffsetNo[E_SHARE_SENSOR_NUM_MAX] = {0};
static UI_32 g_awb_white_block_count;

static UI_32 g_awb_block_count;
static UI_32 g_awb_block_count_indoor;
static UI_32 g_awb_block_count_outdoor;
static UI_32 g_awb_block_count_green_map;
static UI_32 g_awb_block_count_skin_color_map;

static UI_32 g_awb_detect_block_count;
static UI_32 g_awb_detect_block_count_indoor;
static UI_32 g_awb_detect_block_count_outdoor;
static UI_32 g_awb_detect_block_count_green_map;
static UI_32 g_awb_detect_block_count_skin_color_map;
static UI_32 fttAwbWdWork[E_SHARE_SENSOR_NUM_MAX][AWB_MAX_BLK_V*4];

static SQ_32 afxWBGain[E_SHARE_SENSOR_NUM_MAX][3];
static SQ_32 afxCCMtrx[E_SHARE_SENSOR_NUM_MAX][9];

#ifdef AWB_ALL_OUT_OF_RANGE_STABI
static SQ_32 fsqPrevDetectX[E_SHARE_SENSOR_NUM_MAX] = {INVALID_DETECT_XY, INVALID_DETECT_XY, INVALID_DETECT_XY, INVALID_DETECT_XY};
static SQ_32 fsqPrevDetectY[E_SHARE_SENSOR_NUM_MAX] = {INVALID_DETECT_XY, INVALID_DETECT_XY, INVALID_DETECT_XY, INVALID_DETECT_XY};
#endif	//#ifdef AWB_ALL_OUT_OF_RANGE_STABI

#ifdef AWB_SUPPRESS_FAIL_WITH_MAP_CHANGES
static UI_08 fteMapRange[E_SHARE_SENSOR_NUM_MAX] = {eAwbMapOutdoor, eAwbMapOutdoor, eAwbMapOutdoor, eAwbMapOutdoor};
static UI_08 fuiDefaultModeEnable[E_SHARE_SENSOR_NUM_MAX] = {0};
#endif	//#ifdef AWB_SUPPRESS_FAIL_WITH_MAP_CHANGES

static SQ_32 fsqCurrentChromaGainAdjCC[E_SHARE_SENSOR_NUM_MAX][9] ={
	{
	 AL_FIX_VAL(1),	AL_FIX_VAL(0),	AL_FIX_VAL(0),
	 AL_FIX_VAL(0),	AL_FIX_VAL(1),	AL_FIX_VAL(0),
	 AL_FIX_VAL(0),	AL_FIX_VAL(0),	AL_FIX_VAL(1)
	},
	{
	 AL_FIX_VAL(1),	AL_FIX_VAL(0),	AL_FIX_VAL(0),
	 AL_FIX_VAL(0),	AL_FIX_VAL(1),	AL_FIX_VAL(0),
	 AL_FIX_VAL(0),	AL_FIX_VAL(0),	AL_FIX_VAL(1)
	},
	{
	 AL_FIX_VAL(1),	AL_FIX_VAL(0),	AL_FIX_VAL(0),
	 AL_FIX_VAL(0),	AL_FIX_VAL(1),	AL_FIX_VAL(0),
	 AL_FIX_VAL(0),	AL_FIX_VAL(0),	AL_FIX_VAL(1)
	},
	{
	 AL_FIX_VAL(1),	AL_FIX_VAL(0),	AL_FIX_VAL(0),
	 AL_FIX_VAL(0),	AL_FIX_VAL(1),	AL_FIX_VAL(0),
	 AL_FIX_VAL(0),	AL_FIX_VAL(0),	AL_FIX_VAL(1)
	},
};

#ifdef AWB_STARTUP_CHROMA_SUPRESS
STATIC SQ_32 fsqInitChromaGainAdjCC[E_SHARE_SENSOR_NUM_MAX][9] = {
	//0.5
	{AL_FIX_VAL(0.6063),	AL_FIX_VAL(0.3576),	AL_FIX_VAL(0.0361),
	 AL_FIX_VAL(0.1063),	AL_FIX_VAL(0.8576),	AL_FIX_VAL(0.0361),
	 AL_FIX_VAL(0.1063),	AL_FIX_VAL(0.3576),	AL_FIX_VAL(0.5361)
	},
	//0.5
	{AL_FIX_VAL(0.6063),	AL_FIX_VAL(0.3576),	AL_FIX_VAL(0.0361),
	 AL_FIX_VAL(0.1063),	AL_FIX_VAL(0.8576),	AL_FIX_VAL(0.0361),
	 AL_FIX_VAL(0.1063),	AL_FIX_VAL(0.3576),	AL_FIX_VAL(0.5361)
	},
	//0.5
	{AL_FIX_VAL(0.6063),	AL_FIX_VAL(0.3576),	AL_FIX_VAL(0.0361),
	 AL_FIX_VAL(0.1063),	AL_FIX_VAL(0.8576),	AL_FIX_VAL(0.0361),
	 AL_FIX_VAL(0.1063),	AL_FIX_VAL(0.3576),	AL_FIX_VAL(0.5361)
	},
	//0.5
	{AL_FIX_VAL(0.6063),	AL_FIX_VAL(0.3576),	AL_FIX_VAL(0.0361),
	 AL_FIX_VAL(0.1063),	AL_FIX_VAL(0.8576),	AL_FIX_VAL(0.0361),
	 AL_FIX_VAL(0.1063),	AL_FIX_VAL(0.3576),	AL_FIX_VAL(0.5361)
	}
};
#endif //AWB_STARTUP_CHROMA_SUPRESS

SQ_32 g_awb_manual_scene[MANUAL_SCENE_MAX][3] = {
	{ 0x00004DD3,	0x00005333, 0x19AF0000},	//0 
	{ 0x00004DD3,	0x00005333, 0x19AF0000},	//1 g_awb_manual_k = 0x41
	{ 0x000053B6,	0x00005852, 0x157C0000},	//2 g_awb_manual_k = 0x37
	{ 0x0000551F,	0x000058FC, 0x13880000},	//3 g_awb_manual_k = 0x32
	{ 0x00005FBE,	0x00006873, 0x0E740000},	//4 g_awb_manual_k = 0x25
	{ 0x00006F0E,	0x0000672B, 0x0BB80000},	//5 g_awb_manual_k = 0x1E
	{ 0x00007581,	0x00006A3D, 0x0AF00000},	//6 g_awb_manual_k = 0x1C
	{ 0x00007D2F,	0x000052F2, 0x08DE0000},	//7 g_awb_manual_k = 0x16->0x19Ç…ÉN?ÉbÉv
};

SQ_32 g_awb_manual_k[CATE_RANGE_COLOR_TEMP][2] = {
	{ 0x00007D2F	, 0x000052F2 },    // cate value = 0x19 (2500K) 0x07: Horizon
	{ 0x00006F0D	, 0x0000672B },    // cate value = 0x1A (2600K)
	{ 0x00006F0D	, 0x0000672B },    // cate value = 0x1B (2700K)
	{ 0x00007581	, 0x00006A3D },    // cate value = 0x1C (2800K) 0x06: Candlelight
	{ 0x00006F0D	, 0x0000672B },    // cate value = 0x1D (2900K)
	{ 0x00006F0E	, 0x0000672B },    // cate value = 0x1E (3000K) 0x05: Tungsten
	{ 0x00006D77	, 0x00006694 },    // cate value = 0x1F (3100K)
	{ 0x00006BE7	, 0x00006604 },    // cate value = 0x20 (3200K)
	{ 0x00006A51	, 0x0000656D },    // cate value = 0x21 (3300K)
	{ 0x000068C1	, 0x000064DD },    // cate value = 0x22 (3400K)
	{ 0x0000672B	, 0x00006446 },    // cate value = 0x23 (3500K)
	{ 0x00006617	, 0x000063A9 },    // cate value = 0x24 (3600K)
	{ 0x00005FBE	, 0x00006873 },    // cate value = 0x25 (3700K) 0x04: Cool White Fluorescent
	{ 0x000063EA	, 0x0000626E },    // cate value = 0x26 (3800K)
	{ 0x000062D0	, 0x000061D1 },    // cate value = 0x27 (3900K)
	{ 0x000061BD	, 0x00006134 },    // cate value = 0x28 (4000K)
	{ 0x000060B0	, 0x0000607C },    // cate value = 0x29 (4100K)
	{ 0x00005F9D	, 0x00005FC5 },    // cate value = 0x2A (4200K)
	{ 0x00005E90	, 0x00005F14 },    // cate value = 0x2B (4300K)
	{ 0x00005D7D	, 0x00005E5C },    // cate value = 0x2C (4400K)
	{ 0x00005C71	, 0x00005DA5 },    // cate value = 0x2D (4500K)
	{ 0x00005B98	, 0x00005D1B },    // cate value = 0x2E (4600K)
	{ 0x00005AC0	, 0x00005C91 },    // cate value = 0x2F (4700K)
	{ 0x000059EE	, 0x00005C08 },    // cate value = 0x30 (4800K)
	{ 0x00005916	, 0x00005B7E },    // cate value = 0x31 (4900K)
	{ 0x0000551F	, 0x000058FC },    // cate value = 0x32 (5000K) 0x03: Flash
	{ 0x000057A7	, 0x00005A78 },    // cate value = 0x33 (5100K)
	{ 0x00005710	, 0x000059F5 },    // cate value = 0x34 (5200K)
	{ 0x0000567A	, 0x00005978 },    // cate value = 0x35 (5300K)
	{ 0x000055E9	, 0x000058F5 },    // cate value = 0x36 (5400K)
	{ 0x000053B6	, 0x00005852 },    // cate value = 0x37 (5500K) 0x02: Daylight
	{ 0x000054BC	, 0x000057F6 },    // cate value = 0x38 (5600K)
	{ 0x00005425	, 0x00005779 },    // cate value = 0x39 (5700K)
	{ 0x000053A2	, 0x0000570A },    // cate value = 0x3A (5800K)
	{ 0x00005318	, 0x0000569A },    // cate value = 0x3B (5900K)
	{ 0x00005295	, 0x0000562B },    // cate value = 0x3C (6000K)
	{ 0x0000520C	, 0x000055C2 },    // cate value = 0x3D (6100K)
	{ 0x00005189	, 0x00005553 },    // cate value = 0x3E (6200K)
	{ 0x000050FF	, 0x000054E3 },    // cate value = 0x3F (6300K)
	{ 0x0000507C	, 0x00005474 },    // cate value = 0x40 (6400K)
	{ 0x00004DD3	, 0x00005333 },    // cate value = 0x41 (6500K) 0x01: Cloudy
	{ 0x00004FB7	, 0x000053BC },    // cate value = 0x42 (6600K)
	{ 0x00004F83	, 0x0000537B },    // cate value = 0x43 (6700K)
	{ 0x00004F48	, 0x00005333 },    // cate value = 0x44 (6800K)
	{ 0x00004F0D	, 0x000052EB },    // cate value = 0x45 (6900K)
	{ 0x00004ED9	, 0x000052A9 },    // cate value = 0x46 (7000K)
	{ 0x00004E9E	, 0x00005261 },    // cate value = 0x47 (7100K)
	{ 0x00004E63	, 0x00005219 },    // cate value = 0x48 (7200K)
	{ 0x00004E2E	, 0x000051D7 },    // cate value = 0x49 (7300K)
	{ 0x00004DF3	, 0x0000518F },    // cate value = 0x4A (7400K)
	{ 0x00004DB8	, 0x00005147 },    // cate value = 0x4B (7500K)
	{ 0x00004D84	, 0x000050FF },    // cate value = 0x4C (7600K)
	{ 0x00004D49	, 0x000050BE },    // cate value = 0x4D (7700K)
	{ 0x00004D0E	, 0x00005075 },    // cate value = 0x4E (7800K)
	{ 0x00004CD9	, 0x0000502D },    // cate value = 0x4F (7900K)
	{ 0x00004C9E	, 0x00004FEC },    // cate value = 0x50 (8000K)
	{ 0x00004C63	, 0x00004FA4 },    // cate value = 0x51 (8100K)
	{ 0x00004C28	, 0x00004F5C },    // cate value = 0x52 (8200K)
	{ 0x00004BF4	, 0x00004F1A },    // cate value = 0x53 (8300K)
	{ 0x00004BB9	, 0x00004ED2 },    // cate value = 0x54 (8400K)
	{ 0x00004B7E	, 0x00004E8A },    // cate value = 0x55 (8500K)
	{ 0x00004B4A	, 0x00004E48 },    // cate value = 0x56 (8600K)
	{ 0x00004B0F	, 0x00004E00 },    // cate value = 0x57 (8700K)
	{ 0x00004AD4	, 0x00004DB8 },    // cate value = 0x58 (8800K)
	{ 0x00004A9F	, 0x00004D77 },    // cate value = 0x59 (8900K)
	{ 0x00004A64	, 0x00004D2F },    // cate value = 0x5A (9000K)
	{ 0x00004A29	, 0x00004CE7 },    // cate value = 0x5B (9100K)
	{ 0x000049F5	, 0x00004C9E },    // cate value = 0x5C (9200K)
	{ 0x000049BA	, 0x00004C5D },    // cate value = 0x5D (9300K)
	{ 0x0000497F	, 0x00004C15 },    // cate value = 0x5E (9400K)
	{ 0x0000494A	, 0x00004BCD },    // cate value = 0x5F (9500K)
	{ 0x0000490F	, 0x00004B8B },    // cate value = 0x60 (9600K)
	{ 0x000048D4	, 0x00004B43 },    // cate value = 0x61 (9700K)
	{ 0x000048A0	, 0x00004AFB },    // cate value = 0x62 (9800K)
	{ 0x00004865	, 0x00004AB9 },    // cate value = 0x63 (9900K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x64 (10000K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x65 (10100K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x66 (10200K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x67 (10300K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x68 (10400K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x69 (10500K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x6A (10600K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x6B (10700K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x6C (10800K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x6D (10900K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x6E (11000K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x6F (11100K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x70 (11200K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x71 (11300K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x72 (11400K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x73 (11500K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x74 (11600K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x75 (11700K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x76 (11800K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x77 (11900K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x78 (12000K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x79 (12100K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x7A (12200K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x7B (12300K)
	{ 0x0000482A	, 0x00004A71 },    // cate value = 0x7C (12400K)
	{ 0x0000482A	, 0x00004A71 }     // cate value = 0x7D (12500K)
};

static TT_AlHscInstance g_AlHscInstance[E_SHARE_SENSOR_NUM_MAX];
static E_SHARE_SENSOR_ID g_cur_sensor_id;


#ifdef AWB_PREFERRED_CAST
PRIVATE TT_AlAwbPrefCastParam fttAwbPrefCastParam[2][8] = 
{			
		/* for Lower BV side */	
		{	/*  Border, WBGain R, WBGain G, WBGain B  */
			{ AL_FIX_VAL(4.0000), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  2500.0000K */
			{ AL_FIX_VAL(3.5714), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  2800.0000K */
			{ AL_FIX_VAL(3.1250), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  3200.0000K */
			{ AL_FIX_VAL(2.7778), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  3600.0000K */
			{ AL_FIX_VAL(2.5000), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  4000.0000K */
			{ AL_FIX_VAL(1.8182), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  5500.0000K */
			{ AL_FIX_VAL(1.5385), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  6500.0000K */
			{ AL_FIX_VAL(1.2500), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  8000.0000K */
		},	
		/* for Higher BV side */	
		{	/*  Border, WBGain R, WBGain G, WBGain B  */
			{ AL_FIX_VAL(4.0000), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  2500.0000K */
			{ AL_FIX_VAL(3.5714), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  2800.0000K */
			{ AL_FIX_VAL(3.1250), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  3200.0000K */
			{ AL_FIX_VAL(2.7778), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  3600.0000K */
			{ AL_FIX_VAL(2.5000), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  4000.0000K */
			{ AL_FIX_VAL(1.8182), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  5500.0000K */
			{ AL_FIX_VAL(1.5385), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  6500.0000K */
			{ AL_FIX_VAL(1.2500), { AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000), AL_FIX_VAL(1.0000) } },/*  8000.0000K */
		}	
};
#endif	//#ifdef AWB_PREFERRED_CAST

static BOOL g_ave_stat = FALSE;
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static INT32 AWB_Rcv_Msg( T_AWB_MSG* msg );
static void awb_make_one_blktbl(T_SHARE_AEAWB_STAT* const in, const T_AWB_PARAM *param);
static void awb_disable_blk_outof_circle( E_SHARE_SENSOR_ID sensor_id, USHORT blknum_h, USHORT blknum_v );
static void awb_init(void);
static void calc_awb_statistics(E_SHARE_SENSOR_ID sensor_id, UCHAR hbc, UCHAR vbc, USHORT hw, USHORT vw, T_AWB_PARAM *param);
static void calc_awb_main(E_SHARE_SENSOR_ID sensor_id, T_AWB_PARAM *param);
static void set_awb_result(E_SHARE_SENSOR_ID sensor_id, const USHORT *aeyk, T_AWB_PARAM *param);

static void AlAwbWrapStabiMapOutOfRange( E_SHARE_SENSOR_ID sensor_id, T_AWB_PARAM *param );
static void AlAwbWrapSuppressFallWithMapChanges( E_SHARE_SENSOR_ID sensor_id, T_AWB_PARAM *param );
static void AlAwbWrapPreMainProcess( E_SHARE_SENSOR_ID sensor_id, T_AWB_PARAM *param );
//static void AlAwbWrapPrefWBCast(E_SHARE_SENSOR_ID sensor_id, SQ_32* ppqWBGain, SQ_32* ppqCCMatrix);
static void AlAwbWrapUpdateAdjustCC( E_SHARE_SENSOR_ID sensor_id, T_AWB_PARAM *param );
static void AlAwbWrapUpdate(E_SHARE_SENSOR_ID sensor_id, SQ_32 psqBv, T_AWB_PARAM *param);

static void hsc_init(TT_AlHscInstance* fttAlHscInstance);
static void hsc_main(TT_AlHscInstance* fttAlHscInstance, const TT_AwbWorkMem* aptWorkMem, T_AWB_PARAM *param, UCHAR hbc, UCHAR vbc);
static void hsc_reset(TT_AlHscInstance* fttAlHscInstance, UCHAR mode);
static void calc_shd_blending(float ct, T_AWB_PARAM *param);

// called by alhscwrap.c
PUBLIC void	AlHscUserMixTest( 
				UI_32		puiDstAddr		,/*[I ] dst address*/
				UI_32		puiSrc1Addr		,/*[I ] Src1 address*/
				UI_32		puiSrc2Addr		,/*[I ] Src2 address*/
				UI_08		puiSrc1Ratio	);/*[I ] Src1 weight (0.8 fixed point)*/

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void AWB_Task( INT32 stacd )
{
	T_SHARE_AEAWB_STAT out = {0};
	T_AWB_MSG awb;
	OS_USER_ER ercd = 0;
	int i;

	awb_init();
	for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
		hsc_init(&g_AlHscInstance[i]);
		hsc_reset(&g_AlHscInstance[i], 0);
	}

	while (1) {
		ercd = AWB_Rcv_Msg( &awb );
		UPRINTF_ERR(ercd);

		if (awb.msg == E_AWB_MSG_STAT_RDY) {
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AWB, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "awb"));
			Get_Frame_Share_AEAWB_STAT( &out, awb.frame_no );
			for (i = 0; i < out.rect_num; i++) BF_L1l2cache_Flush_Addr(out.rect[i].addr_awb, out.rect[i].h_byte * out.rect[0].blk_num_V);
			
			if( awb.param.dol_en ){
				// use g_awbblk[E_SHARE_SENSOR_ID_0] for long frame
				// use g_awbblk[E_SHARE_SENSOR_ID_1] for short frame
				out.sensor_num = 2;
				out.sensor[E_SHARE_SENSOR_ID_1] = out.sensor[E_SHARE_SENSOR_ID_0];
				for( i = out.dol_rect_num; i < out.dol_rect_num*2; i ++ ){
					out.rect[i].sensor = E_SHARE_SENSOR_ID_1;
				}
				awb_make_one_blktbl(&out, &awb.param);
				out.sensor_num = 1;
			}else{
				awb_make_one_blktbl(&out, &awb.param);
			}
			g_print_on = FALSE;

			memset(&g_awb_result, 0, sizeof(g_awb_result));

			if (awb.param.stat_average) {
				g_ave_stat = TRUE;
				out.sensor_num = 5;
			} else {
				g_ave_stat = FALSE;
			}

			switch (out.sensor_num) {
			case 1:
				g_cur_sensor_id = out.rect[0].sensor;
				calc_awb_statistics(out.rect[0].sensor,
									out.sensor[out.rect[0].sensor].h_blk_num,
									out.sensor[out.rect[0].sensor].v_blk_num,
									out.rect[0].blk_size_H,
									out.rect[0].blk_size_V,
									&awb.param);
				calc_awb_main(out.rect[0].sensor, &awb.param);
				set_awb_result(out.rect[0].sensor, out.sensor[out.rect[0].sensor].aeyk, &awb.param);
				if (awb.param.shd_mode == 1) hsc_main(&g_AlHscInstance[g_cur_sensor_id],
													  g_attWorkMem[out.rect[0].sensor],
													  &awb.param, out.sensor[out.rect[0].sensor].h_blk_num,
													  out.sensor[out.rect[0].sensor].v_blk_num);
				break;
			case 2:
				switch (out.rect_num) {
				case 2:
					for (i = 0; i < 2; i++) {
						g_cur_sensor_id = out.rect[i].sensor;
						calc_awb_statistics(out.rect[i].sensor,
										out.sensor[out.rect[i].sensor].h_blk_num,
										out.sensor[out.rect[i].sensor].v_blk_num,
										out.rect[i].blk_size_H,
										out.rect[i].blk_size_V,
										&awb.param);
						calc_awb_main(out.rect[i].sensor, &awb.param);
						set_awb_result(out.rect[i].sensor, out.sensor[out.rect[i].sensor].aeyk, &awb.param);
						if (awb.param.shd_mode == 1) hsc_main(&g_AlHscInstance[g_cur_sensor_id],
															  g_attWorkMem[out.rect[i].sensor], &awb.param,
															  out.sensor[out.rect[i].sensor].h_blk_num,
															  out.sensor[out.rect[i].sensor].v_blk_num);
					}
					break;
				case 4:
					for (i = 0; i < 4; i+=2) {
						g_cur_sensor_id = out.rect[i].sensor;
						calc_awb_statistics(out.rect[i].sensor,
										out.sensor[out.rect[i].sensor].h_blk_num,
										out.sensor[out.rect[i].sensor].v_blk_num,
										out.rect[i].blk_size_H,
										out.rect[i].blk_size_V,
										&awb.param);
						calc_awb_main(out.rect[i].sensor, &awb.param);
						set_awb_result(out.rect[i].sensor, out.sensor[out.rect[i].sensor].aeyk, &awb.param);
						if (awb.param.shd_mode == 1) hsc_main(&g_AlHscInstance[g_cur_sensor_id],
															  g_attWorkMem[out.rect[i].sensor], &awb.param,
															  out.sensor[out.rect[i].sensor].h_blk_num,
															  out.sensor[out.rect[i].sensor].v_blk_num);
					}
					break;
				default:
					UPRINTF_ERR(-1);
					break;
				}
				break;
			case 4:
				switch (out.rect_num) {
				case 4:
					for (i = 0; i < 4; i++) {
						g_cur_sensor_id = out.rect[i].sensor;
						calc_awb_statistics(out.rect[i].sensor,
										out.sensor[out.rect[i].sensor].h_blk_num,
										out.sensor[out.rect[i].sensor].v_blk_num,
										out.rect[i].blk_size_H,
										out.rect[i].blk_size_V,
										&awb.param);
						calc_awb_main(out.rect[i].sensor, &awb.param);
						set_awb_result(out.rect[i].sensor, out.sensor[out.rect[i].sensor].aeyk, &awb.param);
						if (awb.param.shd_mode == 1) hsc_main(&g_AlHscInstance[g_cur_sensor_id],
															  g_attWorkMem[out.rect[i].sensor],
															  &awb.param, out.sensor[out.rect[i].sensor].h_blk_num,
															  out.sensor[out.rect[i].sensor].v_blk_num);
					}
					break;
				default:
					UPRINTF_ERR(-1);
					break;
				}
				break;
			case 5: // AVE stat is on, only calc sensor 0 and apply to all sensors
				g_cur_sensor_id = out.rect[0].sensor;
				calc_awb_statistics(out.rect[0].sensor,
									out.sensor[out.rect[0].sensor].h_blk_num,
									out.sensor[out.rect[0].sensor].v_blk_num,
									out.rect[0].blk_size_H,
									out.rect[0].blk_size_V,
									&awb.param);
				calc_awb_main(out.rect[0].sensor, &awb.param);
				set_awb_result(out.rect[0].sensor, out.sensor[0].aeyk, &awb.param);
				if (awb.param.shd_mode == 1) hsc_main(&g_AlHscInstance[g_cur_sensor_id],
													  g_attWorkMem[out.rect[0].sensor],
													  &awb.param, out.sensor[out.rect[0].sensor].h_blk_num,
													  out.sensor[out.rect[0].sensor].v_blk_num);
				break;
			default:
				UPRINTF_ERR(-1);
				break;
			}
			Set_Share_AWB_Param( &g_awb_result );
			Send_Share_System_Event( E_SHARE_SYSTEM_EVENT_AWB_END, awb.frame_no );
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AWB, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "awb"));
			Cate_Set_WB_Result();
		}
	}
}

INT32 Snd_Msg_To_AWB(const T_AWB_MSG* msg)
{
	OS_USER_ER	ercd = 0;
	T_AWB_MBOX	*mbox = NULL;

	ercd = OS_User_Get_Mpf( MPFID_AWB, (OS_USER_VP*)&mbox );
	UPRINTF_ERR(ercd);

	mbox->blf_addr = mbox;

	memcpy( mbox->msg, msg, sizeof(T_AWB_MSG) );

	ercd = OS_User_Snd_Mbx( MID_AWB, (T_OS_USER_MSG*)mbox );
	UPRINTF_ERR(ercd);

	return (ercd);
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static INT32 AWB_Rcv_Msg( T_AWB_MSG* msg )
{
	OS_USER_ER	ercd = 0;
	T_AWB_MBOX	*mbox = NULL;
	
	/* [mbox] */
	ercd = OS_User_Rcv_Mbx( MID_AWB, (T_OS_USER_MSG**)&mbox );
	if( ercd != D_OS_USER_E_OK ){
		UPRINTF_ERR(ercd);
		return ( ercd );
	}

	memcpy( msg, mbox->msg, sizeof(T_AWB_MSG) );

	ercd = OS_User_Rel_Mpf( MPFID_AWB, (OS_USER_VP)mbox->blf_addr );
	UPRINTF_ERR(ercd);
	return (ercd);
}

static void one_sensor_one_rect(T_SHARE_AEAWB_STAT* const in, UCHAR rect_idx)
{
	ULONG line_blk_awb_addr		= 0;
	ULONG line_blk_lawb_addr	= 0;
	ULONG *wd_0 = NULL;
	ULONG *wd_1 = NULL;
	int i, j;

	if (g_print_on) {
		printf("sensor[%d]\n", in->rect[rect_idx].sensor);
		printf("rect[%d] blksizV:%d \n", rect_idx, in->rect[rect_idx].blk_size_V);
		printf("rect[%d] blkVnum:%d \n", rect_idx, in->rect[rect_idx].blk_num_V);

		printf("rect%d blkHnum:%d \n", rect_idx, in->rect[rect_idx].blk_num_L_H_R);
		printf("rect%d blksizH:%d \n", rect_idx, in->rect[rect_idx].blk_size_H);
		printf("rect%d blksizL:%d \n", rect_idx, in->rect[rect_idx].blk_size_L);
		printf("rect%d blksizR:%d \n", rect_idx, in->rect[rect_idx].blk_size_R);
	}

	line_blk_awb_addr	= in->rect[rect_idx].addr_awb;
	line_blk_lawb_addr	= in->rect[rect_idx].addr_level_awb;

	for(i = 0; i < in->sensor[in->rect[rect_idx].sensor].v_blk_num; i++) {
		// assign line block addr for each rect
		wd_0 = (ULONG*)line_blk_awb_addr;
		wd_1 = (ULONG*)line_blk_lawb_addr;
		for(j = 0; j < in->sensor[in->rect[rect_idx].sensor].h_blk_num; j++) {
			memcpy(&g_awbblk[in->rect[rect_idx].sensor][i][j], wd_0, 16);
			wd_0 += 4;
			if (g_print_on) printf("gb:%lu r:%lu gr:%lu b:%lu|",
									g_awbblk[in->rect[rect_idx].sensor][i][j].cnt,
									g_awbblk[in->rect[rect_idx].sensor][i][j].r,
									g_awbblk[in->rect[rect_idx].sensor][i][j].g,
									g_awbblk[in->rect[rect_idx].sensor][i][j].b);

			if (in->rect[rect_idx].is_level_awb) {
				memcpy(&g_awblvl_blk[in->rect[rect_idx].sensor][i][j][0], wd_1, 128);
				wd_1 += 32;
			}
		}
		if (g_print_on) printf("\n");
		line_blk_awb_addr	+= in->rect[rect_idx].h_byte;
		line_blk_lawb_addr	+= in->rect[rect_idx].h_byte;
	}
}

static void one_sensor_two_rect(T_SHARE_AEAWB_STAT* const in, UCHAR rect_idx)
{
	UCHAR num_blk_including_next_div_L[2] = {0};
	ULONG line_blk_awb_addr[2] = {0};
	ULONG line_blk_lawb_addr[2] = {0};
	BOOL blk_h_not_eqal_r[2] = {0};
	ULONG *wd_0[2] = {0};
	ULONG *wd_1[2] = {0};
	int i, j, k, rect_cnt;

	if (g_print_on) {
		printf("sensor[%d]\n", in->rect[rect_idx].sensor);
		printf("rect[%d] blksizV:%d \n", rect_idx, in->rect[rect_idx].blk_size_V);
		printf("rect[%d] blkVnum:%d \n", rect_idx, in->rect[rect_idx].blk_num_V);

		printf("rect%d blkHnum:%d \n", rect_idx, in->rect[rect_idx].blk_num_L_H_R);
		printf("rect%d blksizH:%d \n", rect_idx, in->rect[rect_idx].blk_size_H);
		printf("rect%d blksizL:%d \n", rect_idx, in->rect[rect_idx].blk_size_L);
		printf("rect%d blksizR:%d \n", rect_idx, in->rect[rect_idx].blk_size_R);
		
		printf("rect%d blkHnum:%d \n", rect_idx+1, in->rect[rect_idx+1].blk_num_L_H_R);
		printf("rect%d blksizH:%d \n", rect_idx+1, in->rect[rect_idx+1].blk_size_H);
		printf("rect%d blksizL:%d \n", rect_idx+1, in->rect[rect_idx+1].blk_size_L);
		printf("rect%d blksizR:%d \n", rect_idx+1, in->rect[rect_idx+1].blk_size_R);
	}
	// judge remaining blk exist or not
	for(i = rect_idx; i < rect_idx + 2; i++) {
		if(in->rect[i].blk_size_H != in->rect[i].blk_size_R) {
			blk_h_not_eqal_r[i - rect_idx] = TRUE;
		} else {
			blk_h_not_eqal_r[i - rect_idx] = FALSE;
		}
		if(in->rect[i].blk_size_H != in->rect[i].blk_size_L) {
			// rect blk num not including blk_L
			num_blk_including_next_div_L[i - rect_idx] = in->rect[i].blk_num_L_H_R - 1;
		} else {
			// rect blk num including blk_R
			num_blk_including_next_div_L[i - rect_idx] = in->rect[i].blk_num_L_H_R;
		}
	}
	// assign line block addr
	for(rect_cnt = rect_idx; rect_cnt < rect_idx + 2; rect_cnt++) {
		line_blk_awb_addr[rect_cnt - rect_idx]	= in->rect[rect_cnt].addr_awb;
		line_blk_lawb_addr[rect_cnt - rect_idx]	= in->rect[rect_cnt].addr_level_awb;
	}

	for(i = 0; i < in->sensor[in->rect[rect_idx].sensor].v_blk_num; i++) {
		// assign line block addr for rect[1st/2nd]
		for(rect_cnt = 0; rect_cnt < 2; rect_cnt++) {
			wd_0[rect_cnt] = (ULONG*)line_blk_awb_addr[rect_cnt];
			wd_1[rect_cnt] = (ULONG*)line_blk_lawb_addr[rect_cnt];
		}
		for(j = 0; j < in->sensor[in->rect[rect_idx].sensor].h_blk_num; j++) {
			if(j < num_blk_including_next_div_L[0]) {
				memcpy(&g_awbblk[in->rect[rect_idx].sensor][i][j], wd_0[0], 16); // 16 = 4 component x 4 bytes
				wd_0[0] += 4; // gb/r/gr/b 4 component
				if (in->rect[rect_idx].is_level_awb) {
					memcpy(&g_awblvl_blk[in->rect[rect_idx].sensor][i][j][0], wd_1[0], 128); // 128 = 4 component x 4 bytes x 8 levels
					wd_1[0] += 32; // cnt/r/g/b 4 component * 8 levels
				}
				if(blk_h_not_eqal_r[0] && j == num_blk_including_next_div_L[0] - 1) {
					// the block of rect[1st].R + rect[2nd].L
					// the below is for rect[2nd].L
					g_awbblk[in->rect[rect_idx].sensor][i][j].cnt	+= *wd_0[1]++;
					g_awbblk[in->rect[rect_idx].sensor][i][j].r		+= *wd_0[1]++;
					g_awbblk[in->rect[rect_idx].sensor][i][j].g		+= *wd_0[1]++;
					g_awbblk[in->rect[rect_idx].sensor][i][j].b		+= *wd_0[1]++;
					if (in->rect[rect_idx].is_level_awb) {
						for(k = 0; k < AWB_MAX_LVL; k++) {
							g_awblvl_blk[in->rect[rect_idx].sensor][i][j][k].cnt	+= *wd_1[1]++;
							g_awblvl_blk[in->rect[rect_idx].sensor][i][j][k].r		+= *wd_1[1]++;
							g_awblvl_blk[in->rect[rect_idx].sensor][i][j][k].g		+= *wd_1[1]++;
							g_awblvl_blk[in->rect[rect_idx].sensor][i][j][k].b		+= *wd_1[1]++;
						}
					}
				}
			} else {
				// the rect[2nd].remaining block not including rect[2nd].L
				memcpy(&g_awbblk[in->rect[rect_idx].sensor][i][j], wd_0[1], 16); // 16 = 4 component x 4 bytes
				wd_0[1] += 4; // gb/r/gr/b 4 component
				if (in->rect[rect_idx].is_level_awb) {
					memcpy(&g_awblvl_blk[in->rect[rect_idx].sensor][i][j][0], wd_1[1], 128); // 128 = 4 component x 4 bytes x 8 levels
					wd_1[1] += 32; // cnt/r/g/b 4 component * 8 levels
				}
			}
			if (g_print_on) printf("gb:%lu r:%lu gr:%lu b:%lu|",
									g_awbblk[in->rect[rect_idx].sensor][i][j].cnt,
									g_awbblk[in->rect[rect_idx].sensor][i][j].r,
									g_awbblk[in->rect[rect_idx].sensor][i][j].g,
									g_awbblk[in->rect[rect_idx].sensor][i][j].b);
		}
		if (g_print_on) printf("\n");
		for(rect_cnt = rect_idx; rect_cnt < rect_idx + 2; rect_cnt++) {
			line_blk_awb_addr[rect_cnt - rect_idx]	+= in->rect[rect_cnt].h_byte;
			line_blk_lawb_addr[rect_cnt - rect_idx]	+= in->rect[rect_cnt].h_byte;
		}
	}
}

static void one_sensor_four_rect(T_SHARE_AEAWB_STAT* const in)
{
	UCHAR num_blk_including_next_div_L[4] = {0};
	ULONG line_blk_awb_addr[4] = {0};
	ULONG line_blk_lawb_addr[4] = {0};
	BOOL blk_h_not_eqal_r[4] = {0};
	UCHAR h_blk_num = 0;
	ULONG *wd_0[4] = {0};
	ULONG *wd_1[4] = {0};
	int i, j, k, rect_cnt;

	if (g_print_on) {
		printf("sensor[%d]\n", in->rect[0].sensor);
		printf("rect0/1/2/3 blksizV:%d \n", in->rect[0].blk_size_V);
		printf("rect0/1/2/3 blkVnum:%d \n", in->rect[0].blk_num_V);

		printf("rect0 blkHnum:%d \n", in->rect[0].blk_num_L_H_R);
		printf("rect0 blksizH:%d \n", in->rect[0].blk_size_H);
		printf("rect0 blksizL:%d \n", in->rect[0].blk_size_L);
		printf("rect0 blksizR:%d \n", in->rect[0].blk_size_R);
		
		printf("rect1 blkHnum:%d \n", in->rect[1].blk_num_L_H_R);
		printf("rect1 blksizH:%d \n", in->rect[1].blk_size_H);
		printf("rect1 blksizL:%d \n", in->rect[1].blk_size_L);
		printf("rect1 blksizR:%d \n", in->rect[1].blk_size_R);

		printf("rect2 blkHnum:%d \n", in->rect[2].blk_num_L_H_R);
		printf("rect2 blksizH:%d \n", in->rect[2].blk_size_H);
		printf("rect2 blksizL:%d \n", in->rect[2].blk_size_L);
		printf("rect2 blksizR:%d \n", in->rect[2].blk_size_R);

		printf("rect3 blkHnum:%d \n", in->rect[3].blk_num_L_H_R);
		printf("rect3 blksizH:%d \n", in->rect[3].blk_size_H);
		printf("rect3 blksizL:%d \n", in->rect[3].blk_size_L);
		printf("rect3 blksizR:%d \n", in->rect[3].blk_size_R);
	}

	for(i = 0; i < 4; i++) {
		if(in->rect[i].blk_size_H != in->rect[i].blk_size_R) {
			blk_h_not_eqal_r[i] = TRUE;
		} else {
			blk_h_not_eqal_r[i] = FALSE;
		}
		if(in->rect[i].blk_size_H != in->rect[i].blk_size_L) {
			h_blk_num += in->rect[i].blk_num_L_H_R - 1;
			num_blk_including_next_div_L[i] = h_blk_num;
		} else {
			h_blk_num += in->rect[i].blk_num_L_H_R;
			num_blk_including_next_div_L[i] = h_blk_num;
		}
	}
	
	for(rect_cnt = 0; rect_cnt < 4; rect_cnt++) {
		line_blk_awb_addr[rect_cnt]		= in->rect[rect_cnt].addr_awb;
		line_blk_lawb_addr[rect_cnt]	= in->rect[rect_cnt].addr_level_awb;
	}

	for(i = 0; i < in->sensor[in->rect[0].sensor].v_blk_num; i++) {
		// assign line block addr for each rect
		for(rect_cnt = 0; rect_cnt < 4; rect_cnt++) {
			wd_0[rect_cnt] = (ULONG*)line_blk_awb_addr[rect_cnt];
			wd_1[rect_cnt] = (ULONG*)line_blk_lawb_addr[rect_cnt];
		}
		for (j = 0; j < in->sensor[in->rect[0].sensor].h_blk_num; j++) {
			if (j < num_blk_including_next_div_L[0]) {
				memcpy(&g_awbblk[in->rect[0].sensor][i][j], wd_0[0], 16); // 16 = 4 component x 4 bytes
				wd_0[0] += 4; // gb/r/gr/b 4 component
				if (in->rect[0].is_level_awb) {
					memcpy(&g_awblvl_blk[in->rect[0].sensor][i][j][0], wd_1[0], 128); // 128 = 4 component x 4 bytes x 8 levels
					wd_1[0] += 32; // cnt/r/g/b 4 component * 8 levels
				}
				if(blk_h_not_eqal_r[0] && j == num_blk_including_next_div_L[0] - 1) {
					// the block of rect[0].R + rect[1].L
					// the below is for rect[1].L
					g_awbblk[in->rect[0].sensor][i][j].cnt	+= *wd_0[1]++;
					g_awbblk[in->rect[0].sensor][i][j].r	+= *wd_0[1]++;
					g_awbblk[in->rect[0].sensor][i][j].g	+= *wd_0[1]++;
					g_awbblk[in->rect[0].sensor][i][j].b	+= *wd_0[1]++;
					if (in->rect[0].is_level_awb) {
						for(k = 0; k < AWB_MAX_LVL; k++) {
							g_awblvl_blk[in->rect[0].sensor][i][j][k].cnt	+= *wd_1[1]++;
							g_awblvl_blk[in->rect[0].sensor][i][j][k].r		+= *wd_1[1]++;
							g_awblvl_blk[in->rect[0].sensor][i][j][k].g		+= *wd_1[1]++;
							g_awblvl_blk[in->rect[0].sensor][i][j][k].b		+= *wd_1[1]++;
						}
					}
				}
			} else if (j < num_blk_including_next_div_L[1]) {
				memcpy(&g_awbblk[in->rect[0].sensor][i][j], wd_0[1], 16); // 16 = 4 component x 4 bytes
				wd_0[1] += 4; // gb/r/gr/b 4 component
				if (in->rect[0].is_level_awb) {
					memcpy(&g_awblvl_blk[in->rect[0].sensor][i][j][0], wd_1[1], 128); // 128 = 4 component x 4 bytes x 8 levels
					wd_1[1] += 32; // cnt/r/g/b 4 component * 8 levels
				}
				if(blk_h_not_eqal_r[1] && j == num_blk_including_next_div_L[1] - 1) {
					// the block of rect[1].R + rect[2].L
					// the below is for rect[2].L
					g_awbblk[in->rect[0].sensor][i][j].cnt	+= *wd_0[2]++;
					g_awbblk[in->rect[0].sensor][i][j].r	+= *wd_0[2]++;
					g_awbblk[in->rect[0].sensor][i][j].g	+= *wd_0[2]++;
					g_awbblk[in->rect[0].sensor][i][j].b	+= *wd_0[2]++;
					if (in->rect[0].is_level_awb) {
						for(k = 0; k < AWB_MAX_LVL; k++) {
							g_awblvl_blk[in->rect[0].sensor][i][j][k].cnt	+= *wd_1[2]++;
							g_awblvl_blk[in->rect[0].sensor][i][j][k].r		+= *wd_1[2]++;
							g_awblvl_blk[in->rect[0].sensor][i][j][k].g		+= *wd_1[2]++;
							g_awblvl_blk[in->rect[0].sensor][i][j][k].b		+= *wd_1[2]++;
						}
					}
				}
			} else if (j < num_blk_including_next_div_L[2]) {
				memcpy(&g_awbblk[in->rect[0].sensor][i][j], wd_0[2], 16); // 16 = 4 component x 4 bytes
				wd_0[2] += 4; // gb/r/gr/b 4 component
				if (in->rect[0].is_level_awb) {
					memcpy(&g_awblvl_blk[in->rect[0].sensor][i][j][0], wd_1[2], 128); // 128 = 4 component x 4 bytes x 8 levels
					wd_1[2] += 32; // cnt/r/g/b 4 component * 8 levels
				}
				if(blk_h_not_eqal_r[2] && j == num_blk_including_next_div_L[2] - 1) {
					// the block of rect[2].R + rect[3].L
					// the below is for rect[3].L
					g_awbblk[in->rect[0].sensor][i][j].cnt	+= *wd_0[3]++;
					g_awbblk[in->rect[0].sensor][i][j].r	+= *wd_0[3]++;
					g_awbblk[in->rect[0].sensor][i][j].g	+= *wd_0[3]++;
					g_awbblk[in->rect[0].sensor][i][j].b	+= *wd_0[3]++;
					if (in->rect[0].is_level_awb) {
						for(k = 0; k < AWB_MAX_LVL; k++) {
							g_awblvl_blk[in->rect[0].sensor][i][j][k].cnt	+= *wd_1[3]++;
							g_awblvl_blk[in->rect[0].sensor][i][j][k].r		+= *wd_1[3]++;
							g_awblvl_blk[in->rect[0].sensor][i][j][k].g		+= *wd_1[3]++;
							g_awblvl_blk[in->rect[0].sensor][i][j][k].b		+= *wd_1[3]++;
						}
					}
				}
			} else {
				// the rect[3].remaining block not including rect[3].L
				memcpy(&g_awbblk[in->rect[0].sensor][i][j], wd_0[3], 16); // 16 = 4 component x 4 bytes
				wd_0[3] += 4; // gb/r/gr/b 4 component
				if (in->rect[0].is_level_awb) {
					memcpy(&g_awblvl_blk[in->rect[0].sensor][i][j][0], wd_1[3], 128); // 128 = 4 component x 4 bytes x 8 levels
					wd_1[3] += 32; // cnt/r/g/b 4 component * 8 levels
				}
			}
			if (g_print_on) printf("gb:%lu r:%lu gr:%lu b:%lu|",
									g_awbblk[in->rect[0].sensor][i][j].cnt,
									g_awbblk[in->rect[0].sensor][i][j].r,
									g_awbblk[in->rect[0].sensor][i][j].g,
									g_awbblk[in->rect[0].sensor][i][j].b);
		}
		if (g_print_on) printf("\n");
		for(rect_cnt = 0; rect_cnt < 4; rect_cnt++) {
			line_blk_awb_addr[rect_cnt]		+= in->rect[rect_cnt].h_byte;
			line_blk_lawb_addr[rect_cnt]	+= in->rect[rect_cnt].h_byte;
		}
	}
}

static void awb_make_one_blktbl(T_SHARE_AEAWB_STAT *const in, const T_AWB_PARAM *param)
{
	int i, j;
	int rect_idx;
	int lvl_idx;

	switch (in->rect_num) {
	case 1:
		one_sensor_one_rect(in, 0);
		break;
	case 2:
		switch(in->sensor_num) {
		case 1:
			one_sensor_two_rect(in, 0);
			break;
		case 2:
			one_sensor_one_rect(in, 0);
			one_sensor_one_rect(in, 1);
			break;
		default:
			UPRINTF_ERR(-1);
			break;
		}
		break;
	case 4:
		switch(in->sensor_num) {
		case 1:
			one_sensor_four_rect(in);
			break;
		case 2:
			one_sensor_two_rect(in, 0);
			one_sensor_two_rect(in, 2);
			break;
		case 4:
			one_sensor_one_rect(in, 0);
			one_sensor_one_rect(in, 1);
			one_sensor_one_rect(in, 2);
			one_sensor_one_rect(in, 3);
			break;
		default:
			UPRINTF_ERR(-1);
			break;
		}
		break;
	default:
		UPRINTF_ERR(-1);
		break;
	}

	if( param->unuse_outof_circle ){
		switch( in->sensor_num ){
		case 1:
			i = in->rect[0].sensor;
			awb_disable_blk_outof_circle( i, in->sensor[ i ].h_blk_num, in->sensor[ i ].v_blk_num );
			break;
		case 2:
			i = in->rect[0].sensor;
			awb_disable_blk_outof_circle( i, in->sensor[ i ].h_blk_num, in->sensor[ i ].v_blk_num );
			if( in->rect_num == 2 ){
				i = in->rect[1].sensor;
			}else{
				i = in->rect[2].sensor; // different
			}
			awb_disable_blk_outof_circle( i, in->sensor[ i ].h_blk_num, in->sensor[ i ].v_blk_num );
			break;
		case 4:
			i = in->rect[0].sensor;
			awb_disable_blk_outof_circle( i, in->sensor[ i ].h_blk_num, in->sensor[ i ].v_blk_num );
			i = in->rect[1].sensor;
			awb_disable_blk_outof_circle( i, in->sensor[ i ].h_blk_num, in->sensor[ i ].v_blk_num );
			i = in->rect[2].sensor;
			awb_disable_blk_outof_circle( i, in->sensor[ i ].h_blk_num, in->sensor[ i ].v_blk_num );
			i = in->rect[3].sensor;
			awb_disable_blk_outof_circle( i, in->sensor[ i ].h_blk_num, in->sensor[ i ].v_blk_num );
			break;
		default:
			break;
		}
	}
	
	if(param->stat_average) {
		memset(&g_awbblk_sum[0][0], 0, sizeof(g_awbblk_sum));
		memset(&g_awblvl_sum[0][0], 0, sizeof(g_awblvl_sum));

		switch (in->sensor_num) {
		case 2:
			switch(in->rect_num) {
   			case 2:
   				// calc stat sum block
   				for(rect_idx = 0; rect_idx < 2; rect_idx++) {
   					for(i = 0; i < in->sensor[in->rect[rect_idx].sensor].v_blk_num; i++) {
   						for(j = 0; j < in->sensor[in->rect[rect_idx].sensor].h_blk_num; j++) {
							g_awbblk_sum[i][j].r += g_awbblk[in->rect[rect_idx].sensor][i][j].r;
							g_awbblk_sum[i][j].g += g_awbblk[in->rect[rect_idx].sensor][i][j].g;
							g_awbblk_sum[i][j].b += g_awbblk[in->rect[rect_idx].sensor][i][j].b;
							g_awbblk_sum[i][j].cnt += g_awbblk[in->rect[rect_idx].sensor][i][j].cnt;
							for (lvl_idx = 0; lvl_idx < 8; lvl_idx++ ) {
								g_awblvl_sum[i][j][lvl_idx].r += g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].r;
								g_awblvl_sum[i][j][lvl_idx].g += g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].g;
								g_awblvl_sum[i][j][lvl_idx].b += g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].b;
								g_awblvl_sum[i][j][lvl_idx].cnt += g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].cnt;
							}
   						}
   					}
   				}
   				// set stat sum block back to be average for each sensor stat block
   				for(rect_idx = 0; rect_idx < 2; rect_idx++) {
   					for(i = 0; i < in->sensor[in->rect[rect_idx].sensor].v_blk_num; i++) {
   						for(j = 0; j < in->sensor[in->rect[rect_idx].sensor].h_blk_num; j++) {
   							g_awbblk[in->rect[rect_idx].sensor][i][j].r = g_awbblk_sum[i][j].r >> 1;
							g_awbblk[in->rect[rect_idx].sensor][i][j].g = g_awbblk_sum[i][j].g >> 1;
							g_awbblk[in->rect[rect_idx].sensor][i][j].b = g_awbblk_sum[i][j].b >> 1;
							g_awbblk[in->rect[rect_idx].sensor][i][j].cnt = g_awbblk_sum[i][j].cnt >> 1;
							for (lvl_idx = 0; lvl_idx < 8; lvl_idx++ ) {
								g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].r = g_awblvl_sum[i][j][lvl_idx].r >> 1;
								g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].g = g_awblvl_sum[i][j][lvl_idx].g >> 1;
								g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].b = g_awblvl_sum[i][j][lvl_idx].b >> 1 ;
								g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].cnt = g_awblvl_sum[i][j][lvl_idx].cnt >> 1;
							}
   						}
   					}
   				}
   				break;
   			case 4:
   				// calc stat sum block
   				for(rect_idx = 0; rect_idx < 4; rect_idx+=2) {
   					for(i = 0; i < in->sensor[in->rect[rect_idx].sensor].v_blk_num; i++) {
   						for(j = 0; j < in->sensor[in->rect[rect_idx].sensor].h_blk_num; j++) {
   							g_awbblk_sum[i][j].r += g_awbblk[in->rect[rect_idx].sensor][i][j].r;
							g_awbblk_sum[i][j].g += g_awbblk[in->rect[rect_idx].sensor][i][j].g;
							g_awbblk_sum[i][j].b += g_awbblk[in->rect[rect_idx].sensor][i][j].b;
							g_awbblk_sum[i][j].cnt += g_awbblk[in->rect[rect_idx].sensor][i][j].cnt;
							for (lvl_idx = 0; lvl_idx < 8; lvl_idx++ ) {
								g_awblvl_sum[i][j][lvl_idx].r += g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].r;
								g_awblvl_sum[i][j][lvl_idx].g += g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].g;
								g_awblvl_sum[i][j][lvl_idx].b += g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].b;
								g_awblvl_sum[i][j][lvl_idx].cnt += g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].cnt;
							}
   						}
   					}
   				}
   				// set stat sum block back to be average for each sensor stat block
   				for(rect_idx = 0; rect_idx < 4; rect_idx+=2) {
   					for(i = 0; i < in->sensor[in->rect[0].sensor].v_blk_num; i++) {
   						for(j = 0; j < in->sensor[in->rect[0].sensor].h_blk_num; j++) {
   							g_awbblk[in->rect[rect_idx].sensor][i][j].r = g_awbblk_sum[i][j].r >> 1;
							g_awbblk[in->rect[rect_idx].sensor][i][j].g = g_awbblk_sum[i][j].g >> 1;
							g_awbblk[in->rect[rect_idx].sensor][i][j].b = g_awbblk_sum[i][j].b >> 1;
							g_awbblk[in->rect[rect_idx].sensor][i][j].cnt = g_awbblk_sum[i][j].cnt >> 1;

							for (lvl_idx = 0; lvl_idx < 8; lvl_idx++ ) {
								g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].r = g_awblvl_sum[i][j][lvl_idx].r >> 1;
								g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].g = g_awblvl_sum[i][j][lvl_idx].g >> 1;
								g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].b = g_awblvl_sum[i][j][lvl_idx].b >> 1 ;
								g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].cnt = g_awblvl_sum[i][j][lvl_idx].cnt >> 1;
							}
   						}
   					}
   				}
   					break;
   			default:
   				break;
			}
		break;
		case 4:
			// calc stat sum block
			for(rect_idx = 0; rect_idx < 4; rect_idx++) {
				for(i = 0; i < in->sensor[in->rect[rect_idx].sensor].v_blk_num; i++) {
					for(j = 0; j < in->sensor[in->rect[rect_idx].sensor].h_blk_num; j++) {
						g_awbblk_sum[i][j].r += g_awbblk[in->rect[rect_idx].sensor][i][j].r;
   						g_awbblk_sum[i][j].g += g_awbblk[in->rect[rect_idx].sensor][i][j].g;
   						g_awbblk_sum[i][j].b += g_awbblk[in->rect[rect_idx].sensor][i][j].b;
   						g_awbblk_sum[i][j].cnt += g_awbblk[in->rect[rect_idx].sensor][i][j].cnt;

						for (lvl_idx = 0; lvl_idx < 8; lvl_idx++ ) {
							g_awblvl_sum[i][j][lvl_idx].r += g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].r;
							g_awblvl_sum[i][j][lvl_idx].g += g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].g;
							g_awblvl_sum[i][j][lvl_idx].b += g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].b;
							g_awblvl_sum[i][j][lvl_idx].cnt += g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].cnt;
						}
					}
				}
			}
			// set stat sum block back to be average for each sensor stat block
			for(rect_idx = 0; rect_idx < 4; rect_idx++) {
				for(i = 0; i < in->sensor[in->rect[rect_idx].sensor].v_blk_num; i++) {
					for(j = 0; j < in->sensor[in->rect[rect_idx].sensor].h_blk_num; j++) {
						g_awbblk[in->rect[rect_idx].sensor][i][j].r = g_awbblk_sum[i][j].r >> 2;
   						g_awbblk[in->rect[rect_idx].sensor][i][j].g = g_awbblk_sum[i][j].g >> 2;
   						g_awbblk[in->rect[rect_idx].sensor][i][j].b = g_awbblk_sum[i][j].b >> 2;
   						g_awbblk[in->rect[rect_idx].sensor][i][j].cnt = g_awbblk_sum[i][j].cnt >> 2;

						for (lvl_idx = 0; lvl_idx < 8; lvl_idx++ ) {
   							g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].r = g_awblvl_sum[i][j][lvl_idx].r >> 2;
   							g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].g = g_awblvl_sum[i][j][lvl_idx].g >> 2;
   							g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].b = g_awblvl_sum[i][j][lvl_idx].b >> 2 ;
   							g_awblvl_blk[in->rect[rect_idx].sensor][i][j][lvl_idx].cnt = g_awblvl_sum[i][j][lvl_idx].cnt >> 2;
   						}
					}
				}
			}
		break;
		default:
			break;
		}
	}
}

static void awb_disable_blk_outof_circle( E_SHARE_SENSOR_ID sensor_id, USHORT blknum_h, USHORT blknum_v )
{
	const UCHAR* circle;
	INT32 x, y;
	INT32 index;
	
	circle = Get_Share_AEAWB_Blk_Valid_For_Circle( sensor_id );
	
	if( circle == NULL ){
		UPRINTF_ERR(1);
		return;
	}
	
	for( y = 0; y < blknum_v; y ++ ){
		for( x = 0; x < blknum_h; x ++ ){
			index = y * blknum_h + x;
			
			if( circle[ index ] == 0 ){
				g_awbblk[sensor_id][y][x].cnt = 0;
				g_awbblk[sensor_id][y][x].r = 0;
				g_awbblk[sensor_id][y][x].g = 0;
				g_awbblk[sensor_id][y][x].b = 0;
			}
		}
	}
	
#if 0 // debug printf
	{
		static int debug_cnt = 0;
		
		if( debug_cnt < 4 ){
			debug_cnt ++;
		}else if( debug_cnt < 200 ){
			debug_cnt ++;
			return;// no print
		}else{
			debug_cnt = 0;
		}
	}
	printf( "==== Disable Blk Info ====\n" );
	printf( "sensor   = %d\n", sensor_id );
	printf( "blknum_h = %d\n", blknum_h );
	printf( "blknum_v = %d\n", blknum_v );
	for( y = 0; y < blknum_v; y ++ ){
		for( x = 0; x < blknum_h; x ++ ){
			if( g_awbblk[sensor_id][y][x].cnt == 0 ){
				printf( "x" );
			}else{
				printf( "O" );
			}
		}
		printf( "\n" );
	}
#endif
}
	
static void awb_init(void)
{
	SI_32 i, j;
	TT_AlVersion awb_version;
	UINT32 _awb_version;

	AlAwbGetVersion( &awb_version );
	_awb_version = (awb_version.muiMajor<<16)+(awb_version.muiMinor<<8) + (awb_version.muiLocal);
	printf("awb version:%x\n", _awb_version);

	fuiAwbStartupCounter = 0;
	memset(&fuiColorOffsetNo, 0, sizeof(fuiColorOffsetNo));

	for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
		g_attWorkMem[i] = &g_attWorkMemArray[i];
		AlAwbInitLib(g_attWorkMem[i]);
		fpSetup[i] = fptSetupModule[i];

		AlAwbSetColorSource( g_attWorkMem[i], fpSetup[i]->mptAlAwbColorSource_0, fpSetup[i]->muiCcsNum_0 );
		AlAwbSetExtendWhiteMapMode( g_attWorkMem[i], bAwbEnb );
		AlAwbSetWhiteMap( g_attWorkMem[i], (UI_WhiteMap *)&fpSetup[i]->muiWhiteMap_0[0] );
		AlAwbSetExtendMap( g_attWorkMem[i], (UI_WhiteMap *)&fpSetup[i]->muiWhiteMapEx_0[0] );
		AlAwbSetCTempTblEx( g_attWorkMem[i], (UI_CTempTblEx *)&fpSetup[i]->muiCTempTbl_0[0] );
		AlAwbSetRGB2XYZMatrix( g_attWorkMem[i], &fpSetup[i]->msqRGB2XYZMatrix->mttMtx[0] );
		AlAwbSetXCoordinateTbl( g_attWorkMem[i], (SQ_XCoordinateTbl *)&fpSetup[i]->msqXCoordinateTblIndoor->mttTbl[fuiXCTblModeIndoor[i]] );

		for (j=0; j<6; j++) {
			AlAwbSetColorOffset(
				g_attWorkMem[i],
				fpSetup[i]->msqColorOffsetTbl[0].mttTbl[j][0],
				fpSetup[i]->msqColorOffsetTbl[0].mttTbl[j][1],
				fpSetup[i]->msqColorOffsetTbl[0].mttTbl[j][2],
				fpSetup[i]->msqColorOffsetTbl[0].mttTbl[j][3] );
		}

		for (j=0; j<12; j++) {	// ColorOffsetParameter
			AlAwbSetExtendColorOffset(
				g_attWorkMem[i], 
				fpSetup[i]->msqExtendColorOffsetTbl[0].mttTbl[j][0],
				fpSetup[i]->msqExtendColorOffsetTbl[0].mttTbl[j][1],
				fpSetup[i]->msqExtendColorOffsetTbl[0].mttTbl[j][2],
				fpSetup[i]->msqExtendColorOffsetTbl[0].mttTbl[j][3] );
		}
#ifdef AWB_PREFERRED_CAST
		/* Preferred WB Cast Param */
		T_BF_3A_AWB_SIMPLE_TOOL awb_param_tbl;
		if (FJ_AWB_Tbl_Get_Sdram_Common( &awb_param_tbl ) == FJ_ERR_OK) {	//for simple tool
			if (awb_param_tbl.common_mng.is_loaded == TRUE){
				for( i=0; i<awb_param_tbl.common.body.Prefered_Cast.NUM_OF_BOARDER; i++ ){
					fttAwbPrefCastParam[0][i].msqColorBorder = awb_param_tbl.common.body.Prefered_Cast.BOARD[i].BOARDER;
					fttAwbPrefCastParam[0][i].msqGain[0] = awb_param_tbl.common.body.Prefered_Cast.BOARD[i].LO_BV_R_GAIN;
					fttAwbPrefCastParam[0][i].msqGain[2] = awb_param_tbl.common.body.Prefered_Cast.BOARD[i].LO_BV_B_GAIN;
					fttAwbPrefCastParam[1][i].msqColorBorder = awb_param_tbl.common.body.Prefered_Cast.BOARD[i].BOARDER;
					fttAwbPrefCastParam[1][i].msqGain[0] = awb_param_tbl.common.body.Prefered_Cast.BOARD[i].HI_BV_R_GAIN;
					fttAwbPrefCastParam[1][i].msqGain[2] = awb_param_tbl.common.body.Prefered_Cast.BOARD[i].HI_BV_B_GAIN;
				}
				AlAwbSetPreferredCastParam(g_attWorkMem[i],
											awb_param_tbl.common.body.Prefered_Cast.NUM_OF_BOARDER,
											&fttAwbPrefCastParam[0][0],
											&fttAwbPrefCastParam[1][0],
											awb_param_tbl.common.body.Prefered_Cast.BV_MIN,
											awb_param_tbl.common.body.Prefered_Cast.BV_MAX);//for simple tool
			}	
		}
#endif
		AlAwbSetBvThreshold(
			g_attWorkMem[i],
			fpSetup[i]->mptDefindPrm->al_awb_bv_th_reserved,
			fpSetup[i]->mptDefindPrm->al_awb_bv_th_interp,
			fpSetup[i]->mptDefindPrm->al_awb_bv_th_indoor,
			fpSetup[i]->mptDefindPrm->al_awb_bv_th_outdoor);

		/* Speed Bv (MapRangeêÿÇËë÷Ç¶é˚ë©ë¨ìx) */
		AlAwbSetBvLpfSpeed( g_attWorkMem[i], fpSetup[i]->mptDefindPrm->al_awb_bv_lpf_fsmp, fpSetup[i]->mptDefindPrm->al_awb_bv_lpf_fcut );
		AlAwbSetBvLpfMode( g_attWorkMem[i], bAwbEnb );

		/* Speed xy (AWBé˚ë©ë¨ìx) */
		AlAwbSetXyLpfSpeed( g_attWorkMem[i], fpSetup[i]->mptDefindPrm->al_awb_xy_lpf_fsmp, fpSetup[i]->mptDefindPrm->al_awb_xy_lpf_fcut );
		AlAwbSetXyLpfMode( g_attWorkMem[i], bAwbEnb );

		// äJénÉ}ÉbÉv??ÉW
		AlAwbSetMapRange( g_attWorkMem[i], fpSetup[i]->mptDefindPrm->al_awb_start_range );

		//Default xy
		AlAwbSetAutoMode( g_attWorkMem[i], bAwbDsb );
		AlAwbSetManualXy( g_attWorkMem[i], AL_AWB_DEFAULT_X, AL_AWB_DEFAULT_Y, bAwbDsb );
		AlAwbSetBvLpfMode( g_attWorkMem[i], bAwbEnb );
		AlAwbSetXyLpfMode( g_attWorkMem[i], bAwbEnb );
		AlAwbSetAutoMode( g_attWorkMem[i], bAwbEnb );
	}
}

#if 0
static void	AlAwbWrapReset( void )
{
	fuiAwbStartupCounter = 0;
}
#endif
static void AlAwbWrapSetBvThreshold(E_SHARE_SENSOR_ID sensor_id, UI_08 rang)
{
	// Setting MapRange
	switch(rang){
		case 1:
			AlAwbSetBvThreshold(
				g_attWorkMem[sensor_id],		/* MapRange Outdoor fix */
				fpSetup[sensor_id]->mptDefindPrm->al_awb_bv_th_reserved,
				fpSetup[sensor_id]->mptDefindPrm->al_awb_bv_th_interp,
				AL_FIX_VAL(-100.0)		,
				AL_FIX_VAL(-100.0)		);
			break;
		case 2:
			AlAwbSetBvThreshold(
				g_attWorkMem[sensor_id],		/* MapRange Indoor fix */
				fpSetup[sensor_id]->mptDefindPrm->al_awb_bv_th_reserved,
				fpSetup[sensor_id]->mptDefindPrm->al_awb_bv_th_interp,
				AL_FIX_VAL(100.0)		,
				AL_FIX_VAL(100.0)		);
			break;
		default:
			AlAwbSetBvThreshold(
			g_attWorkMem[sensor_id],
			fpSetup[sensor_id]->mptDefindPrm->al_awb_bv_th_reserved,
			fpSetup[sensor_id]->mptDefindPrm->al_awb_bv_th_interp,
			fpSetup[sensor_id]->mptDefindPrm->al_awb_bv_th_indoor,
			fpSetup[sensor_id]->mptDefindPrm->al_awb_bv_th_outdoor);
			break;
	}
}

static void AlAwbWrapStartupControl( E_SHARE_SENSOR_ID sensor_id )
{
#ifdef AWB_STARTUP_ONESHOT
	switch( fuiAwbStartupCounter ) {
	case AL_AWB_STARTUP_VD_NUM:
		AlAwbSetBvLpfMode( g_attWorkMem[sensor_id], bAwbEnb );
		AlAwbSetXyLpfMode( g_attWorkMem[sensor_id], bAwbEnb );
		break;
	case AL_AWB_STARTUP_VD_NUM+1:
		/*do nothing*/
		break;
	default:
		AlAwbSetBvLpfMode( g_attWorkMem[sensor_id], bAwbDsb );
		AlAwbSetXyLpfMode( g_attWorkMem[sensor_id], bAwbDsb );
		break;
	}
#endif	//#ifdef AWB_STARTUP_ONESHOT
}

static void AlAwbWrapUpdate(E_SHARE_SENSOR_ID sensor_id, SQ_32 psqBv, T_AWB_PARAM *param)
{
	UI_32 apiIndoor;		// [O] in the indoor map
	UI_32 apiOutdoor;		// [O] in the outdoor map
	UI_32 apiEx1; 			// [O] in the extension map
	UI_32 apiEx2;			// [O] in the extension map
	TE_AwbMapRange ateRange;
	SQ_32 asqDraftBv;
	SQ_32 asqX, asqY, asqCtemp;
	TE_AlAwbWrapCTempStatusIndoor ateCtempSts;
	SQ_32 Bv = AL_FIX_VAL(4.00);

	//Counter
	if(fuiAwbStartupCounter < 0xffffffff ) {//limit overflow
		fuiAwbStartupCounter++;
	}

	// Setting BV
	if (param->in_outdoor == E_AWB_INDOOR) {
		Bv = D_FORCED_INDOOR_BV;
	} else if (param->in_outdoor == E_AWB_OUTDOOR) {
		Bv = D_FORCED_OUTDOOR_BV;
	} else {
		Bv = psqBv;
	}
	AlAwbSetBv( g_attWorkMem[sensor_id], Bv );
	asqDraftBv = AlAwbGetBv(g_attWorkMem[sensor_id]);
#if 1
	if (param->in_outdoor == E_AWB_IN_OUT_AUTO) {
		ateRange = AlAwbGetMapRange(g_attWorkMem[sensor_id]);
	} else {
		ateRange = param->in_outdoor == E_AWB_INDOOR ? eAwbMapIndoor : eAwbMapOutdoor;
	}
#else
	ateRange = AlAwbGetMapRange(g_attWorkMem[sensor_id]);
#endif
	// Setting Color Temperature
	AlAwbGetDetectXy( g_attWorkMem[sensor_id], &asqX, &asqY) ;

	asqCtemp = AlAwbCalcCTemp( asqX, asqY );
	/*-----------------------------------**
		XCW Table
	**-----------------------------------*/
	if ( ateRange == eAwbMapOutdoor ) {
		if ( fpSetup[sensor_id]->mptDefindPrm->outdoor_xc_bv_lo2hi < asqDraftBv ) {
			fuiXCTblModeOutdoor[sensor_id] = OUTDOOR_XC_MODE_BV_HIGH;
		} else if ( asqDraftBv < fpSetup[sensor_id]->mptDefindPrm->outdoor_xc_bv_hi2lo ) {
			fuiXCTblModeOutdoor[sensor_id] = OUTDOOR_XC_MODE_BV_LOW;
		} else {
			// ????
		}

		AlAwbSetXCoordinateTbl(g_attWorkMem[sensor_id], 
							   (SQ_XCoordinateTbl *)&fpSetup[sensor_id]->msqXCoordinateTblOutdoor->mttTbl[fuiXCTblModeOutdoor[sensor_id]] );
	} else {	// Indoor

		if ( asqDraftBv > fpSetup[sensor_id]->mptDefindPrm->indoor_xc_bv_nor2hi ) {
			if ( asqCtemp >= fpSetup[sensor_id]->mptDefindPrm->indoor_xc_ctmp_norh2hi ) {
				ateCtempSts = eAlAwbWrapCtmpSts1BvH;
			} else if ( asqCtemp >= fpSetup[sensor_id]->mptDefindPrm->indoor_xc_ctmp_hi2norh ) {
				ateCtempSts = eAlAwbWrapCtmpSts2BvH;
			} else if ( asqCtemp >= fpSetup[sensor_id]->mptDefindPrm->indoor_xc_ctmp_norl2norh ) {
				ateCtempSts = eAlAwbWrapCtmpSts3BvH;
			} else if ( asqCtemp >= fpSetup[sensor_id]->mptDefindPrm->indoor_xc_ctmp_norh2norl ) {
				ateCtempSts = eAlAwbWrapCtmpSts4BvH;
			} else if ( asqCtemp >= fpSetup[sensor_id]->mptDefindPrm->indoor_xc_ctmp_lo2norl ) {
				ateCtempSts = eAlAwbWrapCtmpSts5BvH;
			} else if ( asqCtemp >= fpSetup[sensor_id]->mptDefindPrm->indoor_xc_ctmp_norl2lo ) {
				ateCtempSts = eAlAwbWrapCtmpSts6BvH;
			} else {
				ateCtempSts = eAlAwbWrapCtmpSts7BvH;
			}
		} else {
			if ( asqCtemp >= fpSetup[sensor_id]->mptDefindPrm->indoor_xc_ctmp_norh2hi ) {
				ateCtempSts = eAlAwbWrapCtmpSts1BvL;
			} else if ( asqCtemp >= fpSetup[sensor_id]->mptDefindPrm->indoor_xc_ctmp_hi2norh ) {
				ateCtempSts = eAlAwbWrapCtmpSts2BvL;
			} else if ( asqCtemp >= fpSetup[sensor_id]->mptDefindPrm->indoor_xc_ctmp_norl2norh ) {
				ateCtempSts = eAlAwbWrapCtmpSts3BvL;
			} else if ( asqCtemp >= fpSetup[sensor_id]->mptDefindPrm->indoor_xc_ctmp_norh2norl ) {
				ateCtempSts = eAlAwbWrapCtmpSts4BvL;
			} else if ( asqCtemp >= fpSetup[sensor_id]->mptDefindPrm->indoor_xc_ctmp_lo2norl ) {
				ateCtempSts = eAlAwbWrapCtmpSts5BvL;
			} else if ( asqCtemp >= fpSetup[sensor_id]->mptDefindPrm->indoor_xc_ctmp_norl2lo ) {
				ateCtempSts = eAlAwbWrapCtmpSts6BvL;
			} else {
				ateCtempSts = eAlAwbWrapCtmpSts7BvL;
			}
		}

		fuiXCTblModeIndoor[sensor_id] = (UI_08)fuiXCStatusTblIndoor[ateCtempSts][fuiXCTblModeIndoor[sensor_id]];

		AlAwbSetXCoordinateTbl(g_attWorkMem[sensor_id],
							   (SQ_XCoordinateTbl *)&fpSetup[sensor_id]->msqXCoordinateTblIndoor->mttTbl[fuiXCTblModeIndoor[sensor_id]] );
	}// Indoor

	/*-----------------------------------**
		ColorOffset
	**-----------------------------------*/
	{	//ColorOffsetSwitch
		UI_32 i;
		if ( ateRange == eAwbMapOutdoor ) {	// Outdoor
			//This code effects AWB lib Ver 4.4 or later
			fuiColorOffsetNo[sensor_id] = 3;	//forOutdoor SAS OutDoor
			for (i=0; i<6; i++) {
				AlAwbSetColorOffset(g_attWorkMem[sensor_id],
					fpSetup[sensor_id]->msqColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][0]	,
					fpSetup[sensor_id]->msqColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][1]	,
					fpSetup[sensor_id]->msqColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][2]	,
					fpSetup[sensor_id]->msqColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][3]	);
			}
			for (i=0; i<12; i++) {
				AlAwbSetExtendColorOffset(g_attWorkMem[sensor_id],
					fpSetup[sensor_id]->msqExtendColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][0],
					fpSetup[sensor_id]->msqExtendColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][1],
					fpSetup[sensor_id]->msqExtendColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][2],
					fpSetup[sensor_id]->msqExtendColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][3] );
			}
		}// Outdoor
		else {	// Indoor
			
			if( fuiXCTblModeIndoor[sensor_id] == 3 ) {
				fuiColorOffsetNo[sensor_id] = 1;
			} else {
				fuiColorOffsetNo[sensor_id] = 0;
			}
   			for (i=0; i<6; i++) {
   				AlAwbSetColorOffset(g_attWorkMem[sensor_id],
   					fpSetup[sensor_id]->msqColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][0],
   					fpSetup[sensor_id]->msqColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][1],
   					fpSetup[sensor_id]->msqColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][2],
   					fpSetup[sensor_id]->msqColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][3] );
   			}
   			for (i=0; i<12; i++) {
   				AlAwbSetExtendColorOffset(g_attWorkMem[sensor_id],
   					fpSetup[sensor_id]->msqExtendColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][0],
   					fpSetup[sensor_id]->msqExtendColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][1],
   					fpSetup[sensor_id]->msqExtendColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][2],
   					fpSetup[sensor_id]->msqExtendColorOffsetTbl[fuiColorOffsetNo[sensor_id]].mttTbl[i][3] );
   			}
		}
	} //ColorOffsetSwitch

	g_awb_white_block_count = AlAwbGetWhiteBlockCount(g_attWorkMem[sensor_id]);

	AlAwbGetBlockCount(g_attWorkMem[sensor_id], &apiIndoor, &apiOutdoor, &apiEx1, &apiEx2 );
	g_awb_block_count = apiIndoor + apiOutdoor + apiEx1 + apiEx2;
	g_awb_block_count_indoor = apiIndoor;
	g_awb_block_count_outdoor = apiOutdoor;
	g_awb_block_count_green_map = apiEx1;
	g_awb_block_count_skin_color_map = apiEx2;

	AlAwbGetDetectBlockCount( g_attWorkMem[sensor_id], &apiIndoor, &apiOutdoor, &apiEx1, &apiEx2 );
	g_awb_detect_block_count = apiIndoor + apiOutdoor + apiEx1 + apiEx2;
	g_awb_detect_block_count_indoor = apiIndoor;
	g_awb_detect_block_count_outdoor = apiOutdoor;
	g_awb_detect_block_count_green_map = apiEx1;
	g_awb_detect_block_count_skin_color_map = apiEx2;

	/*-----------------------------------**
		TrackingCTemp
	**-----------------------------------*/
	if ( ateRange == eAwbMapOutdoor )
	{	// Outdoor
		if( asqDraftBv >= fpSetup[sensor_id]->mptDefindPrm->awbtrk_brt_bv ) {
			AlAwbSetTrackingCTemp(
					g_attWorkMem[sensor_id],
					eAwbMapOutdoor,
					fpSetup[sensor_id]->mptDefindPrm->awbtrk_brt_lower_x, fpSetup[sensor_id]->mptDefindPrm->awbtrk_brt_lower_y, fpSetup[sensor_id]->mptDefindPrm->awbtrk_brt_lower_ctemp,
					fpSetup[sensor_id]->mptDefindPrm->awbtrk_brt_upper_x, fpSetup[sensor_id]->mptDefindPrm->awbtrk_brt_upper_y, fpSetup[sensor_id]->mptDefindPrm->awbtrk_brt_upper_ctemp );
		} else if( asqDraftBv >=  fpSetup[sensor_id]->mptDefindPrm->awbtrk_mid_bv ) {
			AlAwbSetTrackingCTemp(
					g_attWorkMem[sensor_id],
					eAwbMapOutdoor,
					fpSetup[sensor_id]->mptDefindPrm->awbtrk_mid_lower_x, fpSetup[sensor_id]->mptDefindPrm->awbtrk_mid_lower_y, fpSetup[sensor_id]->mptDefindPrm->awbtrk_mid_lower_ctemp,
					fpSetup[sensor_id]->mptDefindPrm->awbtrk_mid_upper_x, fpSetup[sensor_id]->mptDefindPrm->awbtrk_mid_upper_y, fpSetup[sensor_id]->mptDefindPrm->awbtrk_mid_upper_ctemp );
		} else {// Dark(Normal)
			AlAwbSetTrackingCTemp(
					g_attWorkMem[sensor_id],
					eAwbMapOutdoor,
					fpSetup[sensor_id]->mptDefindPrm->awbtrk_drk_lower_x, fpSetup[sensor_id]->mptDefindPrm->awbtrk_drk_lower_y, fpSetup[sensor_id]->mptDefindPrm->awbtrk_drk_lower_ctemp,
					fpSetup[sensor_id]->mptDefindPrm->awbtrk_drk_upper_x, fpSetup[sensor_id]->mptDefindPrm->awbtrk_drk_upper_y, fpSetup[sensor_id]->mptDefindPrm->awbtrk_drk_upper_ctemp );
		}
	} else { /* Indoor */
		AlAwbSetTrackingCTemp(
				g_attWorkMem[sensor_id],
				eAwbMapIndoor,
				fpSetup[sensor_id]->mptDefindPrm->awbtrk_idr_lower_x, fpSetup[sensor_id]->mptDefindPrm->awbtrk_idr_lower_y, fpSetup[sensor_id]->mptDefindPrm->awbtrk_idr_lower_ctemp,
				fpSetup[sensor_id]->mptDefindPrm->awbtrk_idr_upper_x, fpSetup[sensor_id]->mptDefindPrm->awbtrk_idr_upper_y, fpSetup[sensor_id]->mptDefindPrm->awbtrk_idr_upper_ctemp );
	}

#if 0
	/*-----------------------------------**
		Weight Switch
	**-----------------------------------*/
	if(asqDraftBv < AL_FIX_VAL(2.0) ) {
		AlAwbSetStatisticsBlockWeight(g_attWorkMem[sensor_id], &fpSetup[sensor_id]->mptDefindPrm->mpqWeightTable[0]);
	} else if(asqDraftBv < AL_FIX_VAL(4.0) ) {
		AlAwbSetStatisticsBlockWeight(g_attWorkMem[sensor_id], &fpSetup[sensor_id]->mptDefindPrm->mpqWeightTable[0]);
	} else {
		AlAwbSetStatisticsBlockWeight(g_attWorkMem[sensor_id], &fpSetup[sensor_id]->mptDefindPrm->mpqWeightTable[0]);
	}

	if( Cat_Get_Awb_Converge_Opt() == 1 )
	{
		AlAwbSetPreferredCastParam( fpSetup->pref_cast_count_1,
									(TT_AlAwbPrefCastParam *)fpSetup->pref_cast_param_Lo_1,
									(TT_AlAwbPrefCastParam *)fpSetup->pref_cast_param_Hi_1,
									fpSetup->pref_cast_Bv_Lo_1,
									fpSetup->pref_cast_Bv_Hi_1);
	} 
	else
	{
		AlAwbSetPreferredCastParam( fpSetup->pref_cast_count_0,
									(TT_AlAwbPrefCastParam *)fpSetup->pref_cast_param_Lo_0,
									(TT_AlAwbPrefCastParam *)fpSetup->pref_cast_param_Hi_0,
									fpSetup->pref_cast_Bv_Lo_0,
									fpSetup->pref_cast_Bv_Hi_0);
	}
#endif
}

static void	AlAwbWrapStabiMapOutOfRange( E_SHARE_SENSOR_ID sensor_id, T_AWB_PARAM *param )
{
#ifdef AWB_ALL_OUT_OF_RANGE_STABI
	TE_AwbMapRange ateRange;
	SQ_32 asqX, asqY;
	UI_32	auiIdrCnt, auiOdrCnt, auiEx1Cnt, auiEx2Cnt;
	UI_32	auiWhiteCnt;
#if 1
	if (param->in_outdoor == E_AWB_IN_OUT_AUTO)
		ateRange = AlAwbGetMapRange(g_attWorkMem[sensor_id]);
	else
		ateRange = param->in_outdoor == E_AWB_INDOOR ? eAwbMapIndoor : eAwbMapOutdoor;
#else
	ateRange = AlAwbGetMapRange(g_attWorkMem[sensor_id]);
#endif
	// Setting Color Temperature
	AlAwbGetDetectXy( g_attWorkMem[sensor_id], &asqX, &asqY) ;

	if( (fsqPrevDetectX[sensor_id] == INVALID_DETECT_XY) && (fsqPrevDetectY[sensor_id] == INVALID_DETECT_XY) ) {
		fsqPrevDetectX[sensor_id] = asqX;
		fsqPrevDetectY[sensor_id] = asqY;
	} else {
		AlAwbGetDetectBlockCount( g_attWorkMem[sensor_id], &auiIdrCnt, &auiOdrCnt, &auiEx1Cnt, &auiEx2Cnt );
		auiWhiteCnt = (ateRange == eAwbMapOutdoor) ? auiOdrCnt : auiIdrCnt;
		if( auiWhiteCnt <= AWB_UNSTABLE_WHITECOUNT ) {
			asqX = fsqPrevDetectX[sensor_id];
			asqY = fsqPrevDetectY[sensor_id];
			AlAwbSetDetectXy( g_attWorkMem[sensor_id], asqX, asqY, bAwbDsb );
		} else {
			fsqPrevDetectX[sensor_id] = asqX;
			fsqPrevDetectY[sensor_id] = asqY;
		}
	}
#endif	//#ifdef AWB_ALL_OUT_OF_RANGE_STABI
}

static void AlAwbWrapSuppressFallWithMapChanges( E_SHARE_SENSOR_ID sensor_id, T_AWB_PARAM *param )
{
#ifdef AWB_SUPPRESS_FAIL_WITH_MAP_CHANGES
	SQ_32	detect_x, detect_y;
	
	if( AlAwbGetMapRange(g_attWorkMem[sensor_id]) != fteMapRange[sensor_id] ){
		fteMapRange[sensor_id] = AlAwbGetMapRange(g_attWorkMem[sensor_id]);
		fuiDefaultModeEnable[sensor_id] = 1;
	}

	AlAwbGetDetectXy( g_attWorkMem[sensor_id], &detect_x, &detect_y );
	if( (fuiDefaultModeEnable[sensor_id]==1)&&(detect_x != 0)&&(detect_y != 0) ){
		fuiDefaultModeEnable[sensor_id] = 0;
	}

	if( fuiDefaultModeEnable[sensor_id] == 1 ) {
		AlAwbSetDetectXy( g_attWorkMem[sensor_id], AL_AWB_FAIL_X, AL_AWB_FAIL_Y, bAwbDsb );
	} else {
		AlAwbWrapStabiMapOutOfRange(sensor_id);
	}
#else
	AlAwbWrapStabiMapOutOfRange(sensor_id, param);
#endif	//#ifdef AWB_SUPPRESS_FAIL_WITH_MAP_CHANGES
}

static void AlAwbWrapUpdateAdjustCC( E_SHARE_SENSOR_ID sensor_id, T_AWB_PARAM *param )
{
	SQ_32	asqBv;
	SQ_32	asqAdjCcRatio;
	UI_32	auiLp;

	asqBv = AlAwbGetBv(g_attWorkMem[sensor_id]);

	if( fpSetup[sensor_id]->mptDefindPrm->mttChromaGainAdjCc.msqMaxBv > asqBv ){
		/* Low Brightness */
		if( asqBv < fpSetup[sensor_id]->mptDefindPrm->mttChromaGainAdjCc.msqMinBv ){
			asqBv = fpSetup[sensor_id]->mptDefindPrm->mttChromaGainAdjCc.msqMinBv;
		}
		asqAdjCcRatio = AlFixDiv( asqBv-fpSetup[sensor_id]->mptDefindPrm->mttChromaGainAdjCc.msqMinBv, fpSetup[sensor_id]->mptDefindPrm->mttChromaGainAdjCc.msqMaxBv - fpSetup[sensor_id]->mptDefindPrm->mttChromaGainAdjCc.msqMinBv );
		for( auiLp=0; auiLp<9; auiLp++ ){
			fsqCurrentChromaGainAdjCC[ sensor_id][auiLp] = 
				AlFixMul( fpSetup[sensor_id]->mptDefindPrm->mttChromaGainAdjCc.msqLower[auiLp], AL_FIX_VAL(1.0)-asqAdjCcRatio )
			  + AlFixMul( fpSetup[sensor_id]->mptDefindPrm->mttChromaGainAdjCc.msqUpper[auiLp], asqAdjCcRatio );
		}
	} else {
		/* High Brightness */
		if( asqBv > fpSetup[sensor_id]->mptDefindPrm->mttChromaGainAdjCc.msqMaxBv ){
			asqBv = fpSetup[sensor_id]->mptDefindPrm->mttChromaGainAdjCc.msqMaxBv;
		}
		asqAdjCcRatio = AlFixDiv( asqBv-fpSetup[sensor_id]->mptDefindPrm->mttChromaGainAdjCc.msqMinBv, fpSetup[sensor_id]->mptDefindPrm->mttChromaGainAdjCc.msqMaxBv - fpSetup[sensor_id]->mptDefindPrm->mttChromaGainAdjCc.msqMinBv );
		for( auiLp=0; auiLp<9; auiLp++ ){
			fsqCurrentChromaGainAdjCC[ sensor_id][auiLp] = 
				AlFixMul( fpSetup[sensor_id]->mptDefindPrm->mttChromaGainAdjCc.msqUpper[auiLp], AL_FIX_VAL(1.0)-asqAdjCcRatio )
			  + AlFixMul( fpSetup[sensor_id]->mptDefindPrm->mttChromaGainAdjCc.msqUpper[auiLp], asqAdjCcRatio );
		}
	}

#ifdef AWB_STARTUP_CHROMA_SUPRESS
	{
		SQ_32 asqCnt	= AL_FIX_VAL(fuiAwbStartupCounter) - AL_FIX_VAL(AL_AWB_ADJ_CC_STARTUP_CNT);
		SQ_32 asqRange	= AL_FIX_VAL(AL_AWB_ADJ_CC_NORMAL_CNT) - AL_FIX_VAL(AL_AWB_ADJ_CC_STARTUP_CNT);
		
		asqAdjCcRatio	= AlFixDiv( asqCnt , asqRange );

		if( asqAdjCcRatio < AL_FIX_VAL(0.0)) {
			asqAdjCcRatio = AL_FIX_VAL(0.0);
		}
		if( asqAdjCcRatio < AL_FIX_VAL(1.0)) {	//ratio=1.0
			for( auiLp=0; auiLp<9; auiLp++ ){
				fsqCurrentChromaGainAdjCC[sensor_id][auiLp] = 
					AlFixMul( fsqInitChromaGainAdjCC[sensor_id][auiLp], AL_FIX_VAL(1.0)-asqAdjCcRatio )
				  + AlFixMul( fsqCurrentChromaGainAdjCC[sensor_id][auiLp], asqAdjCcRatio );
			}
		}
	}
#endif	//#ifdef AWB_STARTUP_CHROMA_SUPRESS
	AlAwbSetAdjColorMatrix(g_attWorkMem[sensor_id], fsqCurrentChromaGainAdjCC[sensor_id]);
}

static void AlAwbWrapPreMainProcess( E_SHARE_SENSOR_ID sensor_id, T_AWB_PARAM *param )
{
	AlAwbWrapSuppressFallWithMapChanges(sensor_id, param);
	AlAwbWrapUpdateAdjustCC(sensor_id, param);
}

//static void AlAwbWrapPrefWBCast(E_SHARE_SENSOR_ID sensor_id, SQ_32* ppqWBGain, SQ_32* ppqCCMatrix)
//{
//	AlAwbCorrectPreferredCast( g_attWorkMem[sensor_id], fsqPrevDetectX[sensor_id], fsqPrevDetectY[sensor_id], ppqWBGain );
//}

static void calc_awb_main(E_SHARE_SENSOR_ID sensor_id, T_AWB_PARAM *param)
{
	SQ_32 x, y;
	//UCHAR manual_scene_k;

	switch (param->mode) {
	case E_AWB_MANUAL:// manual wb
		AlAwbSetAutoMode(g_attWorkMem[sensor_id], bAwbDsb);
		if (MANUAL_SCENE_MAX > param->manual_scene) {            // manual_scene 0~7
			x = g_awb_manual_scene[param->manual_scene][0];
			y = g_awb_manual_scene[param->manual_scene][1];
			//manual_scene_k = ((g_awb_manual_scene[param->manual_scene][2]) >> 16) / 100; // 100K unit
			//if( manual_scene_k < CATE_MIM_COLOR_TEMP){ // min clip
			//	manual_scene_k = CATE_MIM_COLOR_TEMP;
			//}
			AlAwbSetManualXy( g_attWorkMem[sensor_id], x, y, bAwbEnb );
		} else if( E_AWB_CUSTOM_CT == param->manual_scene) {	// manual_scene 8
			x = g_awb_manual_k[param->manual_k - CATE_MIM_COLOR_TEMP][0];
			y = g_awb_manual_k[param->manual_k - CATE_MIM_COLOR_TEMP][1];
			AlAwbSetManualXy( g_attWorkMem[sensor_id], x, y, bAwbEnb );
		} else if( E_AWB_CUSTOM_XY == param->manual_scene) {
			x = param->manual_wb_x;
			y = param->manual_wb_y;
			AlAwbSetManualXy( g_attWorkMem[sensor_id], x, y, bAwbEnb );
		} else if( E_AWB_CUSTOM_GAIN_CC == param->manual_scene) {
		}
		break;
	default:
	case E_AWB_AUTO://auto
		AlAwbSetAutoMode(g_attWorkMem[sensor_id], bAwbEnb);
		break;
	}
	if(param->oneshot) {
 		AlAwbSetXyLpfMode( g_attWorkMem[sensor_id], bAwbDsb ); 								// Set One-shot AWB mode
		AlAwbWrapUpdateAdjustCC(sensor_id, param);
		AlAwbMain( g_attWorkMem[sensor_id], &afxWBGain[sensor_id][0], &afxCCMtrx[sensor_id][0] );
 		//AlAwbWrapPrefWBCast(sensor_id, afxWBGain[sensor_id], &afxCCMtrx[sensor_id][0]);					/*### 2016/05/16 ALC takagi ### */
 		AlAwbSetXyLpfMode( g_attWorkMem[sensor_id], bAwbEnb ); 								// Return for preview AWB mode
	}else{
		AlAwbWrapPreMainProcess(sensor_id, param);
		AlAwbMain( g_attWorkMem[sensor_id], &afxWBGain[sensor_id][0], &afxCCMtrx[sensor_id][0] );
 		//AlAwbWrapPrefWBCast(sensor_id, &afxWBGain[sensor_id][0], &afxCCMtrx[sensor_id][0]);					/*### 2016/05/16 ALC takagi ### */
	}
}

static void calc_awb_statistics(E_SHARE_SENSOR_ID sensor_id, UCHAR hbc, UCHAR vbc, USHORT hw, USHORT vw, T_AWB_PARAM *param)
{
	UCHAR vcnt, hcnt;
	UCHAR vcnt2, hcnt2;
	UCHAR rs_hbc, rs_vbc;
	UCHAR hadd, vadd;
	UINT32 max;

	// AWBé˚ë©ë¨ìxê›íË
	AlAwbSetXyLpfMode(g_attWorkMem[sensor_id], bAwbEnb);
	AlAwbSetXyLpfSpeed(g_attWorkMem[sensor_id], AL_FIX_VAL(0x69-param->speed),0x00020000);
	AlAwbSetBvLpfMode(g_attWorkMem[sensor_id], bAwbEnb);
	AlAwbSetBvLpfSpeed(g_attWorkMem[sensor_id], AL_FIX_VAL(0x69-param->speed),0x0002199a);

	// Setting MapRange
	AlAwbWrapSetBvThreshold(sensor_id, 0);
	AlAwbWrapStartupControl(sensor_id);
	AlAwbWrapUpdate( sensor_id, param->sensor[sensor_id].bv, param );

	max = 4095; // 12bits

#if 1
	// 16x16Ç…èkè¨ÇµÇƒLibÇ÷
	// AWB Lib???ä‘íZèkÇÃÇΩÇﬂ?îgílèkè¨
	if (vbc % 16 != 0 || hbc % 16 != 0) {
		UPRINTF_ERR(-1);
	}
	rs_hbc = AWB_RESIZE_BLOCK_H;
	rs_vbc = AWB_RESIZE_BLOCK_V;
	hadd = hbc / rs_hbc;
	vadd = vbc / rs_vbc;
	for(vcnt=0; vcnt<rs_vbc; vcnt++){
		for(hcnt=0; hcnt<rs_hbc; hcnt++){
			// ?ä˙âª
			g_awbcntblk[sensor_id][vcnt][hcnt]		= 0;
			g_awbdatablk[sensor_id][vcnt][hcnt].r	= 0;
			g_awbdatablk[sensor_id][vcnt][hcnt].gr	= 0;
			g_awbdatablk[sensor_id][vcnt][hcnt].gb	= 0;
			g_awbdatablk[sensor_id][vcnt][hcnt].b 	= 0;
			// ?ïΩ?íºÇâ¡éZ
			for(vcnt2=vcnt*vadd; vcnt2<vcnt*vadd+vadd; vcnt2++){
				for(hcnt2=hcnt*hadd; hcnt2<hcnt*hadd+hadd; hcnt2++){
					g_awbcntblk[sensor_id][vcnt][hcnt]		+= g_awbblk[sensor_id][vcnt2][hcnt2].cnt;
					g_awbdatablk[sensor_id][vcnt][hcnt].r	+= g_awbblk[sensor_id][vcnt2][hcnt2].r;
					g_awbdatablk[sensor_id][vcnt][hcnt].gr	+= g_awbblk[sensor_id][vcnt2][hcnt2].g;
					g_awbdatablk[sensor_id][vcnt][hcnt].gb	+= g_awbblk[sensor_id][vcnt2][hcnt2].g;
					g_awbdatablk[sensor_id][vcnt][hcnt].b	+= g_awbblk[sensor_id][vcnt2][hcnt2].b;
				}
			}
		}
	}

	AlAwbSetStatisticsBlockInfo(
		g_attWorkMem[sensor_id],
		(hw * vw) * (hadd * vadd) >> 2,
		(UI_32)max,
		rs_hbc,
		rs_vbc);

	AlAwbResetStatisticsData(g_attWorkMem[sensor_id]);

	for(vcnt = 0; vcnt < rs_vbc; vcnt++){
		AlAwbSetBlockLineStatistics(g_attWorkMem[sensor_id],
									(VOID *)(&g_awbdatablk[sensor_id][vcnt][0]),
									(VOID *)(&g_awbcntblk[sensor_id][vcnt][0]),
									(VOID *)fttAwbWdWork[sensor_id]);
	}
#else
	AlAwbSetStatisticsBlockInfo(
		g_attWorkMem[sensor_id],
		(hw * vw) >> 2,
		(UI_32)max,
		hbc,
		vbc);

	AlAwbResetStatisticsData(g_attWorkMem[sensor_id]);

	for(vcnt = 0; vcnt < vbc; vcnt++){
		for(hcnt = 0; hcnt < hbc; hcnt++){
			g_awbcntblk[sensor_id][vcnt][hcnt]		= g_awbblk[sensor_id][vcnt][hcnt].cnt;
			g_awbdatablk[sensor_id][vcnt][hcnt].r 	= g_awbblk[sensor_id][vcnt][hcnt].r;
			g_awbdatablk[sensor_id][vcnt][hcnt].gr	= g_awbblk[sensor_id][vcnt][hcnt].g;
			g_awbdatablk[sensor_id][vcnt][hcnt].gb	= g_awbblk[sensor_id][vcnt][hcnt].g;
			g_awbdatablk[sensor_id][vcnt][hcnt].b 	= g_awbblk[sensor_id][vcnt][hcnt].b;
			
		}
	}
	//MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_03, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "BlockLine"));
	for(vcnt = 0; vcnt < vbc; vcnt++){
		AlAwbSetBlockLineStatistics(g_attWorkMem[sensor_id],
									(VOID *)(&g_awbdatablk[sensor_id][vcnt][0]),
									(VOID *)(&g_awbcntblk[sensor_id][vcnt][0]),
									(VOID *)fttAwbWdWork[sensor_id]);
	}
	//MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_03, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "BlockLine"));
#endif
	AlAwbCopyStatisticsData(g_attWorkMem[sensor_id]);
}

static void set_awb_result(E_SHARE_SENSOR_ID sensor_id, const USHORT *aeyk, T_AWB_PARAM *param)
{
	SQ_32	current_x, current_y, detect_x, detect_y;
	//TE_AwbMapRange range;
	INT32	cnt;
	USHORT	*p_CC, CC[9], WBGA[3];
	SHORT	cc_chk[9];
	SI_32	CCGain_bias[3] = { AWB_NO_BIAS, AWB_NO_BIAS, AWB_NO_BIAS};

	// CC matrix
	p_CC = CC;
	for( cnt=0; cnt<9; cnt++ ){
		*(p_CC + cnt) = (USHORT)(afxCCMtrx[sensor_id][cnt]>>7) & 0x07ff;
		*(p_CC + cnt) |= ((afxCCMtrx[sensor_id][cnt]>>20) & 0x0800);
		
		cc_chk[cnt] = (SHORT)*(p_CC + cnt);
		if( cc_chk[cnt] & 0x0800 ){
			cc_chk[cnt] = cc_chk[cnt] | 0xF000;
		}
	}
	// CC check
	cc_chk[0] = 0x0200 - cc_chk[1] - cc_chk[2];
	cc_chk[4] = 0x0200 - cc_chk[3] - cc_chk[5];
	cc_chk[8] = 0x0200 - cc_chk[6] - cc_chk[7];
	CC[0] = (cc_chk[0] & 0x07FF) | ((cc_chk[0]>>4) & 0x0800);
	CC[4] = (cc_chk[4] & 0x07FF) | ((cc_chk[4]>>4) & 0x0800);
	CC[8] = (cc_chk[8] & 0x07FF) | ((cc_chk[8]>>4) & 0x0800);
	// WB Gain
	CCGain_bias[AWB_BIAS_R] = AWB_NO_BIAS;
	CCGain_bias[AWB_BIAS_G] = AWB_NO_BIAS;
	CCGain_bias[AWB_BIAS_B] = AWB_NO_BIAS;
	WBGA[0] = (USHORT)( (AlFixMul(afxWBGain[sensor_id][0], CCGain_bias[AWB_BIAS_R]))>>8 );
	WBGA[1] = (USHORT)( (AlFixMul(afxWBGain[sensor_id][1], CCGain_bias[AWB_BIAS_G]))>>8 );
	WBGA[2] = (USHORT)( (AlFixMul(afxWBGain[sensor_id][2], CCGain_bias[AWB_BIAS_B]))>>8 );
	// Result Set
	for(cnt=0;cnt<3;cnt++){
		g_awb_result.sensor[sensor_id].wb_gain[cnt] = WBGA[cnt];
	}
	for(cnt=0;cnt<9;cnt++){
		g_awb_result.sensor[sensor_id].cc_matrix[cnt] = CC[cnt];
	}
	g_awb_result.sensor[sensor_id].aeyk[E_AEYK_R]  = ( ((UINT32)aeyk[E_AEYK_R] * (UINT32)WBGA[0]) >> 8) & 0xFF;
	g_awb_result.sensor[sensor_id].aeyk[E_AEYK_GR] = ( ((UINT32)aeyk[E_AEYK_GR] * (UINT32)WBGA[1] ) >> 8 ) & 0xFF;
	g_awb_result.sensor[sensor_id].aeyk[E_AEYK_GB] = ( ((UINT32)aeyk[E_AEYK_GB] * (UINT32)WBGA[1] ) >> 8 ) & 0xFF;
	g_awb_result.sensor[sensor_id].aeyk[E_AEYK_B]  = ( ((UINT32)aeyk[E_AEYK_B] * (UINT32)WBGA[2] ) >> 8 ) & 0xFF;

	AlAwbGetCurrentXy( g_attWorkMem[sensor_id], &current_x, &current_y);
	g_awb_result.sensor[sensor_id].currentX = current_x;
	g_awb_result.sensor[sensor_id].currentY = current_y;
	AlAwbGetDetectXy( g_attWorkMem[sensor_id], &detect_x, &detect_y);
	g_awb_result.sensor[sensor_id].detectX = detect_x;
	g_awb_result.sensor[sensor_id].detectY = detect_y;
	g_awb_result.sensor[sensor_id].detectCTemp	= AlAwbCalcCTemp(detect_x, detect_y);
	g_awb_result.sensor[sensor_id].currentCTemp	= AlAwbCalcCTemp(current_x, current_y);

	if (param->shd_mode == 2) { // not using HSC but simple method
		calc_shd_blending(RN_FIX_VAL(g_awb_result.sensor[sensor_id].currentCTemp), param);
	}

	if (param->mode == E_AWB_MANUAL && param->manual_scene == E_AWB_CUSTOM_GAIN_CC) {
		g_awb_result.sensor[sensor_id].wb_gain[E_WB_R] = param->Rgain;
		g_awb_result.sensor[sensor_id].wb_gain[E_WB_G] = param->Grgain;
		g_awb_result.sensor[sensor_id].wb_gain[E_WB_B] = param->Bgain;

		g_awb_result.sensor[sensor_id].cc_matrix[E_CC_00] = param->CC00;
		g_awb_result.sensor[sensor_id].cc_matrix[E_CC_01] = param->CC01;
		g_awb_result.sensor[sensor_id].cc_matrix[E_CC_02] = param->CC02;
		g_awb_result.sensor[sensor_id].cc_matrix[E_CC_10] = param->CC10;
		g_awb_result.sensor[sensor_id].cc_matrix[E_CC_11] = param->CC11;
		g_awb_result.sensor[sensor_id].cc_matrix[E_CC_12] = param->CC12;
		g_awb_result.sensor[sensor_id].cc_matrix[E_CC_20] = param->CC20;
		g_awb_result.sensor[sensor_id].cc_matrix[E_CC_21] = param->CC21;
		g_awb_result.sensor[sensor_id].cc_matrix[E_CC_22] = param->CC22;
	}
	if (g_ave_stat) {
		g_awb_result.sensor[3] = g_awb_result.sensor[2] = g_awb_result.sensor[1] = g_awb_result.sensor[0];
	}
	//range = AlAwbGetMapRange(g_attWorkMem[sensor_id]);
}

static void hsc_init(TT_AlHscInstance* fttAlHscInstance)
{
	AlHscWrap_Init(fttAlHscInstance);
}

static void hsc_main(TT_AlHscInstance* fttAlHscInstance, const TT_AwbWorkMem* aptWorkMem, T_AWB_PARAM *param, UCHAR hbc, UCHAR vbc)
{
	UCHAR vcnt, hcnt;
	UCHAR vcnt2, hcnt2;
	UCHAR rs_hbc, rs_vbc;
	UCHAR hadd, vadd;
	int loop = 0;
	INT32 Bv = AL_FIX_VAL(4.00);

	if (vbc % 16 != 0 || hbc % 16 != 0) {
		UPRINTF_ERR(-1);
	}
	if (vbc < AL_HSC_WDV || hbc < AL_HSC_WDH) {
		UPRINTF_ERR(-1);
	}

	rs_hbc = AL_HSC_WDH;
	rs_vbc = AL_HSC_WDV;
	hadd = hbc / rs_hbc;
	vadd = vbc / rs_vbc;

	for(vcnt=0; vcnt<rs_vbc; vcnt++){
		for(hcnt=0; hcnt<rs_hbc; hcnt++){
			// ?ïΩ?íºÇâ¡éZ
			for(vcnt2=vcnt*vadd; vcnt2<vcnt*vadd+vadd; vcnt2++){
				for(hcnt2=hcnt*hadd; hcnt2<hcnt*hadd+hadd; hcnt2++){
					fttAlHscInstance->mttStat.muiR[loop]	+= g_awbblk[g_cur_sensor_id][vcnt2][hcnt2].r;
					fttAlHscInstance->mttStat.muiGr[loop]	+= g_awbblk[g_cur_sensor_id][vcnt2][hcnt2].g;
					fttAlHscInstance->mttStat.muiGb[loop]	+= g_awbblk[g_cur_sensor_id][vcnt2][hcnt2].g;
					fttAlHscInstance->mttStat.muiB[loop]	+= g_awbblk[g_cur_sensor_id][vcnt2][hcnt2].b;
				}
			}
			loop++;
		}
	}

	fttAlHscInstance->muiWdSizeH	= AL_HSC_WDH;
	fttAlHscInstance->muiWdSizeV	= AL_HSC_WDV;
	fttAlHscInstance->muiTblSizeH 	= AL_HSC_WDH; // todo muiWdSizeHÇ≈?ÇÈÇ◊Ç´
	fttAlHscInstance->muiTblSizeV 	= AL_HSC_WDV; // todo muiWdSizeVÇ≈?ÇÈÇ◊Ç´

	fttAlHscInstance->muiHscReqMode	= (UI_32)AL_HSC_WRAP_MD_AUTO;
	fttAlHscInstance->muiManuMix	= (UI_32)0;

	if (param->in_outdoor == E_AWB_INDOOR) {
		Bv = D_FORCED_INDOOR_BV;
	} else if (param->in_outdoor == E_AWB_OUTDOOR) {
		Bv = D_FORCED_OUTDOOR_BV;
	} else {
		Bv = param->sensor[g_cur_sensor_id].bv;
	}

	// êFèÓïÒê›íË
	AL_HSC_SET_BV(fttAlHscInstance, Bv);
	fttAlHscInstance->msqCtemp = g_awb_result.sensor[g_cur_sensor_id].currentCTemp;
	fttAlHscInstance->msqWBGain[0] = (SQ_32)g_awb_result.sensor[g_cur_sensor_id].wb_gain[0]<<8;
	fttAlHscInstance->msqWBGain[1] = (SQ_32)g_awb_result.sensor[g_cur_sensor_id].wb_gain[1]<<8;
	fttAlHscInstance->msqWBGain[2] = (SQ_32)g_awb_result.sensor[g_cur_sensor_id].wb_gain[2]<<8;

	// SVòAìÆ
	//WrapAwb_SwitchHscFunction(g_awb_param.sv);

	// Main??
	AlHscWrap_Main(fttAlHscInstance, aptWorkMem);
}

static void hsc_reset(TT_AlHscInstance* fttAlHscInstance, UCHAR mode)
{
	int table;

	// table copy
	for(table = 0; table < AL_HSC_TBLNUM; table++ ){
		fttAlHscInstance->mttLscTable[table].mpiFshdTable = (UI_16*)(table);	// Just assign table index as pointer
	}

	// paramê›íË
	fttAlHscInstance->muiMode = mode;// todo mon=0 cap=1
	fttAlHscInstance->muiTblFormat = 3; // 16bit (=IO_PRO.PS4.PSCTL1.bit.PSFMT)
	fttAlHscInstance->muiTblSizeH  = PRO_FSHD_AFN_H;
	fttAlHscInstance->muiTblSizeV  = PRO_FSHD_AFN_V;
	fttAlHscInstance->muiTblLineOffset = PRO_FSHD_AFN_GHB;

	// Set??
	AlHscWrap_Reset(fttAlHscInstance);
}

// called by alhscwrap.c
PUBLIC void	AlHscUserMixTest( 
				UI_32		puiDstAddr		,/*[I ] dst address*/
				UI_32		puiSrc1Addr		,/*[I ] Src1 address*/
				UI_32		puiSrc2Addr		,/*[I ] Src2 address*/
				UI_08		puiSrc1Ratio	)/*[I ] Src1 weight (0.8 fixed point)*/
{
	g_awb_result.sensor[g_cur_sensor_id].shdtbl1_idx = puiSrc1Addr; // Its table index not address
	g_awb_result.sensor[g_cur_sensor_id].shdtbl0_idx = puiSrc2Addr; // Its table index not address
	g_awb_result.sensor[g_cur_sensor_id].shdbld1 = (SI_16)(puiSrc1Ratio);
	g_awb_result.sensor[g_cur_sensor_id].shdbld0 = (SI_16)(256-g_awb_result.sensor[g_cur_sensor_id].shdbld1 );
	if (g_ave_stat) {
		g_awb_result.sensor[3] = g_awb_result.sensor[2] = g_awb_result.sensor[1] = g_awb_result.sensor[0]; 
	}
}

static void calc_shd_blending(float ct, T_AWB_PARAM *param)
{
	float ratio_left, interval;

	if (ct >= param->light_source_0) {
		g_awb_result.sensor[g_cur_sensor_id].shdtbl0_idx = 0;
		g_awb_result.sensor[g_cur_sensor_id].shdtbl1_idx = 1;
		g_awb_result.sensor[g_cur_sensor_id].shdbld0 = (256);
		g_awb_result.sensor[g_cur_sensor_id].shdbld1 = (256-g_awb_result.sensor[g_cur_sensor_id].shdbld0 );
	} else if (ct == param->light_source_1) {
		g_awb_result.sensor[g_cur_sensor_id].shdtbl0_idx = 1;
		g_awb_result.sensor[g_cur_sensor_id].shdtbl1_idx = 2;
		g_awb_result.sensor[g_cur_sensor_id].shdbld0 = (256);
		g_awb_result.sensor[g_cur_sensor_id].shdbld1 = (256-g_awb_result.sensor[g_cur_sensor_id].shdbld0 );
	} else if (ct <= param->light_source_2) {
		g_awb_result.sensor[g_cur_sensor_id].shdtbl0_idx = 2;
		g_awb_result.sensor[g_cur_sensor_id].shdtbl1_idx = 1;
		g_awb_result.sensor[g_cur_sensor_id].shdbld0 = (256);
		g_awb_result.sensor[g_cur_sensor_id].shdbld1 = (256-g_awb_result.sensor[g_cur_sensor_id].shdbld0 );
	} else if (ct < param->light_source_0 && ct > param->light_source_1) {
		interval	= (float)(param->light_source_0 - param->light_source_1) / (float)256;
		ratio_left	= (float)(ct - param->light_source_1) / interval;
		g_awb_result.sensor[g_cur_sensor_id].shdtbl0_idx = 0;
		g_awb_result.sensor[g_cur_sensor_id].shdtbl1_idx = 1;
		g_awb_result.sensor[g_cur_sensor_id].shdbld0 = (SHORT)ratio_left;
		g_awb_result.sensor[g_cur_sensor_id].shdbld1 = (256-g_awb_result.sensor[g_cur_sensor_id].shdbld0 );
	} else if (ct < param->light_source_1 && ct > param->light_source_2) {
		interval	= (float)(param->light_source_1 - param->light_source_2) / (float)256;
		ratio_left	= (float)(ct - param->light_source_2) / interval;
		g_awb_result.sensor[g_cur_sensor_id].shdtbl0_idx = 1;
		g_awb_result.sensor[g_cur_sensor_id].shdtbl1_idx = 2;
		g_awb_result.sensor[g_cur_sensor_id].shdbld0 = (SHORT)ratio_left;
		g_awb_result.sensor[g_cur_sensor_id].shdbld1 = (256-g_awb_result.sensor[g_cur_sensor_id].shdbld0 );
	}
}
