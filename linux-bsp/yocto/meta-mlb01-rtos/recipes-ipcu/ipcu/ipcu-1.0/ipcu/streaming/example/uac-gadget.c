/*
 * UAC gadget application
 *
 * Copyright (C) 2017 Infinitegra, Inc.
 *
 * Based on uvc-gadget.c
 * Copyright (C) 2010 Ideas on board SPRL <laurent.pinchart@ideasonboard.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 */

#include <stdio.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/eventfd.h>

#include <unistd.h>

#include "uac-gadget_common.h"

#define max(a, b)	(((a) > (b)) ? (a) : (b))

#define IPCU_RATE_MAX		48000
#define IPCU_CHANNELS_MAX	2
#define IPCU_BUFFER_SIZE	(IPCU_RATE_MAX*(16/8)*IPCU_CHANNELS_MAX)	// 1sec

#define UAC_PCM_CHANNELS	2
#define UAC_PCM_PERIOD_MS	20
#define UAC_PCM_START_THRESHOLD	100
static int log_ipcu_audio = 1; //show icpu callback only once, make sure ipcu in

/* ---------------------------------------------------------------------------
 * IPCU streaming related
 */

static int
ipcu_process_data(struct ipcu_device *dev)
{
	struct uac_device *udev = dev->udev;
	eventfd_t value;
	snd_pcm_sframes_t frames;
	snd_pcm_sframes_t frames_avail, frames_actual;
	int size_period;
	uint32_t offset = 0, remain;
	int ret;

	if (!udev->is_streaming)
		return 0;

	eventfd_read(dev->ipcu_fd, &value);

	pthread_mutex_lock(&dev->lock);

	remain = dev->buffer_size;
	frames_avail = snd_pcm_avail_update(udev->pcm_hnd);
	if(frames_avail == -EPIPE) {
		ret = snd_pcm_prepare(udev->pcm_hnd);
		debug_p("frames_avail = EPIPE -> snd_pcm_prepare() = %d (%s)\n", ret, strerror (errno));
	}
	if (frames_avail == 0) {
		ret = snd_pcm_prepare(udev->pcm_hnd);
		if(ret < 0)
			debug_p("snd_pcm_prepare : ret = %d, (%s)\n", ret, strerror (errno));
		ret = snd_pcm_start(udev->pcm_hnd);
		if(ret < 0)
			debug_p("snd_pcm_start : ret = %d, (%s)\n", ret, strerror (errno));
	}
	frames = snd_pcm_bytes_to_frames(udev->pcm_hnd, remain);
	if (frames_avail > frames)
		frames_avail = frames;

	size_period = snd_pcm_frames_to_bytes(udev->pcm_hnd, udev->pcm_period);

	while (frames_avail >= udev->pcm_period) {
		frames_actual = snd_pcm_writei(udev->pcm_hnd, dev->buffer + offset, udev->pcm_period);
		if(frames_actual != udev->pcm_period) {
			printf("snd_pcm_writei failed\n");
			break;
		} else {
			frames_avail -= udev->pcm_period;
			remain -= size_period;
			offset += size_period;
		}
	}

	if (remain > 0)
		memmove(dev->buffer, dev->buffer + offset, remain);
	dev->buffer_size = remain;

	pthread_mutex_unlock(&dev->lock);

	return 0;
}

/* ---------------------------------------------------------------------------
 * IPCU generic stuff
 */
