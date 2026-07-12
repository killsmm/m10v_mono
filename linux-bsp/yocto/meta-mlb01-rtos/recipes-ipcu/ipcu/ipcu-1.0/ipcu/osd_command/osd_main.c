#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <time.h>
#include <poll.h>
#include "osd_font.h"
#include "osd_common.h"
#include "osd_param_change.h"
#include "cmfwk_mm.h"
#include "cmfwk_std.h"
#include <syslog.h>
#include <errno.h>


extern FJ_ERR_CODE fj_mm_open(VOID);
int syslen;
char syslogstr[64];

void disp_usage( void )
{
	printf("[Usage]\n");
	printf("osdcmd [osd id],[cam id],[x-pos],[y-pos],[fontcolor],[fontsize],[parency],[text]\n");
	printf("\n");
	printf("[osd id]: osd id: 0 - 19\n");
	printf("[cam id]: cam id: 0 - 3, 2147483647 \n");
	printf("                 osd off ^^^^^^^^^^ \n");
	printf("[x-pos]: Disp x position: 0 - 65534\n");
	printf("[y-pos]: Disp y position: 0 - 65534\n");
	printf("[fontcolor]: fontcolor:0 WHITE\n");
	printf("                       1 BLACK\n");
	printf("                       2 GREEN\n");
	printf("                       3 BLUE\n");
	printf("                       4 PEACH\n");
	printf("                       5 YELLOW\n");
	printf("[fontsize]: fontsize: 0 SMALL\n");
	printf("                      1 MIDDLE\n");
	printf("                      2 LARGE\n");
	printf("[parency]: font back-ground type:0 BLACK\n");
	printf("                                 1 SKELTON\n");
	printf("[text]: Disp text: [string]( up to 30 characters)\n");
}

// ----------------------------------------------
// return cam_id corresponds system-texted osd_id.
// return CAM_ID_INVALID if osd_id is not system-texted.
// ----------------------------------------------
unsigned int lat_osdid2camid( unsigned int osd_id )
{
	unsigned int cam_id;
	
	switch( osd_id ){
		case OSD_ID_0:
		case OSD_ID_1:
		case OSD_ID_2:
			cam_id = CAM_ID_0;
			break;
		case OSD_ID_3:
		case OSD_ID_4:
		case OSD_ID_5:
			cam_id = CAM_ID_1;
			break;
		case OSD_ID_6:
		case OSD_ID_7:
		case OSD_ID_8:
			cam_id = CAM_ID_2;
			break;
		case OSD_ID_9:
		case OSD_ID_10:
		case OSD_ID_11:
			cam_id = CAM_ID_3;
			break;
		default:
			cam_id = CAM_ID_INVALID; // owner is user
			break;
	}
	return cam_id;
}

// ----------------------------------------------
// return text_type corresponds osd_id.
// return 255 if osd_id is more than OSD_ID_MAX.
// ----------------------------------------------
unsigned int lat_osdid2text_type( unsigned int osd_id )
{
	unsigned int text_type;
	
	switch( osd_id ){
		case OSD_ID_0:
		case OSD_ID_3:
		case OSD_ID_6:
		case OSD_ID_9:
			text_type = OSD_SYSTEM_TEXT_CAMID;
			break;
		case OSD_ID_1:
		case OSD_ID_4:
		case OSD_ID_7:
		case OSD_ID_10:
			text_type = OSD_SYSTEM_TEXT_IPADR;
			break;
		case OSD_ID_2:
		case OSD_ID_5:
		case OSD_ID_8:
		case OSD_ID_11:
			text_type = OSD_SYSTEM_TEXT_TIME;
			break;
		default:
			text_type = OSD_FREE_TEXT;
			break;
	}
	if( osd_id > OSD_ID_MAX ){
		text_type = CAM_ID_INVALID;
	}
	return text_type;
}


//gchar* get_address(gchar* devif)
char* get_address(char* devif)
{
  int fd;
  struct ifreq ifr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);
  ifr.ifr_addr.sa_family = AF_INET;
  strncpy(ifr.ifr_name, devif, IFNAMSIZ-1);
  ioctl(fd, SIOCGIFADDR, &ifr);
  close(fd);

  return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

