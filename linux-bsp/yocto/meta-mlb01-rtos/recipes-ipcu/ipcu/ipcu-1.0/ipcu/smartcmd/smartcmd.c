/******************************************************************************/
/**
 *  @file   smartcmd.c
 *  @brief  test sample for IPCU Smart Command
 */
/*
 *  Copyright 2015 Socionext Inc.
 ******************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "cmfwk_ipcu.h"

#define SERIAL_PORT "/dev/ttyUSI0"
#define MAX_LENGTH 1024

static INT32 app_abort = 0;
static void signal_handler(INT32 signal)
{
	app_abort = 1;
}

const char* help_msg[]=
{
	"Usage:",
	"  smartcmd id=N [t=TTY]",
	" ",
	"Options:",
	"  id=N         : IPCU channel id (N=0..15)",
	"  t=TTY        : tty path (TTY=Full path) [default: /dev/ttyUSI0]",
	NULL,
};


/******************************************************************************/
/**
 *  @brief  sample_app
 *  @fn void callback(UINT8 id, UINT32 pdata, UINT32 length, UINT32 cont, UINT32 total_length)
 *  @param  [in]    id           : instance ID
 *  @param  [in]    pdata        : top address of the received data table
 *  @param  [in]    length       : byte size of data
 *  @param  [in]    cont         : continue flag: continue=1, no-continue(last)=0 for separeted communitcation by 32KB size
 *  @param  [in]    total_length : total length of data when separeted communitcation
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
void callback(UINT8 id, UINT32 pdata, UINT32 length, UINT32 cont, UINT32 total_length)
{
	LOGV("[callback] IPCU#%d: buf=0x%08x len=%d cont=%d total_len=%d \n", id, pdata, length, cont, total_length);
}


/******************************************************************************/
/**
 *  @brief  sample_app
 *  @fn int smartcmd(UINT8 channel_id, UINT32 paddr, char *tty_path)
 *  @param  [in]    channel_id  : Instance ID of IPCU channel
 *  @param  [in]    paddr       : Address of shared memory (0: non-shared memory mode)
 *  @param  [in]    tty_path    : Full path to a tty device
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
int smartcmd(UINT8 channel_id, UINT32 paddr, char *tty_path)
{
	char     in_char;
	INT32  tty_fd, length;
	struct termios old_ttyarg, new_ttyarg;
	char *buf;
	
	/* Prepare TTY */
	tty_fd = open(tty_path, (O_RDWR | O_SYNC));
	if (tty_fd < 0) {
		LOGE("%s: failed to open %s \n", __func__, tty_path);
		return 1;
	}
	ioctl(tty_fd, TCGETS, &old_ttyarg);
	new_ttyarg = old_ttyarg;
	new_ttyarg.c_lflag &= ~(ECHO | ICANON);
	new_ttyarg.c_cc[VMIN] = 0;
	new_ttyarg.c_cc[VTIME] = 1;
	ioctl(tty_fd, TCSETS, &new_ttyarg);
	
	/* Prepare input buffer */
	if (paddr) {
		/* TODO: Decide memory segment */
		buf = (char *)malloc(MAX_LENGTH);
	} else {
		buf = (char *)malloc(MAX_LENGTH);
	}
	
	/* Main Loop */
	memset(buf, 0, MAX_LENGTH);
	length = 0;
	while (!app_abort) {
		if (read(tty_fd, &in_char, 1) <= 0) {
			continue;
		}
		
		// Non-Control Character
		if (!(iscntrl(in_char))) {
			if (length < (MAX_LENGTH - 2)) {
				buf[length++] = in_char;
				putchar(in_char);
				fflush(NULL);
				continue;
			}
		}
		
		// Control Character
		switch (in_char) {
		case 0x08:	// BS
		case 0x7F:	// Del
			if (length > 0) {
				buf[--length] = '\0';
				char bs = 0x08;
				char space = ' ';
				putchar(bs);
				putchar(space);
				putchar(bs);
				fflush(NULL);
			}
			break;
		case 0x0a:	// Enter
			buf[length++] = '\r';   // CR
			buf[length++] = '\0';
			putchar(in_char);
			
			FJ_IPCU_Send(channel_id, (UINT32)buf, length, IPCU_SEND_SYNC);
			memset(buf, 0, MAX_LENGTH);
			length = 0;
			break;
		}
	}
	
	/* Send a command to terminate the communication */
	memset(buf, 0, MAX_LENGTH);
	length = 0;
	buf[length++] = 'X';    // Toggle between UART and IPCU
	buf[length++] = '\r';   // CR
	buf[length++] = '\0';
	FJ_IPCU_Send(channel_id, (UINT32)buf, length, IPCU_SEND_SYNC);
	
	/* Clean up input buffer */
	if (paddr) {
		// DO NOTHING
	} else {
		free(buf);
	}
	
	/* Clean up */
	ioctl(tty_fd, TCSETS, &old_ttyarg);
	close(tty_fd);
	
	return 0;
}


/******************************************************************************/
/**
 *  @brief  sample_app
 *  @fn int main( int argc, int8_t* argv[] )
 *  @param  [in]    argc      : The number of arguments passed
 *  @param  [in]    argv      : String of arguments passed
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
int main(int argc, char *argv[])
{
	INT32  ret;
	UINT8  ch_id;
	
	INT32  loop;
	UINT32  help = 0, share = 0;
	char   tty_path[255] = {0};
	char   argstr[32] = {0};
	INT32  chid = -1;
	
	if (argc == 1) {
		help = 1;
	} else {
		for(loop = 1; loop < argc; loop++) {
			if( (*((int*)argv[loop]) & 0x0000FFFF) == 0x00003d74 ) // "t="
			{
				strncpy(tty_path, &argv[loop][2], sizeof(tty_path));
				LOGV("File path=%s\n", tty_path);
			}
			else if( (*((int*)argv[loop]) & 0x00FFFFFF) == 0x003d6469 ) // "id="
			{
				strncpy(argstr, &argv[loop][3], 8);
				chid = strtoul(argstr, NULL, 10);
				LOGV("chid=%d\n", chid);
			}
			else
			{
				help = 1;
			}
		}
	}
	
	/* Check parameters */
	if (chid < 0) {
		help = 1;
	}
	
	if (help) {
		for(loop = 0; help_msg[loop] != NULL; loop++) {
			printf("%s\n",help_msg[loop]);
		}
		return 0;
	}
	
	/* Register Ctrl-C event handler */
	signal(SIGINT, signal_handler);
	
	/* Preparation */
	ret = FJ_IPCU_Open(chid, &ch_id);
	if (ret != FJ_ERR_OK) {
		return 1;
	}
	
	/* Main Loop */
	if (tty_path[0]) {
		ret = smartcmd(ch_id, share, tty_path);
	} else {
		ret = smartcmd(ch_id, share, SERIAL_PORT);
	}
	
	/* Clean up */
	ret = FJ_IPCU_Close(ch_id);
	if (ret != FJ_ERR_OK) {
		// DO NOTHING
	}
	
	return 0;
}
