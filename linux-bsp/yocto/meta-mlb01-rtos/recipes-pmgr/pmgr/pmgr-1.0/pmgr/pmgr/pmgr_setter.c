/******************************************************************************
 *
 *  @file   pmgr_setter.c
 *  @brief  Parameter Manager
 *
 *  Copyright 2018 Socionext Inc.
 ******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "pmgr_ipcu.h"
#include "pmgr_debug.h"
#include "pmgr_setter.h"

int do_set_cmd(const struct cvs_cmd_list *cvs_list)
{
    const struct cvs_cmd_list *work = cvs_list;

    CHECK_POINTER(cvs_list);

    if (ipcucif_open()) {
        return -1;
    }

    for (; work; work = work->next) {
        struct ipcucif_send_cmd send_cmd;
        struct ipcucif_recv_cmd recv_cmd;

        memset(&send_cmd, 0, sizeof(send_cmd));
        memset(&recv_cmd, 0, sizeof(recv_cmd));

        send_cmd.cmd_id = work->cmd_id;
        send_cmd.sub_id = work->sub_id;
        send_cmd.param[0] = work->param1;
        send_cmd.param[1] = work->param2;
        send_cmd.param[2] = work->param3;
        send_cmd.param[3] = work->param4;

        if (ipcucif_send(&send_cmd, &recv_cmd)) {
            goto error;
        }
        if (recv_cmd.result[0] != 0x00000000) {
            ERROR_LOG("set command error.(0x%08x-0x%08x-0x%08x-0x%08x-0x%08x-0x%08x: 0x%08x-0x%08x)",
                      send_cmd.cmd_id, send_cmd.sub_id,
                      send_cmd.param[0], send_cmd.param[1], send_cmd.param[2], send_cmd.param[3],
                      recv_cmd.result[0], recv_cmd.result[1]);
        }
    }

    ipcucif_close();
    return 0;

error:
    ipcucif_close();
    return -1;
}
