/******************************************************************************
 *
 *  @file   pmgr_common.c
 *  @brief  Parameter Manager
 *
 *  Copyright 2018 Socionext Inc.
 ******************************************************************************/
#include <string.h>
#include "pmgr_common.h"

void trim_space(char *string)
{
    int i;
    int count = 0;

    if (string == NULL) {
        return;
    }

    i = (int)strlen(string);

    while (--i >= 0) {
        if (string[i] != ' ' && string[i] != '\t') {
            break;
        }
        count++;
    }

    string[i + 1] = '\0';

    i = 0;
    while (string[i] != '\0') {
        if (string[i] != ' ' && string[i] != '\t') {
            break;
        }
        i++;
    }

    strcpy(string, &string[i]);
}

void remove_crlf(char *string)
{
    size_t length;
    length = strlen(string);
    if (length > 0) {
        if (string[length - 1] == '\n') {
            string[length - 1] = '\0';
        }
    }
    if (length > 1) {
        if (string[length - 2] == '\r') {
            string[length - 2] = '\0';
        }
    }
}

void remove_comment(char *string)
{
    char *comment = NULL;
    comment = strstr(string, "//");
    if (comment) {
        *comment = '\0';
    }
    comment = strstr(string, "#");
    if (comment) {
        *comment = '\0';
    }
}