/*
 * ipcu_playstream_file.c
 *
 *  Created on: Mar 11, 2020
 *      Author: jc
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <cmfwk_ipcu.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define DIR_PATH "/run/SD/mmcblk0p1/stream"
#define D_MAX_FRAME_CACHE 300
#define PRELOAD_PHY_ADDR (0x4C82B180) // SDRAM_ADR_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM1_AREA
#define PAGE_SHIFT 12
#define PAGE_MASK  (~((1 << PAGE_SHIFT) - 1))

#define AUDIO_S_TYPE_AAC 0
#define AUDIO_S_TYPE_PCM 1

#define AUDIO_S_FMT_RAW 0
#define AUDIO_S_FMT_ADTS 1

typedef struct {
	UINT32 addr;
	UINT32 size;
} T_AUDIO_STREAM_INFO;

typedef struct {
	ULONG	magic;
	ULONG	fmt;
	ULONG	cmd;
	ULONG	sub;
	ULONG	prm1;
	ULONG	prm2;
	ULONG	prm3;
	ULONG	prm4;
	ULONG	area;
} T_IPCU_IF_REL;

typedef struct {
	ULONG	magic_code;
	ULONG	format_version;
	ULONG	cmd_set;
	ULONG	cmd;
} _T_CPUCMD_HEADER;

typedef struct {
	_T_CPUCMD_HEADER	t_head;
	ULONG	stype;			// aac or pcm
	ULONG	sformat;		// ES (raw) or TS (ADTS)
	ULONG	stream_id;
	ULONG	stream_end_flg;
	ULONG	area_index;		// frame no
	ULONG	sample_size;
	ULONG	sample_address;
} T_CPUCMD_AUDIO_DEC_STREAM;

static T_AUDIO_STREAM_INFO g_aud_stream[D_MAX_FRAME_CACHE];
static int g_mem_fd = -1;
static T_CPUCMD_AUDIO_DEC_STREAM ipcu_aud_header;

static void usage(const char *argv0)
{
	fprintf(stderr, "Usage: %s [options]\n", argv0);
	fprintf(stderr, "Available options are\n");
	fprintf(stderr, " -a			Streaming audio frame by frame from file to RTOS\n");
	fprintf(stderr, " -v			Streaming video frame by frame from file to RTOS\n");
	fprintf(stderr, " -t			ipcu transmit channel [0-15]\n");
	fprintf(stderr, " -r			ipcu receive channel [0-15]\n");
	fprintf(stderr, " -m			max frame to be transferred\n");
	fprintf(stderr, " -i			interval time to transfer each frame [us]\n");
	fprintf(stderr, " -h			Print this help screen and exit\n");
}

static void *mm_mmap(unsigned int paddr, unsigned int length)
{
	void *vaddr, *page_vaddr;
	unsigned int page_paddr;
	unsigned int page_offset;
	unsigned int page_length;

	page_paddr = paddr & PAGE_MASK;
	page_offset = paddr & ~PAGE_MASK;
	page_length = length + page_offset;

	page_vaddr = mmap(0, page_length, PROT_READ | PROT_WRITE,
			MAP_SHARED, g_mem_fd, page_paddr);

	vaddr =  (void*) (page_vaddr + page_offset);

	return vaddr;
}

static int mm_munmap(void *vaddr, unsigned int length)
{
	unsigned int page_vaddr;
	unsigned int page_offset;
	unsigned int page_length;

	page_vaddr = (unsigned int)vaddr & PAGE_MASK;
	page_offset = (unsigned int)vaddr & ~PAGE_MASK;
	page_length = length + page_offset;

	return munmap((void*)page_vaddr, page_length);
}

static void audio_rel_cb(UINT8 id,
	       UINT32 pdata,
	       UINT32 length,
	       UINT32 cont,
	       UINT32 total_length)
{
	T_IPCU_IF_REL* rel;

	rel = (T_IPCU_IF_REL*)pdata;

	if (rel->magic != D_CPU_IF_MCODE_STREAM_RELESE) {
		fprintf(stderr, "audio stream ack magic error\n");
	}

	if (rel->cmd != E_CAM_IF_COM_SET_STREAM_STR) {
		fprintf(stderr, "audio stream ack cmd error\n");
	}

	if (rel->sub != E_CAM_IF_SUB_COM_STREAM_AUDIO) {
		fprintf(stderr, "audio stream ack sub cmd error\n");
	}
}

static void video_rel_cb(UINT8 id,
	       UINT32 pdata,
	       UINT32 length,
	       UINT32 cont,
	       UINT32 total_length)
{
	T_IPCU_IF_REL* rel;

	rel = (T_IPCU_IF_REL*)pdata;

	if (rel->magic != D_CPU_IF_MCODE_STREAM_RELESE) {
		fprintf(stderr, "video stream ack magic error\n");
	}

	if (rel->cmd != E_CAM_IF_COM_SET_STREAM_STR) {
		fprintf(stderr, "video stream ack cmd error\n");
	}

	if (rel->sub != E_CAM_IF_SUB_COM_STREAM_VIDEO) {
		fprintf(stderr, "video stream ack sub cmd error\n");
	}
}

int main(int argc, char *argv[])
{
	int i, opt, ret = 0;
	int audio_on = 0;
	int video_on = 0;
	char file_path[64];
	unsigned int max_frame = 1;
	struct stat statbuf;
	FILE *fid;
	size_t count;
	void *buffer;
	unsigned int pbuf;
	UINT8 send_id, recv_id;
	E_FJ_IPCU_MAILBOX_TYPE send_ch = E_FJ_IPCU_MAILBOX_TYPE_11, recv_ch = E_FJ_IPCU_MAILBOX_TYPE_10;
	unsigned int interval = 0;

	while ((opt = getopt(argc, argv, "a:v:t:r:m:i:h")) != -1) {
		switch (opt) {
		case 'a':
			audio_on = 1;
			if (video_on) {
				fprintf(stderr, "a/v can not exist together\n");
				exit(EXIT_FAILURE);
			}
			ipcu_aud_header.t_head.magic_code = D_CPU_IF_MCODE_STREAM_SEND;
			ipcu_aud_header.t_head.format_version = D_CPU_IF_COM_FORMAT_VERSION;
			ipcu_aud_header.t_head.cmd_set = E_CAM_IF_COM_SET_STREAM_STR;
			ipcu_aud_header.t_head.cmd = E_CAM_IF_SUB_COM_STREAM_AUDIO;

			ipcu_aud_header.stype = AUDIO_S_TYPE_AAC;
			ipcu_aud_header.sformat = AUDIO_S_FMT_ADTS;
			ipcu_aud_header.stream_id = 0;
			break;
		case 'v':
			video_on = 1;
			if (audio_on) {
				fprintf(stderr, "a/v can not exist together\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'm':
			max_frame = atoi(optarg);
			if (max_frame > D_MAX_FRAME_CACHE) {
				fprintf(stderr, "over max frame\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 't':
			send_ch = (E_FJ_IPCU_MAILBOX_TYPE)atoi(optarg);
			break;
		case 'r':
			recv_ch = (E_FJ_IPCU_MAILBOX_TYPE)atoi(optarg);
			break;
		case 'i':
			interval = atoi(optarg);
			break;
		case 'h':
			usage(argv[0]);
			break;
		default:
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	/* Initialize IPCU */
	ret = FJ_IPCU_Open(send_ch, (UINT8*)&send_id);
	if (ret != FJ_ERR_OK) {
		FJ_IPCU_Close(send_id);
		fprintf(stderr, "ipcu ch:%d open failed%d\n", send_ch, ret);
		exit(EXIT_FAILURE);
	}

	ret = FJ_IPCU_Open(recv_ch, (UINT8*)&recv_id);
	if (ret != FJ_ERR_OK) {
		FJ_IPCU_Close(recv_id);
		fprintf(stderr, "ipcu ch:%d open failed%d\n", recv_ch, ret);
		exit(EXIT_FAILURE);
	}

	ret = FJ_IPCU_SetReceiverCB(recv_id, audio_on ? audio_rel_cb : video_rel_cb);
	if (ret != FJ_ERR_OK) {
		FJ_IPCU_Close(send_id);
		FJ_IPCU_Close(recv_id);
		fprintf(stderr, "FJ_IPCU_SetReceiverCB failed%d\n", ret);
		exit(EXIT_FAILURE);
	}

	g_mem_fd = open("/dev/mem", (O_RDWR | O_SYNC));
	if (g_mem_fd < 0) {
		printf("%s: failed to open /dev/mem \n", __func__);
		exit(EXIT_FAILURE);
	}

	pbuf = PRELOAD_PHY_ADDR;

	if (audio_on) {
		// pre-load audio stream from file
		for (i = 0; i < max_frame; i++) {
			sprintf( file_path, "%s/test%04d.aac", DIR_PATH, (INT32)i );

			if (stat(file_path, &statbuf) == -1) {
				handle_error("fstat");
			}

			if ((fid = fopen(file_path, "r")) == NULL) {
				handle_error("fopen");
			}

			buffer = mm_mmap(pbuf, statbuf.st_size);
			if ((count = fread(buffer, sizeof(char), statbuf.st_size, fid)) != statbuf.st_size) {
				handle_error("fread");
			}
			mm_munmap(buffer, statbuf.st_size);

			if (fclose(fid) != 0) {
				handle_error("fclose");
			}

			g_aud_stream[i].addr = pbuf;
			g_aud_stream[i].size = statbuf.st_size;
			pbuf += statbuf.st_size;
		}
		// sending audio stream to RTOS
		for (i = 0; i < max_frame; i++) {
			ipcu_aud_header.area_index = i;
			ipcu_aud_header.stream_end_flg = i == (max_frame - 1) ? 1 : 0;
			ipcu_aud_header.sample_address = g_aud_stream[i].addr;
			ipcu_aud_header.sample_size = g_aud_stream[i].size;

			ret = FJ_IPCU_Send(send_id,
					   (UINT32)&ipcu_aud_header, sizeof(ipcu_aud_header),
					   IPCU_SEND_SYNC);
			if (ret != FJ_ERR_OK) {
				FJ_IPCU_Close(send_id);
				FJ_IPCU_Close(recv_id);
				fprintf(stderr, "FJ_IPCU_Send failed %d\n", ret);
				exit(EXIT_FAILURE);
			}
			usleep(interval);
		}
	}

	FJ_IPCU_Close(send_id);
	FJ_IPCU_Close(recv_id);

	exit(EXIT_SUCCESS);
}
