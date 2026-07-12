/*
 * UVC gadget test application
 *
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

#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <getopt.h>

#include <linux/usb/ch9.h>
#include <linux/usb/video.h>
#include <linux/videodev2.h>
#include <linux/limits.h>

#include "uvc.h"
#include "cif_stream.h"

static int bInitial; 

#if 1

struct uvc15_streaming_control {
	__u16 bmHint;
	__u8  bFormatIndex;
	__u8  bFrameIndex;
	__u32 dwFrameInterval;
	__u16 wKeyFrameRate;
	__u16 wPFrameRate;
	__u16 wCompQuality;
	__u16 wCompWindowSize;
	__u16 wDelay;
	__u32 dwMaxVideoFrameSize;
	__u32 dwMaxPayloadTransferSize;
	__u32 dwClockFrequency;
	__u8  bmFramingInfo;
	__u8  bPreferedVersion;
	__u8  bMinVersion;
	__u8  bMaxVersion;
	__u8  bUsage;
	__u8  bBitDepthLuma;
	__u8  bmSettings;
	__u8  bMaxNumberOfRefFramesPlus1;
	__u16 bmRateControlModes;
	__u64 bmLayoutPerStream;
} __attribute__((__packed__));
#endif

/* Enable debug prints. */
#undef ENABLE_BUFFER_DEBUG
#undef ENABLE_USB_REQUEST_DEBUG
#define DEBUG_PRINT

#ifdef DEBUG_PRINT
#define debug_p printf
#else
#define debug_p 1 ? (void) 0 : printf
#endif /* DEBUG_PRINT */

#define CLEAR(x)	memset (&(x), 0, sizeof (x))
#define max(a, b)	(((a) > (b)) ? (a) : (b))

#define clamp(val, min, max) ({                 \
        typeof(val) __val = (val);              \
        typeof(min) __min = (min);              \
        typeof(max) __max = (max);              \
        (void) (&__val == &__min);              \
        (void) (&__val == &__max);              \
        __val = __val < __min ? __min: __val;   \
        __val > __max ? __max: __val; })

#define ARRAY_SIZE(a)	((sizeof(a) / sizeof(a[0])))

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

//Command Set
typedef enum {
	E_UVC_IFT_COM_SET_PRE_CFG		= 0x00010C00,
} E_UVC_IFT_COM_SET_IFT;

/*
 * The UVC webcam gadget kernel driver (g_webcam.ko) supports changing
 * the Brightness attribute of the Processing Unit (PU). by default. If
 * the underlying video capture device supports changing the Brightness
 * attribute of the image being acquired (like the Virtual Video, VIVI
 * driver), then we should route this UVC request to the respective
 * video capture device.
 *
 * Incase, there is no actual video capture device associated with the
 * UVC gadget and we wish to use this application as the final
 * destination of the UVC specific requests then we should return
 * pre-cooked (static) responses to GET_CUR(BRIGHTNESS) and
 * SET_CUR(BRIGHTNESS) commands to keep command verifier test tools like
 * UVC class specific test suite of USBCV, happy.
 *
 * Note that the values taken below are in sync with the VIVI driver and
 * must be changed for your specific video capture device. These values
 * also work well in case there in no actual video capture device.
 */

/* ---------------------------------------------------------------------------
 * Generic stuff
 */

/* IO methods supported */
enum io_method {
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR,
};

/* ---------------------------------------------------------------------------
 * IPCU specific stuff
 */

struct ipcu_buffer {
	struct cif_stream_send buf;
	TAILQ_ENTRY(ipcu_buffer) entries;
};

/* ---------------------------------------------------------------------------
 * UVC specific stuff
 */

struct uvc_frame_info {
	char *name;
	unsigned int width;
	unsigned int height;
	unsigned int intervals[8];
};

struct uvc_format_info {
	unsigned int fcc;
	const struct uvc_frame_info *frames;
};

static const struct uvc_frame_info uvc_frames_h264[] = {
	{ "UHD",   3840, 2160, { 10000000/30, 0 } },
	{ "FHD",   1920, 1080, { 10000000/30, 0 } },
	{ "HD",    1280,  720, { 10000000/30, 0 } },
	{ "VGA",    640,  480, { 10000000/30, 0 } },
	{  0,         0,    0, { 0 } },
};

static const struct uvc_frame_info ss_uvc_frames_h264[] = {
	{ "UHD",   3840, 2160, { 10000000/30, 0 } },
	{ "FHD",   1920, 1080, { 10000000/30, 0 } },
	{ "HD",    1280,  720, { 10000000/30, 0 } },
	{ "VGA",    640,  480, { 10000000/30, 0 } },
	{  0,         0,    0, { 0 } },
};

/* Represents a UVC based video output device */
struct capability {
	int len;
	unsigned long max;
	unsigned long min;
	unsigned long def;
	unsigned long step;
	long val;
};
struct capability coltemper = {
	.len = 2,
	.min = 1000,
	.max = 16000,
	.def = 4500,
	.step= 1000,
	.val = -1,
};
struct capability brightness = {
	.len = 2,
	.min = 0,
	.max = 18,
	.def = 9,
	.step= 1,
	.val = -1,
};
struct capability saturation = {
	.len = 2,
	.min = 0,
	.max = 200,
	.def = 50,
	.step= 1,
	.val = -1,
};
struct capability sharpness = {
	.len = 2,
	.min = 0,
	.max = 8,
	.def = 4,
	.step= 1,
	.val = -1,
};
struct capability gamma1 = {
	.len = 2,
	.min = 0,
	.max = 200,
	.def = 50,
	.step= 2,
	.val = -1,
};
struct capability ae_priority = {
	.len = 1,
	.min = 0,
	.max = 2,
	.def = 0,
	.step= 1,
	.val = -1
};
struct capability zoom = {
	.len = 2,
	.min = 0,
	.max = 7,
	.def = 0,
	.step= 1,
	.val = -1,
};
struct capability roll = {
	.len = 2,
	.min = 0,
	.max = 3,
	.def = 0,
	.step= 1,
	.val = -1,
};

#define UVC_XU_FOCUS_ONE_PUSH_CONTROL	0x01
struct capability one_push_af = {
	.len = 2,
	.min = 0,
	.max = 1,
	.def = 0,
	.step= 1,
	.val = -1,
};

/* ---------------------------------------------------------------------------
 * IPCU and UVC device instances
 */

struct ipcu_stream {
	int ipcu_fd;
	int is_streaming;
	int is_enabled;

	int ipcu_tfd;

	struct ipcu_buffer *b1, *b2;
	TAILQ_HEAD(list_free, ipcu_buffer) list_free;
	TAILQ_HEAD(list_wait, ipcu_buffer) list_wait;
	TAILQ_HEAD(list_done, ipcu_buffer) list_done;
	TAILQ_HEAD(list_later, ipcu_buffer) list_later;
	pthread_mutex_t list_lock;
	int free_later;

	/* uvc device hook */
	struct uvc_device *udev;

	int (*start_func)(struct ipcu_stream *dev);
	int (*stop_func)(struct ipcu_stream *dev);
};

struct uvc_device {
	/* uvc device specific */
	int uvc_fd;
	int is_stream_on;
	int is_streaming;
	char *uvc_devname;

	/* uvc control request specific */
	struct uvc15_streaming_control probe;
	struct uvc15_streaming_control commit;
	int control;
	struct uvc_request_data request_error_code;

	/* uvc buffer specific */
	enum io_method io;
	unsigned int nbufs;
	unsigned int fcc;
	unsigned int width;
	unsigned int height;
	unsigned int interval;
	unsigned long addr;
	unsigned long size;

	uint8_t color;
	unsigned int imgsize;

	/* USB speed specific */
	int mult;
	int burst;
	int maxpkt;
	int max_burst_size;
	int max_packet_size;
	enum usb_device_speed speed;

	/* uvc specific flags */
	int first_buffer_queued;
	int uvc_shutdown_requested;

	unsigned int dqbufs;
	unsigned int ndqbufs;

	/* uvc interface */
	int intf_control;
	int intf_streaming;

	/* uvc format */
	const struct uvc_format_info *formats;
	int nformats;
	const struct uvc_format_info *ss_formats;
	int ss_nformats;

	/* IPCU device hook */
	struct ipcu_stream *istm;

};

/* forward declarations */
static int uvc_video_stream(struct uvc_device *dev, int enable);

/* ---------------------------------------------------------------------------
 * IPCU streaming related
 */

