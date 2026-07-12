/*
 * Warp freeze command
 *
 *  Copyright (C) 2014 Lineo Solutions, Inc.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>
#include <signal.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

#include "warpd.h"
#include "warp_helper_config.h"
#include "warp_helper_lib.h"

void help()
{
	printf("options:\n");
	printf("\t-?: show this help\n");
	printf("\t-p [proc_num]: process number\n");
	printf("\t-c [conf_path]: specify warpd config file full path\n");
}

int main(int argc, char **argv)
{
	int i;
	int ret = 0;
	char *conf_file_path = CONF_FILE;
	long pnum = 0;

	/**
	 * command option table
	 */
	const struct option longopt[] = {
		{"help", no_argument, NULL, '?'},
		{"proc_num", required_argument, NULL, 'p'},
		{"conffile", required_argument, NULL, 'c'},
		{NULL, 0, NULL, 0}
	};

	/**
	 * parse command option
	 */
	while ((i = getopt_long(argc, argv, "p:c:h?", longopt, NULL)) != -1) {
		switch (i) {
		case 'p':
			pnum = atol(optarg);
			break;
		case 'c':
			conf_file_path = optarg;
			break;
		case 'h':
		case '?':
			help();
			return 0;
		default:
			break;
		}
	}

	ret = warp_helper_libinit( conf_file_path, pnum);
	if ( ret == ERR_SUCCESS ){ 
		ret = warp_helper_enter_freeze();
		if (ret != ERR_SUCCESS && ret != ERR_WARP_SUCCESS) {
			warp_helper_err_to_message( ret);
		}
	}else{
		warp_helper_err_to_message( ret);
	}

	return ret;
}
