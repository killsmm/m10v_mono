#include <string.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include "uvc-gadget_common.h"

enum {
	VCH_MAIN = 0,
	VCH_SUB1,
	VCH_SUB2,
	VCH_MAX
};
// immediate cameraif parameter
typedef enum {
	E_UVC_IFT_COM_INIT_SETTING		= 0x00000100,
	E_UVC_IFT_COM_SET_IMG_CFG		= 0x00010100,
	E_UVC_IFT_COM_SET_VIDEO_CAP_CFG		= 0x00010D00,
	E_UVC_IFT_COM_SET_IMG_EFFECT		= 0x00010E00,
	E_UVC_IFT_COM_SET_MODE_CHANGE		= 0x00020100,
} E_UVC_IFT_COM;

typedef enum {
	E_UVC_IFT_SUB_COM_FINISH_BOOT		= 0x00000100,

	E_UVC_IFT_SUB_COM_EV			= 0x00000100,
	E_UVC_IFT_SUB_COM_AE_METERING		= 0x00000800,
	E_UVC_IFT_SUB_COM_ADAPTIVE_AE		= 0X00001000,

	E_UVC_IFT_SUB_COM_V_CAP_ENABLE		= 0x00000100,
	E_UVC_IFT_SUB_COM_V_CAP_SIZE		= 0x00000200,
	E_UVC_IFT_SUB_COM_V_CAP_FRAMERATE	= 0x00000300,

	E_UVC_IFT_SUB_COM_IMG_ROTATE		= 0x00000100,
	E_UVC_IFT_SUB_COM_IMG_MASK		= 0x00000200,
	E_UVC_IFT_SUB_COM_IMG_ZOOM		= 0x00000300,
	E_UVC_IFT_SUB_COM_IMG_COLOR		= 0x00000400,

	E_UVC_IFT_SUB_COM_CHANGE_MODE		= 0x00000100,
} E_UVC_IFT_SUB_COM;

static const struct uvc_frame_info uvc_frames_mjpeg[] = {
	{ "UHD",   3840, 2160, { 10000000/30, 0 } },
	{ "WQHD",  2560, 1440, { 10000000/60, 10000000/30, 0 } },
	{ "FHD",   1920, 1080, { 10000000/60, 1000000000/5994, 1000000000/2997, 1000000000/2398, 0 } },
	{ "HD",    1280,  720, { 10000000/60, 1000000000/5994, 0 } },
	{ "NTSC",   720,  480, { 10000000/60, 1000000000/5994, 0 } },
	{ "VGA",    640,  480, { 10000000/60, 1000000000/5994, 0 } },
	{  0,         0,    0, { 0 } },
};
/*hs bulk*/
static const struct uvc_frame_info uvc_frames_yuyv_bulk[] = {
	{ "FHD",    1920, 1080, { 10000000/30, 0 } },
	{ "HD",    1280,  720, { 10000000/30, 0 } },
	{  0,         0,    0, { 0 } },
};
/*hs iso*/
static const struct uvc_frame_info uvc_frames_yuyv[] = {
	{ "HD",    1280,  720, { 10000000/15, 0 } },
	{  0,         0,    0, { 0 } },
};


static const struct uvc_frame_info uvc_frames_h264[] = {
	{ "UHD",   3840, 2160, { 10000000/30, 0 } },
	{ "WQHD",  2560, 1440, { 10000000/30, 0 } },
	{ "FHD",   1920, 1080, { 10000000/60, 1000000000/5994, 1000000000/2997, 1000000000/2398, 0 } },
	{ "HD",    1280,  720, { 10000000/60, 1000000000/5994, 0 } },
	{ "NTSC",   720,  480, { 10000000/60, 1000000000/5994, 0 } },
	{ "VGA",    640,  480, { 10000000/60, 1000000000/5994, 0 } },
	{  0,         0,    0, { 0 } },
};
static const struct uvc_frame_info uvc_frames_h264_roi[] = {
	{ "UHD",   3840, 2160, { 10000000/30, 0 } },
	{ "FHD",   1920, 1080, { 10000000/60, 1000000000/5994, 1000000000/2997, 1000000000/2398, 0 } },
	{ "HD",    1280,  720, { 10000000/60, 1000000000/5994, 0 } },
	{ "NTSC",   720,  480, { 10000000/60, 1000000000/5994, 0 } },
	{ "VGA",    640,  480, { 10000000/60, 1000000000/5994, 0 } },
	{  0,         0,    0, { 0 } },
};

/*hs bulk*/
static const struct uvc_format_info uvc_formats_bulk[] = {
	{ V4L2_PIX_FMT_MJPEG, uvc_frames_mjpeg },
	{ V4L2_PIX_FMT_YUYV, uvc_frames_yuyv_bulk },
	{ V4L2_PIX_FMT_NV12, uvc_frames_yuyv_bulk },
	{ V4L2_PIX_FMT_H264, uvc_frames_h264 },
};

/*hs iso*/
static const struct uvc_format_info uvc_formats[] = {
	{ V4L2_PIX_FMT_MJPEG, uvc_frames_mjpeg },
	{ V4L2_PIX_FMT_YUYV, uvc_frames_yuyv },
	{ V4L2_PIX_FMT_NV12, uvc_frames_yuyv },
	{ V4L2_PIX_FMT_H264, uvc_frames_h264 },
};
static const struct uvc_format_info uvc_formats_roi[] = {
	{ V4L2_PIX_FMT_H264, uvc_frames_h264_roi },
};


static const struct uvc_frame_info ss_uvc_frames_mjpeg[] = {
	{ "UHD",   3840, 2160, { 10000000/30, 0 } },
	{ "WQHD",  2560, 1440, { 10000000/60, 10000000/30, 0 } },
	{ "FHD",   1920, 1080, { 10000000/60, 1000000000/5994, 1000000000/2997, 1000000000/2398, 0 } },
	{ "HD",    1280,  720, { 10000000/60, 1000000000/5994, 0 } },
	{ "NTSC",   720,  480, { 10000000/60, 1000000000/5994, 0 } },
	{ "VGA",    640,  480, { 10000000/60, 1000000000/5994, 0 } },
	{  0,         0,    0, { 0 } },
};

static const struct uvc_frame_info ss_uvc_frames_yuyv_bulk[] = {
	{ "FHD",   1920, 1080, { 10000000/60, 1000000000/5994, 10000000/30, 1000000000/2997, 1000000000/2398, 0 } },
	{ "HD",    1280,  720, { 10000000/60, 1000000000/5994, 10000000/30, 0 } },
	{ "NTSC",   720,  480, { 10000000/60, 1000000000/5994, 10000000/30, 0 } },
	{ "VGA",    640,  480, { 10000000/60, 1000000000/5994, 0 } },
	{  0,         0,    0, { 0 } },
};

static const struct uvc_frame_info ss_uvc_frames_yuyv[] = {
	{ "FHD",   1920, 1080, { 10000000/30, 0 } },
	{ "HD",    1280,  720, { 10000000/30, 0 } },
	{  0,         0,    0, { 0 } },
};


static const struct uvc_frame_info ss_uvc_frames_h264[] = {
	{ "UHD",   3840, 2160, { 10000000/30, 0 } },
	{ "WQHD",  2560, 1440, { 10000000/30, 0 } },
	{ "FHD",   1920, 1080, { 10000000/60, 1000000000/5994, 1000000000/2997, 1000000000/2398, 0 } },
	{ "HD",    1280,  720, { 10000000/60, 1000000000/5994, 0 } },
	{ "NTSC",   720,  480, { 10000000/60, 1000000000/5994, 0 } },
	{ "VGA",    640,  480, { 10000000/60, 1000000000/5994, 0 } },
	{  0,         0,    0, { 0 } },
};

static const struct uvc_frame_info ss_uvc_frames_h264_roi[] = {
	{ "UHD",   3840, 2160, { 10000000/30, 0 } },
	{ "FHD",   1920, 1080, { 10000000/60, 1000000000/5994, 1000000000/2997, 1000000000/2398, 0 } },
	{ "HD",    1280,  720, { 10000000/60, 1000000000/5994, 0 } },
	{ "NTSC",   720,  480, { 10000000/60, 1000000000/5994, 0 } },
	{ "VGA",    640,  480, { 10000000/60, 1000000000/5994, 0 } },
	{  0,         0,    0, { 0 } },
};

static const struct uvc_format_info ss_uvc_formats_bulk[] = {
	{ V4L2_PIX_FMT_MJPEG, ss_uvc_frames_mjpeg },
	{ V4L2_PIX_FMT_YUYV, ss_uvc_frames_yuyv_bulk },
	{ V4L2_PIX_FMT_NV12, ss_uvc_frames_yuyv_bulk },
	{ V4L2_PIX_FMT_H264, ss_uvc_frames_h264 },
};