static void
ipcu_queue_buffer(struct ipcu_stream *stm, unsigned long address)
{
	struct ipcu_buffer *b;

	pthread_mutex_lock(&stm->list_lock);
	TAILQ_FOREACH(b, &stm->list_done, entries) {
		if (b->buf.sample_address == address) {
			TAILQ_REMOVE(&stm->list_done, b, entries);
			if (!stm->free_later) {
				Stream_release(&b->buf);
				TAILQ_INSERT_TAIL(&stm->list_free, b, entries);
			} else {
				TAILQ_INSERT_TAIL(&stm->list_later, b, entries);
			}
			break;
		}
	}
	pthread_mutex_unlock(&stm->list_lock);
}

static int
ipcu_dequeue_buffer(struct ipcu_stream *stm,
		    unsigned long *address, unsigned long *size)
{
	eventfd_t value;
	struct ipcu_buffer *b;
	int ret;

	ret = eventfd_read(stm->ipcu_fd, &value);
	if (ret < 0)
		return -1;

	pthread_mutex_lock(&stm->list_lock);
	b = TAILQ_FIRST(&stm->list_wait);
	if (b == NULL) {
		pthread_mutex_unlock(&stm->list_lock);
		*address = 0;
		*size = 0;
		return 0;
	}
	TAILQ_REMOVE(&stm->list_wait, b, entries);
	TAILQ_INSERT_TAIL(&stm->list_done, b, entries);
	pthread_mutex_unlock(&stm->list_lock);

	*address = b->buf.sample_address;
	*size = b->buf.sample_size;

	return 0;
}

static int
ipcu_process_data(struct ipcu_stream *stm)
{
	struct v4l2_buffer ubuf;
	unsigned int index;
	unsigned long address, size;
	int ret;

	/* Return immediately if IPCU streaming has not yet started. */
	if (!stm->is_streaming){
		printf("%s !stm_streaming return\n", __func__);
		return 0;
	}

	for (index = 0; index < stm->udev->nbufs; index++) {
		if (stm->udev->dqbufs & (1 << index))
			break;
	}
	if (index == stm->udev->nbufs){
		printf("%s %d\n", __func__, __LINE__);
		return 0;
	}

	ret = ipcu_dequeue_buffer(stm, &address, &size);
	if (ret < 0 || address == 0)
		return ret;

	/* Queue video buffer to UVC domain. */
	memset (&ubuf, 0, sizeof(ubuf));

	ubuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ubuf.memory = V4L2_MEMORY_USERPTR;
	ubuf.m.userptr = address;
	ubuf.length = stm->udev->imgsize;
	ubuf.index = index;
	ubuf.bytesused = size;

	ret = ioctl(stm->udev->uvc_fd, VIDIOC_QBUF, &ubuf);
	if (ret < 0) {
		printf("UVC: Unable to queue buffer %d: %s (%d).\n",
			ubuf.index, strerror(errno), errno);
		ipcu_queue_buffer(stm, address);
		/* Check for a USB disconnect/shutdown event. */
		if (errno == ENODEV) {
			stm->udev->uvc_shutdown_requested = 1;
			printf("UVC: Possible USB shutdown requested from "
				"Host, seen during VIDIOC_QBUF\n");
			return 0;
		} else {
			return ret;
		}
	}

	stm->udev->dqbufs &= ~(1 << ubuf.index);
	stm->udev->ndqbufs--;

#ifdef ENABLE_BUFFER_DEBUG
	printf("Queueing buffer at UVC side = %d\n", ubuf.index);
#endif

	if (!stm->udev->first_buffer_queued) {
		uvc_video_stream(stm->udev, 1);
		stm->udev->first_buffer_queued = 1;
		stm->udev->is_streaming = 1;
	}
	return ret;
}

/* ---------------------------------------------------------------------------
 * IPCU generic stuff
 */

static int
ipcu_video_callback(struct cif_stream_send* p, void *d)
{
	struct ipcu_buffer *b;
	struct ipcu_stream **stms = (struct ipcu_stream **)d;
	int idx;
	
	idx = p->stream_id;

	//printf("gadget-m, %s, %d, idx:%d\n", __func__, __LINE__, idx);

	
	if (!stms[idx]->is_streaming || !stms[idx]->is_enabled) {
		//printf("gadget-m, %s, %d, return , idx:%d\n", __func__, __LINE__, idx);
		return -1;
	}

	pthread_mutex_lock(&stms[idx]->list_lock);
	b = TAILQ_FIRST(&stms[idx]->list_free);
	if (b == NULL) {
		pthread_mutex_unlock(&stms[idx]->list_lock);
		return -1;
	}
	TAILQ_REMOVE(&stms[idx]->list_free, b, entries);
	b->buf = *p;
	TAILQ_INSERT_TAIL(&stms[idx]->list_wait, b, entries);
	pthread_mutex_unlock(&stms[idx]->list_lock);

	eventfd_write(stms[idx]->ipcu_fd, 1);

	return 0;
}

static int
ipcu_h264_start(struct ipcu_stream *stm)
{
//	uint8_t h264_0_size, h265_fps, h264_0_fps;
        struct camera_if_cmd cmd;
//	unsigned int framerate = 0;
	printf(" %s \n" , __func__);
	if(bInitial==1){
		printf("##### %s, already initial, return\n" , __func__);
		return 0;
	}
#if 0	
	if ((stm->udev->width == 3840) && (stm->udev->height == 2160)) {
		h264_0_size = 0x04;
	} else if ((stm->udev->width == 1920) && (stm->udev->height == 1080)) {
		h264_0_size = 0x0B;
	} else if ((stm->udev->width == 1280) && (stm->udev->height == 720)) {
		h264_0_size = 0x11;
	} else if ((stm->udev->width == 640) && (stm->udev->height == 480)) {
		h264_0_size = 0x16;
	} else
		return -1;

	switch (stm->udev->interval) {
	case 10000000/60:
		h265_fps = 0x09;
		h264_0_fps = 0x0E;
		framerate = 600;
		break;
	case 10000000/30:
		h265_fps = 0x0C;
		h264_0_fps = 0x11;
		framerate = 300;
		break;
	case 10000000/24:
		h265_fps = 0x0D;
		h264_0_fps = 0x13;
		framerate = 240;
		break;
	default:
		h265_fps = 0x0C;
		h264_0_fps = 0x11;
		framerate = 300;
		break;
	}
#endif
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x01;
	cmd.cmd = 0x2;
	cmd.send_param[0] = 0x77;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("h_camera IF error.\n");
		goto err;
	}
	usleep(200*1000);

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x01;
	cmd.cmd = 0xc;
	cmd.send_param[0] = 0xffffffff;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("h_camera IF error.\n");
		goto err;
	}
        usleep(200*1000);

	//fps
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x01;
	cmd.cmd = 0xe;
	cmd.send_param[0] = 0xb;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("h_camera IF error.\n");
		goto err;
	}
        usleep(200*1000);


	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x01;
	cmd.cmd = 0xf;
	cmd.send_param[0] = 0xb;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("h_camera IF error.\n");
		goto err;
	}
        usleep(200*1000);

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x01;
	cmd.cmd = 0x10;
	cmd.send_param[0] = 0x11;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("h_camera IF error.\n");
		goto err;
	}
	usleep(200*1000);

	//bitrate
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x18;
	cmd.send_param[0] = 0x7;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}
	usleep(200*1000);

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x19;
	cmd.send_param[0] = 0x7;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}
	usleep(200*1000);


	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x1a;
	cmd.send_param[0] = 0x7;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}
	usleep(200*1000);

	//roi
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x01;
	cmd.cmd = 0x20;
	cmd.send_param[0] = 0x2;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("h_camera IF error.\n");
		goto err;
	}
	usleep(200*1000);


	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x1;
	cmd.cmd = 0x28;
	cmd.send_param[0] = 0x2;//h264_0_size;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("camera IF error.\n");
		return -1;
	}
        usleep(200*1000);

	// H264_0_FPS
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x01;
	cmd.cmd = 0x11;
	cmd.send_param[0] = 0xc;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("h_camera IF error.\n");
		goto err;
	}
	usleep(200*1000);
	// H264_1_FPS
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x01;
	cmd.cmd = 0x12;
	cmd.send_param[0] = 0x11;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("h_camera IF error.\n");
		goto err;
	}
	usleep(200*1000);
	// H264_2_FPS
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x01;
	cmd.cmd = 0x13;
	cmd.send_param[0] = 0x11;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("h_camera IF error.\n");
		goto err;
	}
	usleep(200*1000);
	// preview _FPS
	system("camera_if_direct 0x0 0x16 f30.0");
	usleep(200*1000);




	// View
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x00;
	cmd.cmd = 0x0B;
	cmd.send_param[0] = 0x02;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("h_camera IF error.\n");
		goto err;
	}
	usleep(200*1000);

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x00;
	cmd.cmd = 0x0B;
	cmd.send_param[0] = 0x08;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("h_camera IF error.\n");
		goto err;
	}
        usleep(200*1000);



	bInitial = 1;
	return 0;

