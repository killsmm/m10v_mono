/**
 * ogma_basic_access.h
 *
 *     Copyright (c) 2015 Socionext Inc.
 *     All rights reserved.
 *
 */

#ifndef OGMA_BASIC_ACCESS_H
#define OGMA_BASIC_ACCESS_H

#include "ogma_internal.h"

static __inline void ogma_write_reg (
    ogma_ctrl_t *ctrl_p,
    ogma_uint32 reg_addr,
    ogma_uint32 value
    )
{

    pfdep_iomem_write( (void *) ( ( pfdep_cpu_addr_t)ctrl_p->base_addr +
                                    reg_addr),
                       value);
}

static __inline ogma_uint32 ogma_read_reg (
    ogma_ctrl_t *ctrl_p,
    ogma_uint32 reg_addr
    ) 
{

    return ( ogma_uint32)pfdep_iomem_read( (void *)( ( pfdep_cpu_addr_t)ctrl_p->base_addr +
                                                      reg_addr));
}

void ogma_set_session_db_item (
    ogma_ctrl_t *ctrl_p,
    ogma_uint8 id,
    ogma_uint8 item_idx,
    ogma_uint32 value);

ogma_uint32 ogma_get_session_db_item (
    ogma_ctrl_t *ctrl_p,
    ogma_uint8 id,
    ogma_uint8 item_idx,
    ogma_bool clear_flag);

void ogma_set_mac_reg (
    ogma_ctrl_t *ctrl_p,
    ogma_uint32 addr,
    ogma_uint32 value);

ogma_uint32 ogma_get_mac_reg (
    ogma_ctrl_t *ctrl_p,
    ogma_uint32 addr);


#endif/* OGMA_BASIC_ACCESS_H */