static const struct uvc_format_info ss_uvc_formats[] = {
	{ V4L2_PIX_FMT_MJPEG, ss_uvc_frames_mjpeg },
	{ V4L2_PIX_FMT_YUYV, ss_uvc_frames_yuyv },
	{ V4L2_PIX_FMT_NV12, ss_uvc_frames_yuyv },
	{ V4L2_PIX_FMT_H264, ss_uvc_frames_h264 },
};
static const struct uvc_format_info ss_uvc_formats_roi[] = {
	{ V4L2_PIX_FMT_H264, ss_uvc_frames_h264_roi },
};


/* Represents a UVC based video output device */
struct uvc_capability brightness = {
	.len = 2,
	.min = 0,
	.max = 6,
	.def = 3,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};
#if 0 //rtos not enable yet.
struct uvc_capability contrast = {
	.len = 2,
	.min = 0,
	.max = 10,
	.def = 5,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};
#endif
struct uvc_capability hue = {
	.len = 2,
	.min = 0,
	.max = 10,
	.def = 5,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};
struct uvc_capability saturation = {
	.len = 2,
	.min = 0,
	.max = 10,
	.def = 5,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};
struct uvc_capability sharpness = {
	.len = 2,
	.min = 0,
	.max = 6,
	.def = 2,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};
struct uvc_capability flicker = {
	.len = 1,
	.min = 0,
	.max = 2,
	.def = 2,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};
struct uvc_capability coltemper = {
	.len = 2,
	.min = 2500,
	.max = 12500,
	.def = 4500,
	.step= 500,
	.val = UVC_VAL_EMPTY,
};
struct uvc_capability coltemper_auto = {
	.len = 1,
	.min = 0,
	.max = 1,
	.def = 1,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};
struct uvc_capability ae_mode = {
	.len = 1,
	.min = 0,
	.max = 0,
	.def = 2,
	.step= 3,
	.val = UVC_VAL_EMPTY
};
/*
struct uvc_capability ae_priority = {
	.len = 1,
	.min = 0,
	.max = 1,
	.def = 0,
	.step= 1,
	.val = UVC_VAL_EMPTY
};
*/
struct uvc_capability exposure_time= {
        .len = 4,
        .min = 1,
        .max = 1000,
        .def = 333,
        .step= 1, 
        .val = UVC_VAL_EMPTY,
};

struct uvc_capability zoom_roi = {
	.len = 2,
	.min = 1,
	.max = 10,
	.def = 5,
	.step= 1,
	.val = -1,
};

struct uvc_capability zoom = {
	.len = 2,
	.min = 1,
	.max = 12,
	.def = 6,
	.step= 1,
	.val = -1,
};


/*
struct uvc_capability roll = {
	.len = 2,
	.min = 0,
	.max = 1,
	.def = 0,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};
*/


/*pantilt, tilt(4byte)+pan(4byte)*/
struct uvc_capability tilt_roi = {
       .len = 8,
       .min = 0,
       .max = 0x0002B110,
       .def = 0x15F90,
       .step= 1,
       .val = -1,
};
struct uvc_capability pan_roi = {
       .len = 8,
       .min = 0,
       .max = 0x0002B110,
       .def = 0x15F90,
       .step= 1,
       .val = -1,
};
struct uvc_capability pantilt_roi = {
       .len = 8,
       .min = 0,
       .max = 0x0002B110,
       .def = 0x15F90,
       .step= 1,
       .val = -1,
       .val2 = -1,
};

struct uvc_capability tilt = {
       .len = 8,
       .min = 0,
       .max = 0x0002B110,
       .def = 0x15F90,
       .step= 1,
       .val = -1,
};
struct uvc_capability pan = {
       .len = 8,
       .min = 0,
       .max = 0x0002B110,
       .def = 0x15F90,
       .step= 1,
       .val = -1,
};
struct uvc_capability pantilt = {
       .len = 8,
       .min = 0,
       .max = 0x0002B110,
       .def = 0x15F90,
       .step= 1,
       .val = -1,
       .val2 = -1,
};


#define UVC_XU_FOCUS_ONE_PUSH_CONTROL	0x01
struct uvc_capability one_push_af = {
	.len = 2,
	.min = 0,
	.max = 1,
	.def = 0,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};

static int bInitial = 0;

static int is_uav_mode(){
       FILE *file;

       file = fopen("/tmp/1011.conf", "r");
       if(file !=NULL) {
         fclose(file);
         return 1;
       }

       return 0;
}
static int is_roi(){
	  /*check aac or pcm*/
	FILE *file;
	//char buffer[10];

	file = fopen("/tmp/roi.conf", "r");
	if(file !=NULL) {
	  fclose(file);
	  return 1;
	}

	return 0;
}

const struct uvc_format_info *
get_uvc_formats(struct uvc_device *dev, unsigned int *nformats)
{
	const struct uvc_format_info *formats;
	printf("%s, dev->bulk:%d\n", __func__, dev->bulk);

	if (dev->format == UVC_FORMAT_AUTO) {
		if (dev->speed == USB_SPEED_SUPER) {
			if(dev->bulk){
				*nformats = ARRAY_SIZE(ss_uvc_formats_bulk);
				formats = ss_uvc_formats_bulk;
			}else if(is_roi()){
				*nformats = ARRAY_SIZE(ss_uvc_formats_roi);
				formats = ss_uvc_formats_roi;

			}else{
				*nformats = ARRAY_SIZE(ss_uvc_formats);
				formats = ss_uvc_formats;
			}
		 } else {
			if(dev->bulk){
				*nformats = ARRAY_SIZE(uvc_formats_bulk);
				formats = uvc_formats_bulk;
			}else if(is_roi()){
				*nformats = ARRAY_SIZE(ss_uvc_formats_roi);
				formats = ss_uvc_formats_roi;
			}else{
				*nformats = ARRAY_SIZE(uvc_formats);
				formats = uvc_formats;
			}
		}
	} else {
		if (dev->speed == USB_SPEED_SUPER) {
			switch(dev->format) {
			case UVC_FORMAT_YUV:
				*nformats = 1;
				if(dev->bulk)
					formats = &ss_uvc_formats_bulk[1];
				else if(is_roi())
					printf("Warning: there is no YUV setting for roi\n");
				else
					formats = &ss_uvc_formats[1];
				break;
			case UVC_FORMAT_NV12:
				*nformats = 1;
				if(dev->bulk)
					formats = &ss_uvc_formats_bulk[2];
				else if(is_roi())
					printf("Warning: there is no NV12 setting for roi\n");
				else
					formats = &ss_uvc_formats[2];
				break;
			case UVC_FORMAT_MJPEG:
				*nformats = 1;
				if(dev->bulk)
					formats = &ss_uvc_formats_bulk[0];
				else if(is_roi())
					printf("Warning: there is no MJPEG setting for roi\n");
				else
					formats = &ss_uvc_formats[0];
				break;
			case UVC_FORMAT_H264:
				*nformats = 1;
				if(dev->bulk)
					formats = &ss_uvc_formats_bulk[3];
				else if(is_roi())
					formats = &ss_uvc_formats_roi[0];
				else
					formats = &ss_uvc_formats[3];
				break;
			default:
				*nformats = 0;
				formats = NULL;
				break;
			}
		} else {
			switch(dev->format) {
			case UVC_FORMAT_YUV:
				*nformats = 1;
				if(dev->bulk)
					formats = &uvc_formats_bulk[1];
				else if(is_roi())
					printf("Warning: there is no YUV setting for roi\n");
				else
					formats = &uvc_formats[1];
				break;
			case UVC_FORMAT_NV12:
				*nformats = 1;
				if(dev->bulk)
					formats = &uvc_formats_bulk[3];
				else if(is_roi())
					printf("Warning: there is no NV12 setting for roi\n");
				else
					formats = &uvc_formats[3];
				break;
			case UVC_FORMAT_MJPEG:
				*nformats = 1;
				if(dev->bulk)
					formats = &uvc_formats_bulk[0];
				else if(is_roi())
					printf("Warning: there is no MJPEG setting for roi\n");
				else
					formats = &uvc_formats[0];
				break;
			case UVC_FORMAT_H264:
				*nformats = 1;
				if(dev->bulk)
					formats = &uvc_formats_bulk[2];
				else if(is_roi())
					formats = &uvc_formats_roi[0];
				else
					formats = &uvc_formats[2];
				break;
			default:
				*nformats = 0;
				formats = NULL;
				break;
			}
		}
	}

	return formats;
}

