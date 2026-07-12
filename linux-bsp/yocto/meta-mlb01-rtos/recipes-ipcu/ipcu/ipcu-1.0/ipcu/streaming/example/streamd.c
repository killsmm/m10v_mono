/******************************************************************************
 *
 *  @file   streamd.c
 *  @brief  
 *          streaming daemon for using NETSEC
 *  Copyright 2017 Socionext Inc.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include "netsecrtp_api.h"
#include "netsecrtp_rtcp.h"
#include "cif_stream.h"
#include "cmfwk_mm.h"
#include <time.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <poll.h>
#include "streamd.h"
#include "set_streamd.h"
#define SOCK_NAME "/tmp/streamd_sock"
#define RTSP_ON_OFF "/tmp/use_rtsp_func.def"
#define DEBUG_ON_OFF_SETTING "/tmp/use_rcp_debug.def"
#define NETSEC_NAME  "/dev/netsec0"
#define NALSZ 4
#define VIDEO_BASE 0
#define JPEG_BASE 16
#define YUV_BASE  20
#define META_BASE 24
#define AUDIO_BASE 28
#define D_ERROR_STREAM_TYPE	(-1)
#define D_ERROR_NETSEC_DEVICE	(-2)
#define D_ERROR_STREAM_NO_EXIST (-3)
#define D_YUV_PORT_BASE	(8004)
#define D_JPEG_PORT_BASE	(7004)
#define D_TEST_SEDN_META_PORT	(9004)
#define D_TEST_SEDN_YUV_PORT	(8004)
#define CO_FORCE_SEND_METER_DATA	(0)
#define CO_FORCE_SEND_YUV_DATA	(0)
#define CO_FORCE_SEND_DATA	(CO_FORCE_SEND_METER_DATA + CO_FORCE_SEND_YUV_DATA)
//#define CO_TEST_META_DATA_SAVE_TO_SD
//#define CO_TEST_YUV_DATA_SAVE_TO_SD

typedef enum {
	E_RTSP_FUNCTION_OFF =0,
	E_RTSP_FUNCTION_RTSP,
	E_RTSP_FUNCTION_RTSP_RTCP,
	E_RTSP_FUNCTION_RTSP_RTCP_SR_RR,	
}E_RTSP_FUNCTION;
	
#define RFC2435
#include <pthread.h>
//#define TEST_AUDIO_VIDEO
#define CO_USE_FRAME_RATE_COUNTER
#define NETSEC_ADDRESS_COUNT	(0x1b101210)
#define D_SEND_RTCP_INTERVAL_COUNTER_VIDEO	(15)	//Per 30 frmae send a RTCP. It must be 2^n-1
#define D_SEND_RTCP_INTERVAL_COUNTER_JPEG	(63)	//Per 30 frmae send a RTCP. It must be 2^n-1
#define D_SEND_RTCP_INTERVAL_COUNTER_AUDIO	(15)	//Per 30 frmae send a RTCP. It must be 2^n-1
#define MC_RTCP_PERIOD	(300)
typedef enum {
	E_DEBUG_FUNC_NONE =0,
	E_DEBUG_FUNC_RTCP_PRINT =0x1,
	E_DEBUG_FUNC_NO_DEMON =0x2,
	E_DEBUG_FUNC_PRINT_META =0x4,
}E_DEBUG_FUNC;
typedef  enum {
	E_THREAD_GET_STREAM = 0,
	E_THREAD_ADD_STREAM,
	E_THREAD_SET_STREAM_INFOR,
	E_THREAD_WAIT_STREAM_SENT,//Wait stream transport completation
	E_THREAD_NUMS
} E_THREAD;
static pthread_t gTh_reviceFromSock[E_THREAD_NUMS];
#define H264_VIDEO_BASE (VIDEO_BASE)
#define HEVC_VIDEO_BASE (VIDEO_BASE+8)

typedef struct {
	unsigned int stream_type_id;//0~7:H264, 8~15:Hevc,16~19:MJPEG,20~23:YUV,24~27:Meta,28~31:Audio
	unsigned int Can_be_send_flag;//1: can be sent, 0:Cna't be sent
	//For RTCP
	T_RTCP_RECEIV_INFOR rtcpiformation;
	unsigned int send_rtcp_interval;
	T_SENDER_INFOR send_rtcp;
#ifdef CO_USE_FRAME_RATE_COUNTER
	struct timespec StartTimeTicket;
#else//CO_USE_FRAME_RATE_COUNTER
	unsigned int rtcp_frame_counter;
#endif//CO_USE_FRAME_RATE_COUNTER
}T_MANGE_ADD_STREAM;
#define SEND_RTP_TIMER "/tmp/use_rcp_send_time.def"

#define D_MAX_OFFLOAD_SELECT_READ_CNT		(512)//Max sending num

static int gSendCompleteWaitTimer=-1;
static int gDebugFunc= E_DEBUG_FUNC_NONE;//Set to E_DEBUG_FUNC
static T_MANGE_ADD_STREAM gMangeStreamInformation[D_MAX_STREAM_NUM];

static T_STREAM_INFOR gStreamInformation[D_MAX_STREAM_NUM];//Can be sent stream data
static int gValid_num=0;//Valid num of gStreamInformation
static int gStream_count[E_ENCODE_TYPE_META]={
	0,0,0,0,0,0
};
static struct timespec gStartTime;
int netsec_fd;
static int app_abort = 0;
struct streamd dparam[STREMD_STRUCT_MAX];
struct streamd param;
int syslen;
char syslogstr[64];
static char* gMacAddress;
static char* gIpAddress;
static int gRTSP_func=E_RTSP_FUNCTION_OFF;//RTSP function on/off flag
static unsigned long gRandamNtp=0;
static int add_stream(char* buf);
static int get_rtsp_on_off(void);

/////////////////////////////////////
//Check if RTSP is valid or not. Set the result to a global varise
//Return always 0.
/////////////////////////////////////
static int get_rtsp_on_off(void)
{
	FILE *file;
	char buffer[10];


	file = fopen(RTSP_ON_OFF, "r");
	if(file !=NULL) {
		fgets(buffer, sizeof(buffer), file);
		gRTSP_func = buffer[0]-0x30;
		fclose(file);
	}
	switch(gRTSP_func) {
	case E_RTSP_FUNCTION_OFF:
		printf("RTSP+NETSEC OFF\n");
		break;
	case E_RTSP_FUNCTION_RTSP:
		printf("RTSP+NETSEC ON\n");
		break;
	default:
		printf("RTSP/RTCP+NETSEC ON\n");
		break;
	}
	file = fopen(SEND_RTP_TIMER, "r");
	if(file !=NULL) {
		fgets(buffer, sizeof(buffer), file);
		gSendCompleteWaitTimer = atoi(buffer);
		fclose(file);
	}
	return 0;
}


/////////////////////////////////////
//End a threads.
//Parameter[in]: path is the thread's path.
//Parameter[in]: end_thread is send end message ID to the thread.
/////////////////////////////////////
static int send_end_socket_thread(const char* path, int end_thread)
{
	struct sockaddr_un saddr;
	int ret = 0;
	int soc = socket( PF_UNIX, SOCK_STREAM, 0 );

	if(soc < 0 ) {
		ret = -1;
	}
	else {
		memset(&saddr, 0, sizeof( saddr ) );
		saddr.sun_family = PF_UNIX;
		strcpy( saddr.sun_path, path );
		if ( connect( soc, ( struct sockaddr * )&saddr, ( socklen_t )sizeof( saddr ) ) < 0 ) {
			ret = -2;
		}
		send(soc, (char*)(&(end_thread)), sizeof(end_thread),0);
		close( soc );
	}
	return ret;
}
/////////////////////////////////////
//End all threads.
/////////////////////////////////////
static void end_all_thread()
{
 	app_abort = 1;
	pthread_cancel(gTh_reviceFromSock[E_THREAD_WAIT_STREAM_SENT]);
	if(gRTSP_func) {
	//RTSP valid
		send_end_socket_thread(RTSP_INFO_SOCK_NAME,E_NETSEC_APL_ID_END);
		send_end_socket_thread(RTSP_ADD_SOCK_NAME,E_NETSEC_APL_ID_END);
	}
	send_end_socket_thread(SET_STREAM_INFOR,E_REQUEST_TYPE_END);
}
/////////////////////////////////////
//SIGINT's handler.
/////////////////////////////////////
static void signal_handler(int signal)
{
	end_all_thread();
}
/////////////////////////////////////
//SIGTERM's handler.
/////////////////////////////////////
static void sigterm_handler(int signal)
{
	end_all_thread();
}

const char* help_msg[]=
{
	"Usage: ",
	" straemd <IPv4>,<MAC>",
	" <IPv4>     : Destination IP  address",
	" <MAC>      : Destination Mac address",
	__DATE__,
	__TIME__,
	NULL,
};
#if defined(CO_USE_FRAME_RATE_COUNTER) || defined(TEST_AUDIO_VIDEO)
/////////////////////////////////////
//Get elapsed time between 2 times.
//Parameter[in] lastTime: Previous time.
//Parameter[in] current_time: Current time.
//Return: elapsed time.
/////////////////////////////////////
static long long GetAbsolute_nanoTimeDiff(
	const struct timespec* lastTime,
	struct timespec* current_time
	)
{
	long long absolute_nano;
	clock_gettime(CLOCK_MONOTONIC, current_time);

	if(current_time->tv_nsec >=lastTime->tv_nsec) {
		absolute_nano = (current_time->tv_sec-lastTime->tv_sec) * 1000*1000
			+(current_time->tv_nsec -lastTime->tv_nsec)/1000;
	}else{
		absolute_nano = (current_time->tv_sec - 1 - lastTime->tv_sec) * 1000*1000
			+(current_time->tv_nsec+1000000000 -lastTime->tv_nsec)/1000;
	}
	return absolute_nano;
}
#if defined(TEST_AUDIO_VIDEO)
/////////////////////////////////////
//Get elapsed time from this application start-up.
//Parameter[in] param1: A denominator of coefficient.
//Parameter[in] param2: A Numerator of coefficient.
//Return: elapsed time.
/////////////////////////////////////
static long long pass_time(int param1, int param2)
{
	struct timespec current_time;
	long long absolute_nano =
		GetAbsolute_nanoTimeDiff(&gStartTime,&current_time);

	absolute_nano /=(param1);
	absolute_nano *=param2;

	return absolute_nano;
}
#endif //TEST_AUDIO_VIDEO
#endif //(CO_USE_FRAME_RATE_COUNTER || TEST_AUDIO_VIDEO)
/////////////////////////////////////
//Get stream ID(serial number) from strem type ID.
//Parameter[in] stream_type_id: stream type ID.
//0~7:H264, 8~15:Hevc,16~19:MJPEG,20~23:YUV,24~27:Meta,28~31:Audio.
//Return: D_ERROR_STREAM_NO_EXIST is not gotten.
//        The other is a stream ID.
/////////////////////////////////////
static int GetStreamIndexFromStreamTypeId(unsigned int stream_type_id)
{
	int ret =D_ERROR_STREAM_NO_EXIST;
	for(int i=0;i<gValid_num;i++) {
		if(gMangeStreamInformation[i].stream_type_id == stream_type_id) {
			ret = i;
		}
	}
	return ret;
}
/////////////////////////////////////
//Check the stream whether it can be sent.
//Parameter[in] p: stream information.
//Parameter[in] stream_type_id: stream type ID.
//0~7:H264, 8~15:Hevc,16~19:MJPEG,20~23:YUV,24~27:Meta,28~31:Audio.
//Return: 0: can't be sent. Other: Can be sent.
/////////////////////////////////////
static int CheckCanbesend(const void *p, unsigned int stream_type_id)
{
	int ret =0;
//printf("stream_type_id:%u\n",stream_type_id);
	int steam_index = GetStreamIndexFromStreamTypeId(stream_type_id);
	if( steam_index >=0) {
		ret = gMangeStreamInformation[steam_index].Can_be_send_flag;
	}
	if(ret != 1) {
		Stream_release_with_queue(p);
	}
	return ret;
}
/////////////////////////////////////
//Copy RR block to allgMangeStreamInformation.rtcpiformation
//so that it can be sent at next RTCP sending
//Parameter[in]  rtcp: Received RTCP information.
//Parameter[out] SaveingRR: using to save RR
//Return: 0: Found out the stream from this SaveingRR and set it
//        other: Not found the stream from this SaveingRR
/////////////////////////////////////
static int SetRRBlock2NextSendSR(const T_RTCP_PACKET* rtcp_packet, T_MANGE_ADD_STREAM* SaveingRR)
{
	int ret=0;
	//found sender
	if(rtcp_packet->rtcp_head.bit.Length -2 >0) {
		char* rr_block =(char* )(rtcp_packet->ReportBlock+1);
		SaveingRR->send_rtcp.ReportBlockSize = 
			(rtcp_packet->rtcp_head.bit.Length - 1 )* 4;//Header and SSRC
		SaveingRR->send_rtcp.ReportBlocknum = SaveingRR->send_rtcp.ReportBlockSize/sizeof(T_REPORT_BLOCK);

		if((size_t)SaveingRR->send_rtcp.ReportBlockSize>
			D_MAX_SENDING_RR_NUM*sizeof(T_REPORT_BLOCK)) {
			SaveingRR->send_rtcp.ReportBlockSize =
				D_MAX_SENDING_RR_NUM*sizeof(T_REPORT_BLOCK);
			printf("Warning: RR bubber are not enough\n");
		}
		memcpy(SaveingRR->send_rtcp.sendingRR,
				rr_block,
				SaveingRR->send_rtcp.ReportBlockSize);
	}
	return ret;
}
//Set Debug function's 
static int GetDebugFunc()
{
	FILE *file;
	char buffer[10];
	int ret = 0;
	file = fopen(DEBUG_ON_OFF_SETTING, "r");
	if(file !=NULL) {
		fgets(buffer, sizeof(buffer), file);
		ret = buffer[0]-0x30;
		fclose(file);
	}
	return ret;
}
/////////////////////////////////////
//Call back fuction received RTCP from client
//Parameter[in] rtcp: Received RTCP information.
/////////////////////////////////////
static void ReceiveRTCPcb(const T_RTCP_RECEIVE_DATA* rtcp)
{

	if((gDebugFunc & E_DEBUG_FUNC_RTCP_PRINT) == E_DEBUG_FUNC_RTCP_PRINT) {
		//Print RTCP information
		printf("Client Port:%d,\n",
			rtcp->src_port);
		for(int i=0;i<rtcp->valid_receive_packet_num;i++) {
			printf("[%d] version:%d,padding:%d,RRcount:%d,packet_type:%d,Length:%d\n",i,
				rtcp->received_rtcp[i].rtcp_head.bit.version,
				rtcp->received_rtcp[i].rtcp_head.bit.padding,
				rtcp->received_rtcp[i].rtcp_head.bit.RRcount,
				rtcp->received_rtcp[i].rtcp_head.bit.packet_type,
				rtcp->received_rtcp[i].rtcp_head.bit.Length);
			for(int j=0;
					j<rtcp->received_rtcp[i].rtcp_head.bit.Length;
					j++) {
				printf("%08x\n",
					rtcp->received_rtcp[i].ReportBlock[j]);
			}
		}
	}
	if(gRTSP_func ==  E_RTSP_FUNCTION_RTSP_RTCP_SR_RR) {
		//Test RR send back
		//Save RR so that send back. 
		for(int i=0;i<rtcp->valid_receive_packet_num;i++) {
			if(rtcp->received_rtcp[i].rtcp_head.bit.packet_type == E_RTCP_RECEIVE_PT_RR) {
				for(int j=0;j<gValid_num;j++) {
					if(gMangeStreamInformation[j].rtcpiformation.src_rtcp_port
						 == rtcp->src_port) {
						SetRRBlock2NextSendSR(rtcp->received_rtcp, &gMangeStreamInformation[j]);
						break;
					}
				}
			}
		}
	}
	return;
}
/////////////////////////////////////
//Send RTCP SR to client. 
//Parameter[in] stream_type_id: stream type ID. 0~7:H264, 8~15:Hevc,16~19:MJPEG,20~23:YUV,24~27:Meta,28~31:Audio
//Parameter[in] rtp_ts: Stream's time stamp.
//Return 0: normal. D_ERROR_STREAM_NO_EXIST:The stream is not exist.
/////////////////////////////////////
static int SendRTCP(unsigned int stream_type_id,
		unsigned long rtp_ts )
{
	int ret =-1;
	int stream_id;
 	if(gRTSP_func <E_RTSP_FUNCTION_RTSP_RTCP){
		return 0;
	}
	stream_id = GetStreamIndexFromStreamTypeId(stream_type_id);
	if(stream_id >=0) {
		//Don't send Reoprt block
#ifdef CO_USE_FRAME_RATE_COUNTER
		struct timespec current_time;
		unsigned int absolute_ms = GetAbsolute_nanoTimeDiff(
			&gMangeStreamInformation[stream_id].StartTimeTicket,
			&current_time) / 1000;
		if(absolute_ms > 
			gMangeStreamInformation[stream_id].send_rtcp_interval){
			T_SENDER_INFOR* sr =&gMangeStreamInformation[stream_id].send_rtcp;
			sr->rtp_ts = rtp_ts ;

//				printf("%s %d,stream_id:%d,ts:%lx\n",
//				__func__,__LINE__,stream_id,rtp_ts);
			gMangeStreamInformation[stream_id].send_rtcp.stream_id = stream_id;
			send_rtcp_report(&gMangeStreamInformation[stream_id].send_rtcp,stream_type_id);
			
		}
#else //CO_USE_FRAME_RATE_COUNTER
		gMangeStreamInformation[stream_id].rtcp_frame_counter++;
		if((gMangeStreamInformation[stream_id].rtcp_frame_counter %
			gMangeStreamInformation[stream_id].send_rtcp_interval) ==0) {

			gMangeStreamInformation[stream_id].send_rtcp.stream_id = stream_id;
			send_rtcp_report(&gMangeStreamInformation[stream_id].send_rtcp,stream_type_id);
		}
#endif //CO_USE_FRAME_RATE_COUNTER
		ret=0;
	}
	return ret;		
}
static void* netsec_waitsend_Thread()
{
	struct pollfd fds ={
		0,0,0
	};
	int    ret;
	struct netsec_comp comp[D_MAX_OFFLOAD_SELECT_READ_CNT];
	int oldState;
	//Set Can cansel state
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldState);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldState);

	fds.fd = netsec_fd;
	fds.events = POLLIN | POLLERR;
	while(1) {
		ret = poll(&fds,1, gSendCompleteWaitTimer);
		if(ret == 0){
			//Timeout
			printf("poll timeout.\n");
		}
		else if(ret > 0){
			int rd = read(netsec_fd,comp,sizeof(comp))/sizeof(struct netsec_comp);
			for (int i = 0; i < rd; i++) {
				if (comp[i].uniq_id != 0) {
					Stream_release_with_queue((void *)comp[i].uniq_id);
				}
#if 0
				printf("rd:%d,comp1[%d](%X,%u)\n",
					rd,
					i,comp[i].uniq_id,comp[i].result);
#endif
			}
		}
		else{
			//Error
			printf("poll error:%d\n",ret);	
		}
	}
	pthread_exit(NULL);
	return 0;
}
void dump_stream_data(struct cif_stream_send* p){

        printf("\nMagicCode:%lx\n", p->MagicCode);
        printf("Format_Version:%lx\n", p->Format_Version);
        printf("Command:%lx\n", p->Command);
        printf("Sub_Command:%lx\n", p->Sub_Command);
        printf("stype:%lx\n", p->stype);
        printf("sformat:%lx\n", p->sformat);
        printf("stream_id:%lx\n", p->stream_id);
        printf("stream_end_flg:%lx\n", p->stream_end_flg);
        printf("area_index:%lx\n", p->area_index);
        printf("sample_size:%lx\n", p->sample_size);
        printf("sample_address:%lx\n", p->sample_address);

//      printf("reserved:%08x\n", p->media.v.reserved);
        printf("sample_pts 0:%lx\n", p->media.v.sample_pts[0]);
        printf("sample_pts 1:%lx\n", p->media.v.sample_pts[1]);
        printf("sample_type:%lx\n", p->media.v.sample_type);
        printf("frame_of_GOP:%lx\n", p->media.v.frame_of_GOP);
        printf("nal_unit_num:%lx\n", p->media.v.nal_unit_num);
        printf("bitrate:%lx\n", p->media.v.bitrate);
        printf("width:%ld\n", p->media.v.width);
        printf("lines:%ld\n\n", p->media.v.lines);
}

/* ------------------------
 IPCU Callback
--------------------------*/
/////////////////////////////////////
//Call back function called while a 
//H264/HEVC stream was sent from RTOS.
//Parameter[in]  p: stream information.
/////////////////////////////////////
int h26x_cb(struct cif_stream_send* p, void *d)
{
	struct ts_descriptor_info desc;
	unsigned long paddr;
	unsigned long vaddr;
	ULONG i;
	int nal_type_nvcl;
	int nal_unit_type;
	unsigned long ofs;
	U_NETSEC_DESC_ID uniq_id;

	if(gRTSP_func) {
	//RTSP valid
		if(CheckCanbesend(p, p->stream_id) !=1) {
			return 0;
		}
		
	}
	paddr  = p->sample_address;
	vaddr = FJ_MM_phys_to_virt(paddr);

	//dump_stream_data(p);
//	printf("%s %s[%d],id:%ul,time_stamp:%ul,\n", __FILE__,__func__,__LINE__,p->stream_id,p->media.v.sample_pts[0]);
	if(gRTSP_func) {
	//RTSP valid
	//Check Send RTCP


#ifdef TEST_AUDIO_VIDEO
//	desc.ts = 10;
		desc.ts = pass_time(100,9);
#else
		desc.ts = p->media.v.sample_pts[0];
#endif//TEST_AUDIO_VIDEO
		SendRTCP(p->stream_id,desc.ts);

	}
	else{
		desc.ts = p->media.v.sample_pts[0];
	}
	ofs = 0;

	desc.fd = netsec_fd;
	desc.len = 0;

	uniq_id.word = (u32)p;

	if( vaddr != (unsigned long)-1 )
	{
		Stream_release_set_count(p, p->media.v.nal_unit_num);
		for( i = 0; i < p->media.v.nal_unit_num; i++ )
		{
			if( p->stype == 2 )//H.264
			{
				//h.264 nal_unit_size does not include NAL header size
				desc.es_vid = p->stream_id;
				desc.paddr = (paddr + ofs + NALSZ);
				p->media.v.nal_unit_size[i] += NALSZ;
				desc.len = p->media.v.nal_unit_size[i] - NALSZ;
				desc.fmt = NSEC_H264;
				nal_type_nvcl = 5;
				nal_unit_type = *((char*)vaddr + ofs + NALSZ) & 0x1F;
			}
			else //HEVC
			{
				//HEVC nal_unit_size include NAL header size
				desc.es_vid = p->stream_id;
				desc.paddr = (paddr + ofs + NALSZ);
				desc.len = p->media.v.nal_unit_size[i] - NALSZ;
				desc.fmt = NSEC_HEVC;
				nal_type_nvcl = 31;
				nal_unit_type = (*((char*)vaddr + ofs + NALSZ) >> 1) & 0x00000003F;
			}
			#if 0
			for(int i = 0; i < 6; i++ )
			{
				printf( "%02X ", *((char*)vaddr + ofs + i) );
			}
			printf("\n");
			#endif
			if( nal_unit_type > nal_type_nvcl )
			{
				desc.nvcl = NSEC_NVCL;
			}
			else
			{
				desc.nvcl = 0;
			}
			//Set Desc release table for release 
			NSec_set_Video_Tx_Descriptor(desc, uniq_id);
			#ifdef EXPERIMENTAL_RTCP	
			RTCP_SETRTPTS(desc.ts);
			RTCP_SETPKTCNT;
			RTCP_SETPKTOCT(desc.len);
			#endif
			//multi for DEBUG
			ofs = ofs + p->media.v.nal_unit_size[i];
		}
	}
	else
	{
		printf("Invalid address received 0x%08lx\n", paddr);
	}

	return 0;
}
/////////////////////////////////////
//Call back function called while a 
//YUV stream was sent from RTOS.
//Parameter[in]  p: stream information.
/////////////////////////////////////
int yuv_cb(const struct cif_stream_send* p, void *d)
{
	struct ts_descriptor_info desc;
	U_NETSEC_DESC_ID uniq_id;

	if(gRTSP_func) {
	//RTSP valid
		int DoSend=CheckCanbesend(p, p->stream_id+YUV_BASE);
		if(DoSend !=1) {
			return 0;
		}
		desc.ts = p->media.y.sample_pts[0];
		SendRTCP(p->stream_id+YUV_BASE,desc.ts);
	}
//	printf("id:%d,w:%u,h:%u,pts:%08X%08X\n",
//		p->stream_id,
//		p->media.y.width,
//		p->media.y.lines,
//		p->media.y.sample_pts[1],p->media.y.sample_pts[0]);
#ifdef CO_TEST_YUV_DATA_SAVE_TO_SD
	{
		char string[256];
		sprintf(string,"/run/SD/mmcblk0p1/mmiotool -R 0x%x 0x%x /run/SD/mmcblk0p1/yuv/%08x.bin\n",
			p->sample_address,
			p->sample_size,
			p->media.y.sample_pts[0]);
		system(string);
	}
#endif 
	desc.nvcl = 0;
	desc.ts = p->media.y.sample_pts[0];
	desc.fd = netsec_fd;
	desc.fmt = NSEC_UYVY;
	desc.paddr  = p->sample_address;
	desc.len = p->sample_size;
	desc.es_vid = p->stream_id + YUV_BASE;

	uniq_id.word = (u32)p;
	NSec_set_Video_Tx_Descriptor(desc, uniq_id);

	return 0;
}
/////////////////////////////////////
//Call back function called while a 
//MJPEG stream was sent from RTOS.
//Parameter[in]  p: stream information.
/////////////////////////////////////
int jpeg_cb(const struct cif_stream_send* p, void *d)
{
	struct jpeg_descriptor_info desc;
	U_NETSEC_DESC_ID uniq_id;

	if(gRTSP_func) {
	//RTSP valid
		if(CheckCanbesend(p, p->stream_id) !=1) {
			return 0;
		}
	}
	desc.nvcl = 0;
	if(gRTSP_func) {
	//RTSP valid
	#ifdef TEST_AUDIO_VIDEO
		desc.ts = pass_time(100,9);
	#else
		desc.ts = p->media.j.sample_pts[0];
	#endif //TEST_AUDIO_VIDEO
		SendRTCP(p->stream_id,desc.ts);
	}
	else{
		desc.ts = p->media.j.sample_pts[0];
	}
	desc.fd = netsec_fd;
	desc.paddr  = p->sample_address;
	desc.len = p->sample_size;
    desc.qtable_ofs[0] = p->media.j.ytbl_ofs;
    desc.qtable_ofs[1] = p->media.j.ctbl_ofs;
    desc.qtable_sz[0] = p->media.j.ytbl_siz;
    desc.qtable_sz[1] = p->media.j.ctbl_siz;
	desc.es_vid = p->stream_id; // VIDEO BASE

	Stream_release_set_count(p, (2 + 1));
	uniq_id.word = (u32)p;
	NSec_set_jpeg2_Tx_Descriptor(desc, uniq_id);

	return 0;
}
int jpegbymeta(const struct cif_stream_send* p, void *d)
{
	struct ts_descriptor_info desc;
	U_NETSEC_DESC_ID uniq_id;

	if(gRTSP_func) {
	//RTSP valid
		if(CheckCanbesend(p, p->stream_id) !=1) {
			return 0;
		}
	}
	desc.nvcl = 0;
	desc.fd = netsec_fd;
	desc.paddr  = p->sample_address;
	desc.len = p->sample_size;
	if(gRTSP_func) {
	//RTSP valid
	#ifdef TEST_AUDIO_VIDEO
		desc.ts = pass_time(100,9);
	#else
		desc.ts = p->media.j.sample_pts[0];
	#endif //TEST_AUDIO_VIDEO
		SendRTCP(p->stream_id,desc.ts);
	}else{
		desc.ts = p->media.j.sample_pts[0];
	}
	desc.es_vid = p->stream_id; // VIDEO BASE
	uniq_id.word = (u32)p;
	NSec_set_meta_Tx_Descriptor(desc,uniq_id);

	return 0;
}
/////////////////////////////////////
//Call back function called while a 
//audio stream was sent from RTOS.
//Parameter[in]  p: stream information.
/////////////////////////////////////
int audio_cb(const struct cif_stream_send* p, void *d)
{
	struct ts_descriptor_info desc;
	unsigned int *vaddr;
	unsigned short size;
	unsigned char ffcnt = 0;
	U_NETSEC_DESC_ID uniq_id;

	if(gRTSP_func) {
	//RTSP valid
		if(CheckCanbesend(p, AUDIO_BASE) !=1) {
			return 0;
		}
	}

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
//	printf("%s %s[%d],id:%u,time_stamp:%u\n", __FILE__,__func__,__LINE__,p->stream_id,p->media.a.time_stamp[0]);
//	desc.ts = pass_time(1000,8);
	if(gRTSP_func) {
	//RTSP valid
	#ifdef TEST_AUDIO_VIDEO
		desc.ts = 0;
	//	desc.ts = pass_time(1000,8);
	#else
		desc.ts = p->media.a.time_stamp[0]*((float)((float)8/(float)90));
	//	desc.ts = p->media.a.time_stamp[0];
	#endif //TEST_AUDIO_VIDEO
		SendRTCP(AUDIO_BASE,desc.ts);
	}else{
		desc.ts = p->media.a.time_stamp[0]*((float)((float)8/(float)90));
	}
	desc.fd = netsec_fd;
	desc.paddr  = p->sample_address - 1 - ffcnt -7;
	desc.len = p->sample_size + 7 + 1 + ffcnt;
	desc.es_vid = AUDIO_BASE;
	uniq_id.word = (u32)p;
	NSec_set_audio_Tx_Descriptor(desc, uniq_id);
	
//	printf("%s[%d]\n", __func__,__LINE__);

	return 0;
}

