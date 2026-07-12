/******************************************************************************
 *
 *  @file   pmgr_main.c
 *  @brief  Parameter Manager
 *
 *  Copyright 2018 Socionext Inc.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include "pmgr_conf.h"
#include "pmgr_csv.h"
#include "pmgr_diff.h"
#include "pmgr_getter.h"
#include "pmgr_setter.h"
#include "pmgr_ipcu.h"
#include "pmgr_debug.h"

#define PMGR_OPTION_NULL        0
#define PMGR_OPTION_SAVE        1
#define PMGR_OPTION_LOAD        2
#define PMGR_OPTION_DIFF        3


static const struct option opts[] = {
    { "help",       no_argument,        NULL,   'h' },
    { "version",    no_argument,        NULL,   'v' },
    { "save",       required_argument,  NULL,   's' },
    { "load",       required_argument,  NULL,   'l' },
    { "diff",       required_argument,  NULL,   'd' },
    { "conf",       required_argument,  NULL,   'c' },
    { 0, 0, 0, 0 }
};

static void pmger_version()
{
    printf("pmgr version %s\n", PMGR_VERSION);
}

static void pmger_usage()
{
    printf("usage: pmgr [options]...\n");
    printf("options:\n");
    printf("  -c, --conf=FILE   parameter manager configuration file\n");
    printf("  -s, --save=FILE   save rtos parameters file\n");
    printf("  -l, --load=FILE   load rtos parameters file\n");
    printf("  -d, --diff=FILE   default rtos parameters file for extracting differences\n");
    printf("  -v, --version     output version information and exit\n");
    printf("  -h, --help        display this help and exit\n");
    printf("example:\n");
    printf("  save rtos parameters to file\n");
    printf("    pmgr -c pmgr.conf -s default.csv\n");
    printf("  save rtos parameters changed from default value\n");
    printf("    pmgr -c pmgr.conf -d default.csv -s pmgr.csv\n");
    printf("  load rtos parameters from file\n");
    printf("    pmgr -l pmgr.csv\n");
}

static void sigint_handler(int signum)
{
    UNUSED_VARIABLE(signum);
    ipcucif_close();
    exit(1);
}

static int get_mode(const char *conf_path, const char *csv_path)
{
    int ret = 1;
    struct conf_cmd_tree *conf_tree = NULL;
    struct cvs_cmd_list *csv_list = NULL;

    conf_tree = load_conf(conf_path);
    if (conf_tree == NULL) {
        goto exit;
    }

    csv_list = do_get_cmd(conf_tree);
    if (csv_list == NULL) {
        goto exit;
    }

    if (save_csv(csv_path, csv_list)) {
        goto exit;
    }

    ret = 0;
exit:
    free_conf(conf_tree);
    free_csv(csv_list);
    return ret;
}

static int set_mode(const char *csv_path)
{
    int ret = 1;
    int num = 0;
    struct cvs_cmd_list *csv_list = NULL;

    csv_list = load_csv(csv_path, &num);
    if (num < 0) {
        goto exit;
    } else if (num > 0) {
        if (do_set_cmd(csv_list)) {
            goto exit;
        }
    }

    ret = 0;
exit:
    free_csv(csv_list);
    return ret;
}

static int diff_mode(const char *conf_path, const char *csv_path, const char *defcsv_path)
{
    int ret = 1;
    int num = 0;
    struct conf_cmd_tree *conf_tree = NULL;
    struct cvs_cmd_list *default_csv_list = NULL;
    struct cvs_cmd_list *get_list = NULL;
    struct cvs_cmd_list *csv_list = NULL;

    conf_tree = load_conf(conf_path);
    if (conf_tree == NULL) {
        goto exit;
    }

    default_csv_list = load_csv(defcsv_path, &num);
    if ((default_csv_list == NULL) || (num <= 0)) {
        goto exit;
    }

    get_list = do_get_cmd(conf_tree);
    if (get_list == NULL) {
        goto exit;
    }

    if (csv_diff(conf_tree, default_csv_list, get_list, &csv_list)) {
        goto exit;
    }

    if (save_csv(csv_path, csv_list)) {
        goto exit;
    }
    ret = 0;
exit:
    free_csv(get_list);
    free_csv(csv_list);
    free_csv(default_csv_list);
    return ret;
}


int main(int argc, char *argv[])
{
    int ret;
    int opt;
    int mode = PMGR_OPTION_NULL;
    struct sigaction sigact;
    char *csv_path = NULL;
    char *conf_path = NULL;
    char *defcsv_path = NULL;

    while ((opt = getopt_long(argc, argv, "hvs:l:d:c:", opts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            pmger_usage();
            return 0;
        case 'v':
            pmger_version();
            return 0;
        case 's':
            if (mode == PMGR_OPTION_DIFF) {
                ;
            } else if (mode == PMGR_OPTION_NULL) {
                mode = PMGR_OPTION_SAVE;
            } else {
                goto argerr;
            }
            csv_path = optarg;
            break;
        case 'l':
            if (mode != PMGR_OPTION_NULL) {
                goto argerr;
            }
            mode = PMGR_OPTION_LOAD;
            csv_path = optarg;
            break;
        case 'c':
            conf_path = optarg;
            break;
        case 'd':
            if (mode == PMGR_OPTION_SAVE) {
                ;
            } else if (mode == PMGR_OPTION_NULL) {
                mode = PMGR_OPTION_DIFF;
            } else {
                goto argerr;
            }
            defcsv_path = optarg;
            break;
        default:
            goto argerr;
        }
    }

    memset(&sigact, 0, sizeof(sigact));
    sigact.sa_handler = sigint_handler;
    sigact.sa_flags = (int)SA_RESETHAND;
    ASSERT(!sigaction(SIGINT, &sigact, NULL));
    ASSERT(!sigaction(SIGTERM, &sigact, NULL));
    ASSERT(!sigaction(SIGABRT, &sigact, NULL));

    if (mode == PMGR_OPTION_SAVE) {
        if ((conf_path == NULL) || (csv_path == NULL)) {
            goto argerr;
        }
        ret = get_mode(conf_path, csv_path);
    } else if (mode == PMGR_OPTION_LOAD) {
        if (csv_path == NULL) {
            goto argerr;
        }
        ret = set_mode(csv_path);
    } else if (mode == PMGR_OPTION_DIFF) {
        if ((conf_path == NULL) || (csv_path == NULL) || (defcsv_path == NULL)) {
            goto argerr;
        }
        ret = diff_mode(conf_path, csv_path, defcsv_path);
    } else {
        goto argerr;
    }

    return ret;

argerr:
    fprintf(stderr, "invalid argument.\n");
    pmger_usage();
    return 1;
}
