#include <signal.h>
#include <syslog.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <poll.h>
#include <errno.h>

#include "netsecrtp_api.h"

#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
/* ============== Configuration for Stream ============== */
#define ETH_SRC_MAC_ADDR_HIGH 0x12345678 // 12:34:56:78:91:23
#define ETH_SRC_MAC_ADDR_LOW  0x9123
#define ETH_TYPE 0x800 // IPv4

#define IPV4_TOS     0xff
#define IPV4_INIT_ID 0x1234
#define IPV4_TTL     0xff11

#define RTP_INIT_SEQNUM 0x00000000
#define RTP_PAYLOAD_LEN 1472
/* ====================================================== */


/* =============== Device & File Path =================== */

#define NETSEC_NAME		     "/dev/netsec0"
/* ====================================================== */


#define MMAP_DEVICE "/dev/mem"

struct	_rxqueue {
	u32		num;
	u32		wr_count;
	u32		reserved[2];

	NETSEC_RX_DATA	rxque [NUM_RXQ];
}rxq;


int getipaddr()
{
	int fd;
	int retry;
	struct ifreq eth0;

	retry = 0;
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if( fd < 0 )
	{
		printf("%s[%d] %03d : %s\n",__func__,__LINE__, errno, strerror(errno));
		for( retry = 0; retry < 3; retry++ )
		{
			usleep(33000);
			fd = socket(AF_INET, SOCK_DGRAM, 0);
			if( fd < 0 )
			{
				printf("%s[%d] %03d : %s retry %d \n",__func__,__LINE__, errno, strerror(errno), retry);
			}
		}
	}
	/* set IPv4 */
 	eth0.ifr_addr.sa_family = AF_INET;

	/* get eth IP */
	strncpy(eth0.ifr_name, "eth0", IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &eth0);
	close(fd);
	return ((struct sockaddr_in *)&eth0.ifr_addr)->sin_addr.s_addr;

}
char ctoi(char str)
{	
	char ret = 0;
	if( str < 0x40 )
	{
		ret = str & 0x0F;
	}
	else if( str < 0x47 || str < 0x67)
	{
		switch( (str & 0x0F) )
		{
			case 0x1:
			ret = 10;
			break;
			case 0x2:
			ret = 11;
			break;
			case 0x3:
			ret = 12;
			break;
			case 0x4:
			ret = 13;
			break;
			case 0x5:
			ret = 14;
			break;
			case 0x6:
			ret = 15;
			break;
		}
	}
	return ret;
}

void getmac(const char* str, unsigned long* h,unsigned long* l )
{
	int i;
	int j;
	int k;
	char c;

	char dt[6][2];

	memset( dt, 0 , sizeof(dt));
	k = 0;
	i = 0;
	j = 0;
	while( k < 6 )
	{
		c = *((char*)str + j);
		j++;
		if( c == '\0' )
		{
			break;
		}
		else if( c == ':' || c == '-' )
		{
			k++;
			if( i == 1 )
			{
				dt[k][1] = dt[k][0];
				dt[k][0] = 0;
			}
			i = 0;
		}
		else
		{
			dt[k][i] = c;
			i++;
		}
	}
	*h  = ctoi(dt[0][0] ) << 12;
	*h |= ctoi(dt[0][1] ) <<  8;
	*h |= ctoi(dt[1][0] ) <<  4;
	*h |= ctoi(dt[1][1] ) <<  0;
	*l  = ctoi(dt[2][0] ) << 28;
	*l |= ctoi(dt[2][1] ) << 24;
	*l |= ctoi(dt[3][0] ) << 20;
	*l |= ctoi(dt[3][1] ) << 16;
	*l |= ctoi(dt[4][0] ) << 12;
	*l |= ctoi(dt[4][1] ) << 8;
	*l |= ctoi(dt[5][0])<< 4;
	*l |= ctoi(dt[5][1])<< 0;
}
#if 0
unsigned long rx_data(char* dt, unsigned long r )
{
	unsigned long len;

	len = rxq.rxque[r].length;
	rxq.rxque[r].length = 0;
	dt = (char*)rxq.rxque[r].rx_buff[0];
	return len;
}
#endif
/**
 * @fn
 * Set Sessiont DB for basic part
 * @brief 
 * @param[in] p session db parameter
 * @return non 
 * @detail 
 */
