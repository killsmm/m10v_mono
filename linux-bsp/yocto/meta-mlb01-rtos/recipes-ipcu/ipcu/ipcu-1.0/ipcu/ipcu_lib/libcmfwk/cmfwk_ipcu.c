/******************************************************************************/
/**
 *  @file   cmfwk_ipcu.c
 *  @brief  IPCU Framework
 */
/*
 *  Copyright 2015 Socionext Inc.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include "cmfwk_mm.h"
#include "cmfwk_ipcu.h"
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
char syslogstr[128];

#ifdef IPCU_QUEUING
#define IPCU_Q_NUM 128
struct q_t {
	unsigned char r;
	unsigned char w;
	unsigned char num;
	unsigned char reserved;
	pthread_cond_t  not_empty;
	pthread_cond_t  not_full;
	pthread_mutex_t mutex;
	T_IPCU_IF buf[IPCU_Q_NUM];
};
#endif

struct sn_send_thread_param {
	UINT8       channel_id;
	UINT32      buf;
	UINT32      len;
	UINT8       sync;
};

/* Resource for IPCU channels */
struct sn_platform_ipcu {
	const UINT32    ch_type;
	const char      *dev_name;
	const char      *sem_name;
	const UINT8     ch_id;
	UINT32          shmem_paddr;
	const UINT32    shmem_size;
	INT32           fd;
	UINT8*          shmem_vaddr;
	UINT32          direction;
	void            (*send_cb)(UINT8, UINT32, UINT32, UINT32, UINT32);
	void            (*recv_cb)(UINT8, UINT32, UINT32, UINT32, UINT32);
	pthread_t       send_th;
	pthread_t       recv_th;
#ifdef IPCU_QUEUING
	pthread_t       qproc_th;
	UINT8           send_cnt;
	struct q_t      ipcuq;
#endif
	pthread_mutex_t send_mutex;
	pthread_mutex_t cfg_mutex;
	pthread_cond_t  cond_send_request;
	pthread_cond_t  cond_send_response;
	sem_t*          sem;
	UINT32          send_queued_num;
	UINT32          recv_cancelling;
	struct sn_send_thread_param     send_param;
};

#define IPCU_DIR_NONE              (0)             /* channel none   */

static struct sn_platform_ipcu ipcu_cfgs[] = {
	[0]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_0,
		.dev_name = IPCU_DEV_NAME0_UNIT0,
		.sem_name = "/ipcu-ch-0",
		.ch_id = 0,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_RECV,
	},
	[1]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_1,
		.dev_name = IPCU_DEV_NAME1_UNIT0,
		.sem_name = "/ipcu-ch-1",
		.ch_id = 1,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_SEND,
	},
	[2]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_2,
		.dev_name = IPCU_DEV_NAME2_UNIT0,
		.sem_name = "/ipcu-ch-2",
		.ch_id = 2,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_NONE,
	},
	[3]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_3,
		.dev_name = IPCU_DEV_NAME3_UNIT0,
		.sem_name = "/ipcu-ch-3",
		.ch_id = 3,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_NONE,
	},
	[4]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_4,
		.dev_name = IPCU_DEV_NAME4_UNIT0,
		.sem_name = "/ipcu-ch-4",
		.ch_id = 4,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_RECV,
	},
	[5]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_5,
		.dev_name = IPCU_DEV_NAME5_UNIT0,
		.sem_name = "/ipcu-ch-5",
		.ch_id = 5,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_SEND,
	},
	[6]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_6,
		.dev_name = IPCU_DEV_NAME6_UNIT0,
		.sem_name = "/ipcu-ch-6",
		.ch_id = 6,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_RECV,
	},
	[7]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_7,
		.dev_name = IPCU_DEV_NAME7_UNIT0,
		.sem_name = "/ipcu-ch-7",
		.ch_id = 7,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_SEND,
	},
	[8]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_8,
		.dev_name = IPCU_DEV_NAME0_UNIT1,
		.sem_name = "/ipcu-ch-8",
		.ch_id = 8,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_RECV,
	},
	[9]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_9,
		.dev_name = IPCU_DEV_NAME1_UNIT1,
		.sem_name = "/ipcu-ch-9",
		.ch_id = 9,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_SEND,
	},
	[10]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_10,
		.dev_name = IPCU_DEV_NAME2_UNIT1,
		.sem_name = "/ipcu-ch-10",
		.ch_id = 10,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_RECV,
	},
	[11]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_11,
		.dev_name = IPCU_DEV_NAME3_UNIT1,
		.sem_name = "/ipcu-ch-11",
		.ch_id = 11,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_SEND,
	},
	[12]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_12,
		.dev_name = IPCU_DEV_NAME4_UNIT1,
		.sem_name = "/ipcu-ch-12",
		.ch_id = 12,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_RECV,
	},
	[13]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_13,
		.dev_name = IPCU_DEV_NAME5_UNIT1,
		.sem_name = "/ipcu-ch-13",
		.ch_id = 13,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_SEND,
	},
	[14]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_14,
		.dev_name = IPCU_DEV_NAME6_UNIT1,
		.sem_name = "/ipcu-ch-14",
		.ch_id = 14,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_NONE,
	},
	[15]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_15,
		.dev_name = IPCU_DEV_NAME7_UNIT1,
		.sem_name = "/ipcu-ch-15",
		.ch_id = 15,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
		.direction = IPCU_DIR_NONE,
	},
#if 0
	[16]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_16,
		.dev_name = "/dev/snrtos0",
		.sem_name = "/ipcu-ch-16",
		.ch_id = 16,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
	},
	[17]  = {
		.ch_type = E_FJ_IPCU_MAILBOX_TYPE_17,
		.dev_name = "/dev/snrtos1",
		.sem_name = "/ipcu-ch-17",
		.ch_id = 17,
		.shmem_size = IPCU_BUFF_SIZE,
		.fd = -1,
	},