void
uvc_events_process_control_sub(struct uvc_device *dev,
			   uint8_t req,
			   uint8_t cs,
			   uint8_t entity_id,
			   uint8_t len,
			   struct uvc_request_data *resp)
{
	switch (entity_id) {
	/* Video Control Interface */
	case 0:
		switch (cs) {
		case UVC_VC_REQUEST_ERROR_CODE_CONTROL:
			debug_p(">>>> UVC_VC_REQUEST_ERROR_CODE_CONTROL\n");
			/* Send the request error code last prepared. */
			resp->data[0] = dev->request_error_code.data[0];
			resp->length = dev->request_error_code.length;
			break;

		default:
			/*
			 * If we were not supposed to handle this
			 * 'cs', prepare an error code response.
			 */
			dev->request_error_code.data[0] = 0x06;
			dev->request_error_code.length = 1;
			break;
		}
		break;

	/* Camera Terminal */
	case 1:
		switch (cs) {
		case UVC_CT_AE_MODE_CONTROL:
			debug_p(">>>> UVC_CT_AE_MODE_CONTROL\n");
			uvc_control(req, dev, resp, &ae_mode);
			break;
/*
		case UVC_CT_AE_PRIORITY_CONTROL:
			debug_p(">>>> UVC_CT_AE_PRIORITY_CONTROL\n");
			uvc_control(req, dev, resp, &ae_priority);
			break;
*/
		case UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
			debug_p(">>>> UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL\n");
			uvc_control(req, dev, resp, &exposure_time);
			break;

		case UVC_CT_ZOOM_ABSOLUTE_CONTROL:
			debug_p(">>>> UVC_CT_ZOOM_ABSOLUTE_CONTROL\n");
			uvc_control(req, dev, resp, &zoom);
			break;

		case UVC_CT_PANTILT_ABSOLUTE_CONTROL:
			debug_p(">>>> UVC_CT_PANTILT_ABSOLUTE_CONTROL\n");
			uvc_control(req, dev, resp, &pantilt);
			break;

		case UVC_CT_SCANNING_MODE_CONTROL: /* not support */
		case UVC_CT_AE_PRIORITY_CONTROL: /* not support */
		case UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL: /* not support */
		case UVC_CT_FOCUS_ABSOLUTE_CONTROL: /* not support */
		case UVC_CT_FOCUS_RELATIVE_CONTROL: /* not support */
		case UVC_CT_FOCUS_AUTO_CONTROL: /* not support */
		case UVC_CT_IRIS_ABSOLUTE_CONTROL: /* not support */
		case UVC_CT_IRIS_RELATIVE_CONTROL: /* not support */
//		case UVC_CT_ZOOM_RELATIVE_CONTROL: /* not support */
//		case UVC_CT_PANTILT_ABSOLUTE_CONTROL: /* not support */
//		case UVC_CT_PANTILT_RELATIVE_CONTROL: /* not support */
		case UVC_CT_ZOOM_RELATIVE_CONTROL:
		case UVC_CT_PANTILT_RELATIVE_CONTROL:
		case UVC_CT_ROLL_RELATIVE_CONTROL: /* not support */
		case UVC_CT_ROLL_ABSOLUTE_CONTROL: /* not support */
		case UVC_CT_PRIVACY_CONTROL: /* not support */
		default:
			resp->length = -EL2HLT;
			dev->request_error_code.data[0] = 0x06;
			dev->request_error_code.length = 1;
			break;
		}
		break;

	/* Processing Unit */
	case 2:
		switch (cs) {
		case UVC_PU_BRIGHTNESS_CONTROL:
			debug_p(">>>> UVC_PU_BRIGHTNESS_CONTROL\n");
			uvc_control(req, dev, resp, &brightness);
			break;
#if 0//rtos not ready yet.
		case UVC_PU_CONTRAST_CONTROL:
			debug_p(">>>> UVC_PU_CONTRAST_CONTROL\n");
			uvc_control(req, dev, resp, &contrast);
			break;
#endif

		case UVC_PU_HUE_CONTROL:
			debug_p(">>>> UVC_PU_HUE_CONTROL\n");
			uvc_control(req, dev, resp, &hue);
			break;

		case UVC_PU_SATURATION_CONTROL:
			debug_p(">>>> UVC_PU_SATURATION_CONTROL\n");
			uvc_control(req, dev, resp, &saturation);
			break;

		case UVC_PU_SHARPNESS_CONTROL:
			debug_p(">>>> UVC_PU_SHARPNESS_CONTROL\n");
			uvc_control(req, dev, resp, &sharpness);
			break;

		case UVC_PU_POWER_LINE_FREQUENCY_CONTROL:
			debug_p(">>>> UVC_PU_POWER_LINE_FREQUENCY_CONTROL\n");
			uvc_control(req, dev, resp, &flicker);
			break;

		case UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
			debug_p(">>>> UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL\n");
			uvc_control(req, dev, resp, &coltemper);
			break;

		case UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
			debug_p(">>>> UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL\n");
			uvc_control(req, dev, resp, &coltemper_auto);
			break;

		default:
			resp->length = -EL2HLT;
			dev->request_error_code.data[0] = 0x06;
			dev->request_error_code.length = 1;
			break;
		}

		break;

	/* Extension Unit */
	case 3:
		switch (cs) {
		case UVC_XU_FOCUS_ONE_PUSH_CONTROL:
			debug_p(">>>> UVC_XU_FOCUS_ONE_PUSH_CONTROL\n");
			uvc_control(req, dev, resp, &one_push_af);
			break;

		default:
			break;
		}
		break;

	default:
		dev->request_error_code.data[0] = 0x06;
		dev->request_error_code.length = 1;
		break;
	}
}
void
uvc_events_process_control_sub_roi(struct uvc_device *dev,
			   uint8_t req,
			   uint8_t cs,
			   uint8_t entity_id,
			   uint8_t len,
			   struct uvc_request_data *resp)
{
	switch (entity_id) {
	/* Video Control Interface */
	case 0:
		switch (cs) {
		case UVC_VC_REQUEST_ERROR_CODE_CONTROL:
			debug_p(">>>> UVC_VC_REQUEST_ERROR_CODE_CONTROL\n");
			/* Send the request error code last prepared. */
			resp->data[0] = dev->request_error_code.data[0];
			resp->length = dev->request_error_code.length;
			break;

		default:
			/*
			 * If we were not supposed to handle this
			 * 'cs', prepare an error code response.
			 */
			dev->request_error_code.data[0] = 0x06;
			dev->request_error_code.length = 1;
			break;
		}
		break;

	/* Camera Terminal */
	case 1:
		switch (cs) {
		case UVC_CT_AE_MODE_CONTROL:
			debug_p(">>>> UVC_CT_AE_MODE_CONTROL\n");
			uvc_control(req, dev, resp, &ae_mode);
			break;
/*
		case UVC_CT_AE_PRIORITY_CONTROL:
			debug_p(">>>> UVC_CT_AE_PRIORITY_CONTROL\n");
			uvc_control(req, dev, resp, &ae_priority);
			break;
*/
		case UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
			debug_p(">>>> UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL\n");
			uvc_control(req, dev, resp, &exposure_time);
			break;

		case UVC_CT_ZOOM_ABSOLUTE_CONTROL:
			debug_p(">>>> UVC_CT_ZOOM_ABSOLUTE_CONTROL\n");
			uvc_control(req, dev, resp, &zoom_roi);
			break;

		case UVC_CT_PANTILT_ABSOLUTE_CONTROL:
			debug_p(">>>> UVC_CT_PANTILT_ABSOLUTE_CONTROL\n");
			uvc_control(req, dev, resp, &pantilt_roi);
			break;

		case UVC_CT_SCANNING_MODE_CONTROL: /* not support */
		case UVC_CT_AE_PRIORITY_CONTROL: /* not support */
		case UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL: /* not support */
		case UVC_CT_FOCUS_ABSOLUTE_CONTROL: /* not support */
		case UVC_CT_FOCUS_RELATIVE_CONTROL: /* not support */
		case UVC_CT_FOCUS_AUTO_CONTROL: /* not support */
		case UVC_CT_IRIS_ABSOLUTE_CONTROL: /* not support */
		case UVC_CT_IRIS_RELATIVE_CONTROL: /* not support */
//		case UVC_CT_ZOOM_RELATIVE_CONTROL: /* not support */
//		case UVC_CT_PANTILT_ABSOLUTE_CONTROL: /* not support */
//		case UVC_CT_PANTILT_RELATIVE_CONTROL: /* not support */
		case UVC_CT_ZOOM_RELATIVE_CONTROL:
		case UVC_CT_PANTILT_RELATIVE_CONTROL:
		case UVC_CT_ROLL_RELATIVE_CONTROL: /* not support */
		case UVC_CT_ROLL_ABSOLUTE_CONTROL: /* not support */
		case UVC_CT_PRIVACY_CONTROL: /* not support */
		default:
			resp->length = -EL2HLT;
			dev->request_error_code.data[0] = 0x06;
			dev->request_error_code.length = 1;
			break;
		}
		break;

	/* Processing Unit */
	case 2:
		switch (cs) {
		case UVC_PU_BRIGHTNESS_CONTROL:
			debug_p(">>>> UVC_PU_BRIGHTNESS_CONTROL\n");
			uvc_control(req, dev, resp, &brightness);
			break;
#if 0//rtos not ready yet.
		case UVC_PU_CONTRAST_CONTROL:
			debug_p(">>>> UVC_PU_CONTRAST_CONTROL\n");
			uvc_control(req, dev, resp, &contrast);
			break;

		case UVC_PU_SHARPNESS_CONTROL:
			debug_p(">>>> UVC_PU_SHARPNESS_CONTROL\n");
			uvc_control(req, dev, resp, &sharpness);
			break;
#endif

		case UVC_PU_HUE_CONTROL:
			debug_p(">>>> UVC_PU_HUE_CONTROL\n");
			uvc_control(req, dev, resp, &hue);
			break;

		case UVC_PU_SATURATION_CONTROL:
			debug_p(">>>> UVC_PU_SATURATION_CONTROL\n");
			uvc_control(req, dev, resp, &saturation);
			break;

		case UVC_PU_POWER_LINE_FREQUENCY_CONTROL:
			debug_p(">>>> UVC_PU_POWER_LINE_FREQUENCY_CONTROL\n");
			uvc_control(req, dev, resp, &flicker);
			break;

		case UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
			debug_p(">>>> UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL\n");
			uvc_control(req, dev, resp, &coltemper);
			break;

		case UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
			debug_p(">>>> UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL\n");
			uvc_control(req, dev, resp, &coltemper_auto);
			break;

		default:
			resp->length = -EL2HLT;
			dev->request_error_code.data[0] = 0x06;
			dev->request_error_code.length = 1;
			break;
		}

		break;

	/* Extension Unit */
	case 3:
		switch (cs) {
		case UVC_XU_FOCUS_ONE_PUSH_CONTROL:
			debug_p(">>>> UVC_XU_FOCUS_ONE_PUSH_CONTROL\n");
			uvc_control(req, dev, resp, &one_push_af);
			break;

		default:
			break;
		}
		break;

	default:
		dev->request_error_code.data[0] = 0x06;
		dev->request_error_code.length = 1;
		break;
	}
}

