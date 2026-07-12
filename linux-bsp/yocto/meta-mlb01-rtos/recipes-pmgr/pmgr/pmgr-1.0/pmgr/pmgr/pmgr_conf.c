/******************************************************************************
 *
 *  @file   pmgr_conf.c
 *  @brief  Parameter Manager
 *
 *  Copyright 2018 Socionext Inc.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include "pmgr_common.h"
#include "pmgr_debug.h"
#include "pmgr_conf.h"

#define TYPE_UINT32     0x00000100
#define TYPE_RESULT     0x00000200
#define TYPE_NOCARE     0x00000400
#define TYPE_MASK       0x0000FF00
#define NULL_ALLOW      0x00000001
#define NULL_DENY       0x00000002
#define NULL_MASK       0x000000FF

struct conf_column_type {
    uint32_t type;
    size_t offset;
};

struct conf_value_result {
    char *string;
    enum e_conf_value_result result;
};

struct conf_line {
    struct conf_cmd get_cmd;
    struct conf_cmd set_cmd;
    struct conf_cmd depend_cmd;
};

const struct conf_column_type g_conf_column[] = {
    { (TYPE_UINT32               | NULL_DENY), offsetof(struct conf_line, get_cmd) + offsetof(struct conf_cmd, cmd_id) },
    { (TYPE_UINT32               | NULL_DENY), offsetof(struct conf_line, get_cmd) + offsetof(struct conf_cmd, sub_id) },
    { (TYPE_UINT32               | NULL_DENY), offsetof(struct conf_line, get_cmd) + offsetof(struct conf_cmd, param1) },
    { (TYPE_UINT32               | NULL_DENY), offsetof(struct conf_line, get_cmd) + offsetof(struct conf_cmd, param2) },
    { (TYPE_UINT32               | NULL_DENY), offsetof(struct conf_line, get_cmd) + offsetof(struct conf_cmd, param3) },
    { (TYPE_UINT32               | NULL_DENY), offsetof(struct conf_line, get_cmd) + offsetof(struct conf_cmd, param4) },
    { (TYPE_UINT32               | NULL_DENY), offsetof(struct conf_line, set_cmd) + offsetof(struct conf_cmd, cmd_id) },
    { (TYPE_UINT32               | NULL_DENY), offsetof(struct conf_line, set_cmd) + offsetof(struct conf_cmd, sub_id) },
    { (TYPE_UINT32 | TYPE_RESULT | NULL_DENY), offsetof(struct conf_line, set_cmd) + offsetof(struct conf_cmd, param1) },
    { (TYPE_UINT32 | TYPE_RESULT | NULL_DENY), offsetof(struct conf_line, set_cmd) + offsetof(struct conf_cmd, param2) },
    { (TYPE_UINT32 | TYPE_RESULT | NULL_DENY), offsetof(struct conf_line, set_cmd) + offsetof(struct conf_cmd, param3) },
    { (TYPE_UINT32 | TYPE_RESULT | NULL_DENY), offsetof(struct conf_line, set_cmd) + offsetof(struct conf_cmd, param4) },
    { (TYPE_UINT32               | NULL_ALLOW), offsetof(struct conf_line, depend_cmd) + offsetof(struct conf_cmd, cmd_id) },
    { (TYPE_UINT32               | NULL_ALLOW), offsetof(struct conf_line, depend_cmd) + offsetof(struct conf_cmd, sub_id) },
    { (TYPE_UINT32 | TYPE_NOCARE | NULL_ALLOW), offsetof(struct conf_line, depend_cmd) + offsetof(struct conf_cmd, param1) },
    { (TYPE_UINT32 | TYPE_NOCARE | NULL_ALLOW), offsetof(struct conf_line, depend_cmd) + offsetof(struct conf_cmd, param2) },
    { (TYPE_UINT32 | TYPE_NOCARE | NULL_ALLOW), offsetof(struct conf_line, depend_cmd) + offsetof(struct conf_cmd, param3) },
    { (TYPE_UINT32 | TYPE_NOCARE | NULL_ALLOW), offsetof(struct conf_line, depend_cmd) + offsetof(struct conf_cmd, param4) },
    { 0, 0 }
};

const struct conf_value_result g_result_map[] = {
    { "RESULT1", E_RESULT_1 },
    { "RESULT2", E_RESULT_2 },
    { "RESULT3", E_RESULT_3 },
    { "RESULT4", E_RESULT_4 },
    { NULL, E_RESULT_MAX },
};

static enum e_conf_value_result get_result(const char *string)
{
    const struct conf_value_result *map = g_result_map;

    CHECK_POINTER(string);

    for (; map->string; map++) {
        if (!strcmp(map->string, string)) {
            break;
        }
    }
    return map->result;
}

static struct conf_cmd_tree *search_cmd(struct conf_cmd_tree *cmd_tree, const struct conf_cmd *cmd)
{
    struct conf_cmd_tree *work = NULL;
    for (; cmd_tree; cmd_tree = cmd_tree->next) {
        if (cmd_tree->child) {
            work = search_cmd(cmd_tree->child, cmd);
            if (work) {
                break;
            }
        }
        if ((!memcmp(&cmd_tree->set_cmd.cmd_id, &cmd->cmd_id, sizeof(struct conf_value))) &&
            (!memcmp(&cmd_tree->set_cmd.sub_id, &cmd->sub_id, sizeof(struct conf_value)))) {
            if ((cmd->param1.type != E_DO_NOT_CARE) &&
                (memcmp(&cmd_tree->set_cmd.param1, &cmd->param1, sizeof(struct conf_value)))) {
                continue;
            }
            if ((cmd->param2.type != E_DO_NOT_CARE) &&
                (memcmp(&cmd_tree->set_cmd.param2, &cmd->param2, sizeof(struct conf_value)))) {
                continue;
            }
            if ((cmd->param3.type != E_DO_NOT_CARE) &&
                (memcmp(&cmd_tree->set_cmd.param3, &cmd->param3, sizeof(struct conf_value)))) {
                continue;
            }
            if ((cmd->param4.type != E_DO_NOT_CARE) &&
                (memcmp(&cmd_tree->set_cmd.param4, &cmd->param4, sizeof(struct conf_value)))) {
                continue;
            }
            work = cmd_tree;
            break;
        }
    }
    return work;
}

struct conf_cmd_tree *load_conf(const char *file)
{
    FILE *fp = NULL;
    char buff[PMGR_LINE_BUFF_SIZE];
    int line = 0;
    struct conf_cmd_tree *root = NULL, *list = NULL;

    CHECK_POINTER(file);
    memset(buff, 0, sizeof(buff));

    fp = fopen(file, "r");
    if (fp == NULL) {
        ERROR_LOG("file open error.(%s)", file);
        return NULL;
    }

    while (fgets(buff, sizeof(buff), fp)) {
        int column = 1;
        char *string = NULL;
        size_t length = 0;
        const struct conf_column_type *type = g_conf_column;
        struct conf_cmd_tree *cmd = NULL;
        struct conf_cmd_tree *parent = NULL;
        struct conf_line row;

        line++;

        memset(&row, 0, sizeof(row));

        if (buff[sizeof(buff) - 1] != '\0') {
            ERROR_LOG("conf format error.(%s[L:%d])", file, line);
            goto error;
        }

        remove_crlf(buff);
        remove_comment(buff);
        trim_space(buff);

        length = strlen(buff);
        if (length == 0) {
            continue;
        }

        string = strtok(buff, ",");
        for (; type->type; type++, column++) {
            struct conf_value data;
            void *work = (void*)&row;
            char *end = NULL;
            char *nullstring = "";
            char *zorostring = "0x00000000";
            uint32_t value;

            memset(&data, 0, sizeof(data));

            if (string == NULL) {
                string = nullstring;
            } else {
                trim_space(string);
            }
            if ((type->type & NULL_MASK) == NULL_DENY) {
                if (strlen(string) == 0) {
                    ERROR_LOG("conf format error.(%s[L:%d,C:%d])", file, line, column);
                    goto error;
                }
            } else if ((type->type & NULL_MASK) == NULL_ALLOW) {
                if (strlen(string) == 0) {
                    string = zorostring;
                }
            } else {
                ASSERT(0);
            }

            value = (uint32_t)strtoul(string, &end, 16);
            data.value.uint32 = (uint32_t)strtoul(string, &end, 16);
            if (((type->type & TYPE_UINT32) == TYPE_UINT32) && (*end == '\0') && (errno == 0)) {
                data.type = E_UINT32;
                data.value.uint32 = value;
            } else {
                if ((type->type & TYPE_RESULT) == TYPE_RESULT) {
                    data.type = E_RESULT;
                    data.value.result = get_result(string);
                    if (data.value.result == E_RESULT_MAX) {
                        ERROR_LOG("conf format error.(%s[L:%d,C:%d])", file, line, column);
                        goto error;
                    }
                } else if ((type->type & TYPE_NOCARE) == TYPE_NOCARE) {
                    if (strcmp(string, "DONOTCARE")) {
                        ERROR_LOG("conf format error.(%s[L:%d,C:%d])", file, line, column);
                        goto error;
                    }
                    data.type = E_DO_NOT_CARE;
                    data.value.uint32 = 0xFFFFFFFF;
                } else if ((type->type & TYPE_UINT32) == TYPE_UINT32) {
                    ERROR_LOG("conf format error.(%s[L:%d,C:%d])", file, line, column);
                    goto error;
                } else {
                    ASSERT(0);
                }
            }

            memcpy(work + type->offset, &data, sizeof(data));
            string = strtok(NULL, ",");
        }

        cmd = malloc(sizeof(struct conf_cmd_tree));
        CHECK_POINTER(cmd);
        memset(cmd, 0, sizeof(struct conf_cmd_tree));
        memcpy(&cmd->get_cmd, &row.get_cmd, sizeof(cmd->get_cmd));
        memcpy(&cmd->set_cmd, &row.set_cmd, sizeof(cmd->set_cmd));

        if (row.depend_cmd.cmd_id.value.uint32 != 0) {
            parent = search_cmd(root, &row.depend_cmd);
            if (parent == NULL) {
                ERROR_LOG("conf format error.(%s[L:%d])", file, line);
                goto error;
            }
        }

        if (parent == NULL) {
            if (root == NULL) {
                root = cmd;
            } else {
                list->next = cmd;
            }
            list = cmd;
        } else {
            cmd->parent = parent;
            if (parent->child == NULL) {
                parent->child = cmd;
            } else {
                struct conf_cmd_tree *child = parent->child;
                while (child->next) {
                    child = child->next;
                }
                child->next = cmd;
            }
        }
    }

    fclose(fp);
    return root;

error:
    fclose(fp);
    free_conf(root);
    return NULL;
}

void free_conf(struct conf_cmd_tree *conf_tree)
{
    while (conf_tree) {
        struct conf_cmd_tree *work = NULL;
        if (conf_tree->child) {
            free_conf(conf_tree->child);
            conf_tree->child = NULL;
        }
        work = conf_tree;
        conf_tree = conf_tree->next;
        free(work);
    }
}