void NSec_set_sessiondb(struct session_db_info p) {
	int src_ip;
	int dst_ip;
	unsigned long dst_mac_h;
	unsigned long dst_mac_l;
	struct netsec_sessdb_arg sessbase;
	struct netsec_sessdb_arg sessdb;
	int pkt_ctrl;

	pkt_ctrl = 0x00000000; 
	ioctl(p.fd, NETSEC_IOCSMODE, pkt_ctrl);

//	printf("        IP   %X\n", getipaddr());
	src_ip = htonl(getipaddr());
	dst_ip = htonl(inet_addr(p.str_dst_ip));
	getmac( p.str_dst_mac, &dst_mac_h, &dst_mac_l );

	if( src_ip == INADDR_NONE && dst_ip == INADDR_NONE )
	{
		printf( "Invalid IP address\n" );
	}
	else if( p.src_port > 65535 && p.dst_port > 65535 )
	{
		printf( "Invalid PORT# source[%d] destination[%d]\n",p.src_port, p.dst_port );
		printf( "        0 <= PORT <= 65535\n" );
	}
	else if( 0 > p.es_vid && p.es_vid > 31 )
	{
		printf( "Invalid es_vid[%d]\n", p.es_vid );
		printf( "        0 <= es_vid <= 5\n" );
	}
	else
	{
		// Set Basic Info	
		sessbase.sess_db_id  = p.sess_db_id;
		sessbase.sess_db[0]	 = RTP_PAYLOAD_LEN;
		sessbase.sess_db[1]	 = dst_mac_h & 0xffff;
		sessbase.sess_db[2]	 = dst_mac_l;
		sessbase.sess_db[3]	 = ETH_SRC_MAC_ADDR_HIGH;
		sessbase.sess_db[4]	 = (ETH_SRC_MAC_ADDR_LOW << 16) | ETH_TYPE;
		sessbase.sess_db[5]	 = (0x45 << 24) | (IPV4_TOS << 16) | 0x00000;
		sessbase.sess_db[6]	 = (IPV4_INIT_ID << 16) | 0x0000;
		sessbase.sess_db[7]	 = (IPV4_TTL << 16) | 0x0000;
		sessbase.sess_db[8]	 = src_ip;
		sessbase.sess_db[9]	 = dst_ip;
		sessbase.sess_db[10] = 0; 
		sessbase.sess_db[11] = 0; 
		sessbase.sess_db[12] = 0; 
		sessbase.sess_db[13] = 0; 
		sessbase.sess_db[14] = 0; 
		sessbase.sess_db[15] = 0;	
	
		ioctl(p.fd, _IOW ( 0x67, 4, struct netsec_sessdb_arg ), 
			  (struct netsec_sessdb_arg *)&sessbase);

		// Set Stream Session
		sessdb.sess_db_id  = p.es_vid;
		sessdb.sess_db[0]  = sessbase.sess_db_id;
		sessdb.sess_db[1]  = ((p.src_port << 16) & 0xFFFF0000) | (0x0000FFFF & p.dst_port);
		sessdb.sess_db[2]  = p.rtp_payload_type;
		sessdb.sess_db[3]  = p.ssrc; 
		sessdb.sess_db[4]  = p.ntp; 
		sessdb.sess_db[5]  = RTP_INIT_SEQNUM;	
		sessdb.sess_db[6]  = 0; 
		sessdb.sess_db[7]  = p.yuv_pix_num; 
		sessdb.sess_db[8]  = p.sessdb08; 
		sessdb.sess_db[9]  = p.sessdb09; 
		sessdb.sess_db[10] = p.sessdb10; 
		sessdb.sess_db[11] = p.sessdb11; 
		sessdb.sess_db[12] = 0; 
		sessdb.sess_db[13] = 0; 
		sessdb.sess_db[14] = 0; 
		sessdb.sess_db[15] = 0;		
	
		ioctl(p.fd, _IOW ( 0x67, 4, struct netsec_sessdb_arg ), 
			  (struct netsec_sessdb_arg *)&sessdb);
		#if 0
		printf("sessbase 0x%08x \n",sessbase.sess_db_id);
		for(int i = 0; i < 15 ; i ++ ) {
			printf("0x%08x \n",sessbase.sess_db[i]);
		}
		printf("\n");
		printf("sessdb 0x%08x \n",sessdb.sess_db_id);
		for(int i = 0; i < 15 ; i ++ ) {
			printf("0x%08x \n",sessdb.sess_db[i]);
		}
		#endif
	}
//	printf("src:%X, dst:%X, get:%X\n",sessbase.sess_db[8],sessbase.sess_db[9],htonl(getipaddr()));
}
void NSec_set_Videobps_Descriptor(struct ts_descriptor_info p, unsigned long bps)
{
	struct netsec_desc desc;
	int ret;

	desc.uniq_id.word = 0;
	desc.desc[0] = 0x82000370;
	desc.desc[1] = 0;  // physical address of buf
	desc.desc[2] = 0;// transmit size
	desc.desc[3] = bps;//sampling starting time
	ret = write(p.fd, &desc, sizeof(struct netsec_desc));
	if( ret < 0 )
	{
		printf("%03d : %s\n", errno, strerror(errno));
	}
}

