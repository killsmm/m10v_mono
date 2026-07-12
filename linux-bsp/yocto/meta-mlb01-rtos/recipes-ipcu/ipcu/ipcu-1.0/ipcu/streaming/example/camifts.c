/******************************************************************************/
/**
 *  @file   camifts.c
 *  @brief  test sample
 */
/*
 *  Copyright 2017 Socionext Inc.
 ******************************************************************************/
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
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>


#define RID0 0 
#define RID1 1
#define RID2 2
#define RID3 3
#define RMAX 4
#define RAPTM(x,y) \
	clock_gettime(CLOCK_REALTIME, &ge[y]); \
	x = ge[y].tv_nsec - gs[y].tv_nsec; \
	if( x < 0 ) x += 1000000000; \
	printf("%s\t:%d\n",__func__,x/1000000); \
	clock_gettime(CLOCK_REALTIME, &gs[y]);

static int app_abort = 0;
static int out_fd = -1;
static int extent_opt;
static int save_mode = 0;
#define SAVE_MODE_ALL   0
#define SAVE_MODE_VIDEO 1
#define SAVE_MODE_YUV   2
#define SAVE_MODE_AUDIO 3
#define SAVE_MODE_RAW   4
#define SAVE_MODE_JPEG  5
#define USB_SAVE_FD "/media/usb/"

unsigned long vpa;
unsigned long vsz;
unsigned long ypa;
unsigned long ysz;
//for stdout
unsigned char opt_stream_id;
sem_t stdoutsem;
struct timespec gs[RMAX], ge[RMAX];//performance
static unsigned char rid = 0;
static unsigned char wid = 0;
// If a frame drop occurs, increase this value to 40
#define STDOUTIDNUM 10
char stdoutbuf[STDOUTIDNUM][2*1024*1024];
unsigned long stdoutlen[STDOUTIDNUM];

static void signal_handler(int signal)
{
	app_abort = 1;
}
static void sigterm_handler(int signal)
{
	app_abort = 1;
}

