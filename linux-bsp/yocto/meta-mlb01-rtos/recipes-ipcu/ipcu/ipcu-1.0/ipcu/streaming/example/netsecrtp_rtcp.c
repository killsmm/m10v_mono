/******************************************************************************
 *
 *  @file   netsecrtp_rtcp.c
 *  @brief  !!5EXPERIMENTAL !!
 *
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
#include <sys/time.h>//for performance
#include <time.h>
#include <pthread.h>
#define RTCP_SR_SZ 28
#define CO_RTCP_NETSEC
#ifdef ROUNDTRIP_DELAY_CHECK
struct timespec t,s; 
int delay_lock = 0;
#endif
#define STREMD_RTCP_MAX (32) //Max receive RTCP num
typedef struct {
	int Sock;
	int src_rtcp_port;
	char des_ip[32];
	CbReceivertcp ProcessRTCP;

}T_RECIEVE_RTCP;
//Define RTCP sending buffer address
const char gRTCP_BUFFER_DEF[] = "/proc/device-tree/shared_commem/rtcp-addr";
const char gRTCP_BUFFER_DEF_SIZE[] = "/proc/device-tree/shared_commem/rtcp-size";
static T_RECIEVE_RTCP gReceive_rtcp[STREMD_RTCP_MAX];
static int gSock_valid_num=0;

static T_RTCP_RECEIVE_DATA gReceive_rtcp_data[STREMD_RTCP_MAX];
static unsigned int gRtcp_pa;
static unsigned int gRtcp_va;
static int gRTCP_Data_size; //SR data size
static fd_set gReadfds;
struct timeval tv;
static int gNetsec_fd;
static pthread_mutex_t gMutex;
static pthread_t gRecvth;
static void rtcp_recv_thread(void *p);
static CbReceivertcp ProcessRTCP=NULL;//Receive RTCP call back funtion

/******************************************************************************/
/**
 *	@brief	Ctreate ntptimestamp
 *	@fn		uint64_t get_rtcp_ntpts64()
 *	@param	[in]	none
 *	@retval	
 *		-	ntptimestamp	:Normal
 *		-	0				:Abnormal
 *	@note
 *		- 
 *	@author		
 */
