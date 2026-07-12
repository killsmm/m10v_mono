/******************************************************************************
 *
 *  @file   pmgr_setter.h
 *  @brief  Parameter Manager
 *
 *  Copyright 2018 Socionext Inc.
 ******************************************************************************/
#ifndef __PMGR_SETTER_H
#define __PMGR_SETTER_H
#include "pmgr_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int do_set_cmd(const struct cvs_cmd_list *cvs_list);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PMGR_SETTER_H */