int
uvc_events_process_control_data_sub(struct uvc_device *dev,
				uint8_t cs,
				uint8_t entity_id,
				struct uvc_request_data *data)
{
	int ret = 0;
	struct camera_if_cmd cmd = {0};
	long val = 0;
	long long llval;
	long panval = 0, tiltval = 0;
	long ae_auto_mode;
	switch (entity_id) {
	case 1:
		switch (cs) {
		case UVC_CT_AE_MODE_CONTROL:
			debug_p(">>> UVC_CT_AE_MODE_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     AE MODE VAL : 0x%lx\n", val);

			if (val & 0x02) {//auto mode, w2 3 1 1
				ae_auto_mode = 0x1;
			} else {//manual mode, w2 3 1 0
				ae_auto_mode = 0x0;
				ae_mode.val = val;
			}

			debug_p("ae_auto_mode: 0x%lx\n", ae_auto_mode);
                        cmd.cmd_set = 0x3;
                        cmd.cmd = 0x1;
                        memset(&cmd.send_param, 0, sizeof(cmd.send_param));
                        cmd.send_param[0] = ae_auto_mode;
                        if (uvc_camera_if_command(&cmd) != 0 ||
                             cmd.recv_param[0] != 0) {
                                 printf("camera IF error.\n");
                                 ret = -1;
                        } 
			break;
/*
		case UVC_CT_AE_PRIORITY_CONTROL:
			debug_p(">>> UVC_CT_AE_PRIORITY_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     AE PRIORITY VAL : 0x%lx\n", val);

			if (val >= ae_priority.min && val <= ae_priority.max) {
				cmd.cmd_set = E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = E_CAM_IF_SUB_COM_AE_METERING;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val;
				cmd.send_param[1] = 0;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					ae_priority.val = val;
			} else {
				printf("AE PRIORITY value NG.\n");
				ret = -1;
			}
			break;
*/
		case UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
			debug_p(">>> UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     EXPOSURE TIME VAL : 0x%lx\n", val);
			if (val >= exposure_time.min && val <= exposure_time.max) {
				cmd.cmd_set = 0x3; //E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = 0x8; //E_CAM_IF_SUB_COM_EXPOSURE_TIME;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.exp_time = (int)val;//100 - ((ULONG)(log(val) / log(2)));//1ms ~ 1000ms
				printf("exp time:%d \n", cmd.exp_time);
				cmd.dec_pos = IPCU_EXP_TIME;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					exposure_time.val = val;
			} else {
				printf("EXPOSURE TIME value NG.\n");
				ret = -1;
			}
			break;

		case UVC_CT_ZOOM_ABSOLUTE_CONTROL:
			printf(">>> UVC_CT_ZOOM_ABSOLUTE_CONTROL\n");
			memcpy(&val, data->data, data->length);
			printf("     ZOOM VAL : 0x%lx, changed:%lx\n", val, val*25+100);

			if (val == zoom.val)
				break;
			if (val >= zoom.min && val <= zoom.max) {
				cmd.cmd_set = 0x1;
				cmd.cmd = 0x21;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val*25+100;
				cmd.dec_pos = IPCU_DECIMAL_NAG2 ;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					zoom.val = val;
			} else {
				printf("ZOOM value NG.\n");
				ret = -1;
			}
			break;

		case UVC_CT_PANTILT_ABSOLUTE_CONTROL:
			debug_p(">>> UVC_CT_PANTILT_ABSOLUTE_CONTROL, process..\n");
			memcpy(&llval, data->data, data->length);
			debug_p("      llval : 0x%llx\n", llval);
			panval = (long)llval;
			tiltval = (long)(llval >> 32);

			debug_p("      tiltval : 0x%lx, %ld\n", tiltval, tiltval/3600);
			debug_p("      panval : 0x%lx, %ld\n", panval, panval/3600);

			/*tilt process*/
			if ((tiltval == tilt.val) || ( tiltval%3600 != 0 )){
				//printf("%s, UVC_CT_PANTILT_ABSOLUTE_CONTROL, return, tiltval:%lx\n", __func__, tiltval);
			}else if (tiltval >= tilt.min && tiltval <= tilt.max) {
				//printf("%s, UVC_CT_PANTILT_ABSOLUTE_CONTROL, tiltval:%lx(%d)\n", __func__, tiltval, tiltval/3600);
				int tmp = ((tiltval/3600)+1)*2;//0~49 to 1~50 to 2~100
				cmd.cmd_set = 0x1;
				cmd.cmd = 0x23;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.dec_pos = IPCU_DECIMAL_NAG2 ;
				cmd.send_param[0] = tmp;
				printf("TILT value :%d\n", tmp);
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					tilt.val = tiltval;
			} else {
				printf("TILT value NG.\n");
				ret = -1;
			}

			/*pan process*/
			if ((panval == pan.val) || ( panval%3600 != 0 )){
				//printf("%s, UVC_CT_PANTILT_ABSOLUTE_CONTROL, return, panval:%lx\n", __func__, panval);
			}else if (panval >= pan.min && panval <= pan.max) {
				//printf("%s, UVC_CT_PANTILT_ABSOLUTE_CONTROL, panval:%lx(%d)\n", __func__, panval, panval/3600);
				int tmp = ((panval/3600)+1)*2;//0~49 to 1~50 to 2~100
				cmd.cmd_set = 0x1;
				cmd.cmd = 0x22;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.dec_pos = IPCU_DECIMAL_NAG2 ;
				cmd.send_param[0] = tmp;
				printf("PAN value :%d\n", tmp);
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					pan.val = panval;
			} else {
				printf("PAN value NG.\n");
				ret = -1;
			}

			ret=0;

			pantilt.val = panval;
			pantilt.val2 = tiltval;
			debug_p("final      pantilt.val : %ld, pantilt.val2 : %ld\n", pantilt.val, pantilt.val2);
			debug_p("final      panval : 0x%lx, %ld\n", panval, panval/3600);
			debug_p("final      tiltval : 0x%lx, %ld\n", tiltval, tiltval/3600);

			break;
		default:
			break;
		}
		break;

	/* Processing unit */
	case 2:
		switch (cs) {
		case UVC_PU_BRIGHTNESS_CONTROL:
			debug_p(">>> UVC_PU_BRIGHTNESS_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     BRIGHTNESS VAL : %ld\n", val);

			if (val >= brightness.min && val <= brightness.max) {
				cmd.cmd_set = 0x3; //E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = 0x6; //E_CAM_IF_SUB_COM_EV;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val; 
				debug_p(" cmd.send_param[0] : 0x%08x\n", cmd.send_param[0]);
				cmd.dec_pos = IPCU_BRIGHTNESS ;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					brightness.val = val;
			} else {
				printf("BRIGHTNESS value NG.\n");
				ret = -1;
			}
			break;
#if 0
		case UVC_PU_CONTRAST_CONTROL://na
			debug_p(">>> E_CAM_IF_SUB_COM_CONTRAST\n");
			memcpy(&val, data->data, data->length);
			debug_p("     CONTRAST VAL : 0x%lx\n", val);

			if (val >= contrast.min && val <= contrast.max) {
				cmd.cmd_set = E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = E_CAM_IF_SUB_COM_CONTRAST;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					contrast.val = val;
			} else {
				printf("CONTRAST value NG.\n");
				ret = -1;
			}
			break;


		case UVC_PU_SHARPNESS_CONTROL://na
			debug_p(">>> UVC_PU_SHARPNESS_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     SHARPNESS VAL : %ld\n", sharpness.val);

			if (val >= sharpness.min && val <= sharpness.max) {
				cmd.cmd_set = E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = E_CAM_IF_SUB_COM_SHARPNESS;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val;
				cmd.send_param[1] = val;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					sharpness.val = val;
			} else {
				printf("SHARPNESS value NG.\n");
				ret = -1;
			}
			break;
#endif

		case UVC_PU_HUE_CONTROL:
			debug_p(">>> UVC_PU_HUE_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("	  HUE VAL : %ld, val:%ld\n", hue.val, val);

			if (val >= hue.min && val <= hue.max) {
				cmd.cmd_set = 0x7;
				cmd.cmd = 0xc;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));

				cmd.send_param[0] = val;
				if (uvc_camera_if_command(&cmd) != 0 ||
					cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					hue.val = val;
			} else {
				printf("HUE value NG.\n");
				ret = -1;
			}
			break;

		case UVC_PU_SATURATION_CONTROL:
			debug_p(">>> UVC_PU_SATURATION_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     SATURATION VAL : %ld\n", saturation.val);

			if (val >= saturation.min && val <= saturation.max) {
				cmd.cmd_set = 0x7;
				cmd.cmd = 0xb;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val; //0~10
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					saturation.val = val;
			} else {
				printf("SATURATION value NG.\n");
				ret = -1;
			}
			break;

		case UVC_PU_POWER_LINE_FREQUENCY_CONTROL:
			debug_p(">>> UVC_PU_POWER_LINE_FREQUENCY_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     POWER_LINE_FREQUENCY_CONTROL VAL : %ld\n", val);
			//50Hz : 0x3 0x5 0x1, 60Hz: 0x3 0x5 0x2
			if (val >= flicker.min && val <= flicker.max) {
				cmd.cmd_set = 0x3;//E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = 0x5;//E_CAM_IF_SUB_COM_FLICKER;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					flicker.val = val;
			} else {
				printf("POWER_LINE_FREQUENCY_CONTROL value NG.\n");
				ret = -1;
			}
			break;

		case UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
			debug_p(">>> UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     WHITE_BALANCE VAL : %ld\n", val);

			//1. 0x6 0x4 0x8
			//2. 12500 to 2500, step = 100
			if (val >= coltemper.min && val <= coltemper.max) {
				cmd.cmd_set = 0x6;//E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = 0x5;//E_CAM_IF_SUB_COM_WB;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val;//2500/100=25, =>0x19
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					coltemper.val = val;
			} else {
				printf("WHITE_BALANCE value NG.\n");
				ret = -1;
			}
			break;

		case UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
			debug_p(">>> UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     WHITE_BALANCE_AUTO VAL : %ld\n", coltemper_auto.val);
			//auto : 6 0 0 , 6 0 1
			if (val >= coltemper_auto.min && val <= coltemper_auto.max) {
				cmd.cmd_set = 0x6; //E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = 0x0;//E_CAM_IF_SUB_COM_WB;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = 0;//(val == ) ? 0x00 : 0x0C;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					coltemper_auto.val = val;
			} else {
				printf("WHITE_BALANCE_AUTO value NG.\n");
				ret = -1;
			}
			break;

		default:
			break;
		}
		break;

	/* Extension unit */
	case 3:
		switch (cs) {
		case UVC_XU_FOCUS_ONE_PUSH_CONTROL:
			debug_p(">>> UVC_XU_FOCUS_ONE_PUSH_CONTROL\n");
			memcpy(&val, data->data, data->length);

			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = E_CAM_IF_COM_SET_IMG_CFG;
			cmd.cmd = E_CAM_IF_SUB_COM_ADAPTIVE_AE;
			cmd.send_param[0] = 0;
			if (uvc_camera_if_command(&cmd) != 0 ||
			    cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				ret = -1;
			}
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}

	return ret;
}

int
uvc_events_process_control_data_sub_roi(struct uvc_device *dev,
				uint8_t cs,
				uint8_t entity_id,
				struct uvc_request_data *data)
{
	int ret = 0;
	struct camera_if_cmd cmd= {0};
	long val = 0;
	long long llval;
	long panval = 0, tiltval = 0;
	long ae_auto_mode;
	switch (entity_id) {
	case 1:
		switch (cs) {
		case UVC_CT_AE_MODE_CONTROL:
			debug_p(">>> UVC_CT_AE_MODE_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     AE MODE VAL : 0x%lx\n", val);

			if (val & 0x02) {//auto mode, w2 3 1 1
				ae_auto_mode = 0x1;
			} else {//manual mode, w2 3 1 0
				ae_auto_mode = 0x0;
				ae_mode.val = val;
			}

			debug_p("ae_auto_mode: 0x%lx\n", ae_auto_mode);
                        cmd.cmd_set = 0x3;
                        cmd.cmd = 0x1;
                        memset(&cmd.send_param, 0, sizeof(cmd.send_param));
                        cmd.send_param[0] = ae_auto_mode;
                        if (uvc_camera_if_command(&cmd) != 0 ||
                             cmd.recv_param[0] != 0) {
                                 printf("camera IF error.\n");
                                 ret = -1;
                        }
			break;
/*
		case UVC_CT_AE_PRIORITY_CONTROL:
			debug_p(">>> UVC_CT_AE_PRIORITY_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     AE PRIORITY VAL : 0x%lx\n", val);

			if (val >= ae_priority.min && val <= ae_priority.max) {
				cmd.cmd_set = E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = E_CAM_IF_SUB_COM_AE_METERING;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val;
				cmd.send_param[1] = 0;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					ae_priority.val = val;
			} else {
				printf("AE PRIORITY value NG.\n");
				ret = -1;
			}
			break;
*/
		case UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
			debug_p(">>> UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     EXPOSURE TIME VAL : 0x%lx\n", val);
			if (val >= exposure_time.min && val <= exposure_time.max) {
				cmd.cmd_set = 0x3; //E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = 0x8; //E_CAM_IF_SUB_COM_EXPOSURE_TIME;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.exp_time = (int)val;//100 - ((ULONG)(log(val) / log(2)));//1ms ~ 1000ms
				printf("exp time:%d \n", cmd.exp_time);
				cmd.dec_pos = IPCU_EXP_TIME;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					exposure_time.val = val;
			} else {
				printf("EXPOSURE TIME value NG.\n");
				ret = -1;
			}
			break;

		case UVC_CT_ZOOM_ABSOLUTE_CONTROL:
			printf(">>> UVC_CT_ZOOM_ABSOLUTE_CONTROL\n");
			memcpy(&val, data->data, data->length);
			printf("     ZOOM roi VAL : 0x%lx, reverse:%lx\n", val, 0xB-val);

			if (val == zoom_roi.val)
				break;
			if (val >= zoom_roi.min && val <= zoom_roi.max) {
				cmd.cmd_set = 0x1;
				cmd.cmd = 0x34;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = 0xB - val;
				cmd.dec_pos = IPCU_DECIMAL_NAG1 ;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					zoom_roi.val = val;
			} else {
				printf("ZOOM roi value NG.\n");
				ret = -1;
			}
			break;

		case UVC_CT_PANTILT_ABSOLUTE_CONTROL:
			debug_p(">>> UVC_CT_PANTILT_ABSOLUTE_CONTROL, process..\n");
			memcpy(&llval, data->data, data->length);
			debug_p("      llval : 0x%llx\n", llval);
			panval = (long)llval;
			tiltval = (long)(llval >> 32);

			debug_p("      tiltval : 0x%lx, %ld\n", tiltval, tiltval/3600);
			debug_p("      panval : 0x%lx, %ld\n", panval, panval/3600);

			/*tilt process*/
			if ((tiltval == tilt_roi.val) || ( tiltval%3600 != 0 )){
				//printf("%s, UVC_CT_PANTILT_ABSOLUTE_CONTROL, return, tiltval:%lx\n", __func__, tiltval);
			}else if (tiltval >= tilt_roi.min && tiltval <= tilt_roi.max) {
				//printf("%s, UVC_CT_PANTILT_ABSOLUTE_CONTROL, tiltval:%lx(%d)\n", __func__, tiltval, tiltval/3600);
				int tmp = ((tiltval/3600)+1)*2;//0~49 to 1~50 to 2~100
				cmd.cmd_set = 0x1;
				cmd.cmd = 0x2d;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.dec_pos = IPCU_DECIMAL_NAG2 ;
				cmd.send_param[0] = tmp;
				printf("TILT value :%d\n", tmp);
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					tilt_roi.val = tiltval;
			} else {
				printf("TILT value NG.\n");
				ret = -1;
			}

			/*pan process*/
			if ((panval == pan_roi.val) || ( panval%3600 != 0 )){
				//printf("%s, UVC_CT_PANTILT_ABSOLUTE_CONTROL, return, panval:%lx\n", __func__, panval);
			}else if (panval >= pan_roi.min && panval <= pan_roi.max) {
				//printf("%s, UVC_CT_PANTILT_ABSOLUTE_CONTROL, panval:%lx(%d)\n", __func__, panval, panval/3600);
				int tmp = ((panval/3600)+1)*2;//0~49 to 1~50 to 2~100
				cmd.cmd_set = 0x1;
				cmd.cmd = 0x2c;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.dec_pos = IPCU_DECIMAL_NAG2 ;
				cmd.send_param[0] = tmp;
				printf("PAN value :%d\n", tmp);
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					pan_roi.val = panval;
			} else {
				printf("PAN value NG.\n");
				ret = -1;
			}

			pantilt.val = panval;
			pantilt.val2 = tiltval;
			debug_p("final      pantilt.val : 0x%ld, pantilt.val2 : 0x%ld\n", pantilt.val, pantilt.val2);
			debug_p("final      panval : 0x%lx, %ld\n", panval, panval/3600);
			debug_p("final      tiltval : 0x%lx, %ld\n", tiltval, tiltval/3600);

			ret=0;
			break;
		default:
			break;
		}
		break;

	/* Processing unit */
	case 2:
		switch (cs) {
		case UVC_PU_BRIGHTNESS_CONTROL:
			debug_p(">>> UVC_PU_BRIGHTNESS_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     BRIGHTNESS VAL : %ld\n", val);

			if (val >= brightness.min && val <= brightness.max) {
				cmd.cmd_set = 0x3; //E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = 0x6; //E_CAM_IF_SUB_COM_EV;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val;
				debug_p(" cmd.send_param[0] : 0x%08x\n", cmd.send_param[0]);
				cmd.dec_pos = IPCU_BRIGHTNESS ;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					brightness.val = val;
			} else {
				printf("BRIGHTNESS value NG.\n");
				ret = -1;
			}
			break;
#if 0
		case UVC_PU_CONTRAST_CONTROL://na
			debug_p(">>> E_CAM_IF_SUB_COM_CONTRAST\n");
			memcpy(&val, data->data, data->length);
			debug_p("     CONTRAST VAL : 0x%lx\n", val);

			if (val >= contrast.min && val <= contrast.max) {
				cmd.cmd_set = E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = E_CAM_IF_SUB_COM_CONTRAST;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					contrast.val = val;
			} else {
				printf("CONTRAST value NG.\n");
				ret = -1;
			}
			break;
#endif

		case UVC_PU_HUE_CONTROL:
			debug_p(">>> UVC_PU_HUE_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("	  HUE VAL : %ld, val:%ld\n", hue.val, val);
			if (val >= hue.min && val <= hue.max) {
				cmd.cmd_set = 0x7;
				cmd.cmd = 0xc;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));

				cmd.send_param[0] = val;
				if (uvc_camera_if_command(&cmd) != 0 ||
					cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					hue.val = val;
			} else {
				printf("HUE value NG.\n");
				ret = -1;
			}
			break;

		case UVC_PU_SATURATION_CONTROL:
			debug_p(">>> UVC_PU_SATURATION_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     SATURATION VAL : %ld\n", saturation.val);

			if (val >= saturation.min && val <= saturation.max) {
				cmd.cmd_set = 0x7;
				cmd.cmd = 0xb;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val; //0~10
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					saturation.val = val;
			} else {
				printf("SATURATION value NG.\n");
				ret = -1;
			}
			break;


		case UVC_PU_SHARPNESS_CONTROL://na
			debug_p(">>> UVC_PU_SHARPNESS_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     SHARPNESS VAL : %ld\n", sharpness.val);

			if (val >= sharpness.min && val <= sharpness.max) {
				cmd.cmd_set = 0x7;
				cmd.cmd = 0x8;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = (val * 50) - 100; // 0~6 => 0~300 => -100~200
				//cmd.send_param[1] = val;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					sharpness.val = val;
			} else {
				printf("SHARPNESS value NG.\n");
				ret = -1;
			}
			break;

		case UVC_PU_POWER_LINE_FREQUENCY_CONTROL:
			debug_p(">>> UVC_PU_POWER_LINE_FREQUENCY_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     POWER_LINE_FREQUENCY_CONTROL VAL : %ld\n", flicker.val);
			//50Hz : 0x3 0x5 0x1, 60Hz: 0x3 0x5 0x2
			if (val >= flicker.min && val <= flicker.max) {
				cmd.cmd_set = 0x3;//E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = 0x5;//E_CAM_IF_SUB_COM_FLICKER;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					flicker.val = val;
			} else {
				printf("POWER_LINE_FREQUENCY_CONTROL value NG.\n");
				ret = -1;
			}
			break;

		case UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
			debug_p(">>> UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     WHITE_BALANCE VAL : %ld\n", val);

			//1. 0x6 0x4 0x8
			//2. 12500 to 2500, step = 100
			if (val >= coltemper.min && val <= coltemper.max) {
				cmd.cmd_set = 0x6;//E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = 0x5;//E_CAM_IF_SUB_COM_WB;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val;//2500/100=25, =>0x19
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					coltemper.val = val;
			} else {
				printf("WHITE_BALANCE value NG.\n");
				ret = -1;
			}
			break;

		case UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
			debug_p(">>> UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     WHITE_BALANCE_AUTO VAL : %ld\n", coltemper_auto.val);
			//auto : 6 0 0 , 6 0 1
			if (val >= coltemper_auto.min && val <= coltemper_auto.max) {
				cmd.cmd_set = 0x6; //E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = 0x0;//E_CAM_IF_SUB_COM_WB;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = 0;//(val == ) ? 0x00 : 0x0C;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					coltemper_auto.val = val;
			} else {
				printf("WHITE_BALANCE_AUTO value NG.\n");
				ret = -1;
			}
			break;

		default:
			break;
		}
		break;

	/* Extension unit */
	case 3:
		switch (cs) {
		case UVC_XU_FOCUS_ONE_PUSH_CONTROL:
			debug_p(">>> UVC_XU_FOCUS_ONE_PUSH_CONTROL\n");
			memcpy(&val, data->data, data->length);

			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = E_CAM_IF_COM_SET_IMG_CFG;
			cmd.cmd = E_CAM_IF_SUB_COM_ADAPTIVE_AE;
			cmd.send_param[0] = 0;
			if (uvc_camera_if_command(&cmd) != 0 ||
			    cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				ret = -1;
			}
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}

	return ret;
}

int
ipcu_open_sub()
{
	struct camera_if_cmd cmd;

	// Finish Boot
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_INIT_SETTING;
	cmd.cmd = E_CAM_IF_SUB_COM_FINISH_BOOT;
	memset(&cmd.send_param, 0, sizeof(cmd.send_param));
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}

