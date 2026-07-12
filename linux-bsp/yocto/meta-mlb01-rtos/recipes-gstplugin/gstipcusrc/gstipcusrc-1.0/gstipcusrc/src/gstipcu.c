/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/tag/tag.h>
#include "gstipcusrc.h"
#include "gstipcudemux.h"
#include "gstipcusink.h"

static gboolean
plugin_init (GstPlugin * plugin)
{
  //gst_tag_image_type_get_type ();

  if (!gst_ipcu_src_plugin_init (plugin))
    return FALSE;

  if (!gst_ipcu_demux_plugin_init (plugin))
    return FALSE;

  if (!gst_ipcu_sink_plugin_init (plugin))
    return FALSE;

  return TRUE;

}

GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    ipcu,
    "Socionext IPCU protocol plugins",
    plugin_init, 
    VERSION, 
    "LGPL", 
    "GStreamer",
    "http://gstreamer.net/"
);
