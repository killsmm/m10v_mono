/******************************************************************************/
/*
 *  @file  media_format.c
 *  @brief media format
 *
 *
 *  Copyright 2017 Socionext Inc.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include "cmfwk_mm.h"
#include "cmfwk_ipcu.h"
#include "cmfwk_std.h"
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sched.h>
#include <errno.h>
#include <semaphore.h>
#include <stdarg.h>
#include "camera_if.h"
#include "ipcu.h"

/********************************************************************
 *	Local Function
 ********************************************************************/
static int check(void)
{
	INT8 str[16];

	while(1){
		printf("\nreally sd card format ok? (y/n) :");
		fgets(str, 16, stdin);
		if(strcmp(str, "y\n") == 0){
			break;
		}
		else if(strcmp(str, "n\n") == 0){
			return -1;
		}
	}

	return 0;
}

/******************************************************************************/
/**
 *  @brief  sample app
 *  @fn int main( int argc, char* argv[] )
 *  @param  [in]    argc      : Number of argument
 *  @param  [in]    argv      : String of argument
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
int main(int argc, char *argv[])
{
	/* camera_if open */
	Camera_If_Open();

	/* Check if there is an input mistake */
	if(check() != 0){
		printf("exit sample.\n");
		return 0;
	}

	/* Media Format */
	Media_Format(FORMAT_SD1);

	/* camera_if close */
	Camera_If_Close();

	printf("sample normal end.\n\n");

	return 0;
}