static int is_saving(){
	/*check aac or pcm*/
	FILE *file;
	//char buffer[10];

	file = fopen("/tmp/save_uac.conf", "r");
	if(file !=NULL) {
		fclose(file);
		return 1;
	}
}
static int ipcu_callback(struct cif_stream_send *p, void *d)
{
	struct ipcu_device *dev = (struct ipcu_device *)d;
	uint32_t size, residue = 0;
	uint8_t *addr;
	bool need_notify = false;

	if (!dev || !dev->is_streaming){
		printf("audio ipcu_callback() return -1\n");
		log_ipcu_audio = 0;
		return -1;
	}
	if (p->Sub_Command != 0x00000200){
		printf("audio ipcu_callback() !=200\n");
		return -1;
	}
	if(log_ipcu_audio<=10){
		printf("audio ipcu_callback(), p->sample_size:%d\n", p->sample_size);
		log_ipcu_audio++;
	}

	size = p->sample_size;
	addr = (uint8_t *)p->sample_address;
	if(is_saving()){//save audio file, test only
		static int out_fd = -1;
		char filename[256];
		void *buf_vaddr = (UINT32 *)p->sample_address;
		sprintf(filename, "/tmp/uac_audio");
	
		out_fd = open(filename, (O_RDWR | O_CREAT | O_APPEND), 0666);
		if (out_fd < 0) {
			printf("%s: failed to open %s \n", __func__, filename);
		}
		else
		{
	
			if (write(out_fd, buf_vaddr, p->sample_size) != p->sample_size) {
				printf("%s: failed to write %s 0x%08x(%04x)\n",
				__func__, (char*)&filename,(unsigned int)buf_vaddr,
				(unsigned int)p->sample_size);
			}
			close(out_fd);
			out_fd = -1;
		}
	}

	pthread_mutex_lock(&dev->lock);

	if (dev->buffer_size + size > dev->buffer_size_max)
		residue = (dev->buffer_size + size) - dev->buffer_size_max;

	if (residue > 0) {
		memmove(dev->buffer, dev->buffer + residue, residue);
		dev->buffer_size -= residue;
	}

	memcpy(dev->buffer + dev->buffer_size, addr, size);
	dev->buffer_size += size;

	if (dev->buffer_size > snd_pcm_frames_to_bytes(dev->udev->pcm_hnd, dev->udev->pcm_period))
		need_notify = true;

	pthread_mutex_unlock(&dev->lock);

	if (need_notify)
		eventfd_write(dev->ipcu_fd, 1);

	return 0;
}

#define RMAX 4
struct timespec gs[RMAX], ge[RMAX];//performance
#if 0
static int ipcu_callback_debug(struct cif_stream_send *p, void *d)
{
	struct ipcu_device *dev = (struct ipcu_device *)d;
	uint32_t size, residue = 0;
	uint8_t *addr;
	bool need_notify = false;
	int long func_us, rap_ms;
	
	if (!dev || !dev->is_streaming)
		return -1;

	if (p->Sub_Command != 0x00000200)
		return -1;

	size = p->sample_size;
	addr = (uint8_t *)p->sample_address;

	pthread_mutex_lock(&dev->lock);

	clock_gettime(CLOCK_REALTIME, &gs[1]);	// func start

	if (dev->buffer_size + size > dev->buffer_size_max)
		residue = (dev->buffer_size + size) - dev->buffer_size_max;

	if (residue > 0) {
		memmove(dev->buffer, dev->buffer + residue, residue);
		dev->buffer_size -= residue;
	}

	memcpy(dev->buffer + dev->buffer_size, addr, size);
	dev->buffer_size += size;

	if (dev->buffer_size > snd_pcm_frames_to_bytes(dev->udev->pcm_hnd, dev->udev->pcm_period))
		need_notify = true;

	clock_gettime(CLOCK_REALTIME, &ge[1]);	// func end
	func_us = ge[1].tv_nsec - gs[1].tv_nsec;
	if( func_us < 0 ) func_us += 1000000000;
	func_us /= 1000;

	clock_gettime(CLOCK_REALTIME, &ge[0]);	// rap end
	rap_ms = ge[0].tv_nsec - gs[0].tv_nsec;
	if( rap_ms < 0 ) rap_ms += 1000000000;
	rap_ms /= 1000000;

	pthread_mutex_unlock(&dev->lock);

	debug_p("  [idx=%lu, add=%08lx, size=%lu] func=%ld us, rap=%ld ms\n",
			p->area_index,
			p->sample_address, p->sample_size,
		    func_us, rap_ms);

	clock_gettime(CLOCK_REALTIME, &gs[0]);	// rap start

	if (need_notify)
		eventfd_write(dev->ipcu_fd, 1);

	return 0;
}
#endif
static int uac_dummy_callback(struct cif_stream_send* p, void *d)
{
	return 0;
}