	// Change IDLE Mode
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x0;
	cmd.cmd = 0xb;
	cmd.send_param[0] = 1;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}
	return 0;
}

int
ipcu_start_capturing_sub(struct ipcu_device *dev, int in_dev_cmd)
{
	unsigned int codec_type = 0;
	unsigned int capture_size = 0;
	unsigned int framerate = 0;
	unsigned int framerate_264 = 0;
	unsigned int framerate_265 = 0;
	//unsigned int bitrate[VCH_MAX] = {0, 0, 0};
	//unsigned int vch = VCH_MAIN;
	unsigned int stream_id = 0;
	struct camera_if_cmd cmd;
	if(dev->udev->rtospl == 1){
		printf("with_header:0x%x\n",dev->udev->with_header);
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x8;
		cmd.cmd = 0x4;
		cmd.send_param[0] = dev->udev->with_header;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.\n");
			return -1;
		}
		printf("header_size:0x%x\n",dev->udev->header_size);
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x8;
		cmd.cmd = 0x5;
		cmd.send_param[0] = dev->udev->header_size;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.\n");
			return -1;
		}
		printf("max_payload_size:0x%x\n",dev->udev->max_payload_size);
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x8;
		cmd.cmd = 0x6;
		cmd.send_param[0] = dev->udev->max_payload_size;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.\n");
			return -1;
		}
	}
	// select format
	printf("format set");
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x8;
	cmd.cmd = 0x7;
	switch (dev->udev->fcc){
		case V4L2_PIX_FMT_YUYV:
			cmd.send_param[0] = 0;
			printf( "YUYV : 0\n" );
			break;
		case V4L2_PIX_FMT_NV12:
			cmd.send_param[0] = 3;
			printf( "NV12 : 3\n" );
			break;
		case V4L2_PIX_FMT_H264:
			cmd.send_param[0] = 1;
			printf( "H264 : 1\n" );
			break;
		case V4L2_PIX_FMT_MJPEG:
			cmd.send_param[0] = 2;
			printf( "MJPEG : 2\n" );
			break;
		default:
			break;
	}
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}

	switch (dev->udev->interval) {
	case 10000000/60:
		printf("frame rate= 60\n");
		framerate = 600;
		framerate_264 = 0xE;
		framerate_265 = 0x9;
		break;
	case 1000000000/5994:
		printf("frame rate= 59.94\n");
		framerate = 599;
		framerate_264 = 0x2;
		framerate_265 = 0x2;
		break;
	case 10000000/30:
		printf("frame rate= 30\n");
		framerate = 300;
		framerate_264 = 0x11;
		framerate_265 = 0xC;
		break;
	case 1000000000/2997:
		printf("frame rate= 29.97\n");
		framerate = 299;
		framerate_264 = 0x4;
		framerate_265 = 0x3;
		break;
	case 1000000000/2398:
		printf("frame rate= 23.97\n");
		framerate = 239;
		framerate_264 = 0x5;
		framerate_265 = 0x4;
		break;
	case 10000000/15:
		printf("frame rate= 15\n");
		framerate = 150;
		framerate_264 = 0x14;
		framerate_265 = 0xE;
		break;
	default:
		framerate = 300;
		framerate_264 = 0x11;
		framerate_265 = 0xC;
		break;
	}

	//265 fps
	printf("interval:%d, framerate_265:%d\n",dev->udev->interval, framerate_265);
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x11;
	cmd.send_param[0] = framerate_265;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}
	//preview fps
	printf("interval:%d, framerate:%d\n",dev->udev->interval, framerate);
	memset(&cmd, 0, sizeof(cmd));
  	cmd.cmd_set = 0x1;
	cmd.cmd = 0x16;
	cmd.send_param[0] = framerate;
	cmd.dec_pos = IPCU_DECIMAL_NAG1;	  
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}
	//264 fps
	printf("interval:%d, framerate_264:%d\n",dev->udev->interval, framerate_264);
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x12;
	cmd.send_param[0] = framerate_264;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}

	//size
	if ((dev->udev->width == 3840) && (dev->udev->height == 2160)) {
		capture_size = 0x4;
	} else if ((dev->udev->width == 2560) && (dev->udev->height == 1440)) {
		capture_size = 0x1F;
	} else if ((dev->udev->width == 1920) && (dev->udev->height == 1080)) {
		capture_size = 0xB;
	} else if ((dev->udev->width == 1280) && (dev->udev->height == 720)) {
		capture_size = 0x11;
	} else if ((dev->udev->width == 720) && (dev->udev->height == 480)) {
		capture_size = 0x15;
	} else if ((dev->udev->width == 640) && (dev->udev->height == 480)) {
		capture_size = 0x16;
	} else {
		printf("size error.(%dx%d)\n", dev->udev->width, dev->udev->height);
		return -1;
	}

	switch (dev->udev->fcc){
		case V4L2_PIX_FMT_H264:
			codec_type = 1;
			stream_id = 0;

			//265 size
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x1;
			cmd.cmd = 0xe;
			cmd.send_param[0] = capture_size;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				return -1;
			}
			
			//264 size
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x1;
			cmd.cmd = 0xf;
			cmd.send_param[0] = capture_size;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				return -1;
			}

			//close 265
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x1;
			cmd.cmd = 0xe;
			cmd.send_param[0] = 0xffffffff;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				return -1;
			}

			//trigger uvc-h264
			usleep(200*1000);
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x1;
			cmd.cmd = 0x2;
			if(is_uav_mode())/*for uvc+uac*/
				cmd.send_param[0] = 0x2a;
			else
				cmd.send_param[0] = 0x22;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				return -1;
			}
			
			usleep(200*1000);
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x0;
			cmd.cmd = 0xb;
			cmd.send_param[0] = in_dev_cmd;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				return -1;
			}

			usleep(100*1000);
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x8;
			cmd.cmd = 0x3;
			cmd.send_param[0] = 1;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				return -1;
			}

			usleep(200*1000);
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x0;
			cmd.cmd = 0xb;
			cmd.send_param[0] = 0x8;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				return -1;
			}
			
			break;
		case V4L2_PIX_FMT_YUYV:
		case V4L2_PIX_FMT_NV12:
			codec_type = 3;
			stream_id = 1;

			//265 size
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x1;
			cmd.cmd = 0xe;
			cmd.send_param[0] = capture_size;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				return -1;
			}
			
			//usr0 size
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x1;
			cmd.cmd = 0xc;
			cmd.send_param[0] = capture_size;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				return -1;
			}

			//close 265
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x1;
			cmd.cmd = 0xe;
			cmd.send_param[0] = 0xffffffff;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				return -1;
			}

			//trigger yuv
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x00000000;
			cmd.cmd = 0x0000000b;
			cmd.send_param[0] = in_dev_cmd;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.(%d)\n", __LINE__);
				goto err;
			}
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x00000008;
			cmd.cmd = 0x00000000;
			cmd.send_param[0] = 0x00000001;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.(%d)\n", __LINE__);
				goto err;
			}
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x00000008;
			cmd.cmd = 0x00000003;
			cmd.send_param[0] = 0x00000001;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.(%d)\n", __LINE__);
				goto err;
			}			
			break;
		case V4L2_PIX_FMT_MJPEG:
			codec_type = 2;
			stream_id = 16;

			//265 size
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x1;
			cmd.cmd = 0xe;
			cmd.send_param[0] = capture_size;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				return -1;
			}
			
			//mjpg size
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x1;
			cmd.cmd = 0xc;
			cmd.send_param[0] = capture_size;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				return -1;
			}

			//close 265
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x1;
			cmd.cmd = 0xe;
			cmd.send_param[0] = 0xffffffff;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				return -1;
			}

			//trigger mjpg
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x00000000;
			cmd.cmd = 0x0000000b;
			cmd.send_param[0] = in_dev_cmd;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.(%d)\n", __LINE__);
				goto err;
			}
			
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x00000008;
			cmd.cmd = 0x00000002;
			cmd.send_param[0] = 0x00000001;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.(%d)\n", __LINE__);
				goto err;
			}
			
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x00000008;
			cmd.cmd = 0x00000003;
			cmd.send_param[0] = 0x00000001;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.(%d)\n", __LINE__);
				goto err;
			}			
			break;

		default:
			break;
		
	}

	printf("codec_type:%d, dev->udev->width:%d, dev->udev->height:%d\n",codec_type, dev->udev->width, dev->udev->height);
	printf("capture_size:%x, stream_id:%d\n", capture_size, stream_id);

	
	dev->stream_id = stream_id;
	debug_p("  IPCU Send Start Streamig\n");
	return 0;

