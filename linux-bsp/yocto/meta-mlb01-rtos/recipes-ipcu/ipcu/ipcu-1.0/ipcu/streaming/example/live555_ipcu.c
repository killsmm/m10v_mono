/******************************************************************************
  @file   live555_ipcu.c
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
#include <sys/msg.h>
#include <sys/stat.h>

#define MAX_TEXT 4
#define MSGKEY_HEVC 0x9527
#define MSGKEY_H264 0x5487
#define MSGKEY_D1	0x6833
#define MSGKEY_HEVC_AUDIO 0x7895
#define MSGKEY_H264_AUDIO 0x5987
#define MSGKEY_CATCMD 0x9525
#define MSGKEY_AUDIO_CATCMD 0x9535
#define MSGKEY_AUDIO_SAMPLE 0x9536

struct msg_que_st
{
	long msg_type;
	unsigned long phy_address;
	unsigned long frame_size;
	long ctrl_parameter;
	unsigned long long timestamp;
	unsigned long long frame_no;
	char content_text[MAX_TEXT];
};

struct msg_que_st_a
{
	long msg_type;
	unsigned long phy_address;
	unsigned long frame_size;
	long ctrl_parameter;
	unsigned long long timestamp;
	unsigned long long frame_no;
	unsigned long audio_samprate;
	unsigned long ch;
	char content_text[MAX_TEXT];
};

struct msg_que_cmd
{
	long msg_type;
	unsigned long cmd;
	unsigned long sub;
	unsigned long data;
	char content_text[MAX_TEXT];
};

struct msg_que_audio_sample
{
	long msg_type;
	unsigned long samplingrate;
	char content_text[MAX_TEXT];
};

static struct msg_que_st hevc_que_data;
static struct msg_que_st h264_que_data;
static struct msg_que_st d1_que_data;
static struct msg_que_st_a hevc_audio_que_data;
static struct msg_que_st_a h264_audio_que_data;
static struct msg_que_cmd g_catcmd_que_data;
static struct msg_que_audio_sample g_audio_sample;

struct msqid_ds msq_info;

static int msgid_hevc;
static int msgid_h264;
static int msgid_d1;
static int msgid_hevc_audio;
static int msgid_h264_audio;
static int msgid_catcmd;
static int msgid_audio_catcmd;
static int msgid_audio_samplingrate;

static unsigned char enable_hevc;
static unsigned char enable_h264;
static unsigned char enable_d1;
static unsigned char enable_hevc_audio = 0;
static unsigned char enable_h264_audio = 0;
static unsigned char hevc_vps_flag;
static unsigned char h264_vps_flag;
static unsigned char d1_vps_flag;

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

int gSem_id;

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

void Init_IPC()
{
	enable_hevc = 0;
	enable_h264 = 0;
	enable_hevc_audio = 0;
	enable_h264_audio = 0;
	hevc_vps_flag = 0;
	h264_vps_flag = 0;

	msgid_hevc = msgget(MSGKEY_HEVC, 0666 | IPC_CREAT);

	if(msgid_hevc == -1)
	{
		fprintf(stderr, "msgget msgid_hevc failed with error: %d\n", errno);
		printf("%s, %d, msgget msgid hevc failed\n", __func__, __LINE__);
		exit(EXIT_FAILURE);
	}

	msgid_h264 = msgget(MSGKEY_H264, 0666 | IPC_CREAT);

	if(msgid_h264 == -1)
	{
		fprintf(stderr, "msgget msgid_h264 failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	msgid_d1 = msgget(MSGKEY_D1, 0666 | IPC_CREAT);

	if(msgid_d1 == -1)
	{
		fprintf(stderr, "msgget msgid_d1 failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	msgid_h264_audio = msgget(MSGKEY_H264_AUDIO, 0666 | IPC_CREAT);

	if(msgid_h264_audio == -1)
	{
		fprintf(stderr, "msgget msgid_h264_audio failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	msgid_hevc_audio = msgget(MSGKEY_HEVC_AUDIO, 0666 | IPC_CREAT);

	if(msgid_hevc_audio == -1)
	{
		fprintf(stderr, "msgget msgid_hevc_audio failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	msgid_catcmd = msgget(MSGKEY_CATCMD, 0666 | IPC_CREAT);

	if(msgid_catcmd == -1)
	{
		fprintf(stderr, "msgget msgid_catcmd failed with error: %d\n", errno);
		printf("%s, %d, msgget msgid_catcmd failed\n", __func__, __LINE__);
		exit(EXIT_FAILURE);
	}

	msgid_audio_catcmd = msgget(MSGKEY_AUDIO_CATCMD, 0666 | IPC_CREAT);

	if(msgid_audio_catcmd == -1)
	{
		fprintf(stderr, "msgget msgid_catcmd failed with error: %d\n", errno);
		printf("%s, %d, msgget msgid_catcmd failed\n", __func__, __LINE__);
		exit(EXIT_FAILURE);
	}

	msgid_audio_samplingrate = msgget(MSGKEY_AUDIO_SAMPLE, 0666 | IPC_CREAT);

	if (msgid_audio_samplingrate == -1) {
		printf("%s, %d, msgget failed \n", __func__, __LINE__);
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
}

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
	//semapho_operation( gSem_id, UNLOCK );
}

static void sigterm_handler(int signal)
{
	app_abort = 1;
	//semapho_operation( gSem_id, UNLOCK );
}

/******************************************************************************
   @brief  jpeglpbcb
   @fn void jpeglpbcb(struct cif_stream_send* p)
   @param  [in]    *p      : callback parameter from RTOS
   @note
    The function is called by IPCU receiving notification.
   @author
   @version
*****************************************************************************/
int jpeglpbcb(struct cif_stream_send* p)
{
	//semapho_operation( gSem_id, UNLOCK );
	return 0;
}