void NSec_set_Video_Tx_Descriptor(struct ts_descriptor_info p,U_NETSEC_DESC_ID uniq_id)
{
	struct netsec_desc desc;
	int ret;
//	struct timeval s, t;//performance
	unsigned long stid;

	desc.uniq_id.word = uniq_id.word;
	stid = 0;
	stid = ((p.es_vid & 0x00000010) << 12) | (p.es_vid & 0x0000000F);
	// Video Tx Descriptor 0
	desc.desc[0] = (0x82200300 | (0x000F0000 & p.fmt));
	desc.desc[0] = (desc.desc[0] | stid);
	desc.desc[0] = (desc.desc[0] | (0x000000F0 & p.nvcl));

	desc.desc[1] = p.paddr;  // physical address of buf
	desc.desc[2] = p.len;// transmit size
	desc.desc[3] = p.ts;//sampling starting time
	//printf(".desc[%d] 0x%08x \n",0, desc.desc[0]);
	//printf(".desc[%d] 0x%08x \n",1, desc.desc[1]);
	//printf(".desc[%d] 0x%08x \n",2, desc.desc[2]);
	//printf(".desc[%d] 0x%08x \n\n",3, desc.desc[3]);

//	gettimeofday(&s, NULL);//performance
	ret = write(p.fd, &desc, sizeof(struct netsec_desc));
	if( ret < 0 )
	{
		printf("%03d : %s\n", errno, strerror(errno));
	}

//	gettimeofday(&t, NULL);//performance
//	printf("%s %d ms\n",__func__, (t.tv_sec - s.tv_sec) * 1000 + (t.tv_usec - s.tv_usec) / 1000);//performance
}
void NSec_set_rtcp_Tx_Descriptor(struct ts_descriptor_info p, U_NETSEC_DESC_ID uniq_id)
{
	struct netsec_desc desc;
	int ret;
	unsigned int stid = 0;

	desc.uniq_id.word = uniq_id.word;

	stid = ((p.es_vid & 0x00000010) << 12) | (p.es_vid & 0x0000000F);
	desc.desc[0] = 0x82000360 | stid;
	desc.desc[1] = p.paddr;  // physical address of buf
	desc.desc[2] = p.len;// transmit size
	desc.desc[3] = p.ts;//sampling starting time
	
	ret = write(p.fd, &desc, sizeof(struct netsec_desc));
	if( ret < 0 )
	{
		printf("%03d : %s\n", errno, strerror(errno));
	}
//	printf("\n0x%08x \n", desc.desc[0]);
//	printf("0x%08x \n", desc.desc[1]);
//	printf("0x%08x \n", desc.desc[2]);
//	printf("0x%08x \n", desc.desc[3]);
}
void NSec_set_meta_Tx_Descriptor(struct ts_descriptor_info p,U_NETSEC_DESC_ID uniq_id)
{
	struct netsec_desc desc;
	int ret;
	unsigned long stid;
	
	desc.uniq_id.word = uniq_id.word;
	stid = 0;
	stid = ((p.es_vid & 0x00000010) << 12) | (p.es_vid & 0x0000000F);
	desc.desc[0] = (0x8d200300 | stid);
	desc.desc[1] = p.paddr;  // physical address of buf
	desc.desc[2] = p.len;// transmit size
	desc.desc[3] = p.ts;//sampling starting time
	ret = write(p.fd, &desc, sizeof(struct netsec_desc));
	if( ret < 0 )
	{
		printf("%03d : %s\n", errno, strerror(errno));
	}
//	printf("\n0x%08x \n", desc.desc[0]);
//	printf("0x%08x \n", desc.desc[1]);
//	printf("0x%08x \n", desc.desc[2]);
//	printf("0x%08x \n", desc.desc[3]);
}
void NSec_set_audio_Tx_Descriptor(struct ts_descriptor_info p,U_NETSEC_DESC_ID uniq_id)
{
	struct netsec_desc desc;
	int ret;
	unsigned long stid;
	
	desc.uniq_id.word = uniq_id.word;
	stid = 0;
	stid = ((p.es_vid & 0x00000010) << 12) | (p.es_vid & 0x0000000F);
	desc.desc[0] = 0x84200300 | stid;
	//desc.desc[0] = (0x84200300 | p.es_vid);//for G.711
	desc.desc[1] = p.paddr;  // physical address of buf
	desc.desc[2] = p.len;// transmit size
	desc.desc[3] = p.ts;//sampling starting time
	ret = write(p.fd, &desc, sizeof(struct netsec_desc));
	if( ret < 0 )
	{
		printf("%03d : %s\n", errno, strerror(errno));
	}
}
void NSec_set_jpeg2_Tx_Descriptor(struct jpeg_descriptor_info p,U_NETSEC_DESC_ID uniq_id)
{
	struct netsec_desc desc;
	int ret;
	int i;
	unsigned int payload_ofs;
//	struct timeval s, t;//performance
	unsigned long stid;
	//timing
//	desc.desc[0] = (0x8b000210 | p.es_vid);
//	desc.desc[1] = 0;
//	desc.desc[2] = 0;
//	desc.desc[3] = 0;
//	ret = write(p.fd, &desc, sizeof(struct netsec_desc));
//	if( ret < 0 )
//		printf("%s[%d] %03d : %s\n",__func__,__LINE__, errno, strerror(errno));

	// RTOS JPEG Const.
	p.qtable_cnt = 2;
//	p.qtable_ofs[0] = 0x07;
//	p.qtable_ofs[1] = 0x48;
//	p.qtable_sz[0] = 0x40;
//	p.qtable_sz[1] = 0x40;
	payload_ofs = 0x24D;
	
	desc.uniq_id.word = uniq_id.word;
	stid = 0;
	stid = ((p.es_vid & 0x00000010) << 12) | (p.es_vid & 0x0000000F);
	for( i = 0; i < p.qtable_cnt; i++ )
	{
		desc.desc[1] = p.paddr + p.qtable_ofs[i];
		desc.desc[2] = p.qtable_sz[i];
		desc.desc[3] = p.ts;//sampling starting time
		if( (i & 0x1) == 0 )//brightness?
		{
			desc.desc[0] = (0x8b000210 | stid);
		}
		else
		{
			desc.desc[0] = (0x8b000110 | stid);
		}
		ret = write(p.fd, &desc, sizeof(struct netsec_desc));
		if( ret < 0 )
			printf("%s[%d] %03d : %s\n",__func__,__LINE__, errno, strerror(errno));

	//printf("\n0x%08x \n", desc.desc[0]);
	//printf("0x%08x \n", desc.desc[1]);
	//printf("0x%08x \n", desc.desc[2]);
	//printf("0x%08x \n", desc.desc[3]);
	}
	//jpeg payload
	desc.desc[0] = (0x8b210300 | (p.es_vid & 0x0000000F));
	desc.desc[1] = p.paddr + payload_ofs;  // physical address of buf
	desc.desc[2] = p.len - payload_ofs;// transmit size
	desc.desc[3] = p.ts;//sampling starting time
	ret = write(p.fd, &desc, sizeof(struct netsec_desc));

	if( ret < 0 )
		printf("%s[%d] %03d : %s\n",__func__,__LINE__, errno, strerror(errno));

	//printf("\n0x%08x \n", desc.desc[0]);
	//printf("0x%08x \n", desc.desc[1]);
	//printf("0x%08x \n", desc.desc[2]);
	//printf("0x%08x \n", desc.desc[3]);

}

