/******************************************************************************
 *
 *  @file   pmgr_csv.c
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
#include "pmgr_csv.h"

#define CSV_COLUMN_END 0xFFFFFFFF

struct csv_column_type {
    size_t offset;
};

const struct csv_column_type g_csv_column[] = {
    { offsetof(struct cvs_cmd_list, cmd_id) },
    { offsetof(struct cvs_cmd_list, sub_id) },
    { offsetof(struct cvs_cmd_list, param1) },
    { offsetof(struct cvs_cmd_list, param2) },
    { offsetof(struct cvs_cmd_list, param3) },
    { offsetof(struct cvs_cmd_list, param4) },
    { CSV_COLUMN_END }
};

struct cvs_cmd_list *load_csv(const char *file, int *num)
{
    FILE *fp = NULL;
    char buff[PMGR_LINE_BUFF_SIZE];
    int line = 0;
    struct cvs_cmd_list *top = NULL, *list = NULL;

    CHECK_POINTER(file);
    CHECK_POINTER(num);
    memset(buff, 0, sizeof(buff));
    *num = 0;

    fp = fopen(file, "r");
    if (fp == NULL) {
        ERROR_LOG("file open error.(%s)", file);
        return NULL;
    }

    while (fgets(buff, sizeof(buff), fp)) {
        int column = 1;
        char *string;
        size_t length;
        const struct csv_column_type *type = g_csv_column;
        struct cvs_cmd_list *cmd;

        line++;

        if (buff[sizeof(buff) - 1] != '\0') {
            ERROR_LOG("csv format error.(%s[L:%d])", file, line);
            goto error;
        }

        remove_crlf(buff);
        remove_comment(buff);
        trim_space(buff);

        length = strlen(buff);
        if (length == 0) {
            continue;
        }

        cmd = malloc(sizeof(struct cvs_cmd_list));
        CHECK_POINTER(cmd);
        memset(cmd, 0, sizeof(struct cvs_cmd_list));

        if (top == NULL) {
            top = cmd;
        } else {
            list->next = cmd;
        }
        list = cmd;

        string = strtok(buff, ",");

        while (type->offset != CSV_COLUMN_END) {
            uint32_t value = 0;
            void *work = (void*)cmd;
            char *endp;

            if (string == NULL) {
                ERROR_LOG("csv format error.(%s[L:%d,C:%d])", file, line, column);
                goto error;
            }

            trim_space(string);

            value = (uint32_t)strtoul(string, &endp, 16);
            if ((*endp != '\0') || (errno != 0)) {
                ERROR_LOG("csv format error.(%s[L:%d,C:%d])", file, line, column);
                goto error;
            }

            memcpy(work + type->offset, &value, sizeof(value));
            string = strtok(NULL, ",");

            type++;
            column++;
        }
        (*num)++;
    }

    fclose(fp);
    return top;

error:
    *num = -1;
    fclose(fp);
    free_csv(top);
    return NULL;
}

int save_csv(const char *file, const struct cvs_cmd_list *csv_list)
{
    const struct cvs_cmd_list *work = csv_list;
    FILE *fp = NULL;

    CHECK_POINTER(file);

    fp = fopen(file, "w");
    if (fp == NULL) {
        ERROR_LOG("file open error.(%s)", file);
        return -1;
    }

    fprintf(fp, "// CmdID  ,Sub-CmdID ,Param1    ,Param2    ,Param3    ,Param4\n");

    for (; work; work = work->next) {
        fprintf(fp, "0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
                work->cmd_id, work->sub_id, work->param1, work->param2, work->param3, work->param4);
    }

    fclose(fp);
    return 0;
}

void free_csv(struct cvs_cmd_list *csv_list)
{
    while (csv_list) {
        struct cvs_cmd_list *work = csv_list;
        csv_list = csv_list->next;
        free(work);
    }
}
