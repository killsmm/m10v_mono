#include "uac-gadget_common.h"

static int is_uav_mode(){
       FILE *file;

       file = fopen("/tmp/1011.conf", "r");
       if(file !=NULL) {
         fclose(file);
         return 1;
       }

       return 0;
}

int
ipcu_open_sub(int audio_src, int audio_ch, int sampling_rate)
{
	struct camera_if_cmd cmd;
	printf("%s, audio_src:%d\n", __func__, audio_src);
	printf("%s, audio_ch:%d\n", __func__, audio_ch);
	printf("%s, sampling_rate:%d\n", __func__, sampling_rate);


#if 1
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0xe;
	cmd.cmd = 0x8;
	cmd.send_param[0] = 0x1;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.(%d)\n", __LINE__);
			return -1;
	}

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x0;
	cmd.cmd = 0x1;
	cmd.send_param[0] = 0x1;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	sleep(3);

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x2;
	cmd.send_param[0] = 0x8;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x3;
	cmd.send_param[0] = audio_ch;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	if( audio_src==2 ){//lontium encoder card
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x0;
		cmd.cmd = 0xb;
		cmd.send_param[0] = 0xb;
		if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.(%d)\n", __LINE__);
			return -1;
		}
	} else if( audio_src == 1 ){//mic
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x0;
		cmd.cmd = 0xb;
		cmd.send_param[0] = 0x2;
		if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.(%d)\n", __LINE__);
			return -1;
		}
	}

#else
	if( audio_src==2 ){//lontium encoder card
        memset(&cmd, 0, sizeof(cmd));
        cmd.cmd_set = 0xe;
        cmd.cmd = 0x8;
        cmd.send_param[0] = 0x1;
        if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
                printf("camera IF error.(%d)\n", __LINE__);
                return -1;
        }

		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x0;
		cmd.cmd = 0x1;
		cmd.send_param[0] = 0x1;
		if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.(%d)\n", __LINE__);
			return -1;
		}

		sleep(3);

		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x1;
		cmd.cmd = 0x2;
		cmd.send_param[0] = 0x8;
		if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.(%d)\n", __LINE__);
			return -1;
		}

		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x1;
		cmd.cmd = 0x3;
		cmd.send_param[0] = audio_ch;
		if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.(%d)\n", __LINE__);
			return -1;
		}

		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x0;
		cmd.cmd = 0xb;
		cmd.send_param[0] = 0xb;
		if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.(%d)\n", __LINE__);
			return -1;
		}
	} else if( audio_src == 1 ){//mic
        memset(&cmd, 0, sizeof(cmd));
        cmd.cmd_set = 0xe;
        cmd.cmd = 0x8;
        cmd.send_param[0] = 0x1;
        if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
                printf("camera IF error.(%d)\n", __LINE__);
                return -1;
        }


		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x1;
		cmd.cmd = 0x3;
		cmd.send_param[0] = audio_ch;
		if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.(%d)\n", __LINE__);
			return -1;
		}

		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x1;
		cmd.cmd = 0x2;
		cmd.send_param[0] = 0x8;
		if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.(%d)\n", __LINE__);
			return -1;
		}

		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x0;
		cmd.cmd = 0xb;
		cmd.send_param[0] = 0x2;
		if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.(%d)\n", __LINE__);
			return -1;
		}
	} else {
		printf("audio_src:%d failed\n", audio_src);
	}
#endif
	return 0;
}

int
ipcu_start_capturing_sub(struct ipcu_device *dev)
{
	struct camera_if_cmd cmd;
	printf("uac %s, %d\n", __func__, __LINE__);

#if 0
	unsigned int  sampling_rate;
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
#endif

#if 0 //lontium encoder card, rtos need to change (user_yuvin_view.c).audio.codec_type
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x0;
	cmd.cmd = 0x1;
	cmd.send_param[0] = 0x1;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}
	sleep(2);

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set 	= 0x1;
	cmd.cmd			= 0x2;
	cmd.send_param[0] = 0x89;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x18;
	cmd.send_param[0] = 0x6;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}


	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x3;
	cmd.send_param[0] = 0x2;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x0;
	cmd.cmd = 0xb;
	cmd.send_param[0] = 0xb;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}
#endif

	if(is_uav_mode()){
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x0;
		cmd.cmd = 0xb;
		cmd.send_param[0] = 0x2;
		if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
			printf("camera IF error.(%d)\n", __LINE__);
			return -1;
		}
		sleep(1);
	}
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x0;
	cmd.cmd = 0xb;
	cmd.send_param[0] = 0x8;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	printf("%s, %d\n", __func__, __LINE__);
	return 0;
}

int
ipcu_stop_capturing_sub(struct ipcu_device *dev)
{
	struct camera_if_cmd cmd;
	printf("uac %s\n", __func__);
	// Change Idle Mode
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x0;
	cmd.cmd = 0xb;
	cmd.send_param[0] = 0x9;
	if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.(%d)\n", __LINE__);
		return -1;
	}

	return 0;
}
