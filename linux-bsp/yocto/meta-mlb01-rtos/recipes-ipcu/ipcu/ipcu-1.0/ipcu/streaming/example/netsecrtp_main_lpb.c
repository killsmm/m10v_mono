/******************************************************************************
  @file   netsecrtplpb.c
  @brief  test sample
  Copyright 2018 Socionext Inc.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "cif_stream.h"
#include "cmfwk_mm.h"
#include <time.h>
#include "netsecrtp_api.h"
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <arpa/inet.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};

enum SEMAPHORE_OPERATION
{
    UNLOCK = -1,
    WAIT = 0,
    LOCK = 1,
};

static int app_abort = 0;
NETSEC_RX_QUE* rxq;
int netsec_fd;
int gSem_id;

#define H264_BASE_PORT	5004
#define HEVC_BASE_PORT	6004
#define JPEG_BASE_PORT	7004
#define AUDIO_BASE_PORT	10004
#define VIDEO_BASE_SESSDBID	0
//#define H264_BASE_SESSDBID	0
//#define HEVC_BASE_SESSDBID	8
#define JPEG_BASE_SESSDBID	16
#define AUDIO_BASE_SESSDBID	24

//#define PERF_ENABLE
#ifdef PERF_ENABLE
struct timespec gstart, gend;//performance
long diff;
void perf_init()
{
	clock_getres(CLOCK_REALTIME, &gstart);
	clock_getres(CLOCK_REALTIME, &gend);
	printf("res :    %10ld.%09ld \n", gstart.tv_sec, gstart.tv_nsec);
	printf("res :    %10ld.%09ld \n", gend.tv_sec, gend.tv_nsec);
}
#define perf_start() \
	clock_gettime(CLOCK_REALTIME, &gstart);

#define perf_end() \
	clock_gettime(CLOCK_REALTIME, &gend);

#define perf_show() \
	diff = gend.tv_nsec - gstart.tv_nsec; \
	if( diff < 0 ) diff += 1000000000; \
	printf("%s\t:%ld ns\n",__func__,diff); 
#endif //PERF_ENABLE

int semapho_init( void )
{
    /* get semapho */
    const key_t key = 112;
    int sem_flags = 0666;
    int sem_id = semget(key, 1, sem_flags | IPC_CREAT);
    if(-1 == sem_id)
    {
		printf("semapho get fail\n");
        return (-1);
    }
    /* Init */
    union semun argument;
    unsigned short values[1];
    values[0] = 1;
    argument.array = values;
    semctl(sem_id, 0, SETALL, argument);

    return sem_id;
}

int semapho_free( int sem_id )
{
	int result;
    /* semapho free */
    result = semctl(sem_id, 1, IPC_RMID, NULL);
    if(-1 == result)
    {
 		printf("semapho free fail\n");
        return (-1);
    }
	return 0;
}

int semapho_operation( int sem_id , enum SEMAPHORE_OPERATION op)
{
    struct sembuf operations[1];
    operations[0].sem_num = 0;
    operations[0].sem_op = op;
    operations[0].sem_flg = SEM_UNDO;
    semop(sem_id, operations, 1);
	return 0;
}

static void signal_handler(int signal)
{
	app_abort = 1;
	semapho_operation( gSem_id, UNLOCK );
}
static void sigterm_handler(int signal)
{
	app_abort = 1;
	semapho_operation( gSem_id, UNLOCK );
}
int socket_fd;
struct sockaddr_in addr[32];

