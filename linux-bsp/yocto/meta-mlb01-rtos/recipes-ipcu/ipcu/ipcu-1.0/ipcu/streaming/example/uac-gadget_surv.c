#include "uac-gadget_common.h"

enum {
	VCH_MAIN = 0,
	VCH_SUB1,
	VCH_SUB2,
	VCH_SUB3,
	VCH_MAX
};

int
ipcu_open_sub()
{
	struct camera_if_cmd cmd;

	// Finish Boot
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_INIT_SETTING;
	cmd.cmd = E_CAM_IF_SUB_COM_FINISH_BOOT;
	memset(&cmd.send_param, 0, sizeof(cmd.send_param));
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}

	// Change IDLE Mode
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_MODE_CHANGE;
	cmd.cmd = E_CAM_IF_SUB_COM_CHANGE_MODE;
	cmd.send_param[0] = 1;
	cmd.to_sec = 10;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}

	return 0;
}

int
ipcu_start_capturing_sub(struct ipcu_device *dev)
{
	struct camera_if_cmd cmd;
	unsigned int  sampling_rate;
	unsigned int framerate[VCH_MAX] = {0x09, 0xFF, 0x0D, 0xFF};
	unsigned int codec_type[VCH_MAX] = {0x01, 0xFF, 0x02, 0xFF};
	unsigned int average_bitrate[VCH_MAX] = {14000000, 0, 0, 0};
	unsigned int bitrate[VCH_MAX] = {15000000, 0, 0, 0};
	unsigned int bitrate_algorithm[VCH_MAX] = {0x00, 0x00, 0x00, 0x00};
	unsigned int capture_size[VCH_MAX] = {0x04, 0x00, 0x05, 0x00};

	int vch = VCH_MAIN;

	switch (dev->udev->pcm_rate_current) {
	case 8000:
		sampling_rate = 0;
		break;
	case 12000:
		sampling_rate = 1;
		break;
	case 16000:
		sampling_rate = 2;
		break;
	case 24000:
		sampling_rate = 3;
		break;
	case 32000:
		sampling_rate = 4;
		break;
	case 48000:
		sampling_rate = 6;
		break;
	default:
		return -1;
	}

	// Change Idle Mode
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_MODE_CHANGE;
	cmd.cmd = E_CAM_IF_SUB_COM_CHANGE_MODE;
	cmd.send_param[0] = 1;
	cmd.to_sec = 10;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
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
			cmd.to_sec = 3;
			if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.(%d)\n", __LINE__);
				return -1;
			}
		}

		if (codec_type[vch] <= 0x01) {
			// Bit Rate
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = E_CAM_IF_COM_SET_VIDEO_CFG;
			cmd.cmd = E_CAM_IF_SUB_COM_V_BIT_RATE;
			cmd.send_param[0] = vch;
			cmd.send_param[1] = average_bitrate[vch];
			cmd.send_param[2] = bitrate[vch];
			cmd.send_param[3] = bitrate_algorithm[vch];
			cmd.to_sec = 3;
			if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
				printf("camera IF error.(%d)\n", __LINE__);
				return -1;
			}
		}

		// Image Size Variation
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = E_CAM_IF_COM_SET_VIDEO_CFG;
		cmd.cmd = E_CAM_IF_SUB_COM_V_IMAGE_SIZE;
		cmd.send_param[0] = vch;
		cmd.send_param[1] = capture_size[vch];
		cmd.to_sec = 3;
		if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.(%d)\n", __LINE__);
			return -1;
		}
	}

	// Set ISP Option for Video
	// 3DNR
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_ISP_OPT_VIDEO;
	cmd.cmd = E_CAM_IF_SUB_COM_3DNR;
	cmd.send_param[0] = 0x01;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	// Set ISP Option for Video
	// Video HDR
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_ISP_OPT_VIDEO;
	cmd.cmd = E_CAM_IF_SUB_COM_VIDEO_HDR;
	cmd.send_param[0] = 0x00;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	// Codec Mode
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_AUDIO_CFG;
	cmd.cmd = E_CAM_IF_SUB_COM_A_CODEC;
	cmd.send_param[0] = 0x00;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	// Sampling Rate
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_AUDIO_CFG;
	cmd.cmd = E_CAM_IF_SUB_COM_A_SAMPLE_RATE;
	cmd.send_param[0] = sampling_rate;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	// Channel Type
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_AUDIO_CFG;
	cmd.cmd = E_CAM_IF_SUB_COM_A_CH_TYPE;
	cmd.send_param[0] = 0x00;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	// Mic Selection
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_AUDIO_CFG;
	cmd.cmd = E_CAM_IF_SUB_COM_A_MIC_SELECT;
	cmd.send_param[0] = 0x00;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	// Audio Enable ON
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_AUDIO_CFG;
	cmd.cmd = E_CAM_IF_SUB_COM_A_ENABLE;
	cmd.send_param[0] = 1;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	// Change Preview Mode
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_MODE_CHANGE;
	cmd.cmd = E_CAM_IF_SUB_COM_CHANGE_MODE;
	cmd.send_param[0] = 2;
	cmd.to_sec = 10;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	// Change Video Capture Mode
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_MODE_CHANGE;
	cmd.cmd = E_CAM_IF_SUB_COM_CHANGE_MODE;
	cmd.send_param[0] = 8;
	cmd.to_sec = 10;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	return 0;
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
	cmd.to_sec = 10;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	// Change Idle Mode
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_MODE_CHANGE;
	cmd.cmd = E_CAM_IF_SUB_COM_CHANGE_MODE;
	cmd.send_param[0] = 1;
	cmd.to_sec = 10;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	return 0;
}