#endif
};

#define IPCU_CFG_MAX (sizeof(ipcu_cfgs) / sizeof(struct sn_platform_ipcu))

/* Private access to Memory Manager */
extern FJ_ERR_CODE fj_mm_open(VOID);
extern FJ_ERR_CODE fj_mm_close(VOID);
extern UINT32 fj_mm_virt_to_phys(UINT32 vaddr);
extern UINT32 fj_mm_is_shared_region(UINT32 paddr);
extern UINT32 fj_mm_phys_to_virt(UINT32 paddr);
extern void* fj_mm_mmap(UINT32 paddr, UINT32 length);
extern INT32 fj_mm_munmap(void* vaddr, UINT32 length);

/******************************************************************************/
/*
 * Private functions
 */
/******************************************************************************/
#ifdef IPCU_QUEUING
void q_init(void *p)
{
	struct sn_platform_ipcu *ipcu_cfg = (struct sn_platform_ipcu *)p;
	int i;

	ipcu_cfg->ipcuq.r = 0;
	ipcu_cfg->ipcuq.w = 0;
	ipcu_cfg->ipcuq.num = 0;
	for( i = 0; i < IPCU_Q_NUM; i ++ )
	{
		memset(&ipcu_cfg->ipcuq.buf[i], 0, sizeof(T_IPCU_IF));
	}
	pthread_mutex_init(&ipcu_cfg->ipcuq.mutex, NULL);
	pthread_cond_init(&ipcu_cfg->ipcuq.not_full, NULL);
	pthread_cond_init(&ipcu_cfg->ipcuq.not_empty, NULL);
}
void q_term(void *p)
{
	struct sn_platform_ipcu *ipcu_cfg = (struct sn_platform_ipcu *)p;
	int ret;
	pthread_cond_signal(&ipcu_cfg->ipcuq.not_empty);//for exit from deq()
	pthread_cond_signal(&ipcu_cfg->ipcuq.not_full);//for exit from enq()
	pthread_join(ipcu_cfg->qproc_th, NULL);
	ipcu_cfg->qproc_th = (pthread_t)NULL;

	ret = pthread_mutex_destroy(&ipcu_cfg->ipcuq.mutex);
	ret = pthread_cond_destroy(&ipcu_cfg->ipcuq.not_full);
	ret = pthread_cond_destroy(&ipcu_cfg->ipcuq.not_empty);
}
void enq(const T_IPCU_IF* data,void *p)
{
	struct sn_platform_ipcu *ipcu_cfg = (struct sn_platform_ipcu *)p;
	unsigned long id;
	pthread_mutex_lock(&ipcu_cfg->ipcuq.mutex);
	
	id = ipcu_cfg->ipcuq.w;
	while( ipcu_cfg->ipcuq.num >= IPCU_Q_NUM )// buffer full
	{
		pthread_cond_wait(&ipcu_cfg->ipcuq.not_full, &ipcu_cfg->ipcuq.mutex);
		if (ipcu_cfg->recv_cancelling == D_SYS_ON)
		{
			sprintf(syslogstr, "%s: Cancelling point \n", __func__);
			syslog(LOG_INFO,syslogstr);
			pthread_mutex_unlock(&ipcu_cfg->ipcuq.mutex);
			break;
		}
	}
	ipcu_cfg->ipcuq.buf[id].id = data->id;
	ipcu_cfg->ipcuq.buf[id].buf = data->buf;
	ipcu_cfg->ipcuq.buf[id].len = data->len;
	ipcu_cfg->ipcuq.buf[id].cont = data->cont;
	ipcu_cfg->ipcuq.buf[id].total_len = data->total_len;

	//sprintf(syslogstr, "%s [%2d]%8d %08X\n", __func__,ipcu_cfg->ipcuq.buf[id].id,id,ipcu_cfg->ipcuq.buf[id].buf);
	//syslog(LOG_INFO,syslogstr);
	ipcu_cfg->ipcuq.w++;
	if( ipcu_cfg->ipcuq.w >= IPCU_Q_NUM )
	{
		ipcu_cfg->ipcuq.w = 0;
	}
	ipcu_cfg->ipcuq.num++;
	if( ipcu_cfg->ipcuq.num >= IPCU_Q_NUM )
	{
		ipcu_cfg->ipcuq.num = IPCU_Q_NUM;
	}
	pthread_mutex_unlock(&ipcu_cfg->ipcuq.mutex);
	pthread_cond_signal(&ipcu_cfg->ipcuq.not_empty);

}
void deq(T_IPCU_IF* data, void *p)
{
	unsigned long id;
	struct sn_platform_ipcu *ipcu_cfg = (struct sn_platform_ipcu *)p;
	pthread_mutex_lock(&ipcu_cfg->ipcuq.mutex);
	id = ipcu_cfg->ipcuq.r;
	while( ipcu_cfg->ipcuq.num == 0 )// buffer is empty
	{
		pthread_cond_wait(&ipcu_cfg->ipcuq.not_empty, &ipcu_cfg->ipcuq.mutex);
		if (ipcu_cfg->recv_cancelling == D_SYS_ON)
		{
			sprintf(syslogstr, "%s: Cancelling point \n", __func__);
			syslog(LOG_INFO,syslogstr);
			pthread_mutex_unlock(&ipcu_cfg->ipcuq.mutex);
			break;
		}
	}
	data->id = ipcu_cfg->ipcuq.buf[id].id;
	data->buf = ipcu_cfg->ipcuq.buf[id].buf;
	data->len = ipcu_cfg->ipcuq.buf[id].len;
	data->cont = ipcu_cfg->ipcuq.buf[id].cont;
	data->total_len = ipcu_cfg->ipcuq.buf[id].total_len;
	//sprintf(syslogstr, "%s [%2d]%8d %08X\n", __func__,ipcu_cfg->ipcuq.buf[id].id,id,ipcu_cfg->ipcuq.buf[id].buf);
	//syslog(LOG_INFO,syslogstr);
	ipcu_cfg->ipcuq.r++;
	if( ipcu_cfg->ipcuq.r >= IPCU_Q_NUM )
	{
		ipcu_cfg->ipcuq.r = 0;
	}
	ipcu_cfg->ipcuq.num--;
	if( ipcu_cfg->ipcuq.num <= 0 )
	{
		ipcu_cfg->ipcuq.num = 0;
	}

	pthread_mutex_unlock(&ipcu_cfg->ipcuq.mutex);
	pthread_cond_signal(&ipcu_cfg->ipcuq.not_full);

}
#endif //IPCU_QUEUING
static struct sn_platform_ipcu *ipcu_select_cfg_where_id(UINT8 id)
{
	INT32 i;
	UINT32 top_address;
	UINT32 size;

