/******************************************************************************/
/**
 *  @file   fb_connector.c
 *  @brief sample connector for control frame buffer(fb).
 */
/*
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

#include <linux/fb.h>
#include <linux/fs.h>

/******************************************************************** 
 *	local definition
 ********************************************************************/
#define FB_CONNECTOR_FHD_VGA	(0)
#define FB_CONNECTOR_FHD	(1)
#define FB_CONNECTOR_FHD_4444	(2)
#define FB_CONNECTOR_HD		(3)
#define FB_CONNECTOR_4K		(4)
#define FB_CONNECTOR_4K_4444	(5)

/******************************************************************** 
 *	Structure
 ********************************************************************/
typedef struct {
	unsigned int vic;
	unsigned int arg_num;
	unsigned int hex;
	unsigned int video_size;
} FB_CONNECTOR_VIC_TABLE;

/******************************************************************** 
 *	Local Variable
 ********************************************************************/
const static FB_CONNECTOR_VIC_TABLE fb_connector_vic_table_array[15] = {
	{ 2, 0, 0x00000002, FB_CONNECTOR_FHD_VGA},
	{17, 0, 0x00010000, FB_CONNECTOR_FHD_VGA},
	{ 4, 0, 0x00000008, FB_CONNECTOR_HD},
	{19, 0, 0x00040000, FB_CONNECTOR_HD},
	{61, 1, 0x10000000, FB_CONNECTOR_HD},
	{62, 1, 0x20000000, FB_CONNECTOR_HD},
	{ 5, 0, 0x00000010, FB_CONNECTOR_FHD},
	{16, 0, 0x00008000, FB_CONNECTOR_FHD},
	{20, 0, 0x00080000, FB_CONNECTOR_FHD},
	{31, 0, 0x40000000, FB_CONNECTOR_FHD},
	{33, 1, 0x00000001, FB_CONNECTOR_FHD},
	{34, 1, 0x00000002, FB_CONNECTOR_FHD},
	{94, 2, 0x20000000, FB_CONNECTOR_4K},
	{95, 2, 0x40000000, FB_CONNECTOR_4K},
	{97, 3, 0x00000001, FB_CONNECTOR_4K},
};

/******************************************************************** 
 *	Local Function
 ********************************************************************/
static int fb_connector_get_vic(char* argv[])
{
	int i;
	unsigned int param[4];
	int vic;

	param[0] = strtoul(argv[2], NULL, 0);
	param[1] = strtoul(argv[3], NULL, 0);
	param[2] = strtoul(argv[4], NULL, 0);
	param[3] = strtoul(argv[5], NULL, 0);

	for (i = 14; 0 <= i; i--) {
		if ((fb_connector_vic_table_array[i].hex & param[fb_connector_vic_table_array[i].arg_num]) != 0) {
			vic = fb_connector_vic_table_array[i].vic;
			printf("VIC determine VIC=%d!\n", vic);
			return vic;
		}
	}

	printf("not detected correct VIC!\n");

	return -1;
}

static int fb_connector_get_vsize(unsigned int vic)
{
	int i;
	int vsize;

	for (i = 14; 0 <= i; i--) {
		if (fb_connector_vic_table_array[i].vic == vic) {
			vsize = fb_connector_vic_table_array[i].video_size;
			printf("video size determine size=%d!\n", vsize);
			return vsize;
		}
	}

	printf("not detected correct video size!\n");

	return -1;
}

/******************************************************************************/
/**
 *  @brief  
 *  @fn int main( int argc, char *argv[] )
 *  @param  [in]    argc      : The number of arguments passed
 *  @param  [in]    argv      : String of arguments passed
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
int main(int argc, char *argv[])
{
	int ret = -1;

	if (argc < 2 ) {
		printf("please check correct parameter nums! argc=%d\n", argc);
	}
	else {
		if (strcmp(argv[1], "get_vic") == 0) {
			if (argc != 6 ) {
				printf("please check correct parameter nums! argc=%d\n", argc);
			}
			else {
				ret = fb_connector_get_vic(argv);
			}
		}
		else if (strcmp(argv[1], "get_vsize") == 0) {
			if (argc != 3 ) {
				printf("please check correct parameter nums! argc=%d\n", argc);
			}
			else {
				ret = fb_connector_get_vsize(strtoul(argv[2], NULL, 0));
			}
		}
		else {
			printf("please check correct operation!\n");
		}
	}

	return ret;
}

