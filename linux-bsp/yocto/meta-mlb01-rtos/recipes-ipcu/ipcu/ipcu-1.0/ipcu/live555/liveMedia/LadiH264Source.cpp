/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2018 Live Networks, Inc.  All rights reserved.
// Implementation

#include "LadiH264Source.hh"
#include "LadiAudioSource.hh"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define H264_MSGKEY 0x5487
#define MSGKEY_CATCMD 0x9525
#define D1_MSGKEY	0x6833
#define MAX_TEXT 4

#define MAX_FRAME_SIZE 8000000

static Boolean fIPCInit = False;
static Boolean fEncoder = False;

#define PAGE_SHIFT 12
#define PAGE_MASK  (~((1 << PAGE_SHIFT) - 1))

static int mem_fd = -1;
static int msgid = -1;
static int msgid_catcmd = -1;

struct timeval res_h264;

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

struct msg_que_cmd
{
	long msg_type;
	unsigned long cmd;
	unsigned long sub;
	unsigned long data;
	char content_text[MAX_TEXT];
};

static struct msg_que_st h264_que_data;
static struct msg_que_cmd g_catcmd_que_data;

LadiH264Source* LadiH264Source::createNew(UsageEnvironment& env, char request_source) {
	DEBUG_PRINT(("====== LadiH264Source::createNew ======\n"));

	return new LadiH264Source(env, request_source);
}

LadiH264Source::LadiH264Source(UsageEnvironment& env, char request_source)
  : FramedSource(env), frequest_source(request_source) {

	struct msqid_ds msq_info;
	unsigned int skip_h264 = 0;

	if(!fIPCInit){
		LadiH264Source::IPCCreate();
		fIPCInit = true;
		memset(&res_h264, 0, sizeof(res_h264));
	}
	if(!fEncoder){
		// clean the queue
		if (msgctl(msgid, IPC_STAT, &msq_info) < 0) {
			printf("%s msgctl h264 failed with error: %d\n", __func__, errno);
		}

		do {
			msgrcv(msgid, &h264_que_data, sizeof(struct msg_que_st) - sizeof(long), 2, IPC_NOWAIT);
			skip_h264++;
		} while(skip_h264 < msq_info.msg_qnum);

		g_catcmd_que_data.msg_type = 4;
		g_catcmd_que_data.cmd = 0;
		g_catcmd_que_data.sub = 0xb;
		g_catcmd_que_data.data = 0x8;
		if (msgsnd(msgid_catcmd, &g_catcmd_que_data, sizeof(g_catcmd_que_data) - sizeof(long), IPC_NOWAIT) == -1) {
			//printf("%s, %d, catcmd msgsnd enable failed\n", __func__, __LINE__);
		} else {
			//printf("%s, %d, catcmd msgsnd enable OK\n", __func__, __LINE__);
		}
#if 0
		h264_que_data.msg_type = 4;
		h264_que_data.ctrl_parameter = 0x1234;
		if (msgsnd(msgid, &h264_que_data, sizeof(h264_que_data) - sizeof(long), IPC_NOWAIT) == -1)
		{
			DEBUG_PRINT(("====== h264 msgsnd disable failed ======\n"));
		}
#endif
		fEncoder = true;
	}
}

LadiH264Source::~LadiH264Source() {
}

void LadiH264Source::doGetNextFrame() {
  DEBUG_PRINT(("====== LadiH264Source::doGetNextFrame ======\n"));

  nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc*)ReadDataHandler, this); 
}

void LadiH264Source::doStopGettingFrames() {
	DEBUG_PRINT(("====== LadiH264Source::doStopGettingFrames ======\n"));

	if(fEncoder != False)
	{
		g_catcmd_que_data.msg_type = 4;
		g_catcmd_que_data.cmd = 0;
		g_catcmd_que_data.sub = 0xb;
		g_catcmd_que_data.data = 0x9;
		if (msgsnd(msgid_catcmd, &g_catcmd_que_data, sizeof(g_catcmd_que_data) - sizeof(long), IPC_NOWAIT) == -1)
		{
			//printf("%s, %d, catcmd msgsnd enable failed\n", __func__, __LINE__);
		} else {
			//printf("%s, %d, catcmd msgsnd enable OK\n", __func__, __LINE__);
		}
		#if 0
		h264_que_data.msg_type = 4;
		h264_que_data.ctrl_parameter = 0x5678;
		if (msgsnd(msgid, &h264_que_data, sizeof(h264_que_data) - sizeof(long), IPC_NOWAIT) == -1)
		{
			DEBUG_PRINT(("====== h264 msgsnd disable failed ======\n"));
		}
		#endif
		fEncoder = False;
	}

	LadiH264Source::IPCDelete();
	envir().taskScheduler().unscheduleDelayedTask(nextTask());
}

void LadiH264Source::ReadDataHandler(LadiH264Source* source) {
  DEBUG_PRINT(("====== LadiH264Source::ReadDataHandler ======\n"));
  if (!source->isCurrentlyAwaitingData()) {
  	DEBUG_PRINT(("====== LadiH264Source::ReadDataHandler:isCurrentlyAwaitingData ======\n"));
    //source->doStopGettingFrames(); // we're not ready for the data yet
    return;
  }
  source->doReadData();
}

