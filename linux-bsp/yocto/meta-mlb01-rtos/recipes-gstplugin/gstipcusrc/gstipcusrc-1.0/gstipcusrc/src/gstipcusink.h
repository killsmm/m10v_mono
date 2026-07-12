/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wtay@chello.be>
 *
 * gstipcusink.h:
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


#ifndef __GST_IPCU_SINK_H__
#define __GST_IPCU_SINK_H__

#include <gst/gst.h>
#include <gst/base/gstbasesink.h>

G_BEGIN_DECLS


#define GST_TYPE_IPCU_SINK \
  (gst_ipcu_sink_get_type())
#define GST_IPCU_SINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_IPCU_SINK,GstIpcuSink))
#define GST_IPCU_SINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_IPCU_SINK,GstIpcuSinkClass))
#define GST_IS_IPCU_SINK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_IPCU_SINK))
#define GST_IS_IPCU_SINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_IPCU_SINK))
#define GST_IPCU_SINK_CAST(obj) ((GstIpcuSink *)obj)


typedef struct _GstIpcuSink GstIpcuSink;
typedef struct _GstIpcuSinkClass GstIpcuSinkClass;

/**
 * GstIpcuSink:
 *
 * The opaque #GstIpcuSink data structure.
 */
struct _GstIpcuSink {
  GstBaseSink		element;

  gboolean      dump;
  gint          num_buffers_left;
};

struct _GstIpcuSinkClass {
  GstBaseSinkClass parent_class;
};

G_GNUC_INTERNAL GType gst_ipcu_sink_get_type (void);
gboolean gst_ipcu_sink_plugin_init (GstPlugin * plugin);

G_END_DECLS

#endif /* __GST_IPCU_SINK_H__ */