void insert_system_text( T_OSD_OPTION* option )
{
	unsigned int text_type = lat_osdid2text_type( option->osd_id );
	char* ip;
	time_t timer;
	struct tm *t_st;

	switch( text_type ){
		case OSD_SYSTEM_TEXT_CAMID:
			sprintf( option->text,"Camera No.%1d",lat_osdid2camid(option->osd_id) );
			break;
		case OSD_SYSTEM_TEXT_IPADR:
			ip = get_address("eth0");
			sprintf( option->text,"%s",ip );
			break;
		case OSD_SYSTEM_TEXT_TIME:
			time(&timer);
			t_st = localtime(&timer);
			sprintf(option->text,"%4d/%02d/%02d %02d:%02d:%02d",
				(t_st->tm_year+1900),(t_st->tm_mon+1),t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec);
			break;
		default:
			break;
	}
}


int sn_daemon( int nochdir, int noclose )
{
	pid_t pid;

	pid = fork();
	if( pid != 0 )
	{
		syslen = sprintf(syslogstr, "%s[%d] waitpid error\n",__func__,__LINE__);
		syslog(LOG_ERR,syslogstr);
		exit( 0 );
	}
	else
	{
		if (setsid() < 0) {
			syslen = sprintf(syslogstr, "%s[%d] setsid error\n",__func__,__LINE__);
			syslog(LOG_ERR,syslogstr);
			exit(1);
		}
		if( pid != 0 )
		{
			exit( 0 );
		}
	}
	chdir("/");
	close(0);
	close(1);
	close(2);

	return 0;
}


int optstrtok(char **ostr, char* istr)
{
    char *tok;
    int cnt;
    cnt = 0;
    tok = strtok(istr,",");
    while( tok != NULL )
    {
        *ostr = tok;
        tok = strtok(NULL,",");
        *ostr++;
        cnt++;
    }
    return cnt;
}

//
// Socket communication
//