void LadiH264Source::doReadData() {
  unsigned long long ofs_us;
  static unsigned long long base_tp;
  DEBUG_PRINT(("====== LadiH264Source::doReadData ======\n"));
  void *mmap_vaddr;

#if 0
  if(!fEncoder){
	h264_que_data.msg_type = 4;
	h264_que_data.ctrl_parameter = 0x1234;
	if (msgsnd(msgid, &h264_que_data, sizeof(struct msg_que_st) - sizeof(long), IPC_NOWAIT) == -1)
	{
		DEBUG_PRINT(("====== hevc msgsnd enable failed ======\n"));
	}
	fEncoder = true;
  }
#endif
  if(msgrcv(msgid, &h264_que_data, sizeof(struct msg_que_st) - sizeof(long), 2, IPC_NOWAIT) == -1)
  {
    //printf("h264 video msgrcv failed with error: %d\n", errno);
	nextTask() = envir().taskScheduler().scheduleDelayedTask(1000, (TaskFunc*)ReadDataHandler, this);
  }
  else
  {
	fFrameSize = h264_que_data.frame_size;

	mmap_vaddr = LadiH264Source::fj_mm_mmap(h264_que_data.phy_address, fFrameSize);
	memcpy(fTo, mmap_vaddr, fFrameSize);
	LadiH264Source::fj_mm_munmap(mmap_vaddr, fFrameSize);
	if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
		base_tp = h264_que_data.timestamp;
		gettimeofday(&res_h264, NULL);
		fPresentationTime.tv_sec	= res_h264.tv_sec;
		fPresentationTime.tv_usec	= res_h264.tv_usec;
		printf("h264 start:%ds %dus, base_tp:%d, size:%x\n", (unsigned long)fPresentationTime.tv_sec, (unsigned long)fPresentationTime.tv_usec, (unsigned long)base_tp, h264_que_data.frame_size);
	} else {
		ofs_us = h264_que_data.timestamp - base_tp;
		ofs_us = (double)ofs_us * ((double)1000000.0 / (double)90000.0); // convert 90KHz to us
		ofs_us += res_h264.tv_usec;
		fPresentationTime.tv_sec	= res_h264.tv_sec + (ofs_us / 1000000);
		fPresentationTime.tv_usec	= ofs_us % 1000000;
	}
	FramedSource::afterGetting(this);	
  }
}

void LadiH264Source::IPCCreate() {

	DEBUG_PRINT(("====== LadiH264Source::IPCCreate ======\n"));

	switch(frequest_source)
	{
		case VIDEO_H264:
			msgid = msgget(H264_MSGKEY, 0666 | IPC_CREAT);
			printf("msgid H264_VIDEO created\n");
			break;
		case VIDEO_D1:
			msgid = msgget(D1_MSGKEY, 0666 | IPC_CREAT);
			printf("msgid H264_VIDEO_D1 created\n");
			break;
		default:
			break;
	}
	
	if (msgid == -1) 
	{
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	msgid_catcmd = msgget(MSGKEY_CATCMD, 0666 | IPC_CREAT);

	if (msgid_catcmd == -1)
	{
		printf("%s, %d, msgget failed \n", __func__, __LINE__);
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	
	mem_fd = open("/dev/mem", (O_RDWR | O_SYNC));
	if (mem_fd < 0) {
		printf("%s: failed to open\n", __func__);
		exit(EXIT_FAILURE);
	}	
}

void LadiH264Source::IPCDelete() {

	DEBUG_PRINT(("====== LadiH264Source::IPCDelete ======\n"));

	unsigned int count = 0;
  	struct msqid_ds msq_info;

	if(msgctl(msgid, IPC_STAT, &msq_info) < 0)
	{
		DEBUG_PRINT(("msgctl failed with error: %d\n", errno));
	}

	do{
		msgrcv(msgid, &h264_que_data, sizeof(h264_que_data) - sizeof(long), 2, IPC_NOWAIT);
		count++;
	}while(count < msq_info.msg_qnum);
}

void *LadiH264Source::fj_mm_mmap(unsigned int paddr, unsigned int length) {

	DEBUG_PRINT(("====== LadiH264Source::fj_mm_mmap ======\n"));
	void *vaddr, *page_vaddr;
	unsigned int page_paddr;
	unsigned int page_offset;
	unsigned int page_length;
	
	page_paddr = paddr & PAGE_MASK;
	page_offset = paddr & ~PAGE_MASK;
	page_length = length + page_offset;

	page_vaddr = mmap(0, page_length, PROT_READ | PROT_WRITE,
			MAP_SHARED, mem_fd, page_paddr);

	vaddr =  (void*) (page_vaddr + page_offset);

	return vaddr;
}

int LadiH264Source::fj_mm_munmap(void *vaddr, unsigned int length)
{
	unsigned int page_vaddr;
	unsigned int page_offset;
	unsigned int page_length;
	
	page_vaddr = (unsigned int)vaddr & PAGE_MASK;
	page_offset = (unsigned int)vaddr & ~PAGE_MASK;
	page_length = length + page_offset;
		
	return munmap((void*)page_vaddr, page_length);
}

int LadiH264Source::debug_dump(char* buf, unsigned long size)
{
	FILE* fp;

	int ret;

	fp = fopen("/tmp/test.264", "ab+");
	if (fp < 0) {
		printf("%s: failed to open %s \n", __func__, "/tmp/test.264");
		ret = -1;
	}
	else
	{
		fwrite(buf, sizeof(char), size, fp);
		ret = 0;
	}
	fclose(fp);

	return ret;
}

unsigned LadiH264Source::maxFrameSize() const {
  return MAX_FRAME_SIZE;
}