err:
	return -1;
}

static int
ipcu_h264_stop(struct ipcu_stream *stm)
{
	struct camera_if_cmd cmd;

	if(bInitial==0){
		printf("##### %s, not initial, return\n" , __func__);
		return 0;
	}


	// Record Stop
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x00;
	cmd.cmd = 0x0B;
	cmd.send_param[0] = 0x09;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("h_camera IF error.\n");
		goto err;
	}
	usleep(200*1000);

	// Standby
	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd_set = 0x00;
	cmd.cmd = 0x0B;
	cmd.send_param[0] = 0x01;
	cmd.to_sec = 3;
	if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
		printf("h_camera IF error.\n");
		goto err;
	}
	usleep(200*1000);
	bInitial = 0;
	return 0;

err:
	return -1;
}

static int
ipcu_start_capturing(struct ipcu_stream *stm)
{
	int ret;

	ret = stm->start_func(stm);
	if (ret < 0) {
		goto err;
	}

	debug_p("  IPCU Send Start Streamig\n");
	return 0;

err:
	debug_p("  IPCU Send Start Streamig NG\n");
	return -1;
}

static int
ipcu_stop_capturing(struct ipcu_stream *stm)
{
	struct ipcu_buffer *b;
	int ret;

	if (!stm->is_streaming)
		return 0;

	stm->is_enabled = 0;
	stm->is_streaming = 0;

	pthread_mutex_lock(&stm->list_lock);
	if (!TAILQ_EMPTY(&stm->list_wait)) {
		TAILQ_FOREACH(b, &stm->list_wait, entries) {
			TAILQ_REMOVE(&stm->list_wait, b, entries);
			TAILQ_INSERT_TAIL(&stm->list_done, b, entries);
		}
	}
	if (!TAILQ_EMPTY(&stm->list_done)) {
		TAILQ_FOREACH(b, &stm->list_done, entries) {
			TAILQ_REMOVE(&stm->list_done, b, entries);
			Stream_release(&b->buf);
			TAILQ_INSERT_TAIL(&stm->list_free, b, entries);
		}
	}
	if (!TAILQ_EMPTY(&stm->list_later)) {
		TAILQ_FOREACH(b, &stm->list_later, entries) {
			TAILQ_REMOVE(&stm->list_later, b, entries);
			Stream_release(&b->buf);
			TAILQ_INSERT_TAIL(&stm->list_free, b, entries);
		}
	}
	pthread_mutex_unlock(&stm->list_lock);

	ret = stm->stop_func(stm);
	if (ret < 0) {
		goto err;
	}

	debug_p("  IPCU Send Stop Streamig\n");
	return 0;

err:
	debug_p("  IPCU Send Stop Streamig NG\n");
	return -1;
}

static void
ipcu_disable_capturing(struct ipcu_stream *stm)
{
	stm->is_enabled = 0;
}

static int
ipcu_stream_create(struct ipcu_stream **stmp)
{
	struct ipcu_stream *stm = NULL;
	int fd;

	fd = eventfd(0, EFD_NONBLOCK);
	if (fd < 0) {
		return -1;
	}

	stm = calloc(1, sizeof *stm);
	if (stm == NULL) {
		goto err;
	}

	stm->b1 = malloc(sizeof(struct ipcu_buffer));
	if (stm->b1 == NULL) {
		goto err;
	}
	stm->b2 = malloc(sizeof(struct ipcu_buffer));
	if (stm->b2 == NULL) {
		goto err;
	}

	TAILQ_INIT(&stm->list_free);
	TAILQ_INIT(&stm->list_wait);
	TAILQ_INIT(&stm->list_done);
	TAILQ_INIT(&stm->list_later);
	TAILQ_INSERT_TAIL(&stm->list_free, stm->b1, entries);
	TAILQ_INSERT_TAIL(&stm->list_free, stm->b2, entries);
	pthread_mutex_init(&stm->list_lock, NULL);

	stm->ipcu_tfd = timerfd_create(CLOCK_MONOTONIC, 0);
	if (stm->ipcu_tfd < 0) {
		goto err;
	}

	stm->ipcu_fd = fd;
	*stmp = stm;

	return 0;

err:
	if (stm) {
		printf("/n/n error!! /n/n");
		if (stm->b2)
			free(stm->b2);
		if (stm->b1)
			free(stm->b1);
		free(stm);
	}

	close(fd);

	return -1;
}

static void
ipcu_stream_destroy(struct ipcu_stream *stm)
{
	if (stm) {
		if (stm->b2)
			free(stm->b2);
		if (stm->b1)
			free(stm->b1);
		close(stm->ipcu_fd);
		close(stm->ipcu_tfd);
		free(stm);
	}
}

static int
ipcu_open(struct ipcu_stream ***stmsp)
{
	struct ipcu_stream **stms = NULL;
	struct cb_main_func func_p;
	struct ipcu_param ipcu_prm;
	int ret;
	printf("gadget-m, %s, %d\n", __func__, __LINE__);

	stms = (struct ipcu_stream **)calloc(1, sizeof(struct ipcu_stream *) * 3);
	if (stms == NULL) {
		printf("gadget-m, %s, %d, return\n", __func__, __LINE__);
		goto err;
	}

	//stream 2
	ret = ipcu_stream_create(&stms[2]);
	if (ret < 0) {
		goto err;
	}
	stms[2]->start_func = ipcu_h264_start;
	stms[2]->stop_func = ipcu_h264_stop;	


	//stream 0
	ret = ipcu_stream_create(&stms[0]);
	if (ret < 0) {
		goto err;
	}
	stms[0]->start_func = ipcu_h264_start;
	stms[0]->stop_func = ipcu_h264_stop;

	//stream 1
	ret = ipcu_stream_create(&stms[1]);
	if (ret < 0) {
		goto err;
	}
	stms[1]->start_func = ipcu_h264_start;
	stms[1]->stop_func = ipcu_h264_stop;




	memset(&func_p, 0x00, sizeof(func_p));
	func_p.video = (void*)ipcu_video_callback;
	func_p.user_data = (void*)stms;

	memset(&ipcu_prm, 0x00, sizeof(ipcu_prm));
	ipcu_prm.chid = 6;
	ipcu_prm.ackid = 7;
	ret = Stream_ipcu_ch_init(&func_p, ipcu_prm);
	if (ret < 0) {
		goto err;
	}

	*stmsp = stms;

	return 0;

err:
	if (stms) {
		printf("gadget-m, %s, %d ipcu stream create error\n", __func__, __LINE__);
		ipcu_stream_destroy(stms[0]);
		ipcu_stream_destroy(stms[1]);
		ipcu_stream_destroy(stms[2]);
		free(stms);
	}

	return -1;
}

static void
ipcu_close(struct ipcu_stream **stms)
{
	Stream_ipcu_ch_close();
	if (stms) {
		ipcu_stream_destroy(stms[0]);
		ipcu_stream_destroy(stms[1]);
		free(stms);
	}
}

/* ---------------------------------------------------------------------------
 * UVC generic stuff
 */

static int
uvc_video_stream(struct uvc_device *dev, int enable)
{
	int type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	int ret;

	if (!enable) {
		ret = ioctl(dev->uvc_fd, VIDIOC_STREAMOFF, &type);
		if (ret < 0) {
			printf("UVC: VIDIOC_STREAMOFF failed: %s (%d).\n",
					strerror(errno), errno);
			return ret;
		}

		printf("UVC: Stopping video stream.\n");

		return 0;
	}

	ret = ioctl(dev->uvc_fd, VIDIOC_STREAMON, &type);
	if (ret < 0) {
		printf("UVC: Unable to start streaming %s (%d).\n",
			strerror(errno), errno);
		return ret;
	}

	printf("UVC: Starting video stream.\n");

	dev->uvc_shutdown_requested = 0;

	return 0;
}