	for (i = 0; i < IPCU_CFG_MAX; i++) {
		if (ipcu_cfgs[i].ch_id == id) {
			if( FJ_MM_getPhysAddrInfo(IPCU_MM_BUFF, &top_address, &size) == FJ_ERR_NG )
			{
				break;
			}
			ipcu_cfgs[i].shmem_paddr = top_address + (i << 15);
			return &ipcu_cfgs[i];
		}
	}
	LOGE("%s: IPCU#%d not found\n", __func__, id);
	return NULL;
}

static struct sn_platform_ipcu *ipcu_select_cfg_where_type(E_FJ_IPCU_MAILBOX_TYPE ipctype)
{
	INT32 i;
	UINT32 top_address;
	UINT32 size;

	for (i = 0; i < IPCU_CFG_MAX; i++) {
		if (ipcu_cfgs[i].ch_type == ipctype) {
			if( FJ_MM_getPhysAddrInfo(IPCU_MM_BUFF, &top_address, &size) == FJ_ERR_NG )
			{
				break;
			}
			ipcu_cfgs[i].shmem_paddr = 
				top_address + (ipcu_cfgs[i].ch_id << 15);
			return &ipcu_cfgs[i];
		}
	}
	LOGE("%s: IPCU#%d not found\n", __func__, ipctype);
	return NULL;
}

static INT32 sn_ioctl(INT32 fd, UINT32 request, void* arg)
{
#ifdef DEBUG_ON_MLB01
	UINT32 *dump = (UINT32 *)arg;
	LOGI("%s: fd=%d request=%08x \n", __func__, fd, request);
	LOGI("    arg= %08x %08x %08x \n", dump[0], dump[1], dump[2]);
	
	/* compatibility for IPCU IF structure of MLB01 */
	if (request == IPCU_IOCTL_SENDMSG) {
		struct ipcu_send_recv_msg_argv *req_msg = (struct ipcu_send_recv_msg_argv *)arg;
		T_IPCU_IF *send_msg = (T_IPCU_IF *)req_msg->buf;
		
		LOGI("    msg= %08x %08x %08x %08x \n", send_msg->id, send_msg->buf, send_msg->len, send_msg->cont);
		send_msg->id &= 0x7;
	}
	
	/* compatibility for IPCU ioctl structure of MLB01 */
	UINT32 i, modify[4];
	for (i = 0; i < IPCU_CFG_MAX; i++) {
		if (ipcu_cfgs[i].fd == fd) {
			modify[0] = ipcu_cfgs[i].ch_id & 0x7;
			modify[1] = dump[0];
			modify[2] = dump[1];
			modify[3] = dump[2];
			
			request += (sizeof(UINT32) << 16);
			break;
		}
	}
	
	INT32 ret;
	ret = ioctl(fd, request, (void *)modify);
	
	if (request == IPCU_IOCTL_RECVMSG) {
		dump[0] = modify[1];
		dump[1] = modify[2];
		dump[2] = modify[3];
	}
	return ret;
#endif
	
	return ioctl(fd, request, arg);
}


/******************************************************************************/
/**
 *  @brief  IPCU Framework
 *  @fn void send_from_shared(const struct sn_platform_ipcu *ipcu_cfg, UINT32 paddr, const struct sn_send_thread_param *param)
 *  @param  [in]    ipcu_cfg   : Resource of IPCU send channel
 *  @param  [in]    paddr      : Physical address for sending data
 *  @param  [in]    param      : Information for sending data
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
#if 0
static void send_from_shared(const struct sn_platform_ipcu *ipcu_cfg, UINT32 paddr, const struct sn_send_thread_param *param)
{
	INT32 ret;
	struct ipcu_send_recv_msg_argv  req_msg;
	T_IPCU_IF                       send_msg;
	
	/* Send a message */
	send_msg.id    = param->channel_id;
	send_msg.buf   = paddr;
	send_msg.len   = param->len;
	send_msg.cont  = D_NOT_CONTINUE;
	send_msg.total_len = param->len;
	
	req_msg.buf = (void*)&send_msg;
	req_msg.len = sizeof(send_msg);
	req_msg.flags = FLAG_SEND_NOTIFY;
	ret = sn_ioctl(ipcu_cfg->fd, IPCU_IOCTL_SENDMSG, &req_msg);
	if (ret != D_SYS_NO_ERR) {
		LOGE("%s: IPCU#%d failed to send. \n", __func__, param->channel_id);
	}
}
#endif


