/******************************************************************************/
/*
 *  @file  single_capture.c
 *  @brief single capture
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
#include "camera_if.h"
#include "ipcu.h"

/********************************************************************
 *			Local Variable
 ********************************************************************/
static UINT32	s_format;
static UINT32	s_sd_card_slot;
static UINT32	s_capture_size;
static UINT32	s_quality;

/********************************************************************
 *	Local Function
 ********************************************************************/
static void usage()
{
	printf("Usage:\n");
	printf("single_capture <format> <sd card slot> <capture size> <quality>.\n");
	printf("  Example:\n");
	printf("    cui_sample.sh single_capture jpeg sd1 4000x3000 low\n");
}

/* get command line */
static int single_capture_get_command_line(int argc, char *argv[])
{
	INT32 i = 0;
	UINT32 number = 0;

	/* argument check */
	if(argc == 1 || argc >= 8){
		usage();
		return -1;
	}

	/* Image Type */
	if(strcmp(argv[1], "jpeg") == 0){
		s_format = IMAGE_TYPE_JPEG;
	}
	else{
		printf("error: [%s] string unmach\n", argv[1]);
		usage();
		return -1;
	}

	/* Store Channel */
	if(strcmp(argv[2], "sd1") == 0){
		s_sd_card_slot = IMAGE_STORE_CHANNEL_SD1;
	}
	else{
		printf("error: [%s] string unmach\n", argv[2]);
		usage();
		return -1;
	}

	/* Capture Size */
	if(strcmp(argv[3], "4000x3000") == 0){
		s_capture_size = IMAGE_CAPTURE_SIZE_12M;
	}
	else{
		printf("error: [%s] string unmach\n", argv[3]);
		usage();
		return -1;
	}

	/* Quality */
	if(strcmp(argv[4], "low") == 0){
		s_quality = IMAGE_QUALITY_LOW;
	}
	else if(strcmp(argv[4], "middle") == 0){
		s_quality = IMAGE_QUALITY_MIDDLE;
	}
	else if(strcmp(argv[4], "high") == 0){
		s_quality = IMAGE_QUALITY_HIGH;
	}
	else{
		printf("error: [%s] string unmach\n", argv[4]);
		usage();
		return -1;
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

	/* get command line */
	if(single_capture_get_command_line(argc, argv) != 0){
		printf("error: get command line.\n");
		return 0;
	}

	/* camera_if open */
	Camera_If_Open();

	/* Preview */
	Preview();

	/* CUI menu */
//	Sample_Menu();

	/* Single capture */
	Single_Capture(s_format, s_sd_card_slot, s_capture_size, s_quality);

	/* camera_if close */
	Camera_If_Close();

	printf("sample normal end.\n\n");

	return 0;
}
