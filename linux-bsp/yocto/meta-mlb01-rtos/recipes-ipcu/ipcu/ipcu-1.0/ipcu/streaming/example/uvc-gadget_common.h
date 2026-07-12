#ifndef _UVC_GADGET_COMMON_H_
#define _UVC_GADGET_COMMON_H_
#include <stdint.h>
#include <pthread.h>
#include <sys/queue.h>

#include <linux/usb/video.h>
#include <linux/videodev2.h>

#include "uvc.h"
#include "cif_stream.h"

/* Enable debug prints. */
#undef ENABLE_BUFFER_DEBUG
#undef ENABLE_USB_REQUEST_DEBUG
#define DEBUG_PRINT

#ifdef DEBUG_PRINT
#define debug_p(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define debug_p(fmt, ...)
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
/* ---------------------------------------------------------------------------
 * IPCU specific stuff
 */

struct ipcu_buffer {
	struct cif_stream_send buf;
	TAILQ_ENTRY(ipcu_buffer) entries;
};

/* ---------------------------------------------------------------------------
 * Generic stuff
 */

/* UVC output format */
enum uvc_format {
	UVC_FORMAT_AUTO = -1,
	UVC_FORMAT_YUV = 0,
	UVC_FORMAT_MJPEG = 1,
	UVC_FORMAT_H264 = 2,
	UVC_FORMAT_NV12 = 3,
	UVC_FORMAT_MAX
};

/* IO methods supported */
enum io_method {
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR,
};

/* IPCU data format*/
enum ipcu_data_fmt {
	IPCU_DECIMAL	=	0,
	IPCU_DECIMAL_NAG1	=	1,	/* 1/10 */
	IPCU_DECIMAL_NAG2	=	2,	/* 1/100 */
	IPCU_DECIMAL_NAG3       =       3,      /* 1/1000 */
	IPCU_BRIGHTNESS		=	4,	/* for brightness*/
	IPCU_EXP_TIME		=	5,	/* for exposure time*/
};

/* ---------------------------------------------------------------------------
 * IPCU and UVC device instances
 */

struct ipcu_device {
	int ipcu_fd;
	int is_streaming;
	int is_enabled;
	int fps_debug;

	struct ipcu_buffer *b1, *b2;
	TAILQ_HEAD(list_free, ipcu_buffer) list_free;
	TAILQ_HEAD(list_wait, ipcu_buffer) list_wait;
	TAILQ_HEAD(list_done, ipcu_buffer) list_done;
	pthread_mutex_t list_lock;

	unsigned int stream_id;
	unsigned int stream_id_delta;

	/* uvc device hook */
	struct uvc_device *udev;
};

struct uvc_device {
	/* uvc device specific */
	int uvc_fd;
	int is_stream_on;
	int is_streaming;
	char *uvc_devname;
	enum uvc_format format;

	/* uvc control request specific */
	struct uvc_streaming_control probe;
	struct uvc_streaming_control commit;
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
	unsigned int bulk;

	uint8_t color;
	unsigned int imgsize;
	unsigned int rtospl;
	unsigned int with_header;
	unsigned int max_payload_size;
	unsigned int header_size;

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

	/* IPCU device hook */
	struct ipcu_device *idev;
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

#define UVC_VAL_EMPTY -1

struct uvc_capability {
	short len;
	short reserved;
	long max;
	long min;
	long def;
	long step;
	long val;
	long val2;
};

const struct uvc_format_info *
get_uvc_formats(struct uvc_device *dev, unsigned int *nformats);

void
uvc_events_process_control_sub(struct uvc_device *dev,
			   uint8_t req,
			   uint8_t cs,
			   uint8_t entity_id,
			   uint8_t len,
			   struct uvc_request_data *resp);

void
uvc_events_process_control_sub_roi(struct uvc_device *dev,
			   uint8_t req,
			   uint8_t cs,
			   uint8_t entity_id,
			   uint8_t len,
			   struct uvc_request_data *resp);
int
uvc_events_process_control_data_sub(struct uvc_device *dev,
				uint8_t cs,
				uint8_t entity_id,
				struct uvc_request_data *data);

int
uvc_events_process_control_data_sub_roi(struct uvc_device *dev,
				uint8_t cs,
				uint8_t entity_id,
				struct uvc_request_data *data);


int
ipcu_open_sub();

int
ipcu_start_capturing_sub(struct ipcu_device *dev, int in_dev_cmd);

int
ipcu_stop_capturing_sub(struct ipcu_device *dev);

/*for ROI, different command flow*/
int
ipcu_start_capturing_sub_roi(struct ipcu_device *dev, int in_dev_cmd);

int
ipcu_stop_capturing_sub_roi(struct ipcu_device *dev);


int
uvc_camera_if_command(struct camera_if_cmd *cmd);

void
uvc_control(int req,
	       struct uvc_device *dev,
	       struct uvc_request_data *resp,
	       struct uvc_capability *capa);

#endif /* _UVC_GADGET_COMMON_H_ */
