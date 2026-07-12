/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2005 Wim Taymans <wim@fluendo.com>
 *
 * gstipcusink.c:
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
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
/**
 * SECTION:element-ipcusink
 * @see_also: #GstIpcuSrc
 *
 * Dummy sink that swallows everything.
 * 
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 audiotestsrc num-buffers=1000 ! ipcusink sync=false
 * ]| Render 1000 audio buffers (of default size) as fast as possible.
 * </refsect2>
 
PC
gst-launch-1.0 audiotestsrc wave=sine ! 'audio/x-raw, format=(string)S16LE, layout=(string)interleaved, rate=(int)44100, channels=(int)2' ! audioconvert ! rtpL16pay ! udpsink host=192.168.10.16 port=5004

M10V
cp /run/SD/mmcblk0p1/usr/lib/liborc-test-0.4.so.0 /usr/lib
cp /run/SD/mmcblk0p1/usr/lib/gstreamer-1.0/libgstaudioresample.so /usr/lib/gstreamer-1.0/
cp /run/SD/mmcblk0p1/usr/lib/gstreamer-1.0/libgstaudioconvert.so /usr/lib/gstreamer-1.0/
gst-launch-1.0 udpsrc port=5004 caps="application/x-rtp,media=(string)audio,clock-rate=(int)44100,encoding-name=(string)L16,channels=(int)2,width=16,depth=16,signed=true" ! rtpL16depay ! audioconvert ! audioresample ! ipcusink
 
 
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

//#include "gstelements_private.h"
#include "gstipcusink.h"
#include <linux/ipcu_userland.h>
#include "cmfwk_std.h"
#include "cmfwk_mm.h"
#include "cmfwk_ipcu.h"
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

GST_DEBUG_CATEGORY_STATIC (gst_ipcu_sink_debug);
#define GST_CAT_DEFAULT gst_ipcu_sink_debug

#define DEFAULT_SYNC FALSE

#define DEFAULT_DUMP FALSE
#define SEND_DT_SIZE 0x1800

enum
{
  PROP_0,
  PROP_DUMP,
};

static char tx_ch = 0xFF;
static char rx_ch = 0xFF;
static int mem_fd = -1;
static unsigned long audio_va, audio_sz,audio_pa;
static unsigned long audio_ofs;
struct audio_buf_manage {
	unsigned char sample1024cnt;
	unsigned long* vaid;
	unsigned short max;
	unsigned short round;
	unsigned short rid;
	unsigned short wid;
	unsigned long sample1024adr;
	unsigned short sample1024sz;
};
struct audio_buf_manage bufmgr;
gint sample1024;

#define _do_init \
    GST_DEBUG_CATEGORY_INIT (gst_ipcu_sink_debug, "ipcusink", 0, "ipcusink element");
#define gst_ipcu_sink_parent_class parent_class
G_DEFINE_TYPE_WITH_CODE (GstIpcuSink, gst_ipcu_sink, GST_TYPE_BASE_SINK,
    _do_init);

static void gst_ipcu_sink_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_ipcu_sink_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);
static void gst_ipcu_sink_finalize (GObject * obj);

//static GstStateChangeReturn gst_ipcu_sink_change_state (GstElement * element,GstStateChange transition);
//static GstFlowReturn gst_ipcu_sink_preroll (GstBaseSink * bsink,GstBuffer * buffer);
static GstFlowReturn gst_ipcu_sink_render (GstBaseSink * bsink,GstBuffer * buffer);
static gboolean gst_ipcu_sink_event (GstBaseSink * bsink, GstEvent * event);
static gboolean gst_ipcu_sink_query (GstBaseSink * bsink, GstQuery * query);
static int audio_store_to_shared_memory(GstBuffer * buf);
static int audio_ch_open(void);
static void audio_ch_close(void);

static void
gst_ipcu_sink_class_init (GstIpcuSinkClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSinkClass *gstbase_sink_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gstelement_class = GST_ELEMENT_CLASS (klass);
  gstbase_sink_class = GST_BASE_SINK_CLASS (klass);

  gobject_class->set_property = gst_ipcu_sink_set_property;
  gobject_class->get_property = gst_ipcu_sink_get_property;
  gobject_class->finalize = gst_ipcu_sink_finalize;

  g_object_class_install_property (gobject_class, PROP_DUMP,
      g_param_spec_boolean ("dump", "Dump", "Dump buffer contents to stdout",
          DEFAULT_DUMP, G_PARAM_READWRITE | GST_PARAM_MUTABLE_PLAYING |
          G_PARAM_STATIC_STRINGS));

  gst_element_class_set_static_metadata (gstelement_class,
      "IPCU Sink",
      "Sink",
      "IPCU callback sink",
      "SOCIONEXT");

  gst_element_class_add_static_pad_template (gstelement_class, &sinktemplate);

//  gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_ipcu_sink_change_state);
  gstbase_sink_class->event = GST_DEBUG_FUNCPTR (gst_ipcu_sink_event);
//  gstbase_sink_class->preroll = GST_DEBUG_FUNCPTR (gst_ipcu_sink_preroll);
  gstbase_sink_class->render = GST_DEBUG_FUNCPTR (gst_ipcu_sink_render);
//  gstbase_sink_class->query = GST_DEBUG_FUNCPTR (gst_ipcu_sink_query);
}

static void
gst_ipcu_sink_init (GstIpcuSink * ipcusink)
{
  ipcusink->dump = DEFAULT_DUMP;
  tx_ch = 0xFF;
  rx_ch = 0xFF;
  if( audio_ch_open() == FJ_ERR_OK )
  {
    gst_base_sink_set_sync (GST_BASE_SINK (ipcusink), DEFAULT_SYNC);
      g_print("%s[%d]\n",__func__,__LINE__);
  }
  else
  {
    g_print("IPCU sink plugin init Error\n");
  }

}

static void
gst_ipcu_sink_finalize (GObject * obj)
{
  G_OBJECT_CLASS (parent_class)->finalize (obj);
    g_print("%s[%d]\n",__func__,__LINE__);
  audio_ch_close();
}

static void
gst_ipcu_sink_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstIpcuSink *sink;

  sink = GST_IPCU_SINK (object);

  switch (prop_id) {
    case PROP_DUMP:
      sink->dump = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
    g_print("%s[%d]\n",__func__,__LINE__);
}

static void
gst_ipcu_sink_get_property (GObject * object, guint prop_id, GValue * value,
    GParamSpec * pspec)
{
  GstIpcuSink *sink;

  sink = GST_IPCU_SINK (object);

  switch (prop_id) {
    case PROP_DUMP:
      g_value_set_boolean (value, sink->dump);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
    g_print("%s[%d]\n",__func__,__LINE__);
}

static gboolean
gst_ipcu_sink_event (GstBaseSink * bsink, GstEvent * event)
{
  GstIpcuSink *sink = GST_IPCU_SINK (bsink);
  GstEventType type;
  gchar *str;
  gint channels;
  gint rate;
  gint depth;

  type = GST_EVENT_TYPE (event);
  switch (type) {
    case GST_EVENT_CAPS:
    {
      GstCaps * caps;
      GstStructure *structure;
      gst_event_parse_caps (event, &caps);
      structure = gst_caps_get_structure (caps, 0);
      gst_structure_get_int (structure, "channels", &channels);
      gst_structure_get_int (structure, "rate", &rate);
      str = gst_structure_get_string (structure, "encoding-name");
      if ( str != 0 )
      {
         if( strcmp(str, "L16") == 0 )
         {
             depth = 16;
         }
         else
         {
             depth = 16;
         }
         g_print("%d %d %s\n",rate,channels,str);
      }
      else
      {
          depth = 16;
      }
      sample1024 = 1024 * (depth / 8) * channels;
      g_print("1024Sample byte %d\n",sample1024);
      break;
    }
    case GST_EVENT_SEGMENT:
    break;
    case GST_EVENT_EOS:
    break;
    default:
        GST_OBJECT_LOCK (sink);
        GST_OBJECT_UNLOCK (sink);
    break;
  }

    g_print("%s[%d]\n",__func__,__LINE__);

  return GST_BASE_SINK_CLASS (parent_class)->event (bsink, event);
}

//static GstFlowReturn
//gst_ipcu_sink_preroll (GstBaseSink * bsink, GstBuffer * buffer)
//{
//  GstIpcuSink *sink = GST_IPCU_SINK (bsink);
//
//    GST_OBJECT_LOCK (sink);
//
//    GST_OBJECT_UNLOCK (sink);
//
//  return GST_FLOW_OK;
//
//  /* ERRORS */
//  {
//    GST_DEBUG_OBJECT (sink, "we are EOS");
//    return GST_FLOW_EOS;
//  }
//}

