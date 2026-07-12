/******************************************************************************/
/*
 *  @file  preview.c
 *  @brief preview
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

	/* Preview */
	Preview();

	/* CUI menu */
	Sample_Menu();

	/* camera_if close */
	Camera_If_Close();

	printf("sample normal end.\n\n");

	return 0;
}