int uac_camera_if_command(struct camera_if_cmd *cmd)
{
	int ret;
	int retry_cnt;

#define UAC_CAMIF_RETRY_CNT	(10)
	
	printf("cmd=0x%08x, subcmd=0x%08x, para=0x%x, 0x%x, 0x%x, 0x%x\n",
		   cmd->cmd_set, cmd->cmd,
		   cmd->send_param[0], cmd->send_param[1], cmd->send_param[2], cmd->send_param[3]);

	cmd->to_sec = 5;	// timeout time
	
	for(retry_cnt = 0; retry_cnt < UAC_CAMIF_RETRY_CNT; retry_cnt++) {
		ret = camera_if_command(cmd);
		if(ret == 0)
			break;
	}
	
	if(retry_cnt > 0 )
		printf("uac_camera_if_command : retry %d times\n", retry_cnt);
	
	if(retry_cnt == UAC_CAMIF_RETRY_CNT)
		ret = -1;
	
	if(ret == 0 && cmd->recv_param[0] != 0)
		printf("cmd=0x%08x, subcmd=0x%08x, para=0x%x, 0x%x, 0x%x, 0x%x, recv_param[0]=0x%x, recv_param[1]=0x%x\n",
			  cmd->cmd_set, cmd->cmd,
			  cmd->send_param[0], cmd->send_param[1], cmd->send_param[2], cmd->send_param[3],
			  cmd->recv_param[0], cmd->recv_param[1]);
	
	return ret;
}

static int
ipcu_start_capturing(struct ipcu_device *dev)
{
	int ret;

	dev->is_streaming = true;
	dev->buffer_size = 0;
	printf("%s, dev->buffer_size_max:%d, IPCU_BUFFER_SIZE:%d\n", __func__, dev->buffer_size_max, IPCU_BUFFER_SIZE);
#if 0
                dev->pcm_rate = 48000;
                dev->pcm_period = 960;
                dev->pcm_start_threshold = 480;
                ret = uac_alsa_set_params(dev);
                if (ret < 0) {
                        printf("uac_alsa_set_params failed\n");
                        break;
                }

#endif
	ret = ipcu_start_capturing_sub(dev);
	if (ret < 0) {
		goto err;
	}

	printf("uac  IPCU Send Start Streamig\n");
	return 0;

err:
	printf("uac  IPCU Send Start Streamig NG\n");
	return -1;
}

static int
ipcu_stop_capturing(struct ipcu_device *dev)
{
	int ret;

	if (!dev->is_streaming)
		return 0;

	dev->is_streaming = false;

	ret = ipcu_stop_capturing_sub(dev);
	if (ret < 0) {
		goto err;
	}

	debug_p("  IPCU Send Stop Streamig\n");
	return 0;

err:
	debug_p("  IPCU Send Stop Streamig NG\n");
	return -1;
}
//uav mode
static int is_using_1011(){
	  /*check aac or pcm*/
	FILE *file;
	//char buffer[10];

	file = fopen("/tmp/1011.conf", "r");
	if(file !=NULL) {
	  fclose(file);
	  return 1;
	}

	return 0;
}

