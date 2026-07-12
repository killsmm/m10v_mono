#include "gst_cif_meta.h"

static gboolean
cif_meta_init (GstMeta * meta, gpointer params, GstBuffer * buffer)
{
  CifMeta *emeta = (CifMeta *) meta;

  emeta->stream = 0;
  emeta->format = 0;

  return TRUE;
}
static void
cif_meta_free (GstMeta * meta, GstBuffer * buffer)
{
  // if a buf allocate, free.
}
static gboolean
cif_meta_transform (GstBuffer * transbuf, GstMeta * meta,
    GstBuffer * buffer, GQuark type, gpointer data)
{
  CifMeta *emeta = (CifMeta *) meta;

  gst_buffer_add_cif_meta (transbuf, emeta->stream, emeta->format);

  return TRUE;
}
GType
cif_meta_api_get_type (void)
{
  static volatile GType type;
  static const gchar *tags[] = { "foo", "bar", NULL };

  if (g_once_init_enter (&type)) {
    GType _type = gst_meta_api_type_register ("CifMetaAPI", tags);
    g_once_init_leave (&type, _type);
  }
  return type;
}
const GstMetaInfo *
cif_meta_get_info (void)
{
  static const GstMetaInfo *meta_info = NULL;

  if (g_once_init_enter (&meta_info)) {
    const GstMetaInfo *mi = gst_meta_register (CIF_META_API_TYPE,
        "CifMeta",
        sizeof (CifMeta),
        cif_meta_init,
        cif_meta_free,
        cif_meta_transform);
    g_once_init_leave (&meta_info, mi);
  }
  return meta_info;
}
/* Public */
CifMeta *
gst_buffer_add_cif_meta (GstBuffer *buffer,
                                gulong format,
                                gulong stream)
{
  CifMeta *meta;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), NULL);

  meta = (CifMeta *) gst_buffer_add_meta (buffer,
      CIF_META_INFO, NULL);

  meta->stream = stream;
  meta->format = format;

  return meta;
}

