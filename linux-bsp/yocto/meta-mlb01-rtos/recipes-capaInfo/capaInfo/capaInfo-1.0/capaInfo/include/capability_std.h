#ifndef __CAPABILITY_STD_H__
#define __CAPABILITY_STD_H__

/**** include ***************************************************/


/**** define ****************************************************/
#define D_V_CHEANEL_MAX		4
#define D_CHARACTER_SET		32
#define D_GOP_STRUCT_MAX	2
#define D_V_STRAGE_FORMAT_MAX	2
#define D_A_CODEC_MODE_MAX	2
#define D_A_SAMPLING_RATE_MAX	6
#define D_A_MIC_SELECTION_MAX	2
#define D_A_CHANNEL_TYPE_MAX	2
#define D_SCAN_MODE_MAX		2
#define D_EXPOSURE_TIME_MAX	20
#define D_SENSOR_MAX		4
#define D_AREA_NO_MAX		8
#define D_SHARPNESS_MAX		15
#define D_EDGE_MAX		15
#define D_WHITE_BALANC_MAX	12
#define D_TIME_LAPSE_MAX	7
#define D_OSD_STYLE_TP_MAX	4


/**** typedef ***************************************************/
typedef unsigned long ALIGNMENT;


/**** enumeration ***********************************************/


/**** Structure *************************************************/
/********* Common ***********************************************/
struct cap_cameraIf_char {
	unsigned long cameraIf;
	ALIGNMENT Reserve[3];
	char value[D_CHARACTER_SET];
};

struct cap_cameraIf_num {
	unsigned long cameraIf;
	unsigned long numVal;
};

struct cap_imageSize_info {
	unsigned long cameraIf;
	unsigned long wSize;
	unsigned long hSize;
	ALIGNMENT Reserve;
	char size[D_CHARACTER_SET];
};

struct cap_rangeSetting_info {
	unsigned long min;
	unsigned long max;
	unsigned long def;
	unsigned long res;
};

struct cap_sensor {
	unsigned long maxNum;
	ALIGNMENT Reserve;
	struct cap_cameraIf_char pos[D_SENSOR_MAX];
	struct cap_rangeSetting_info range;
};

struct cap_area_no {
	unsigned long maxNum;
	ALIGNMENT Reserve;
	struct cap_cameraIf_char num[D_AREA_NO_MAX];
	struct cap_rangeSetting_info range;
};

/********* Sensor Type ******************************************/
struct cap_sensor_type {
	unsigned long sensorType;
	unsigned long maxStream;
	unsigned long streamType[D_V_CHEANEL_MAX];
};

/********* Stitching Type ***************************************/
struct cap_stitching_type {
	unsigned long maxStitchingType;
	unsigned long stitchingTypeOffset;
};

typedef cap_cameraIf_char stitchingType_t;

/********* DOL Mode *********************************************/
struct cap_dol_mode {
	unsigned long maxDolMode;
	unsigned long dolModeOffset;
};

typedef cap_cameraIf_char dolMode_t;

/********* Stream ***********************************************/
struct cap_video_stream_channel {
	unsigned long streamType;
	unsigned long mode;
	unsigned long select;
	unsigned long maxCodec;
	unsigned long codecOffset;
	unsigned long sdSave;
	unsigned long ispOption;
	unsigned long maxFrameRate;
	unsigned long frameRateOffset;
	unsigned long maxImgSize;
	unsigned long imgSizeOffset;
};
typedef cap_cameraIf_char codec_t;

typedef cap_cameraIf_num frameRate_t;

typedef cap_imageSize_info imgSize_t;

/********* Bitrate **********************************************/
struct cap_bitrate_info {
	unsigned long algorithm;
	ALIGNMENT Reserve;
	unsigned long cbrMin;
	unsigned long cbrMax;
	unsigned long cbrAverage;
	unsigned long cbrRes;
	unsigned long vbrMin;
	unsigned long vbrMax;
	unsigned long vbrAverage;
	unsigned long vbrRes;
};

/********* GOP **************************************************/
struct cap_gop_struct {
	unsigned long maxNum;
	ALIGNMENT Reserve;
	struct cap_cameraIf_char g_struct[D_GOP_STRUCT_MAX];
	struct cap_rangeSetting_info range;
};

