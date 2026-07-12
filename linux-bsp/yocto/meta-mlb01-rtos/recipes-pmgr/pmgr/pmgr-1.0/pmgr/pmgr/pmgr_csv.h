/******************************************************************************
 *
 *  @file   pmgr_csv.h
 *  @brief  Parameter Manager
 *
 *  Copyright 2018 Socionext Inc.
 ******************************************************************************/
#ifndef __PMGR_CSV_H
#define __PMGR_CSV_H
#include "pmgr_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct cvs_cmd_list *load_csv(const char *file, int *num);
int save_csv(const char *file, const struct cvs_cmd_list *csv_list);
void free_csv(struct cvs_cmd_list *csv_list);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PMGR_CSV_H */
