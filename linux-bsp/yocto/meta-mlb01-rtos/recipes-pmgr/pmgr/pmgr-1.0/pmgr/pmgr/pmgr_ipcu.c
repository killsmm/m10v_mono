/******************************************************************************
 *
 *  @file   pmgr_ipcu.c
 *  @brief  Parameter Manager
 *
 *  Copyright 2018 Socionext Inc.
 ******************************************************************************/
#include "pmgr_ipcu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include "pmgr_debug.h"
#ifndef PMGR_DEBUG_LOCAL_PC
#include "cmfwk_ipcu.h"
#include "pmgr_ipcu.h"

#define CMD_ID_NOT_SET 0xFF
#define RECV_WAIT_TIMEOUT 1

static UINT8 g_cmd_send_id = CMD_ID_NOT_SET;
static UINT8 g_cmd_recv_id = CMD_ID_NOT_SET;

static pthread_mutex_t g_mutex_recv = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_cond_recv = PTHREAD_COND_INITIALIZER;

static T_CPUCMD_COMMAND_ACK g_cmd_ack;
static int g_cmd_ack_result = -1;

static void cmd_rcv_callback(UINT8 id, UINT32 pdata, UINT32 length, UINT32 cont, UINT32 total_length)
{
    if (id != g_cmd_recv_id) {
        ERROR_LOG("recv id error.(%lu)", id);
        goto error;
    }
    if ((length != sizeof(g_cmd_ack)) || (total_length != length)) {
        ERROR_LOG("recv size error.(%lu, %lu)", length, total_length);
        goto error;
    }
    if (cont == D_CONTINUE) {
        ERROR_LOG("cont error.(%lu)", cont);
        goto error;
    }

    ASSERT(!pthread_mutex_lock(&g_mutex_recv));
    memcpy(&g_cmd_ack, (void *)pdata, sizeof(g_cmd_ack));
    g_cmd_ack_result = 0;
    ASSERT(!pthread_cond_signal(&g_cond_recv));
    ASSERT(!pthread_mutex_unlock(&g_mutex_recv));

    return;

error:
    ASSERT(!pthread_mutex_lock(&g_mutex_recv));
    memset(&g_cmd_ack, 0, sizeof(g_cmd_ack));
    g_cmd_ack_result = -1;
    ASSERT(!pthread_cond_signal(&g_cond_recv));
    ASSERT(!pthread_mutex_unlock(&g_mutex_recv));
}

int ipcucif_open()
{
    FJ_ERR_CODE ret;

    ret = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_1, &g_cmd_send_id);
    if (ret != FJ_ERR_OK) {
        ERROR_LOG("FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_1) error.");
        goto error;
    }

    ret = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_0, &g_cmd_recv_id);
    if (ret != FJ_ERR_OK) {
        ERROR_LOG("FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_0) error.");
        goto error;
    }

    ret = FJ_IPCU_SetReceiverCB(g_cmd_recv_id, cmd_rcv_callback);
    if (ret != FJ_ERR_OK) {
        ERROR_LOG("FJ_IPCU_SetReceiverCB() error.");
        goto error;
    }

    return 0;

error:
    ipcucif_close();
    return -1;
}

void ipcucif_close()
{
    if (g_cmd_send_id != CMD_ID_NOT_SET) {
        FJ_IPCU_Close(g_cmd_send_id);
    }
    if (g_cmd_recv_id != CMD_ID_NOT_SET) {
        FJ_IPCU_SetReceiverCB(g_cmd_recv_id, NULL);
        FJ_IPCU_Close(g_cmd_recv_id);
    }
    g_cmd_send_id = CMD_ID_NOT_SET;
    g_cmd_recv_id = CMD_ID_NOT_SET;
}

int ipcucif_send(const struct ipcucif_send_cmd *send_cmd, struct ipcucif_recv_cmd *recv_cmd)
{
    T_CPUCMD_COMMAND_REQUEST cmd_req;
    FJ_ERR_CODE ret;
    int result = -1;
    struct timespec timeout;

    CHECK_POINTER(send_cmd);
    CHECK_POINTER(recv_cmd);

    ASSERT(g_cmd_send_id != CMD_ID_NOT_SET);
    ASSERT(g_cmd_recv_id != CMD_ID_NOT_SET);

    memset(&cmd_req, 0, sizeof(cmd_req));
    cmd_req.t_head.magic_code = D_CPU_IF_MCODE_COMMAND_REQ;
    cmd_req.t_head.format_version = D_CPU_IF_COM_FORMAT_VERSION;
    cmd_req.t_head.cmd_set = send_cmd->cmd_id;
    cmd_req.t_head.cmd = send_cmd->sub_id;
    cmd_req.param1 = send_cmd->param[0];
    cmd_req.param2 = send_cmd->param[1];
    cmd_req.param3 = send_cmd->param[2];
    cmd_req.param4 = send_cmd->param[3];

    ASSERT(!pthread_mutex_lock(&g_mutex_recv));

    ret = FJ_IPCU_Send(g_cmd_send_id, (UINT32)&cmd_req, sizeof(cmd_req), IPCU_SEND_SYNC);
    if (ret != FJ_ERR_OK) {
        ERROR_LOG("FJ_IPCU_Send() error.");
        goto exit;
    }

    timeout.tv_sec = time(NULL) + RECV_WAIT_TIMEOUT;
    timeout.tv_nsec = 0;
    switch (pthread_cond_timedwait(&g_cond_recv, &g_mutex_recv, &timeout)) {
    case 0:
        break;
    case EINTR:
        ERROR_LOG("FJ_IPCU_Send() interrupted.");
        goto exit;
    case ETIMEDOUT:
        ERROR_LOG("FJ_IPCU_Send() timeout.");
        goto exit;
    default:
        ASSERT(0);
    }

    recv_cmd->cmd_id = g_cmd_ack.t_head.cmd_set;
    recv_cmd->sub_id = g_cmd_ack.t_head.cmd;
    recv_cmd->result[0] = g_cmd_ack.ret1;
    recv_cmd->result[1] = g_cmd_ack.ret2;
    recv_cmd->result[2] = g_cmd_ack.ret3;
    recv_cmd->result[3] = g_cmd_ack.ret4;
    result = g_cmd_ack_result;

exit:
    ASSERT(!pthread_mutex_unlock(&g_mutex_recv));

    return result;
}
#else /* PMGR_DEBUG_LOCAL_PC */
int ipcucif_open()
{
    return 0;
}

void ipcucif_close()
{

}

int ipcucif_send(const struct ipcucif_send_cmd *send_cmd, struct ipcucif_recv_cmd *recv_cmd)
{
    return 0;
}
#endif /* PMGR_DEBUG_LOCAL_PC */