static GstFlowReturn
gst_ipcu_sink_render (GstBaseSink * bsink, GstBuffer * buf)
{
	#if 0
  GstIpcuSink *sink = GST_IPCU_SINK_CAST (bsink);

    gchar dts_str[64], pts_str[64], dur_str[64];
    gchar *flag_str;

    GST_OBJECT_LOCK (sink);

    if (GST_BUFFER_DTS (buf) != GST_CLOCK_TIME_NONE) {
      g_snprintf (dts_str, sizeof (dts_str), "%" GST_TIME_FORMAT,
          GST_TIME_ARGS (GST_BUFFER_DTS (buf)));
    } else {
      g_strlcpy (dts_str, "none", sizeof (dts_str));
    }

    if (GST_BUFFER_PTS (buf) != GST_CLOCK_TIME_NONE) {
      g_snprintf (pts_str, sizeof (pts_str), "%" GST_TIME_FORMAT,
          GST_TIME_ARGS (GST_BUFFER_PTS (buf)));
    } else {
      g_strlcpy (pts_str, "none", sizeof (pts_str));
    }

    if (GST_BUFFER_DURATION (buf) != GST_CLOCK_TIME_NONE) {
      g_snprintf (dur_str, sizeof (dur_str), "%" GST_TIME_FORMAT,
          GST_TIME_ARGS (GST_BUFFER_DURATION (buf)));
    } else {
      g_strlcpy (dur_str, "none", sizeof (dur_str));
    }

    flag_str = gst_buffer_get_flags_string (buf);

    g_free (flag_str);
    GST_OBJECT_UNLOCK (sink);

  if (sink->dump) {
    GstMapInfo info;

    if (gst_buffer_map (buf, &info, GST_MAP_READ)) {
      gst_util_dump_mem (info.data, info.size);
      gst_buffer_unmap (buf, &info);
    }
  }

  return GST_FLOW_OK;

  /* ERRORS */
eos:
  {
    GST_DEBUG_OBJECT (sink, "we are EOS");
    return GST_FLOW_EOS;
  }
	#else
  GstIpcuSink *sink = GST_IPCU_SINK_CAST (bsink);
  GstFlowReturn flow;
  guint8 n_mem;

  n_mem = gst_buffer_n_memory (buf);

  if (n_mem > 0) {
    //flow = gst_file_sink_render_buffers (sink, &buf, 1, &n_mem, n_mem);
  } else {
    flow = GST_FLOW_OK;
  }

  //g_print("%s[%d]\n",__func__,__LINE__);
  audio_store_to_shared_memory(buf);
  return GST_FLOW_OK;

	#endif
}
static gboolean
gst_ipcu_sink_query (GstBaseSink * bsink, GstQuery * query)
{
  gboolean ret;
  GstFormat fmt;

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_SEEKING:{
      /* we don't supporting seeking */
      gst_query_parse_seeking (query, &fmt, NULL, NULL, NULL);
      gst_query_set_seeking (query, fmt, FALSE, 0, -1);
      ret = TRUE;
      break;
    }
    case GST_QUERY_FORMATS:
      gst_query_set_formats (query, 2, GST_FORMAT_DEFAULT, GST_FORMAT_BYTES);
      ret = TRUE;
      break;
    default:
      ret = GST_BASE_SINK_CLASS (parent_class)->query (bsink, query);
      break;
  }
  g_print("%s[%d]\n",__func__,__LINE__);

  return ret;
}