static int
uvc_open(struct uvc_device **uvc, char *devname)
{
	struct uvc_device *dev;
	struct v4l2_capability cap;
	int fd;
	int ret = -EINVAL;

	fd = open(devname, O_RDWR | O_NONBLOCK);
	if (fd == -1) {
		printf("UVC: device open failed: %s (%d).\n",
				strerror(errno), errno);
		return ret;
	}

	ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
	if (ret < 0) {
		printf("UVC: unable to query uvc device: %s (%d)\n",
				strerror(errno), errno);
		goto err;
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)) {
		printf("UVC: %s is no video output device\n", devname);
		goto err;
	}

	dev = calloc(1, sizeof *dev);
	if (dev == NULL) {
		ret = -ENOMEM;
		goto err;
	}

	printf("uvc device is %s on bus %s\n", cap.card, cap.bus_info);
	printf("uvc open succeeded, file descriptor = %d\n", fd);

	dev->uvc_fd = fd;
	*uvc = dev;

	return 0;

err:
	close(fd);
	return ret;
}

static void
uvc_close(struct uvc_device *dev)
{
	if (dev) {
		close(dev->uvc_fd);
		free(dev);
	}
}

/* ---------------------------------------------------------------------------
 * UVC streaming related
 */

static int
uvc_video_process(struct uvc_device *dev)
{
	struct v4l2_buffer ubuf;
	int ret;

	/* Return immediately if UVC video output device has not started
	 * streaming yet.
	 */
	if (!dev->is_streaming){
		printf("%s, !=streaming, return\n", __func__);
		//	return 0;
	}

	/* Prepare a v4l2 buffer to be dequeued from UVC domain. */
	CLEAR(ubuf);

	ubuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ubuf.memory = V4L2_MEMORY_USERPTR;

	/* Dequeue the spent buffer from UVC domain */
	ret = ioctl(dev->uvc_fd, VIDIOC_DQBUF, &ubuf);
	if (ret < 0){
		printf("%s, ioctl dqbuf failed, return\n", __func__);
		return ret;
	}

	dev->dqbufs |= (1 << ubuf.index);
	dev->ndqbufs++;

#ifdef ENABLE_BUFFER_DEBUG
	printf("DeQueued buffer at UVC side = %d\n", ubuf.index);
#endif

	/*
	 * If the dequeued buffer was marked with state ERROR by the
	 * underlying UVC driver gadget, do not queue the same to V4l2
	 * and wait for a STREAMOFF event on UVC side corresponding to
	 * set_alt(0). So, now all buffers pending at UVC end will be
	 * dequeued one-by-one and we will enter a state where we once
	 * again wait for a set_alt(1) command from the USB host side.
	 */
	if (ubuf.flags & V4L2_BUF_FLAG_ERROR) {
		dev->uvc_shutdown_requested = 1;
		printf("UVC: Possible USB shutdown requested from "
			"Host, seen during VIDIOC_DQBUF\n");
		return 0;
	}

	/* Queue the buffer to IPCU domain */
	ipcu_queue_buffer(dev->istm, ubuf.m.userptr);

#ifdef ENABLE_BUFFER_DEBUG
	printf("ReQueueing buffer at IPCU side = 0x%08X\n", ubuf.m.userptr);
#endif

	return 0;
}

static int
uvc_video_reqbufs(struct uvc_device *dev, int nbufs)
{
	struct v4l2_requestbuffers rb;
	unsigned int i;
	int ret;

	CLEAR(rb);

	rb.count = nbufs;
	rb.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	rb.memory = V4L2_MEMORY_USERPTR;

	ret = ioctl(dev->uvc_fd, VIDIOC_REQBUFS, &rb);
	if (ret < 0) {
		if (ret == -EINVAL)
			printf("UVC: does not support user pointer i/o\n");
		else
			printf("UVC: VIDIOC_REQBUFS error %s (%d).\n",
					strerror(errno), errno);
		goto err;
	}

	if (!rb.count)
		return 0;

	dev->nbufs = rb.count;
	dev->dqbufs = 0;
	dev->ndqbufs = 0;
	for (i = 0; i < dev->nbufs; i++) {
		dev->dqbufs |= (1 << i);
		dev->ndqbufs++;
	}
	printf("UVC: %u buffers allocated.\n", rb.count);

	return 0;

err:
	return ret;

}

/*
 * This function is called in response to either:
 * 	- A SET_ALT(interface 1, alt setting 1) command from USB host,
 * 	  if the UVC gadget supports an ISOCHRONOUS video streaming endpoint
 * 	  or,
 *
 *	- A UVC_VS_COMMIT_CONTROL command from USB host, if the UVC gadget
 *	  supports a BULK type video streaming endpoint.
 */
static int
uvc_handle_streamon_event(struct uvc_device *dev)
{
	int ret;

	ret = uvc_video_reqbufs(dev, dev->nbufs);
	if (ret < 0)
		goto err;
	dev->is_stream_on = 1;
	printf("%s, here streaming=1\n", __func__);

	ipcu_start_capturing(dev->istm);
	dev->istm->is_streaming = 1;
	dev->istm->is_enabled = 1;

	return 0;

err:
	return ret;
}

static int
uvc_handle_streamoff_event(struct uvc_device *dev)
{
	/* Disable IPCU streaming... */
	ipcu_disable_capturing(dev->istm);
	/* Stop UVC streaming... */
	dev->is_stream_on = 0;
	if (dev->is_streaming) {
		uvc_video_stream(dev, 0);
		uvc_video_reqbufs(dev, 0);
		dev->is_streaming = 0;
		dev->first_buffer_queued = 0;
	}
	/* ... and now IPCU streaming.. */
	ipcu_stop_capturing(dev->istm);

	return 0;
}

/* ---------------------------------------------------------------------------
 * UVC Request processing
 */

static void
uvc_fill_streaming_control(struct uvc_device *dev,
			   struct uvc15_streaming_control *ctrl,
			   int iframe, int iformat)
{
	const struct uvc_format_info *formats;
	const struct uvc_format_info *format;
	const struct uvc_frame_info *frame;
	unsigned int nformats, nframes;

	if (dev->speed == USB_SPEED_SUPER) {
		nformats = dev->ss_nformats;
		formats = dev->ss_formats;
	 } else {
		nformats = dev->nformats;
		formats = dev->formats;
	}
	if (iformat < 0)
		iformat = nformats + iformat;
	if (iformat < 0 || iformat >= (int)nformats)
		return;
	format = &formats[iformat];

	nframes = 0;
	while (format->frames[nframes].width != 0)
		++nframes;

	if (iframe < 0)
		iframe = nframes + iframe;
	if (iframe < 0 || iframe >= (int)nframes)
		return;
	frame = &format->frames[iframe];

	memset(ctrl, 0, sizeof *ctrl);

	ctrl->bmHint = 1;
	ctrl->bFormatIndex = iformat + 1;
	ctrl->bFrameIndex = iframe + 1;
	ctrl->dwFrameInterval = frame->intervals[0];
	switch (dev->fcc) {
	case V4L2_PIX_FMT_YUYV:
		ctrl->dwMaxVideoFrameSize = frame->width * frame->height * 2;
		break;
	case V4L2_PIX_FMT_MJPEG:
	case V4L2_PIX_FMT_H264:
		ctrl->dwMaxVideoFrameSize = 4*1024*1024;
		break;
	}

	/* TODO: the UVC maxpayload transfer size should be filled
	 * by the driver.
	 */
	ctrl->dwMaxPayloadTransferSize = (dev->maxpkt) *
				(dev->mult) * (dev->burst + 1);

	ctrl->bmFramingInfo = 3;
	ctrl->bPreferedVersion = 1;
	ctrl->bMaxVersion = 1;

	if (format->fcc == V4L2_PIX_FMT_H264) {
		ctrl->bUsage = 18;	// IB...BP
		ctrl->bmSettings = 0x26;
	}
}

void
uvc_control(int req,
	       struct uvc_device *dev,
	       struct uvc_request_data *resp,
	       struct capability *capa)
{
	switch (req) {
	case UVC_SET_CUR:
		if (capa->val == -1)
			resp->data[0] = capa->def;
		else
			resp->data[0] = capa->val;
		resp->length = 1;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> SET_CUR:0x%x\n", resp->data[0]);
		break;

	case UVC_GET_MIN:
		resp->data[0] = capa->min;
		resp->length = 2;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> GET_MIN:0x%x\n", resp->data[0]);
		break;

	case UVC_GET_MAX:
		resp->data[0] = capa->max;
		resp->length = 2;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> GET_MAX:0x%x\n", resp->data[0]);
		break;

	case UVC_GET_CUR:
		if (capa->val == -1)
			resp->data[0] = capa->def;
		else
			resp->data[0] = capa->val;
		resp->length = 2;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> GET_CUR:0x%x\n", resp->data[0]);
		break;

	case UVC_GET_INFO:
		resp->data[0] = 0x03;
		resp->length = 1;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> GET_INFO:0x%x\n", resp->data[0]);
		break;

	case UVC_GET_DEF:
		resp->data[0] = capa->def;
		resp->length = 2;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> GET_DEF:0x%x\n", resp->data[0]);
		break;

	case UVC_GET_RES:
		resp->data[0] = capa->step;
		resp->length = 2;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> GET_RES:0x%x\n", resp->data[0]);
		break;

	case UVC_GET_LEN:
		resp->data[0] = capa->len;
		resp->length = 2;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> GET_RES:0x%x\n", resp->data[0]);
		break;


	default:
		resp->length = -EL2HLT;
		dev->request_error_code.data[0] = 0x07;
		dev->request_error_code.length = 1;
		debug_p(">>>>> OTHER:0x%x\n", req);
		break;
	}
}

