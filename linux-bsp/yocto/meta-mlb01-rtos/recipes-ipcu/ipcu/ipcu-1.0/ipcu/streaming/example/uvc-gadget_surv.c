#include <string.h>
#include <errno.h>
#include <math.h>
#include "uvc-gadget_common.h"

enum {
	VCH_MAIN = 0,
	VCH_SUB1,
	VCH_SUB2,
	VCH_SUB3,
	VCH_MAX
};

static const struct uvc_frame_info uvc_frames_mjpeg[] = {
	{ "HD",    1280,  720, { 10000000/1, 0 } },
	{  0,         0,    0, { 0 } },
};

static const struct uvc_frame_info uvc_frames_h264[] = {
	{ "FHD",   1920, 1080, { 10000000/15, 0 } },
	{ "HD",    1280,  720, { 10000000/15, 0 } },
	{ "HVGAW",  640,  360, { 10000000/15, 0 } },
	{  0,         0,    0, { 0 } },
};

static const struct uvc_format_info uvc_formats[] = {
	{ V4L2_PIX_FMT_MJPEG, uvc_frames_mjpeg },
	{ V4L2_PIX_FMT_H264, uvc_frames_h264 },
};

static const struct uvc_frame_info ss_uvc_frames_mjpeg[] = {
	{ "HD",    1280,  720, { 10000000/1, 0 } },
	{  0,         0,    0, { 0 } },
};

static const struct uvc_frame_info ss_uvc_frames_h264[] = {
	{ "FHD",   1920, 1080, { 10000000/15, 0 } },
	{ "HD",    1280,  720, { 10000000/15, 0 } },
	{ "HVGAW",  640,  360, { 10000000/15, 0 } },
	{  0,         0,    0, { 0 } },
};

static const struct uvc_format_info ss_uvc_formats[] = {
	{ V4L2_PIX_FMT_MJPEG, ss_uvc_frames_mjpeg },
	{ V4L2_PIX_FMT_H264, ss_uvc_frames_h264 },
};

/* Represents a UVC based video output device */
struct uvc_capability brightness = {
	.len = 2,
	.min = 0,
	.max = 18,
	.def = 9,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};
/*
struct uvc_capability contrast = {
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
	.max = 18,
	.def = 10,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};
*/
struct uvc_capability sharpness = {
	.len = 2,
	.min = 0,
	.max = 14,
	.def = 7,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};
struct uvc_capability flicker = {
	.len = 1,
	.min = 0,
	.max = 2,
	.def = 1,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};
/*
struct uvc_capability coltemper = {
	.len = 2,
	.min = 2500,
	.max = 12500,
	.def = 4500,
	.step= 500,
	.val = UVC_VAL_EMPTY,
};
*/
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
	.max = 80000,
	.def = 300,
	.step= 1,
	.val = UVC_VAL_EMPTY
};
/*
struct uvc_capability zoom = {
	.len = 2,
	.min = 0,
	.max = 7,
	.def = 0,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};
struct uvc_capability roll = {
	.len = 2,
	.min = 0,
	.max = 1,
	.def = 0,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};
*/
#define UVC_XU_FOCUS_ONE_PUSH_CONTROL	0x01
struct uvc_capability one_push_af = {
	.len = 2,
	.min = 0,
	.max = 1,
	.def = 0,
	.step= 1,
	.val = UVC_VAL_EMPTY,
};