//static GstStateChangeReturn
//gst_ipcu_sink_change_state (GstElement * element, GstStateChange transition)
//{
//  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
//  GstIpcuSink *ipcusink = GST_IPCU_SINK (element);
//
//  switch (transition) {
//    case GST_STATE_CHANGE_NULL_TO_READY:
//    case GST_STATE_CHANGE_READY_TO_PAUSED:
//    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
//    case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
//    case GST_STATE_CHANGE_PAUSED_TO_READY:
//    case GST_STATE_CHANGE_READY_TO_NULL:
//      GST_OBJECT_LOCK (ipcusink);
//      GST_OBJECT_UNLOCK (ipcusink);
//      break;
//    default:
//      break;
//  }
//    g_print("%s[%d]\n",__func__,__LINE__);
//
//  return ret;
//
//  /* ERROR */
//error:
//  GST_ELEMENT_ERROR (element, CORE, STATE_CHANGE, (NULL),
//      ("Erroring out on state change as requested"));
//  return GST_STATE_CHANGE_FAILURE;
//}

/* gstreamer looks for this structure to register ipcusrc
 */
gboolean
gst_ipcu_sink_plugin_init (GstPlugin * plugin)
{
  return gst_element_register (plugin, "ipcusink",
      GST_RANK_SECONDARY, GST_TYPE_IPCU_SINK);
}