static int
ipcu_open(struct ipcu_device **idev, int src, int ch, int rate)
{
	struct ipcu_device *dev;
	struct cb_main_func func;
	struct ipcu_param ipcu_prm;
	int ret;
	int buffsize = 0, sampling_rate = rate;//48000;
	struct camera_if_cmd cmd;

	dev = malloc(sizeof(struct ipcu_device));
	if (!dev) {
		printf("malloc failed\n");
		goto err;
	}

	memset(dev, 0x00, sizeof(struct ipcu_device));

#if 1
	/* for encoder case, try to update sampling rate */
	if (src==2){
		memset(&cmd, 0, sizeof(cmd));
		cmd.cmd_set = 0x8000000e;
		cmd.cmd = 0x19;
		cmd.send_param[0] = 0x1;

		if (uac_camera_if_command(&cmd) != 0 || cmd.recv_param[0] == 0) {
			printf("sampling rate :%d\n", sampling_rate);
		} else {
			sampling_rate = cmd.recv_param[0];
			printf("%s, %d, update new sampling rate,  cmd.recv_param[0]:%lx, sampling_rate:%d\n", __func__, __LINE__, cmd.recv_param[0], sampling_rate);
		}
	}
#endif
	buffsize = sampling_rate*(16/8)*IPCU_CHANNELS_MAX;
	printf("%s, sampling_rate:%d, buffsize:%d\n", __func__,sampling_rate, buffsize);
	dev->buffer = malloc(buffsize);
	if (!dev->buffer) {
		printf("malloc failed\n");
		goto err;
	}

	dev->buffer_size = 0;
	dev->buffer_size_max = buffsize;

	dev->ipcu_fd = eventfd(0, EFD_NONBLOCK);
	if (dev->ipcu_fd < 0)
		goto err;

	pthread_mutex_init(&dev->lock, NULL);

	*idev = dev;

	memset(&func, 0x00, sizeof(struct cb_main_func));
	func.audio = (void*)ipcu_callback;
	func.video = (void*)uac_dummy_callback;
	func.jpeg = (void*)uac_dummy_callback;

	func.user_data = (void*)dev;

	memset(&ipcu_prm, 0x00, sizeof(struct ipcu_param));

	if(is_using_1011()){
		printf("uac ch 10, 11\n");
		ipcu_prm.chid = 10;
		ipcu_prm.ackid = 11;
	}else{
		printf("uac ch 6 7\n");
		ipcu_prm.chid = 6;
		ipcu_prm.ackid = 7;
	}

	ret = Stream_ipcu_ch_init(&func, ipcu_prm);
	if (ret < 0) {
		printf("Stream_ipcu_ch_init failed\n");
		return -1;
	}

	ret = ipcu_open_sub(src, ch, rate);
	if (ret < 0) {
		goto err_close;
	}

	return 0;

err:
	if (dev) {
		if (dev->buffer)
			free(dev->buffer);
		free(dev);
	}

err_close:
	Stream_ipcu_ch_close();

	return -1;
}

static void
ipcu_close(struct ipcu_device *dev)
{
	ipcu_stop_capturing(dev);
	pthread_mutex_destroy(&dev->lock);
	close(dev->ipcu_fd);
	free(dev->buffer);
	free(dev);
	Stream_ipcu_ch_close();
}

/* ---------------------------------------------------------------------------
 * ALSA generic stuff
 */

