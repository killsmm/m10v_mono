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
#include <netinet/in.h>
#include <errno.h>

#include <sys/un.h>
#include "set_streamd.h"
#define D_ERROR_STREAM_TYPE	(-1)
#define D_ERROR_PARAMETER_NUM	(-2)
#define D_ERROR_PARMENTER (-3)
#define D_ERROR_STREAMD_NO_EXIST (-4)

/////////////////////////////////////
// Main function for setting parameter of stream
// It is valid only when the content in /tmp/use_rtsp_func.def is 1 or 2.
// The streamd process must be started before this APL is executed.
// First parameter is -s or -p.
// -s mean set a stream's parameter.
// -p mean print a stream's parameter.
// Second parameter is stream type ID setting.
// 0~7:H264, 8~15:Hevc,16~19:MJPEG,20~23:YUV,24~27:Meta,28~31:Audio. 
// Third parameter is not permitted to set if first is -p.
// It is frame rate per second.
// Fourth and fifth are size of Jpeg sent. They are meaningless if the stream ID is not MJPEG.
////////////////////////////////////
int main(int argc, char *argv[])
{
	struct sockaddr_un saddr;
	int ret = 0;
	T_SET_STREAM_INFORMATION send_data;

	if(argc >1) {
		if(strcmp(argv[1],"-s")==0) {
			send_data.request_type = E_REQUEST_TYPE_SET;
			if(argc != 6) {
				printf("USAGE:%s [-s/-p] stream-id frame-rate width height\n",argv[0]);
				ret = D_ERROR_PARAMETER_NUM;
			}
			else{ 
				send_data.stream_id = atoi(argv[2]);
				send_data.stream_frame_rate = atoi(argv[3]);
				send_data.image_width = atoi(argv[4]);
				send_data.image_height = atoi(argv[5]);
			}
		}
		else if(strcmp(argv[1],"-p")==0) {
			send_data.request_type = E_REQUEST_TYPE_PRINT;
			if(argc != 3) {
				printf("USAGE:%s [-s/-p] stream-id frame-rate width height\n",argv[0]);
				printf("The -s is set parameter to stream-id and the -p is print parameter for a stream-id.\n");
				ret = D_ERROR_PARAMETER_NUM;
			}
			else{ 
				send_data.stream_id = atoi(argv[2]);
			}
		}
		else {
			printf("USAGE:%s [-s/-p] stream-id frame-rate width height\n",argv[0]);
			printf("The -s is set parameter to stream-id and the -p is print parameter for a stream-id.\n");
			ret = D_ERROR_PARMENTER;
		}
	}
	else {
		printf("USAGE:%s [-s/-p] stream-id frame-rate width height\n",argv[0]);
		printf("The -s is set parameter to stream-id and the -p is print parameter for a stream-id.\n");
		ret = D_ERROR_PARAMETER_NUM;

	}
	if(ret == 0) {
		if(send_data.stream_id<32 && send_data.stream_id>=0) {
			int soc = socket( PF_UNIX, SOCK_STREAM, 0 );

			if(soc < 0 ) {
				printf("Can't open stream socket\n");
				ret = D_ERROR_STREAMD_NO_EXIST;
			}
			else {
				memset(&saddr, 0, sizeof( saddr ) );
				saddr.sun_family = PF_UNIX;
				strcpy( saddr.sun_path, SET_STREAM_INFOR );
				if ( connect( soc, ( struct sockaddr * )&saddr, 
					( socklen_t )sizeof( saddr ) ) < 0 ) {
					ret = D_ERROR_STREAMD_NO_EXIST;
					printf("Can't connect stream socket. Please start streamd before %s is executed.\n",argv[0]);
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
			printf("It must be 0<= stream-id <32\n");
		}
	}
	return ret;
}