/******************************************************************************
   @brief  audiolpbcb
   @fn void audiolpbcb(struct cif_stream_send* p)
   @param  [in]    *p      : callback parameter from RTOS
   @note
    The function is called by IPCU receiving notification.
   @author
   @version
*****************************************************************************/
int audiolpbcb(struct cif_stream_send* p)
{
	unsigned int loss_audio;

	if(msgrcv(msgid_hevc_audio, (void *)&hevc_audio_que_data, sizeof(struct msg_que_st_a) - sizeof(long), 4, IPC_NOWAIT) != -1)
	{
		if(hevc_audio_que_data.ctrl_parameter == 0x1234)
		{
			enable_hevc_audio = 1;
		}
		else if(hevc_audio_que_data.ctrl_parameter == 0x5678)
		{
			enable_hevc_audio = 0;
		}
		printf("audiolpbcb(), enable_hevc_audio:%d\n", enable_hevc_audio);
	}

	if(enable_hevc_audio)
	{
		memset(hevc_audio_que_data.content_text, '\0', sizeof(MAX_TEXT));
		hevc_audio_que_data.msg_type = 2;
		hevc_audio_que_data.phy_address = p->sample_address;
		hevc_audio_que_data.frame_size = p->sample_size;
		hevc_audio_que_data.timestamp = (unsigned long long)(((unsigned long long)(p->media.a.time_stamp[1]) << 32) | ((unsigned long long)p->media.a.time_stamp[0]));
		hevc_audio_que_data.frame_no = p->media.a.frame_no;
		hevc_audio_que_data.audio_samprate = p->media.a.sampling_freq;
		hevc_audio_que_data.ch = p->media.a.channel;
		if(msgsnd(msgid_hevc_audio, &hevc_audio_que_data, sizeof(struct msg_que_st_a) - sizeof(long), IPC_NOWAIT) == -1)
		{
			loss_audio = 0;

			if(msgctl(msgid_hevc_audio, IPC_STAT, &msq_info) < 0)
			{
				printf("msgctl hevc audio failed with error: %d\n", errno);
			}

			enable_hevc_audio = 0;

			do{
				msgrcv(msgid_hevc_audio, &hevc_audio_que_data, sizeof(struct msg_que_st) - sizeof(long), 2, IPC_NOWAIT);
				loss_audio++;
			}while(loss_audio < msq_info.msg_qnum);

			printf("%s, %d, loss_audio:%d, msq_info.msg_qnum:%d \n", __func__, __LINE__, loss_audio, (int)msq_info.msg_qnum);
			fprintf(stderr, "====== netsec msgsnd hevc audio failed ======\n");
		}
	}

	if(msgrcv(msgid_h264_audio, (void *)&h264_audio_que_data, sizeof(struct msg_que_st_a) - sizeof(long), 4, IPC_NOWAIT) != -1)
	{
		if(h264_audio_que_data.ctrl_parameter == 0x1234)
		{
			enable_h264_audio = 1;
		}
		else if(h264_audio_que_data.ctrl_parameter == 0x5678)
		{
			enable_h264_audio = 0;
		}
		printf("audiolpbcb(), enable_h264_audio:%x\n", enable_h264_audio);
	}

	if(enable_h264_audio)
	{
		memset(h264_audio_que_data.content_text, '\0', sizeof(MAX_TEXT));
		h264_audio_que_data.msg_type = 2;
		h264_audio_que_data.phy_address = p->sample_address;
		h264_audio_que_data.frame_size = p->sample_size;
		h264_audio_que_data.timestamp = (unsigned long long)(((unsigned long long)(p->media.a.time_stamp[1]) << 32) | ((unsigned long long)p->media.a.time_stamp[0]));
		h264_audio_que_data.audio_samprate = p->media.a.sampling_freq;
		h264_audio_que_data.frame_no = p->media.a.frame_no;
		h264_audio_que_data.ch = p->media.a.channel;

		//printf("audiolpbcb(), addr:%x, size:%x\n", h264_audio_que_data.phy_address, h264_audio_que_data.frame_size);

		if(msgsnd(msgid_h264_audio, &h264_audio_que_data, sizeof(struct msg_que_st_a) - sizeof(long), IPC_NOWAIT) == -1)
		{
			loss_audio = 0;

			if(msgctl(msgid_h264_audio, IPC_STAT, &msq_info) < 0)
			{
				printf("msgctl h264 audio failed with error: %d\n", errno);
			}

			enable_h264_audio = 0;

			do{
				msgrcv(msgid_h264_audio, &h264_audio_que_data, sizeof(h264_audio_que_data) - sizeof(long), 2, IPC_NOWAIT);
				loss_audio++;
			}while(loss_audio < msq_info.msg_qnum);
			printf("%s, %d, loss_audio:%d, msq_info.msg_qnum:%d \n", __func__, __LINE__, loss_audio, (int)msq_info.msg_qnum);
			fprintf(stderr, "====== netsec msgsnd h264 audio failed ======\n");
		}
	}

	//semapho_operation( gSem_id, UNLOCK );
	return 0;
}

