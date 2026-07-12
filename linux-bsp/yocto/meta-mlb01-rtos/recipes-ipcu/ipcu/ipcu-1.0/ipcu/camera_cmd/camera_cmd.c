/******************************************************************************/
/**
 *  @file   camera_cmd.c
 *  @brief  call camera_if and unzip result to stdout.
 *  @author tool grp
 */
/*
 *  Copyright 2017 Socionext Inc.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <linux/ipcu_userland.h>
#include "cmfwk_std.h"

// prototype's
static int call_camera_if(int argc, char *argv[], int recvLen);
static int call_shell(char cmd[], char *out[]);
static int print_ipcu_recv(int argc, char *argv[], int recvLen);

/******************************************************************************/
/**
 *  @brief  simple_tool_app
 *  @fn void main( int argc, int8_t* argv[] )
 *  @param  [in]    argc      : The number of arguments passed
 *  @param  [in]    argv      : String of arguments passed
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
int main(int argc, char *argv[])
{
	INT32 optchk = 0;
	INT32 ret = 0;
	int recvLen = 2;
	int isInternal = 0;

	// Check mode
	// -i 1: internal mode >> Read result & stdout
	// -i 0: Command request mode >> Call camera_if
	while((optchk = getopt(argc, argv, "i:r:")) != -1)
	{
		switch(optchk)
		{
			case 'i':
				isInternal = atoi(optarg);
				break;
			case 'r':
				recvLen = atoi(optarg);
				break;
		}
	}

	// Execution
	ret = !isInternal
	    ? call_camera_if(argc, argv, recvLen)
	    : print_ipcu_recv(argc, argv, recvLen);

	return ret;
}

static int call_camera_if(int argc, char *argv[], int recvLen)
{
	char buf[1024] = "";
	char cmd[1024] = "";
	INT32 i = 0;
	INT32 ret = 0;

	// Create argument
	for (i = optind; i < argc; i++)
	{
		strcat(buf, argv[i]);
		strcat(buf, " ");
	}

	sprintf(cmd, "camera_if %s", buf);
	strcpy(buf, "");
	
	// Call camera_if: call camera_if
	ret = call_shell(cmd, &buf);

	if (ret)
	{
		// Create argument
		sprintf(cmd, "camera_cmd -i 1 -r %d %s", recvLen, buf);
		strcpy(buf, "");

		// Call camera_if: unzip & stdout
		ret = call_shell(cmd, &buf);

		if (ret)
		{
			printf("%s\n", buf);
		}
	}

	return ret;
}

static int call_shell(char cmd[], char *out[])
{
	char buf[1024];
	char *ptr = NULL;
	FILE *fp = NULL;
	INT32 ret = 0;

	// Call process
	if ((fp = popen(cmd, "r")) != NULL)
	{
		strcpy(buf, "");

		// Regist result stdout
		while (1)
		{
			fgets(buf, 1024, fp);

			if (!feof(fp))
			{
				if ((ptr = strchr(buf, '\n')) != NULL)
				{
					*ptr = '\0';
				}

				strcat(out, buf);
				strcat(out, " ");
			}
			else
			{
				break;
			}
		}
	
		pclose(fp);
		ret = 1;
	}

	return ret;
}

static int print_ipcu_recv(int argc, char *argv[], int recvLen)
{
	INT32 ret = 1;
	INT32 cnt = 0;

	// Search result
	while (!(strcmp(argv[optind + cnt], "Receive") == 0
	     && strcmp(argv[optind + cnt + 1], "response.") == 0))
	{
		if (cnt > argc)
		{
			ret = 0;
			break;
		}

		cnt++;
	}

	if (ret)
	{
		// stdout to separated result
		switch (recvLen)
		{
			case 1:
				printf("%s\n",
					argv[optind + cnt + 5]);			
				break;
			case 2:
				printf("%s,%s\n",
					argv[optind + cnt + 5],
					argv[optind + cnt + 9]);	
				break;
			case 3:
				printf("%s,%s,%s\n",
					argv[optind + cnt + 5],
					argv[optind + cnt + 9],
					argv[optind + cnt + 13]);	
				break;
			case 4:
				printf("%s,%s,%s,%s\n",
					argv[optind + cnt + 5],
					argv[optind + cnt + 9],
					argv[optind + cnt + 13],
					argv[optind + cnt + 17]);	
				break;
			default:
				ret = 0;
				break;
		}
	}

	return ret;
}