/******************************************************************************/
static uint64_t get_rtcp_ntpts64()
{
	struct timespec ts;
	uint64_t ntpts = 0;
	
	int32_t ret = 0;
	ret = clock_gettime(CLOCK_REALTIME, &ts);
	if( ret < 0 )
	{
		printf("clock getting error!");
	}
	else {
		/* Convert nanoseconds to 32-bits fraction (232 picosecond units) */
		ntpts = (uint64_t)(ts.tv_nsec) << 32;
		ntpts /= 1000000000;
	
		/* There is 70 years (incl. 17 leap ones) offset to the Unix Epoch.
		 * No leap seconds during that period since they were not invented yet.
		 */
		//assert(ntpts < 0x100000000)

		ntpts |= ((70LL * 365 + 17) * 24 * 60 * 60 + ts.tv_sec) << 32;
	}	
	return ntpts;
}
/******************************************************************************
 *  @brief  Make SR data
 *  @fn make_sr
 *  @param  [out]    rtcp    : SR data sending buffer
 *  @param  [in]   sr    : SR data
 *  @param  [in]   ReportBlockSize    : Report block data size
 *  @note
******************************************************************************/
static uint32_t make_sr(uint32_t *rtcp,
		 const T_SENDER_INFOR* sr, int ReportBlockSize)
{
	uint32_t rtph;
	unsigned long long ntpts = get_rtcp_ntpts64();
	rtph = 0;
	rtph |= (2 << 30);//version
	rtph |= (0 << 29);//padding
	if(ReportBlockSize>0) {
		rtph |= (sr->ReportBlocknum << 24);//payload num
	}
	rtph |= (200 << 16);
	rtph |= (((RTCP_SR_SZ + ReportBlockSize)/4 -1) << 0);
	//sender info
	rtcp[0] = htonl(rtph);
	rtcp[1] = htonl(sr->ssrc);
	rtcp[2] = htonl(ntpts>>32);
	rtcp[3] = htonl((uint32_t)ntpts);
	rtcp[4] = htonl(sr->rtp_ts+sr->ntp);
	rtcp[5] = htonl(sr->pkt_cnt);
	rtcp[6] = htonl(sr->pkt_octet);

	return RTCP_SR_SZ;
}
/******************************************************************************
 *  @brief  Get RTCP data from received buffer
 *  @fn GetReceiveData
 *  @param  [in]    ibuf    : UDP receiving data
 *  @param  [out]   received_rtcp    : It is stored RTCP "receive report"
 *  @return Processed size
 *  @note
******************************************************************************/
static int GetReceiveData(const char* ibuf,
						 T_RTCP_PACKET* received_rtcp)
{
	int ret =0;
	received_rtcp->rtcp_head.word=
		ntohl(*((unsigned int*)(ibuf)));
#if 0
	printf("%d:version padding:%d,RRcount:%d,packet_type:%d,Length:%d\n",
		received_rtcp->rtcp_head.bit.version,
			received_rtcp->rtcp_head.bit.padding,
			received_rtcp->rtcp_head.bit.RRcount,
			received_rtcp->rtcp_head.bit.packet_type,
			received_rtcp->rtcp_head.bit.Length);
#endif
	switch(received_rtcp->rtcp_head.bit.packet_type) {
	case E_RTCP_RECEIVE_PT_BYE://good bye
		ret= RTCP_BYE;
	case E_RTCP_RECEIVE_PT_RR:
	case E_RTCP_RECEIVE_PT_SD:
		{
			int bytelen=received_rtcp->rtcp_head.bit.Length*4;
			//Hunt memory for report block
			received_rtcp->ReportBlock=
					malloc(bytelen);

			if(received_rtcp->ReportBlock) {
				for(int i=1;
					i<=received_rtcp->rtcp_head.bit.Length;
					i++) {
					received_rtcp->ReportBlock[i-1] =
						ntohl(*((unsigned int*)(&ibuf[i*4])));
				}
#if 0
				if(received_rtcp->rtcp_head.bit.packet_type == E_RTCP_RECEIVE_PT_RR){
					printf("SSRCofSender1:%x\n",
						(received_rtcp->ReportBlock[0]));
					printf("SSRCofSender2:%x\n",
						(received_rtcp->ReportBlock[1]));
				}
				else{
					printf("SSRCofSender3:%x\n",
						(received_rtcp->ReportBlock[0]));
				}
#endif
			}
			else {
				printf("No enough memory\n");
				ret=-3;
			}				
			break;
		}
	default:
		break;	
	}
	return ret;
}
/******************************************************************************
 *  @brief  sample_app
 *  @fn get_recv_repo
 *  @param  [in]    ibuf    : UDP receiving data
 *  @param  [in]    size    : UDP receiving size
 *  @param  [in]   rtcp_index    : RTCP saving area's index
 *  @note
******************************************************************************/
static int get_recv_repo(char* ibuf, int size, int rtcp_index)
{
	unsigned int total =0;
	int ret= -1;
	int copy_index=0;
	gReceive_rtcp_data[rtcp_index].valid_receive_packet_num = 0;
    do {
		gReceive_rtcp_data[rtcp_index].valid_receive_packet_num++;
		if(copy_index >=D_RTCP_RECEIVE_TYPE_NUM){
			printf("RTCP receive buffer is not enough\n");
			ret = -1;
			break;
		}
		ret=GetReceiveData(&ibuf[total],
			&(gReceive_rtcp_data[rtcp_index].received_rtcp[copy_index])
			);
		if(ret !=0 ){
			if(ret != RTCP_BYE) {
				printf("%s %d ret=%d\n",__func__,__LINE__,ret);
			}
			break;
		}
		total +=
			((gReceive_rtcp_data[rtcp_index].received_rtcp[copy_index].rtcp_head.bit.Length+1)*4);
		copy_index++;
	}while( total < size );

	return ret;
}
/******************************************************************************
 *  @brief  RTCP receiver's Thread
 *  @fn rtcp_recv_thread
 *  @param  [in]    p    : Unused
 *  @note
******************************************************************************/
static void rtcp_recv_thread(void *p)
{
	int sz;
	int iscontinue=1;
	char buf[2048];
	fd_set fds;
	int Maxfds =-1;
	int ret;
	struct sockaddr_in addr[STREMD_RTCP_MAX];

	FD_ZERO(&gReadfds);
	for(int i=0;i<gSock_valid_num;i++) {
		gReceive_rtcp[i].Sock = 
			socket(AF_INET, SOCK_DGRAM, 0);
	}
	for(int i=0;i<gSock_valid_num;i++) {
		addr[i].sin_family = AF_INET;
		addr[i].sin_addr.s_addr = htonl(INADDR_ANY);//htonl(inet_addr((const char*)(gReceive_rtcp[i].des_ip)));
		addr[i].sin_port = htons(gReceive_rtcp[i].src_rtcp_port);
	}
	for(int i=0;i<gSock_valid_num;i++) {
		if(bind(gReceive_rtcp[i].Sock, 
			(struct sockaddr *)&addr[i], sizeof(struct sockaddr_in))<0){
			printf("bind->%d\n",i);
		}
	}
	for(int i=0;i<gSock_valid_num;i++) {
		// Register sock to for wait using select
		FD_SET(gReceive_rtcp[i].Sock, &gReadfds);
		if(Maxfds < gReceive_rtcp[i].Sock) {
			Maxfds = gReceive_rtcp[i].Sock;
		}
	}
	while(iscontinue)
	{
		//initialize fd_set
		//select should be initialized every time
		memcpy(&fds, &gReadfds, sizeof(fd_set));

		//Waiting for fds to be able read
        //Add 1 to file descriptor
		select(Maxfds+1, &fds, NULL, NULL, NULL);
		for(int i=0;i<gSock_valid_num;i++) {
			if (FD_ISSET(gReceive_rtcp[i].Sock, &fds)) {
				sz = recv(gReceive_rtcp[i].Sock, buf, sizeof(buf), 0);

				memset(gReceive_rtcp_data[i].received_rtcp,0,
					sizeof(gReceive_rtcp_data[i].received_rtcp));
				gReceive_rtcp_data[i].src_port =gReceive_rtcp[i].src_rtcp_port;
				ret =get_recv_repo( buf, sz, i);
				if(ProcessRTCP) {
					ProcessRTCP(&gReceive_rtcp_data[i]);
				}
				//Release memory
				for(int j=0;j<D_RTCP_RECEIVE_TYPE_NUM;j++){
					if(gReceive_rtcp_data[i].received_rtcp[j].ReportBlock){
						free(gReceive_rtcp_data[i].received_rtcp[j].ReportBlock);
					}
					else{
						break;
					}
				}
				if(ret==RTCP_BYE) {
					iscontinue =0;
					break;
				}
			}
		}
	}
	for(int i=0;i<gSock_valid_num;i++) {
		close(gReceive_rtcp[i].Sock);
	}
	gSock_valid_num=0;
	printf("Finished RTCP receive as Goodbye was received\n");
	return ;
}
/******************************************************************************
 *  @brief  Send RTCP's SR
 *  @fn send_rtcp_report
 *  @param  [in]   sr    : SR's data sent
 *  @param  [in]   stream_type_id    : Stream type ID(0~7:H264, 8~15:Hevc,16~19:MJPEG,20~23:YUV,24~27:Meta,28~31)
 *  @note
******************************************************************************/
int send_rtcp_report(const T_SENDER_INFOR* sr,
					int stream_type_id
					)
{
	struct ts_descriptor_info rtcp_dsc;
	uint32_t* rtcp_data =(uint32_t*)gRtcp_va;
	U_NETSEC_DESC_ID uniq_id = {0};
	int copy_size = sr->ReportBlockSize;
	uint32_t* sendingRRdata=(uint32_t*)sr->sendingRR;
	uint32_t* sendingRRToNet;

	if(copy_size <=0 ){
		copy_size = 0;
	}
	else if(copy_size>=gRTCP_Data_size-RTCP_SR_SZ) {
		printf("SR's buffer size is not enough!!\n");
		copy_size = 0;
	}

	rtcp_dsc.fd = gNetsec_fd;
	rtcp_dsc.paddr = gRtcp_pa;
	rtcp_dsc.len = RTCP_SR_SZ + copy_size;
	rtcp_dsc.ts = sr->rtp_ts;
	rtcp_dsc.es_vid = stream_type_id;
	pthread_mutex_lock(&gMutex);

	uniq_id.word = 0;

	make_sr(rtcp_data,sr,copy_size);
	copy_size /= 4;
	sendingRRToNet = (unsigned int*)(gRtcp_va + RTCP_SR_SZ);
	for(int i=0; i<copy_size;i++) {
		sendingRRToNet[i] = htonl(sendingRRdata[i]);
	}
	NSec_set_rtcp_Tx_Descriptor(rtcp_dsc,uniq_id);
	pthread_mutex_unlock(&gMutex);
	return 0;
}
	
