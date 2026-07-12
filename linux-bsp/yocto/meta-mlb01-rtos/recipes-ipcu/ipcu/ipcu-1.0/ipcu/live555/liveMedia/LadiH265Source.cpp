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

#include "LadiH265Source.hh"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MSGKEY 0x9527
#define MSGKEY_CATCMD 0x9525
#define MAX_TEXT 4

#define MAX_FRAME_SIZE 3000000

static Boolean fIPCInit = False;
static Boolean fEncoder = False;

#define PAGE_SHIFT 12
#define PAGE_MASK  (~((1 << PAGE_SHIFT) - 1))

static int mem_fd = -1;
static int msgid = -1;
static int msgid_catcmd = -1;

struct timeval res_hevc;

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

static struct msg_que_st hevc_que_data;
static struct msg_que_cmd g_catcmd_que_data;

LadiH265Source* LadiH265Source::createNew(UsageEnvironment& env) {
	DEBUG_PRINT(("====== LadiH265Source::createNew ======\n"));

	return new LadiH265Source(env);
}

LadiH265Source::LadiH265Source(UsageEnvironment& env)
  : FramedSource(env) {

	struct msqid_ds msq_info;
	unsigned int skip_hevc = 0;

	if(!fIPCInit){
		LadiH265Source::IPCCreate();
		fIPCInit = true;
		memset(&res_hevc, 0, sizeof(res_hevc));
	}

	if(!fEncoder){
#if 1
		// clean the queue
		if (msgctl(msgid, IPC_STAT, &msq_info) < 0) {
			printf("%s msgctl hevc failed with error: %d\n", __func__, errno);
		}

		do {
			msgrcv(msgid, &hevc_que_data, sizeof(struct msg_que_st) - sizeof(long), 2, IPC_NOWAIT);
			skip_hevc++;
		} while(skip_hevc < msq_info.msg_qnum);

		g_catcmd_que_data.msg_type = 4;
		g_catcmd_que_data.cmd = 0;
		g_catcmd_que_data.sub = 0xb;
		g_catcmd_que_data.data = 0x8;
		if (msgsnd(msgid_catcmd, &g_catcmd_que_data, sizeof(g_catcmd_que_data) - sizeof(long), IPC_NOWAIT) == -1) {
			printf("%s, %d, catcmd msgsnd enable failed\n", __func__, __LINE__);
		} else {
			printf("%s, %d, catcmd msgsnd enable OK\n", __func__, __LINE__);
		}
#endif
#if 0
		hevc_que_data.msg_type = 4;
		hevc_que_data.ctrl_parameter = 0x1234;
		if (msgsnd(msgid, &hevc_que_data, sizeof(hevc_que_data) - sizeof(long), IPC_NOWAIT) == -1)
		{
			DEBUG_PRINT(("====== hevc msgsnd enable failed ======\n"));
			printf("%s, %d, hevc msgsnd enable failed\n", __func__, __LINE__);
		}
#endif
		fEncoder = true;
	}
}

LadiH265Source::~LadiH265Source() {
}

void LadiH265Source::doGetNextFrame() {
	//struct timeval dbg_time;
	DEBUG_PRINT(("====== LadiH265Source::doGetNextFrame ======\n"));
	//gettimeofday(&dbg_time, NULL);
	//printf("hevc read %ds, %dus\n", (unsigned)dbg_time.tv_sec, (unsigned)dbg_time.tv_usec);
	nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc*)ReadDataHandler, this);
	//LadiH265Source::doReadData();
}

void LadiH265Source::doStopGettingFrames() {
	DEBUG_PRINT(("====== LadiH265Source::doStopGettingFrames ======\n"));

	if(fEncoder != False)
	{
#if 1
		g_catcmd_que_data.msg_type = 4;
		g_catcmd_que_data.cmd = 0;
		g_catcmd_que_data.sub = 0xb;
		g_catcmd_que_data.data = 0x9;
		if (msgsnd(msgid_catcmd, &g_catcmd_que_data, sizeof(g_catcmd_que_data) - sizeof(long), IPC_NOWAIT) == -1)
		{
			printf("%s, %d, catcmd msgsnd enable failed\n", __func__, __LINE__);
		} else {
			printf("%s, %d, catcmd msgsnd enable OK\n", __func__, __LINE__);
		}
#endif
#if 0
		hevc_que_data.msg_type = 4;
		hevc_que_data.ctrl_parameter = 0x5678;
		if (msgsnd(msgid, &hevc_que_data, sizeof(hevc_que_data) - sizeof(long), IPC_NOWAIT) == -1)
		{
			DEBUG_PRINT(("====== hevc msgsnd disable failed ======\n"));
		}
#endif
		fEncoder = False;
	}
	LadiH265Source::IPCDelete();
	envir().taskScheduler().unscheduleDelayedTask(nextTask());
}