err:
	debug_p("  IPCU Send Start Streamig NG\n");
	return -1;
}

int
ipcu_stop_capturing_sub(struct ipcu_device *dev)
{
	struct camera_if_cmd cmd;

	// Change IDLE Mode
	if (dev->udev->fcc == V4L2_PIX_FMT_H264) {
                memset(&cmd, 0, sizeof(cmd));
                cmd.cmd_set = 0x0;
                cmd.cmd = 0xb;
                cmd.send_param[0] = 0x9;
                if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
                        printf("camera IF error.\n");
                        goto err;
                }

		sleep(1);
	}

        memset(&cmd, 0, sizeof(cmd));
        cmd.cmd_set = 0x0;
        cmd.cmd = 0xb;
        cmd.send_param[0] = 0x1;
        if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
                        printf("camera IF error.\n");
                        goto err;
        }

	switch (dev->udev->fcc){
		case V4L2_PIX_FMT_H264: //close 264
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = 0x1;
			cmd.cmd = 0xf;
			cmd.send_param[0] = 0xffffffff;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				return -1;
			}
		break;

		case V4L2_PIX_FMT_YUYV: //close user_0
		case V4L2_PIX_FMT_NV12: //close user_0
		case V4L2_PIX_FMT_MJPEG:
			memset(&cmd, 0, sizeof(cmd));
                        cmd.cmd_set = 0x1;
                        cmd.cmd = 0xc;
                        cmd.send_param[0] = 0xffffffff;
                        if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
                                printf("camera IF error.\n");
                                return -1;
                        }

		break;

		default:
		break;
	}

	
        memset(&cmd, 0, sizeof(cmd));
        cmd.cmd_set = 0x8;
        cmd.cmd = 0x0;
        cmd.send_param[0] = 0x0;
        if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
                printf("camera IF error.\n");
                goto err;
        }

        memset(&cmd, 0, sizeof(cmd));
        cmd.cmd_set = 0x8;
        cmd.cmd = 0x1;
        cmd.send_param[0] = 0x0;
        if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
                printf("camera IF error.\n");
                goto err;
        }

        memset(&cmd, 0, sizeof(cmd));
        cmd.cmd_set = 0x8;
        cmd.cmd = 0x2;
        cmd.send_param[0] = 0x0;
        if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
                printf("camera IF error.\n");
                goto err;
        }
        memset(&cmd, 0, sizeof(cmd));
        cmd.cmd_set = 0x08;
        cmd.cmd = 0x3;
        cmd.send_param[0] = 0x0;
        if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
                printf("camera IF error.\n");
                goto err;
        }

	debug_p("  IPCU Send Stop Streamig\n");
	return 0;