const char* help_msg[]=
{
	"Usage:",
	"  camifts <option>",
	"Options:",
	"  -r    :IPCU Rx ch(receive) default 6",
	"  -t    :IPCU Tx ch(response) default 7",
	"  -s    :enable simple interface(only video callback)",
	"  -f    :file save callback",
	"         It puts the received data to /tmp/video_xxx.",
	"  -e    :dump struct streaming IF to /tmp/sif. If no \"-f\" option,it is invalid",
	"  -p    :for \"-e\" option. sample_address is physical address ",
	"  -i    :show debug print. it is not working with \"-f,-e\"",
	"  -h    :help",
	//"  -v    : video stream to stdout",
	//"  -Num  : 1 upto 4, Select stream_id for stdout",
	//"          If you want to save to SD, camifts -v > /path/to/SD/filename",
	NULL,
};
void extent_filesave(char stream_id, unsigned long file_number, struct cif_stream_send* ptr)
{
	int* buf_vaddr = (int*)ptr;
	char filename[256];
	FILE *fp;

	{
		// streaming processing
		sprintf(filename, "%s_%02d_%04lu.txt", "/tmp/sif",stream_id, file_number);
		fp = fopen(filename, "w");
		if (fp < 0) {
			printf("%s: failed to open %s \n", __func__, filename);
		}
		else
		{
			for(int i = 0; i < sizeof(struct cif_stream_send) / 4; i++)
			{
				fprintf(fp,"[%02d]0x%08X \n",i, buf_vaddr[i]);
			}
			fclose(fp);
		}
	}
}
int printf_callback(struct cif_stream_send* p, void *d)
{
	void* buf_vaddr;
	void* buf_paddr;
	static unsigned long num = 0;
	int long msec;
	int size;
	cif_stream_meta_send* mp;
	unsigned char id = 0;
	clock_gettime(CLOCK_REALTIME, &ge[0]);
	msec = ge[0].tv_nsec - gs[0].tv_nsec;
	if( msec < 0 ) msec += 1000000000;

	if( p->Sub_Command != 0x10000 )
	{
		id = p->stream_id;
		size = p->sample_size;
		buf_vaddr = (UINT32 *)FJ_MM_phys_to_virt(p->sample_address);
		buf_paddr = (void*)(int)p->sample_address;
	}
	else
	{
		mp = (cif_stream_meta_send*)p;
		id = 90 + mp->stype;
		size = mp->sample_size;
		buf_vaddr = (UINT32 *)FJ_MM_phys_to_virt(mp->sample_address);
		buf_paddr = (void*)(int)mp->sample_address;
	}
	printf("%4lu:%2d P:%08lX V:%08lX SZ:%d pts[0]%08lX %lums\n",num,id,(unsigned long)buf_paddr,(unsigned long)buf_vaddr,
	size,p->media.v.sample_pts[0],msec/1000000);
	num++;

	clock_gettime(CLOCK_REALTIME, &gs[0]);

	return 0;
}
#if 0
void printf_meta( struct cif_meta_send* p )
{
	printf("meta rcv\n");
}
#endif
int filesave_callback(struct cif_stream_send* p, void *d)
{
	void* buf_vaddr;
	int addr;
	unsigned long length;
	static unsigned long file_number = 0;
	unsigned char stream_id = 0;
	char filename[256];
	//char filename2[256];
	cif_stream_meta_send* mp;
	srand(time(NULL));
	int r = rand()%10000;

	if( p->Sub_Command == 0x10000 )
	{
		mp = (cif_stream_meta_send*)p;
		length = mp->sample_size;
		stream_id = 90 + mp->stype;
		addr = mp->sample_address;
	}
	else
	{
		length = p->sample_size;
		stream_id = p->stream_id;
		addr = p->sample_address;
	}

	buf_vaddr = (void*)(int)addr;

	if (out_fd < 0)
	{
		file_number++;
		if( extent_opt == 1)
		{
			extent_filesave(stream_id, file_number, (struct cif_stream_send*)p);
		}
		printf("sub cmd:%lx\n", p->Sub_Command);
		switch( p->Sub_Command )
		{
			// streaming processing
			case 0x00000100:
				sprintf(filename, "%s_%02lu_%04lu", "/tmp/video",(unsigned long)stream_id, file_number);
				break;
			case 0x00000200:
				sprintf(filename, "%s_%04lu", "/tmp/audio", file_number);
				break;
			case 0x00000300:
				sprintf(filename, "%sraw%04d_%04lu.raw", USB_SAVE_FD, r, file_number);
				break;
			case 0x00000400:
				sprintf(filename, "%syuv%04d_%02lu_%04lu.yuv", USB_SAVE_FD, r, (unsigned long)stream_id, file_number);
				break;
			case 0x00000500:
				sprintf(filename, "%sjpg%04d_%02lu_%04lu.jpg", USB_SAVE_FD, r, (unsigned long)stream_id, file_number);
				break;
			case 0x00010000:
				sprintf(filename, "%s_%02lu_%04lu.bin", "/tmp/meta",(unsigned long)stream_id, file_number);
				break;
			default:
				sprintf(filename, "%s_id%lX_%04lu", "/tmp/unknown", p->Sub_Command, file_number);
			break;
		}

		printf("filename:%s\n", filename);

		out_fd = open(filename, (O_RDWR | O_CREAT | O_TRUNC), 0666);
		if (out_fd < 0) {
			printf("%s: failed to open %s \n", __func__, filename);
		}
		else
		{
				printf("%s: try to write %s 0x%08x(%04x)\n",
				__func__, (char*)&filename,(unsigned int)buf_vaddr,
				(unsigned int)length);
			if (write(out_fd, buf_vaddr, length) != length) {
				printf("%s: failed to write %s 0x%08x(%04x)\n",
				__func__, (char*)&filename,(unsigned int)buf_vaddr,
				(unsigned int)length);
			}
			close(out_fd);
			out_fd = -1;
		}

	}

	return 0;
}

int stdout_callback(struct cif_stream_send* p, void *d)
{
	void* buf_vaddr;
	unsigned long length;

	length = p->sample_size;

	buf_vaddr = (void*)(int)p->sample_address;

	if( opt_stream_id != 0xFF )
	{
		if( opt_stream_id == p->stream_id )
		{
			memcpy((char*)stdoutbuf[wid],buf_vaddr, length);
			sem_post(&stdoutsem);
			stdoutlen[wid] = length;
			wid++;
			if( wid >= STDOUTIDNUM ) {
				wid = 0;
			}
		}
	}
	else
	{
		memcpy((char*)stdoutbuf[wid],buf_vaddr, length);
		sem_post(&stdoutsem);
		stdoutlen[wid] = length;
		wid++;
		if( wid >= STDOUTIDNUM ) {
			wid = 0;
		}
	}

	return 0;
}
void stdout_send_thread(void* p)
{
	while( 1 )
	{
		sem_wait(&stdoutsem);
		write(1, (char*)stdoutbuf[rid], stdoutlen[rid]);
		rid++;
		if( rid >= STDOUTIDNUM ) {
			rid = 0;
		}
	}
}