/******************************************************************************
 *  @brief  End the RTCP receiving thread.
 *  @fn rtcp_end 
 *  @note
******************************************************************************/
int rtcp_end()
{
	if(gSock_valid_num>0) {
		pthread_cancel(gRecvth);
		pthread_join(gRecvth, NULL);
		for(int i=0;i<gSock_valid_num;i++) {
			close(gReceive_rtcp[i].Sock);
		}
	}
	gSock_valid_num=0;
	munmap((void*)gRtcp_va, gRTCP_Data_size);
	printf("Finished RTCP receive as Close was called\n");
	return 0;
}
/******************************************************************************
 *  @brief  Add a RTCP received setting
 *  @fn rtcp_open 
 *  @param  [in]    rtcp_rr    : RTCP Receive setting
 *  @param  [in]    data_num   : rtcp_rr's size
 *  @note
******************************************************************************/
int Add_rtcp_receiver(const T_RTCP_RECEIV_INFOR* rtcp_rr, int data_num)
{
	int ret = 0;
	if(data_num > STREMD_RTCP_MAX) {
		ret = -1;
	}
	else {
		for(int i=0;i<data_num;i++) {
			gReceive_rtcp[i].src_rtcp_port = rtcp_rr[i].src_rtcp_port;
			strncpy(gReceive_rtcp[i].des_ip, rtcp_rr[i].des_ip,sizeof(gReceive_rtcp[i].des_ip));
		}
		gSock_valid_num = data_num;
		pthread_create(&gRecvth, NULL, (void*)rtcp_recv_thread, (void*)NULL);
	}
	return ret;
}
/////////////////////////////////////
//Get RTCP buffer's address
/////////////////////////////////////
static unsigned int get_rtcp_buffer()
{
	FILE *file;
	unsigned int  buffer=0;


	file = fopen(gRTCP_BUFFER_DEF, "rb");
	if(file !=NULL) {
		fread((&buffer), 1,sizeof(buffer),file);
		fclose(file);
		buffer=ntohl(buffer);
		file = fopen(gRTCP_BUFFER_DEF_SIZE, "rb");
		if(file !=NULL) {
			fread(&gRTCP_Data_size, 1,sizeof(gRTCP_Data_size),file);
			fclose(file);
			gRTCP_Data_size=ntohl(gRTCP_Data_size);
		}
	}
	return buffer;
}
/******************************************************************************
 *  @brief  RTCP sending/receiving initialization
 *  @fn rtcp_open 
 *  @param  [in]    fd    : NETSEC's file descripor
 *  @param  [in]    cbRTCP   : A call back function for received RTCP
 *  @ret always 0
 *  @note
******************************************************************************/
int rtcp_init(int fd, CbReceivertcp cbRTCP)
{
	//netsec for TX
	//get VA and PA
	int mmem_fd = open("/dev/mem", (O_RDWR | O_SYNC));
	int offset;

//	FJ_MM_getPhysAddrInfo(IPCU_MM_BUFF, &gRtcp_pa, &dummy);
	gRtcp_pa = get_rtcp_buffer(); 
	//Check the address whether it can be divided by 0x1000
	offset = (int)gRtcp_pa - (int)(gRtcp_pa & (~(0x1000 - 1)));
	
 
	gRtcp_va = (unsigned int)mmap(0, gRTCP_Data_size, PROT_READ | PROT_WRITE, 
			MAP_SHARED, mmem_fd, gRtcp_pa & (~(0x1000 - 1)));
	gRtcp_va +=offset;
	gRTCP_Data_size -=offset;
	close(mmem_fd);
	//thread
	gNetsec_fd = fd;
	ProcessRTCP = cbRTCP;
	pthread_mutex_init(&gMutex, NULL);
	return 0;
}