static void
uvc_events_process_standard(struct uvc_device *dev,
			    struct usb_ctrlrequest *ctrl,
			    struct uvc_request_data *resp)
{
	printf("standard request\n");
	(void)dev;
	(void)ctrl;

	resp->length = 0;
	switch (ctrl->bRequest) {
	case 0x00:/* GET_STATUS */
		printf("GET_STATUS\n");
		break;

	case 0x01:/* CLEAR_FEATURE */
		printf("CLEAR_FEATURE\n");
		break;

	case 0x03:/* SET_FEATURE */
		printf("SET_FEATURE\n");
		break;
	case 0x05:/* SET_ADDRESS */
		printf("SET_ADDRESS\n");
		break;

	case 0x06:/* GET_DESCIPTOR */
		printf("GET_DESCIPTOR\n");
		break;

	case 0x07:/* SET_DESCRIPTOR */
		printf("SET_DESCRIPTOR\n");
		break;

	case 0x08:/* GET_CONFIGURATION */
		printf("GET_CONFIGURATION\n");
		break;

	case 0x09:/* SET_CONFIGURATION */
		printf("SET_CONFIGURATION\n");
		break;

	case 0x0A:/* GET_INTERFACE */
		printf("GET_INTERFACE\n");
		break;

	case 0x0B:/* SET_INTERFACE */
		printf("SET_INTERFACE\n");
		break;

	case 0x0C:/* SYNCH_FRAME */
		printf("SYNCH_FRAME\n");
		break;

	case 0x30:/* SET_SEL */
		printf("SET_SEL\n");
		break;

	case 0x31:/* SET_ISOCH_DELAY */
		printf("SET_ISOCH_DELAY\n");
		break;

	default:
		printf("%s %d\n",__func__,ctrl->bRequest);
		break;
	}
}

static void
uvc_events_process_control(struct uvc_device *dev,
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
		case UVC_CT_AE_PRIORITY_CONTROL:
			debug_p(">>>> UVC_CT_AE_PRIORITY_CONTROL\n");
			uvc_control(req, dev, resp, &ae_priority);
			break;

		case UVC_CT_ZOOM_ABSOLUTE_CONTROL:
			debug_p(">>>> UVC_CT_ZOOM_ABSOLUTE_CONTROL\n");
			uvc_control(req, dev, resp, &zoom);
			break;

		case UVC_CT_ROLL_ABSOLUTE_CONTROL:
			debug_p(">>>> UVC_CT_ROLL_ABSOLUTE_CONTROL\n");
			uvc_control(req, dev, resp, &roll);
			break;

		case UVC_CT_SCANNING_MODE_CONTROL: /* not support */
		case UVC_CT_AE_MODE_CONTROL: /* not support */
		case UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL: /* not support */
		case UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL: /* not support */
		case UVC_CT_FOCUS_ABSOLUTE_CONTROL: /* not support */
		case UVC_CT_FOCUS_RELATIVE_CONTROL: /* not support */
		case UVC_CT_FOCUS_AUTO_CONTROL: /* not support */
		case UVC_CT_IRIS_ABSOLUTE_CONTROL: /* not support */
		case UVC_CT_IRIS_RELATIVE_CONTROL: /* not support */
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

		case UVC_PU_SATURATION_CONTROL:
			debug_p(">>>> UVC_PU_SATURATION_CONTROL\n");
			uvc_control(req, dev, resp, &saturation);
			break;

		case UVC_PU_SHARPNESS_CONTROL:
			debug_p(">>>> UVC_PU_SHARPNESS_CONTROL\n");
			uvc_control(req, dev, resp, &sharpness);
			break;

		case UVC_PU_GAMMA_CONTROL:
			debug_p(">>>> UVC_PU_GAMMA_CONTROL\n");
			uvc_control(req, dev, resp, &gamma1);
			break;

		case UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
			debug_p(">>>> UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL\n");
			uvc_control(req, dev, resp, &coltemper);
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

	debug_p("--control request (entity_id %02x cs %02x req %02x)\n\n", entity_id, cs, req);
}

static void
uvc_events_process_streaming(struct uvc_device *dev,
			     uint8_t req,
			     uint8_t cs,
			     struct uvc_request_data *resp)
{
	struct uvc15_streaming_control *ctrl;

	debug_p("--streaming request (req %02x cs %02x)\n\n", req, cs);

	if (cs != UVC_VS_PROBE_CONTROL && cs != UVC_VS_COMMIT_CONTROL)
		return;

	ctrl = (struct uvc15_streaming_control *)&resp->data;
	resp->length = sizeof *ctrl;

	switch (req) {
	case UVC_SET_CUR:
		dev->control = cs;
		resp->length = sizeof *ctrl;
		break;

	case UVC_GET_CUR:
		if (cs == UVC_VS_PROBE_CONTROL)
			memcpy(ctrl, &dev->probe, sizeof *ctrl);
		else
			memcpy(ctrl, &dev->commit, sizeof *ctrl);
		break;

	case UVC_GET_MIN:
	case UVC_GET_MAX:
	case UVC_GET_DEF:
		uvc_fill_streaming_control(dev,
					   ctrl,
					   req == UVC_GET_MAX ? -1 : 0,
					   req == UVC_GET_MAX ? -1 : 0);
		break;

	case UVC_GET_RES:
		CLEAR(ctrl);
		break;

	case UVC_GET_LEN:
		resp->data[0] = 0x00;
		resp->data[1] = sizeof *ctrl;
		resp->length = 2;
		break;

	case UVC_GET_INFO:
		resp->data[0] = 0x03;
		resp->length = 1;
		break;
	}
}

static void
uvc_events_process_class(struct uvc_device *dev,
			 struct usb_ctrlrequest *ctrl,
			 struct uvc_request_data *resp)
{
	int intf = ctrl->wIndex & 0xff;
	//printf("%s, dev->uvc_fd:%d\n", __func__, dev->uvc_fd);
	if ((ctrl->bRequestType & USB_RECIP_MASK) != USB_RECIP_INTERFACE){
		printf("%s, return\n", __func__);
		return;
	}
	//printf("intf:%d, intf_control:%d, dev->intf_streaming:%d\n", intf, dev->intf_control, dev->intf_streaming);
	if (intf == dev->intf_control) {//0, 2, 4
		debug_p(">>> UVC_INTF_CONTROL\n");
		uvc_events_process_control(dev,
					   ctrl->bRequest,
					   ctrl->wValue >> 8,
					   ctrl->wIndex >> 8,
					   ctrl->wLength, resp);
	} else if (intf == dev->intf_streaming) {// 1
		debug_p(">>> UVC_INTF_STREAMING\n");
		uvc_events_process_streaming(dev,
					     ctrl->bRequest,
					     ctrl->wValue >> 8,
					     resp);
	}
}