int videocb(struct cif_stream_send* p, void *d)
{
	char filename[256];
	void* buf_vaddr;
	unsigned long addr;
	unsigned long length = 0;

	if( p->Sub_Command == 0x10000 ){
		printf("%s, sub command == 0x10000 return \n", __func__);
		return -1;
	}

	length = p->sample_size;
	addr = p->sample_address;

	buf_vaddr = (void*)(int)addr;

	switch( p->Sub_Command )
	{
		// streaming processing
		case 0x00000100:
			sprintf(filename, "/tmp/100");
			break;
		case 0x00000200:
			sprintf(filename, "/tmp/200");
			break;
		case 0x00000300:
			sprintf(filename, "/tmp/300");
			break;
		case 0x00000400:
			sprintf(filename, "/tmp/400");
			break;
		case 0x00000500:
			sprintf(filename, "/tmp/500");
			break;
		case 0x00010000:
		default:
		break;
	}

	out_fd = open(filename, (O_RDWR | O_CREAT | O_APPEND), 0666);
	if (out_fd < 0) {
		printf("%s: failed to open %s \n", __func__, filename);
	}
	else
	{
#if 0
			printf("%s: try to write %s 0x%08x(%04x)\n",
			__func__, &filename,(unsigned int)buf_vaddr,
			(unsigned int)length);
#endif
		if (write(out_fd, buf_vaddr, length) != length) {
			printf("%s: failed to write %s 0x%08x(%04x)\n",
			__func__, (char*)&filename,(unsigned int)buf_vaddr,
			(unsigned int)length);
		}
		close(out_fd);
		out_fd = -1;
	}


	return 0;
}
int audiocb(struct cif_stream_send* p, void *d)
{
		char filename[256];
		void* buf_vaddr;
		unsigned long addr;
		unsigned long length;

		if( p->Sub_Command == 0x10000 ){
			printf("%s, sub command == 0x10000 return \n", __func__);
			return -1;
		}
		length = p->sample_size;
		addr = p->sample_address;

		buf_vaddr = (UINT32 *)addr;

		sprintf(filename, "/tmp/audio");

		out_fd = open(filename, (O_RDWR | O_CREAT | O_APPEND), 0666);
		if (out_fd < 0) {
			printf("%s: failed to open %s \n", __func__, filename);
		}
		else
		{
			if (write(out_fd, buf_vaddr, length) != length) {
				printf("%s: failed to write %s 0x%08x(%04x)\n",
				__func__, (char*)&filename,(unsigned int)buf_vaddr,
				(unsigned int)length);
			}
			close(out_fd);
			out_fd = -1;
		}
	return 0;
}
int rawcb(struct cif_stream_send* p, void *d)
{
//	int long msec;
//	RAPTM(msec,RID3);
//	printf("%s %d\n",__func__,__LINE__);
	return 0;
}
int yuvcb(struct cif_stream_send* p, void *d)
{
//	int long msec;
//	RAPTM(msec,RID1);
//	if( (p->sample_address < (ypa + ysz) && p->sample_address < ypa) && mode_phys == 1 )
//	printf("YUV ADDRESS ERROR [%08x]\n",p->sample_address);
	return 0;
}
int jpegcb(struct cif_stream_send* p, void *d)
{
//	int long msec;
//	RAPTM(msec,RID0);
//	if( (p->sample_address < (vpa + vsz) && p->sample_address < vpa) && mode_phys == 1 )
	return 0;
}
int metacbch6(struct cif_stream_send* p, void *d)
{
	return 0;
}
pthread_t sendth;