// For Socket
void sock_send(int sfd, char* buf, unsigned long len, int sessdbid)
{
	sendto(sfd, buf, len, 0, (struct sockaddr *)&addr[sessdbid], sizeof(struct sockaddr_in));
}
int sock_init(const char* ip)
{
	int sessdbid;
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

	// common
	for(sessdbid=0; sessdbid<32; sessdbid++){
		addr[sessdbid].sin_family = AF_INET;
		addr[sessdbid].sin_addr.s_addr = inet_addr(ip);
	}
	
	for(sessdbid=0; sessdbid<8; sessdbid++){
		addr[sessdbid    ].sin_port = htons(H264_BASE_PORT  + 10*sessdbid );
		addr[sessdbid + 8].sin_port = htons(HEVC_BASE_PORT  + 10*sessdbid );
		addr[sessdbid +16].sin_port = htons(JPEG_BASE_PORT  + 10*sessdbid );
		addr[sessdbid +24].sin_port = htons(AUDIO_BASE_PORT + 10*sessdbid );
	}
	printf("Destination IP %s\n", ip);
	printf("Destination port H264 %5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d\n",
		   ntohs(addr[0].sin_port),ntohs(addr[1].sin_port),ntohs(addr[2].sin_port),ntohs(addr[3].sin_port),
		   ntohs(addr[4].sin_port),ntohs(addr[5].sin_port),ntohs(addr[6].sin_port),ntohs(addr[7].sin_port));
	printf("Destination port HEVC %5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d\n",
		   ntohs(addr[8].sin_port),ntohs(addr[9].sin_port),ntohs(addr[10].sin_port),ntohs(addr[11].sin_port),
		   ntohs(addr[12].sin_port),ntohs(addr[13].sin_port),ntohs(addr[14].sin_port),ntohs(addr[15].sin_port));
	printf("Destination port JPEG %5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d\n",
		   ntohs(addr[16].sin_port),ntohs(addr[17].sin_port),ntohs(addr[18].sin_port),ntohs(addr[19].sin_port),
		   ntohs(addr[20].sin_port),ntohs(addr[21].sin_port),ntohs(addr[22].sin_port),ntohs(addr[23].sin_port));
	printf("Destination port AUDIO %5d\n", ntohs(addr[24].sin_port));
	return 0;
}
int sock_term()
{
	return close(socket_fd);
}

// For data dump
int debug_dump(char* buf, unsigned long size)
{
	FILE* fp;
	static unsigned long i = 0;
	char filename[255];
	int ret;

	sprintf(filename, "%s_%04lu", "/tmp/rtplpb", i++);
	fp = fopen(filename, "wb");
	if (fp < 0) {
		printf("%s: failed to open %s \n", __func__, filename);
		ret = -1;
	}
	else
	{
		fwrite(buf, sizeof(char), size, fp);
		ret = 0;
	}
	fclose(fp);

	return ret;
}

const char* help_msg[]=
{
	"Usage:",
	"  netsecrtplpb",
//	"  -a    :Audio streaming enable.(exclusive video streaming)  default Video streaming",
//	"  -p    :Port for destination. default 5004",
	"  -t    :Payload type of RTP. default 96 ",
	"  -i    :IPv4 address for destination. default 192.168.10.7",
	"  -w    :width for M-JPEG. default 2040",
	"  -v    :vertical size for M-JPEG. default 2040",
	"  -h    :help",
	"",
//	" [For Video streaming command] / # netseclpb -v 0 -p 5004",
//	" [For audio streaming command] / # netseclpb -v 1 -a -p 10004",
	" [Example] / # netseclpb -t 96 -i 192.168.10.7 -w 1280 -v 720",
	"You should write SDP file as the following for Playback"
	"  Computer IP address is 192.168.10.7",
	"  M10V IP address is 192.168.10.16",
	"---8<-------------------",
	"v=0",
	"c=IN IP4 192.168.10.16",
	"m=video 5004 RTP/AVP 96",
	"a=rtpmap:96 H264/90000",
	"a=fmtp:96",
	"------------------>8----",
	NULL,
};
int get_rtppkt_and_send_sock( void )
{
	static unsigned long rid = 0;
	int retry;
	int ret;
	int sessdbid;
	retry = 0;
	
	ret = 0;
	while( 1 )
	{
		if(rxq->rxque[rid].length > 0) {
//			debug_dump(&rxq.rxque[rid].rx_buff,rxq.rxque[rid].length);
//			printf("rtplpb DS 0x%08x\n", rxq->rxque[rid].desc0);
			sessdbid = (0x0F&rxq->rxque[rid].desc0) | ( (rxq->rxque[rid].desc0>>12)&0x10 );	// tentative
			sock_send(socket_fd,(char*)&rxq->rxque[rid].rx_buff,rxq->rxque[rid].length, sessdbid );
			retry = 0;
			rxq->rxque[rid].length = 0;
			rid++;
			if( rid >= NUM_RXQ ){
				rid = 0;
			}

		}
		if(retry++ > 10){
			ret = -1;
			break;
		}
	}
	return ret;
}