const struct uvc_format_info *
get_uvc_formats(struct uvc_device *dev, unsigned int *nformats)
{
	const struct uvc_format_info *formats;

	if (dev->format == UVC_FORMAT_AUTO) {
		if (dev->speed == USB_SPEED_SUPER) {
			*nformats = ARRAY_SIZE(ss_uvc_formats);
			formats = ss_uvc_formats;
		 } else {
			*nformats = ARRAY_SIZE(uvc_formats);
			formats = uvc_formats;
		}
	} else {
		if (dev->speed == USB_SPEED_SUPER) {
			switch(dev->format) {
			case UVC_FORMAT_MJPEG:
				*nformats = 1;
				formats = &ss_uvc_formats[0];
				break;
			case UVC_FORMAT_H264:
				*nformats = 1;
				formats = &ss_uvc_formats[1];
				break;
			default:
				*nformats = 0;
				formats = NULL;
				break;
			}
		} else {
			switch(dev->format) {
			case UVC_FORMAT_MJPEG:
				*nformats = 1;
				formats = &uvc_formats[0];
				break;
			case UVC_FORMAT_H264:
				*nformats = 1;
				formats = &uvc_formats[1];
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
/*
		case UVC_CT_ZOOM_ABSOLUTE_CONTROL:
			debug_p(">>>> UVC_CT_ZOOM_ABSOLUTE_CONTROL\n");
			uvc_control(req, dev, resp, &zoom);
			break;
*/
		case UVC_CT_SCANNING_MODE_CONTROL: /* not support */
		case UVC_CT_AE_PRIORITY_CONTROL: /* not support */
		case UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL: /* not support */
		case UVC_CT_FOCUS_ABSOLUTE_CONTROL: /* not support */
		case UVC_CT_FOCUS_RELATIVE_CONTROL: /* not support */
		case UVC_CT_FOCUS_AUTO_CONTROL: /* not support */
		case UVC_CT_IRIS_ABSOLUTE_CONTROL: /* not support */
		case UVC_CT_IRIS_RELATIVE_CONTROL: /* not support */
		case UVC_CT_ZOOM_ABSOLUTE_CONTROL: /* not support */
		case UVC_CT_ZOOM_RELATIVE_CONTROL: /* not support */
		case UVC_CT_PANTILT_ABSOLUTE_CONTROL: /* not support */
		case UVC_CT_PANTILT_RELATIVE_CONTROL: /* not support */
		case UVC_CT_ROLL_RELATIVE_CONTROL: /* not support */
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
/*
		case UVC_PU_CONTRAST_CONTROL:
			debug_p(">>>> UVC_PU_CONTRAST_CONTROL\n");
			uvc_control(req, dev, resp, &contrast);
			break;

		case UVC_PU_SATURATION_CONTROL:
			debug_p(">>>> UVC_PU_SATURATION_CONTROL\n");
			uvc_control(req, dev, resp, &saturation);
			break;
*/
		case UVC_PU_SHARPNESS_CONTROL:
			debug_p(">>>> UVC_PU_SHARPNESS_CONTROL\n");
			uvc_control(req, dev, resp, &sharpness);
			break;

		case UVC_PU_POWER_LINE_FREQUENCY_CONTROL:
			debug_p(">>>> UVC_PU_POWER_LINE_FREQUENCY_CONTROL\n");
			uvc_control(req, dev, resp, &flicker);
			break;
/*
		case UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
			debug_p(">>>> UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL\n");
			uvc_control(req, dev, resp, &coltemper);
			break;

		case UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
			debug_p(">>>> UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL\n");
			uvc_control(req, dev, resp, &coltemper_auto);
			break;
*/
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
	struct camera_if_cmd cmd;
	long val = 0;

	switch (entity_id) {
	case 1:
		switch (cs) {
		case UVC_CT_AE_MODE_CONTROL:
			debug_p(">>> UVC_CT_AE_MODE_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     AE MODE VAL : 0x%lx\n", val);

			if (val & 0x02) {
				cmd.cmd_set = E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = E_CAM_IF_SUB_COM_EXPOSURE_TIME;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = 0x00;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					ae_mode.val = val;
			} else {
				ae_mode.val = val;
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
				cmd.cmd_set = E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = E_CAM_IF_SUB_COM_EXPOSURE_TIME;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = 0x11 - ((ULONG)(log(val) / log(2)));
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
/*
		case UVC_CT_ZOOM_ABSOLUTE_CONTROL:
			debug_p(">>> UVC_CT_ZOOM_ABSOLUTE_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     ZOOM VAL : 0x%lx\n", val);

			if (val == zoom.val)
				break;
			if (val >= zoom.min && val <= zoom.max) {
				cmd.cmd_set = E_UVC_IFT_COM_SET_IMG_EFFECT;
				cmd.cmd = E_UVC_IFT_SUB_COM_IMG_COLOR;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = 0;
				cmd.send_param[1] = 0;
				cmd.send_param[2] = val * 100;
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

		case UVC_CT_ROLL_ABSOLUTE_CONTROL:
			debug_p(">>> UVC_CT_ROLL_ABSOLUTE_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     ROLL VAL : 0x%lx\n", val);

			if (val == roll.val)
				break;
			if (val >= roll.min && val <= roll.max) {
				cmd.cmd_set = E_UVC_IFT_COM_SET_IMG_EFFECT;
				cmd.cmd = E_UVC_IFT_SUB_COM_IMG_ROTATE;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val;
				if (uvc_camera_if_command(&cmd) != 0 ||
				    cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
					ret = -1;
				} else
					roll.val = val;
			} else {
				printf("ROLL value NG.\n");
				ret = -1;
			}
			break;
*/
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
			debug_p("     BRIGHTNESS VAL : 0x%lx\n", val);

			if (val >= brightness.min && val <= brightness.max) {
				cmd.cmd_set = E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = E_CAM_IF_SUB_COM_EV;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = 18 - val;
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
/*
		case UVC_PU_CONTRAST_CONTROL:
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

		case UVC_PU_SATURATION_CONTROL:
			debug_p(">>> UVC_PU_SATURATION_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     SATURATION VAL : %ld\n", saturation.val);

			if (val >= saturation.min && val <= saturation.max) {
				cmd.cmd_set = E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = E_CAM_IF_SUB_COM_SATURATION;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val;
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
*/
		case UVC_PU_SHARPNESS_CONTROL:
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

		case UVC_PU_POWER_LINE_FREQUENCY_CONTROL:
			debug_p(">>> UVC_PU_POWER_LINE_FREQUENCY_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     POWER_LINE_FREQUENCY_CONTROL VAL : %ld\n", flicker.val);

			if (val >= flicker.min && val <= flicker.max) {
				cmd.cmd_set = E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = E_CAM_IF_SUB_COM_FLICKER;
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
/*
		case UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
			debug_p(">>> UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     WHITE_BALANCE VAL : %ld\n", coltemper.val);

			if (val >= coltemper.min && val <= coltemper.max) {
				cmd.cmd_set = E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = E_CAM_IF_SUB_COM_WB;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = 0x0C;
				cmd.send_param[1] = val;
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

			if (val >= coltemper_auto.min && val <= coltemper_auto.max) {
				cmd.cmd_set = E_CAM_IF_COM_SET_IMG_CFG;
				cmd.cmd = E_CAM_IF_SUB_COM_WB;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = (val == 1) ? 0x00 : 0x0C;
				cmd.send_param[1] = 6500;
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
*/
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
	cmd.cmd_set = E_CAM_IF_COM_SET_MODE_CHANGE;
	cmd.cmd = E_CAM_IF_SUB_COM_CHANGE_MODE;
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
	unsigned int framerate[VCH_MAX] = {0x09, 0x09, 0x0D, 0xFF};
	unsigned int codec_type[VCH_MAX] = {0x01, 0x01, 0x02, 0xFF};
	unsigned int bitrate[VCH_MAX] = {6000000, 3000000, 0, 0};
	unsigned int capture_size[VCH_MAX] = {0x04, 0x05, 0x05, 0x00};

	unsigned int stream_id = 0;
	int vch = VCH_MAIN;
	struct camera_if_cmd cmd;

	switch (dev->udev->fcc) {
	case V4L2_PIX_FMT_H264:
		if ((dev->udev->width == 1920) && (dev->udev->height == 1080)) {
			stream_id = 0;
			vch = VCH_MAIN;
		} else {
			stream_id = 4;
			vch = VCH_SUB1;
		}
		break;
	case V4L2_PIX_FMT_MJPEG:
		stream_id = 16;
		vch = VCH_SUB2;
		break;
	default:
		printf("codec error.(%d) %d\n", __LINE__, dev->udev->fcc);
		return -1;
	}

	if ((dev->udev->width == 1920) && (dev->udev->height == 1080)) {
		capture_size[vch] = 0x04;
		if (dev->udev->fcc == V4L2_PIX_FMT_H264) {
			bitrate[vch] = 6000000;
		}
	} else if ((dev->udev->width == 1280) && (dev->udev->height == 720)) {
		capture_size[vch] = 0x05;
		if (dev->udev->fcc == V4L2_PIX_FMT_H264) {
			bitrate[vch] = 3000000;
		}
	} else if ((dev->udev->width == 640) && (dev->udev->height == 360)) {
		capture_size[vch] = 0x07;
		if (dev->udev->fcc == V4L2_PIX_FMT_H264) {
			bitrate[vch] = 1000000;
		}
	} else {
		printf("size error.(%dx%d)\n", dev->udev->width, dev->udev->height);
		return -1;
	}

	switch (dev->udev->interval) {
	case 10000000/15:
		framerate[vch] = 0x09;
		break;
	case 10000000/1:
		framerate[vch] = 0x0D;
		break;
	default:
		printf("interval error.(%d)\n", dev->udev->interval);
		return -1;
	}

	// Change IDLE Mode
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_MODE_CHANGE;
	cmd.cmd = E_CAM_IF_SUB_COM_CHANGE_MODE;
	cmd.send_param[0] = 1;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		goto err;
	}

	for (vch = 0; vch < VCH_MAX; vch++) {
		// Set Video Capture Configuration
		// Sensor type & Framerate 
		if (codec_type[vch] != 0xFF) {
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = E_CAM_IF_COM_SET_VIDEO_CFG;
			cmd.cmd = E_CAM_IF_SUB_COM_V_SIZE_FRAMERATE;
			cmd.send_param[0] = 0x00;
			cmd.send_param[1] = vch;
			cmd.send_param[2] = framerate[vch];
			cmd.send_param[3] = codec_type[vch];
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.(%d)\n", __LINE__);
				goto err;
			}
		}

		if (codec_type[vch] <= 0x01) {
			// Bit Rate
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = E_CAM_IF_COM_SET_VIDEO_CFG;
			cmd.cmd = E_CAM_IF_SUB_COM_V_BIT_RATE;
			cmd.send_param[0] = vch;
			cmd.send_param[1] = bitrate[vch];
			cmd.send_param[2] = bitrate[vch];
			cmd.send_param[3] = 0x00;
			if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.(%d)\n", __LINE__);
				goto err;
			}
		}

		// Image Size Variation
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = E_CAM_IF_COM_SET_VIDEO_CFG;
		cmd.cmd = E_CAM_IF_SUB_COM_V_IMAGE_SIZE;
		cmd.send_param[0] = vch;
		cmd.send_param[1] = capture_size[vch];
		if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.(%d)\n", __LINE__);
			goto err;
		}
	}

	// Set ISP Option for Video
	// 3DNR
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_ISP_OPT_VIDEO;
	cmd.cmd = E_CAM_IF_SUB_COM_3DNR;
	cmd.send_param[0] = 0x01;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		goto err;
	}

	// Set ISP Option for Video
	// Video HDR
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_ISP_OPT_VIDEO;
	cmd.cmd = E_CAM_IF_SUB_COM_VIDEO_HDR;
	cmd.send_param[0] = 0x00;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		goto err;
	}

	// Change Stitching Type for Surv
	// 4sensors Camera
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x01030100;
	cmd.cmd = 0x00000100;
	cmd.send_param[0] = 0x00;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		goto err;
	}

	// Change Preview Mode
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_MODE_CHANGE;
	cmd.cmd = E_CAM_IF_SUB_COM_CHANGE_MODE;
	cmd.send_param[0] = 2;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		goto err;
	}

	// Change Video Capture Mode
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_MODE_CHANGE;
	cmd.cmd = E_CAM_IF_SUB_COM_CHANGE_MODE;
	cmd.send_param[0] = 8;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		goto err;
	}

	dev->stream_id = stream_id + dev->stream_id_delta;
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

	// Change Preview Mode
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_MODE_CHANGE;
	cmd.cmd = E_CAM_IF_SUB_COM_CHANGE_MODE;
	cmd.send_param[0] = 2;
	if (uvc_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		goto err;
	}

	// Change IDLE Mode
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_MODE_CHANGE;
	cmd.cmd = E_CAM_IF_SUB_COM_CHANGE_MODE;
	cmd.send_param[0] = 1;
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