/******************************************************************************/
/**
 *  @brief  IPCU Framework
 *  @fn void send_from_noshared(const struct sn_platform_ipcu *ipcu_cfg, const struct sn_send_thread_param *param)
 *  @param  [in]    ipcu_cfg   : Resource of IPCU send channel
 *  @param  [in]    param      : Information for sending data
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
static void send_from_noshared(struct sn_platform_ipcu *ipcu_cfg, const struct sn_send_thread_param *param)
{
	INT32 ret;
	struct ipcu_send_recv_msg_argv  req_msg;
	T_IPCU_IF                       send_msg;
	UINT8  *remaining_data;
	UINT32 fragment_len, remaining_len;
#ifdef IPCU_QUEUING
	UINT32 ofs;
	ofs = (ipcu_cfg->send_cnt * 0x100);
#endif
	remaining_data = (UINT8 *)param->buf;
	remaining_len = param->len;
	do {
		/* Create a fragment */
		if (remaining_len > ipcu_cfg->shmem_size) {
			fragment_len = ipcu_cfg->shmem_size;
		} else {
			fragment_len = remaining_len;
		}
#ifdef IPCU_QUEUING
		memcpy((char*)(ipcu_cfg->shmem_vaddr + ofs), remaining_data, fragment_len);
		
		/* Send a message */
		send_msg.id    = param->channel_id;
		send_msg.buf   = (ipcu_cfg->shmem_paddr + ofs);
#else
		memcpy(ipcu_cfg->shmem_vaddr, remaining_data, fragment_len);
		
		/* Send a message */
		send_msg.id    = param->channel_id;
		send_msg.buf   = ipcu_cfg->shmem_paddr;
#endif
		send_msg.len   = fragment_len;
		send_msg.cont  = (remaining_len > fragment_len) ? D_CONTINUE : D_NOT_CONTINUE;
		send_msg.total_len = param->len;
		
		req_msg.buf = (void*)&send_msg;
		req_msg.len = sizeof(send_msg);
		req_msg.flags = FLAG_SEND_NOTIFY;
		ret = sn_ioctl(ipcu_cfg->fd, IPCU_IOCTL_SENDMSG, &req_msg);
		if (ret != D_SYS_NO_ERR) {
			LOGE("%s: IPCU#%d failed to send. \n", __func__, param->channel_id);
			break;
		}
		
		remaining_data += fragment_len;
		remaining_len -= fragment_len;
#ifdef IPCU_QUEUING
//	sprintf(syslogstr, "%s %8x %08X\n", __func__,ipcu_cfg->send_cnt,(ipcu_cfg->shmem_paddr + ofs));
//	syslog(LOG_INFO,syslogstr);
		ipcu_cfg->send_cnt++;
		if( ipcu_cfg->send_cnt >= IPCU_Q_NUM )
		{
			ipcu_cfg->send_cnt = 0;
		}
#endif
	} while (remaining_len > 0);
}


/******************************************************************************/
/**
 *  @brief  IPCU Framework
 *  @fn void* ipcu_sender_thread(void *p)
 *  @param  [in]    p          :  Resource of IPCU send channel
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
static void* ipcu_sender_thread(void *p)
{
	struct sn_platform_ipcu         *ipcu_cfg = (struct sn_platform_ipcu *)p;
	struct sn_send_thread_param     *param;
//	UINT32 share;
//	UINT32 paddr;
	char thread_name[32];
	
	snprintf(thread_name, sizeof(thread_name), "IPCU#%d:sender", ipcu_cfg->ch_id);
	prctl(PR_SET_NAME, thread_name, 0, 0, 0);
	
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	while (1) {
		/* Wait for sending data */
		pthread_cleanup_push((void *)pthread_mutex_unlock, (void *)&ipcu_cfg->send_mutex);
		pthread_mutex_lock(&ipcu_cfg->send_mutex);
		
		while (ipcu_cfg->send_queued_num == 0) {
			/* Cancellation point */
			pthread_cond_wait(&ipcu_cfg->cond_send_request, &ipcu_cfg->send_mutex);
		}
		param = &ipcu_cfg->send_param;
		
		pthread_mutex_unlock(&ipcu_cfg->send_mutex);
		pthread_cleanup_pop(0);
		
		/* Send */
//		paddr = fj_mm_virt_to_phys(param->buf);
//		share = fj_mm_is_shared_region(paddr);
//		if (share == D_SHARED) {
//			send_from_shared(ipcu_cfg, paddr, param);
//		} else {
			send_from_noshared(ipcu_cfg, param);
//		}
		
		/* Callback */
		pthread_mutex_lock(&ipcu_cfg->cfg_mutex);
		if ((param->sync == IPCU_SEND_ASYNC) && (ipcu_cfg->send_cb)) {
			ipcu_cfg->send_cb(ipcu_cfg->ch_id, (UINT32)param->buf, param->len, D_NOT_CONTINUE, param->len);
		}
		pthread_mutex_unlock(&ipcu_cfg->cfg_mutex);
		
		/* Throw away the sent data */
		pthread_mutex_lock(&ipcu_cfg->send_mutex);
		ipcu_cfg->send_queued_num--;
		pthread_mutex_unlock(&ipcu_cfg->send_mutex);
		
		pthread_cond_signal(&ipcu_cfg->cond_send_response);
	}
	return NULL;
}