// For dummy callback
int videocb(struct cif_stream_send* p, void *d)
{
	return 0;
}

/******************************************************************************
   @brief  jpeglpbcb
   @fn int jpeglpbcb(struct cif_stream_send* p, void *d)
   @param  [in]    *p      : callback parameter from RTOS
   @param  [in]    *d      : user data
   @note
    The function is called by IPCU receiving notification.
   @author
   @version
*****************************************************************************/
int jpeglpbcb(struct cif_stream_send* p, void *d)
{
	struct jpeg_descriptor_info desc;

	desc.nvcl = 0;
	desc.ts = p->media.j.sample_pts[0];
	desc.fd = netsec_fd;
	desc.paddr  = p->sample_address;
	desc.len = p->sample_size;
    desc.qtable_ofs[0] = p->media.j.ytbl_ofs;
    desc.qtable_ofs[1] = p->media.j.ctbl_ofs;
    desc.qtable_sz[0] = p->media.j.ytbl_siz;
    desc.qtable_sz[1] = p->media.j.ctbl_siz;
	desc.es_vid = p->stream_id; // VIDEO BASE
	NSec_set_jpeg2_rtp_lpb_Descriptor(desc);
//	get_rtppkt_and_send_sock();
	semapho_operation( gSem_id, UNLOCK );

	return 0;
}

/******************************************************************************
   @brief  audiolpbcb
   @fn int audiolpbcb(struct cif_stream_send* p, void *d)
   @param  [in]    *p      : callback parameter from RTOS
   @param  [in]    *d      : user data
   @note
    The function is called by IPCU receiving notification.
   @author
   @version
*****************************************************************************/
int audiolpbcb(struct cif_stream_send* p, void *d)
{
	struct ts_descriptor_info desc;
	unsigned int *vaddr;
	unsigned short size;
	unsigned char ffcnt = 0;

	vaddr = (UINT32 *)FJ_MM_phys_to_virt(p->sample_address);

	size = p->sample_size+7;

	while(ffcnt < 10){ //
		if( size < 0xFF ){
			*((char*)vaddr - 1 -7) = (size & 0xFF);
			break;
		}else{
			size = size - 0xFF;
			ffcnt++;
			*((char*)vaddr - ffcnt - 1 -7) = 0xFF;
		}
	}
	desc.ts = p->media.a.time_stamp[0];
	desc.fd = netsec_fd;
	desc.paddr  = p->sample_address - 1 - ffcnt -7;
	desc.len = p->sample_size + 7 + 1 + ffcnt;
	desc.es_vid = AUDIO_BASE_SESSDBID;
	NSec_set_audio_rtp_lpb_Descriptor(desc);
//	get_rtppkt_and_send_sock();
	semapho_operation( gSem_id, UNLOCK );

	return 0;
}