struct cap_video_strage_format {
	unsigned long maxNum;
	ALIGNMENT Reserve;
	struct cap_cameraIf_char g_format[D_V_STRAGE_FORMAT_MAX];
	struct cap_rangeSetting_info range;
};

struct cap_gop_info {
	struct cap_gop_struct gopStruct;
	struct cap_video_strage_format format;
};

/********* IP ***************************************************/
struct cap_ip_setting {
	unsigned long hlc;
	unsigned long backlight;
	unsigned long defog;
	unsigned long ta;
	unsigned long od;
	unsigned long fd;
	unsigned long fr;
	unsigned long eis;
	unsigned long metadata;
	ALIGNMENT Reserve;
};

/********* Audio ************************************************/
struct cap_audio_codec {
	unsigned long maxNum;
	ALIGNMENT Reserve;
	struct cap_cameraIf_char mode[D_A_CODEC_MODE_MAX];
	struct cap_rangeSetting_info range;
};

struct cap_audio_sampling_rate {
	unsigned long maxNum;
	ALIGNMENT Reserve;
	struct cap_cameraIf_char rate[D_A_SAMPLING_RATE_MAX];
	struct cap_rangeSetting_info range;
};

struct cap_audio_mic_sel {
	unsigned long maxNum;
	ALIGNMENT Reserve;
	struct cap_cameraIf_char mic[D_A_MIC_SELECTION_MAX];
	struct cap_rangeSetting_info range;
};

struct cap_audio_channel_type {
	unsigned long maxNum;
	ALIGNMENT Reserve;
	struct cap_cameraIf_char chtyp[D_A_CHANNEL_TYPE_MAX];
	struct cap_rangeSetting_info range;
};

struct cap_audio_channel {
	unsigned long suport;
	struct cap_audio_codec codec;
	struct cap_audio_sampling_rate sampRate;
	struct cap_audio_mic_sel mic;
	struct cap_rangeSetting_info bitRate;
	struct cap_audio_channel_type channelType;
};

/********* Camera Terminal **************************************/
struct cap_camera_terminal {
	unsigned long scanningMode;
	unsigned long autoExposure;
	unsigned long exposureTime;
	unsigned long focus;
	unsigned long iris;
	unsigned long zoom;
	unsigned long pan;
	unsigned long tilt;
	unsigned long roll;
	unsigned long privacy;
};

/********* Scanning Mode ****************************************/
struct cap_scanning_mode {
	unsigned long maxNum;
	ALIGNMENT Reserve;
	struct cap_cameraIf_char mode[D_SCAN_MODE_MAX];
};

/********* AE ***************************************************/
struct cap_auto_exposure {
	unsigned long modeManual;
	unsigned long modeAuto;
	unsigned long shutterPriority;
	unsigned long aperturePriority;
	unsigned long aePriorityCtl;
	ALIGNMENT Reserve;
};

/********* Exposure Time ****************************************/
struct cap_exposure_time_param {
	unsigned long cameraIf;
	unsigned long numerator;
	unsigned long denominator;
	ALIGNMENT Reserve;
};

struct cap_exposure_time {
	unsigned long maxNum;
	ALIGNMENT Reserve;
	struct cap_exposure_time_param param[D_EXPOSURE_TIME_MAX];
	struct cap_rangeSetting_info range;
};

/********* Focus ************************************************/
struct cap_focus {
	struct cap_rangeSetting_info range;
	unsigned long focusAutoCtl;
	ALIGNMENT Reserve;
};

/********* Iris *************************************************/
struct cap_iris {
	struct cap_rangeSetting_info range;
};

/********* Zoom *************************************************/
struct cap_zoom {
	struct cap_rangeSetting_info range;
};

/********* Pan **************************************************/
struct cap_pan {
	struct cap_rangeSetting_info range;
};

/********* Tilt *************************************************/
struct cap_tilt {
	struct cap_rangeSetting_info range;
};

/********* Roll *************************************************/
struct cap_roll {
	struct cap_rangeSetting_info range;
};

/********* Privacy Mask *****************************************/
struct cap_privacy_mask {
	unsigned long suport;
	struct cap_sensor sensor;
	struct cap_area_no areaNum;
	struct cap_rangeSetting_info xCoordinate;
	struct cap_rangeSetting_info yCoordinate;
	struct cap_rangeSetting_info width;
	struct cap_rangeSetting_info lines;
	struct cap_rangeSetting_info y;
	struct cap_rangeSetting_info cb;
	struct cap_rangeSetting_info cr;
};