/**
 * @fn
 * Set rtp loopback buffer
 * @brief 
 * @param[in] fd: netsec_fd, rxq: NETSEC_RX_QUE
 * @return non 
 * @detail 
 */
void NSec_set_rtp_lpb_buffer(int fd, NETSEC_RX_QUE *rxq ) {
	int pkt_ctrl;
	struct netsec_init_arg args;

	rxq->num = NUM_RXQ;
	args.lpbk_rx_que = rxq;
	ioctl(fd, NETSEC_IOCINIT, &args);
	pkt_ctrl = 0x00000000; 
	ioctl(fd, NETSEC_IOCSMODE, pkt_ctrl);
}

/**
 * @fn
 * Set Sessiont DB for rtp loopback
 * @brief 
 * @param[in] p session db parameter
 * @return non 
 * @detail 
 */
void NSec_set_streamdb_rtp_lpb(struct session_db_info p) {
	struct netsec_sessdb_arg sessdb;

	// Set Stream Session
	sessdb.sess_db_id  = p.es_vid;
	sessdb.sess_db[0]  = p.es_vid + 32;// don't care
	sessdb.sess_db[1]  = RTP_PAYLOAD_LEN;
	sessdb.sess_db[2]  = p.rtp_payload_type;
	sessdb.sess_db[3]  = p.ssrc; 
	sessdb.sess_db[4]  = p.ntp; 
	sessdb.sess_db[5]  = RTP_INIT_SEQNUM;
	sessdb.sess_db[6]  = 0; 
	sessdb.sess_db[7]  = p.yuv_pix_num;
	sessdb.sess_db[8]  = p.sessdb08;
	sessdb.sess_db[9]  = p.sessdb09;
	sessdb.sess_db[10] = p.sessdb10;
	sessdb.sess_db[11] = p.sessdb11;
	sessdb.sess_db[12] = 0;
	sessdb.sess_db[13] = 0;
	sessdb.sess_db[14] = 0;
	sessdb.sess_db[15] = 0;
	ioctl(p.fd, _IOW ( 0x67, 4, struct netsec_sessdb_arg ), 
		  (struct netsec_sessdb_arg *)&sessdb);
	#if 0
	printf("sessdb 0x%08x \n",sessdb.sess_db_id);
	for(int i = 0; i < 15 ; i ++ ) {
		printf("0x%08x \n",sessdb.sess_db[i]);
	}
	#endif
}

