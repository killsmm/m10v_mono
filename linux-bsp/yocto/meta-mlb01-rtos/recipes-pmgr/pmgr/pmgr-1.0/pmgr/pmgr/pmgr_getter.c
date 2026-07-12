/******************************************************************************
 *
 *  @file   pmgr_getter.c
 *  @brief  Parameter Manager
 *
 *  Copyright 2018 Socionext Inc.
 ******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "pmgr_ipcu.h"
#include "pmgr_csv.h"
#include "pmgr_debug.h"
#include "pmgr_getter.h"

struct cmd_list_ctx {
    struct cvs_cmd_list *top;
    struct cvs_cmd_list *cur;
};

static void set_param(const struct conf_value *param, const struct ipcucif_recv_cmd *recv_cmd, uint32_t *result)
{
    switch (param->type) {
    case E_UINT32:
        *result = param->value.uint32;
        break;
    case E_RESULT:
        switch (param->value.result) {
        case E_RESULT_1:
            *result = recv_cmd->result[0];
            break;
        case E_RESULT_2:
            *result = recv_cmd->result[1];
            break;
        case E_RESULT_3:
            *result = recv_cmd->result[2];
            break;
        case E_RESULT_4:
            *result = recv_cmd->result[3];
            break;
        default:
            ASSERT(0);
        }
        break;
    default:
        ASSERT(0);
    }
}

static int do_get_cmd_tree(struct cmd_list_ctx *ctx, const struct conf_cmd_tree *conf_tree)
{
    const struct conf_cmd_tree *work = conf_tree;

    for (; work; work = work->next) {
        struct ipcucif_send_cmd send_cmd;
        struct ipcucif_recv_cmd recv_cmd;
        struct cvs_cmd_list *csv;

        ASSERT(work->get_cmd.cmd_id.type == E_UINT32);
        ASSERT(work->get_cmd.sub_id.type == E_UINT32);
        ASSERT(work->get_cmd.param1.type == E_UINT32);
        ASSERT(work->get_cmd.param2.type == E_UINT32);
        ASSERT(work->get_cmd.param3.type == E_UINT32);
        ASSERT(work->get_cmd.param4.type == E_UINT32);
        ASSERT(work->set_cmd.cmd_id.type == E_UINT32);
        ASSERT(work->set_cmd.sub_id.type == E_UINT32);

        memset(&send_cmd, 0, sizeof(send_cmd));
        memset(&recv_cmd, 0, sizeof(recv_cmd));

        csv = malloc(sizeof(struct cvs_cmd_list));
        CHECK_POINTER(csv);
        memset(csv, 0, sizeof(struct cvs_cmd_list));

        if (ctx->top == NULL) {
            ctx->top = csv;
        } else {
            ctx->cur->next = csv;
        }
        ctx->cur = csv;

        send_cmd.cmd_id = work->get_cmd.cmd_id.value.uint32;
        send_cmd.sub_id = work->get_cmd.sub_id.value.uint32;
        send_cmd.param[0] = work->get_cmd.param1.value.uint32;
        send_cmd.param[1] = work->get_cmd.param2.value.uint32;
        send_cmd.param[2] = work->get_cmd.param3.value.uint32;
        send_cmd.param[3] = work->get_cmd.param4.value.uint32;

        if (ipcucif_send(&send_cmd, &recv_cmd)) {
            return -1;
        }

        csv->cmd_id = work->set_cmd.cmd_id.value.uint32;
        csv->sub_id = work->set_cmd.sub_id.value.uint32;
        set_param(&work->set_cmd.param1, &recv_cmd, &csv->param1);
        set_param(&work->set_cmd.param2, &recv_cmd, &csv->param2);
        set_param(&work->set_cmd.param3, &recv_cmd, &csv->param3);
        set_param(&work->set_cmd.param4, &recv_cmd, &csv->param4);

        if (work->child) {
            if (do_get_cmd_tree(ctx, work->child)) {
                return -1;
            }
        }
    }
    return 0;
}

struct cvs_cmd_list *do_get_cmd(const struct conf_cmd_tree *conf_tree)
{
    struct cmd_list_ctx ctx;
    CHECK_POINTER(conf_tree);

    memset(&ctx, 0, sizeof(ctx));

    if (ipcucif_open()) {
        return NULL;
    }

    if (do_get_cmd_tree(&ctx, conf_tree)) {
        free_csv(ctx.top);
        ctx.top = NULL;
    }

    ipcucif_close();
    return ctx.top;
}