/******************************************************************************/
/**
 *  @brief  IPCU Framework
 *  @fn  void* ipcu_receiver_thread(void *p)
 *  @param  [in]    p          : Resource of IPCU receive channel
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
static void* ipcu_receiver_thread(void *p)
{
	INT32 ret;
	char buf[255];
	struct ipcu_send_recv_msg_argv req_msg;
	T_IPCU_IF *recv_msg;
	struct sn_platform_ipcu *ipcu_cfg = (struct sn_platform_ipcu *)p;
	void *buf_vaddr, *mmap_vaddr;
	char thread_name[32];
	
	snprintf(thread_name, sizeof(thread_name), "IPCU#%d:receiver", ipcu_cfg->ch_id);
	prctl(PR_SET_NAME, thread_name, 0, 0, 0);
	
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	while(1) {
		/* Artificial cancellation point */
		pthread_mutex_lock(&ipcu_cfg->cfg_mutex);
		if (ipcu_cfg->recv_cancelling == D_SYS_ON) {
			LOGV("%s: IPCU#%d: cancellation point \n", __func__, ipcu_cfg->ch_id);
			pthread_mutex_unlock(&ipcu_cfg->cfg_mutex);
			break;
		}
		pthread_mutex_unlock(&ipcu_cfg->cfg_mutex);
		
		/* Wait for receiving a message */
		req_msg.buf = (void*)buf;
		req_msg.len = sizeof(buf);
		req_msg.flags = FLAG_RECV_WAIT;
		
		ret = sn_ioctl(ipcu_cfg->fd, IPCU_IOCTL_RECVMSG, &req_msg);
		if (ret != D_SYS_NO_ERR) {
			LOGE("IPCU#%d: failed to receive. fd=%d buf=%p len=%d flags=0x%x \n", 
				ipcu_cfg->ch_id, ipcu_cfg->fd, req_msg.buf, req_msg.len, req_msg.flags);
			usleep(100000);
			continue;
		}
		
		/* Artificial cancellation point */
		pthread_mutex_lock(&ipcu_cfg->cfg_mutex);
		if (ipcu_cfg->recv_cancelling == D_SYS_ON) {
			LOGV("%s: IPCU#%d: cancelled IPCU_IOCTL_RECVMSG \n", __func__, ipcu_cfg->ch_id);
			pthread_mutex_unlock(&ipcu_cfg->cfg_mutex);
			break;
		}
		pthread_mutex_unlock(&ipcu_cfg->cfg_mutex);

#ifdef IPCU_QUEUING
		if( ipcu_cfg->ch_id > 3 )
		{
			recv_msg = (T_IPCU_IF *)buf;
			mmap_vaddr = NULL;
			buf_vaddr = (void*) fj_mm_phys_to_virt(recv_msg->buf);
			recv_msg->buf = (int)buf_vaddr;
			enq((T_IPCU_IF *)buf,p);
		}
		else
		{
#endif
			/* Callback */
			recv_msg = (T_IPCU_IF *)buf;
			mmap_vaddr = NULL;
			buf_vaddr = (void*) fj_mm_phys_to_virt(recv_msg->buf);
			
			if (buf_vaddr == MAP_FAILED) {
				mmap_vaddr = fj_mm_mmap(recv_msg->buf, recv_msg->len);
				if (mmap_vaddr == MAP_FAILED) {
					LOGE("%s: failed to mmap %p (%s) \n", __func__, mmap_vaddr, strerror(errno));
					sprintf(syslogstr, "%s: CH#%d failed to mmap VA %p PA 0x%08X(%s) \n", __func__,ipcu_cfg->ch_id, mmap_vaddr, recv_msg->buf, strerror(errno));
					syslog(LOG_ERR,syslogstr);
				}
				buf_vaddr = mmap_vaddr;
			}
			if( (int)mmap_vaddr != -1 )
			{
				pthread_mutex_lock(&ipcu_cfg->cfg_mutex);
				if ( ipcu_cfg->recv_cb != NULL ) {
					ipcu_cfg->recv_cb((UINT8)recv_msg->id, (UINT32)buf_vaddr, recv_msg->len, recv_msg->cont, recv_msg->total_len);
				}
				else
				{
					//callback does not registerd
				}
				pthread_mutex_unlock(&ipcu_cfg->cfg_mutex);
			}
			if ((mmap_vaddr != MAP_FAILED) && (mmap_vaddr != NULL)) {
				fj_mm_munmap(mmap_vaddr, recv_msg->len);
			}
#ifdef IPCU_QUEUING
		}
#endif
		/* Send back a response message */
		ret = sn_ioctl(ipcu_cfg->fd, IPCU_IOCTL_ACKSEND, &req_msg);
		if (ret != D_SYS_NO_ERR) {
			LOGE("IPCU#%d: failed to send back an ACK message. \n", ipcu_cfg->ch_id);
			continue;
		}
	}
	
	pthread_exit(NULL);
	return NULL;
}
#ifdef IPCU_QUEUING
/*
cp /run/SD/mmcblk0p1/usr/lib/libcmfwk.so /usr/lib/
busybox syslogd
camifts -i
*/
static void* ipcu_receiver_q_processing_thread(void *p)
{
	struct sn_platform_ipcu *ipcu_cfg = (struct sn_platform_ipcu *)p;
	void *buf_vaddr;
	T_IPCU_IF recv_msg;
	unsigned long pa;
	printf("%s, %d\n", __func__, __LINE__);
	while( ipcu_cfg->recv_cancelling != D_SYS_ON )
	{
		deq(&recv_msg,p);
		//buf_vaddr = (void*) fj_mm_phys_to_virt(recv_msg.buf);
		buf_vaddr = recv_msg.buf;
		if (buf_vaddr == MAP_FAILED) {
			sprintf(syslogstr, "%s: CH#%d failed to mmap PA 0x%08X(%s) \n", __func__,ipcu_cfg->ch_id, recv_msg.buf, strerror(errno));
			syslog(LOG_ERR,syslogstr);
		}
		else
		{
			if ( ipcu_cfg->recv_cb != NULL ) {
				ipcu_cfg->recv_cb((UINT8)recv_msg.id, (UINT32)buf_vaddr, recv_msg.len, recv_msg.cont, recv_msg.total_len);
			}
			else
			{
				//callback does not registerd
			}
		}
	}
	pthread_exit(NULL);
	return NULL;
}
#endif //IPCU_QUEUING
/******************************************************************************/
/**
 *  @brief  IPCU Framework
 *  @fn void stop_sending_channel(struct sn_platform_ipcu *ipcu_cfg)
 *  @param  [in]    ipcu_cfg   : Resource of IPCU send channel
 *  @note
 *  @author
 *  @version
 *  @attention
 */