void LadiH265Source::ReadDataHandler(LadiH265Source* source) {
  DEBUG_PRINT(("====== LadiH265Source::ReadDataHandler ======\n"));
  if (!source->isCurrentlyAwaitingData()) {
  	DEBUG_PRINT(("====== LadiH265Source::ReadDataHandler:isCurrentlyAwaitingData ======\n"));
    //source->doStopGettingFrames(); // we're not ready for the data yet
    return;
  }
  source->doReadData();
}

void LadiH265Source::doReadData() {

  DEBUG_PRINT(("====== LadiH265Source::doReadData ======\n"));
  unsigned long long ofs_us;
  static unsigned long long base_tp;
  void *mmap_vaddr, *addr;
  unsigned long fsize;

  if(msgrcv(msgid, &hevc_que_data, sizeof(hevc_que_data) - sizeof(long), 2, IPC_NOWAIT) == -1)
  {
	//printf("%s, %d, msgrcv hevc failed:%d\n", __func__, __LINE__, errno);
	nextTask() = envir().taskScheduler().scheduleDelayedTask(1000, (TaskFunc*)ReadDataHandler, this);
  }
  else
  {
	fFrameSize = hevc_que_data.frame_size;

	mmap_vaddr = LadiH265Source::fj_mm_mmap(hevc_que_data.phy_address, fFrameSize);
	// removed start code
	addr = mmap_vaddr;
	fsize = fFrameSize;
	addr += 4;
	fsize -= 4;

	memcpy(fTo, addr, fsize);
	LadiH265Source::fj_mm_munmap(mmap_vaddr, fFrameSize);

	if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
		base_tp = hevc_que_data.timestamp;
		gettimeofday(&res_hevc, NULL);

		fPresentationTime.tv_sec	= res_hevc.tv_sec;
		fPresentationTime.tv_usec	= res_hevc.tv_usec;
		printf("hevc start:%ds %dus, base_tp:%d, size:%x\n", (int)fPresentationTime.tv_sec, (int)fPresentationTime.tv_usec, (int)base_tp, (unsigned int)fFrameSize);
	} else {
		ofs_us = hevc_que_data.timestamp - base_tp;
		ofs_us = (double)ofs_us * ((double)1000000.0 / (double)90000.0); // convert 90KHz to us
		ofs_us += res_hevc.tv_usec;
		fPresentationTime.tv_sec	= res_hevc.tv_sec + (ofs_us / 1000000);
		fPresentationTime.tv_usec	= ofs_us % 1000000;
	}

	FramedSource::afterGetting(this);
  }
}

void LadiH265Source::IPCCreate() {

	DEBUG_PRINT(("====== LadiH265Source::IPCCreate ======\n"));

	msgid = msgget(MSGKEY, 0666 | IPC_CREAT);
	
	if (msgid == -1) 
	{
		printf("%s, %d, msgget failed \n", __func__, __LINE__);
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

void LadiH265Source::IPCDelete() {

	DEBUG_PRINT(("====== LadiH265Source::IPCDelete ======\n"));

	unsigned int count = 0;
	struct msqid_ds msq_info;

	if(msgctl(msgid, IPC_STAT, &msq_info) < 0)
	{
		DEBUG_PRINT(("msgctl failed with error: %d\n", errno));
	}

	do{
		msgrcv(msgid, &hevc_que_data, sizeof(hevc_que_data) - sizeof(long), 2, IPC_NOWAIT);
		count++;
	}while(count < msq_info.msg_qnum);
}

void *LadiH265Source::fj_mm_mmap(unsigned int paddr, unsigned int length) {

	DEBUG_PRINT(("====== LadiH265Source::fj_mm_mmap ======\n"));
	void *vaddr, *page_vaddr;
	unsigned int page_paddr;
	unsigned int page_offset;
	unsigned int page_length;

	page_paddr = paddr & PAGE_MASK;
	page_offset = paddr & ~PAGE_MASK;
	page_length = length + page_offset;

	page_vaddr = mmap(0, page_length, PROT_READ | PROT_WRITE,
			MAP_SHARED, mem_fd, page_paddr);

	vaddr = (void*) (page_vaddr + page_offset);

	return vaddr;
}

int LadiH265Source::fj_mm_munmap(void *vaddr, unsigned int length)
{
	unsigned int page_vaddr;
	unsigned int page_offset;
	unsigned int page_length;

	page_vaddr = (unsigned int)vaddr & PAGE_MASK;
	page_offset = (unsigned int)vaddr & ~PAGE_MASK;
	page_length = length + page_offset;

	return munmap((void*)page_vaddr, page_length);
}

int LadiH265Source::debug_dump(char* buf, unsigned long size)
{
	FILE* fp;

	int ret;

	fp = fopen("/tmp/test.265", "ab+");
	if (fp < 0) {
		printf("%s: failed to open %s \n", __func__, "/tmp/test.265");
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

unsigned LadiH265Source::maxFrameSize() const {
  return MAX_FRAME_SIZE;
}

