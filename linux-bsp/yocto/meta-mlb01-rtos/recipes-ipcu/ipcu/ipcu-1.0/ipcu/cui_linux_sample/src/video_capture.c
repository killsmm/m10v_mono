/******************************************************************************/
/*
 *  @file  video_capture.c
 *  @brief video capture
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
static UINT32	v_size;
static UINT32	v_frame_rate;
static UINT32	v_codec_type;

/********************************************************************
 *	Local Function
 ********************************************************************/
static void usage()
{
	printf("Usage:\n");
	printf("video_capture <video size> <frame rate> <codec type>\n");
	printf("  Example:\n");
	printf("    video_capture 3840x2160 60fps hevc\n");
}

/* get command line */
static int video_capture_get_command_line(int argc, char *argv[])
{
	INT32 i = 0;
	UINT32 number = 0;

	/* argument check */
	if(argc == 1 || argc >= 5){
		usage();
		return -1;
	}

	/* Size */
	if(strcmp(argv[1], "3840x2160") == 0){
		v_size = VIDEO_SIZE_4K;
	}
	else{
		printf("error: [%s] string unmach\n", argv[1]);
		usage();
		return -1;
	}

	/* Framerate */
	if(strcmp(argv[2], "60fps") == 0){
		v_frame_rate = VIDEO_60FPS;
	}
	else{
		printf("error: [%s] string unmach\n", argv[2]);
		usage();
		return -1;
	}

	/* Codec type */
	if(strcmp(argv[3], "hevc") == 0){
		v_codec_type = VIDEO_HEVC;
	}
	else{
		printf("error: [%s] string unmach\n", argv[3]);
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
	UINT32 file_number = 1;

	/* get command line */
	if(video_capture_get_command_line(argc, argv) != 0){
		printf("error: get command line.\n");
		return 0;
	}

	/* camera_if open */
	Camera_If_Open();

	/* Preview */
	Preview();

	/* CUI menu */
//	Sample_Menu();

	/* Video capture */
	Video_Capture(v_size, v_frame_rate, v_codec_type);

	/* camera_if close */
	Camera_If_Close();

	printf("sample normal end.\n\n");

	return 0;
}