/********* Processing Unit **************************************/
struct cap_processing_unit {
	unsigned long brightness;
	unsigned long contrast;
	unsigned long hue;
	unsigned long saturation;
	unsigned long sharpness;
	unsigned long gamma;
	unsigned long digitalMultiplierZoom;
	unsigned long whiteBalance;
	unsigned long backlight;
	unsigned long gain;
	unsigned long powerLineFrequency;
	unsigned long analogVideo;
};

/********* Brightness *******************************************/
struct cap_brightness {
	struct cap_rangeSetting_info range;
};

/********* Contrast *********************************************/
struct cap_contrast {
	struct cap_rangeSetting_info range;
};

/********* Hue **************************************************/
struct cap_hue {
	unsigned long autoSupport;
	struct cap_rangeSetting_info range;
};

/********* Saturation *******************************************/
struct cap_saturation {
	struct cap_rangeSetting_info range;
};

/********* Sharpness ********************************************/
struct cap_sharpness {
	unsigned long maxNum;
	ALIGNMENT Reserve;
	struct cap_cameraIf_num param[D_SHARPNESS_MAX];
	struct cap_rangeSetting_info range;
};

struct cap_edge {
	unsigned long maxNum;
	ALIGNMENT Reserve;
	struct cap_cameraIf_num param[D_EDGE_MAX];
	struct cap_rangeSetting_info range;
};

struct cap_sharpness_edge {
	struct cap_sharpness sharpness;
	struct cap_edge edge;
};

/********* Gamma ************************************************/
struct cap_gamma {
	struct cap_rangeSetting_info range;
};

/********* Digital Multiplier(Zoom) *****************************/
struct cap_digital_multi_zoom {
	struct cap_rangeSetting_info range;
};

/********* White Balance,Auto ***********************************/
struct cap_wb_component {
	unsigned long wbTempSupport;
	ALIGNMENT Reserve;
	struct cap_rangeSetting_info wbTemp;
	unsigned long wbCompSupport;
	unsigned long maxNum;
	struct cap_cameraIf_char param[D_WHITE_BALANC_MAX];
	struct cap_rangeSetting_info range;
	struct cap_rangeSetting_info rGain;
	struct cap_rangeSetting_info bGain;
};

/********* Backlight Compensation *******************************/
struct cap_backlight {
	struct cap_rangeSetting_info range;
};

/********* Gain *************************************************/
struct cap_gain {
	struct cap_rangeSetting_info range;
};

/********* Power Line Frequency *********************************/
struct cap_power_line_frequency {
	struct cap_rangeSetting_info range;
};

/********* Analog Video Standard Status *************************/
struct cap_analog_video {
	struct cap_rangeSetting_info standardStatus;
	struct cap_rangeSetting_info lockStatus;
};

/********* Specific function ************************************/
struct cap_specific_function {
	unsigned long timeLapse;
	unsigned long osd;
	unsigned long storage;
};

/********* Time Lapse *******************************************/
struct cap_time_lapse {
	unsigned long maxNum;
	struct cap_cameraIf_num interval[D_TIME_LAPSE_MAX];
	struct cap_rangeSetting_info range;
};

/********* OSD **************************************************/
struct cap_osd_style {
	unsigned long maxNum;
	ALIGNMENT Reserve;
	struct cap_cameraIf_num trans_perency[D_OSD_STYLE_TP_MAX];
	struct cap_rangeSetting_info range;
};

struct cap_osd_area_select {
	struct cap_sensor sensor;
	struct cap_area_no areaNum;
	struct cap_rangeSetting_info xCoordinate;
	struct cap_rangeSetting_info yCoordinate;
};

struct cap_osd_disp_control {
	struct cap_sensor sensor;
	struct cap_area_no areaNum;
	unsigned long enabled;
	ALIGNMENT Reserve;
};

struct cap_osd {
	struct cap_osd_style style;
	struct cap_osd_area_select areaSelect;
	struct cap_osd_disp_control dispControl;
};

/********* Storage **********************************************/
struct cap_sd_save {
	unsigned long saveSetting;
	unsigned long def;
};

struct cap_storage {
	struct cap_sd_save sdSave;
};



#endif// __CAPABILITY_STD_H__
