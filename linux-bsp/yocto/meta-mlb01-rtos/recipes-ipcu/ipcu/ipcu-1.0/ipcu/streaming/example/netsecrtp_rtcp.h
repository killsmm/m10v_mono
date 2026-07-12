#ifndef __NETSECRTP_RTCP_H__
#define __NETSECRTP_RTCP_H__

//#define EXPERIMENTAL_RTCP
//#define ROUNDTRIP_DELAY_CHECK
#define RTCP_BYE -2
#define D_RTCP_RECEIVE_TYPE_NUM	(3)//One time can receive RTCP packet num
#define D_MAX_SENDING_RR_NUM (4)//Max sending RR block num
typedef enum {
	E_RTCP_RECEIVE_PT_SR =200,//Sender report
	E_RTCP_RECEIVE_PT_RR ,//Receive report
	E_RTCP_RECEIVE_PT_SD,//Source description
	E_RTCP_RECEIVE_PT_BYE,//Goodbye
	E_RTCP_RECEIVE_PT_APP_DEFINE,//Application define
}E_RTCP_RECEIVE_PT;
typedef union {
	unsigned int word;
	struct {
		unsigned int  Length :16;
		unsigned int  packet_type	:8;
		unsigned int  RRcount :5;//reception report count
		unsigned int  padding :1;
		unsigned int  version :2;
	}bit;
}U_RTCP_HEAD;
typedef struct {
	U_RTCP_HEAD rtcp_head;
	unsigned int* ReportBlock;
}T_RTCP_PACKET;
typedef struct {
	unsigned short src_port;//Camara's RTCP port
	unsigned short valid_receive_packet_num;//received_rtcp's num
	T_RTCP_PACKET received_rtcp[D_RTCP_RECEIVE_TYPE_NUM];
}T_RTCP_RECEIVE_DATA;
typedef void (*CbReceivertcp)(const T_RTCP_RECEIVE_DATA*);

typedef union {
	unsigned int word;
 	struct {
		unsigned int CumulativeNumPacketsLost	:24;
		unsigned int FractionLost	:8;
	}rr_second;
}U_RR_SECOND;
typedef struct {
	U_RR_SECOND PacketsLost;
	unsigned int HighestSQnum;//extended highest sequence number received
	unsigned int InterarrivalJitter;//interarrival jitter
	unsigned int lastSR;//last SR (LSR)
	unsigned int DelaySinceLastSR;//delay since last SR (DLSR)
}T_REPORT_BLOCK;

typedef struct {
	unsigned long rtp_ts;
	unsigned long pkt_cnt;
	unsigned long pkt_octet;
	unsigned int ssrc;
	unsigned long ntp;
	unsigned int stream_id;
	short ReportBlockSize;//Report Block's valid bytes
	short ReportBlocknum;//Report Block's num
	T_REPORT_BLOCK sendingRR[D_MAX_SENDING_RR_NUM];//Report Block's data
}T_SENDER_INFOR;
typedef struct {
	unsigned int src_rtcp_port;// RTCP's port
	char des_ip[32];//destination IP address
}T_RTCP_RECEIV_INFOR;

extern int rtcp_init(int fd,CbReceivertcp cbRTCP);
extern int Add_rtcp_receiver(const T_RTCP_RECEIV_INFOR* rtcp_rr, int data_num);
extern int send_rtcp_report(const T_SENDER_INFOR* sr,int stream_type_id);
extern int rtcp_end();
#endif//__NETSECRTP_RTCP_H__