/******************************************************************************/
static void stop_sending_channel(struct sn_platform_ipcu *ipcu_cfg)
{
	/* Wait for the send thread to finish */
	if (ipcu_cfg->send_th != (pthread_t)NULL) {
		pthread_mutex_lock(&ipcu_cfg->send_mutex);
		pthread_cancel(ipcu_cfg->send_th);
		pthread_mutex_unlock(&ipcu_cfg->send_mutex);
		pthread_join(ipcu_cfg->send_th, NULL);
		ipcu_cfg->send_th = (pthread_t)NULL;
		ipcu_cfg->send_queued_num = 0;
	}
}

/******************************************************************************/
/**
 *  @brief  IPCU Framework
 *  @fn void stop_receiving_channel(struct sn_platform_ipcu *ipcu_cfg)
 *  @param  [in]    ipcu_cfg   : Resource of IPCU receive channel
 *  @note
 *  @author
 *  @version
 *  @attention
 */
/******************************************************************************/
static void stop_receiving_channel(struct sn_platform_ipcu *ipcu_cfg)
{
	INT32 ret;
	struct ipcu_send_recv_msg_argv req_msg;
	
	/* Wait for the receive thread to finish */
	if (ipcu_cfg->recv_th != (pthread_t)NULL) {
		LOGV("%s: IPCU#%d: begin \n", __func__, ipcu_cfg->ch_id);
		
		/* prepare */
		pthread_mutex_lock(&ipcu_cfg->cfg_mutex);
		ipcu_cfg->recv_cancelling = D_SYS_ON;
		pthread_mutex_unlock(&ipcu_cfg->cfg_mutex);
		
		/* request */
		req_msg.buf = NULL;
		req_msg.len = 0;
		req_msg.flags = FLAG_RECV_WAIT;
		
		ret = sn_ioctl(ipcu_cfg->fd, IPCU_IOCTL_RECV_FLASH, &req_msg);
		if (ret != D_SYS_NO_ERR) {
			LOGE("IPCU#%d: failed to flash. fd=%d buf=%p len=%d flags=0x%x \n", 
				ipcu_cfg->ch_id, ipcu_cfg->fd, req_msg.buf, req_msg.len, req_msg.flags);
		}
		
		/* wait */
#ifdef IPCU_QUEUING
		if( ipcu_cfg->ch_id > 3 )
		{
			q_term((void*)ipcu_cfg);
		}
#endif
		pthread_join(ipcu_cfg->recv_th, NULL);
		/* cleanup */
		ipcu_cfg->recv_th = (pthread_t)NULL;
		ipcu_cfg->recv_cancelling = D_SYS_OFF;
		
		LOGV("%s: IPCU#%d: finished \n", __func__, ipcu_cfg->ch_id);
	}
}


/******************************************************************************/
/**
 *  @brief  IPCU Framework
 *  @fn FJ_ERR_CODE open_channel(UINT8 id, UINT32 direction)
 *  @param  [in]    id        : instance ID. 
 *  @param  [in]    direction : IPCU direction
 *  @note
 *  @author
 *  @version
 *  @attention
 */
/******************************************************************************/
static FJ_ERR_CODE open_channel(UINT8 id, UINT32 direction)
{
	INT32 ret;
	struct ipcu_open_close_ch_argv  open_msg;
	struct sn_platform_ipcu *ipcu_cfg = ipcu_select_cfg_where_id(id);
	pthread_attr_t attr;
	struct sched_param param;
	
	if (ipcu_cfg == NULL) {
		return FJ_ERR_NG;
	}
	
	/* Open an IPCU channel */
	open_msg.direction = direction;
	ret = sn_ioctl(ipcu_cfg->fd, IPCU_IOCTL_OPENCH, &open_msg);
	if (ret != D_SYS_NO_ERR) {
		LOGE("%s: failed to open IPCU#%d \n", __func__, ipcu_cfg->ch_id);
		return FJ_ERR_NG;
	}
	
	ipcu_cfg->direction = direction;
	
	/* Create send/receive thread */
	pthread_attr_init (&attr);
	pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
	pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED);
	param.sched_priority = sched_get_priority_min(SCHED_FIFO);
	pthread_attr_setschedparam (&attr, &param);
	
	if (ipcu_cfg->direction == IPCU_DIR_SEND) {
		stop_receiving_channel(ipcu_cfg);
		if (ipcu_cfg->send_th == (pthread_t)NULL) {
			pthread_create(&ipcu_cfg->send_th, &attr, (void*)ipcu_sender_thread, (void*)ipcu_cfg);
		}
	}
	else if (ipcu_cfg->direction == IPCU_DIR_RECV) {
		stop_sending_channel(ipcu_cfg);
		if (ipcu_cfg->recv_th == (pthread_t)NULL) {
#ifdef IPCU_QUEUING
			if( ipcu_cfg->ch_id > 3 )
			{
				q_init((void*)ipcu_cfg);
				pthread_create(&ipcu_cfg->qproc_th, NULL, (void*)ipcu_receiver_q_processing_thread, (void*)ipcu_cfg);
			}
#endif
			pthread_create(&ipcu_cfg->recv_th, &attr, (void*)ipcu_receiver_thread, (void*)ipcu_cfg);
		}
	}
#ifdef IPCU_QUEUING
	ipcu_cfg->send_cnt = 0;
#endif
	return FJ_ERR_OK;
}

/******************************************************************************/
/**
 *  @brief  IPCU Framework
 *  @fn FJ_ERR_CODE close_channel(UINT8 id)
 *  @param  [in]    id        : instance ID. 
 *  @note
 *  @author
 *  @version
 *  @attention
 */