err:
	debug_p("  IPCU Send Stop Streamig NG\n");
	return -1;
}

/*ROI*/
int
ipcu_start_capturing_sub_roi(struct ipcu_device *dev, int in_dev_cmd)
{

	unsigned int stream_id = 1;
	struct camera_if_cmd cmd;

	//unsigned int codec_type = 0;
	unsigned int capture_size = 0;
	unsigned int bitrate = 0;
	unsigned int framerate = 0;
	unsigned int framerate_264 = 0;
	unsigned int framerate_265 = 0;

	if(dev->udev->rtospl == 1){
		printf("with_header:0x%x\n",dev->udev->with_header);
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x8;
		cmd.cmd = 0x4;
		cmd.send_param[0] = dev->udev->with_header;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.\n");
			return -1;
		}
		printf("header_size:0x%x\n",dev->udev->header_size);
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x8;
		cmd.cmd = 0x5;
		cmd.send_param[0] = dev->udev->header_size;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.\n");
			return -1;
		}
		printf("max_payload_size:0x%x\n",dev->udev->max_payload_size);
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x8;
		cmd.cmd = 0x6;
		cmd.send_param[0] = dev->udev->max_payload_size;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.\n");
			return -1;
		}
	}
	// select format
	printf("format set");
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x8;
	cmd.cmd = 0x7;
	switch (dev->udev->fcc){
		case V4L2_PIX_FMT_YUYV:
			cmd.send_param[0] = 0;
			printf( "YUYV : 0\n" );
			break;
		case V4L2_PIX_FMT_NV12:
			cmd.send_param[0] = 3;
			printf( "NV12 : 3\n" );
			break;
		case V4L2_PIX_FMT_H264:
			cmd.send_param[0] = 1;
			printf( "H264 : 1\n" );
			break;
		case V4L2_PIX_FMT_MJPEG:
			cmd.send_param[0] = 2;
			printf( "MJPEG : 2\n" );
			break;
		default:
			break;
	}
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}

	switch (dev->udev->interval) {
	case 10000000/60:
		printf("frame rate= 60\n");
		framerate = 600;
		framerate_264 = 0xE;
		framerate_265 = 0x9;
		break;
	case 1000000000/5994:
		printf("frame rate= 59.94\n");
		framerate = 599;
		framerate_264 = 0x2;
		framerate_265 = 0x2;
		break;
	case 10000000/30:
		printf("frame rate= 30\n");
		framerate = 300;
		framerate_264 = 0x11;
		framerate_265 = 0xC;
		break;
	case 1000000000/2997:
		printf("frame rate= 29.97\n");
		framerate = 299;
		framerate_264 = 0x4;
		framerate_265 = 0x3;
		break;
	case 1000000000/2398:
		printf("frame rate= 23.97\n");
		framerate = 239;
		framerate_264 = 0x5;
		framerate_265 = 0x4;
		break;
	case 10000000/15:
		printf("frame rate= 15\n");
		framerate = 150;
		framerate_264 = 0x14;
		framerate_265 = 0xE;
		break;
	default:
		framerate = 300;
		framerate_264 = 0x11;
		framerate_265 = 0xC;
		break;
	}

	//265 fps
	printf("interval:%d, framerate_265:%d\n",dev->udev->interval, framerate_265);
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x11;
	cmd.send_param[0] = framerate_265;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}
	//preview fps
	printf("interval:%d, framerate:%d\n",dev->udev->interval, framerate);
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x16;
	cmd.send_param[0] = framerate;
	cmd.dec_pos = IPCU_DECIMAL_NAG1;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}
	//264 fps
	printf("interval:%d, framerate_264:%d\n",dev->udev->interval, framerate_264);
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x12;
	cmd.send_param[0] = framerate_264;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}

	//size
	printf("ipcu_start_capturing_sub_roi, size .(%dx%d)\n", dev->udev->width, dev->udev->height);
	if ((dev->udev->width == 3840) && (dev->udev->height == 2160)) {
		capture_size = 0x4;
	} else if ((dev->udev->width == 2560) && (dev->udev->height == 1440)) {
		capture_size = 0x1F;
	} else if ((dev->udev->width == 1920) && (dev->udev->height == 1080)) {
		capture_size = 0xB;
	} else if ((dev->udev->width == 1280) && (dev->udev->height == 720)) {
		capture_size = 0x11;
	} else if ((dev->udev->width == 720) && (dev->udev->height == 480)) {
		capture_size = 0x15;
	} else if ((dev->udev->width == 640) && (dev->udev->height == 480)) {
		capture_size = 0x16;
	} else {
		printf("size error.(%dx%d)\n", dev->udev->width, dev->udev->height);
		return -1;
	}

	if( bInitial == 0 ){
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x1;
		cmd.cmd = 0x2;
		cmd.send_param[0] = 0x44;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				goto err;
		}

		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x1;
		cmd.cmd = 0x3;
		cmd.send_param[0] = 0;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				goto err;
		}

		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x1;
		cmd.cmd = 0xc;
		cmd.send_param[0] = 0xffffffff;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				goto err;
		}

		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x1;
		cmd.cmd = 0xe;
		cmd.send_param[0] = 0xffffffff;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				goto err;
		}

		// h264.0 size,  fixed
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x1;
		cmd.cmd = 0xf;
		cmd.send_param[0] = 0x4;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				goto err;
		}

		// h264.0 fps, fixed
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x1;
		cmd.cmd = 0x12;
		cmd.send_param[0] = 0x11;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				goto err;
		}

		// h264.1 fps
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x1;
		cmd.cmd = 0x13;
		cmd.send_param[0] = 0x11; //11:30fps
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				goto err;
		}

