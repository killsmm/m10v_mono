/******************************************************************************
*
*  @file   pmgr_diff.h
*  @brief  Parameter Manager
*
*  Copyright 2018 Socionext Inc.
******************************************************************************/
#ifndef __PMGR_DIFF_H
#define __PMGR_DIFF_H
#include "pmgr_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int csv_diff(const struct conf_cmd_tree *conf_tree, const struct cvs_cmd_list *default_cmd,
             const struct cvs_cmd_list *cur_cmd, struct cvs_cmd_list **csv_list);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PMGR_DIFF_H */
