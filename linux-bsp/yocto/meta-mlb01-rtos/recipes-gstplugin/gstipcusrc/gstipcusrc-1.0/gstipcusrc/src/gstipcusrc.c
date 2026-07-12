/*
 * GStreamer
 * Copyright (C) 2016 Socionext Inc.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-ipcusrc
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 -v ipcusrc ! h264parse ! rtph264pay ! udpsink 
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/base/gstbytereader.h>
#include "gstipcusrc.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/sysinfo.h>
#include "cmfwk_mm.h"
#include "cmfwk_ipcu.h"
#include "cif_stream.h"
#include "gst_cif_meta.h"

GST_DEBUG_CATEGORY_STATIC (gst_ipcu_src_debug);
#define GST_CAT_DEFAULT gst_ipcu_src_debug
#define OK_PF
/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SIMPLE_IF,
  PROP_EXTEND,
  PROP_PLAYCTRL,
  PROP_ISPMETA,
  PROP_ADTS_EN,
};
#define RTSP_ON_OFF "/tmp/use_rtsp_func.def"

/* the capabilities of the inputs and outputs. */
static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

#define gst_ipcu_src_parent_class parent_class
G_DEFINE_TYPE (GstIpcuSrc, gst_ipcu_src, GST_TYPE_PUSH_SRC);

static void gst_ipcu_src_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_ipcu_src_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);
static void gst_ipcu_src_finalize (GObject * obj);
static GstStateChangeReturn gst_ipcu_src_change_state (GstElement * element,
    GstStateChange transition);
static gboolean gst_ipcu_src_start (GstBaseSrc * bsrc);
static gboolean gst_ipcu_src_stop (GstBaseSrc * bsrc);
static gboolean gst_ipcu_src_is_seekable (GstBaseSrc * bsrc);
static gboolean gst_ipcu_src_do_seek (GstBaseSrc * src, GstSegment * segment);
static gboolean gst_ipcu_src_query (GstBaseSrc * src, GstQuery * query);
static gboolean gst_ipcu_src_event (GstBaseSrc * src, GstEvent * event);
static GstFlowReturn gst_ipcu_src_create (GstPushSrc * psrc, GstBuffer ** outbuf);
static int audioCallback(struct cif_stream_send* p, void *d);
static int videoCallback(struct cif_stream_send* p, void *d);
int metaCB(cif_stream_meta_send* p, void *d);
static int gRTSP_func = 0;//RTSP function on/off flag
static int get_rtsp_on_off(void);

