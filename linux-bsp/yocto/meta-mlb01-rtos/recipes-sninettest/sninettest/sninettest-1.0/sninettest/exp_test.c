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

#include "mil_netsec_api.h"

/* ============== Configuration for Stream ============== */

#define ETH_DST_MAC_ADDR_HIGH 0x000a     // 00:0a:e4:9e:32:19
#define ETH_DST_MAC_ADDR_LOW  0xe49e3219
#define ETH_SRC_MAC_ADDR_HIGH 0x12345678 // 12:34:56:78:91:23
#define ETH_SRC_MAC_ADDR_LOW  0x9123
#define ETH_TYPE 0x800 // IPv4

#define IPV4_TOS     0xff
#define IPV4_INIT_ID 0x1234
#define IPV4_TTL     0xff11
#define IPV4_SRC_IP_ADDR 0xa9fec13f // 169.254.193.63
#define IPV4_DST_IP_ADDR 0xa9fec140 // 169.254.193.64

#define RTP_INIT_SEQNUM 0x00000000
#define RTP_PAYLOAD_LEN 1472
#define RTP_PAYLOAD_TYPE 96
#define RTP_SSRC 0x12345678
#define RTP_TIMESTAMP_OFFSET 0x00000000

#define UDP_DST_PORT 5004
/* ====================================================== */


/* =============== Device & File Path =================== */

#define NETSEC_NAME		     "/dev/netsec0"
#define DMAC_HM_ME_CODE_FILE "/root/og_hm_core.bin"
#define DMAC_MH_ME_CODE_FILE "/root/og_mh_core.bin"
#define PACKETME_CODE_FILE   "/root/og_pk_core.bin"
/* ====================================================== */


#define MMAP_DEVICE "/dev/mem"
#define MMAP_OFFSET 0x66B00000
#define MMAP_FLAGS MAP_SHARED

#define	NUM_RXQ			(256)
#define	HOST_FW_INIT_OK	(0x1)

struct	_rxqueue {
	u32		num;
	u32		wr_count;
	u32		reserved[2];

	NETSEC_RX_DATA	rxque [NUM_RXQ];
}			rxq;

#define DATA_BUF_MAX_SIZE (0x10000)


__inline int char2hex(int c)
{
	int result;
	if ('0' <= c && c <= '9') {
		result = c - '0';
	} else if ('a' <= c && c <= 'f') {
		result = c - 'a' + 10;
	} else {
		result = c - 'A' + 10;
	}
	return result;
}

int strconvert2bin(char *str,
				   unsigned int len,
				   char *output)
{
	int i;
	for(i=0;i<len;i++) {
		*(output+i) =
			(char2hex(*(str+(i<<1)))<<4)+char2hex(*(str+(i<<1)+1));
	}
	return i;
}

int set_rxque()
{
	int	 				fd_netsec;
	int					retVal  = 0;
	struct netsec_init_arg		args;
	
	fd_netsec = open( NETSEC_NAME , O_RDWR);
	
	if( fd_netsec < 0 )
	{
		printf("Error: %s: %d\n", __func__, __LINE__);
		return -1;
	}
	
	rxq.num = NUM_RXQ;
	args.lpbk_rx_que		= (NETSEC_RX_QUE *) &rxq;

	ioctl(fd_netsec, NETSEC_IOCINIT, &args);

	close(fd_netsec);
	return retVal;
}

void set_sessiondb(int fd) {
	struct netsec_sessdb_arg sessdb;
	struct netsec_sessdb_arg sessbase;

	// Set Base Info	
	sessbase.sess_db_id  = 14;
	sessbase.sess_db[0]	 = 0; 
	sessbase.sess_db[1]	 = ETH_DST_MAC_ADDR_HIGH & 0xffff;
	sessbase.sess_db[2]	 = ETH_DST_MAC_ADDR_LOW;
	sessbase.sess_db[3]	 = ETH_SRC_MAC_ADDR_HIGH;
	sessbase.sess_db[4]	 = (ETH_SRC_MAC_ADDR_LOW << 16) | ETH_TYPE;
	sessbase.sess_db[5]	 = (0x45 << 28) | (IPV4_TOS << 20) | 0x00000;
	sessbase.sess_db[6]	 = (IPV4_INIT_ID << 16) | 0x0000;
	sessbase.sess_db[7]	 = (IPV4_TTL << 16) | 0x0000;
	sessbase.sess_db[8]	 = IPV4_SRC_IP_ADDR;
	sessbase.sess_db[9]	 = IPV4_DST_IP_ADDR;
	sessbase.sess_db[10] = 0; 
	sessbase.sess_db[11] = 0; 
	sessbase.sess_db[12] = 0; 
	sessbase.sess_db[13] = 0; 
	sessbase.sess_db[14] = 0; 
	sessbase.sess_db[15] = 0;	

	ioctl(fd, _IOW ( 0x67, 4, struct netsec_sessdb_arg ), 
		  (struct netsec_sessdb_arg *)&sessbase);

	// Set Session (ID=0, Video)
	sessdb.sess_db_id  = 0;
	sessdb.sess_db[0]  = 0x0000000e; 
	sessdb.sess_db[1]  = RTP_PAYLOAD_LEN;
	sessdb.sess_db[2]  = RTP_PAYLOAD_TYPE;
	sessdb.sess_db[3]  = RTP_SSRC; 
	sessdb.sess_db[4]  = RTP_TIMESTAMP_OFFSET; 
	sessdb.sess_db[5]  = RTP_INIT_SEQNUM;	
	sessdb.sess_db[6]  = 0; 
	sessdb.sess_db[7]  = 0; 
	sessdb.sess_db[8]  = 0; 
	sessdb.sess_db[9]  = 0; 
	sessdb.sess_db[10] = 0; 
	sessdb.sess_db[11] = 0; 
	sessdb.sess_db[12] = 0; 
	sessdb.sess_db[13] = 0; 
	sessdb.sess_db[14] = 0; 
	sessdb.sess_db[15] = 0;		

	ioctl(fd, _IOW ( 0x67, 4, struct netsec_sessdb_arg ), 
		  (struct netsec_sessdb_arg *)&sessdb);

}


