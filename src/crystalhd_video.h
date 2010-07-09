/*
 * Copyright (c) 2010 iWorldcom Co., Ltd. All Rights Reserved.
 *
 *    - Victor Tseng <palatis@iworldcom.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _CRYSTALHD_DRIVER_H_
#define _CRYSTALHD_DRIVER_H_

#include <va/va.h>
#include <va/va_backend.h>

#include <libcrystalhd/bc_dts_types.h>
#include <libcrystalhd/bc_dts_defs.h>
#include <libcrystalhd/libcrystalhd_if.h>
#include <libcrystalhd/libcrystalhd_version.h>

#include "object_heap.h"

#define DTS_OUTPUT_TIMEOUT			1000

#define CRYSTALHD_MAX_PROFILES			11
#define CRYSTALHD_MAX_ENTRYPOINTS		5
#define CRYSTALHD_MAX_CONFIG_ATTRIBUTES		10
#define CRYSTALHD_MAX_IMAGE_FORMATS		5
#define CRYSTALHD_MAX_SUBPIC_FORMATS		4
#define CRYSTALHD_MAX_DISPLAY_ATTRIBUTES	4
#define CRYSTALHD_STR_VENDOR			"Broadcom Crystal HD Video Decoder " RC_PRODUCT_VERSION

#define ALIGN(i, n)		(((i) + (n) - 1) & ~((n) - 1))
#define STRIDE(w)		(((w) + 0xf) & ~0xf)
#define SIZE_YUV420(w, h)	((h) * (STRIDE(w) + STRIDE((w) >> 1)))

struct crystalhd_driver_data
{
	struct object_heap	config_heap;
	struct object_heap	context_heap;
	struct object_heap	surface_heap;
	struct object_heap	buffer_heap;
	struct object_heap	image_heap;

	HANDLE			hdev;
};

struct object_config {
	struct object_base base;
	VAProfile profile;
	VAEntrypoint entrypoint;
	VAConfigAttrib attrib_list[CRYSTALHD_MAX_CONFIG_ATTRIBUTES];
	int attrib_count;
};

struct object_context {
	struct object_base base;
	VAContextID context_id;
	VAConfigID config_id;
	VASurfaceID current_render_target;
	int picture_width;
	int picture_height;
	int num_render_targets;
	int flags;
	VASurfaceID *render_targets;
};

struct object_surface {
	struct object_base base;
	VASurfaceID surface_id;
	uint8_t *metadata;
	int metadata_size;
	uint8_t *data;
	int data_size;
	VAImageID output_image_id;
};

struct object_buffer {
	struct object_base base;
	VABufferType type;
	void *buffer_data;
	int element_size;
	int num_elements;
	int max_num_elements;
};

struct object_image {
	struct object_base base;
	VAImage image;
	unsigned int *palette;
};

typedef struct object_config *object_config_p;
typedef struct object_context *object_context_p;
typedef struct object_surface *object_surface_p;
typedef struct object_buffer *object_buffer_p;
typedef struct object_image *object_image_p;

static inline struct crystalhd_driver_data *
crystalhd_driver_data(VADriverContextP ctx)
{
	return (struct crystalhd_driver_data *)ctx->pDriverData;
}

#endif
