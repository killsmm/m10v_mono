/******************************************************************************
 *
 *  @file   pmgr_getter.h
 *  @brief  Parameter Manager
 *
 *  Copyright 2018 Socionext Inc.
 ******************************************************************************/
#ifndef __PMGR_GETTER_H
#define __PMGR_GETTER_H
#include "pmgr_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct cvs_cmd_list *do_get_cmd(const struct conf_cmd_tree *conf_tree);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PMGR_GETTER_H */