int metacb(const cif_stream_meta_send* p, void *d)
{
	struct ts_descriptor_info desc;
	int pa;
	unsigned long len;
	U_NETSEC_DESC_ID uniq_id;

	if(gRTSP_func) {
	    if(CheckCanbesend(p, p->stream_id + META_BASE) !=1) {
			return 0;
		}
	}
#ifdef CO_TEST_META_DATA_SAVE_TO_SD
	{
		char string[256];
		static unsigned int file_no=1;
		sprintf(string,"/run/SD/mmcblk0p1/mmiotool -R 0x%x 0x%x /run/SD/mmcblk0p1/meta/%08x.%08x\n",
			p->sample_address,
			p->sample_size,
			file_no,
			p->sample_address);
		system(string);
		file_no++;
	}
#endif //CO_TEST_META_DATA_SAVE_TO_SD
	pa = p->sample_address;
	len = p->sample_size;
	desc.es_vid = p->stream_id + META_BASE;
	desc.fd = netsec_fd;
	desc.paddr = pa;
	desc.len = len;
	if(gRTSP_func) {
	//RTSP valid
	//Check Send RTCP
#ifdef TEST_AUDIO_VIDEO
		desc.ts = pass_time(100,9);
#else
		desc.ts = p->sample_pts[0];
#endif//TEST_AUDIO_VIDEO
		SendRTCP(p->stream_id + META_BASE,desc.ts);
	}
	else{
		desc.ts = p->sample_pts[0];
	}
	uniq_id.word = (u32)p;
	NSec_set_meta_Tx_Descriptor(desc, uniq_id);

	return 0;
}


