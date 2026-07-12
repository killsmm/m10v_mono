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
#include <sys/eventfd.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

#include <linux/usb/ch9.h>
#include <linux/limits.h>

#include "uvc-gadget_common.h"

//#define EOF_SOF_DBG

//#define TIME_DEBUG
//#ifdef TIME_DEBUG
static struct timeval tv1, tv2;
static UINT32 tv_count1;
static int counter=0;
//#endif

static int g_in_dev_cmd = 0x2;
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

/* forward declarations */
static int uvc_video_stream(struct uvc_device *dev, int enable);

static int is_rtospl(){
        FILE *file;
        //char buffer[10];

        file = fopen("/tmp/rtospl.conf", "r");
        if(file !=NULL) {
          fclose(file);
          return 1;
        }
        return 0;
}

static int is_bulk(){
	FILE *file;
	//char buffer[10];

	file = fopen("/tmp/bulk.conf", "r");
	if(file !=NULL) {
	  fclose(file);
	  return 1;
	}
	return 0;
}

static int fps_debug(){
	FILE *file;
	//char buffer[10];

	file = fopen("/tmp/uvc_fps.conf", "r");
	if(file !=NULL) {
	  fclose(file);
	  return 1;
	}
	return 0;
}

static int max_payload_size(){
	FILE *file;
	char buffer[10];
	int ret = 0;

	file = fopen("/tmp/max_payload_size.conf", "r");
	if(file !=NULL) {
	  fread(buffer, 1, 10, file );
	  ret = atoi(buffer);
	  fclose(file);
	  return ret;
	}
	return 1024*1024;
}
static int max_payload_size_host(){
	FILE *file;
	char buffer[10];
	int ret = 0;

	file = fopen("/tmp/max_payload_size_host.conf", "r");
	if(file !=NULL) {
	  fread(buffer, 1, 10, file );
	  ret = atoi(buffer);
	  fclose(file);
	  return ret;
	}
	return 1024*1024;
}
static int with_header(){
	FILE *file;
	char buffer[10];
	int ret = 0;

	file = fopen("/tmp/with_header.conf", "r");
	if(file !=NULL) {
	  fread(buffer, 1, 10, file );
	  ret = atoi(buffer);
	  fclose(file);
	  return ret;
	}
	return 1;
}
static int header_size(){
	FILE *file;
	char buffer[10];
	int ret = 0;

	file = fopen("/tmp/header_size.conf", "r");
	if(file !=NULL) {
	  fread(buffer, 1, 10, file );
	  ret = atoi(buffer);
	  fclose(file);
	  return ret;
	}
	return 16;
}
static int is_uav_mode(){
       FILE *file;

       file = fopen("/tmp/1011.conf", "r");
       if(file !=NULL) {
         fclose(file);
         return 1;
       }

       return 0;
}

/* ---------------------------------------------------------------------------
 * IPCU streaming related
 */

static void
ipcu_queue_buffer(struct ipcu_device *dev, unsigned long address)
{
	struct ipcu_buffer *b;

	pthread_mutex_lock(&dev->list_lock);
	TAILQ_FOREACH(b, &dev->list_done, entries) {
		if (b->buf.sample_address == address) {
			TAILQ_REMOVE(&dev->list_done, b, entries);
			Stream_release(&b->buf);
			TAILQ_INSERT_TAIL(&dev->list_free, b, entries);
			break;
		}
	}
	pthread_mutex_unlock(&dev->list_lock);
}

static int
ipcu_dequeue_buffer(struct ipcu_device *dev,
		    unsigned long *address, unsigned long *size)
{
	eventfd_t value;
	struct ipcu_buffer *b;
	int ret;

	ret = eventfd_read(dev->ipcu_fd, &value);
	if (ret < 0)
		return -1;

	pthread_mutex_lock(&dev->list_lock);
	b = TAILQ_FIRST(&dev->list_wait);
	if (b == NULL) {
		pthread_mutex_unlock(&dev->list_lock);
		*address = 0;
		*size = 0;
		return 0;
	}
	TAILQ_REMOVE(&dev->list_wait, b, entries);
	TAILQ_INSERT_TAIL(&dev->list_done, b, entries);
	pthread_mutex_unlock(&dev->list_lock);

	*address = b->buf.sample_address;
	*size = b->buf.sample_size;
	return 0;
}

static int
ipcu_process_data(struct ipcu_device *dev)
{
	struct v4l2_buffer ubuf;
	unsigned int index;
	unsigned long address, size;
	int ret;
	
	/* Return immediately if IPCU streaming has not yet started. */
	if (!dev->is_streaming){
		return 0;
	}

	for (index = 0; index < dev->udev->nbufs; index++) {
		if (dev->udev->dqbufs & (1 << index))
			break;
	}
	if (index == dev->udev->nbufs){
		return 0;
	}

	ret = ipcu_dequeue_buffer(dev, &address, &size);
	if (ret < 0 || address == 0){
		return ret;
	}

	/* Queue video buffer to UVC domain. */
	memset (&ubuf, 0, sizeof(ubuf));

	ubuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ubuf.memory = V4L2_MEMORY_USERPTR;
	ubuf.m.userptr = address;
	ubuf.length = dev->udev->imgsize;
	ubuf.index = index;
	ubuf.bytesused = size;


	ret = ioctl(dev->udev->uvc_fd, VIDIOC_QBUF, &ubuf);
	if (ret < 0) {
		printf("UVC: Unable to queue buffer %d: %s (%d).\n",
			ubuf.index, strerror(errno), errno);
		ipcu_queue_buffer(dev, address);
		/* Check for a USB disconnect/shutdown event. */
		if (errno == ENODEV) {
			dev->udev->uvc_shutdown_requested = 1;
			printf("UVC: Possible USB shutdown requested from "
				"Host, seen during VIDIOC_QBUF\n");
			return 0;
		} else {
			return ret;
		}
	}

	dev->udev->dqbufs &= ~(1 << ubuf.index);
	dev->udev->ndqbufs--;
#ifdef ENABLE_BUFFER_DEBUG
	printf("Queueing buffer at UVC side = %d\n", ubuf.index);
#endif
	if (!dev->udev->first_buffer_queued) {
		uvc_video_stream(dev->udev, 1);
		dev->udev->first_buffer_queued = 1;
		dev->udev->is_streaming = 1;
	}
	return ret;
}

