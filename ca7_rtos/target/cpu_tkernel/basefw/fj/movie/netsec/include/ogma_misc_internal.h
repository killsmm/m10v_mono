/**
 * ogma_misc_internal.h
 *
 *     Copyright (c) 2015 Socionext Inc.
 *     All rights reserved.
 *
 */

#ifndef OGMA_MISC_INTERNAL_H
#define OGMA_MISC_INTERNAL_H

#include "ogma_basic_type.h"

#define OGMA_ENC_BASIC_DB_ITEM_NUM  22
#define OGMA_DEC_BASIC_DB_ITEM_NUM  29
#define OGMA_ENC_CONTEXT_DB_ITEM_NUM 27
#define OGMA_DEC_CONTEXT_DB_ITEM_NUM 27
#define OGMA_ARC4DB_ITEM_NUM        64

typedef struct ogma_global_s ogma_global_t;

struct ogma_global_s{
    ogma_uint valid_flag:1;

    ogma_uint8 list_entry_num;

    ogma_ctrl_t *list_head_p;
};

#endif /* OGMA_MISC_INTERNAL_H */
