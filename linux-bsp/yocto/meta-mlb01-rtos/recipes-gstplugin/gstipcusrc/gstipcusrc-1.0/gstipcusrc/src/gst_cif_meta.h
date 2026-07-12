#include <gst/gst.h>

typedef struct _CifMeta CifMeta;
struct _CifMeta {
  GstMeta       meta;
  gulong        stream;
  gulong        format;
};

GType cif_meta_api_get_type (void);
#define CIF_META_API_TYPE (cif_meta_api_get_type())

#define gst_buffer_get_cif_meta(b) \
  ((CifMeta*)gst_buffer_get_meta((b),CIF_META_API_TYPE))

/* implementation */
const GstMetaInfo *cif_meta_get_info (void);
#define CIF_META_INFO (cif_meta_get_info())

CifMeta * gst_buffer_add_cif_meta (GstBuffer *buffer, gulong format, gulong stream);