/* ---------------------------------------------------------------------------
 * IPCU generic stuff
 */
static int
ipcu_callback(struct cif_stream_send* p, void *d)
{
	struct ipcu_buffer *b;
	struct ipcu_device *dev = (struct ipcu_device *)d;
#ifdef EOF_SOF_DBG
       static UINT32 eof = 0, sof = 0;
#endif

	if (!dev->is_streaming || !dev->is_enabled) {
		return -1;
	}

	if (dev->stream_id != p->stream_id) {
		return -1;
	}

#ifdef EOF_SOF_DBG
       switch (p->Sub_Command){
               case 0x500: //mjpeg
                       eof = (UINT32)p->media.j.sensor_timestamp.eof;
                       sof = (UINT32)p->media.j.sensor_timestamp.sof;
                       printf("mjpeg sof:%ld, eof:%ld\n", sof, eof);
        break;
               case 0x100: //264
                       eof = (UINT32)p->media.v.sensor_timestamp.eof;
                       sof = (UINT32)p->media.v.sensor_timestamp.sof;
                       printf("h264 sof:%ld, eof:%ld\n", sof, eof);
               break;
       }
#endif

	if(dev->fps_debug){
		if(counter==0){
			printf("\n\n begin counting, start time:(counter:%d)\n", counter);
			gettimeofday(&tv2, NULL);
		}

		counter++;
		if(counter%30==0){
			int fps=0;
			tv1 = tv2; //previous
			gettimeofday(&tv2, NULL); //now
			tv_count1 = (tv2.tv_sec - tv1.tv_sec) * 1000000L + tv2.tv_usec - tv1.tv_usec;
			printf("uvc process time usage: %d us for 30 frames passed(counter:%d)\n", tv_count1, counter);
			//to ms
			tv_count1 = tv_count1/1000;
			//fps
			fps = (30*10000)/tv_count1;
			printf("fps:%d\n", fps);
		}
	}

	pthread_mutex_lock(&dev->list_lock);
	b = TAILQ_FIRST(&dev->list_free);
	if (b == NULL) {
		pthread_mutex_unlock(&dev->list_lock);
		return -1;
	}
	TAILQ_REMOVE(&dev->list_free, b, entries);
	b->buf = *p;
	TAILQ_INSERT_TAIL(&dev->list_wait, b, entries);
	pthread_mutex_unlock(&dev->list_lock);
	eventfd_write(dev->ipcu_fd, 1);

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
static int
ipcu_start_capturing(struct ipcu_device *dev)
{
	int ret = 0;
	if(is_roi())
		ret = ipcu_start_capturing_sub_roi(dev, g_in_dev_cmd);
	else
		ret = ipcu_start_capturing_sub(dev, g_in_dev_cmd);
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
ipcu_stop_capturing(struct ipcu_device *dev)
{
	int ret = 0;
	struct ipcu_buffer *b;

	if (!dev->is_streaming)
		return 0;

	dev->is_enabled = 0;
	dev->is_streaming = 0;

	pthread_mutex_lock(&dev->list_lock);
	if (!TAILQ_EMPTY(&dev->list_wait)) {
		TAILQ_FOREACH(b, &dev->list_wait, entries) {
			TAILQ_REMOVE(&dev->list_wait, b, entries);
			TAILQ_INSERT_TAIL(&dev->list_done, b, entries);
		}
	}
	if (!TAILQ_EMPTY(&dev->list_done)) {
		TAILQ_FOREACH(b, &dev->list_done, entries) {
			TAILQ_REMOVE(&dev->list_done, b, entries);
			Stream_release(&b->buf);
			TAILQ_INSERT_TAIL(&dev->list_free, b, entries);
		}
	}
	pthread_mutex_unlock(&dev->list_lock);
	if(is_roi())
		ret = ipcu_stop_capturing_sub_roi(dev);
	else
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

static void
ipcu_enable_capturing(struct ipcu_device *dev)
{
	dev->is_enabled = 1;
}

static void
ipcu_disable_capturing(struct ipcu_device *dev)
{
	dev->is_enabled = 0;
}

static int
ipcu_open(struct ipcu_device **idev)
{
	struct ipcu_device *dev;
	struct cb_main_func func_p;
	struct ipcu_param ipcu_prm;
	int fd;
	int ret;
	fd = eventfd(0, EFD_NONBLOCK);
	if (fd < 0) {
		return -1;
	}

	dev = calloc(1, sizeof *dev);
	if (dev == NULL) {
		goto err;
	}

	dev->b1 = malloc(sizeof(struct ipcu_buffer));
	if (dev->b1 == NULL) {
		goto err_free;
	}
	dev->b2 = malloc(sizeof(struct ipcu_buffer));
	if (dev->b2 == NULL) {
		goto err_free;
	}

	TAILQ_INIT(&dev->list_free);
	TAILQ_INIT(&dev->list_wait);
	TAILQ_INIT(&dev->list_done);
	TAILQ_INSERT_TAIL(&dev->list_free, dev->b1, entries);
	TAILQ_INSERT_TAIL(&dev->list_free, dev->b2, entries);
	pthread_mutex_init(&dev->list_lock, NULL);

	memset(&func_p, 0x00, sizeof(func_p));
	func_p.video = (void*)ipcu_callback;
	func_p.jpeg = (void*)ipcu_callback;
	func_p.yuv = (void*)ipcu_callback;
	func_p.user_data = (void*)dev;

	memset(&ipcu_prm, 0x00, sizeof(ipcu_prm));
	ipcu_prm.chid = 6;
	ipcu_prm.ackid = 7;
	ret = Stream_ipcu_ch_init(&func_p, ipcu_prm);
	if (ret < 0) {
		goto err_free;
	}

	//ret = ipcu_open_sub();
	//if (ret < 0) {
	//	goto err_close;
	//}

	dev->ipcu_fd = fd;
	*idev = dev;

	return 0;


err_free:
	if (dev) {
		if (dev->b2)
			free(dev->b2);
		if (dev->b1)
			free(dev->b1);
		free(dev);
	}

err:
	close(fd);

	return -1;
}

static void
ipcu_close(struct ipcu_device *dev)
{
	Stream_ipcu_ch_close();
	free(dev->b2);
	free(dev->b1);
	close(dev->ipcu_fd);
	free(dev);
}

int
uvc_camera_if_command(struct camera_if_cmd *cmd)
{
#if 1
	int ret=0;
	int retry_cnt;
#define UVC_CAMIF_RETRY_CNT	(10)

	debug_p("cmd=0x%08x, subcmd=0x%08x, para=0x%x, 0x%x, 0x%x, 0x%x\n",
		   cmd->cmd_set, cmd->cmd,
		   cmd->send_param[0], cmd->send_param[1], cmd->send_param[2], cmd->send_param[3]);

	cmd->to_sec = 5;	// timeout time
	
	for(retry_cnt = 0; retry_cnt < UVC_CAMIF_RETRY_CNT; retry_cnt++) {
		ret = camera_if_command(cmd);
		if(ret == 0)
			break;
	}
	
	if(retry_cnt > 0 )
		printf("uc_camera_if_command : retry %d times\n", retry_cnt);
	
	if(retry_cnt == UVC_CAMIF_RETRY_CNT)
		ret = -1;

	cmd->recv_param[0] = 0;	
	if(ret == 0 && cmd->recv_param[0] != 0)
		printf("cmd=0x%08x, subcmd=0x%08x, para=0x%x, 0x%x, 0x%x, 0x%x, recv_param[0]=0x%x, recv_param[1]=0x%x\n",
			  cmd->cmd_set, cmd->cmd,
			  cmd->send_param[0], cmd->send_param[1], cmd->send_param[2], cmd->send_param[3],
			  cmd->recv_param[0], cmd->recv_param[1]);
	return ret;
#else
        char string[128];
        printf("cmd=0x%08x, subcmd=0x%08x, para=0x%x, 0x%x, 0x%x, 0x%x\n",
                   cmd->cmd_set, cmd->cmd,
                   cmd->send_param[0], cmd->send_param[1], cmd->send_param[2], cmd->send_param[3]);

        sprintf(string,"camera_if_direct 0x%x 0x%x 0x%x", cmd->cmd_set, cmd->cmd, cmd->send_param[0]);
        printf("%s(), str:[%s] \n", __func__, string);
        system(string);
        cmd->recv_param[0] = 0;
        return 0;
#endif
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
	close(dev->uvc_fd);
	free(dev);
}

/* ---------------------------------------------------------------------------
 * UVC streaming related
 */

static int
uvc_video_process(struct uvc_device *dev)
{
	struct v4l2_buffer ubuf;
	int ret;
	/*
	 * Return immediately if UVC video output device has not started
	 * streaming yet.
	 */
	if (!dev->is_streaming)
		return 0;

	/* Prepare a v4l2 buffer to be dequeued from UVC domain. */
	CLEAR(ubuf);

	ubuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ubuf.memory = V4L2_MEMORY_USERPTR;

	/* Dequeue the spent buffer from UVC domain */
	ret = ioctl(dev->uvc_fd, VIDIOC_DQBUF, &ubuf);
	if (ret < 0)
		return ret;

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
	ipcu_queue_buffer(dev->idev, ubuf.m.userptr);

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
	if(dev->rtospl == 1){
		struct uvc_user_param param;
		if(!dev->bulk){
			dev->max_payload_size  = (dev->maxpkt) * (dev->mult) * (dev->burst + 1);
		}
		dev->with_header = with_header();
		param.with_header = dev->with_header;
		param.header_size = dev->header_size;
		param.max_payload_size = dev->max_payload_size;
		ret = ioctl(dev->uvc_fd, UVCIOC_SET_USER_PARAM, &param);
		if (ret < 0 )
			goto err;
	}

	ret = uvc_video_reqbufs(dev, dev->nbufs);
	if (ret < 0)
		goto err;
	dev->is_stream_on = 1;

	ipcu_start_capturing(dev->idev);
	dev->idev->is_streaming = 1;
	ipcu_enable_capturing(dev->idev);

	return 0;

err:
	return ret;
}

static int
uvc_handle_streamoff_event(struct uvc_device *dev)
{
	/* Disable IPCU streaming... */
	ipcu_disable_capturing(dev->idev);
	/* Stop UVC streaming... */
	dev->is_stream_on = 0;
	if (dev->is_streaming) {
		uvc_video_stream(dev, 0);
		uvc_video_reqbufs(dev, 0);
		dev->is_streaming = 0;
		dev->first_buffer_queued = 0;
	}
	/* ... and now IPCU streaming.. */
	ipcu_stop_capturing(dev->idev);

	return 0;
}

/* ---------------------------------------------------------------------------
 * UVC Request processing
 */

static void
uvc_fill_streaming_control(struct uvc_device *dev,
			   struct uvc_streaming_control *ctrl,
			   int iframe, int iformat)
{
	const struct uvc_format_info *formats;
	const struct uvc_format_info *format;
	const struct uvc_frame_info *frame;
	unsigned int nformats, nframes;
	formats = get_uvc_formats(dev, &nformats);

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
	switch (format->fcc) {
		case V4L2_PIX_FMT_YUYV:
			printf("%s, %d V4L2_PIX_FMT_YUYV\n", __func__, __LINE__);
			ctrl->dwMaxVideoFrameSize = frame->width * frame->height * 2;
			break;
		case V4L2_PIX_FMT_NV12:
			printf("%s, %d V4L2_PIX_FMT_NV12\n", __func__, __LINE__);
			ctrl->dwMaxVideoFrameSize = (frame->width * frame->height * 3) / 2;
			break;
		case V4L2_PIX_FMT_MJPEG:
			printf("%s, %d V4L2_PIX_FMT_MJPEG\n", __func__, __LINE__);
			ctrl->dwMaxVideoFrameSize = 4*1024*1024;
			break;
		case V4L2_PIX_FMT_H264:
			printf("%s, %d V4L2_PIX_FMT_H264\n", __func__, __LINE__);
			ctrl->dwMaxVideoFrameSize = 4*1024*1024;
			break;
	}
	printf("%s, %d , ctrl->dwMaxVideoFrameSize:%d\n", __func__, __LINE__, ctrl->dwMaxVideoFrameSize);
	/* TODO: the UVC maxpayload transfer size should be filled
	 * by the driver.
	 */
	if(dev->rtospl == 1){//using rtos payload, it should be bulk mode
		if(!dev->bulk)
			ctrl->dwMaxPayloadTransferSize = (dev->maxpkt) * (dev->mult) * (dev->burst + 1);
		else
			ctrl->dwMaxPayloadTransferSize = max_payload_size_host();//dev->max_payload_size;
	}else{//it should be iso mode instead because we do not enable rtospl for bulk
		//if(!dev->bulk)
			ctrl->dwMaxPayloadTransferSize = (dev->maxpkt) * (dev->mult) * (dev->burst + 1);
		//else
		//	ctrl->dwMaxPayloadTransferSize = ctrl->dwMaxVideoFrameSize;
	}
	printf("%s, %d , ctrl->dwMaxPayloadTransferSize:%d\n", __func__, __LINE__, ctrl->dwMaxPayloadTransferSize);
	ctrl->bmFramingInfo = 3;
	ctrl->bPreferedVersion = 1;
	ctrl->bMaxVersion = 1;
}
#if 0
static void
uvc_set_data(void *data, long val, short len)
{
	switch (len) {
	case 1:
		*((char *)data) = (char)val;
		break;
	case 2:
		*((short *)data) = (short)val;
		break;
	case 4:
		*((long *)data) = (long)val;
		break;
	case 8:
		*((long *)data) = (long)val;
		printf("uvc set data case val :0x%lx, des:0x%lx\n", val, *((long *)data));
		break;
	default:
		debug_p("uvcset_data error %d.\n", len);
		break;
	}
}
#endif
#if 0 //original
void
uvc_control(int req,
	       struct uvc_device *dev,
	       struct uvc_request_data *resp,
	       struct uvc_capability *capa)
{
	long tmp;
#ifdef DEBUG_PRINT
	unsigned int *val = (unsigned int *)resp->data;
#endif
	switch (req) {
	case UVC_SET_CUR:
		if (capa->val == UVC_VAL_EMPTY)
			tmp = capa->def;
		else
			tmp = capa->val;

		if (capa->len==8){
			debug_p(">>>>> UVC_SET_CUR 8:0x%x\n", tmp);
			/*tilt*/
			uvc_set_data(resp->data+4, tmp, capa->len);
			resp->length = capa->len;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> UVC_SET_CUR tilt:0x%x\n", (unsigned int *)(resp->data+4));

			/*pan*/
			uvc_set_data(resp->data, tmp, capa->len);
			resp->length = capa->len;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> UVC_SET_CUR pan:0x%x\n", (unsigned int *)resp->data);
		}else{
			uvc_set_data(resp->data, tmp, capa->len);
			resp->length = capa->len;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
		}
		break;

	case UVC_GET_MIN:
		if (capa->len==8){
			debug_p(">>>>> GET_MIN 8:0x%x\n", *val);
			/*tilt*/
			uvc_set_data(resp->data+4, capa->min, capa->len);
			resp->length = capa->len;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> GET_MIN tilt:0x%x\n", *(resp->data+4));

			/*pan*/
			uvc_set_data(resp->data, capa->min, capa->len);
			resp->length = capa->len;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> GET_MIN pan:0x%x\n", *val);
		}else{
			uvc_set_data(resp->data, capa->min, capa->len);
			resp->length = capa->len;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> GET_MIN:0x%x\n", *val);
		}
		break;

	case UVC_GET_MAX:
		if (capa->len==8){
			debug_p(">>>>> GET_MAX capa->max_ll:0x%llx\n", capa->max_ll);
			/*tilt*/
			uvc_set_data(resp->data+4, capa->max_ll>>32, capa->len);
			resp->length = capa->len;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> GET_MAX tilt:0x%x\n", (unsigned int *)(resp->data+4));

			/*pan*/
			uvc_set_data(resp->data, (long)capa->max_ll, capa->len);
			resp->length = capa->len;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> GET_MAX pan:0x%x\n", (unsigned int *)resp->data);
		}else{
			uvc_set_data(resp->data, capa->max, capa->len);
			resp->length = capa->len;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> GET_MAX:0x%x\n", *val);
		}
		break;

	case UVC_GET_CUR:
		if (capa->val == UVC_VAL_EMPTY)
			tmp = capa->def;
		else
			tmp = capa->val;

		if (capa->len==8){
			/*tilt*/
			uvc_set_data(resp->data+4, tmp, capa->len);
			resp->length = 2;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> UVC_GET_CUR tilt:0x%x\n", *(resp->data+4));

			/*pan*/
			uvc_set_data(resp->data, tmp, capa->len);
			resp->length = 2;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> UVC_GET_CUR pan:0x%x\n", *val);
		}else{
			uvc_set_data(resp->data, tmp, capa->len);
			resp->length = 2;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> GET_CUR:0x%x\n", *val);
		}
		break;

	case UVC_GET_INFO:
		if (capa->len==8)
			resp->data[4] = 0x03;

		resp->data[0] = 0x03;
		resp->length = 1;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> GET_INFO:0x%x\n", *val);
		break;

	case UVC_GET_DEF:
		if (capa->len==8){
			/*tilt*/
			uvc_set_data(resp->data+4, capa->def, capa->len);
			resp->length = capa->len;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> UVC_GET_DEF tilt:0x%x\n", *(resp->data+4));

			/*pan*/
			uvc_set_data(resp->data, capa->def, capa->len);
			resp->length = capa->len;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> UVC_GET_DEF pan:0x%x\n", *val);
		}else{
			uvc_set_data(resp->data, capa->def, capa->len);
			resp->length = capa->len;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> GET_DEF:0x%x\n", *val);
		}
		break;

	case UVC_GET_RES:
		if (capa->len==8){
			/*tilt*/
			uvc_set_data(resp->data+4, capa->step, capa->len);
			resp->length = capa->len;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> UVC_GET_RES tilt:0x%x\n", *(resp->data+4));

			/*pan*/
			uvc_set_data(resp->data, capa->step, capa->len);
			resp->length = capa->len;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> UVC_GET_RES pan:0x%x\n", *val);
		}else{
			uvc_set_data(resp->data, capa->step, capa->len);
			resp->length = capa->len;
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> GET_RES:0x%x\n", *val);
		}
		break;

	case UVC_GET_LEN:
		if (capa->len==8){
			/*tilt*/
			uvc_set_data(resp->data+4, capa->len, capa->len);
			resp->length = sizeof(capa->len);
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> UVC_GET_LEN tilt:0x%x\n", *(resp->data+4));

			/*pan*/
			uvc_set_data(resp->data, capa->len, capa->len);
			resp->length = sizeof(capa->len);
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> UVC_GET_LEN pan:0x%x\n", *val);
		}else{
			uvc_set_data(resp->data, capa->len, sizeof(capa->len));
			resp->length = sizeof(capa->len);
			dev->request_error_code.data[0] = 0x00;
			dev->request_error_code.length = 1;
			debug_p(">>>>> GET_RES:0x%x\n", *val);
		}
		break;
	default:
		resp->length = -EL2HLT;
		dev->request_error_code.data[0] = 0x07;
		dev->request_error_code.length = 1;
		debug_p(">>>>> OTHER:0x%x\n", req);
		break;
	}
}
#else
void
uvc_control(int req,
	       struct uvc_device *dev,
	       struct uvc_request_data *resp,
	       struct uvc_capability *capa)
{
	unsigned long * ptr;
	unsigned long tmp;
	switch (req) {
	case UVC_SET_CUR:
		if (capa->val == UVC_VAL_EMPTY)
			tmp = capa->def;
		else
			tmp = capa->val;
#if 0
		*resp->data = tmp;
		if (capa->len==8){/* pantilt */
			ptr = (unsigned long *)(resp->data+4);
			*ptr = tmp;
			debug_p("ptr:%lx\n", *ptr);
		}
#else
		if (capa->len==8){/* pantilt */
			memcpy(&resp->data, &tmp, 4);
			memcpy((unsigned long *)(resp->data+4), &tmp, 4);
		}else{
			memcpy(&resp->data, &tmp, capa->len);
		}
#endif
		resp->length = capa->len;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> SET_CUR:0x%x\n", *resp->data);
		break;

	case UVC_GET_MIN:
#if 0
		*resp->data = capa->min;

		if (capa->len==8){/* pantilt */
			ptr = (unsigned long *)(resp->data+4);
			*ptr = capa->min;
			debug_p("ptr:%lx\n", *ptr);
		}
#else
		if (capa->len==8){/* pantilt */
			memcpy(&resp->data, &capa->min, 4);
			memcpy((unsigned long *)(resp->data+4), &capa->min, 4);
		}else{
			memcpy(&resp->data, &capa->min, capa->len);
		}
#endif
		resp->length = capa->len;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> GET_MIN:0x%x\n", *resp->data);
		break;

	case UVC_GET_MAX:
#if 0
		ptr = (unsigned long *)resp->data;
		*ptr = capa->max;

		debug_p("*ptr:%lx, *resp->data:%x\n" , *ptr, *resp->data);
		if (capa->len==8){/* pantilt */
			ptr = (unsigned long *)(resp->data+4);
			*ptr = capa->max;
			debug_p("ptr:%lx\n", *ptr);
		}
		ptr = (unsigned long *)resp->data;
#else
		if (capa->len==8){/* pantilt */
			memcpy(&resp->data, &capa->max, 4);
			memcpy((unsigned long *)(resp->data+4), &capa->max, 4);
		}else{
			memcpy(&resp->data, &capa->max, capa->len);
		}
#endif

		resp->length = capa->len;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> GET_MAX:0x%x, capa->max:%ld\n", *resp->data, capa->max);
		break;

	case UVC_GET_CUR:

		if (capa->val == UVC_VAL_EMPTY)
			tmp = capa->def;
		else
			tmp = capa->val;
#if 0
		ptr = (unsigned long *)resp->data;
		*ptr = tmp;

		if (capa->len==8){/* pantilt */
			ptr = (unsigned long *)(resp->data+4);
			if (capa->val2 == UVC_VAL_EMPTY)
				*ptr = capa->def;
			else
				*ptr = capa->val2;
			debug_p("*ptr2 :%lx\n" , *ptr);
		}
#else
		if (capa->len==8){/* pantilt */
			memcpy(&resp->data, &tmp, 4);
			if (capa->val2 == UVC_VAL_EMPTY)
				memcpy((unsigned long *)(resp->data+4), &tmp, 4);
			else
				memcpy((unsigned long *)(resp->data+4), &capa->val2, 4);
		}else{
			memcpy(&resp->data, &tmp, capa->len);
		}

#endif
		resp->length = capa->len;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> GET_CUR:0x%x\n", *resp->data);
		break;

	case UVC_GET_INFO:
		*resp->data = 0x03;
		if (capa->len==8){/* pantilt */
			ptr = (unsigned long *)(resp->data+4);
			*ptr = 0x03;
			debug_p("ptr:%lx\n", *ptr);
		}

		resp->length = capa->len;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> GET_INFO:0x%x\n", *resp->data);
		break;

	case UVC_GET_DEF:
#if 0
		ptr = (unsigned long *)resp->data;
		*ptr = capa->def;
		debug_p("*resp->data:%lx, capa->def:%ld\n" , *ptr, capa->def);

		if (capa->len==8){/* pantilt */
			ptr = (unsigned long *)(resp->data+4);
			*ptr = capa->def;
			debug_p("ptr:%lx\n", *ptr);
		}
#else
		if (capa->len==8){/* pantilt */
			memcpy(&resp->data, &capa->def, 4);
			memcpy((unsigned long *)(resp->data+4), &capa->def, 4);
		}else{
			memcpy(&resp->data, &capa->def, capa->len);
		}
#endif
		resp->length = capa->len;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> GET_DEF:0x%x\n", *resp->data);
		break;

	case UVC_GET_RES:
#if 0
		*resp->data = capa->step;

		if (capa->len==8){/* pantilt */
			ptr = (unsigned long *)(resp->data+4);
			*ptr = capa->step;
			debug_p("ptr:%lx\n", *ptr);
		}
#else
		if (capa->len==8){/* pantilt */
			memcpy(&resp->data, &capa->step, 4);
			memcpy((unsigned long *)(resp->data+4), &capa->step, 4);
		}else{
			memcpy(&resp->data, &capa->step, capa->len);
		}
#endif
		resp->length = capa->len;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> GET_RES:0x%x\n", *resp->data);
		break;

	case UVC_GET_LEN:
#if 0
		*resp->data = capa->len;

		if (capa->len==8){/* pantilt */
			ptr = (unsigned long *)(resp->data+4);
			*ptr = capa->len;
			debug_p("ptr:%lx\n", *ptr);
		}
#endif
		resp->length = capa->len;
		dev->request_error_code.data[0] = 0x00;
		dev->request_error_code.length = 1;
		debug_p(">>>>> GET_LEN:0x%x\n", *resp->data);
		break;


	default:
		resp->length = -EL2HLT;
		dev->request_error_code.data[0] = 0x07;
		dev->request_error_code.length = 1;
		debug_p(">>>>> OTHER:0x%x\n", req);
		break;
	}
}


#endif
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
	if(entity_id==1 && cs==0x14 && dev->bulk){//workaround
		uvc_handle_streamoff_event(dev);
	}else{
		if(is_roi())
			uvc_events_process_control_sub_roi(dev, req, cs, entity_id, len, resp);
		else
			uvc_events_process_control_sub(dev, req, cs, entity_id, len, resp);
	}
	debug_p("--control request (entity_id %02x cs %02x req %02x)\n\n", entity_id, cs, req);
}

static void
uvc_events_process_streaming(struct uvc_device *dev,
			     uint8_t req,
			     uint8_t cs,
			     struct uvc_request_data *resp)
{
	struct uvc_streaming_control *ctrl;
	debug_p("--streaming request (req %02x cs %02x)\n\n", req, cs);

	if (cs != UVC_VS_PROBE_CONTROL && cs != UVC_VS_COMMIT_CONTROL)
		return;

	ctrl = (struct uvc_streaming_control *)&resp->data;
	resp->length = sizeof *ctrl;

	switch (req) {
	case UVC_SET_CUR:
		dev->control = cs;
		resp->length = 34;
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
		resp->data[1] = 0x22;
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
	if ((ctrl->bRequestType & USB_RECIP_MASK) != USB_RECIP_INTERFACE)
		return;

	switch (ctrl->wIndex & 0xff) {
	case UVC_INTF_CONTROL:
		debug_p(">>> UVC_INTF_CONTROL\n");
		uvc_events_process_control(dev,
					   ctrl->bRequest,
					   ctrl->wValue >> 8,
					   ctrl->wIndex >> 8,
					   ctrl->wLength, resp);
		break;

	case UVC_INTF_STREAMING:
		debug_p(">>> UVC_INTF_STREAMING\n");
		uvc_events_process_streaming(dev,
					     ctrl->bRequest,
					     ctrl->wValue >> 8,
					     resp);
		break;

	default:
		break;
	}
}

static int
uvc_events_process_control_data(struct uvc_device *dev,
				uint8_t cs,
				uint8_t entity_id,
				struct uvc_request_data *data)
{
	int ret = 0;
	if(is_roi()) //roi mode
		ret = uvc_events_process_control_data_sub_roi(dev, cs, entity_id, data);
	else //normal mode
		ret = uvc_events_process_control_data_sub(dev, cs, entity_id, data);

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
	struct uvc_streaming_control *target;
	struct uvc_streaming_control *ctrl;
	const struct uvc_format_info *formats;
	const struct uvc_format_info *format;
	const struct uvc_frame_info *frame;
	const unsigned int *interval;
	unsigned int iformat, iframe;
	unsigned int nformats, nframes;
	debug_p(">> uvc_events_process_data control:0x%x wValue:0x%x\n",
						dev->control, dctrl->wValue);

	switch (dctrl->wIndex & 0xff) {
	case UVC_INTF_CONTROL:
		return uvc_events_process_control_data(dev, dctrl->wValue >> 8, dctrl->wIndex >> 8, data);
	case UVC_INTF_STREAMING:
		break;
	default:
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

	ctrl = (struct uvc_streaming_control *)&data->data;
	formats = get_uvc_formats(dev, &nformats);
	if (formats == NULL) {
		printf("format error = %d\n", dev->format);
		return -1;
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
		printf("%s, %d , V4L2_PIX_FMT_YUYV\n", __func__, __LINE__);
		target->dwMaxVideoFrameSize = frame->width * frame->height * 2;
		break;
	case V4L2_PIX_FMT_NV12:
		printf("%s, %d , V4L2_PIX_FMT_NV12\n", __func__, __LINE__);
		target->dwMaxVideoFrameSize = (frame->width * frame->height * 3) / 2;
		break;
	case V4L2_PIX_FMT_MJPEG:
		printf("%s, %d , V4L2_PIX_FMT_MJPEG\n", __func__, __LINE__);
		target->dwMaxVideoFrameSize = 4*1024*1024;
		break;

	case V4L2_PIX_FMT_H264:
		printf("%s, %d , V4L2_PIX_FMT_H264\n", __func__, __LINE__);
		target->dwMaxVideoFrameSize = 4*1024*1024;
		break;
	}
	printf("%s, %d , ctrl->dwMaxVideoFrameSize:%d\n", __func__, __LINE__, ctrl->dwMaxVideoFrameSize);
	target->dwFrameInterval = *interval;

	if (dev->control == UVC_VS_COMMIT_CONTROL) {
		dev->fcc = format->fcc;
		dev->width = frame->width;
		dev->height = frame->height;
		dev->interval = target->dwFrameInterval;
		dev->imgsize = target->dwMaxVideoFrameSize;
		printf("dev->fcc:%08x, dev->width:%d, dev->height:%d\n", dev->fcc, dev->width, dev->height);
		if(dev->bulk){
			uvc_handle_streamon_event(dev);
		}
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
		debug_p("> UVC_EVENT_CONNECT\n");
		dev->speed = uvc_event->speed;
		switch (dev->speed) {
		case USB_SPEED_FULL:
			debug_p("Full Speed\n");
			dev->burst = 0;
			if(dev->bulk)
				dev->maxpkt = 64;
			else
				dev->maxpkt = MIN(dev->max_packet_size, 1023);
			break;
		case USB_SPEED_HIGH:
			debug_p("High Speed\n");
			dev->burst = 0;
			if(dev->bulk)
				dev->maxpkt = 512;
			else
				dev->maxpkt = MIN(dev->max_packet_size, 1024);
			break;
		case USB_SPEED_SUPER:
			debug_p("Super Speed\n");

			if(dev->bulk){
				if(dev->rtospl == 1){
					printf("%s, Using RTOS payload\n", __func__);
					dev->burst = MIN(dev->max_burst_size, 15);
				}else
					dev->maxpkt = 1024;
			} else {
				dev->burst = MIN(dev->max_burst_size, 15);
				dev->maxpkt = MIN(dev->max_packet_size, 1024);
			}
			break;
		default:
			printf("Unknown Speed\n");
			dev->burst = 0;
			dev->maxpkt = MIN(dev->max_packet_size, 1024);
			break;
		}
	
		if((!dev->bulk && dev->rtospl==0) || dev->rtospl==1) {
			uvc_fill_streaming_control(dev, &dev->probe, 0, 0);
			uvc_fill_streaming_control(dev, &dev->commit, 0, 0);
		}

		return;

	case UVC_EVENT_DISCONNECT:
		debug_p("> UVC_EVENT_DISCONNECT\n");
		dev->uvc_shutdown_requested = 1;
		printf("UVC: Possible USB shutdown requested from "
				"Host, seen via UVC_EVENT_DISCONNECT\n");
		uvc_handle_streamoff_event(dev);
		return;

	case UVC_EVENT_SETUP:
		debug_p("> UVC_EVENT_SETUP\n");
		memcpy(&ctrl, &uvc_event->req, sizeof(struct usb_ctrlrequest));
		uvc_events_process_setup(dev, &uvc_event->req, &resp);
		break;

	case UVC_EVENT_DATA:
		debug_p("> UVC_EVENT_DATA\n");
		if (ctrl.bRequestType == 0x21 && ctrl.bRequest == 0x01 )
			ret = uvc_events_process_data(dev, &uvc_event->data, &ctrl);
		memset(&ctrl, 0, sizeof(struct usb_ctrlrequest));
		if (ret < 0)
			break;
		return;

	case UVC_EVENT_STREAMON:
		debug_p("> UVC_EVENT_STREAMON\n");
		if(!dev->bulk)
			uvc_handle_streamon_event(dev);
		return;

	case UVC_EVENT_STREAMOFF:
		debug_p("> UVC_EVENT_STREAMOFF\n");
		uvc_handle_streamoff_event(dev);
		return;

	default:
		debug_p("> OTHER\n");
		break;
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
	unsigned int payload_size = dev->imgsize;
	if(dev->rtospl == 0){
		switch(dev->fcc){
			case V4L2_PIX_FMT_YUYV:
				payload_size = dev->width * dev->height * 2;
				break;
			case V4L2_PIX_FMT_NV12:
				payload_size = (dev->width * dev->height * 3) / 2;
				break;
			case V4L2_PIX_FMT_H264:
			case V4L2_PIX_FMT_MJPEG:
				payload_size = dev->imgsize;
				break;
		}
		printf("#### uvc_events_init, payoad_size:%x\n", payload_size);
		if(dev->bulk){
			uvc_fill_streaming_control(dev, &dev->probe, 0, 0);
			uvc_fill_streaming_control(dev, &dev->commit, 0, 0);
			dev->probe.dwMaxPayloadTransferSize = dev->commit.dwMaxPayloadTransferSize = payload_size;  //confirmed
		}
	}
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

static int app_abort = 0;

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
	fprintf(stderr, " -b			Streaming max burst (b/w 0 and 15)\n");
	fprintf(stderr, " -p			Streaming max packet (b/w 1 and 3072)\n");
	fprintf(stderr, " -u device		UVC Video Output device\n");
	fprintf(stderr, " -i stream_id		If the ISP supports multi-stream, set it\n");
	fprintf(stderr, " -f format		Select frame format\n"
					"				0 = V4L2_PIX_FMT_YUYV\n"
					"				1 = V4L2_PIX_FMT_MJPEG\n"
					"				2 = V4L2_PIX_FMT_NV12\n"
					"				3 = V4L2_PIX_FMT_H264\n");
	fprintf(stderr, " -h			Print this help screen and exit\n");
}

int
main(int argc, char *argv[])
{
	struct uvc_device *udev = NULL;
	struct ipcu_device *idev = NULL;
	struct timeval *timeout = NULL;
	struct timeval tv;
	char *uvc_devname = "/dev/video0";
	fd_set fdsv, dfds, efds;
	int ret, opt, nfds;
	/* Frame format/resolution related params. */
	int nbufs = 2;			/* Ping-Pong buffers */
	/* USB speed related params */
	int max_packet_mult = 0;
	int max_burst_size = 0;
	int max_packet_size = 1024;
	int stream_id = 0;
	enum uvc_format format = UVC_FORMAT_AUTO;
	if (signal(SIGINT, signal_handler) == SIG_ERR)
		perror("signal(SIGINT)");
	if (signal(SIGTERM, sigterm_handler) == SIG_ERR)
		perror("signal(SIGTERM)");

	while ((opt = getopt(argc, argv, "b:d:p:u:s:i:f:h")) != -1) {
		switch (opt) {
		case 'd':
			g_in_dev_cmd = atoi(optarg);
			break;

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
			uvc_devname = optarg;
			break;

		case 'i':
			stream_id = atoi(optarg);
			if (stream_id < 0 || stream_id > 3) {
				usage(argv[0]);
				return 1;
			}
			break;

		case 'f':
			format = atoi(optarg);
			if (format < 0 || format >= UVC_FORMAT_MAX) {
				usage(argv[0]);
				return 1;
			}
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
	ret = ipcu_open(&idev);
	if (idev == NULL || ret < 0){
		return 1;
	}

	/* Open the UVC device. */

	ret = uvc_open(&udev, uvc_devname);
	if (udev == NULL || ret < 0){
		return 1;
	}
	if(is_bulk()){
		printf("%s, bulk mode\n", __func__);
		udev->bulk = 1;
	}
	if(is_rtospl()){
		printf("%s, using rtol pl\n", __func__);
		udev->rtospl = 1; 
	}

	udev->uvc_devname = uvc_devname;
	udev->idev = idev;
	idev->udev = udev;
	idev->fps_debug = fps_debug();
	udev->format = format;

	udev->io = IO_METHOD_USERPTR;
	udev->nbufs = nbufs;
	udev->mult = max_packet_mult;
	udev->max_burst_size = max_burst_size;
	udev->max_packet_size = max_packet_size;
	udev->speed = USB_SPEED_UNKNOWN;
	if(udev->rtospl == 1){
		printf("yocto using RTOS PL\n");
		udev->max_payload_size = max_payload_size();
		udev->with_header = with_header();
		udev->header_size = header_size();
	}else
		printf("yocto not RTOS PL \n");

	idev->stream_id_delta = (unsigned long)stream_id;
	/* Init UVC events. */
	uvc_events_init(udev);
	while (!app_abort) {
		FD_ZERO(&fdsv);
		FD_ZERO(&dfds);
		FD_ZERO(&efds);

		FD_SET(idev->ipcu_fd, &fdsv);
		if (udev->is_stream_on)
			FD_SET(udev->uvc_fd, &dfds);
		FD_SET(udev->uvc_fd, &efds);

		/* Timeout. */
		if (udev->is_stream_on) {
			tv.tv_sec = 2;
			tv.tv_usec = 0;
			timeout = &tv;
		} else
			timeout = NULL;

		nfds = max(idev->ipcu_fd, udev->uvc_fd);
		ret = select(nfds + 1, &fdsv, &dfds, &efds, timeout);

		if (-1 == ret) {
			printf("select error %d, %s\n",
					errno, strerror (errno));
			if (EINTR == errno)
				continue;

			break;
		}

		if (0 == ret) {
			printf("select timeout\n");
			continue;
		}

		if (FD_ISSET(udev->uvc_fd, &efds))
			uvc_events_process(udev);
		if (FD_ISSET(udev->uvc_fd, &dfds))
			uvc_video_process(udev);
		if (FD_ISSET(idev->ipcu_fd, &fdsv))
			ipcu_process_data(idev);
	}

	uvc_handle_streamoff_event(udev);

	ipcu_close(idev);

	uvc_close(udev);

	return 0;
}