/******************************************************************************
   @brief  videolpbcb
   @fn int videolpbcb(struct cif_stream_send* p, void *d)
   @param  [in]    *p      : callback parameter from RTOS
   @param  [in]    *d      : user data
   @note
    The function is called by IPCU receiving notification.
   @author
   @version
*****************************************************************************/
int videolpbcb(struct cif_stream_send* p, void *d)
{
	struct ts_descriptor_info desc;
	unsigned long paddr;
	unsigned long vaddr;
	int i;
	int nal_type_nvcl = 0;
	int nal_unit_type = 0;
	unsigned long ofs;
	int nal_num;

	#ifdef PERF_ENABLE
	perf_start();
	#endif

	const char nalsz = 4;
	paddr = p->sample_address;
	vaddr = (unsigned long)FJ_MM_phys_to_virt(paddr);
	nal_num = p->media.v.nal_unit_num;
	desc.ts = p->media.v.sample_pts[0];
	ofs = 0;

	desc.fd = netsec_fd;
	desc.len = 0;
	if( vaddr != -1 )
	{
		//debug_dump((char*)(int)vaddr,p->sample_size);
		for( i = 0; i < nal_num; i++ )
		{
			if( p->stype == 2 )//H.264
			{
				//h.264 nal_unit_size does not include NAL header size
				desc.es_vid = p->stream_id + VIDEO_BASE_SESSDBID;
				desc.paddr = (paddr + ofs + nalsz);
				p->media.v.nal_unit_size[i] += nalsz;
				desc.len = p->media.v.nal_unit_size[i] - nalsz;
				desc.fmt = NSEC_H264;
				nal_type_nvcl = 5;
				nal_unit_type = *((char*)vaddr + ofs + nalsz) & 0x1F;
			}
			else if( p->stype == 1 )//HEVC
			{
				//HEVC nal_unit_size include NAL header size
				desc.es_vid = p->stream_id + VIDEO_BASE_SESSDBID;
				desc.paddr = (paddr + ofs + nalsz);
				desc.len = p->media.v.nal_unit_size[i] - nalsz;
				desc.fmt = NSEC_HEVC;
				nal_type_nvcl = 31;
				nal_unit_type = (*((char*)vaddr + ofs + nalsz) >> 1) & 0x00000003F;
			}
			else
			{
			}

			if( nal_unit_type > nal_type_nvcl )
			{
				desc.nvcl = NSEC_NVCL;
			}
			else
			{
				desc.nvcl = 0;
			}
			// Start RTP Loopback
			NSec_set_Video_rtp_lpb_Descriptor(desc);
//			get_rtppkt_and_send_sock();
			semapho_operation( gSem_id, UNLOCK );
			ofs = ofs + p->media.v.nal_unit_size[i];
		}
	}
	else
	{
		printf("Invalid address received 0x%08lx\n", paddr);
	}
	#ifdef PERF_ENABLE
	perf_end();
	perf_show();
	#endif

	return 0;
}

