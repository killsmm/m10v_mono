//******************************************************************************/
//
//  @file   net_init.c
//  @brief  netsec initialization sample app
//
//
//  Copyright 2015 Socionext Inc.
//******************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "mil_netsec_api.h"

#define MIL_CTL_PROC_OK 							(0)		/*!< Return OK	*/
#define MIL_CTL_PROC_NG 							(-1)	/*!< Return NG	*/
#define NETSEC_NAME		"/dev/netsec0"

// MicroCode File
#define DMAC_HM_ME_CODE_FILE						"/root/og_hm_core.bin"
#define DMAC_MH_ME_CODE_FILE						"/root/og_mh_core.bin"
#define PACKETME_CODE_FILE							"/root/og_pk_core.bin"

typedef enum
{
	MIL_NETSEC_DESCRIPTOR_PKT_RX = 0,
	MIL_NETSEC_DESCRIPTOR_ETHER_LOOPBACK,
	MIL_NETSEC_DESCRIPTOR_RTP_LOOPBACK,
	MIL_NETSEC_DESCRIPTOR_CRYPT_RX,
	MIL_NETSEC_DESCRIPTOR_PKT_TX,
	MIL_NETSEC_DESCRIPTOR_VIDEO_TX,
	MIL_NETSEC_DESCRIPTOR_AUDIO_TX,
	MIL_NETSEC_DESCRIPTOR_CONTEXT_TX,
	MIL_NETSEC_DESCRIPTOR_CRYPT_TX,
	MIL_NETSEC_DESCRIPTOR_JPEG1_TX,
	MIL_NETSEC_DESCRIPTOR_JPEG2_TX,
	MIL_NETSEC_DESCRIPTOR_VIDEO_SUB_TX,
	MIL_NETSEC_DESCRIPTOR_METADATA_TX,
	MIL_NETSEC_DESCRIPTOR_TS_TX,

	MIL_NETSEC_DESCRIPTOR__COUNT
} E_MIL_NETSEC_DESCRIPTOR_INDEX;

#define	NUM_RXQ			(256)
#define	HOST_FW_INIT_OK	(0x1)
struct	_rxqueue {
	u32		num;
	u32		wr_count;
	u32		reserved[2];

	NETSEC_RX_DATA	rxque [NUM_RXQ];
}			rxq;



int sucCPUCtl_proc_getMeCode(char *fileName, uint **MEcodeAddr,uint *MEcodeSize)
{
	int			fd;
	struct stat		stat;
	char			*p_text = NULL;
	size_t			size;


	/* Open MicroCode File */
	fd = open(fileName , O_RDONLY);
	if( MIL_CTL_PROC_NG == fstat( fd, &stat ) )
	{
		return MIL_CTL_PROC_NG;
	}

	size =  (stat.st_size+3)/4 ;

	p_text = malloc( size * 4 );

	read(fd,p_text,stat.st_size);

	if( MIL_CTL_PROC_NG == close( fd ) ){
		return MIL_CTL_PROC_NG;
	}

	*MEcodeAddr = (uint*)p_text;
	*MEcodeSize	= (uint)size;

	return MIL_CTL_PROC_OK;
}

int main(int argc, char *argv[])
{
	int		 				fd_netsec;
	int					retVal  = MIL_CTL_PROC_OK;
	int					retVal1 = MIL_CTL_PROC_OK;
	int					retVal2 = MIL_CTL_PROC_OK;
	int					retVal3 = MIL_CTL_PROC_OK;
	int					i;
	struct netsec_init_arg		args;
	int ret;
	
	ret = 0;
	fd_netsec = open( NETSEC_NAME , O_RDWR);
	
	if(  MIL_CTL_PROC_NG == fd_netsec )
	{
		retVal =  MIL_CTL_PROC_NG;
		printf("%s:%d\n", __func__, __LINE__);
		return retVal;
	}
	
	args.num_descriptors[MIL_NETSEC_DESCRIPTOR_PKT_RX] = 1024;
	args.num_descriptors[MIL_NETSEC_DESCRIPTOR_ETHER_LOOPBACK] = 64;
	args.num_descriptors[MIL_NETSEC_DESCRIPTOR_RTP_LOOPBACK] = 64;
	args.num_descriptors[MIL_NETSEC_DESCRIPTOR_CRYPT_RX] = 64;
	args.num_descriptors[MIL_NETSEC_DESCRIPTOR_PKT_TX] = 64;
	args.num_descriptors[MIL_NETSEC_DESCRIPTOR_VIDEO_TX] = 450;
	args.num_descriptors[MIL_NETSEC_DESCRIPTOR_AUDIO_TX] = 64;
	args.num_descriptors[MIL_NETSEC_DESCRIPTOR_CONTEXT_TX] = 64;
	args.num_descriptors[MIL_NETSEC_DESCRIPTOR_CRYPT_TX] = 64;
	args.num_descriptors[MIL_NETSEC_DESCRIPTOR_JPEG1_TX] = 150;
	args.num_descriptors[MIL_NETSEC_DESCRIPTOR_JPEG2_TX] = 64;
	args.num_descriptors[MIL_NETSEC_DESCRIPTOR_VIDEO_SUB_TX] = 64;
	args.num_descriptors[MIL_NETSEC_DESCRIPTOR_METADATA_TX] = 64;
	args.num_descriptors[MIL_NETSEC_DESCRIPTOR_TS_TX] = 64;

	for( i = 0;i<4;i++ )
	{
		args.rx_buf_size[i] = 0x05f0;
	}
	args.size_ctx			= 0x2000;
	rxq.num = NUM_RXQ;
	args.lpbk_rx_que		= (NETSEC_RX_QUE *) &rxq;
	//Set MAC Address. Now set MAC Address to local
	args.mac[0]				= 0x02;//0x08;
	args.mac[1]				= 0x00;
	args.mac[2]				= 0x70;
	args.mac[3]				= 0x98;
	args.mac[4]				= 0x76;
	args.mac[5]				= 0x54;

	args.dmac_hm_me_code = NULL;
	retVal1 = sucCPUCtl_proc_getMeCode( DMAC_HM_ME_CODE_FILE, &(args.dmac_hm_me_code), &(args.dmac_hm_me_size) );

	args.dmac_mh_me_code = NULL;
	retVal2 = sucCPUCtl_proc_getMeCode( DMAC_MH_ME_CODE_FILE, &(args.dmac_mh_me_code), &(args.dmac_mh_me_size) );

	args.pktc_code = NULL;
	retVal3 = sucCPUCtl_proc_getMeCode( PACKETME_CODE_FILE, &(args.pktc_code), &(args.pktc_size) );
	
	/* Failed to Read */
	if(  ( MIL_CTL_PROC_OK != retVal1 ) ||
		 ( MIL_CTL_PROC_OK != retVal2 ) ||
		 ( MIL_CTL_PROC_OK != retVal3 )  )
	{
		retVal =  MIL_CTL_PROC_NG;
	}
	else
	{
		ioctl(fd_netsec, NETSEC_IOCINIT, &args);
	}

	close(fd_netsec);
	return ret;
}