#if 0
		// view fps
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x1;
		cmd.cmd = 0x16;
		cmd.send_param[0] = 0x41F00000;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				goto err;
		}
#endif
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x1;
		cmd.cmd = 0x19;
		cmd.send_param[0] = 0xF;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				goto err;
		}

		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x1;
		cmd.cmd = 0x20;
		cmd.send_param[0] = 0;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				goto err;
		}

		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x1;
		cmd.cmd = 0x28;
		cmd.send_param[0] = 0x2;
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.\n");
				goto err;
		}

		bInitial = 1;
	}

	// h264.1 size
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x10;
	cmd.send_param[0] = capture_size;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.\n");
			goto err;
	}

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x0;
	cmd.cmd = 0xb;
	cmd.send_param[0] = in_dev_cmd;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.\n");
			goto err;
	}

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x14;
	cmd.send_param[0] = 1;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.\n");
			goto err;
	}

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x0;
	cmd.cmd = 0xb;
	cmd.send_param[0] = 8;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.\n");
			goto err;
	}


	dev->stream_id = stream_id;
	debug_p("  IPCU Send Start Streamig\n");
	return 0;

err:
	debug_p("  IPCU Send Start Streamig NG\n");
	return -1;
}

int
ipcu_stop_capturing_sub_roi(struct ipcu_device *dev)
{
	struct camera_if_cmd cmd;

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x0;
	cmd.cmd = 0xb;
	cmd.send_param[0] = 0x1;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.\n");
			goto err;
	}

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x14;
	cmd.send_param[0] = 0;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.\n");
			goto err;
	}

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x0;
	cmd.cmd = 0xb;
	cmd.send_param[0] = 0x9;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.\n");
			goto err;
	}

	debug_p("  IPCU Send Stop Streamig\n");
	return 0;

err:
	debug_p("  IPCU Send Stop Streamig NG\n");
	return -1;
}