static GMutex *mw_queue_lock = NULL;
static GCond *mw_queue_cond = NULL;
static GQueue *mw_bufqueue = NULL;
static UINT8 mw_ipcu_id_resp = 0xFF;
static struct cb_main_func func;
static UINT8 extend_flag = FALSE;
struct ipcu_param ipcu_prm;
//#define _MEASURE_SPEC_
#ifdef _MEASURE_SPEC_
struct timespec meas_spec_st, meas_spec_ed;
#endif
/***********************************/
/* GObject vmethod implementations */
/***********************************/
static int get_rtsp_on_off(void)
{
	FILE *file;
	char buffer[10];


	file = fopen(RTSP_ON_OFF, "r");
	if(file !=NULL) {
		fgets(buffer, sizeof(buffer), file);
		gRTSP_func = buffer[0]-0x30;
		fclose(file);
	}
	return 0;
}
/* initialize the ipcusrc's class */
static void
gst_ipcu_src_class_init (GstIpcuSrcClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSrcClass *basesrc_class;
  GstPushSrcClass *pushsrc_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  basesrc_class = (GstBaseSrcClass *) klass;
  pushsrc_class = (GstPushSrcClass *) klass;

  gobject_class->set_property = gst_ipcu_src_set_property;
  gobject_class->get_property = gst_ipcu_src_get_property;
  gobject_class->finalize = gst_ipcu_src_finalize;

  g_object_class_install_property (gobject_class, PROP_SIMPLE_IF,
      g_param_spec_boolean ("simple_if", "Get data with simple interface via ipcu", "Plugin gets address and size only",
          TRUE, G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_EXTEND,
      g_param_spec_boolean ("extend", "extend flag", "Add header for ipcudemux",
          TRUE, G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_PLAYCTRL,
      g_param_spec_boolean ("playctrl", "Control auto Start/Stop for RTOS", "for rtsp",
          TRUE, G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_ISPMETA,
      g_param_spec_boolean ("ispmeta", "Boolean","Allow to receive of ISP output",
          TRUE, G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_ADTS_EN,
      g_param_spec_boolean ("adts_en", "Boolean","Allow to send adts audio",
          TRUE, G_PARAM_READWRITE));

  gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_ipcu_src_change_state);

  gst_element_class_set_static_metadata (gstelement_class,
    "IPCU Source",
    "Source",
    "Reads H.264 stream from IPCU device",
    "Socionext Inc.");
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));

  basesrc_class->start = GST_DEBUG_FUNCPTR (gst_ipcu_src_start);
  basesrc_class->stop = GST_DEBUG_FUNCPTR (gst_ipcu_src_stop);
  basesrc_class->is_seekable = GST_DEBUG_FUNCPTR (gst_ipcu_src_is_seekable);
  basesrc_class->do_seek = GST_DEBUG_FUNCPTR (gst_ipcu_src_do_seek);
  basesrc_class->query = GST_DEBUG_FUNCPTR (gst_ipcu_src_query);
  basesrc_class->event = GST_DEBUG_FUNCPTR (gst_ipcu_src_event);

  pushsrc_class->create = GST_DEBUG_FUNCPTR (gst_ipcu_src_create);
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_ipcu_src_init (GstIpcuSrc * src)
{
  GST_TRACE_OBJECT (src, "called");

  src->simple_if = FALSE;
  src->extend = FALSE;
  src->playctrl = FALSE;
  src->ipcu_type_smartcmd = 1;

  g_cond_init (&src->queue_cond);
  g_mutex_init (&src->queue_lock);
  src->bufqueue = g_queue_new();

#ifdef _MEASURE_SPEC_
  clock_getres(CLOCK_REALTIME, &meas_spec_st);
  clock_getres(CLOCK_REALTIME, &meas_spec_ed);
#endif
  get_rtsp_on_off();//Get RTSP valid flag

}

static void
gst_ipcu_src_finalize (GObject * obj)
{
  GstIpcuSrc *src = GST_IPCUSRC (obj);
  GST_TRACE_OBJECT (src, "called");

  g_queue_free(src->bufqueue);
  g_mutex_clear (&src->queue_lock);
  g_cond_clear (&src->queue_cond);

  G_OBJECT_CLASS (parent_class)->finalize (obj);
}


static void
gst_ipcu_src_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstIpcuSrc *src = GST_IPCUSRC (object);
  GST_TRACE_OBJECT (src, "called");

  switch (prop_id) {
    case PROP_SIMPLE_IF:
      src->simple_if = g_value_get_boolean (value);
      break;
    case PROP_EXTEND:
      src->extend = g_value_get_boolean (value);
      extend_flag = src->extend;
      break;
    case PROP_PLAYCTRL:
      src->playctrl = g_value_get_boolean (value);
      break;
    case PROP_ISPMETA:
      src->ispmeta = g_value_get_boolean (value);
      break;
    case PROP_ADTS_EN:
      src->adts_en = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_ipcu_src_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstIpcuSrc *src = GST_IPCUSRC (object);
  GST_TRACE_OBJECT (src, "called");

  switch (prop_id) {
    case PROP_SIMPLE_IF:
      g_value_set_boolean (value, src->simple_if);
      break;
    case PROP_EXTEND:
      src->extend = extend_flag;
      g_value_set_boolean (value, src->extend);
      break;
    case PROP_PLAYCTRL:
      g_value_set_boolean (value, src->playctrl);
      break;
    case PROP_ISPMETA:
      g_value_set_boolean (value, src->ispmeta);
      break;
    case PROP_ADTS_EN:
      g_value_set_boolean (value, src->adts_en);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static gboolean ipcu_core_start (GstIpcuSrc *src)
{
	gboolean ret = TRUE;
	if(gRTSP_func == 0) {
		//RTSP invalid
		func.video = videoCallback;
		if( src->adts_en == TRUE )
		{
			func.audio = audioCallback;
		}
		else
		{
			func.audio = videoCallback;
		}
		func.jpeg = videoCallback;
		func.raw = NULL;
		if( src->ispmeta == TRUE )
		{
			func.meta = metaCB;
		}
		else
		{
			func.meta = NULL;
		}
		ipcu_prm.phys = 1;
		ipcu_prm.chid = 6;
		ipcu_prm.ackid = 7;
		if( src->simple_if == TRUE )
		{
			ipcu_prm.sw = 1;
		}
		else
		{
			ipcu_prm.sw = 0;
		}
		if( Stream_ipcu_ch_init(&func, ipcu_prm) == 0 )
		{
		  mw_queue_lock = &src->queue_lock;
		  mw_queue_cond = &src->queue_cond;
		  mw_bufqueue = src->bufqueue;
		}
		else
		{
		  /* ERROR */
		  mw_queue_lock = NULL;
		  mw_queue_cond = NULL;
		  mw_bufqueue = NULL;
		  mw_ipcu_id_resp = 0xFF;
		  
		  Stream_ipcu_ch_close();
		  ret = FALSE;
		}
	}
	return ret;
}


static gboolean
gst_ipcu_src_start (GstBaseSrc * bsrc)
{
  gboolean ret = TRUE;
  if(gRTSP_func == 0) {
	//RTSP invalid
	  GstIpcuSrc *src = GST_IPCUSRC (bsrc);
	  GST_TRACE_OBJECT (src, "called");
		if( src->playctrl == FALSE )
		{
		  ret = ipcu_core_start(src);
		}
  }
	return ret;
}

static gboolean
gst_ipcu_src_stop (GstBaseSrc * bsrc)
{
  if(gRTSP_func == 0) {
	//RTSP invalid
	  INT32  ipcu_ret;
	  GstIpcuSrc *src = GST_IPCUSRC (bsrc);
	  GST_TRACE_OBJECT (src, "called");

	  if (ipcu_ret != FJ_ERR_OK) {
		// DO NOTHING
	  }
	  mw_queue_lock = NULL;
	  mw_queue_cond = NULL;
	  mw_bufqueue = NULL;

	  Stream_ipcu_ch_close();
  }
  return TRUE;
}

static gboolean
gst_ipcu_src_query (GstBaseSrc * basesrc, GstQuery * query)
{
  gboolean ret = FALSE;
  GstIpcuSrc *src = GST_IPCUSRC (basesrc);
  GST_TRACE_OBJECT (src, "GST_QUERY_TYPE_NAME=%s", GST_QUERY_TYPE_NAME (query));

  switch (GST_QUERY_TYPE (query)) {
    default:
      break;
  }

  if (!ret)
    ret = GST_BASE_SRC_CLASS (parent_class)->query (basesrc, query);

  return ret;
}

static gboolean
gst_ipcu_src_event (GstBaseSrc * basesrc, GstEvent * event)
{
  gboolean ret = FALSE;
  GstIpcuSrc *src = GST_IPCUSRC (basesrc);
  GST_TRACE_OBJECT (src, "GST_EVENT_TYPE_NAME=%s", GST_EVENT_TYPE_NAME (event));

  switch (GST_EVENT_TYPE (event)) {
    default:
     break;
  }

  if (!ret)
    ret = GST_BASE_SRC_CLASS (parent_class)->event (basesrc, event);

  return ret;
}


static gboolean
gst_ipcu_src_is_seekable (GstBaseSrc * bsrc)
{
  GstIpcuSrc *src = GST_IPCUSRC (bsrc);
  GST_TRACE_OBJECT (src, "called");
  /* This element doesn't support pull mode scheduling */
  return FALSE;
}

static gboolean
gst_ipcu_src_do_seek (GstBaseSrc * bsrc, GstSegment * segment)
{
  GstIpcuSrc *src = GST_IPCUSRC (bsrc);
  GST_TRACE_OBJECT (src, "called");

  segment->position = segment->start;
  segment->time = segment->start;

  return TRUE;
}

static GstStateChangeReturn
gst_ipcu_src_change_state (GstElement * element, GstStateChange transition)
{
  GstIpcuSrc *src = GST_IPCUSRC (element);
  GST_TRACE_OBJECT (src, "transition: %s -> %s", 
    gst_element_state_get_name(GST_STATE_TRANSITION_CURRENT(transition)),
    gst_element_state_get_name(GST_STATE_TRANSITION_NEXT(transition)));
  static char flag = 0;
  switch (transition) {
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      src->shutdown = FALSE;
      break;

    case GST_STATE_CHANGE_PAUSED_TO_READY:
      g_mutex_lock (&src->queue_lock);
      src->shutdown = TRUE;
      g_cond_signal (&src->queue_cond);
      g_mutex_unlock (&src->queue_lock);
      break;

    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
      if( src->playctrl == TRUE && flag == 0 )
      {
        flag = 1;
        ipcu_core_start(src);
        #ifdef OK_PF
        system("camera_if_direct 0x0 0xb 0x8");
        #else
        system("camera_if_direct 0x00020100 0x00000100 0x2");
        system("camera_if_direct 0x00020100 0x00000100 0x8");
        #endif
      }
      break;
    case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
      if( src->playctrl == TRUE )
      {
        flag = 0;
        #ifdef OK_PF
        system("camera_if_direct 0x0 0xb 0x9");
        #else
        system("camera_if_direct 0x00020100 0x00000100 0x2");
        system("camera_if_direct 0x00020100 0x00000100 0x1");
        #endif
      }
      break;
    case GST_STATE_CHANGE_NULL_TO_READY:
      if( src->playctrl == TRUE && flag == 0 )
      {
        flag = 1;
        ipcu_core_start(src);
        #ifdef OK_PF
        system("camera_if_direct 0x0 0xb 0x8");
        #else
        system("camera_if_direct 0x00020100 0x00000100 0x2");
        system("camera_if_direct 0x00020100 0x00000100 0x8");
        #endif
      }
      break;
    case GST_STATE_CHANGE_READY_TO_NULL:
    default:
      break;
  }

  if (GST_ELEMENT_CLASS (parent_class)->change_state)
    return GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

  return GST_STATE_CHANGE_SUCCESS;
}


static GstFlowReturn
gst_ipcu_src_create (GstPushSrc * psrc, GstBuffer ** outbuf)
{
  GstBuffer *buf = NULL;
  GstIpcuSrc *src = GST_IPCUSRC (psrc);
  GST_TRACE_OBJECT (src, "called");

  while (!buf) {
    g_mutex_lock (&src->queue_lock);
    buf = (GstBuffer *)g_queue_pop_tail(src->bufqueue);
    if (!buf) {
      g_cond_wait (&src->queue_cond, &src->queue_lock);
    }
    g_mutex_unlock (&src->queue_lock);
    
    if (src->shutdown) {
      goto shutdown;
    }
  }

  /* we're done, return the buffer */
  *outbuf = buf;
  return GST_FLOW_OK;

  /* ERRORS */
shutdown:
  {
    GST_DEBUG_OBJECT (src, "Shutting down.");
    return GST_FLOW_FLUSHING;
  }
}


/**********************************/
/* Private method implementations */
/**********************************/
int videoCallback(struct cif_stream_send* p, void *d)
{
	GstBuffer *buf;
	GstClockTime timestamp;
	UINT32 payload_pdata, payload_length;
	GstMemory *mem;
	//struct sysinfo info;
#ifdef _MEASURE_SPEC_
	int long nsec;
	clock_gettime(CLOCK_REALTIME, &meas_spec_st);
#endif
	payload_length = p->sample_size;
//	payload_pdata = p->sample_address;
	payload_pdata = FJ_MM_phys_to_virt(p->sample_address);
	if( payload_pdata != 0xFFFFFFFF && payload_length != 0xFFFFFFFF )
	{
		//sysinfo(&info);
		//printf("%d mem \n",info.freeram);
		//if( info.freeram > 40*1024*1024 )
		if( mw_bufqueue->length < 3 )
		{
			#if 0
			 /* Create buffer */
			 buf = gst_buffer_new_allocate (NULL, payload_length, NULL);
			 if (G_UNLIKELY (buf == NULL)) {
				err = FJ_ERR_NG;
			 } else {
				 /* Fill buffer */
				if( extend_flag == TRUE )
				{
				 	gst_buffer_add_cif_meta(buf, p->Sub_Command, p->stream_id);
				}
			 	gst_buffer_fill (buf, 0, (gconstpointer)payload_pdata, (gsize)payload_length);
				timestamp = (GstClockTime)(p->media.v.sample_pts[1] << 32) | p->media.v.sample_pts[0];
				GST_BUFFER_TIMESTAMP (buf) = timestamp;
				//GST_BUFFER_TIMESTAMP (buf) = GST_CLOCK_TIME_NONE;
				//g_print("%d length %d\n",__LINE__,mw_bufqueue->length);
				/* push buffer */
				g_mutex_lock (mw_queue_lock);
				g_queue_push_head(mw_bufqueue, buf);
				g_cond_signal (mw_queue_cond);
				g_mutex_unlock (mw_queue_lock);
			}
			#else
			 /* Create buffer */
			buf = gst_buffer_new();
			mem = gst_memory_new_wrapped(0, (gpointer)payload_pdata, payload_length, 0, payload_length, NULL, NULL);
			gst_buffer_append_memory (buf, mem);
			/* Fill buffer */
			if( extend_flag == TRUE )
			{
			 	gst_buffer_add_cif_meta(buf, p->Sub_Command, p->stream_id);
			}
			timestamp = ((GstClockTime)p->media.v.sample_pts[1] << 32) | p->media.v.sample_pts[0];
			if ( p->Sub_Command == 1280 ) {
				GST_BUFFER_DTS(buf) = GST_CLOCK_TIME_NONE;
				GST_BUFFER_PTS(buf) = GST_CLOCK_TIME_NONE;
				GST_BUFFER_TIMESTAMP (buf) = (timestamp *1000000) / 90;
			} else if (p->Sub_Command == 512) {
				//audio
				timestamp = ((GstClockTime)p->media.a.time_stamp[1] << 32) | p->media.a.time_stamp[0];
				GST_BUFFER_DTS(buf) = timestamp * 8 / 90;
				GST_BUFFER_PTS(buf) = GST_CLOCK_TIME_NONE;
				GST_BUFFER_TIMESTAMP (buf) = (timestamp *1000000) / 90;
			} else {
				GST_BUFFER_DTS(buf) = timestamp;
				GST_BUFFER_PTS(buf) = GST_CLOCK_TIME_NONE;
				GST_BUFFER_TIMESTAMP (buf) = (timestamp *1000000) / 90;
			}
			//g_print("%d length %d\n",__LINE__,mw_bufqueue->length);
			/* push buffer */
			g_mutex_lock (mw_queue_lock);
			g_queue_push_head(mw_bufqueue, buf);
			g_cond_signal (mw_queue_cond);
			g_mutex_unlock (mw_queue_lock);
			#endif
		}
		else
		{
			//g_print("%d free memory %d\n",__LINE__,info.freeram);
		}
	}
#ifdef _MEASURE_SPEC_
	clock_gettime(CLOCK_REALTIME, &meas_spec_ed);
	nsec = meas_spec_ed.tv_nsec - meas_spec_st.tv_nsec;
	if( nsec < 0 ) nsec += 1000000000;
	//printf("time :%d ms\n",nsec/1000);
	printf("[%d]time :%10d \n",p->stream_id,nsec);
#endif
	return 0;
}
int audioCallback(struct cif_stream_send* p, void *d)
{
	GstBuffer *buf;
	GstClockTime timestamp;
	UINT32 payload_pdata, payload_length;
	GstMemory *mem;
	unsigned short size;
	const char to_adts_ofs = 0;
	guint8 *data;
	guint16 objectType;
	guint16 samplingIdx;
	guint16 channelCfg;
	guint16 AudioSpecificConfig;
	GstMemory *AudioSpecificConfig_mem;

	payload_length = p->sample_size + to_adts_ofs;
	payload_pdata = FJ_MM_phys_to_virt(p->sample_address - to_adts_ofs);

	if( payload_pdata != 0xFFFFFFFF && payload_length != 0xFFFFFFFF )
	{
		 /* Create buffer */
		buf = gst_buffer_new();
		mem = gst_memory_new_wrapped(0, (gpointer)payload_pdata, payload_length, 0, payload_length, NULL, NULL);
		gst_buffer_append_memory (buf, mem);
		/* Fill buffer */
		if( extend_flag == TRUE )
		{
		 	gst_buffer_add_cif_meta(buf, p->Sub_Command, p->stream_id);
		}
		timestamp = ((GstClockTime)p->media.v.sample_pts[1] << 32) | p->media.v.sample_pts[0];
		GST_BUFFER_DTS(buf) = timestamp * 8 / 90;
		GST_BUFFER_PTS(buf) = GST_CLOCK_TIME_NONE;
		GST_BUFFER_TIMESTAMP (buf) = (timestamp *1000000) / 90;
		//g_print("%d length %d\n",__LINE__,mw_bufqueue->length);
		/* push buffer */
		g_mutex_lock (mw_queue_lock);
		g_queue_push_head(mw_bufqueue, buf);
		g_cond_signal (mw_queue_cond);
		g_mutex_unlock (mw_queue_lock);
		//printf("%s[%d]\n",__func__,__LINE__);
	}

	return 0;
}
int metaCB(cif_stream_meta_send* p, void *d)
{
	GstBuffer *buf;
	GstClockTime timestamp;
	UINT32 payload_pdata, payload_length;
	GstMemory *mem;
#ifdef _MEASURE_SPEC_
	int long nsec;
	clock_gettime(CLOCK_REALTIME, &meas_spec_st);
#endif
	payload_length = p->sample_size;
	//payload_pdata = p->sample_address;
	payload_pdata = FJ_MM_phys_to_virt(p->sample_address);
	if( payload_pdata != 0xFFFFFFFF && payload_length != 0xFFFFFFFF )
	{
		if( mw_bufqueue->length < 3 )
		{
			 /* Create buffer */
			buf = gst_buffer_new();
			mem = gst_memory_new_wrapped(0, (gpointer)payload_pdata, payload_length, 0, payload_length, NULL, NULL);
			gst_buffer_append_memory (buf, mem);
			/* Fill buffer */
			if( extend_flag == TRUE )
			{
			 	gst_buffer_add_cif_meta(buf, p->Sub_Command, p->stype);
			}
			timestamp = ((GstClockTime)p->sample_pts[1] << 32) | p->sample_pts[0];
			GST_BUFFER_DTS(buf) = timestamp;
			GST_BUFFER_PTS(buf) = timestamp;
			GST_BUFFER_TIMESTAMP (buf) = GST_CLOCK_TIME_NONE;
			//g_print("%d length %d\n",__LINE__,mw_bufqueue->length);
			/* push buffer */
			g_mutex_lock (mw_queue_lock);
			g_queue_push_head(mw_bufqueue, buf);
			g_cond_signal (mw_queue_cond);
			g_mutex_unlock (mw_queue_lock);
		}
		else
		{
			//g_print("%d length %d\n",__LINE__,mw_bufqueue->length);
		}
	}
#ifdef _MEASURE_SPEC_
	clock_gettime(CLOCK_REALTIME, &meas_spec_ed);
	nsec = meas_spec_ed.tv_nsec - meas_spec_st.tv_nsec;
	if( nsec < 0 ) nsec += 1000000000;
	//printf("time :%d ms\n",nsec/1000);
	printf("[%d]time :%10d \n",p->stream_id,nsec);
#endif
	return 0;
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
#if 0 //Because not using
static gboolean
ipcusrc_init (GstPlugin * ipcusrc)
{
  GST_DEBUG_CATEGORY_INIT (gst_ipcu_src_debug, "ipcusrc",
      0, "IPCU Source");

  return gst_element_register (ipcusrc, "ipcusrc", GST_RANK_NONE,
      GST_TYPE_IPCUSRC);
}
#endif
/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "ipcusrc"
#endif

/* gstreamer looks for this structure to register ipcusrc
 */
gboolean
gst_ipcu_src_plugin_init (GstPlugin * plugin)
{
  return gst_element_register (plugin, "ipcusrc",
      GST_RANK_SECONDARY, GST_TYPE_IPCUSRC);
}