/******************************************************************************
   @brief  videolpbcb
   @fn void videolpbcb(struct cif_stream_send* p)
   @param  [in]    *p      : callback parameter from RTOS
   @note
    The function is called by IPCU receiving notification.
   @author
   @version
*****************************************************************************/
int videolpbcb(struct cif_stream_send* p)
{
	unsigned int loss_frame;

	#ifdef PERF_ENABLE
	perf_start();
	#endif

	if( p->stream_id == 8 )
	{
#if 0
		if(msgrcv(msgid_hevc, (void *)&hevc_que_data, sizeof(struct msg_que_st) - sizeof(long), 4, IPC_NOWAIT) != -1)
		{
			if(hevc_que_data.ctrl_parameter == 0x1234)
			{
				enable_hevc = 1;				
			}
			else if(hevc_que_data.ctrl_parameter == 0x5678)
			{
				enable_hevc = 0;
				hevc_vps_flag = 0;
			}
		}

		if(enable_hevc == 1 && hevc_vps_flag != 1)
		{
			//printf("%s, %d, p->media.v.frame_of_GOP:%d\n", __func__, __LINE__, p->media.v.frame_of_GOP);
			if(p->media.v.frame_of_GOP == 0)
			{
				printf("%s, %d, gop=0, set hevc_vps_flag as 1\n", __func__, __LINE__);
				hevc_vps_flag = 1;
			}
		}
#endif
		if(1/*hevc_vps_flag*/)
		{
			memset(hevc_que_data.content_text, '\0', sizeof(MAX_TEXT));
			hevc_que_data.msg_type = 2;
			hevc_que_data.phy_address = p->sample_address;
			hevc_que_data.frame_size = p->sample_size;
			hevc_que_data.timestamp = (unsigned long long)(((unsigned long long)(p->media.v.sample_pts[1]) << 32) | ((unsigned long long)p->media.v.sample_pts[0]));
			hevc_que_data.frame_no = p->media.v.frame_no;
			//printf("%s, %d, \n", __func__, __LINE__);
			if(msgsnd(msgid_hevc, &hevc_que_data, sizeof(struct msg_que_st) - sizeof(long), IPC_NOWAIT) == -1)
			{
				loss_frame = 0;
				//printf("%s, %d\n", __func__, __LINE__);
				if(msgctl(msgid_hevc, IPC_STAT, &msq_info) < 0)
				{
					printf("msgctl failed with error: %d\n", errno);
				}

				enable_hevc = 0;
				hevc_vps_flag = 0;

				do{
					msgrcv(msgid_hevc, &hevc_que_data, sizeof(struct msg_que_st) - sizeof(long), 2, IPC_NOWAIT);
					loss_frame++;
				}while(loss_frame < msq_info.msg_qnum);

				printf("%s, %d, loss_frmae:%d, msq_info.msg_qnum:%lx \n", __func__, __LINE__, loss_frame, msq_info.msg_qnum);
				fprintf(stderr, "====== netsec msgsnd hevc failed ======\n");
			}
		}
	}

	if( p->stream_id == 0 )
	{
#if 0
		if(msgrcv(msgid_h264, (void *)&h264_que_data, sizeof(h264_que_data) - sizeof(long), 4, IPC_NOWAIT) != -1)
		{
			if(h264_que_data.ctrl_parameter == 0x1234)
			{
				enable_h264 = 1;	
			}
			else if(h264_que_data.ctrl_parameter == 0x5678)
			{
				enable_h264 = 0;
			    h264_vps_flag = 0;	
			}
		}

		if(enable_h264 == 1 && h264_vps_flag != 1)
		{
			if(p->media.v.frame_of_GOP == 1)
			{
				h264_vps_flag = 1;
			}
		}
#endif
		if(1/*h264_vps_flag*/)
		{					
			memset(h264_que_data.content_text, '\0', sizeof(MAX_TEXT));
			h264_que_data.msg_type = 2;
			h264_que_data.phy_address = p->sample_address;
			h264_que_data.frame_size = p->sample_size;
			h264_que_data.timestamp = (unsigned long long)(((unsigned long long)(p->media.v.sample_pts[1]) << 32) | ((unsigned long long)p->media.v.sample_pts[0]));
			h264_que_data.frame_no = p->media.v.frame_no;

			if(msgsnd(msgid_h264, &h264_que_data, sizeof(struct msg_que_st) - sizeof(long), IPC_NOWAIT) == -1)
			{
				loss_frame = 0;

				if(msgctl(msgid_h264, IPC_STAT, &msq_info) < 0)
				{
					printf("msgctl failed with error: %d\n", errno);
				}

				enable_h264 = 0;
			    h264_vps_flag = 0;

				do{
					msgrcv(msgid_h264, &h264_que_data, sizeof(h264_que_data) - sizeof(long), 2, IPC_NOWAIT);
					loss_frame++;
				}while(loss_frame < msq_info.msg_qnum);
				printf("%s, %d, loss_frmae:%d, msq_info.msg_qnum:%lx \n", __func__, __LINE__, loss_frame, msq_info.msg_qnum);
				fprintf(stderr, "====== netsec msgsnd h264 failed ======\n");
			}
		}
	}

	if( p->stream_id == 1 )
	{
#if 0
		if(msgrcv(msgid_d1, (void *)&d1_que_data, sizeof(d1_que_data) - sizeof(long), 4, IPC_NOWAIT) != -1)
		{
			if(d1_que_data.ctrl_parameter == 0x1234)
			{
				enable_d1 = 1;
			}
			else if(d1_que_data.ctrl_parameter == 0x5678)
			{
				enable_d1 = 0;
			    d1_vps_flag = 0;
			}
		}

		if(enable_d1 == 1 && d1_vps_flag != 1)
		{
			if(p->media.v.frame_of_GOP == 1)
			{
				d1_vps_flag = 1;
			}
		}
#endif
		if(1/*d1_vps_flag*/)
		{
			memset(&d1_que_data, 0, sizeof(d1_que_data));
			d1_que_data.msg_type = 2;
			d1_que_data.phy_address = p->sample_address;
			d1_que_data.frame_size = p->sample_size;
			d1_que_data.timestamp = (unsigned long long)(((unsigned long long)(p->media.v.sample_pts[1]) << 32) | ((unsigned long long)p->media.v.sample_pts[0]));
			d1_que_data.frame_no = p->media.v.frame_no;

			if(msgsnd(msgid_d1, &d1_que_data, sizeof(struct msg_que_st) - sizeof(long), IPC_NOWAIT) == -1)
			{
				loss_frame = 0;

				if(msgctl(msgid_d1, IPC_STAT, &msq_info) < 0)
				{
					printf("msgctl d1 failed with error: %d\n", errno);
				}

				enable_d1 = 0;
			    d1_vps_flag = 0;

				do{
					msgrcv(msgid_d1, &d1_que_data, sizeof(d1_que_data) - sizeof(long), 2, IPC_NOWAIT);
					loss_frame++;
				}while(loss_frame < msq_info.msg_qnum);
				printf("%s, %d, loss_frmae:%d, msq_info.msg_qnum:%d \n", __func__, __LINE__, loss_frame, (int)msq_info.msg_qnum);
				fprintf(stderr, "====== netsec msgsnd d1 failed ======\n");
			}
		}
	}
	
	//semapho_operation( gSem_id, UNLOCK );

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
	struct cb_main_func func;
	struct ipcu_param ipcu_prm;
	struct camera_if_cmd cmd;
	int ret = 0;

	if (signal(SIGINT, signal_handler) == SIG_ERR)
	printf("SIGINT erro\n");
	if (signal(SIGTERM, sigterm_handler) == SIG_ERR)
	printf("SIGTERM erro\n");

	#ifdef PERF_ENABLE
	perf_init();
	#endif
	
	func.video = (void*)videolpbcb;
	func.audio = (void*)audiolpbcb;
	func.jpeg  = (void*)jpeglpbcb;
	func.raw   = NULL;
	func.yuv   = NULL;
	func.meta  = NULL;
	ipcu_prm.chid = 6;
	ipcu_prm.ackid = 7;
	ipcu_prm.phys = 1;
	ipcu_prm.sw = 0;

	/* Register Ctrl-C event handler */
	signal(SIGINT, signal_handler);

	ipcu_prm.sw = 0;
	Stream_ipcu_ch_init(&func, ipcu_prm);

	//gSem_id = semapho_init();

	//if(gSem_id == -1){
	//	return -1;
	//}

	Init_IPC();
	
	//semapho_operation( gSem_id, LOCK );
	
	/* Main Loop */
	while (!app_abort) {
#if 1
		if(msgrcv(msgid_catcmd, (void *)&g_catcmd_que_data, sizeof(struct msg_que_cmd) - sizeof(long), 4, IPC_NOWAIT) != -1) {
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = g_catcmd_que_data.cmd | 0x80000000;
			cmd.cmd = g_catcmd_que_data.sub;
			cmd.send_param[0] = g_catcmd_que_data.data;
			cmd.to_sec = 3;
			camera_if_command(&cmd);
			printf("CatCmd read, cmd:0x%x, sub:0x%x, data:0x%x\n", (unsigned int)g_catcmd_que_data.cmd, (unsigned int)g_catcmd_que_data.sub, (unsigned int)cmd.recv_param[0]);
			if (cmd.recv_param[0] != cmd.send_param[0]) {
				printf("CatCmd write, cmd:0x%x, sub:0x%x, data:0x%x\n", (unsigned int)g_catcmd_que_data.cmd, (unsigned int)g_catcmd_que_data.sub, (unsigned int)g_catcmd_que_data.data);
				memset(&cmd, 0, sizeof(cmd));
				cmd.cmd_set = g_catcmd_que_data.cmd;
				cmd.cmd = g_catcmd_que_data.sub;
				cmd.send_param[0] = g_catcmd_que_data.data;
				cmd.to_sec = 3;
				if (camera_if_command(&cmd) != 0 || cmd.recv_param[0] != 0) {
					printf("camera IF error.\n");
				}
			}
		}
		// To get audio samping rate, and send back to live555
		if(msgrcv(msgid_audio_catcmd, (void *)&g_catcmd_que_data, sizeof(struct msg_que_cmd) - sizeof(long), 4, IPC_NOWAIT) != -1) {
			memset(&cmd, 0, sizeof(cmd));
			cmd.cmd_set = g_catcmd_que_data.cmd;
			cmd.cmd = g_catcmd_que_data.sub;
			cmd.send_param[0] = g_catcmd_que_data.data;
			cmd.to_sec = 3;
			ret = camera_if_command(&cmd);
			if (ret == 0) {
				printf("CatCmd read, cmd:0x%x, sub:0x%x, data:0x%x\n", (unsigned int)g_catcmd_que_data.cmd, (unsigned int)g_catcmd_que_data.sub, (unsigned int)cmd.recv_param[0]);
				g_audio_sample.samplingrate = cmd.recv_param[0];
				g_audio_sample.msg_type = 4;
				if (msgsnd(msgid_audio_samplingrate, &g_audio_sample, sizeof(struct msg_que_audio_sample) - sizeof(long), IPC_NOWAIT) == -1) {
					printf("%s, %d, catcmd msgsnd enable failed\n", __func__, __LINE__);
				}
			}

		}
#endif
		usleep(10*1000);
		//semapho_operation( gSem_id, WAIT );
		//semapho_operation( gSem_id, LOCK );
	}

	Stream_ipcu_ch_close();
	//semapho_free( gSem_id );

	return 0;
}