/* --------------------------------------------
 IPCU procedure
---------------------------------------------*/
int debug_dump(char* buf, unsigned long size)
{
	static unsigned short cnt = 0;
	FILE* fp;
	char* filename[64];
	int ret;

	sprintf(filename, "/tmp/dump_%04d.bin",cnt++);
	fp = fopen(filename, "wb");
	if (fp < 0) {
		printf("%s: failed to open %s \n", __func__, filename);
		ret = -1;
	}
	else
	{
		fwrite(buf, sizeof(char), size, fp);
		ret = 0;
	}
	fclose(fp);

	return ret;
}

void *fw_mmap(int paddr, int length)
{
	void *vaddr, *page_vaddr;
	int page_paddr;
	int page_offset;
	int page_length;
	int ret;

	mem_fd = open("/dev/mem", (O_RDWR | O_SYNC));
	if (mem_fd < 0)
	{
		g_print("%s: not ready \n", __func__);
		ret = MAP_FAILED;
	}
	else
	{
		page_paddr = paddr & PAGE_MASK;
		page_offset = paddr & ~PAGE_MASK;
		page_length = length + page_offset;

		page_vaddr = mmap(0, page_length, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, page_paddr);
		if (page_vaddr == MAP_FAILED)
		{
			ret = MAP_FAILED;
		}
		else
		{
			vaddr = page_vaddr + page_offset;
			ret = vaddr;
		}
	}

	return ret;
}
INT32 fw_munmap(void *vaddr, UINT32 length)
{
	UINT32 page_vaddr;
	UINT32 page_offset;
	UINT32 page_length;
	
	page_vaddr = (UINT32)vaddr & PAGE_MASK;
	page_offset = (UINT32)vaddr & ~PAGE_MASK;
	page_length = length + page_offset;
	close(mem_fd);

	return munmap((void*)page_vaddr, page_length);
}
#if 0
static int audio_store_to_shared_memory(GstBuffer * buf)
{
	int ret;
	gsize bufsz;
	GstMapInfo gstinfo;
	gint32 store_va;
	struct tx_ch_format
	{
		unsigned long magic_code;
		unsigned long format_version;
		unsigned long command;
		unsigned long sub_command;
		unsigned long stype;
		unsigned long sformat;
		unsigned long stream_id;
		unsigned long stream_end_flg;
		unsigned long area_index;
		unsigned long sample_size;
		unsigned long sample_address;
		unsigned long reserved1;
		unsigned long time_stamp[2];
	};
	struct tx_ch_format info = {
		.magic_code = 0xEE1111EE,
		.format_version = 0x00000001,
		.command = 0x00010100,
		.sub_command = 0x00000200,
		.stype = 1,//PCM
		.sformat = 3,//RAW
		.stream_id = 1,
		.stream_end_flg = 0,
	};

	info.time_stamp[1] = buf->dts >> 32;
	info.time_stamp[0] = buf->dts >>  0;
	info.sample_size = gst_buffer_get_size (buf);


	if( bufmgr.wid > bufmgr.max )
	{
		bufmgr.wid = 0;
		bufmgr.round++;
	}
	else
	{
		bufmgr.wid++;
	}
	if( bufmgr.round > 0 && (bufmgr.rid <= bufmgr.wid) )
	{
		g_print("Buffer-read slow!!\n");
	}
	//info.sample_address = FJ_MM_virt_to_phys(store_va);
	store_va = audio_ofs + audio_va;
	info.sample_address = (audio_pa + audio_ofs);
	if( (audio_ofs + SEND_DT_SIZE) < audio_sz )
	{
		audio_ofs += SEND_DT_SIZE;
	}
	else
	{
		audio_ofs = 0;
	}
	// Audio data copy
	gst_buffer_map(buf, &gstinfo, GST_MAP_READ);
	memcpy((int*)(int)store_va, gstinfo.data, info.sample_size);
	#if 0
	//For DEBUG Audio data
	g_print("0x%08X (va 0x%08x) - sz %d\n",info.sample_address,store_va,info.sample_size);
	g_print("%x%x\n",info.time_stamp[1],info.time_stamp[0]);
	debug_dump(((int*)(int)store_va),info.sample_size );
	//debug_dump(gstinfo.data,info.sample_size );
	#endif
	// notify to RTOS
	ret = FJ_IPCU_Send(tx_ch, (unsigned long)&info, sizeof(struct tx_ch_format),IPCU_SEND_SYNC);
	if (ret != FJ_ERR_OK)
	{
		g_print("Audio Send error Error\n",tx_ch);
	}
	else
	{
		#if 0
		{
			// For DEBUG ipcu tx
			char* read_ptr;
			int dbgi;
			read_ptr = (unsigned long*)fw_mmap(0x4FE48000, 32*1024);
			g_print("va 0x%08x\n",read_ptr);
			g_print("\n -- ipcu ch9 \n");
			for( dbgi = 0; dbgi < sizeof(struct tx_ch_format)/4; dbgi++ )
			{
				g_print("0x%08x\n",*((unsigned long*)read_ptr + dbgi));
			}
			fw_munmap(read_ptr,32*1024);
		}
		#endif
//		int i;
//		for( i = 0; i < sizeof(struct tx_ch_format)/4; i++ )
//		{
//			g_print("0x%08X\n",*((int*)(int)store_va + i));
//		}
	}
	return ret;

}
#else
static int audio_store_to_shared_memory(GstBuffer * buf)
{
	int ret;
	gsize bufsz;
	GstMapInfo gstinfo;
	gint32 store_va;
	struct tx_ch_format
	{
		unsigned long magic_code;
		unsigned long format_version;
		unsigned long command;
		unsigned long sub_command;
		unsigned long stype;
		unsigned long sformat;
		unsigned long stream_id;
		unsigned long stream_end_flg;
		unsigned long area_index;
		unsigned long sample_size;
		unsigned long sample_address;
		unsigned long reserved1;
		unsigned long time_stamp[2];
	};
	struct tx_ch_format info = {
		.magic_code = 0xEE1111EE,
		.format_version = 0x00000001,
		.command = 0x00010100,
		.sub_command = 0x00000200,
		.stype = 1,//PCM
		.sformat = 3,//RAW
		.stream_id = 1,
		.stream_end_flg = 0,
	};

	info.time_stamp[1] = buf->dts >> 32;
	info.time_stamp[0] = buf->dts >>  0;
	info.sample_size = gst_buffer_get_size (buf);
	//g_print("0x%08X%08x\n",info.time_stamp[1],info.time_stamp[0]);

	store_va = bufmgr.sample1024adr + bufmgr.sample1024sz;
	gst_buffer_map(buf, &gstinfo, GST_MAP_READ);
	memcpy((int*)(int)store_va, gstinfo.data, info.sample_size);
	gst_buffer_unmap(buf,&gstinfo);
	bufmgr.sample1024sz += info.sample_size;
	bufmgr.sample1024cnt++;
	info.sample_address = (audio_pa + audio_ofs);
	//g_print("0x%08X, 0x%08x\n",bufmgr.sample1024adr, bufmgr.sample1024sz);

	if( bufmgr.sample1024sz >= sample1024 )
	{
		bufmgr.sample1024sz = sample1024;
		info.sample_size = bufmgr.sample1024sz;
		//g_print("[%d] 0x%08X, 0x%08x\n",__LINE__,info.sample_address, info.sample_size);
		// notify to RTOS
		ret = FJ_IPCU_Send(tx_ch, (unsigned long)&info, sizeof(struct tx_ch_format),IPCU_SEND_SYNC);
		if (ret != FJ_ERR_OK)
		{
			g_print("Audio Send error Error\n");
		}
		else
		{
		}
		if( bufmgr.wid > bufmgr.max )
		{
			bufmgr.wid = 0;
			bufmgr.round++;
		}
		else
		{
			bufmgr.wid++;
		}
		if( bufmgr.round > 0 && (bufmgr.rid <= bufmgr.wid) )
		{
			g_print("Buffer-read slow!!\n");
		}
		store_va = audio_ofs + audio_va;
		if( (audio_ofs + SEND_DT_SIZE) < audio_sz )
		{
			audio_ofs += SEND_DT_SIZE;
		}
		else
		{
			audio_ofs = 0;
		}
		bufmgr.sample1024cnt = 0;
		bufmgr.sample1024sz = 0;
		bufmgr.sample1024adr = store_va;
	}
	return ret;

}
#endif
static void audio_response_callback(unsigned char id,unsigned long pdata, unsigned long length, unsigned long cont, unsigned long total_length)
{
	struct rx_ch_format
	{
		unsigned long magic_code;
		unsigned long format_version;
		unsigned long command;
		unsigned long sub_command;
		unsigned long stype;
		unsigned long sformat;
		unsigned long stream_id;
		unsigned long stream_end_flg;
		unsigned long area_index;
	};
	const struct rx_ch_format rx_info = {
		.magic_code = 0xEE2222EE,
		.format_version = 0x00000001,
		.command = 0x00010100,
		.sub_command = 0x00000200,
		.stype = 1,//PCM
		.sformat = 3,//RAW
		.stream_end_flg = 0,
	};
	if( *((unsigned long*)pdata + 0) != rx_info.magic_code )
	{
		g_print("Wrong magic_code 0x%08x\n",*((unsigned long*)pdata + 0));
	}
	if( *((unsigned long*)pdata + 1) != rx_info.format_version )
	{
		g_print("Wrong format_version 0x%08x\n",*((unsigned long*)pdata + 1));
	}
	if( *((unsigned long*)pdata + 2) != rx_info.command )
	{
		g_print("Wrong command 0x%08x\n",*((unsigned long*)pdata + 2));
	}
	if( *((unsigned long*)pdata + 3) != rx_info.sub_command )
	{
		g_print("Wrong sub_command 0x%08x\n",*((unsigned long*)pdata + 3));
	}
	if( *((unsigned long*)pdata + 4) != rx_info.stype )
	{
		g_print("Wrong stype 0x%08x\n",*((unsigned long*)pdata + 4));
	}
	if( *((unsigned long*)pdata + 5) != rx_info.sformat )
	{
		g_print("Wrong sformat 0x%08x\n",*((unsigned long*)pdata + 5));
	}
	if( bufmgr.rid > bufmgr.max )
	{
		bufmgr.rid = 0;
		if( bufmgr.round > 0 )
		{
			bufmgr.round--;
		}
	}
	else
	{
		bufmgr.rid++;
	}


}
static int audio_ch_open(void)
{
	int ret;
	int i;
	int a_sz;

	ret = FJ_ERR_NG;
	/* Initialize IPCU */
	ret = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_9, (unsigned char*)&tx_ch);
	if (ret != FJ_ERR_OK)
	{
		g_print("IPCU Tx [%d]ch open Error\n",tx_ch);
		tx_ch = 0xFF;
		ret = FJ_ERR_NG;
	}
	else
	{
		ret = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_8, (unsigned char*)&rx_ch);
		if (ret != FJ_ERR_OK)
		{
			g_print("IPCU Rx [%d]ch open Error\n",rx_ch);
			FJ_IPCU_Close(tx_ch);
			rx_ch = 0xFF;
			ret = FJ_ERR_NG;
		}
		else
		{
			ret = FJ_IPCU_SetReceiverCB(rx_ch, audio_response_callback);
			if (ret != FJ_ERR_OK)
			{
				g_print("IPCU Rx ch Callback regist Error\n");
				FJ_IPCU_Close(tx_ch);
				FJ_IPCU_Close(rx_ch);
				tx_ch = 0xFF;
				rx_ch = 0xFF;
				ret = FJ_ERR_NG;
			}
			else
			{
				FJ_MM_getPhysAddrInfo(IPCU_MM_AOUT, &audio_pa, &audio_sz);
				audio_va = (unsigned long)fw_mmap(audio_pa, audio_sz);
				audio_ofs = 0;

				a_sz = 0;
				bufmgr.max = 0;
				bufmgr.rid = 0;
				bufmgr.wid = 0;
				bufmgr.round = 0;
				bufmgr.sample1024cnt = 0;
				bufmgr.sample1024adr = audio_va;
				bufmgr.sample1024sz = 0;
				for( i = 0; i < 2048; i++ )
				{
					if( a_sz >= audio_sz )
					{
						break;
					}
					else
					{
						a_sz += SEND_DT_SIZE;
						bufmgr.max = i;
					}
				}
				//bufmgr.vaid = (unsigned long *)malloc(bufmgr.max * sizeof(int));
				//if(bufmgr.vaid == NULL) {
				//	g_print("malloc error\n");
				//}
				ret = FJ_ERR_OK;
			}
		}

	}

	return ret;
}

static void audio_ch_close(void)
{
	if (tx_ch != 0xFF)
	{
		FJ_IPCU_Close(tx_ch);
	}
	if (rx_ch != 0xFF)
	{
		FJ_IPCU_Close(rx_ch);
	}
	fw_munmap(audio_va, audio_sz);
	tx_ch = 0xFF;
	rx_ch = 0xFF;
}
