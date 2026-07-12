/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2017  <<user@hostname.org>>
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
 * SECTION:element-ipcudemux
 *
 * FIXME:Describe ipcudemux here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! ipcudemux ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <gst/base/base.h>
#include "gstipcudemux.h"
#include "gst_cif_meta.h"//FIXME
GST_DEBUG_CATEGORY_STATIC (gst_ipcu_demux_debug);
#define GST_CAT_DEFAULT gst_ipcu_demux_debug
static int is_pcm;
/* Filter signals and args */
enum
{
	PROP_0,
	PROP_STREAM,
	PROP_FORMAT,
	PROP_RATE,
	PROP_CHANNEL,
};
enum
{
	IPCUFMT_H264 = 0,
	IPCUFMT_HEVC = 1,
	IPCUFMT_AAC  = 20,
	IPCUFMT_RAW  = 30,
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

#define gst_ipcu_demux_parent_class parent_class
G_DEFINE_TYPE (GstIpcuDemux, gst_ipcu_demux, GST_TYPE_ELEMENT);

static void gst_ipcu_demux_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_ipcu_demux_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static gboolean gst_ipcu_demux_sink_event (GstPad * pad, GstObject * parent, GstEvent * event);
static GstFlowReturn gst_ipcu_demux_chain (GstPad * pad, GstObject * parent, GstBuffer * buf);

/* GObject vmethod implementations */

/* initialize the ipcudemux's class */
static void
gst_ipcu_demux_class_init (GstIpcuDemuxClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
    /*check aac or pcm*/
  FILE *file;
  char buffer[10];

  file = fopen("/tmp/pcm.conf", "r");
  if(file !=NULL) {
    is_pcm = 1;
    fclose(file);
  } else {
    is_pcm = 0;
  }
  printf("%s is_pcm:%d \n", __func__, is_pcm);

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_ipcu_demux_set_property;
  gobject_class->get_property = gst_ipcu_demux_get_property;

  g_object_class_install_property (gobject_class, PROP_STREAM,
      g_param_spec_ulong ("stream", 
      "Demux stream_id", 
      "Specify to pull out ID",
      0,0xFFFFFFFF,0, G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_FORMAT,
      g_param_spec_ulong ("format", 
      "Demux format(Sub_Command)", 
      "Specify to pull out format",
      0,0xFFFFFFFF,0, G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_RATE,
      g_param_spec_ulong ("rate", 
      "sampling rate for audio", 
      "Specify to pull out rate",
      0,0xFFFFFFFF,0, G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_CHANNEL,
      g_param_spec_ulong ("channel", 
      "number of channel for audio", 
      "Specify to pull out channel",
      0,0xFFFFFFFF,0, G_PARAM_READWRITE));

  gst_element_class_set_static_metadata (gstelement_class, "ipcudemux",
    "FIXME:Generic",
    "FIXME:Generic Template Element",
    " <<user@hostname.org>>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_ipcu_demux_init (GstIpcuDemux * filter)
{
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_pad_set_event_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_ipcu_demux_sink_event));
  gst_pad_set_chain_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_ipcu_demux_chain));
  GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  GST_PAD_SET_PROXY_CAPS (filter->srcpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

}

static void
gst_ipcu_demux_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstIpcuDemux *filter = GST_IPCUDEMUX (object);

  switch (prop_id) {
    case PROP_STREAM:
      filter->stream = g_value_get_ulong (value);
      //g_print("stream %d\n",filter->stream);
      break;
    case PROP_FORMAT:
      filter->format = g_value_get_ulong (value);
      //g_print("format %d\n",filter->format);
      break;
    case PROP_RATE:
      filter->rate = g_value_get_ulong (value);
      break;
    case PROP_CHANNEL:
      filter->channel = g_value_get_ulong (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_ipcu_demux_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstIpcuDemux *filter = GST_IPCUDEMUX (object);

  switch (prop_id) {
    case PROP_STREAM:
      g_value_set_ulong (value, filter->stream);
      break;
    case PROP_FORMAT:
      g_value_set_ulong (value, filter->format);
      break;
    case PROP_RATE:
      g_value_set_ulong (value, filter->rate);
      break;
    case PROP_CHANNEL:
      g_value_set_ulong (value, filter->channel);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean
gst_ipcu_demux_sink_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  GstIpcuDemux *filter;
  gboolean ret;

  filter = GST_IPCUDEMUX (parent);

  GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
      GST_EVENT_TYPE_NAME (event), event);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps * caps;

      gst_event_parse_caps (event, &caps);
      /* do something with the caps */

      /* and forward */
      //ret = gst_pad_event_default (pad, parent, event);
      gst_event_unref (event);
      break;
    }
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}
static _get_rate_index (gint rate)
{
	static const gint rate_table[] = {
		96000, 88200, 64000, 48000, 44100, 32000,
		24000, 22050, 16000, 12000, 11025, 8000
	};
	gint i;
	for (i = 0; i < G_N_ELEMENTS (rate_table); ++i) {
		if (rate == rate_table[i]) {
			return i;
		}
	}
	return -1;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn
gst_ipcu_demux_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
  GstIpcuDemux *filter = GST_IPCUDEMUX (parent);
  CifMeta *meta;
  gint ret;

  ret = GST_FLOW_OK;
  meta = gst_buffer_get_cif_meta(buf);
  if( meta == NULL ){
    printf("%s[%d] Meta is not found\n",__func__,__LINE__);
  }else{

    if( meta->format == filter->format && meta->stream == filter->stream )
    {
       if (filter->format == 512) {
			GstBuffer *codec_data;
			GstMapInfo map;
			GstCaps *src_caps;
			GstStructure *s;
			gint index;
			gint rate,channel;

			if( is_pcm == 1){
				src_caps = gst_caps_new_empty_simple ("audio/x-raw");
				s = gst_caps_get_structure (src_caps, 0);
				gst_structure_set (s, "format", G_TYPE_STRING, "S16LE", NULL);
				gst_structure_set (s, "layout", G_TYPE_STRING, "interleaved", NULL);

				rate = filter->rate;
				channel = filter->channel;
				gst_structure_set (s, "rate", G_TYPE_INT, rate, NULL);
				gst_structure_set (s, "channels", G_TYPE_INT, channel, NULL);
				gst_pad_set_caps (GST_BASE_PARSE_SRC_PAD(filter), src_caps);
				gst_caps_unref (src_caps);
		   } else {
				src_caps = gst_caps_new_empty_simple ("audio/mpeg");
				s = gst_caps_get_structure (src_caps, 0);
				rate = filter->rate;
				channel = filter->channel;
				gst_structure_set (s, "mpegversion", G_TYPE_INT, 4, NULL);
				gst_structure_set (s, "rate", G_TYPE_INT, rate, NULL);
				gst_structure_set (s, "channels", G_TYPE_INT, channel, NULL);
				gst_structure_set (s, "encoding-name", G_TYPE_STRING, "MP4A-LATM", NULL);
				codec_data = gst_buffer_new_and_alloc (2);
				gst_buffer_map (codec_data, &map, GST_MAP_WRITE);
				index = _get_rate_index(rate);
				map.data[0] = ((0x02 << 3) | (index >> 1));
				map.data[1] = ((index & 0x01) << 7) | (channel << 3);
				gst_buffer_unmap (codec_data, &map);
				gst_structure_set (s, "codec_data", GST_TYPE_BUFFER, codec_data, NULL);//voaacenc
				gst_buffer_unref (codec_data);
				gst_pad_set_caps (GST_BASE_PARSE_SRC_PAD(filter), src_caps);
				gst_caps_unref (src_caps);
		   }
		}
		ret = gst_pad_push (filter->srcpad, buf);
	}
    else
    {
      gst_buffer_unref (buf);
      ret = GST_FLOW_OK;
    }
  }
  return ret;
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
ipcudemux_init (GstPlugin * ipcudemux)
{
  /* debug category for fltering log messages
   *
   * exchange the string 'Template ipcudemux' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_ipcu_demux_debug, "ipcudemux",
      0, "Template ipcudemux");



  return gst_element_register (ipcudemux, "ipcudemux", GST_RANK_NONE,
      GST_TYPE_IPCUDEMUX);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstipcudemux"
#endif

/* gstreamer looks for this structure to register myfilters
 *
 * exchange the string 'Template ipcudemux' with your ipcudemux description
 */
 
gboolean
gst_ipcu_demux_plugin_init (GstPlugin * plugin)
{
  return gst_element_register (plugin, "ipcudemux",
      GST_RANK_SECONDARY, GST_TYPE_IPCUDEMUX);
}
