/******************************************************************************
*
*  @file   pmgr_diff.c
*  @brief  Parameter Manager
*
*  Copyright 2018 Socionext Inc.
******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "pmgr_debug.h"
#include "pmgr_diff.h"
#include "pmgr_csv.h"

struct work_cmd_tree {
    uint32_t cmd_id;
    uint32_t sub_id;
    uint32_t param1;
    uint32_t param2;
    uint32_t param3;
    uint32_t param4;
    struct work_cmd_tree *parent;
    struct work_cmd_tree *next;
};

static const struct conf_cmd_tree *conf_cmd_tree_next(const struct conf_cmd_tree *conf_tree)
{
    if (conf_tree->child) {
        conf_tree = conf_tree->child;
    } else if (conf_tree->next) {
        conf_tree = conf_tree->next;
    } else if (conf_tree->parent) {
        conf_tree = conf_tree->parent->next;
    } else {
        conf_tree = NULL;
    }
    return conf_tree;
}

static struct work_cmd_tree *search_parent(struct work_cmd_tree *head, const struct conf_cmd_tree *cmd)
{
    for (; head; head = head->next) {
        if ((head->cmd_id != cmd->set_cmd.cmd_id.value.uint32) ||
            (head->sub_id != cmd->set_cmd.sub_id.value.uint32)) {
            continue;
        }
        if ((cmd->set_cmd.param1.type == E_UINT32) &&
            (head->param1 != cmd->set_cmd.param1.value.uint32)) {
            continue;
        }
        if ((cmd->set_cmd.param2.type == E_UINT32) &&
            (head->param2 != cmd->set_cmd.param2.value.uint32)) {
            continue;
        }
        if ((cmd->set_cmd.param3.type == E_UINT32) &&
            (head->param3 != cmd->set_cmd.param3.value.uint32)) {
            continue;
        }
        if ((cmd->set_cmd.param4.type == E_UINT32) &&
            (head->param4 != cmd->set_cmd.param4.value.uint32)) {
            continue;
        }
        return head;
    }
    return NULL;
}

static struct cvs_cmd_list *search_cvs_cmd_list(struct cvs_cmd_list *head, const struct work_cmd_tree *cmd)
{
    for (; head; head = head->next) {
        if ((cmd->cmd_id == head->cmd_id) &&
            (cmd->sub_id == head->sub_id) &&
            (cmd->param1 == head->param1) &&
            (cmd->param2 == head->param2) &&
            (cmd->param3 == head->param3) &&
            (cmd->param4 == head->param4)) {
            return head;
        }
    }
    return NULL;
}

static void add_cmd_list(struct cvs_cmd_list **head, struct cvs_cmd_list **tail, const struct work_cmd_tree *add_cmd)
{
    struct cvs_cmd_list *cmd = NULL;
    struct cvs_cmd_list *parent = NULL;

    if (add_cmd->parent) {
        parent = search_cvs_cmd_list(*head, add_cmd->parent);
        if (parent == NULL) {
            add_cmd_list(head, tail, add_cmd->parent);
        }
    }
    cmd = malloc(sizeof(struct cvs_cmd_list));
    CHECK_POINTER(cmd);
    memset(cmd, 0, sizeof(struct cvs_cmd_list));

    cmd->cmd_id = add_cmd->cmd_id;
    cmd->sub_id = add_cmd->sub_id;
    cmd->param1 = add_cmd->param1;
    cmd->param2 = add_cmd->param2;
    cmd->param3 = add_cmd->param3;
    cmd->param4 = add_cmd->param4;

    if (*head == NULL) {
        *head = cmd;
    } else {
        (*tail)->next = cmd;
    }
    *tail = cmd;
}

static void free_work_cmd_tree(struct work_cmd_tree *head)
{
    while (head) {
        struct work_cmd_tree *work = head;
        head = head->next;
        free(work);
    }
}

static struct work_cmd_tree *conv_cmd_tree(const struct conf_cmd_tree *conf_tree, const struct cvs_cmd_list *cur_cmd)
{
    struct work_cmd_tree *head = NULL, *tail = NULL;
    const struct conf_cmd_tree *work = conf_tree;

    for (; cur_cmd; cur_cmd = cur_cmd->next) {
        struct work_cmd_tree *cmd = NULL;

        if (work == NULL) {
            ERROR_LOG("diff conf param error.");
            goto error;
        }
        if ((cur_cmd->cmd_id != work->set_cmd.cmd_id.value.uint32) ||
            (cur_cmd->sub_id != work->set_cmd.sub_id.value.uint32)) {
            ERROR_LOG("diff conf cmd_id error.");
            goto error;
        }
        if ((work->set_cmd.param1.type == E_UINT32) &&
            (cur_cmd->param1 != work->set_cmd.param1.value.uint32)) {
            ERROR_LOG("diff conf param error.");
            goto error;
        }
        if ((work->set_cmd.param2.type == E_UINT32) &&
            (cur_cmd->param2 != work->set_cmd.param2.value.uint32)) {
            ERROR_LOG("diff conf param error.");
            goto error;
        }
        if ((work->set_cmd.param3.type == E_UINT32) &&
            (cur_cmd->param3 != work->set_cmd.param3.value.uint32)) {
            ERROR_LOG("diff conf param error.");
            goto error;
        }
        if ((work->set_cmd.param4.type == E_UINT32) &&
            (cur_cmd->param4 != work->set_cmd.param4.value.uint32)) {
            ERROR_LOG("diff conf param error.");
            goto error;
        }

        cmd = malloc(sizeof(struct work_cmd_tree));
        CHECK_POINTER(cmd);
        memset(cmd, 0, sizeof(struct work_cmd_tree));
        cmd->cmd_id = cur_cmd->cmd_id;
        cmd->sub_id = cur_cmd->sub_id;
        cmd->param1 = cur_cmd->param1;
        cmd->param2 = cur_cmd->param2;
        cmd->param3 = cur_cmd->param3;
        cmd->param4 = cur_cmd->param4;
        if (work->parent) {
            cmd->parent = search_parent(head, work->parent);
            ASSERT(cmd->parent);
        } else {
            cmd->parent = NULL;
        }
        if (head == NULL) {
            head = cmd;
        } else {
            tail->next = cmd;
        }
        tail = cmd;

        work = conf_cmd_tree_next(work);
    }
    return head;

error:
    free_work_cmd_tree(head);
    return NULL;
}

int csv_diff(const struct conf_cmd_tree *conf_tree, const struct cvs_cmd_list *default_cmd,
             const struct cvs_cmd_list *cur_cmd, struct cvs_cmd_list **csv_list)
{
    struct cvs_cmd_list *head = NULL, *tail = NULL;
    struct work_cmd_tree *work_cmd_tree = NULL, *work = NULL;

    CHECK_POINTER(conf_tree);
    CHECK_POINTER(default_cmd);
    CHECK_POINTER(cur_cmd);
    CHECK_POINTER(csv_list);

    work_cmd_tree = conv_cmd_tree(conf_tree, cur_cmd);
    if (work_cmd_tree == NULL) {
        goto error;
    }

    for (work = work_cmd_tree; default_cmd && work; default_cmd = default_cmd->next, work = work->next) {
        if ((work->cmd_id != default_cmd->cmd_id) ||
            (work->sub_id != default_cmd->sub_id)) {
            ERROR_LOG("diff csv cmd_id error.");
            goto error;
        }

        if ((work->param1 == default_cmd->param1) &&
            (work->param2 == default_cmd->param2) &&
            (work->param3 == default_cmd->param3) &&
            (work->param4 == default_cmd->param4)) {
            continue;
        }
        add_cmd_list(&head, &tail, work);
    }

    free_work_cmd_tree(work_cmd_tree);
    *csv_list = head;
    return 0;

error:
    free_work_cmd_tree(work_cmd_tree);
    free_csv(head);
    return -1;
}