/******************************************************************************/
/**
 *  @brief  sample_app
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
	int simpleif;
	int i;
	int shared_info;
	struct cb_main_func func;
	struct ipcu_param ipcu_prm;

	if (signal(SIGINT, signal_handler) == SIG_ERR)
	printf("SIGINT erro\n");
	if (signal(SIGTERM, sigterm_handler) == SIG_ERR)
	printf("SIGTERM erro\n");

	for(loop = 0; loop < RMAX; loop++) {
		clock_getres(CLOCK_REALTIME, &ge[RMAX]);
		clock_getres(CLOCK_REALTIME, &gs[RMAX]);
	}

	shared_info = 0;
	simpleif = 0;
	extent_opt = 0;
	func.video = (void*)videocb;
	func.audio = (void*)audiocb;
	func.raw   = (void*)rawcb;
	func.yuv   = (void*)yuvcb;
	func.jpeg  = (void*)jpegcb;
	func.meta  = (void*)metacbch6;
	ipcu_prm.chid = 6;
	ipcu_prm.ackid = 7;
	ipcu_prm.sw = 0;
	opt_stream_id = 0xFF;
	while ((opt = getopt(argc, argv, "012345678vedsfhr:t:c:")) != -1)
	{
		switch (opt)
		{
		case 't':
			ipcu_prm.ackid = atoi(optarg);
			printf("Stream Response ch %d\n",ipcu_prm.ackid);
			break;
		case 'r':
			ipcu_prm.chid = atoi(optarg);
			printf("Stream Receive ch %d\n",ipcu_prm.chid);
			break;
		case 'e':
			extent_opt = 1;
			break;
		case 's':
			simpleif = 1;
			break;
		case 'f':
			/* if simpleif is 1, audio and raw callback are not working */
			func.video = (void*)filesave_callback;
			func.audio = (void*)filesave_callback;
			func.raw   = (void*)filesave_callback;
			func.yuv   = (void*)filesave_callback;
			func.jpeg   = (void*)filesave_callback;
			func.meta   = (void*)filesave_callback;
			printf("%s %d Dump to /tmp/xxx \n",__func__,__LINE__);
			break;
		case 'c':
			save_mode = atoi(optarg);
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case 'v':
			if( opt == 'v' ){
				opt_stream_id = 0xFF;
			printf("%s %d DEBUGt \n",__func__,opt_stream_id);
			}else{
			opt_stream_id = (opt & 0x0F);
			}
			func.video = (void*)stdout_callback;
			sem_init(&stdoutsem, 0, 1);
			pthread_create(&sendth, NULL, (void*)stdout_send_thread, (void*)NULL);
			printf("%s %d Video to stdout \n",__func__,__LINE__);
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
	
	// if "f" is set and save_mode is not 0, disable callback and then targeand then the target format stores to file.
	if( save_mode == SAVE_MODE_VIDEO )
	{
		printf("file dump is only video callback\n");
		func.audio = (void*)audiocb;
		func.raw   = (void*)rawcb;
		func.yuv   = (void*)yuvcb;
		func.jpeg   = (void*)jpegcb;
	}
	else if( save_mode == SAVE_MODE_YUV )
	{
		printf("file dump is only YUV callback\n");
		func.video = (void*)videocb;
		func.audio = (void*)audiocb;
		func.raw   = (void*)rawcb;
		func.jpeg   = (void*)jpegcb;
	}
	else if( save_mode == SAVE_MODE_AUDIO )
	{
		printf("file dump is only audio callback\n");
		func.video = (void*)videocb;
		func.raw   = (void*)rawcb;
		func.yuv   = (void*)yuvcb;
		func.jpeg   = (void*)jpegcb;
	}
	else if( save_mode == SAVE_MODE_RAW )
	{
		printf("file dump is only raw callback\n");
		func.video = (void*)videocb;
		func.audio = (void*)audiocb;
		func.yuv   = (void*)yuvcb;
		func.jpeg   = (void*)jpegcb;
	}
	else if( save_mode == SAVE_MODE_RAW )
	{
		printf("file dump is only jpeg callback\n");
		func.video = (void*)videocb;
		func.raw   = (void*)rawcb;
		func.audio = (void*)audiocb;
		func.yuv   = (void*)yuvcb;
	}
	else
	{
		//
	}
	func.user_data = NULL;

	ipcu_prm.sw = simpleif;
	Stream_ipcu_ch_init(&func, ipcu_prm);

	if( shared_info == 1 )
	{
		printf("-- Shared address Check --\n");
		for( i = 0; i < FJ_MM_Get_SharedMemory_Max(); i++ )
		{
			FJ_MM_getPhysAddrInfo(i, (UINT32 *)&vpa, (UINT32 *)&vsz);
			printf("addr:0x%08lX ,size:0x%08lX\n",vpa,vsz);
		}
	}

	/* Main Loop */
	while (!app_abort) {
		usleep(100000);
	}

	Stream_ipcu_ch_close();
	if (out_fd >= 0) {
		close(out_fd);
	}
	if( sendth != 0 )
	{
		sem_destroy(&stdoutsem);
		pthread_cancel(sendth);
		pthread_join(sendth, NULL);
		sendth = 0;
	}
	return 0;
}