void NSec_set_Video_rtp_lpb_Descriptor(struct ts_descriptor_info p)
{
	struct netsec_desc desc;
	int ret;
	unsigned long stid;

	desc.uniq_id.word = 0;
	stid = 0;
	stid = ((p.es_vid & 0x00000010) << 12) | (p.es_vid & 0x0000000F);
	desc.desc[0] = (0x82200300 | (0x000F0000 & p.fmt));
	desc.desc[0] = (desc.desc[0] | stid);
	desc.desc[0] = (desc.desc[0] | (0x000000F0 & p.nvcl));
	desc.desc[0] = ((desc.desc[0] & 0xFFEFFFFF) | 0x00100000);//for RTPloopback

	desc.desc[1] = p.paddr;  // physical address of buf
	desc.desc[2] = p.len;// transmit size
	desc.desc[3] = p.ts;//sampling starting time
	ret = write(p.fd, &desc, sizeof(struct netsec_desc));
	if( ret < 0 )
	{
		printf("%03d : %s\n", errno, strerror(errno));
	}
//	printf("video FDS 0x%08x \n", desc.desc[0]);

}
void NSec_set_audio_rtp_lpb_Descriptor(struct ts_descriptor_info p)
{
	struct netsec_desc desc;
	int ret;
	unsigned long stid;

	desc.uniq_id.word = 0;
	stid = 0;
	stid = ((p.es_vid & 0x00000010) << 12) | (p.es_vid & 0x0000000F);
	desc.desc[0] = 0x84200300 | stid;
	//desc.desc[0] = (0x84200300 | p.es_vid);//for G.711
	desc.desc[0] = ((desc.desc[0] & 0xFFEFFFFF) | 0x00100000);//for RTPloopback
	desc.desc[1] = p.paddr;  // physical address of buf
	desc.desc[2] = p.len;// transmit size
	desc.desc[3] = p.ts;//sampling starting time
	ret = write(p.fd, &desc, sizeof(struct netsec_desc));
	if( ret < 0 )
	{
		printf("%03d : %s\n", errno, strerror(errno));
	}
//	printf("audio FDS 0x%08x \n", desc.desc[0]);
}


