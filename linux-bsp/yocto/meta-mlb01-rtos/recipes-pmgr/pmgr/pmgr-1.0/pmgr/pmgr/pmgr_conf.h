/******************************************************************************
 *
 *  @file   pmgr_conf.h
 *  @brief  Parameter Manager
 *
 *  Copyright 2018 Socionext Inc.
 ******************************************************************************/
#ifndef __PMGR_CONF_H
#define __PMGR_CONF_H
#include "pmgr_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct conf_cmd_tree *load_conf(const char *file);
void free_conf(struct conf_cmd_tree *conf_tree);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PMGR_CONF_H */