int main(int argc, const char *argv[])
{
	unsigned int			r, len;
	struct netsec_desc desc[32];
	int desc_id = 0;
	const char *strm_data_path = argv[1];
	FILE *fp;
	int fd;
	fd_set sfd;

	// Create Network Socket
	unsigned short port = UDP_DST_PORT;
	int destSocket;
	struct sockaddr_in destSockAddr;
	memset((void *) &destSockAddr, 0, (size_t) sizeof(destSockAddr));
	destSockAddr.sin_addr.s_addr =
		((IPV4_DST_IP_ADDR & 0x000000ff) << 24) |
		((IPV4_DST_IP_ADDR & 0x0000ff00) <<  8) |
		((IPV4_DST_IP_ADDR & 0x00ff0000) >>  8) |
		((IPV4_DST_IP_ADDR & 0xff000000) >> 24);   
	destSockAddr.sin_port = htons(port);
	destSockAddr.sin_family = AF_INET;
	destSocket = socket((int)AF_INET, (int) SOCK_DGRAM, 0);
	
	set_rxque();
	
	if(!strm_data_path)
		return 1;

	fd = open(NETSEC_NAME, O_RDWR);

	if(!fd)
		return 1;
	
	set_sessiondb(fd);
	
	char *strm_data_p = malloc(DATA_BUF_MAX_SIZE * 2);
	
	int fd_map = open(MMAP_DEVICE, O_RDWR);
	if (fd_map == -1) {
		fprintf(stderr, "Unable to open mmap file\n");
		return 1;
	}

	char *buf = mmap(NULL, DATA_BUF_MAX_SIZE * 2, PROT_READ | PROT_WRITE,
							 MMAP_FLAGS, fd_map, MMAP_OFFSET);
	if (buf == MAP_FAILED) {
		fprintf(stderr, "Unable to mmap %d bytes\n", DATA_BUF_MAX_SIZE * 2);
		return 1;
	}
	
	fp = fopen(strm_data_path, "r");
	r = 0;

	// Get Eval Data from a File (Text Data)
	if(fgets(strm_data_p, DATA_BUF_MAX_SIZE * 2, fp) == NULL) {
		printf("Error: %s: %d:No Data found.\n", __func__, __LINE__);
		return 0;
	}
	
	len = strlen(strm_data_p) / 2;
	// Convert Text Data to bin data
	strconvert2bin(strm_data_p, len, buf);			
	
	while(1) {

		// Set Descriptor
		desc[desc_id].desc[0] = 0x82300300;
		if((int)(buf[0] & 0x0f) > 5) {     // NAL Unit Type
			desc[desc_id].desc[0] |= 0x10; // Flag of non frame data
		}			
		desc[desc_id].desc[1] = (u32) MMAP_OFFSET; // physcal address
		desc[desc_id].desc[2] = rand() % len + 1;
		desc[desc_id].desc[3] = desc[desc_id].desc[2] - 1;

		FD_ZERO(&sfd);
		FD_SET(fd, &sfd);

        // RTP offload
		write(fd, &desc[desc_id], sizeof(struct netsec_desc));
		
		while(1) {
			if(rxq.rxque[r].length > 0) break;
			usleep(100);
		}
		
		// Send RTP packet on UDP
		while(rxq.rxque[r].length > 0) {

			sendto(destSocket,
				   (const void *) rxq.rxque[r].rx_buff,
				   (size_t) rxq.rxque[r].length,
				   0,
				   (const struct sockaddr *)&destSockAddr,
				   (socklen_t)sizeof(destSockAddr));
			
			rxq.rxque[r].length = 0;
			r++;
			if(r >= NUM_RXQ) r = 0;

			usleep(20000); // Delay to Compare in the opponent
		}

		desc_id++;
		if(desc_id > 16) {
			desc_id = 0;
		}

	}
	
 	free(buf);	
	fclose(fp);
	close(fd);

	return 0;
}
