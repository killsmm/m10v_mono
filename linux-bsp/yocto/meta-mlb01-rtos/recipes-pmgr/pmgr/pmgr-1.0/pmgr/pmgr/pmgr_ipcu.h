/******************************************************************************
 *
 *  @file   pmgr_ipcu.h
 *  @brief  Parameter Manager
 *
 *  Copyright 2018 Socionext Inc.
 ******************************************************************************/
#ifndef __IPCUCIF_H
#define __IPCUCIF_H
#include <stdint.h>

#define PMGR_PARAM_MAX 4

struct ipcucif_send_cmd {
    uint32_t cmd_id;
    uint32_t sub_id;
    uint32_t param[PMGR_PARAM_MAX];
};

struct ipcucif_recv_cmd {
    uint32_t cmd_id;
    uint32_t sub_id;
    uint32_t result[PMGR_PARAM_MAX];
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int ipcucif_open();
void ipcucif_close();
int ipcucif_send(const struct ipcucif_send_cmd *send_cmd, struct ipcucif_recv_cmd *recv_cmd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __IPCUCIF_H */