/******************************************************************************/
static FJ_ERR_CODE close_channel(UINT8 id)
{
	struct ipcu_open_close_ch_argv  close_msg;
	struct sn_platform_ipcu *ipcu_cfg = ipcu_select_cfg_where_id(id);
	
	if (ipcu_cfg == NULL) {
		return FJ_ERR_NG;
	}
	
	stop_sending_channel(ipcu_cfg);
	stop_receiving_channel(ipcu_cfg);
	
	/* Close an IPCU channel */
	if (ipcu_cfg->direction != IPCU_DIR_NONE) {
		close_msg.direction = ipcu_cfg->direction;
		sn_ioctl(ipcu_cfg->fd, IPCU_IOCTL_CLOSECH, &close_msg);
	}
	return FJ_ERR_OK;
}

/******************************************************************************/
/*
 * Public functions
 */
/******************************************************************************/
/******************************************************************************/
/**
 *  @brief  Open IPCU (Inter-Processor Control Unit) 
 *  @fn FJ_ERR_CODE FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE ipctype, UINT8 *id)
 *  @param  [in]    ipctype   : indicated IPCU channel and command type. See E_FJ_IPCU_MAILBOX_TYPE
 *  @param  [out]   id        : instance ID. 
 *  @retval FJ_ERR_OK		: Success
 *  @retval FJ_ERR_NG		: Error
 *  @note
 *  @author
 *  @version
 *  @attention
 */
/******************************************************************************/
FJ_ERR_CODE FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE ipctype, UINT8 *id)
{
	INT32 fd, ret;
	struct sn_platform_ipcu *ipcu_cfg;
	sem_t* sem_p;
	struct timespec time;

	ret = fj_mm_open();
	if (ret != FJ_ERR_OK) {
		LOGE("%s: failed to open memory manager \n", __func__);
		//FJ_IPCU_Close(ipcu_cfg->ch_id);
		return FJ_ERR_NG;
	}

	ipcu_cfg = (struct sn_platform_ipcu *)ipcu_select_cfg_where_type(ipctype);
	if (ipcu_cfg == NULL) {
		return FJ_ERR_NG;
	}

	/* Avoid to open an IPCU channel twice at the same time */
	sem_p = sem_open(ipcu_cfg->sem_name, O_CREAT, 0600, 1);
	if (sem_p == SEM_FAILED) {
		LOGE("%s: Failed to create a semaphore %s. \n", __func__, ipcu_cfg->sem_name);
		return FJ_ERR_NG;
	}

	ret = clock_gettime(CLOCK_MONOTONIC, &time);
	if (ret != FJ_ERR_OK) {
		LOGE("%s: failed to get clock time %s\n", __func__, strerror(errno));
		return FJ_ERR_NG;
	}

	// wait 1sec
	time.tv_sec = time.tv_sec + 1;

	ret = sem_timedwait(sem_p, &time);
	if (ret != 0) {
		LOGE("%s: IPCU#%d wait open timeout. \n", __func__, ipctype);
		return FJ_ERR_NG;
	}
	ipcu_cfg->sem = sem_p;
	
	pthread_mutex_init(&ipcu_cfg->send_mutex, NULL);
	pthread_mutex_init(&ipcu_cfg->cfg_mutex, NULL);
	pthread_cond_init(&ipcu_cfg->cond_send_request, NULL);
	pthread_cond_init(&ipcu_cfg->cond_send_response, NULL);
	
	/* Open IPCU device */
	fd = open(ipcu_cfg->dev_name, O_RDWR);
	if (fd < 0) {
		LOGE("%s: failed to open %s \n", __func__, ipcu_cfg->dev_name);
		FJ_IPCU_Close(ipcu_cfg->ch_id);
		return FJ_ERR_NG;
	}
	LOGV("%s: open %s (IPCU#%d) \n", __func__, ipcu_cfg->dev_name, ipcu_cfg->ch_id);
	ipcu_cfg->fd = fd;
	
	/* Map a shared memory segment */
	ipcu_cfg->shmem_vaddr = (void*) fj_mm_phys_to_virt(ipcu_cfg->shmem_paddr);
	
	/* Open IPCU channel if the direction has been determined */
	if (ipcu_cfg->direction != IPCU_DIR_NONE) {
		ret = open_channel(ipcu_cfg->ch_id, ipcu_cfg->direction);
		if (ret != D_SYS_NO_ERR) {
			return FJ_ERR_NG;
		}
	}
	
	*id = ipcu_cfg->ch_id;
	return FJ_ERR_OK;
}

/******************************************************************************/
/**
 *  @brief  Close IPCU (Inter-Processor Control Unit) 
 *  @fn FJ_ERR_CODE FJ_IPCU_Close(UINT8 id)
 *  @param  [in]    id        : instance ID. 
 *  @retval FJ_ERR_OK		: Success
 *  @retval FJ_ERR_NG		: Error
 *  @note
 *  @author
 *  @version
 *  @attention
 */
