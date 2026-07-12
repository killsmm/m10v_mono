#ifndef H_OSD_PARAM_CHANGE_H
#define H_OSD_PARAM_CHANGE_H

#define OSD_SETTING	"/tmp/osd_setting"
typedef enum {
	E_REQUEST_TYPE_SET = 1,//Set data
	E_REQUEST_TYPE_PRINT,//Printe data
	E_REQUEST_TYPE_END,//End Receive

}E_REQUEST_TYPE;

typedef struct {
	E_REQUEST_TYPE request_type;
	unsigned short stream_id;//0~31 is stream ID. Other is finishing
	unsigned short stream_frame_rate;
	unsigned short image_width;//Used for Jpeg only
	unsigned short image_height;//Used for Jpeg only
}T_OSD_PARAM_CHANGE;
#endif
