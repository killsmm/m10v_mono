/**
 * ogma_basic_access.c
 *
 *     Copyright (c) 2015 Socionext Inc.
 *     All rights reserved.
 *
 */

#include "ogma_internal.h"
#include "ogma_basic_access.h"


/**********************************************************************
 * Function definitions
 **********************************************************************/

void ogma_set_session_db_item (
    ogma_ctrl_t *ctrl_p,
    ogma_uint8 id,
    ogma_uint8 item_idx,
    ogma_uint32 value)
{
    ogma_uint32 cmd;

    /*
     * Argument check is omitted because this function is
     * of private use only.
     */

    ogma_write_reg( ctrl_p, OGMA_REG_ADDR_SDB_DATA, value);

	cmd = (( id << 12) | ( item_idx << 8) | OGMA_DB_CMD_ST_WRITE);

    ogma_write_reg(ctrl_p, OGMA_REG_ADDR_SDB_CMD_ST, cmd);

    while ( ( ogma_read_reg( ctrl_p, OGMA_REG_ADDR_SDB_CMD_ST)
              & OGMA_DB_CMD_ST_BUSY)
            != 0) {
        ;
    }
}

ogma_uint32 ogma_get_session_db_item (
    ogma_ctrl_t *ctrl_p,
    ogma_uint8 id,
    ogma_uint8 item_idx,
    ogma_bool clear_flag)
{
    ogma_uint32 cmd;
    ogma_uint32 rd_clr = clear_flag ? 0x1U : 0;

    /*
     * Argument check is omitted because this function is
     * of private use only.
     */

	cmd = (( id << 12) | ( item_idx << 8) | ( rd_clr << 1) | OGMA_DB_CMD_ST_READ);
	
    ogma_write_reg( ctrl_p, OGMA_REG_ADDR_SDB_CMD_ST, cmd);

    while (( ogma_read_reg( ctrl_p, OGMA_REG_ADDR_SDB_CMD_ST)
			 & OGMA_DB_CMD_ST_BUSY)
		   != 0) {
        ;
    }

    return ogma_read_reg( ctrl_p, OGMA_REG_ADDR_SDB_DATA);
}
