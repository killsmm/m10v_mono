/******************************************************************************
 *
 *  @file   set_streamd.c
 *  @brief  
 *          Set streaming information for using NETSEC
 *  Copyright 2017 Socionext Inc.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#include <sys/un.h>
#include "osd_param_change.h"
#include "osd_common.h"
#define D_ERROR_STREAM_TYPE	(-1)
#define D_ERROR_PARAMETER_NUM	(-2)
#define D_ERROR_PARMENTER (-3)
#define D_ERROR_STREAMD_NO_EXIST (-4)


void disp_usage( char* cmd )
{
	printf("USAGE:%s [-s] osd_id x_pos y_pos color size trans-parent \n",cmd);
}

/////////////////////////////////////
// The osd process must be started before this APL is executed.
// First -s means set osd date&time's parameters
// Second parameter is OSD id ( Only 0,3,6,9 Valid.)
// 3rd parameter is x-position.
// 4th parameter is y-position.
// 5th parameter is font color.
// 6th parameter is font size.
// 7th parameter is trans-parency.
////////////////////////////////////
int main(int argc, char *argv[])
{
	struct sockaddr_un saddr;
	int ret = 0;
	T_OSD_OPTION send_data;
	char charbuf[64];

	if(argc >1) {
		if(strcmp(argv[1],"-s")==0) {
			if(argc != 8) {
				disp_usage(argv[0]);
				ret = D_ERROR_PARAMETER_NUM;
			}
			else{ 
				send_data.osd_id = atoi(argv[2]);
				send_data.x_pos = atoi(argv[3]);
				send_data.y_pos = atoi(argv[4]);
				send_data.fontcolor = atoi(argv[5]);
				send_data.fontsize = atoi(argv[6]);
				send_data.parency = atoi(argv[7]);
			}
		}else{
			disp_usage(argv[0]);
			ret = D_ERROR_PARMENTER;
		}
	}
	else {
		disp_usage(argv[0]);
		ret = D_ERROR_PARAMETER_NUM;

	}
	if(ret == 0) {
		if(send_data.osd_id<100) {
			int soc = socket( PF_UNIX, SOCK_STREAM, 0 );

			if(soc < 0 ) {
				printf("Can't open stream socket\n");
				ret = D_ERROR_STREAMD_NO_EXIST;
			}
			else {
				memset(&saddr, 0, sizeof( saddr ) );
				saddr.sun_family = PF_UNIX;
				sprintf(charbuf,"%s%02d",OSD_SETTING,send_data.osd_id);
				strcpy( saddr.sun_path, charbuf );
//				printf("socket path \"%s\"\n", charbuf);
				if ( connect( soc, ( struct sockaddr * )&saddr, 
					( socklen_t )sizeof( saddr ) ) < 0 ) {
					ret = D_ERROR_STREAMD_NO_EXIST;
					printf("Can't connect osd socket. Please start osdcmd before\n");
				}
				else {
					send(soc, (char*)(&(send_data)),
						 sizeof(send_data),0);
				}
				close( soc );
			}
		}
		else {
			ret=D_ERROR_STREAM_TYPE;
			printf("It must be 0<= osd_id < 100\n");
		}
	}
	return ret;
}