static int
uvc_events_process_control_data(struct uvc_device *dev,
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
		case UVC_CT_AE_PRIORITY_CONTROL:
			debug_p(">>> UVC_CT_AE_PRIORITY_CONTROL\n");
			memcpy(&val, data->data, data->length);
			debug_p("     AE PRIORITY VAL : 0x%lx\n", val);

			if (val >= ae_priority.min && val <= ae_priority.max) {
				cmd.cmd_set = E_UVC_IFT_COM_SET_IMG_CFG;
				cmd.cmd = E_UVC_IFT_SUB_COM_AE_METERING;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = val;
				cmd.send_param[1] = 0;
				cmd.to_sec = 3;
				if (camera_if_command(&cmd) != 0 ||
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
				cmd.to_sec = 3;
				if (camera_if_command(&cmd) != 0 ||
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
				cmd.to_sec = 3;
				if (camera_if_command(&cmd) != 0 ||
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
				cmd.cmd_set = E_UVC_IFT_COM_SET_IMG_CFG;
				cmd.cmd = E_UVC_IFT_SUB_COM_EV;
				memset(&cmd.send_param, 0, sizeof(cmd.send_param));
				cmd.send_param[0] = 18 - val;
				cmd.to_sec = 3;
				if (camera_if_command(&cmd) != 0 ||
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

		case UVC_PU_SATURATION_CONTROL:
			debug_p(">>> UVC_PU_SATURATION_CONTROL\n");
			memcpy(&saturation.val, data->data, data->length);
			debug_p("     SATURATION VAL : %ld\n", saturation.val);
			break;

		case UVC_PU_SHARPNESS_CONTROL:
			debug_p(">>> UVC_PU_SHARPNESS_CONTROL\n");
			memcpy(&sharpness.val, data->data, data->length);
			debug_p("     SHARPNESS VAL : %ld\n", sharpness.val);
			break;

		case UVC_PU_GAMMA_CONTROL:
			debug_p(">>> UVC_PU_GAMMA_CONTROL\n");
			memcpy(&gamma1.val, data->data, data->length);
			debug_p("     GAMMA VAL : %ld\n", gamma1.val);
			break;

		case UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
			debug_p(">>> UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL\n");
			memcpy(&coltemper.val, data->data, data->length);
			debug_p("     WHITE_BALANCE VAL : %ld\n", coltemper.val);
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
			cmd.cmd_set = E_UVC_IFT_COM_SET_IMG_CFG;
			cmd.cmd = E_UVC_IFT_SUB_COM_ADAPTIVE_AE;
			cmd.send_param[0] = 0;
			if (camera_if_command(&cmd) != 0 ||
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

	debug_p("--Control Request data phase (cs 0x%02x entity 0x%02x)\n\n",
		cs, entity_id);

	return ret;
}

static void
uvc_events_process_setup(struct uvc_device *dev,
			 struct usb_ctrlrequest *ctrl,
			 struct uvc_request_data *resp)
{
	dev->control = 0;
	//printf("%s, dev->uvc_fd:%d\n", __func__, dev->uvc_fd);
#ifdef ENABLE_USB_REQUEST_DEBUG
	printf("\nbRequestType %02x bRequest %02x wValue %04x wIndex %04x "
		"wLength %04x\n", ctrl->bRequestType, ctrl->bRequest,
		ctrl->wValue, ctrl->wIndex, ctrl->wLength);
#endif

	switch (ctrl->bRequestType & USB_TYPE_MASK) {
	case USB_TYPE_STANDARD:
		debug_p(">> USB_TYPE_STANDARD\n");
		uvc_events_process_standard(dev, ctrl, resp);
		break;

	case USB_TYPE_CLASS:
		debug_p(">> USB_TYPE_CLASS\n");
		uvc_events_process_class(dev, ctrl, resp);
		break;

	default:
		break;
	}
}


static int
uvc_events_process_data(struct uvc_device *dev,
			struct uvc_request_data *data,
			struct usb_ctrlrequest *dctrl)
{
	struct uvc15_streaming_control *target;
	struct uvc15_streaming_control *ctrl;
	const struct uvc_format_info *formats;
	const struct uvc_format_info *format;
	const struct uvc_frame_info *frame;
	const unsigned int *interval;
	unsigned int iformat, iframe;
	unsigned int nformats, nframes;
	int intf = dctrl->wIndex & 0xff;

	debug_p(">> uvc_events_process_data control:0x%x wValue:0x%x\n",
						dev->control, dctrl->wValue);

	if  (intf == dev->intf_control) {
		return uvc_events_process_control_data(dev, dctrl->wValue >> 8, dctrl->wIndex >> 8, data);
	} else if (intf != dev->intf_streaming) {
		return -1;
	}

	switch (dev->control) {
	case UVC_VS_PROBE_CONTROL:
		printf("setting probe control, length = %d\n", data->length);
		target = &dev->probe;
		break;

	case UVC_VS_COMMIT_CONTROL:
		printf("setting commit control, length = %d\n", data->length);
		target = &dev->commit;
		break;

	default:
		return -1;
	}

	ctrl = (struct uvc15_streaming_control *)&data->data;
	if (dev->speed == USB_SPEED_SUPER) {
		nformats = dev->ss_nformats;
		formats = dev->ss_formats;
	 } else {
		nformats = dev->nformats;
		formats = dev->formats;
	}
	iformat = clamp((unsigned int)ctrl->bFormatIndex, 1U, nformats);
	format = &formats[iformat-1];

	nframes = 0;
	while (format->frames[nframes].width != 0)
		++nframes;

	iframe = clamp((unsigned int)ctrl->bFrameIndex, 1U, nframes);
	frame = &format->frames[iframe-1];
	interval = frame->intervals;

	while (interval[0] < ctrl->dwFrameInterval && interval[1])
		++interval;

	target->bFormatIndex = iformat;
	target->bFrameIndex = iframe;

	switch (format->fcc) {
	case V4L2_PIX_FMT_YUYV:
		target->dwMaxVideoFrameSize = frame->width * frame->height * 2;
		break;
	case V4L2_PIX_FMT_MJPEG:
	case V4L2_PIX_FMT_H264:
		target->dwMaxVideoFrameSize = 4*1024*1024;
		break;
	}
	target->dwFrameInterval = *interval;

	if (dev->control == UVC_VS_COMMIT_CONTROL) {
		dev->fcc = format->fcc;
		dev->width = frame->width;
		dev->height = frame->height;
		dev->interval = target->dwFrameInterval;
		dev->imgsize = target->dwMaxVideoFrameSize;
	}

	return 0;
}

static void
uvc_events_process(struct uvc_device *dev)
{
	struct v4l2_event v4l2_event;
	struct uvc_event *uvc_event = (void *)&v4l2_event.u.data;
	struct uvc_request_data resp;
	static struct usb_ctrlrequest ctrl;
	int ret;

	ret = ioctl(dev->uvc_fd, VIDIOC_DQEVENT, &v4l2_event);
	if (ret < 0) {
		printf("VIDIOC_DQEVENT failed: %s (%d)\n", strerror(errno), errno);
		return;
	}

	memset(&resp, 0, sizeof resp);
	resp.length = -EL2HLT;

	switch (v4l2_event.type) {
	case UVC_EVENT_CONNECT:
		printf("> UVC_EVENT_CONNECT\n");
		dev->speed = uvc_event->speed;
		switch (dev->speed) {
		case USB_SPEED_FULL:
			printf("Full Speed\n");
			dev->burst = 0;
			dev->maxpkt = MIN(dev->max_packet_size, 1023);
			break;
		case USB_SPEED_HIGH:
			printf("High Speed\n");
			dev->burst = 0;
			dev->maxpkt = MIN(dev->max_packet_size, 1024);
			break;
		case USB_SPEED_SUPER:
			printf("Super Speed\n");
			dev->burst = MIN(dev->max_burst_size, 15);
			dev->maxpkt = MIN(dev->max_packet_size, 1024);
			break;
		default:
			printf("Unknown Speed\n");
			dev->burst = 0;
			dev->maxpkt = MIN(dev->max_packet_size, 1024);
			break;
		}
		uvc_fill_streaming_control(dev, &dev->probe, 0, 0);
		uvc_fill_streaming_control(dev, &dev->commit, 0, 0);
		return;

	case UVC_EVENT_DISCONNECT:
		printf("> UVC_EVENT_DISCONNECT\n");
		dev->uvc_shutdown_requested = 1;
		printf("UVC: Possible USB shutdown requested from "
				"Host, seen via UVC_EVENT_DISCONNECT\n");
		uvc_handle_streamoff_event(dev);
		return;

	case UVC_EVENT_SETUP:
		printf("> UVC_EVENT_SETUP\n");
		memcpy(&ctrl, &uvc_event->req, sizeof(struct usb_ctrlrequest));
		uvc_events_process_setup(dev, &uvc_event->req, &resp);
		break;

	case UVC_EVENT_DATA:
		printf("> UVC_EVENT_DATA\n");
		if (ctrl.bRequestType == 0x21 && ctrl.bRequest == 0x01 )
			ret = uvc_events_process_data(dev, &uvc_event->data, &ctrl);
		memset(&ctrl, 0, sizeof(struct usb_ctrlrequest));
		if (ret < 0)
			break;
		return;

	case UVC_EVENT_STREAMON:
		debug_p("> UVC_EVENT_STREAMON\n");
		uvc_handle_streamon_event(dev);
		return;

	case UVC_EVENT_STREAMOFF:
		debug_p("> UVC_EVENT_STREAMOFF\n");
		uvc_handle_streamoff_event(dev);
		return;
	}

	ret = ioctl(dev->uvc_fd, UVCIOC_SEND_RESPONSE, &resp);
	if (ret < 0) {
		printf("UVCIOC_S_EVENT failed: %s (%d)\n", strerror(errno), errno);
		return;
	}
}

static void
uvc_events_init(struct uvc_device *dev)
{
	struct v4l2_event_subscription sub;

	memset(&sub, 0, sizeof sub);
	sub.type = UVC_EVENT_CONNECT;
	ioctl(dev->uvc_fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
	sub.type = UVC_EVENT_DISCONNECT;
	ioctl(dev->uvc_fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
	sub.type = UVC_EVENT_SETUP;
	ioctl(dev->uvc_fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
	sub.type = UVC_EVENT_DATA;
	ioctl(dev->uvc_fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
	sub.type = UVC_EVENT_STREAMON;
	ioctl(dev->uvc_fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
	sub.type = UVC_EVENT_STREAMOFF;
	ioctl(dev->uvc_fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
}

/* ---------------------------------------------------------------------------
 * main
 */

struct thread_arg {
	pthread_t tid;

	char *devname;
	int nbufs;
	int max_packet_mult;
	int max_burst_size;
	int max_packet_size;

	struct ipcu_stream *istm;
};

static volatile int app_abort = 0;

static void signal_handler(int signal)
{
	app_abort = 1;
}

static void sigterm_handler(int signal)
{
	app_abort = 1;
}

static void
usage(const char *argv0)
{
	fprintf(stderr, "Usage: %s [options]\n", argv0);
	fprintf(stderr, "Available options are\n");
	fprintf(stderr, " -b		Streaming max burst (b/w 0 and 15)\n");
	fprintf(stderr, " -p		Streaming max packet (b/w 1 and 3072)\n");
	fprintf(stderr, " -u device	UVC Video Output device\n");
	fprintf(stderr, " -h		Print this help screen and exit\n");
}

static void *
thread_h264_func(void *arg)
{
	struct thread_arg *targ = (struct thread_arg *)arg;
	struct ipcu_stream *istm = targ->istm;
	struct uvc_device *udev = NULL;
	struct timeval *timeout;
	struct timeval tv;
	fd_set fdsv, dfds, efds;
	int ret, nfds;

	const struct uvc_format_info uvc_formats[] = {
		{ V4L2_PIX_FMT_H264, uvc_frames_h264 },
	};
	const struct uvc_format_info ss_uvc_formats[] = {
		{ V4L2_PIX_FMT_H264, ss_uvc_frames_h264 },
	};

	uvc_open(&udev, targ->devname);
	udev->istm = istm;
	istm->udev = udev;
	udev->io = IO_METHOD_USERPTR;
	udev->nbufs = targ->nbufs;
	udev->mult = targ->max_packet_mult;
	udev->max_burst_size = targ->max_burst_size;
	udev->max_packet_size = targ->max_packet_size;
	udev->speed = USB_SPEED_UNKNOWN;

	udev->intf_control = UVC_INTF_CONTROL;
	udev->intf_streaming = UVC_INTF_STREAMING;

	udev->formats = uvc_formats;
	udev->nformats = ARRAY_SIZE(uvc_formats);
	udev->ss_formats = ss_uvc_formats;
	udev->ss_nformats = ARRAY_SIZE(ss_uvc_formats);

	/* Init UVC events. */
	uvc_events_init(udev);

	while (!app_abort) {
		FD_ZERO(&fdsv);
		FD_ZERO(&dfds);
		FD_ZERO(&efds);
		//printf("%s(), 1\n",__func__);
		FD_SET(istm->ipcu_fd, &fdsv);
		if (udev->is_stream_on)
			FD_SET(udev->uvc_fd, &dfds);
		FD_SET(udev->uvc_fd, &efds);
		//printf("%s(), 2\n",__func__);
		/* Timeout. */
		if (udev->is_stream_on) {
			tv.tv_sec = 2;
			tv.tv_usec = 0;
			timeout = &tv;
		} else
			timeout = NULL;
		//printf("%s(), 3, istm->ipcu_fd:%d, udev->uvc_fd:%d\n",__func__, istm->ipcu_fd, udev->uvc_fd);
		nfds = max(istm->ipcu_fd, udev->uvc_fd);
		ret = select(nfds + 1, &fdsv, &dfds, &efds, timeout);
		//printf("%s(), 4\n",__func__);
		if (-1 == ret) {
			printf("select error %d, %s\n",
					errno, strerror (errno));
			if (EINTR == errno)
				continue;

			break;
		}
		//printf("%s(), 5\n",__func__);
		if (0 == ret) {
			printf("select timeout\n");
			continue;
		}
		//printf("%s(), 6\n",__func__);
		if (FD_ISSET(udev->uvc_fd, &efds)){
			//printf("%s(), 6-1\n",__func__);
			uvc_events_process(udev);
		}
		//printf("%s(), 7\n",__func__);
		if (FD_ISSET(udev->uvc_fd, &dfds)){
			//printf("%s(), 7-1\n",__func__);
			uvc_video_process(udev);
		}
		//printf("%s(), 8\n",__func__);
		if (FD_ISSET(istm->ipcu_fd, &fdsv)){
			//printf("%s(), 8-1\n",__func__);
			ipcu_process_data(istm);
		}
	}
	//printf("%s(), 9\n",__func__);

	uvc_handle_streamoff_event(udev);
	uvc_close(udev);
	//printf("%s(), 10\n",__func__);
	return NULL;
}

static void *
thread_h264_func2(void *arg)
{
	struct thread_arg *targ = (struct thread_arg *)arg;
	struct ipcu_stream *istm = targ->istm;
	struct uvc_device *udev = NULL;
	struct timeval *timeout;
	struct timeval tv;
	fd_set fdsv, dfds, efds;
	int ret, nfds;

	const struct uvc_format_info uvc_formats[] = {
		{ V4L2_PIX_FMT_H264, uvc_frames_h264 },
	};
	const struct uvc_format_info ss_uvc_formats[] = {
		{ V4L2_PIX_FMT_H264, ss_uvc_frames_h264 },
	};

	uvc_open(&udev, targ->devname);
	udev->istm = istm;
	istm->udev = udev;
	udev->io = IO_METHOD_USERPTR;
	udev->nbufs = targ->nbufs;
	udev->mult = targ->max_packet_mult;
	udev->max_burst_size = targ->max_burst_size;
	udev->max_packet_size = targ->max_packet_size;
	udev->speed = USB_SPEED_UNKNOWN;

	udev->intf_control = 2; //UVC_INTF_CONTROL;
	udev->intf_streaming = 3; //UVC_INTF_STREAMING;

	udev->formats = uvc_formats;
	udev->nformats = ARRAY_SIZE(uvc_formats);
	udev->ss_formats = ss_uvc_formats;
	udev->ss_nformats = ARRAY_SIZE(ss_uvc_formats);

	/* Init UVC events. */
	uvc_events_init(udev);

	while (!app_abort) {
		FD_ZERO(&fdsv);
		FD_ZERO(&dfds);
		FD_ZERO(&efds);
		//printf("%s(), 1\n",__func__);
		FD_SET(istm->ipcu_fd, &fdsv);
		if (udev->is_stream_on)
			FD_SET(udev->uvc_fd, &dfds);
		FD_SET(udev->uvc_fd, &efds);
		//printf("%s(), 2\n",__func__);
		/* Timeout. */
		if (udev->is_stream_on) {
			tv.tv_sec = 2;
			tv.tv_usec = 0;
			timeout = &tv;
		} else
			timeout = NULL;
		//printf("%s(), 3, istm->ipcu_fd:%d, udev->uvc_fd:%d\n",__func__, istm->ipcu_fd, udev->uvc_fd);
		nfds = max(istm->ipcu_fd, udev->uvc_fd);
		ret = select(nfds + 1, &fdsv, &dfds, &efds, timeout);
		//printf("%s(), 4\n",__func__);
		if (-1 == ret) {
			printf("select error %d, %s\n",
					errno, strerror (errno));
			if (EINTR == errno)
				continue;

			break;
		}
		//printf("%s(), 5\n",__func__);
		if (0 == ret) {
			//printf("select timeout\n");
			continue;
		}
		//printf("%s(), 6\n",__func__);
		if (FD_ISSET(udev->uvc_fd, &efds)){
			//printf("%s(), 6-1\n",__func__);
			uvc_events_process(udev);
		}
		//printf("%s(), 7\n",__func__);
		if (FD_ISSET(udev->uvc_fd, &dfds)){
			//printf("%s(), 7-1\n",__func__);
			uvc_video_process(udev);
		}
		//printf("%s(), 8\n",__func__);
		if (FD_ISSET(istm->ipcu_fd, &fdsv)){
			//printf("%s(), 8-1\n",__func__);
			ipcu_process_data(istm);
		}
	}
	//printf("%s(), 9\n",__func__);

	uvc_handle_streamoff_event(udev);
	uvc_close(udev);
	//printf("%s(), 10\n",__func__);
	return NULL;


}

static void *
thread_h264_func3(void *arg)
{
	struct thread_arg *targ = (struct thread_arg *)arg;
	struct ipcu_stream *istm = targ->istm;
	struct uvc_device *udev = NULL;
	struct timeval *timeout;
	struct timeval tv;
	fd_set fdsv, dfds, efds;
	int ret, nfds;

	const struct uvc_format_info uvc_formats[] = {
		{ V4L2_PIX_FMT_H264, uvc_frames_h264 },
	};
	const struct uvc_format_info ss_uvc_formats[] = {
		{ V4L2_PIX_FMT_H264, ss_uvc_frames_h264 },
	};

	uvc_open(&udev, targ->devname);
	udev->istm = istm;
	istm->udev = udev;
	udev->io = IO_METHOD_USERPTR;
	udev->nbufs = targ->nbufs;
	udev->mult = targ->max_packet_mult;
	udev->max_burst_size = targ->max_burst_size;
	udev->max_packet_size = targ->max_packet_size;
	udev->speed = USB_SPEED_UNKNOWN;

	udev->intf_control = 4; //UVC_INTF_CONTROL;
	udev->intf_streaming = 5; //UVC_INTF_STREAMING;

	udev->formats = uvc_formats;
	udev->nformats = ARRAY_SIZE(uvc_formats);
	udev->ss_formats = ss_uvc_formats;
	udev->ss_nformats = ARRAY_SIZE(ss_uvc_formats);

	/* Init UVC events. */
	uvc_events_init(udev);

	while (!app_abort) {
		FD_ZERO(&fdsv);
		FD_ZERO(&dfds);
		FD_ZERO(&efds);
		//printf("%s(), 1\n",__func__);
		FD_SET(istm->ipcu_fd, &fdsv);
		if (udev->is_stream_on)
			FD_SET(udev->uvc_fd, &dfds);
		FD_SET(udev->uvc_fd, &efds);
		//printf("%s(), 2\n",__func__);
		/* Timeout. */
		if (udev->is_stream_on) {
			tv.tv_sec = 2;
			tv.tv_usec = 0;
			timeout = &tv;
		} else
			timeout = NULL;
		//printf("%s(), 3, istm->ipcu_fd:%d, udev->uvc_fd:%d\n",__func__, istm->ipcu_fd, udev->uvc_fd);
		nfds = max(istm->ipcu_fd, udev->uvc_fd);
		ret = select(nfds + 1, &fdsv, &dfds, &efds, timeout);
		//printf("%s(), 4\n",__func__);
		if (-1 == ret) {
			printf("select error %d, %s\n",
					errno, strerror (errno));
			if (EINTR == errno)
				continue;

			break;
		}
		//printf("%s(), 5\n",__func__);
		if (0 == ret) {
			//printf("select timeout\n");
			continue;
		}
		//printf("%s(), 6\n",__func__);
		if (FD_ISSET(udev->uvc_fd, &efds)){
			//printf("%s(), 6-1\n",__func__);
			uvc_events_process(udev);
		}
		//printf("%s(), 7\n",__func__);
		if (FD_ISSET(udev->uvc_fd, &dfds)){
			//printf("%s(), 7-1\n",__func__);
			uvc_video_process(udev);
		}
		//printf("%s(), 8\n",__func__);
		if (FD_ISSET(istm->ipcu_fd, &fdsv)){
			//printf("%s(), 8-1\n",__func__);
			ipcu_process_data(istm);
		}
	}
	//printf("%s(), 9\n",__func__);

	uvc_handle_streamoff_event(udev);
	uvc_close(udev);
	//printf("%s(), 10\n",__func__);
	return NULL;


}


int
main(int argc, char *argv[])
{
	struct ipcu_stream **istms = NULL;
	struct thread_arg th_h264_arg, th_h264_arg2, th_h264_arg3;
	char *uvc_devname[3] = { "/dev/video0", "/dev/video1", "/dev/video2" };
	
	int ret, opt, option_index;
	/* Frame format/resolution related params. */
	int nbufs = 2;			/* Ping-Pong buffers */
	/* USB speed related params */
	int max_packet_mult = 0;
	int max_burst_size = 0;
	int max_packet_size = 1024;
	bInitial = 0;
	printf("gadget-m, %s, %d\n", __func__, __LINE__);
	struct option long_options[] = {
		{ "max-burst-size",	required_argument,	NULL, 'b' },
		{ "max-packet-size",	required_argument,	NULL, 'p' },
		{ "uvc-devname",	required_argument,	NULL, 'u' },
		{ "uvc-devname-jpeg",	required_argument,	NULL, 'u' },
		{ "help",		no_argument,		NULL, 'h' },
		{ 0, 0,	0, 0 },
	};

	if (signal(SIGINT, signal_handler) == SIG_ERR)
		perror("signal(SIGINT)");
	if (signal(SIGTERM, sigterm_handler) == SIG_ERR)
		perror("signal(SIGTERM)");

	while ((opt = getopt_long(argc, argv, "b:p:h", long_options, &option_index)) != -1) {
		switch (opt) {
		case 'b':
			max_burst_size = atoi(optarg);
			if (max_burst_size < 0 || max_burst_size > 15) {
				usage(argv[0]);
				return 1;
			}
			break;
		case 'p':
			max_packet_size = atoi(optarg);
			if (max_packet_size < 1 || max_packet_size > 3072) {
				usage(argv[0]);
				return 1;
			}
			break;
		case 'u':
			if (strcmp(long_options[option_index].name, "uvc-devname") == 0)
				uvc_devname[0] = optarg;
			else
				uvc_devname[1] = optarg;
			break;			
		case 'h':
			usage(argv[0]);
			return 1;
		default:
			printf("Invalid option '-%c'\n", opt);
			usage(argv[0]);
			return 1;
		}
	}

	if (max_packet_size <= 1024) {
		max_packet_mult = 1;
	} else if (max_packet_size <= 2048) {
		max_packet_mult = 2;
		max_packet_size = max_packet_size / 2;
	} else {
		max_packet_mult = 3;
		max_packet_size = max_packet_size / 3;
	}

	/* Open the IPCU device. */
	ret = ipcu_open(&istms);
	if (istms == NULL || ret < 0){
		printf("gadget-m, %s, %d, return \n", __func__, __LINE__);
		return 1;
	}

	th_h264_arg2.devname = uvc_devname[1];
	th_h264_arg2.nbufs = nbufs;
	th_h264_arg2.max_packet_mult = max_packet_mult;
	th_h264_arg2.max_burst_size = max_burst_size;
	th_h264_arg2.max_packet_size = max_packet_size;
	th_h264_arg2.istm = istms[1];
	pthread_create(&th_h264_arg2.tid, NULL, thread_h264_func2, &th_h264_arg2);


	th_h264_arg.devname = uvc_devname[0];
	th_h264_arg.nbufs = nbufs;
	th_h264_arg.max_packet_mult = max_packet_mult;
	th_h264_arg.max_burst_size = max_burst_size;
	th_h264_arg.max_packet_size = max_packet_size;
	th_h264_arg.istm = istms[0];
	pthread_create(&th_h264_arg.tid, NULL, thread_h264_func, &th_h264_arg);


	th_h264_arg3.devname = uvc_devname[2];
	th_h264_arg3.nbufs = nbufs;
	th_h264_arg3.max_packet_mult = max_packet_mult;
	th_h264_arg3.max_burst_size = max_burst_size;
	th_h264_arg3.max_packet_size = max_packet_size;
	th_h264_arg3.istm = istms[2];
	pthread_create(&th_h264_arg3.tid, NULL, thread_h264_func3, &th_h264_arg3);


	pthread_join(th_h264_arg2.tid, NULL);	
	pthread_join(th_h264_arg.tid, NULL);
	pthread_join(th_h264_arg3.tid, NULL);
	printf("gadget-m, %s, %d, done\n", __func__, __LINE__);
	ipcu_close(istms);

	return 0;
}

