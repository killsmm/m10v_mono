/*
 * (C) Copyright 2013 Andy Green <andy.green@linaro.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "asm/utils.h"
#include "mmc.h"
#include "fat.h"
#include "block.h"
#include "string.h"

#ifdef CONFIG_DRIVER_BMPRLE8

struct bmp_header {
	char magic[2]; /* 'B', 'M' */
	u32 length;
	u16 reserved[2];
	u32 data_offset;
} __attribute__ ((packed));

struct dibv3 {
	u32 length;
	u32 width;
	u32 height;
	u16 junk_1;
	u16 bpp;
	u32 compression; /* RLE8 = 1 */
	u32 pixel_data_length;
	u32 junk_2[2];
	u32 count_colours;
	u32 junk_3;
} __attribute__ ((packed));

static u32 una_u32(u32 *p)
{
	u8 *b = (u8 *)p;

	return b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
}

static u16 una_u16(u16 *p)
{
	u8 *b = (u8 *)p;

	return b[0] | (b[1] << 8);
}

/*
 * Compressed BMP decoder
 *
 * Tested with 768 x 1280 bitmap exported by GIMP, 256 colour palette, RLE
 * 
 * Restrictions
 *
 * 256 colour palettized bitmap
 * RLE8 coding
 * width divisible by 4
 * Only 32bpp framebuffer
 * BMP must be same dimensions as framebuffer (avoid attacks around that)
 * May overwrite max 253 bytes past fb end, accepted for speed
 *
 * Notice header members are unaligned and must be accessed via wrappers.
 */

int unpack_rle256_bmp(void *in, void *fb32, int fb_width, int fb_height)
{
	struct bmp_header *h = in;
	struct dibv3 *dib = in + sizeof(*h);
	u32 px, *palette32 = (void *)dib + una_u32(&dib->length);
#if CONFIG_SPLASH_FB_BPP == 16
	u16 *line, *limit, *start, *palette = in;
#else
	u32 *line, *limit, *start, *palette = in;
#endif

	u8 *data = (void *)(palette + una_u32(&dib->count_colours));
	int y, n, run = 0, unencoded = 0, skip_after_run = 0;
	char doneline;

	limit = fb32 + (fb_height * fb_width * sizeof(*line));
	start = limit - fb_width;

	/* check the BMP is usable */
	if (h->magic[0] != 'B' || h->magic[1] != 'M' || (void *)start < fb32 ||
		una_u16(&dib->bpp) != 8 || una_u32(&dib->compression) != 1 ||
			una_u32(&dib->count_colours) > 256 ||
			una_u32(&dib->width) != fb_width ||
					    una_u32(&dib->height) != fb_height)
		return 1;

	/*
 	 * correct alignment for palette lookups: overwrite BMP header
 	 * so at this point can't use header members any more
 	 */
	n = una_u32(&dib->count_colours);
	for (y = 0; y < n; y++)
#if CONFIG_SPLASH_FB_BPP == 16
		palette[y] =(((una_u32(&palette32[y]) >>  3) & 0x1f) << 0) |
			    (((una_u32(&palette32[y]) >> 10) & 0x3f) << 5) |
			    (((una_u32(&palette32[y]) >> 19) & 0x1f) << 11);
#else
		palette[y] = una_u32(&palette32[y]);
#endif

	/* BMP is stored bottom - top */
	for (y = fb_height - 1; y >= 0; y--) {

		line = start;
		doneline = 0;

		while (!doneline) {

			if (run--) {
				if (unencoded)
					*line++ = palette[*data++];
				else
					*line++ = px;
				continue;
			}
			if (skip_after_run) {
				skip_after_run = 0;
				data++;
			}
			/*
			 * to avoid checking inside the fast loop,
			 * accept up to 253 byte overwrite past fb end
			 */
			if (data > (u8 *)limit)
				return 3;

			if (*data) {
				unencoded = 0;
				run = *data++;
				px = palette[*data++];
				continue;
			}
			data++;
			run = *data++;

			switch (run) {
			case 0: /* end of scan line */
				run = 0;
				doneline = 1;
				continue;
			case 1: /* end of RLE data */
				goto bail;
			case 2: /* delta x y */
				run = 0;
				line += *data++;
				start -= *data * fb_width;
				/* watch for being attacked */
				if ((void *)start < fb32)
					return 3;
				line -= *data * fb_width;
				/* we can even be attacked like this */
				if ((void *)line < fb32)
					return 3;
				y -= *data++;
				continue;
			default:
				unencoded = 1;
				skip_after_run = run & 1;
				continue;
			}
		}

		start -= fb_width;
	}
bail:
	/* flush bitmap data hanging around in processor cache */

//	flush_dcache_range(fb32, fb32 + (fb_width * fb_height));

	return 0;
}
#endif