static int
uac_alsa_open(struct uac_device *uac, char *devname)
{
	int ret;
	ret = snd_pcm_open(&uac->pcm_hnd, devname, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
	if (ret < 0) {
		printf("snd_pcm_open failed (%s)\n", snd_strerror(ret));
		goto err;
	}

	ret = snd_pcm_hw_params_malloc(&uac->pcm_hw_params);
	if (ret < 0) {
		printf("snd_pcm_hw_params_malloc failed (%s)\n", snd_strerror(ret));
		goto err;
	}

	ret = snd_pcm_sw_params_malloc(&uac->pcm_sw_params);
	if (ret < 0) {
		printf("snd_pcm_sw_params_malloc failed (%s)\n", snd_strerror(ret));
		goto err;
	}

	ret = snd_pcm_poll_descriptors(uac->pcm_hnd, &uac->pcm_pfd, 1);
	if (ret != 1) {
		printf("snd_pcm_poll_descriptors failed\n");
		goto err;
	}

	return 0;

err:
	if (uac->pcm_sw_params)
		snd_pcm_sw_params_free(uac->pcm_sw_params);

	if (uac->pcm_hw_params)
		snd_pcm_hw_params_free(uac->pcm_hw_params);

	if (uac->pcm_hnd) {
		ret = snd_pcm_close(uac->pcm_hnd);
		if (ret < 0)
			printf("snd_pcm_close failed (%s)\n", snd_strerror(ret));
	}

	return -1;
}

static int
uac_alsa_close(struct uac_device *uac)
{
	int ret = 0;

	if (uac->pcm_hw_params)
		snd_pcm_hw_params_free(uac->pcm_hw_params);

	if (uac->pcm_hnd) {
		ret = snd_pcm_close(uac->pcm_hnd);
		if (ret < 0)
			printf("snd_pcm_close failed (%s)\n", snd_strerror(ret));
	}

	return ret;
}

static int
uac_alsa_set_params(struct uac_device *uac)
{
	unsigned int val;
	int dir;
	int ret;

	ret = snd_pcm_hw_params_any(uac->pcm_hnd, uac->pcm_hw_params);
	if (ret < 0) {
		printf("snd_pcm_hw_params_any failed (%s)\n",
			snd_strerror(ret));
		return -1;
	}

	ret = snd_pcm_hw_params_set_access(uac->pcm_hnd, uac->pcm_hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (ret < 0) {
		printf("snd_pcm_hw_params_set_access failed (%s)\n",
			snd_strerror(ret));
		return -1;
	}

	ret = snd_pcm_hw_params_set_format(uac->pcm_hnd, uac->pcm_hw_params, SND_PCM_FORMAT_S16_LE);
	if (ret < 0) {
		printf("snd_pcm_hw_params_set_format failed (%s)\n",
			snd_strerror(ret));
		return -1;
	}

	val = uac->pcm_rate;
	ret = snd_pcm_hw_params_set_rate_near(uac->pcm_hnd, uac->pcm_hw_params, &val, &dir);
	if (ret < 0) {
		printf("snd_pcm_hw_params_set_rate_near failed (%s)\n",
			snd_strerror(ret));
		return -1;
	}
	debug_p("exact sampling rate is %d, (dir=%d)\n", val, dir);

	ret = snd_pcm_hw_params_set_channels(uac->pcm_hnd, uac->pcm_hw_params, uac->pcm_channels);
	if (ret < 0) {
		printf("snd_pcm_hw_params_set_channels failed (%s)\n",
			snd_strerror(ret));
		return -1;
	}
	debug_p("  : pcm_rate=%u\n", uac->pcm_rate);
	debug_p("  : pcm_period=%ld\n", uac->pcm_period);
	debug_p("  : pcm_start_threshold=%lu\n", uac->pcm_start_threshold);

	ret = snd_pcm_hw_params_set_period_size(uac->pcm_hnd, uac->pcm_hw_params, uac->pcm_period, 0);
	if (ret < 0) {
		printf("snd_pcm_hw_params_set_period_size failed (%s)\n",
			snd_strerror(ret));
	}

	ret = snd_pcm_hw_params(uac->pcm_hnd, uac->pcm_hw_params);
	if (ret < 0) {
		printf("snd_pcm_hw_params failed (%s)\n",
			snd_strerror(ret));
		return -1;
	}

	ret = snd_pcm_sw_params_current(uac->pcm_hnd, uac->pcm_sw_params);
	if (ret < 0) {
		printf("snd_pcm_sw_params_current failed (%s)\n",
			snd_strerror(ret));
		return -1;
	}

	ret = snd_pcm_sw_params_set_start_threshold(uac->pcm_hnd, uac->pcm_sw_params, uac->pcm_start_threshold);
	if (ret < 0) {
		printf("snd_pcm_sw_params_set_start_threshold failed (%s)\n",
			snd_strerror(ret));
		return -1;
	}

	return 0;
}

/* ---------------------------------------------------------------------------
 * UAC generic stuff
 */

static void
uac_audio_stream(struct uac_device *uac, bool enable)
{
	if (uac->is_streaming && !enable) {
		snd_pcm_drop(uac->pcm_hnd);
		ioctl(uac->uac_fd, UACIOC_STREAMOFF, 0);
		uac->is_streaming = false;
	} else if (!uac->is_streaming && enable) {
		snd_pcm_prepare(uac->pcm_hnd);
		ioctl(uac->uac_fd, UACIOC_STREAMON, 0);
		uac->is_streaming = true;
	}
}

static int
uac_open(struct uac_device **uac, char *devname)
{
	struct uac_device *dev;
	int srate;
	int ret;
	dev = malloc(sizeof(struct uac_device));
	if (!dev) {
		printf("malloc failed\n");
		return -1;
	}
	memset(dev, 0x00, sizeof(struct uac_device));
	ret = uac_alsa_open(dev, devname);
	if (ret < 0){
		printf("alsa open failed, %s, %d\n", __func__, __LINE__);
		goto err;
	}

	dev->uac_fd = open("/dev/uac_gadget", O_RDWR|O_NONBLOCK);
	if (dev->uac_fd < 0)
		goto err_alsa_close;
	ret = ioctl(dev->uac_fd, UACIOC_SRATE, &srate);
	if (ret < 0)
		goto err_close;
	dev->pcm_rate = srate;
	dev->pcm_channels = UAC_PCM_CHANNELS;
	dev->pcm_period = (dev->pcm_rate * UAC_PCM_PERIOD_MS) / 1000;
	dev->pcm_start_threshold = (dev->pcm_rate * UAC_PCM_START_THRESHOLD) / 1000;
	dev->pcm_rate_current = dev->pcm_rate;
	dev->pcm_period_current = dev->pcm_period;
	uac_alsa_set_params(dev);
	*uac = dev;

	return 0;

err_close:
	close(dev->uac_fd);
err_alsa_close:
	uac_alsa_close(dev);
err:
	free(dev);
	return -1;
}

static void
uac_close(struct uac_device *dev)
{
	uac_audio_stream(dev, false);
	close(dev->uac_fd);
	uac_alsa_close(dev);
	free(dev);
}

/* ---------------------------------------------------------------------------
 * UAC Request processing
 */

static void
uac_events_process(struct uac_device *dev)
{
	struct uac_event uac_event;
	bool uac_is_streaming, ipcu_is_streaming;
	int ret;

	ret = ioctl(dev->uac_fd, UACIOC_DQEVENT, &uac_event);
	if (ret < 0) {
		printf("UACIOC_DQEVENT err: %s (%d)\n", strerror(errno), errno);
		return;
	}

	switch (uac_event.type) {
	case UAC_EVENT_CONNECT:
		printf("> UAC_EVENT_CONNECT\n");
		break;

	case UAC_EVENT_DISCONNECT:
		printf("> UAC_EVENT_DISCONNECT\n");
		ipcu_stop_capturing(dev->idev);
		uac_audio_stream(dev, false);
		break;

	case UAC_EVENT_STREAMON:
		printf("> UAC_EVENT_STREAMON\n");
#if 0
                dev->pcm_rate = 48000;
                dev->pcm_period = 960;
                dev->pcm_start_threshold = 4800;
                ret = uac_alsa_set_params(dev);
                if (ret < 0) {
                        printf("uac_alsa_set_params failed\n");
                        break;
                }

#endif
		uac_audio_stream(dev, true);
		ipcu_start_capturing(dev->idev);
		break;

	case UAC_EVENT_STREAMOFF:
		printf("> UAC_EVENT_STREAMOFF\n");
		ipcu_stop_capturing(dev->idev);
		uac_audio_stream(dev, false);
		break;

	case UAC_EVENT_SRATE:
		printf("> UAC_EVENT_SRATE\n");
		if (uac_event.u.srate == dev->pcm_rate_current)
			break;
		ipcu_is_streaming = dev->idev->is_streaming;
		uac_is_streaming = dev->is_streaming;
		sleep(1);
		if (ipcu_is_streaming){
			ipcu_stop_capturing(dev->idev);
		}
		if (uac_is_streaming){
			uac_audio_stream(dev, false);
		}
		dev->pcm_rate = uac_event.u.srate;
		dev->pcm_period = (dev->pcm_rate * UAC_PCM_PERIOD_MS) / 1000;
		dev->pcm_start_threshold = (dev->pcm_rate * UAC_PCM_START_THRESHOLD) / 1000;
		ret = uac_alsa_set_params(dev);
		if (ret < 0) {
			printf("uac_alsa_set_params failed\n");
			break;
		}
		dev->pcm_rate_current = dev->pcm_rate;
		dev->pcm_period_current = dev->pcm_period;
		printf("  : pcm_rate=%u\n", dev->pcm_rate);
		printf("  : pcm_period=%ld\n", dev->pcm_period);
		printf("  : pcm_start_threshold=%lu\n", dev->pcm_start_threshold);
		sleep(1);
		if (uac_is_streaming){
			uac_audio_stream(dev, true);
		}
		if (ipcu_is_streaming){
			ipcu_start_capturing(dev->idev);
		}
		break;
	}
}

/* ---------------------------------------------------------------------------
 * main
 */

static int app_abort = 0;

static void signal_handler(int signal)
{
	app_abort = 1;
}

static void sigterm_handler(int signal)
{
	app_abort = 1;
}

int
main(int argc, char **argv)
{
	struct uac_device *udev = NULL;
	struct ipcu_device *idev = NULL;
	struct timeval *timeout;
	struct timeval tv;
	char *uac_devname = "hw:0,0";
	fd_set fdsv, fdsu;
	int ret, nfds;
	int audio_src = -1;
	int audio_ch = -1;
	int sampling_rate = 0;
	printf("%s, %s\n", __FILE__, __func__);

	if (signal(SIGINT, signal_handler) == SIG_ERR)
		perror("signal(SIGINT)");
	if (signal(SIGTERM, sigterm_handler) == SIG_ERR)
		perror("signal(SIGTERM)");
#if 0
	while ((opt = getopt(argc, argv, "s")) != -1) {
		switch (opt) {
			case 's':
				audio_src = atoi(optarg);
			break;
			default:
			break;
		}
	}
#endif

	ret = uac_open(&udev, uac_devname);
	if (udev == NULL || ret < 0){
		printf("%s, %d\n", __func__, __LINE__);
		return 1;
	}
	audio_src = atoi(argv[1]);
	audio_ch = atoi(argv[2]);
	sampling_rate = atoi(argv[3]);
	printf("uac-gadget, audio src:%d, audio ch:%d, sampling rate:%d\n", audio_src, audio_ch, sampling_rate);

	ret = ipcu_open(&idev, audio_src, audio_ch, sampling_rate);
	if (idev == NULL || ret < 0) {
		printf("ipcu_open failed\n");
		uac_close(udev);
		return -1;
	}

	udev->idev = idev;
	idev->udev = udev;

	while (!app_abort) {
		FD_ZERO(&fdsv);
		FD_ZERO(&fdsu);

		FD_SET(udev->uac_fd, &fdsu);

		fd_set efds = fdsu;

		FD_SET(idev->ipcu_fd, &fdsv);

		/* Timeout. */
		if (udev->is_streaming) {
			tv.tv_sec = 5;
			tv.tv_usec = 0;
			timeout = &tv;
		} else
			timeout = NULL;

		nfds = max(idev->ipcu_fd, udev->uac_fd);
		ret = select(nfds + 1, &fdsv, NULL, &efds, timeout);

		if (-1 == ret) {
			printf("select error %d, %s\n",
					errno, strerror (errno));
			if (EINTR == errno)
				continue;

			break;
		}

		if (0 == ret) {
			printf("uac select timeout\n");
			continue;
		}

		if (FD_ISSET(udev->uac_fd, &efds))
			uac_events_process(udev);
		if (FD_ISSET(idev->ipcu_fd, &fdsv))
			ipcu_process_data(idev);
	}

	ipcu_close(idev);

	uac_close(udev);

	return 0;
}
