/******************************************************************************
 *
 *  @file   pmgr_common.h
 *  @brief  Parameter Manager
 *
 *  Copyright 2018 Socionext Inc.
 ******************************************************************************/
#ifndef __PMGR_COMMON_H
#define __PMGR_COMMON_H
#include <stdint.h>
#include "pmgr_common.h"

#define PMGR_LINE_BUFF_SIZE 512
#define COUNTOF(array) (sizeof(array) / sizeof(array[0]))

enum e_conf_value_type {
    E_UINT32,
    E_RESULT,
    E_DO_NOT_CARE,
};

enum e_conf_value_result {
    E_RESULT_1,
    E_RESULT_2,
    E_RESULT_3,
    E_RESULT_4,
    E_RESULT_MAX
};

struct conf_value {
    enum e_conf_value_type type;
    union {
        uint32_t uint32;
        enum e_conf_value_result result;
    } value;
};

struct conf_cmd {
    struct conf_value cmd_id;
    struct conf_value sub_id;
    struct conf_value param1;
    struct conf_value param2;
    struct conf_value param3;
    struct conf_value param4;
};

struct conf_cmd_tree {
    struct conf_cmd get_cmd;
    struct conf_cmd set_cmd;
    struct conf_cmd_tree *parent;
    struct conf_cmd_tree *child;
    struct conf_cmd_tree *next;
};

struct cvs_cmd_list {
    uint32_t cmd_id;
    uint32_t sub_id;
    uint32_t param1;
    uint32_t param2;
    uint32_t param3;
    uint32_t param4;
    struct cvs_cmd_list *next;
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void trim_space(char *string);
void remove_crlf(char *string);
void remove_comment(char *string);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PMGR_COMMON_H */