/* ------------------------
 Socket
--------------------------*/

int streaming_sock_initalize(int* sfd)
{
	struct sockaddr_un saddr;
	int len;
	int ret;

	ret = -1;
	if ((*sfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		printf("%s[%d] %03d : %s\n",__func__,__LINE__, errno, strerror(errno));
		ret = -1;
	}
	else
	{
		memset(&saddr, 0x00, sizeof(saddr));
		saddr.sun_family = PF_UNIX;
		strcpy(saddr.sun_path, SOCK_NAME);
		unlink(SOCK_NAME);
		len = sizeof(saddr.sun_family) + strlen(SOCK_NAME);
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
				printf("Streamd ready\n");
				ret = 0;
			}
		}
	}
	return ret;
}
int streaming_sock_receive(int* sfd, int tout_ms )
{
	struct sockaddr_un caddr;
	int cfd;
	int ret=0;
	int len;
	char buf[1024];
	struct pollfd pfd[1] = {0,};

	pfd[0].fd = *sfd;
	pfd[0].events = POLLIN;

	poll(pfd, 1, tout_ms);
	if( pfd[0].revents & POLLIN )
	{
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
				// start and stop
				add_stream(buf);
				//fprintf( stderr, "read: %s\n", buf );
				ret = 0;
			}
			else
			{
				ret = -1;
			}
			close(cfd);
		}
	}

	return ret;
}
int streaming_sock_terminate(int* sfd)
{
	close(*sfd);
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
        ostr[cnt] = tok;
        tok = strtok(NULL,",");
        cnt++;
    }
    return cnt;
}
/* --------------------------
 Add Streaming to NETSEC
 receiving foramt: 
  <MAC>,<IP>,<PORT>,<format>,<PT>,<yuv width>,<stream ID from 0 to 31>
00-12-34-56-78-9A,192.168.10.7,5004,v,96,0,0
00-12-34-56-78-9A,192.168.10.7,6004,v,96,0,1
00-12-34-56-78-9A,192.168.10.7,7004,v,96,0,2
00-12-34-56-78-9A,192.168.10.7,8004,v,96,0,3
00:12:34:56:78:9A,192.168.10.7,5100,a,96,0,31
00:12:34:56:78:9A,192.168.10.7,7100,m,112,0,0

----------------------------*/
int add_stream(char* buf)
{
	char *p[8];
	time_t ntptm;
	struct session_db_info sess_info;
	int ret;
	unsigned long w;
	unsigned long h;

	optstrtok(p,buf);
//	printf("%s \n",p[0]);
//	printf("%s \n",p[1]);
//	printf("%s \n",p[2]);
//	printf("%s \n",p[3]);
//	printf("%s \n",p[4]);
//	printf("%s \n",p[5]);
//	printf("%s \n",p[6]);
	strcpy(param.mac, p[0]);
	strcpy(param.ip, p[1]);
	param.port = atoi(p[2]);
	param.fmt = (char)*p[3];
	param.pt = atoi(p[4]);
	w = atoi(p[5]);
	param.yuv_width = w;
	h = atoi(p[6]);
	param.sess_id = atoi(p[7]);

	if( netsec_fd != 0 )
	{
		sess_info.fd = netsec_fd;
		if(gRandamNtp == 0){
			ntptm = time(NULL) + 2208988800;
			sess_info.ntp = gRandamNtp
				=(unsigned long)ntptm;
		}
		else{
			sess_info.ntp = gRandamNtp;
		}

		sess_info.sess_db_id = param.sess_id + 32; 
		sess_info.str_dst_ip = (char*)param.ip;
		sess_info.str_dst_mac =(char*)param.mac;
		sess_info.src_port = param.port;
		sess_info.dst_port = param.port;
		sess_info.rtp_payload_type = param.pt;
		sess_info.yuv_pix_num = param.yuv_width;
		sess_info.ssrc = 0x12345678 + param.sess_id;
		sess_info.es_vid = param.sess_id;
		sess_info.sessdb08 = 0;
		sess_info.sessdb09 = 0;
		sess_info.sessdb10 = 0;
		sess_info.sessdb11 = 0;
		ret = 0;
		if( param.fmt == 'a' )
		{
			sess_info.yuv_pix_num = 0;
			printf(" -- Audio -- \n");
		}
		else if( param.fmt == 'm' )
		{
			sess_info.yuv_pix_num = 0;
			printf(" -- META -- \n");
		}
		else if( param.fmt == 'J' )
		{
			sess_info.yuv_pix_num = 0;
			printf(" -- JPEG over 2048 -- \n");
		}
		else if( param.fmt == 'j' )
		{
			// jpeg
			printf(" -- JPEG -- \n");
			sess_info.yuv_pix_num = 0;
			h = h / 8;
			w = w / 8;
			sess_info.sessdb08  = 0x01000000 & 0xFF000000;
			sess_info.sessdb08 |= 0x00FF0000 & 0x00FF0000;//Q
			sess_info.sessdb08 |= (w <<  8)  & 0x0000FF00;//width/8. max 8*255=2040
			sess_info.sessdb08 |= (h <<  0)  & 0x000000FF;//height/8. max 8*255=2040
			sess_info.sessdb09  = 0x00000000 & 0x0000FFFF;//restart interval
			sess_info.sessdb09 |= 0x00003000 & 0xFFFF3FFF;
			sess_info.sessdb09 |= 0x00003FFF & 0xFFFFC000;//restart count
			sess_info.sessdb10  = 0x00000000 & 0x00FFFFFF;//MBZ
			sess_info.sessdb10 |= 0x00000000 & 0xFF00FFFF;//precision					jpeg_num++;
		}
		else if( param.fmt == 'y' )
		{
			printf(" -- YUV -- \n");
		}
		else
		{
			printf(" -- H264/HEVC -- \n");
			sess_info.yuv_pix_num = 0;
		}
		NSec_set_sessiondb(sess_info);
		printf("   IP   %s\n", sess_info.str_dst_ip);
		printf("   MAC  %s\n", sess_info.str_dst_mac);
		printf("   VID  %d\n", sess_info.es_vid);
		printf("   Port %d\n", sess_info.dst_port);
		printf("   type %d\n", sess_info.rtp_payload_type);
		printf("   width %d\n",param.yuv_width);
	}
	else
	{
		printf("%s[%d] %03d : %s\n",__func__,__LINE__, -1, "NETSEC error");
		ret = -1;
	}
	return ret;
}
int add_all(char* ip, char *mac)
{
	char string[256];

	// H264 from 0 to 7 
	sprintf(string, "%s,%s,5004,v,96,0,0,%d", mac, ip, VIDEO_BASE);
	add_stream(string);
	sprintf(string, "%s,%s,5014,v,96,0,0,%d", mac, ip, VIDEO_BASE+1);
	add_stream(string);
	sprintf(string, "%s,%s,5024,v,96,0,0,%d", mac, ip, VIDEO_BASE+2);
	add_stream(string);
	sprintf(string, "%s,%s,5034,v,96,0,0,%d", mac, ip, VIDEO_BASE+3);
	add_stream(string);
	sprintf(string, "%s,%s,5044,v,96,0,0,%d", mac, ip, VIDEO_BASE+4);
	add_stream(string);
	sprintf(string, "%s,%s,5054,v,96,0,0,%d", mac, ip, VIDEO_BASE+5);
	add_stream(string);
	sprintf(string, "%s,%s,5064,v,96,0,0,%d", mac, ip, VIDEO_BASE+6);
	add_stream(string);
	sprintf(string, "%s,%s,5074,v,96,0,0,%d", mac, ip, VIDEO_BASE+7);
	add_stream(string);
	// HEVC from 8 to 15
	sprintf(string, "%s,%s,6004,v,96,0,0,%d", mac, ip, VIDEO_BASE+8);
	add_stream(string);                              
	sprintf(string, "%s,%s,6014,v,96,0,0,%d", mac, ip, VIDEO_BASE+9);
	add_stream(string);                              
	sprintf(string, "%s,%s,6024,v,96,0,0,%d", mac, ip, VIDEO_BASE+10);
	add_stream(string);                              
	sprintf(string, "%s,%s,6034,v,96,0,0,%d", mac, ip, VIDEO_BASE+11);
	add_stream(string);                              
	sprintf(string, "%s,%s,6044,v,96,0,0,%d", mac, ip, VIDEO_BASE+12);
	add_stream(string);                              
	sprintf(string, "%s,%s,6054,v,96,0,0,%d", mac, ip, VIDEO_BASE+13);
	add_stream(string);                              
	sprintf(string, "%s,%s,6064,v,96,0,0,%d", mac, ip, VIDEO_BASE+14);
	add_stream(string);                              
	sprintf(string, "%s,%s,6074,v,96,0,0,%d", mac, ip, VIDEO_BASE+15);
	add_stream(string);
	// JPEG from 16 to 19
#ifdef	RFC2435
	sprintf(string, "%s,%s,7004,j,26,1280,720,%d", mac, ip, VIDEO_BASE+16);
#else
	sprintf(string, "%s,%s,7004,J,96,1280,720,%d", mac, ip, VIDEO_BASE+16);
#endif
	add_stream(string);
#ifdef	RFC2435
	sprintf(string, "%s,%s,7014,j,26,1280,720,%d", mac, ip, VIDEO_BASE+17);
#else
	sprintf(string, "%s,%s,7014,J,96,1280,720,%d", mac, ip, VIDEO_BASE+17);
#endif
	add_stream(string);
#ifdef	RFC2435
	sprintf(string, "%s,%s,7024,j,26,1280,720,%d", mac, ip, VIDEO_BASE+18);
#else
	sprintf(string, "%s,%s,7024,J,96,1280,720,%d", mac, ip, VIDEO_BASE+18);
#endif
	add_stream(string);
#ifdef	RFC2435
	sprintf(string, "%s,%s,7034,j,26,1280,720,%d", mac, ip, VIDEO_BASE+19);
#else
	sprintf(string, "%s,%s,7034,J,96,1280,720,%d", mac, ip, VIDEO_BASE+19);
#endif
	add_stream(string);
	// YUV from 20 to 23
	sprintf(string, "%s,%s,8004,y,96,1000,750,%d", mac, ip, YUV_BASE);
	add_stream(string);
	sprintf(string, "%s,%s,8014,y,96,1000,750,%d", mac, ip, YUV_BASE+1);
	add_stream(string);
	sprintf(string, "%s,%s,8024,y,96,1000,750,%d", mac, ip, YUV_BASE+2);
	add_stream(string);
	sprintf(string, "%s,%s,8034,y,96,1000,750,%d", mac, ip, YUV_BASE+3);
	add_stream(string);
	// Meta from 24 to 27
	sprintf(string, "%s,%s,9004,m,96,0,0,%d", mac, ip, META_BASE);
	add_stream(string);
	sprintf(string, "%s,%s,9014,m,96,0,0,%d", mac, ip, META_BASE+1);
	add_stream(string);
	sprintf(string, "%s,%s,9024,m,96,0,0,%d", mac, ip, META_BASE+2);
	add_stream(string);
	sprintf(string, "%s,%s,9034,m,96,0,0,%d", mac, ip, META_BASE+3);
	add_stream(string);
	// Audio from 28 to 31
	sprintf(string, "%s,%s,10004,a,96,0,0,%d", mac, ip, AUDIO_BASE);
	add_stream(string);
	sprintf(string, "%s,%s,10014,a,96,0,0,%d", mac, ip, AUDIO_BASE+1);
	add_stream(string);
	sprintf(string, "%s,%s,10024,a,96,0,0,%d", mac, ip, AUDIO_BASE+2);
	add_stream(string);
	return 0;
}
int add_g_stream(char* ip, char *mac)
{
	char string[256];

	// HEVC from 8 to 16
	sprintf(string, "%s,%s,5004,v,96,0,0,4", mac, ip);
	add_stream(string);
	sprintf(string, "%s,%s,6004,v,96,0,0,5", mac, ip);
	add_stream(string);
	// YUV from 17 to 20
	sprintf(string, "%s,%s,7004,y,96,1000,750,%d", mac, ip, YUV_BASE);
	add_stream(string);
	sprintf(string, "%s,%s,8004,y,96,1000,750,%d", mac, ip, YUV_BASE+1);
	add_stream(string);
	sprintf(string, "%s,%s,9004,m,96,0,0,%d", mac, ip, META_BASE);
	add_stream(string);
	sprintf(string, "%s,%s,9104,m,96,0,0,%d", mac, ip, META_BASE+1);
	add_stream(string);
	return 0;
}
int del_stream(unsigned int id)
{
	unsigned int i;
	for( i = 0; i <= STREMD_STRUCT_MAX; i++ )
	{
		if( i == id )
		{
		    memset( &dparam[i], 0, sizeof(struct streamd) );
		}
		else
		{
			// non
		}
	}
	return 0;
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
/////////////////////////////////////
//Get stream information sending to client.
//Parameter[out]  request_stream: stream information.
//Return a size of data to send to RTSP server
/////////////////////////////////////
static int set_stream_infor(T_RES_STREAM_INFOR* request_stream)
{

	request_stream->request_id = E_NETSEC_APL_ID_RES_GET_STREAM;

	request_stream->stream_num = gValid_num;
	memcpy(request_stream->stream_infor,gStreamInformation,sizeof(T_STREAM_INFOR)*gValid_num);
	return (sizeof(request_stream->request_id) + 
		sizeof(request_stream->stream_num) +
		sizeof(T_STREAM_INFOR) * gValid_num);
}
/////////////////////////////////////
//Get a MAC address of client.
//Parameter[in]  ip: Client's IP address.
//Parameter[out]  mac_address: Client's MAC address.
/////////////////////////////////////
static void get_mac_from_ip(const char* ip, char mac_address[64] )
{
	const char * tmpFile = "/tmp/get_client_mac_aadress.tmp";
	char comand[512];
	FILE *file;
	int j=0,k=0;
	int length_mac;
	char sigle_mac[6][4];

	strcpy(comand,"arping  -c 1 ");
	strcat(comand,ip);
	strcat(comand," | grep  \"reply from\" |cut -d \" \" -f 5 > ");
	strcat(comand,tmpFile);
	system(comand);

	file = fopen(tmpFile, "r");
	fgets(comand, 512, file);
	fclose(file);
//	printf("mac_address:%s,len:%d\n",comand,strlen(comand));
	memset(sigle_mac,0,sizeof(sigle_mac));
	length_mac =strlen(comand)-2;
	for(int i=1;i<length_mac;i++) { //Begining is "[", last is "]" and return code
		sigle_mac[j][k++]=comand[i];
		if(comand[i] ==':'){
			j++;
			k=0;
		}
	}
	k=0;
	for(j=0;j<6;j++){
//	printf("sigle_mac[%d]:%s,len:%d\n",j,sigle_mac[j],strlen(sigle_mac[j]));
		if(strlen(sigle_mac[j])<3 && j<5) {
			mac_address[k++]='0';
			mac_address[k++]=sigle_mac[j][0];
			mac_address[k++]=sigle_mac[j][1];
		}
		else{
			mac_address[k++]=sigle_mac[j][0];
			mac_address[k++]=sigle_mac[j][1];
			mac_address[k++]=sigle_mac[j][2];
		}
	}
	mac_address[k]=0;
}
/////////////////////////////////////
//Remove a stream to from sending list.
//Parameter[in]  stream_id: Removing stream ID(Serial number).
//Return: 0 mean normal.
//		D_ERROR_STREAM_NO_EXIST: Stream not exist.
/////////////////////////////////////
static int remove_stream(unsigned int stream_id)
{
	int ret= D_ERROR_STREAM_NO_EXIST;

	gRandamNtp = 0;//Reset randam ntp
	if(stream_id<(unsigned int)gValid_num) {
		unsigned int encode_type = gStreamInformation[stream_id].encode_type;
		gMangeStreamInformation[stream_id].Can_be_send_flag = 0;//Clear added flag
		if(gStream_count[encode_type-1] >0){
			gStream_count[encode_type-1]--;//Stream type count
			ret=0;
		}
		else{
			printf("Error!!,gStream_count[%d]=%d\n",encode_type-1,gStream_count[encode_type-1]);
		}
	}
	return ret;
}
/////////////////////////////////////
//Add a stream to NETSEC.
//Parameter[in]  addStream: Stream added information.
//Parameter[in]  mac_address: Client side MAC address.
//Return: 0 mean normal.
//		D_ERROR_STREAM_TYPE: Stream encode type error.It should be [0,31]
//		D_ERROR_NETSEC_DEVICE: Netsec device is not openned.
/////////////////////////////////////
static int set_add_stream(T_ADD_STREAM* addStream, char* mac_address)
{
	int ret = 0;
	int ssid;
	unsigned int encode_type = gStreamInformation[addStream->stream_id].encode_type;
	struct session_db_info sess_info;
#ifndef CO_USE_FRAME_RATE_COUNTER
	gMangeStreamInformation[addStream->stream_id].rtcp_frame_counter =0;
#endif //CO_USE_FRAME_RATE_COUNTER
	//Clear rtcp's block size
	gMangeStreamInformation[addStream->stream_id].send_rtcp.ReportBlocknum =
	gMangeStreamInformation[addStream->stream_id].send_rtcp.ReportBlockSize = 0;
	sess_info.yuv_pix_num = 0;
	sess_info.sessdb08 = 0;
	sess_info.sessdb09 = 0;
	sess_info.sessdb10 = 0;
	sess_info.sessdb11 = 0;
	switch(encode_type) {
	case E_ENCODE_TYPE_H264:
	case E_ENCODE_TYPE_HEVC:
#ifdef CO_USE_FRAME_RATE_COUNTER	
		if(gStreamInformation[addStream->stream_id].frame_rate<=15) {
			//GOP is 15
			gMangeStreamInformation[addStream->stream_id].send_rtcp_interval
				= 1000;//1Sec
		}
		else {
			//GOP = frame/15
			gMangeStreamInformation[addStream->stream_id].send_rtcp_interval
				= (1000*15)/gStreamInformation[addStream->stream_id].frame_rate;
		}
#else //CO_USE_FRAME_RATE_COUNTER
		gMangeStreamInformation[addStream->stream_id].send_rtcp_interval
			=D_SEND_RTCP_INTERVAL_COUNTER_VIDEO;
#endif //CO_USE_FRAME_RATE_COUNTER

		if(encode_type == E_ENCODE_TYPE_H264) {
			ssid = H264_VIDEO_BASE;
		} else {
			ssid = HEVC_VIDEO_BASE;
		}
		break;
	case E_ENCODE_TYPE_MJPEG:
		ssid = JPEG_BASE;
		sess_info.sessdb08  = 0x01000000;
		sess_info.sessdb08 |= 0x00FF0000;//Q
		if(gStreamInformation[addStream->stream_id].width<2048 &&
			gStreamInformation[addStream->stream_id].height<2048) {
			unsigned short h = gStreamInformation[addStream->stream_id].height / 8;
			unsigned short w = gStreamInformation[addStream->stream_id].width / 8;
			sess_info.sessdb08 |= ((w << 8)  & 0x0000FF00);//width/8. max 8*255=2040
			sess_info.sessdb08 |= ((h <<  0)  & 0x000000FF);//height/8. max 8*255=2040
			printf(" -- JPEG -- \n");
		}
		else{
			printf(" -- JPEG over 2048 -- \n");
		}
		sess_info.sessdb09  = (0x00000000 & 0x0000FFFF);//restart interval
		sess_info.sessdb09 |= (0x00003000 & 0xFFFF3FFF);
		sess_info.sessdb09 |= (0x00003FFF & 0xFFFFC000);//restart count
		sess_info.sessdb10  = (0x00000000 & 0x00FFFFFF);//MBZ
		sess_info.sessdb10 |= (0x00000000 & 0xFF00FFFF);//precision	jpeg_num++;
#ifdef CO_USE_FRAME_RATE_COUNTER
		gMangeStreamInformation[addStream->stream_id].send_rtcp_interval
				= 2*1000/(gStreamInformation[addStream->stream_id].frame_rate);
#else //CO_USE_FRAME_RATE_COUNTER
		gMangeStreamInformation[addStream->stream_id].send_rtcp_interval
			=D_SEND_RTCP_INTERVAL_COUNTER_JPEG;
#endif //CO_USE_FRAME_RATE_COUNTER
		break;
	case E_ENCODE_TYPE_ACC:
		ssid = AUDIO_BASE;
#ifdef CO_USE_FRAME_RATE_COUNTER
		gMangeStreamInformation[addStream->stream_id].send_rtcp_interval
				= MC_RTCP_PERIOD;
#else //CO_USE_FRAME_RATE_COUNTER
		gMangeStreamInformation[addStream->stream_id].send_rtcp_interval
			=D_SEND_RTCP_INTERVAL_COUNTER_AUDIO;
#endif //CO_USE_FRAME_RATE_COUNTER
		printf(" -- Audio -- \n");
		break;
	case E_ENCODE_TYPE_META:
		ssid = META_BASE;
		printf(" -- META -- \n");
		break;
	case E_ENCODE_TYPE_YUV:
		ssid = YUV_BASE;
		sess_info.yuv_pix_num = gStreamInformation[addStream->stream_id].width;
		printf(" -- YUV -- \n");
		break;
	default:
		ret = D_ERROR_STREAM_TYPE;
		break;
	}
	if(ret == 0) {
		time_t ntptm;
		ssid +=gStream_count[encode_type-1];
		if( netsec_fd != 0 )
		{
			sess_info.fd = netsec_fd;
			if(gRandamNtp == 0){
				ntptm = time(NULL) + 2208988800;
				sess_info.ntp = gRandamNtp
					=(unsigned long)ntptm;
			}
			else{
				sess_info.ntp = gRandamNtp;
			}

			sess_info.sess_db_id = ssid + 32; 
			sess_info.str_dst_ip = addStream->des_ip;
			sess_info.str_dst_mac = mac_address;
			sess_info.src_port = addStream->src_port[0];
			sess_info.dst_port = addStream->des_port[0];
			//Set RTCP's information to receive RR
			gMangeStreamInformation[addStream->stream_id].rtcpiformation.src_rtcp_port=
				addStream->src_port[1];
			strncpy(gMangeStreamInformation[addStream->stream_id].rtcpiformation.des_ip,
					addStream->des_ip,
				sizeof(gMangeStreamInformation[addStream->stream_id].rtcpiformation.des_ip));
			sess_info.rtp_payload_type = 
				gStreamInformation[addStream->stream_id].payload_id;
			sess_info.ssrc = 0x12345678 + ssid;
			sess_info.es_vid = ssid;
			NSec_set_sessiondb(sess_info);
			printf("   IP   %s\n", sess_info.str_dst_ip);
			printf("   MAC  %s\n", sess_info.str_dst_mac);
			printf("   VID  %d\n", sess_info.es_vid);
			printf("   Port %d\n", sess_info.dst_port);
			printf("   type %d\n", sess_info.rtp_payload_type);
			gStream_count[encode_type-1]++;//Stream type count
			gMangeStreamInformation[addStream->stream_id].Can_be_send_flag = 1;//Set added flag
			gMangeStreamInformation[addStream->stream_id].
				send_rtcp.pkt_cnt = 0;//set sent packet num to 0
			gMangeStreamInformation[addStream->stream_id].
				send_rtcp.pkt_octet = 0;//set sent packet num to 0
		
#ifdef CO_USE_FRAME_RATE_COUNTER
			clock_gettime(CLOCK_MONOTONIC,	//Get start time
			 &gMangeStreamInformation[addStream->stream_id].StartTimeTicket);
#endif//CO_USE_FRAME_RATE_COUNTER
			gMangeStreamInformation[addStream->stream_id].send_rtcp.ssrc
			 =sess_info.ssrc;
			gMangeStreamInformation[
				addStream->stream_id].send_rtcp.ntp = sess_info.ntp;
		}
		else
		{
			printf("%s[%d] %03d : %s\n",__func__,__LINE__, -1, "NETSEC error");
			ret = D_ERROR_NETSEC_DEVICE;
		}
	}
	return ret;
}
/////////////////////////////////////
//A thread received a request to get stream 
//information from RTSP server process.
/////////////////////////////////////
static void* stream_infor_Thread( void* arg )
{
	struct sockaddr_un saddr,dstAddr;
	socklen_t client_name_len;
    int numrcv;
	int receive_buffer_counter;
	unsigned int receive_buffer[D_BUFFER_SIZE];
	char* buffer=(char*)receive_buffer;
	int IsContinue=1;
	int soc,client_socket_fd;

	unlink(RTSP_INFO_SOCK_NAME);
	soc = socket( PF_UNIX, SOCK_STREAM, 0 );
	if(soc < 0 ) {
		printf ("Can't open stream socket");
	}
	else {
		memset(&saddr, 0, sizeof( saddr ) );
		saddr.sun_family = PF_UNIX;
		strcpy( saddr.sun_path, RTSP_INFO_SOCK_NAME );
	/* Set bind for docket */
		bind(soc, (struct sockaddr *) &saddr, sizeof(saddr));

	  /* Permit connect */
		listen(soc, 1);

	  /* Receipt connectiont */
		printf("Waiting for connection with RTSP infor...\n");
		while(IsContinue) {
	  /* Receive socket */
			int send_data_size;
			memset((char*)receive_buffer,0,sizeof(receive_buffer));
			client_socket_fd = accept(soc,
				 (struct sockaddr *) &dstAddr,&client_name_len);
			printf("Connected from RTSP %s \n", dstAddr.sun_path);
			receive_buffer_counter=0;
			
//			do {
//				printf("buffer:[%d] D_BUFFER_SIZE:%d,T_ADD_STREAM:%d\n", 
//					receive_buffer_counter,sizeof(receive_buffer),sizeof(T_RES_STREAM_INFOR));

				numrcv = recv(client_socket_fd, 
					&buffer[receive_buffer_counter], 
					sizeof(T_RES_STREAM_INFOR), 0); 

				receive_buffer_counter +=numrcv;
//				printf("buffer:[%d] D_BUFFER_SIZE:%d,T_RES_STREAM_INFOR:%d\n", 
//					receive_buffer_counter,sizeof(receive_buffer),sizeof(T_RES_STREAM_INFOR));

//			}while(numrcv>0 && receive_buffer_counter<sizeof(T_RES_STREAM_INFOR));
			switch(receive_buffer[0]) {
				case E_NETSEC_APL_ID_GET_STREAM:{
					T_RES_STREAM_INFOR request_stream;
					send_data_size = set_stream_infor(&request_stream);
					request_stream.request_id = E_NETSEC_APL_ID_RES_GET_STREAM;
//				printf("%s %s %d %u\n", __FILE__,__func__,__LINE__,send_data_size);
					send(client_socket_fd, 
						(unsigned char*)(&request_stream), 
						send_data_size, 0);
					break;
				}
				case E_NETSEC_APL_ID_END:
					IsContinue =0;
					break;
				default:{
					printf("receive a unkonw message %08x\n", 
				receive_buffer[0]);
					break;
				}
			}
			close(client_socket_fd);
		}
		close(soc);
	}
//	printf("%s %s %d\n",__FILE__,__func__,__LINE__);
	return 0;
}
/////////////////////////////////////
//Receive a request to set stream 
//information by the thread named stream_Set_Thread
//Parameter[in]:receive_data Received data.
//Return: 0 mean normal
/////////////////////////////////////
static int ProcessStreamSeting(const T_SET_STREAM_INFORMATION* receive_data)
{
	if(gRTSP_func) {
		int stream_index =
			GetStreamIndexFromStreamTypeId(receive_data->stream_id);
		if(stream_index<0) {
			printf("No this stream to set or print\n");
		}
		else {
			switch(receive_data->request_type){
			case E_REQUEST_TYPE_SET:
				if(receive_data->stream_id>=VIDEO_BASE && 
					receive_data->stream_id<JPEG_BASE) {
				//H264/365
						gStreamInformation[stream_index].frame_rate =
							receive_data->stream_frame_rate;
				}
				else if(receive_data->stream_id>=JPEG_BASE && 
					receive_data->stream_id<META_BASE) {
				//YUV or Jpeg
					gStreamInformation[stream_index].frame_rate =
					receive_data->stream_frame_rate;
					gStreamInformation[stream_index].width =
						receive_data->image_width;
					gStreamInformation[stream_index].height =
						receive_data->image_height;
				}
				else if(receive_data->stream_id>=META_BASE && 
					receive_data->stream_id<32) {
					//Do nothing
				}
				else {
					//Do nothing
				}
				printf("id:%d,rate:%d,w:%d,h:%d\n",
					receive_data->stream_id,
					receive_data->stream_frame_rate,
					receive_data->image_width,
					receive_data->image_height);
				break;
			default:
				printf("Current setting[%u]:rate:%u,w:%u,h:%u\n",
					receive_data->stream_id,
					gStreamInformation[stream_index].frame_rate,
					gStreamInformation[stream_index].width,
					gStreamInformation[stream_index].height);
				break;
			}
		}
	}
	else {
		char string[256];
		if(receive_data->stream_id>=YUV_BASE && 
			receive_data->stream_id<META_BASE) {

			sprintf(string, "%s,%s,%u,y,96,%u,%u,%d", 
					gMacAddress,
					gIpAddress,
				D_YUV_PORT_BASE + (receive_data->stream_id-YUV_BASE)*10,
				receive_data->image_width,
				receive_data->image_height,
				receive_data->stream_id);
			add_stream(string);
		}
		else if(receive_data->stream_id>=JPEG_BASE && 
					receive_data->stream_id<YUV_BASE) {
#ifdef	RFC2435
			char* const JpegType="j";
#else
			char* const JpegType="J";
#endif //RFC2435
			unsigned int w= receive_data->image_width;
			unsigned int h= receive_data->image_height;
			if((receive_data->image_width >2040 ) || (receive_data->image_height>2040)) {
				w=h=0;
			}
			sprintf(string, "%s,%s,%u,%s,96,%u,%u,%d",
					gMacAddress,
					gIpAddress,
				D_JPEG_PORT_BASE + (receive_data->stream_id-JPEG_BASE)*10,
				JpegType,
				w,
				h,
				receive_data->stream_id);
			add_stream(string);
		}
		else{
			printf("No this stream to set or print\n");
		}
	}
	return 0;
}
/////////////////////////////////////
//A thread received setting for the 
//stream from set_streamd process.
/////////////////////////////////////
static void* stream_Set_Thread(void* arg)
{
	struct sockaddr_un saddr,dstAddr;
	socklen_t client_name_len;
	T_SET_STREAM_INFORMATION receive_data;
	char* buffer=(char*)(&receive_data);
	int IsContinue=1;
	int soc,client_socket_fd;

	unlink(SET_STREAM_INFOR);
	soc = socket( PF_UNIX, SOCK_STREAM, 0 );
	if(soc < 0 ) {
		printf ("Can't open stream socket");
	}
	else {
		memset(&saddr, 0, sizeof( saddr ) );
		saddr.sun_family = PF_UNIX;
		strcpy( saddr.sun_path, SET_STREAM_INFOR );
		bind(soc, (struct sockaddr *) &saddr, sizeof(saddr));

		listen(soc, 1);
		while(IsContinue) {
			memset(buffer,0,sizeof(receive_data));
			client_socket_fd = accept(soc,
				 (struct sockaddr *) &dstAddr,&client_name_len);

			recv(client_socket_fd, 
					buffer, 
					sizeof(receive_data), 0); 
			if(receive_data.request_type == E_REQUEST_TYPE_END) {
				//End receive 
				IsContinue =0;
			}
			else{
				ProcessStreamSeting(&receive_data);
			}
			close(client_socket_fd);
		}
		close(soc);
	}
	return 0;
}
/////////////////////////////////////
//Add RTCP sending to all streams.
/////////////////////////////////////
static int start_sending()
{
	int ret=0;
	if(gRTSP_func >= E_RTSP_FUNCTION_RTSP_RTCP) {
		int i;
		T_RTCP_RECEIV_INFOR rtcp_rr[D_MAX_STREAM_NUM];
		for( i=0;i<gValid_num;i++) {
			if(gMangeStreamInformation[i].Can_be_send_flag) {
				rtcp_rr[i].src_rtcp_port =
					gMangeStreamInformation[i].rtcpiformation.src_rtcp_port;
				strncpy(rtcp_rr[i].des_ip,gMangeStreamInformation[i].rtcpiformation.des_ip,
						sizeof(rtcp_rr[i].des_ip));
			}
		}
		ret = Add_rtcp_receiver(rtcp_rr,i);
	}
	return ret;
}
#if (CO_FORCE_SEND_DATA != 0)
static int ForceAddStream(T_ADD_STREAM addMetaDataStream, char mac_address[264])
{
#if (CO_FORCE_SEND_DATA == 2)
	E_ENCODE_TYPE encode_type[CO_FORCE_SEND_DATA] = {
		E_ENCODE_TYPE_YUV,
		E_ENCODE_TYPE_META,
	};
	unsigned int src_port[CO_FORCE_SEND_DATA] = {
		D_TEST_SEDN_YUV_PORT,
		D_TEST_SEDN_META_PORT,
	};
#elif (CO_FORCE_SEND_METER_DATA == 1)
	E_ENCODE_TYPE encode_type[CO_FORCE_SEND_DATA] = {
		E_ENCODE_TYPE_META,
	};
	unsigned int src_port[CO_FORCE_SEND_DATA] = {
		D_TEST_SEDN_META_PORT,
	};
#elif (CO_FORCE_SEND_YUV_DATA == 1)
	E_ENCODE_TYPE encode_type[CO_FORCE_SEND_DATA] = {
		E_ENCODE_TYPE_YUV,
	};
	unsigned int src_port[CO_FORCE_SEND_DATA] = {
		D_TEST_SEDN_YUV_PORT,
	};
#endif
	if(addMetaDataStream.stream_id == 0) {
		//Add meta data sending forcefully when stream is 0
		for(addMetaDataStream.stream_id=0;
			addMetaDataStream.stream_id<D_MAX_STREAM_NUM;
			addMetaDataStream.stream_id++) {
			for(int i=0;i<CO_FORCE_SEND_DATA;i++) {
				if(gStreamInformation[addMetaDataStream.stream_id].encode_type == encode_type[i]) {
					//Can send meta/YUV data
					addMetaDataStream.src_port[0] = src_port[i];
					addMetaDataStream.src_port[1] = src_port[i] +1;
					addMetaDataStream.des_port[0] = src_port[i];
					addMetaDataStream.des_port[1] =addMetaDataStream.des_port[0]+1;
					set_add_stream(&addMetaDataStream,mac_address);
					src_port[i]+=4;
				}
			}
		}
	}
	return 0;
}
#endif //CO_FORCE_SEND_METER_DATA
/////////////////////////////////////
//A thread receiving a request adding stream from 
//RTSP server process by socket communication.
/////////////////////////////////////
static void* stream_Add_Thread( void* arg )
{
	struct sockaddr_un saddr,dstAddr;
	socklen_t client_name_len;
    int numrcv;
	int receive_buffer_counter;
	unsigned int receive_buffer[D_BUFFER_SIZE];
	char* buffer=(char*)receive_buffer;
	int IsContinue=1;
	int soc,client_socket_fd;

	unlink(RTSP_ADD_SOCK_NAME);
	soc = socket( PF_UNIX, SOCK_STREAM, 0 );
	if(soc < 0 ) {
		printf ("Can't open stream socket");
	}
	else {
		memset(&saddr, 0, sizeof( saddr ) );
		saddr.sun_family = PF_UNIX;
		strcpy( saddr.sun_path, RTSP_ADD_SOCK_NAME );
		bind(soc, (struct sockaddr *) &saddr, sizeof(saddr));

		listen(soc, 1);

		printf("Waiting for connection with RTSP add stream...\n");
		while(IsContinue) {
			memset((char*)receive_buffer,0,sizeof(receive_buffer));
			client_socket_fd = accept(soc,
				 (struct sockaddr *) &dstAddr,&client_name_len);
			printf("Connected from RTSP %s \n", dstAddr.sun_path);
			receive_buffer_counter=0;
			
//			do {
//				printf("buffer:[%d] D_BUFFER_SIZE:%d,T_ADD_STREAM:%d\n", 
//					receive_buffer_counter,sizeof(receive_buffer),sizeof(T_ADD_STREAM));

				numrcv = recv(client_socket_fd, 
					&buffer[receive_buffer_counter], 
					sizeof(T_ADD_STREAM), 0); 

				receive_buffer_counter +=numrcv;
//				printf("buffer:[%d] D_BUFFER_SIZE:%d,T_ADD_STREAM:%d\n", 
//					receive_buffer_counter,sizeof(receive_buffer),sizeof(T_ADD_STREAM));

//			}while(numrcv>0 && receive_buffer_counter<sizeof(T_ADD_STREAM));
			switch(receive_buffer[0]) {
				int ret;
				case E_NETSEC_APL_ID_ADD_STREAM:
				{
					T_NETSC_APL_RESULT SendRes;
					T_ADD_STREAM* addStream =(T_ADD_STREAM*)receive_buffer;
					char mac_address[264];
					get_mac_from_ip(addStream->des_ip,mac_address);
					ret = set_add_stream(addStream,mac_address);
#if (CO_FORCE_SEND_DATA != 0)
					//Send mete data forcefully to top stream 
					ForceAddStream(*addStream,mac_address);
#endif //(CO_FORCE_SEND_DATA!=0)
					SendRes.request_id=E_NETSEC_APL_ID_RES_ADD_STREAM;
					if(ret == 0) {
						SendRes.result = E_NETSEC_APL_RESULT_OK;
					}
					else {
						SendRes.result = E_NETSEC_APL_RESULT_NG;
					}
					ret= send(client_socket_fd, (unsigned char*)(&SendRes), 
						sizeof(T_NETSC_APL_RESULT), 0);
					break;
				}
				case E_NETSEC_APL_ID_RMV_STREAM:{
					T_NETSC_APL_RESULT SendRes={
						E_NETSEC_APL_ID_RES_RMV_STREAM,0
					};
					T_REMOVE_STREAM* RemoveStream = (T_REMOVE_STREAM*)receive_buffer;
					SendRes.result =remove_stream(RemoveStream->stream_id);
					ret= send(client_socket_fd, (unsigned char*)(&SendRes), 
						sizeof(T_NETSC_APL_RESULT), 0);
					break;
				}
				case E_NETSEC_APL_ID_END:
					IsContinue =0;
					if(gRTSP_func >= E_RTSP_FUNCTION_RTSP_RTCP) {
						rtcp_end();//End RTCP receive
					}
					break;
				case E_NETSEC_APL_ID_PLAY_STREAM:
					start_sending();
					break;
				default:{
					printf("receive a unkonw message %08x\n", 
				receive_buffer[0]);
					break;
				}
			}
			close(client_socket_fd);
		}
		close(soc);
	}
	return 0;
}
/////////////////////////////////////
// Main function for delivery stream
// When the content in /tmp/use_rtsp_func.def is 2,
// RTSP and RTCP used to send stream.
// RTSP only used to stream and RTCP will be not processed if that content is 1.
// Both RTSP and RTCP will not used to send stream.
// When the content in /tmp/use_rtsp_func.def is 2 or 1, the parameter must be 
// set as stream type ID which stream would send to client side.
// 0~7:H264, 8~15:Hevc,16~19:MJPEG,20~23:YUV,24~27:Meta,28~31:Audio. 
// e.g. streamd 0 1 8 16 28 mean 2 H264, 1 HEVC, 1 MJPEG and 1 AUDIO streams will be
// sent to client.
// When the content in /tmp/use_rtsp_func.def is neither 2 or 1, the parameter must be 
// set to client's IP address and MAC address. e.g. streamd 192.168.10.7 00-12-34-56-78-9A.
////////////////////////////////////
int main(int argc, char *argv[])
{
	int sfd;
	int ret=0;
	struct ipcu_param ipcu_prm;
	struct cb_main_func func;
	struct sigaction sa;
	memset( &sa, 0, sizeof(struct sigaction) );
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = SA_NOCLDWAIT;
	sigaction( SIGCHLD, &sa, 0 );
	get_rtsp_on_off();//Get RTSP whether valid
	gDebugFunc =GetDebugFunc();//Get Debug function
	if( (gDebugFunc & E_DEBUG_FUNC_NO_DEMON) != E_DEBUG_FUNC_NO_DEMON) {
		sn_daemon(0,1);
	}
	sa.sa_handler = signal_handler;
	sa.sa_flags = SA_RESTART;
	sigaction( SIGINT, &sa, 0 );
	sa.sa_handler = sigterm_handler;
	sa.sa_flags = SA_RESTART;
	sigaction( SIGTERM, &sa, 0 );

	// register IPCU cb
	ipcu_prm.sw = 2;
	ipcu_prm.phys = 1;
	ipcu_prm.chid = 6;
	ipcu_prm.ackid = 7;
	func.video = (void*)h26x_cb;
	func.yuv = (void*)yuv_cb;
	func.audio = (void*)audio_cb;
	func.meta = (void*)metacb;
	if(gRTSP_func) {
		//RTSP valid
		func.jpeg = (void*)jpeg_cb;
	}
	else{
#ifdef	RFC2435
		func.jpeg = (void*)jpeg_cb;
#else
		func.jpeg = (void*)jpegbymeta;
#endif
	}
	func.raw = NULL;
	func.user_data = NULL;

	Stream_ipcu_ch_init(&func, ipcu_prm);

	netsec_fd = open(NETSEC_NAME, O_RDWR);
	if( netsec_fd != 0 )
	{
		// initialize
		memset( &param, 0, sizeof(param) );
		streaming_sock_initalize(&sfd);
		pthread_create(
				&gTh_reviceFromSock[E_THREAD_SET_STREAM_INFOR],
				(void*)NULL,
				stream_Set_Thread,
				(void*)0);
		//Waiting for stream sending end to release stream buffer.
		pthread_create(
				&gTh_reviceFromSock[E_THREAD_WAIT_STREAM_SENT],
				(void*)NULL,
				netsec_waitsend_Thread,
				(void*)0);

		if(gRTSP_func) {
		//RTSP valid
			//Set stream information
			if(argc - 1 >=D_MAX_STREAM_NUM) {
				printf("Parameter num must be less than 19\n");
				ret = -11;
			}
			else{
				memset(gMangeStreamInformation,0,sizeof(gMangeStreamInformation));
				for(int i=1;i<argc;i++){
					int stream_type=atoi(argv[i]);
					gMangeStreamInformation[i-1].stream_type_id=stream_type;
					gStreamInformation[i-1].stream_id = i-1;
					gStreamInformation[i-1].clock_rate = 90000; 
					gStreamInformation[i-1].profile_level_id = 640028;
					gStreamInformation[i-1].frame_rate = 30;
					gStreamInformation[i-1].media_type = D_MEDIA_TYPE_VIDEO;

					if(stream_type >=H264_VIDEO_BASE && stream_type < HEVC_VIDEO_BASE){
						//H264
						gStreamInformation[i-1].encode_type = E_ENCODE_TYPE_H264; 
						gStreamInformation[i-1].payload_id = 96; 
					} else if(stream_type >=HEVC_VIDEO_BASE && stream_type < JPEG_BASE){
						//H265
						gStreamInformation[i-1].encode_type = E_ENCODE_TYPE_HEVC; 
						gStreamInformation[i-1].payload_id = 97; 
					} else if(stream_type >=JPEG_BASE && stream_type < YUV_BASE){
						//MJPEG
						gStreamInformation[i-1].encode_type = E_ENCODE_TYPE_MJPEG; 
						gStreamInformation[i-1].width = 1280;
						gStreamInformation[i-1].height=720;
						gStreamInformation[i-1].payload_id = 26; 	
						gStreamInformation[i-1].frame_rate = 2;	
					} else if(stream_type >=YUV_BASE && stream_type < META_BASE){
						//YUV
						gStreamInformation[i-1].encode_type = E_ENCODE_TYPE_YUV;
						gStreamInformation[i-1].sampling = E_SAMPLING_YCC422;
						gStreamInformation[i-1].depth = 8;
						gStreamInformation[i-1].colorimetry = E_COLORIMETRY_BT709;
						gStreamInformation[i-1].width = 640;
						gStreamInformation[i-1].height = 480;
						gStreamInformation[i-1].payload_id = 100; 
					} else if(stream_type >=META_BASE && stream_type < AUDIO_BASE){
						//Meter data
						gStreamInformation[i-1].media_type = D_MEDIA_TYPE_META;
						gStreamInformation[i-1].encode_type = E_ENCODE_TYPE_META; 
						gStreamInformation[i-1].payload_id = 99; 
					} else if(stream_type >=AUDIO_BASE && stream_type < 32){
						gStreamInformation[i-1].media_type = D_MEDIA_TYPE_AUDIO;
						gStreamInformation[i-1].encode_type = E_ENCODE_TYPE_ACC; 
	//					gStreamInformation[i-1].clock_rate = 48000; 
						gStreamInformation[i-1].clock_rate = 8000; 
						gStreamInformation[i-1].payload_id = 98; 
					}else{
						printf("Parameter error! You must set the parameter from 0~31\n");
						ret = -10;
						break;
					}
				}
				//Initialize RTCP setting
			 	if(gRTSP_func >=E_RTSP_FUNCTION_RTSP_RTCP) {
					rtcp_init(netsec_fd,ReceiveRTCPcb);
				}
			}
			if(ret ==0){
				gValid_num = argc-1;
				pthread_create(
						&gTh_reviceFromSock[E_THREAD_GET_STREAM],
						(void*)NULL,
						stream_infor_Thread,
						(void*)0);
				pthread_create(
						&gTh_reviceFromSock[E_THREAD_ADD_STREAM],
						(void*)NULL,
						stream_Add_Thread,
						(void*)0);
				//Get start time
				clock_gettime(CLOCK_MONOTONIC, &gStartTime);
				pthread_join( gTh_reviceFromSock[E_THREAD_GET_STREAM], NULL );
				pthread_join( gTh_reviceFromSock[E_THREAD_ADD_STREAM], NULL );
			}
		}else {
			//For YUV size setting
			gMacAddress =argv[2];
			gIpAddress =argv[1]; 

			add_all(argv[1],argv[2]);
			while ( !app_abort )
			{
				streaming_sock_receive(&sfd, 1000);
			//add_all("192.168.10.7","00-12-34-56-78-9A");
			}
			streaming_sock_terminate(&sfd);
		}
		pthread_join( gTh_reviceFromSock[E_THREAD_SET_STREAM_INFOR], NULL );
		pthread_join( gTh_reviceFromSock[E_THREAD_WAIT_STREAM_SENT], NULL );
	}
	else
	{
		printf("%s[%d] %03d : %s\n",__func__,__LINE__, errno, strerror(errno));
		ret = -1;
	}

	Stream_ipcu_ch_close();

	printf("End of %s\n",argv[0]);
	return ret;
}