int osd_sock_initialize(int* sfd, int osd_id)
{
	struct sockaddr_un saddr;
	int len;
	int ret;
	char charbuf[64];

	ret = -1;
	if (( *sfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		printf("%s[%d] %03d : %s\n",__func__,__LINE__, errno, strerror(errno));
		ret = -1;
	}
	else
	{
		memset(&saddr, 0x00, sizeof(saddr));
		saddr.sun_family = PF_UNIX;
		sprintf(charbuf,"%s%02d",OSD_SETTING,osd_id);
		strcpy( saddr.sun_path, charbuf );
		printf("socket path \"%s\"\n", charbuf);
		unlink(charbuf);
		len = sizeof(saddr.sun_family) + strlen(charbuf);
		if (bind(*sfd, (struct sockaddr *)&saddr, len) < 0)
		{
			printf("%s[%d] %03d : %s\n",__func__,__LINE__, errno, strerror(errno));
			ret = -1;
		}
		else
		{
			if(listen(*sfd, 5) == -1)
			{
				printf("%s[%d] %03d : %s\n",__func__,__LINE__, errno, strerror(errno));
				ret = -1;
			}
			else
			{
				printf("OSD Socket ready\n");
				ret = 0;
			}
		}
	}
	return ret;
}
int osd_sock_receive(int* sfd, int tout_ms, T_OSD_OPTION* osd_option)
{
	struct sockaddr_un caddr;
	int cfd;
	int ret=0;
	int len;
	char buf[1024];
	struct pollfd pfd;

	pfd.fd = *sfd;
	pfd.events = POLLIN;

	poll(&pfd, 1, tout_ms);
	if( pfd.revents & POLLIN )
	{
//		printf("POLL IN\n");
		len = sizeof(caddr);
		if ((cfd = accept(*sfd, (struct sockaddr *)&caddr, (socklen_t *)&len)) < 0)
		{
			//printf("%s[%d] %03d : %s\n",__func__,__LINE__, errno, strerror(errno));
			ret = -1;
		}
		else
		{
			len = read(cfd, buf, sizeof(buf));
			if( len > 0 )
			{
				memcpy( (void*)osd_option, (void*)buf, sizeof(T_OSD_OPTION) );
				ret = 0;
/*
				printf("result from socket\n");
				printf("osd_id %08xh\n",osd_option->osd_id);
				printf("cam_id %08xh\n",osd_option->cam_id);
				printf("x_pos %d\n",osd_option->x_pos);
				printf("y_pos %d\n",osd_option->y_pos);
				printf("color %d\n",osd_option->fontcolor);
				printf("size %d\n",osd_option->fontsize);
				printf("transparent %d\n",osd_option->parency);
				printf("text not disp\n");
*/
			}
			else
			{
				ret = -1;
			}
			close(cfd);
		}
	}
	else
	{
//		printf("POLL TIMEOUT\n");
		ret = -1;
	}
	
	return ret;
}
int streaming_sock_terminate(int* sfd)
{
	close(*sfd);
	return 0;
}

int illegal_option_check( T_OSD_OPTION *option )
{
	int ret=0;
	
	if( option->osd_id > OSD_ID_MAX ){
		printf("illegal osd_id %d\n", option->osd_id);
		ret=-1;
	}
	if( option->cam_id > CAM_ID_MAX ){
		printf("illegal cam_id %d\n",option->cam_id);
		ret=-1;
	}
	if( option->x_pos > X_POS_MAX ){
		printf("illegal x_pos %d\n",option->x_pos);
		ret=-1;
	}
	if( option->y_pos > Y_POS_MAX ){
		printf("illegal y_pos %d\n",option->y_pos);
		ret=-1;
	}
	if( option->fontcolor > FONTCOLOR_MAX ){
		printf("illegal fontcolor %d\n",option->fontcolor);
		ret=-1;
	}
	if( option->fontsize > FONTSIZE_MAX ){
		printf("illegal fontsize %d\n",option->fontsize);
		ret=-1;
	}
	if( option->parency > PARENCY_MAX ){
		printf("illegal parency %d\n",option->parency);
		ret=-1;
	}
	if( strlen(&option->text[0]) > TEXT_LEN_MAX ){
		printf("text is too long\n");
		ret=-1;
	}
	return ret;
}

int arg_parse(char *str, T_OSD_OPTION *option)
{
	INT32 client_num;
	char *op_src[9];

	client_num = optstrtok(op_src,str);
	if(client_num == 2){
		option->osd_id = atoi(op_src[0]);
		option->cam_id = atoi(op_src[1]);
	}else if(client_num == 8){
		option->osd_id = atoi(op_src[0]);
		option->cam_id = atoi(op_src[1]);
		option->x_pos = atoi(op_src[2]);
		option->y_pos = atoi(op_src[3]);
		option->fontcolor = atoi(op_src[4]);
		option->fontsize = atoi(op_src[5]);
		option->parency = atoi(op_src[6]);
		strcpy( &(option->text[0]), op_src[7]);
	}

	return client_num;
}

void osdimageadr_rgba_init( UINT32 image_adr[][BUFF_NUM][YUV_ALPHA], UINT32 base_adr )
{
	// ALPHA is not used
	UINT32 i, buf_temp = base_adr;
	
	for(i=0;i<OSD_NUM;i++){ // DATE&TIME is double buffer, others are single.
		image_adr[i][0][0] = buf_temp;
		image_adr[i][0][1] = buf_temp; // not use
		buf_temp = buf_temp + RGBA_DRAW_BUFF_SIZE;
		if(lat_osdid2text_type(i) == OSD_SYSTEM_TEXT_TIME){
			image_adr[i][1][0] = buf_temp;
			image_adr[i][1][1] = buf_temp; // not use
			buf_temp = buf_temp + RGBA_DRAW_BUFF_SIZE;
		}else{
			image_adr[i][1][0] = image_adr[i][0][0];
			image_adr[i][1][1] = image_adr[i][0][1];
		}
	}
}

void date_time_disp_loop( T_OSD_OPTION* option, T_OSDCMD_COMMON_MEM* memarea, UINT32 img_adr[][YUV_ALPHA] )
{
	UCHAR *buf;
	UCHAR buff_id=0;
	UINT32 i,font_size=2;
	INT32 sfd, ret;
	T_OSD_OPTION osd_option_buf;
	
	// Init
	ret = osd_sock_initialize( &sfd, option->osd_id);
	if( ret != 0 ){
		printf("socket initialize failed\n");
	}
	
	while( 1 ){
		if( memarea->osdih[option->osd_id].cam_id != 0xFFFFFFFF ){
			insert_system_text( option );
//			printf("time=%s\n",option->text);
			buf = (UCHAR*)img_adr[buff_id][0];

			set_osd_font_color( option->fontcolor );
			set_osd_font_alpha( option->parency );
			font_size=2;
			for( i=0; i<option->fontsize;i++){	// Tricky Process :-)
				font_size = font_size * 2;
			}
			// DRAW
			font_write_RGBA4444( buf, strlen(option->text), (const UCHAR*)option->text, font_size );
			
			if( memarea->osdih[option->osd_id].rtos_busy_flg != 1 ){
				memarea->osdih[option->osd_id].linux_busy_flg = 1;
				
				memarea->osdih[option->osd_id].x_pos = (option->x_pos & 0xFFFE);
				memarea->osdih[option->osd_id].y_pos = (option->y_pos & 0xFFFE);
				memarea->osdih[option->osd_id].yuv_width = strlen(option->text)*OSD_CHAR_WIDTH*font_size;
				memarea->osdih[option->osd_id].yuv_height = OSD_CHAR_HEIGHT*font_size;
				// ADDRESS SWITCH
				memarea->osdih[option->osd_id].yuv_dst_addr = FJ_MM_virt_to_phys( img_adr[buff_id][0] );

				memarea->osdih[option->osd_id].linux_busy_flg = 0;
				buff_id++;
				buff_id = (buff_id % BUFF_NUM);
				// DEBUG PRINT
/*				printf("OSD Infomation Header is ...\n");
				printf("camid   : %08xh\n",memarea->osdih[option->osd_id].cam_id );
				printf("osd_adr : %08xh\n",memarea->osdih[option->osd_id].yuv_dst_addr );
				printf("linux bu: %08xh\n",memarea->osdih[option->osd_id].linux_busy_flg );
				printf("rtos bus: %08xh\n",memarea->osdih[option->osd_id].rtos_busy_flg );
				printf("x_pos   : %08xh\n",memarea->osdih[option->osd_id].x_pos );
				printf("y_pos   : %08xh\n",memarea->osdih[option->osd_id].y_pos );
				printf("osd_widt: %08xh\n",memarea->osdih[option->osd_id].yuv_width );
				printf("osd_heig: %08xh\n\n",memarea->osdih[option->osd_id].yuv_height );
*/
			}
		}
		ret = osd_sock_receive( &sfd, 900, &osd_option_buf );
		if( ret != 0 ){
//			printf("socket data recv failed or timeout\n");
		}else{
			sprintf(osd_option_buf.text,"tentative!");
			osd_option_buf.cam_id = lat_osdid2camid(osd_option_buf.osd_id);
			if( illegal_option_check( &osd_option_buf ) == -1 ){
				printf("socket communication success but parameter is invalid\n");
			}else{
				memcpy( (void*)option, (void*)&osd_option_buf, sizeof(T_OSD_OPTION) );
			}
		}
	}

	streaming_sock_terminate(&sfd);
	exit(-1);
}


int main(int argc, char *argv[]){
//  osdcmd [osd id],[cam id],[x-pos],[y-pos],[fontcolor],[fontsize],[parency],[text or format]
//	FILE *fpw;
//	UCHAR fname_w[] = "out.yuv";	// fileout debug
//	UCHAR str[]="RTOS_LINUX"; // drawing character
	UCHAR font_size=2;  // must be twice number
	UCHAR *buf;
	UCHAR timeout_cnt=0;
	UCHAR osd_off_flg = 0;
	T_OSD_OPTION option;
	T_OSDCMD_COMMON_MEM *memarea;
	UINT32 osdimage_adr_list[OSD_NUM][BUFF_NUM][YUV_ALPHA];
	UINT32 yuv_width, yuv_height, draw_size, i, phys_adr_top, m_size;
	INT32 client_num;

	// GET OPTION
	if( argc == 1 ){
		disp_usage();
		return 0;
	}else{
		client_num = arg_parse( argv[1], &option );
	}
	if( option.cam_id == 0x7FFFFFFF ){
		osd_off_flg=1;
	}else if( client_num != 8){
		disp_usage();
		return 0;
	}else if( illegal_option_check( &option ) == -1 ){
		disp_usage();
		return 0;
	}

    sn_daemon(0,1);
    
	// DEBUG PRINT
/*	printf("argc=%d\n",argc);
	printf("client_num=%d\n",client_num);
	printf("option parse result=0x%02x,0x%08x,0x%04x,0x%04x,0x%02x,0x%02x,0x%02x,%s \n",
	option.osd_id,option.cam_id,option.x_pos,option.y_pos,option.fontcolor,option.fontsize,
	option.parency,option.text);*/

	// get & asign share memory
	if (fj_mm_open() != FJ_ERR_OK) {
		printf("share mem open error.\n");
		return FJ_ERR_NG;
	}
	FJ_MM_getPhysAddrInfo(IPCU_MM_OSD, &phys_adr_top, &m_size);
	// DEBUG PRINT
//	printf("phys_adr=0x%08x, size=0x%08x\n",phys_adr_top,m_size);
	memarea = (T_OSDCMD_COMMON_MEM*)FJ_MM_phys_to_virt( phys_adr_top );
	
	osdimageadr_rgba_init( osdimage_adr_list, (UINT32)(&memarea->osd_image_area[0]) );

	// DEBUG PRINT
/*	for(i=0;i<OSD_NUM;i++){
		for(yuv_width=0;yuv_width<2;yuv_width++){
			for(yuv_height=0;yuv_height<2;yuv_height++){
				printf("osdimage_adr%3d%3d%3d: 0x%08x\n",i,yuv_width,yuv_height,osdimage_adr_list[i][yuv_width][yuv_height]);
			}
		}
	}
*/	
	// OSD OFF & exit
	if( osd_off_flg == 1){
		memarea->osdih[option.osd_id].cam_id = 0xFFFFFFFF;
		printf("osd_id%d off\n",option.osd_id);
		return 0;
	}
	
	if( lat_osdid2text_type(option.osd_id) == OSD_SYSTEM_TEXT_TIME ){
		date_time_disp_loop( &option, memarea, &osdimage_adr_list[option.osd_id][0][0] );
	}

	insert_system_text( &option );
	if( lat_osdid2text_type(option.osd_id) != OSD_FREE_TEXT ){
		option.cam_id = lat_osdid2camid( option.osd_id );
	}
	
	// draw size
	for( i=0; i<option.fontsize;i++){	// Tricky Process :-)
		font_size = font_size * 2;
	}
	yuv_width = strlen(option.text)*OSD_CHAR_WIDTH*font_size;
	yuv_height = OSD_CHAR_HEIGHT*font_size;
	draw_size = yuv_width*yuv_height; // Pixel
	draw_size = 2*draw_size ; // Pixel*2=RGBA4444
//	if( draw_size > YUV_DRAW_BUFF_SIZE ){
	if( draw_size > RGBA_DRAW_BUFF_SIZE ){
		printf("drawing size is too large\n");
		printf("reduce text or fontsize\n");
		return 0;
	}
	
	buf = (UCHAR*)(osdimage_adr_list[option.osd_id][0][0]);

	// DEBUG PRINT
//	printf("draw buffer: 0x%08x\n",buf);

	// DRAW
	set_osd_font_color( option.fontcolor );
	set_osd_font_alpha( option.parency );
	font_write_RGBA4444( buf, strlen(option.text), (const UCHAR*)option.text, font_size );
	
	// Update OSD Information Header
	while( timeout_cnt < 5 ){
		if( memarea->osdih[option.osd_id].rtos_busy_flg != 1 ){
			memarea->osdih[option.osd_id].linux_busy_flg = 1;
			memarea->osdih[option.osd_id].cam_id = option.cam_id;
			memarea->osdih[option.osd_id].yuv_dst_addr = FJ_MM_virt_to_phys( osdimage_adr_list[option.osd_id][0][0] );
			memarea->osdih[option.osd_id].x_pos = ( option.x_pos & 0xFFFE);
			memarea->osdih[option.osd_id].y_pos = ( option.y_pos & 0xFFFE);
			memarea->osdih[option.osd_id].yuv_width = yuv_width;
			memarea->osdih[option.osd_id].yuv_height = yuv_height;
			memarea->osdih[option.osd_id].linux_busy_flg = 0;
			break;
		}else{
			timeout_cnt++;
//			printf("rtos is busy\n");
			usleep(40*1000);
		}
	}
	if(timeout_cnt >= 5){
		printf("now, rtos busy. timeout\n");
		return 0;
	}

	// DEBUG PRINT
/*	printf("OSD Infomation Header is ...\n");
	printf("camid   : %08xh\n",memarea->osdih[option.osd_id].cam_id );
	printf("osd_adr : %08xh\n",memarea->osdih[option.osd_id].yuv_dst_addr );
	printf("linux bu: %08xh\n",memarea->osdih[option.osd_id].linux_busy_flg );
	printf("rtos bus: %08xh\n",memarea->osdih[option.osd_id].rtos_busy_flg );
	printf("x_pos   : %08xh\n",memarea->osdih[option.osd_id].x_pos );
	printf("y_pos   : %08xh\n",memarea->osdih[option.osd_id].y_pos );
	printf("osd_widt: %08xh\n",memarea->osdih[option.osd_id].yuv_width );
	printf("osd_heig: %08xh\n\n",memarea->osdih[option.osd_id].yuv_height );
*/
//	fpw = fopen( (const CHAR*)fname_w, "wb" );
//	fwrite( buf, sizeof( UCHAR ), draw_size, fpw );
//	fclose( fpw );
//	free(buf);
//	printf("use fontsize=%d\n",font_size);
//	printf("draw_size=%d\n",draw_size);
	return 0;
}