/******************************************************************************/
FJ_ERR_CODE FJ_IPCU_Close(UINT8 id)
{
	INT32 i, ret;
	struct sn_platform_ipcu *ipcu_cfg = ipcu_select_cfg_where_id(id);
	
	if (ipcu_cfg == NULL) {
		return FJ_ERR_NG;
	}
	
	close_channel(id);
	ipcu_cfg->send_cb = NULL;
	ipcu_cfg->recv_cb = NULL;
	
	/* Unmap a shared memory segment */
	if (ipcu_cfg->shmem_vaddr != MAP_FAILED) {
		ipcu_cfg->shmem_vaddr = NULL;
	}
	ipcu_cfgs->shmem_paddr = 0;

	/* Close the IPCU device */
	if (ipcu_cfg->fd >= 0) {
		close(ipcu_cfg->fd);
		ipcu_cfg->fd = -1;
		LOGV("%s: close %s (IPCU#%d) \n", __func__, ipcu_cfg->dev_name, id);
	}
	
	/* Close /dev/mem device when all channels have been closed */
	for (i = 0; i < IPCU_CFG_MAX; i++) {
		if (ipcu_cfgs[i].fd != -1) {
			break;
		}
	}
	if (i == IPCU_CFG_MAX) {
		fj_mm_close();
	}
	
	/* unlock semaphore */
	ret = sem_post(ipcu_cfg->sem);
	if (ret != 0) {
		LOGE("%s: IPCU#%d semaphore unlock failed. \n", __func__, id);
	}
	
	/* clean thread */
	if (ipcu_cfg->sem != NULL) {
		pthread_cond_destroy(&ipcu_cfg->cond_send_request);
		pthread_cond_destroy(&ipcu_cfg->cond_send_response);
		pthread_mutex_destroy(&ipcu_cfg->cfg_mutex);
		pthread_mutex_destroy(&ipcu_cfg->send_mutex);
	} else {
		LOGE("%s: failed to clean thread IPCU#%d \n", __func__, id);
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

/******************************************************************************/
/**
 *  @brief  Send data by IPCU (Inter-Processor Control Unit) 
 *  @fn FJ_ERR_CODE FJ_IPCU_Send(UINT8 id, UINT32 pdata, UINT32 length, UINT8 sync)
 *  @param  [in]    id        : instance ID. 
 *  @param  [in]    pdata     : top address of the sending data table.
 *  @param  [in]    length    : byte size of data. 
 *  @param  [in]    sync      : selection for sync and async type function
 *  @retval FJ_ERR_OK		: Success
 *  @retval FJ_ERR_NG		: Error. It's busy. This IPCU channel is used by other communication
 *  @remarks This API checks the data table on shared or none-shared memory.If none-shared memory, then this will copy data table to internal reserved shared memory. Its max size is 32kByte. (Preliminary)
 *  @note
 *  @author
 *  @version
 *  @attention
 */
/******************************************************************************/
FJ_ERR_CODE FJ_IPCU_Send(UINT8 id, UINT32 pdata, UINT32 length, UINT8 sync)
{
	INT32 ret;
	struct sn_platform_ipcu *ipcu_cfg = ipcu_select_cfg_where_id(id);
	struct sn_send_thread_param     *param;
	
	LOGV("%s: id=%d pdata=0x%08x length=%d sync=%d \n", __func__, id, pdata, length, sync);
	
	if (ipcu_cfg == NULL) {
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	if (ipcu_cfg->fd < 0) {
		LOGE("%s: IPCU#%d is not opened. \n", __func__, id);
		return FJ_ERR_NG_COMMUNICATON;
	}
	
	/* Re-open an IPCU channel */
	if (ipcu_cfg->direction == IPCU_DIR_RECV) {
		close_channel(id);
	}
	
	if (ipcu_cfg->direction != IPCU_DIR_SEND) {
		ret = open_channel(id, IPCU_DIR_SEND);
		if (ret != D_SYS_NO_ERR) {
			return FJ_ERR_NG_COMMUNICATON;
		}
	}
	
	pthread_mutex_lock(&ipcu_cfg->send_mutex);
	if (ipcu_cfg->send_queued_num) {
		LOGE("%s: IPCU#%d is busy. \n", __func__, id);
		pthread_mutex_unlock(&ipcu_cfg->send_mutex);
		return FJ_ERR_NG_CONDITION;
	}
	
	param = &ipcu_cfg->send_param;
	param->channel_id = id;
	param->buf = pdata;
	param->len = length;
	param->sync = sync;
	ipcu_cfg->send_queued_num++;
	
	pthread_mutex_unlock(&ipcu_cfg->send_mutex);
	pthread_cond_signal(&ipcu_cfg->cond_send_request);
	
	if (sync == IPCU_SEND_SYNC) {
		pthread_mutex_lock(&ipcu_cfg->send_mutex);
		while (ipcu_cfg->send_queued_num > 0) {
			pthread_cond_wait(&ipcu_cfg->cond_send_response, &ipcu_cfg->send_mutex);
		}
		pthread_mutex_unlock(&ipcu_cfg->send_mutex);
	}
	return FJ_ERR_OK;
}

/******************************************************************************/
/**
 *  @brief  Regist request receive callback function for IPCU (Inter-Processor Control Unit) 
 *  @fn FJ_ERR_CODE FJ_IPCU_SetReceiverCB(UINT8 id, void(*req_fn_ptr)())
 *  @param  [in]    id          : instance ID. 
 *  @param  [in]    req_fn_ptr  : callback function for request interrupt. 
 *  @retval FJ_ERR_OK		: Success
 *  @retval FJ_ERR_NG		: Error
 *  @remarks (*req_fn_ptr)() function will be called on a task, not on any interrupt.
 *  @note
 *  @author
 *  @version
 *  @attention
 */
/******************************************************************************/
FJ_ERR_CODE FJ_IPCU_SetReceiverCB(UINT8 id, void(*req_fn_ptr)(UINT8, UINT32, UINT32, UINT32, UINT32))
{
	INT32 ret;
	struct sn_platform_ipcu *ipcu_cfg = ipcu_select_cfg_where_id(id);
	
	if (ipcu_cfg == NULL) {
		return FJ_ERR_NG;
	}
	
	pthread_mutex_lock(&ipcu_cfg->cfg_mutex);
	ipcu_cfg->send_cb = req_fn_ptr;
	ipcu_cfg->recv_cb = req_fn_ptr;
	pthread_mutex_unlock(&ipcu_cfg->cfg_mutex);
	
	if (ipcu_cfg->fd < 0) {
		LOGE("%s: IPCU#%d is not opened. \n", __func__, id);
		return FJ_ERR_NG;
	}
	
	/* Open receive channel if the direction has NOT been determined */
	if (ipcu_cfg->direction == IPCU_DIR_NONE) {
		ret = open_channel(id, IPCU_DIR_RECV);
		if (ret != D_SYS_NO_ERR) {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