void NSec_set_jpeg2_rtp_lpb_Descriptor(struct jpeg_descriptor_info p)
{
	struct netsec_desc desc;
	int ret;
	int i;
	unsigned int payload_ofs;
	unsigned long stid;

	// RTOS JPEG Const.
	p.qtable_cnt = 2;
	payload_ofs = 0x24D;
	
	desc.uniq_id.word = 0;
	stid = 0;
	stid = ((p.es_vid & 0x00000010) << 12) | (p.es_vid & 0x0000000F);
	for( i = 0; i < p.qtable_cnt; i++ )
	{
		desc.desc[1] = p.paddr + p.qtable_ofs[i];
		desc.desc[2] = p.qtable_sz[i];
		desc.desc[3] = p.ts;//sampling starting time
		if( (i & 0x1) == 0 )//brightness?
		{
			desc.desc[0] = (0x8b000210 | stid);
		}
		else
		{
			desc.desc[0] = (0x8b000110 | stid);
		}
		desc.desc[0] |= 0x00100000; // for RTPloopback
		desc.desc[0] &= (~0x00008080); // for RTPloopback
		ret = write(p.fd, &desc, sizeof(struct netsec_desc));
		if( ret < 0 )
			printf("%s[%d] %03d : %s\n",__func__,__LINE__, errno, strerror(errno));

	//printf("\n0x%08x \n", desc.desc[0]);
	//printf("0x%08x \n", desc.desc[1]);
	//printf("0x%08x \n", desc.desc[2]);
	//printf("0x%08x \n", desc.desc[3]);
	}
	//jpeg payload
	desc.desc[0] = 0x8b200300;
	desc.desc[0] |= ((p.es_vid & 0x00000010) << 12) | (p.es_vid & 0x0000000F);
	desc.desc[0] |= 0x00100000; // for RTPloopback
	desc.desc[0] &= (~0x00008080); // for RTPloopback
	desc.desc[1] = p.paddr + payload_ofs;  // physical address of buf
	desc.desc[2] = p.len - payload_ofs;// transmit size
	desc.desc[3] = p.ts;//sampling starting time
	ret = write(p.fd, &desc, sizeof(struct netsec_desc));

	if( ret < 0 )
		printf("%s[%d] %03d : %s\n",__func__,__LINE__, errno, strerror(errno));

	//printf("\n0x%08x \n", desc.desc[0]);
	//printf("0x%08x \n", desc.desc[1]);
	//printf("0x%08x \n", desc.desc[2]);
	//printf("0x%08x \n", desc.desc[3]);
}