/******************************************************************************/
/**
 *  @brief  main
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
	int loop = 0;
	int opt;
	int sessdbid;
	unsigned long jpeg_h, jpeg_w;
	struct cb_main_func func;
	struct ipcu_param ipcu_prm;
	char ip[32];
	unsigned short payloadtype;

	if (signal(SIGINT, signal_handler) == SIG_ERR)
	printf("SIGINT erro\n");
	if (signal(SIGTERM, sigterm_handler) == SIG_ERR)
	printf("SIGTERM erro\n");

	#ifdef PERF_ENABLE
	perf_init();
	#endif
	
	func.video = (void*)videolpbcb;
	func.audio = (void*)audiolpbcb;
	func.raw   = NULL;
	func.yuv   = NULL;
	func.meta  = NULL;
	func.jpeg  = (void*)jpeglpbcb;
	func.user_data = NULL;
	ipcu_prm.chid = 6;
	ipcu_prm.ackid = 7;
	ipcu_prm.phys = 1;
	ipcu_prm.sw = 0;
	payloadtype = 96;
	jpeg_h = 2040;
	jpeg_w = 2040;
	memset(ip, 0, sizeof(ip));
	ip[0] = 0;
	while ((opt = getopt(argc, argv, "hi:t:w:v:")) != -1)
	{
		switch (opt)
		{
		case 't':
			payloadtype = atoi(optarg);
			break;
		case 'i':
			sprintf(ip, "%s",optarg);
			break;
		case 'w':
			jpeg_w = atoi(optarg);
			break;
		case 'v':
			jpeg_h = atoi(optarg);
			break;
		case 'h':
			for(loop = 0; help_msg[loop] != NULL; loop++) {
				printf("%s\n",help_msg[loop]);
			}
			return 0;
		case '?':
			break;
		}
	}
	/* Register Ctrl-C event handler */
	signal(SIGINT, signal_handler);

	if((jpeg_h < 0)||(jpeg_h > 2040)){
		jpeg_h = 2040;
	}
	if((jpeg_w < 0)||(jpeg_w > 2040)){
		jpeg_w = 2040;
	}

	if( ip[0] == 0 )
	{
		sprintf(ip, "192.168.10.7");
	}
	sock_init(&ip[0]);
	
	// NETSEC initialize for Loop Back
	struct session_db_info sess_info;
	netsec_fd = open("/dev/netsec0", O_RDWR);
	if( netsec_fd < 0 )
	{
		printf("%03d : %s\n", errno, strerror(errno));
	}
	
	rxq = (NETSEC_RX_QUE*)malloc( sizeof(NETSEC_RX_QUE) );
	if( rxq == NULL ){
		return 0;
	}
	NSec_set_rtp_lpb_buffer( netsec_fd, rxq );

	for( sessdbid=0; sessdbid<32; sessdbid++){
		sess_info.fd = netsec_fd;
		sess_info.es_vid = sessdbid;
		sess_info.rtp_payload_type = payloadtype;// RTP payload type
		sess_info.ssrc = 0x12345678 + sessdbid;// RTP SSRC
		sess_info.ntp = 0;
		sess_info.yuv_pix_num = 0;
		if( (sessdbid >= JPEG_BASE_SESSDBID) && (sessdbid < JPEG_BASE_SESSDBID + 8) ){ // for MJPEG setting
			sess_info.sessdb08  = 0x01000000 & 0xFF000000;
			sess_info.sessdb08 |= 0x00FF0000 & 0x00FF0000;//Q
			sess_info.sessdb08 |= ((jpeg_w>>3) <<  8)  & 0x0000FF00;//width/8. max 8*255=2040
			sess_info.sessdb08 |= ((jpeg_h>>3) <<  0)  & 0x000000FF;//height/8. max 8*255=2040
			sess_info.sessdb09  = 0x00000000 & 0x0000FFFF;//restart interval
			sess_info.sessdb09 |= 0x00003000 & 0xFFFF3FFF;
			sess_info.sessdb09 |= 0x00003FFF & 0xFFFFC000;//restart count
			sess_info.sessdb10  = 0x00000000 & 0x00FFFFFF;//MBZ
			sess_info.sessdb10 |= 0x00000000 & 0xFF00FFFF;//precision					jpeg_num++;
		}else{
			sess_info.sessdb08 = 0;
			sess_info.sessdb09 = 0;
			sess_info.sessdb10 = 0;
		}
		sess_info.sessdb11 = 0;
		// set session DB and regist receiving buffer
		NSec_set_streamdb_rtp_lpb(sess_info);
	}

	ipcu_prm.sw = 0;
	Stream_ipcu_ch_init(&func, ipcu_prm);

	gSem_id = semapho_init();
	if(gSem_id == -1){
		return -1;
	}
	semapho_operation( gSem_id, LOCK );
	
	/* Main Loop */
	while (!app_abort) {
//		usleep(100000);
		semapho_operation( gSem_id, WAIT );
		get_rtppkt_and_send_sock();
		semapho_operation( gSem_id, LOCK );
	}

	sock_term();
	Stream_ipcu_ch_close();
	free( rxq );
	semapho_free( gSem_id );

	return 0;
}

