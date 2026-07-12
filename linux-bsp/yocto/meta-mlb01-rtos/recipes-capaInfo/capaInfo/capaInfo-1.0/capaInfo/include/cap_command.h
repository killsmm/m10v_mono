#ifndef CAP_COMMAND_H_
#define CAP_COMMAND_H_

#ifdef __cplusplus
extern "C" {
#endif

/**** define **********************/
#define D_CM_RESULT_OK 	0
#define D_CM_RESULT_NG 	1

#define D_CHAR_SET 	64

/**** enumeration *****************/
enum E_CP_VIDEO_CHANNL {
	E_VIDEOCHANNL_MAIN,
	E_VIDEOCHANNL_SUB_1,
	E_VIDEOCHANNL_SUB_2,
	E_VIDEOCHANNL_SUB_3,
};

enum E_CP_RENGE_TYPE {
	E_GOP_ST		= 0x01,
	E_GOP_V_STG_FMT,
	E_AUDIO,
	E_EXPOSURE_TIME,
	E_FOCUS,
	E_IRIS,
	E_ZOOM,
	E_PAN,
	E_TILT,
	E_ROLL,
	E_PRIVACY_MASK_SENSOR,
	E_PRIVACY_MASK_AREA_NO,
	E_PRIVACY_MASK_X_COORDINATE,
	E_PRIVACY_MASK_Y_COORDINATE,
	E_PRIVACY_MASK_WIDTH,
	E_PRIVACY_MASK_LINES,
	E_PRIVACY_MASK_Y,
	E_PRIVACY_MASK_CB,
	E_PRIVACY_MASK_CR,
	E_BRIGHTNESS,
	E_CONTRAST,
	E_HUE,
	E_SATURATION,
	E_SHARPNESS,
	E_EDGE,
	E_GAMMA,
	E_DIGITAL_MULTIPLIER,
	E_WHITE_BALANCE_TEMPERATURE,
	E_WHITE_BALANCE_COMPONENT,
	E_WHITE_BALANCE_COMPONENT_R_GAIN,
	E_WHITE_BALANCE_COMPONENT_B_GAIN,
	E_BACKLIGHT,
	E_GAIN,
	E_POWER_LINE_FREQUENCY,
	E_ANALOG_VIDEO_STANDARD_STATUS,
	E_ANALOG_VIDEO_LOCK_STATUS,
	E_TIME_LAPSE,
	E_OSD,
	E_OSD_SENSOR,
	E_OSD_AREA_NO,
	E_OSD_X_COORDINATE,
	E_OSD_Y_COORDINATE,
	E_OSD_DISP_CONTROL_SENSOR,
	E_OSD_DISP_CONTROL_AREA_NO,
};

enum E_CP_GOP_TYPE {
	E_GOP_STRUCT,
	E_VIDEO_STRANGE_FORMAT,
};

enum E_CP_IP_TYPE {
	E_HLC,
	E_BACKLIGHTCOMP,
	E_DEFOG,
	E_TA,
	E_OD,
	E_FD,
	E_FR,
	E_EIS,
	E_METADATA,
};

enum E_CP_AUDIO_TYPE {
	E_CODEC,
	E_SAMPLING,
	E_MIC,
	E_CH_TYPE,
};

enum E_CP_SELECT_TYPE {
	E_SENSOR_SET,
	E_AREA_NO_SET,
	E_DSP_CTL_SENSOR_SET,
	E_DSP_CTL_AREA_NO_SET,
};

enum E_CP_SHARPEDGE_TYPE {
	E_SHARPNESS_SET,
	E_EDGE_SET,
};

/**** structure *******************/
struct cameraif_string{
	char str[D_CHAR_SET];
	unsigned long camif;
};

struct cameraif_number{
	unsigned long num;
	unsigned long camif;
};

struct cameraif_size{
	char str[D_CHAR_SET];
	unsigned long width;
	unsigned long height;
	unsigned long camif;
};

struct bitrate_info {
	unsigned long algorithm;
	unsigned long cbrMin;
	unsigned long cbrMax;
	unsigned long cbrAverage;
	unsigned long cbrRes;
	unsigned long vbrMin;
	unsigned long vbrMax;
	unsigned long vbrAverage;
	unsigned long vbrRes;
};

struct range_info {
	unsigned long min;
	unsigned long max;
	unsigned long def;
	unsigned long res;
};

struct ae_info {
	unsigned long manual;
	unsigned long at;
	unsigned long shutter;
	unsigned long aperture;
	unsigned long control;
};

struct expo_time_info {
	unsigned long camif;
	unsigned long numerator;
	unsigned long denominator;
};

/* Capability generate & delete */
void generate_capa_info();
void delete_capa_info();

/* Get basic information */
int get_Sensor_type(unsigned long* type, unsigned long* num, struct cameraif_string* mode);
int get_Channel_type(unsigned long* maxnum, unsigned long* chList);

/* Video channel make & delete */
int make_Video_channel_info(unsigned long type, unsigned long mode);
void delete_Video_channel_info();

/* Get video channel information */
int get_Codec(unsigned long type, struct cameraif_string* codec);
int get_Sd_save_info(unsigned long type, unsigned long* sd);
int get_Isp_info(unsigned long type, unsigned long* isp);
int get_Frame_rate(unsigned long type, unsigned long* num, struct cameraif_number* fr);
int get_Size(unsigned long type, unsigned long* num, struct cameraif_size* size);

/* Get Bitrate information */
int get_Bitrate_info(struct bitrate_info* bitrate);

/* Get Range information */
int get_Range_info(unsigned long type, struct range_info* range);

/* Get GOP information */
int get_Gop_info(unsigned long type, unsigned long* num, struct cameraif_string* str);

/* Get IP information */
int get_Ip_info(unsigned long type, unsigned long* ip);

/* Get Audio information */
int get_Audio_info(unsigned long type, unsigned long* num, struct cameraif_string* info);

/* Get Scan Mode information */
int get_Scan_mode_info(unsigned long* flag, unsigned long* num, struct cameraif_string* info);

/* Get Auto-Exposure information */
int get_Ae_info(struct ae_info* ae);

/* Get Exposure Time information */
int get_Expo_time_info(unsigned long* num, struct expo_time_info* expotime);

/* Get Focus information */
int get_Focus_info(struct range_info range, unsigned long* at);

/* Get Privacy Mask information */
int get_Priv_mask_info(unsigned long type, unsigned long* num, struct cameraif_string* info);

/* Get Hue information */
int get_Hue_info(unsigned long* at);

/* Get Sharpness / Edge information */
int get_SharpEdge_info(unsigned long type, unsigned long* num, struct cameraif_number* info);

/* Get White Balance component information */
int get_Wb_component_info(unsigned long* num, struct cameraif_string* info);

/* Get Time Lapse information */
int get_Time_lapse_info(unsigned long* num, struct cameraif_number* info);

/* Get OSD information */
int get_Osd_info(unsigned long* num, struct cameraif_number* tran, unsigned long* enab);

/* Get OSD Details information */
int get_Osd_detail_info(unsigned long type, unsigned long* num, struct cameraif_string* info);

/* Get Storage information */
int get_Storage_info(unsigned long* save, unsigned long* def);

#ifdef __cplusplus
};
#endif
#endif /* CAP_COMMAND_H_ */
