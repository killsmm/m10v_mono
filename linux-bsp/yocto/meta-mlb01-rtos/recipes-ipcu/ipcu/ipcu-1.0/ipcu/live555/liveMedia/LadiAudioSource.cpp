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

#include "LadiAudioSource.hh"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>

#define H264_AUDIO_MSGKEY 0x5987
#define HEVC_AUDIO_MSGKEY 0x7895
#define MAX_TEXT 4

static Boolean fIPCInit = False;
static Boolean fEncoder = False;

#define PAGE_SHIFT 12
#define PAGE_MASK  (~((1 << PAGE_SHIFT) - 1))

static int mem_fd = -1;
static int msgid = -1;

struct timeval res_audio;

struct msg_que_st
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

struct msg_que_st audio_que_data;

LadiAudioSource* LadiAudioSource::createNew(UsageEnvironment& env, char request_source) {
  DEBUG_PRINT(("====== LadiAudioSource::createNew ======\n"));

  return new LadiAudioSource(env, request_source);
}

LadiAudioSource::LadiAudioSource(UsageEnvironment& env, char request_source)
  : FramedSource(env), frequest_source(request_source) {
	unsigned int skip_audio = 0;
	struct msqid_ds msq_info;

	DEBUG_PRINT(("====== LadiAudioSource::LadiAudioSource ======\n"));

	if(!fIPCInit){
		LadiAudioSource::IPCCreate();
		fIPCInit = true;
	}

	memset(&res_audio, 0, sizeof(res_audio));
#if 1
	if(!fEncoder){
		// clean the queue
		if (msgctl(msgid, IPC_STAT, &msq_info) < 0) {
			printf("%s msgctl audio failed with error: %d\n", __func__, errno);
		}

		do {
			msgrcv(msgid, &audio_que_data, sizeof(struct msg_que_st) - sizeof(long), 2, IPC_NOWAIT);
			skip_audio++;
		} while(skip_audio < msq_info.msg_qnum);
		fEncoder = true;

		memset(&audio_que_data, 0, sizeof(audio_que_data));
		audio_que_data.msg_type = 4;
		audio_que_data.ctrl_parameter = 0x1234;
		printf("LadiAudioSource(), enable_audio send\n");
		if (msgsnd(msgid, &audio_que_data, sizeof(audio_que_data) - sizeof(long), IPC_NOWAIT) == -1)
		{
			printf("====== audio msgsnd enable failed ======\n");
		}
	}
#endif
}

LadiAudioSource::~LadiAudioSource() {
}

void LadiAudioSource::doGetNextFrame() {
	//struct timeval dbg_time;
	DEBUG_PRINT(("====== LadiAudioSource::doGetNextFrame ======\n"));
	//gettimeofday(&dbg_time, NULL);
	//printf("audio read %ds, %dus\n", (unsigned)dbg_time.tv_sec, (unsigned)dbg_time.tv_usec);
	nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc*)ReadDataHandler, this);
}

void LadiAudioSource::doStopGettingFrames() {
  DEBUG_PRINT(("====== LadiAudioSource::doStopGettingFrames ======\n"));

  if(fEncoder != False)
  {
	memset(&audio_que_data, 0, sizeof(audio_que_data));
  	audio_que_data.msg_type = 4;
  	audio_que_data.ctrl_parameter = 0x5678;
  	if (msgsnd(msgid, &audio_que_data, sizeof(audio_que_data) - sizeof(long), IPC_NOWAIT) == -1)
  	{
		printf("====== audio msgsnd disable failed ======\n");
  	}
	fEncoder = False;
  }
  LadiAudioSource::IPCDelete();
  envir().taskScheduler().unscheduleDelayedTask(nextTask());
}

void LadiAudioSource::ReadDataHandler(LadiAudioSource* source) {
  DEBUG_PRINT(("====== LadiAudioSource::ReadDataHandler ======\n"));
  if (!source->isCurrentlyAwaitingData()) {
  	DEBUG_PRINT(("====== LadiAudioSource::ReadDataHandler:isCurrentlyAwaitingData ======\n"));
    //source->doStopGettingFrames(); // we're not ready for the data yet
    return;
  }
  source->doReadData();
}

void LadiAudioSource::doReadData() {

  DEBUG_PRINT(("====== LadiAudioSource::doReadData ======\n"));
  void *mmap_vaddr;
  unsigned long long ofs_us;
  static unsigned long long base_tp;

#if 0
  if(!fEncoder){
  		// clean the queue
  		if (msgctl(msgid, IPC_STAT, &msq_info) < 0) {
  			printf("%s msgctl hevc audio failed with error: %d\n", __func__, errno);
  		}

  		do {
  			msgrcv(msgid, &audio_que_data, sizeof(struct msg_que_st) - sizeof(long), 2, IPC_NOWAIT);
  			skip_audio++;
  		} while(skip_audio < msq_info.msg_qnum);

  		memset(&audio_que_data, 0, sizeof(audio_que_data));
  		audio_que_data.msg_type = 4;
  		audio_que_data.ctrl_parameter = 0x1234;
  		printf("LadiAudioSource(), enable_audio send\n");
  		if (msgsnd(msgid, &audio_que_data, sizeof(audio_que_data) - sizeof(long), IPC_NOWAIT) == -1)
  		{
  			printf("====== audio msgsnd enable failed ======\n");
  		}
  		fEncoder = true;
  	}
#endif

  if(msgrcv(msgid, &audio_que_data, sizeof(audio_que_data) - sizeof(long), 2, IPC_NOWAIT) == -1)
  {
	  //printf("msgrcv audio failed with error: %d\n", errno);
	  nextTask() = envir().taskScheduler().scheduleDelayedTask(1000, (TaskFunc*)ReadDataHandler, this);
  }
  else
  {
	fFrameSize = audio_que_data.frame_size;
	//printf("%s, %d, fFrameSize:%d\n", __func__, __LINE__, fFrameSize);
	mmap_vaddr = LadiAudioSource::fj_mm_mmap(audio_que_data.phy_address, fFrameSize);
	memcpy(fTo, mmap_vaddr, fFrameSize);
	LadiAudioSource::fj_mm_munmap(mmap_vaddr, fFrameSize);
#if 0
{
		static int out_fd = open("/tmp/555audio", (O_RDWR | O_CREAT | O_APPEND), 0666);
		if (out_fd < 0) {
			printf("%s: failed to open 555audio \n", __func__);
		}
		else
		{
			if (write(out_fd, mmap_vaddr, fFrameSize) != fFrameSize) {
				printf("failed to write\n");
		}
	}
}
#endif

#if 1
	if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
		base_tp = audio_que_data.timestamp;
		gettimeofday(&res_audio, NULL);
		fPresentationTime.tv_sec	= res_audio.tv_sec;
		fPresentationTime.tv_usec	= res_audio.tv_usec;
		printf("audio start:%ds %dus, base_tp:%d, size:%d\n", (unsigned long)fPresentationTime.tv_sec, (unsigned long)fPresentationTime.tv_usec, (unsigned long)base_tp, fFrameSize);
	} else {
		ofs_us = audio_que_data.timestamp - base_tp;
		ofs_us = (double)ofs_us * ((double)1000000.0 / (double)90000.0); // convert 90KHz to us
		ofs_us += res_audio.tv_usec;
		fPresentationTime.tv_sec	= res_audio.tv_sec + (ofs_us / 1000000);
		fPresentationTime.tv_usec	= ofs_us % 1000000;
	}
#else
	gettimeofday(&res_audio, NULL);
	fPresentationTime.tv_sec = res_audio.tv_sec;
	fPresentationTime.tv_usec = res_audio.tv_usec;
#endif
	FramedSource::afterGetting(this);
  }
}

void LadiAudioSource::IPCCreate() {

	DEBUG_PRINT(("====== LadiAudioSource::IPCCreate ======\n"));

	switch(frequest_source)
	{
		case VIDEO_H264:
			msgid = msgget(H264_AUDIO_MSGKEY, 0666 | IPC_CREAT);
			printf("msgid H264_AUDIO created\n");
			break;
		case VIDEO_HEVC:
			msgid = msgget(HEVC_AUDIO_MSGKEY, 0666 | IPC_CREAT);
			printf("msgid HEVC_AUDIO created\n");
			break;
		default:
			break;
	}

	if (msgid == -1) 
	{
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	mem_fd = open("/dev/mem", (O_RDWR | O_SYNC));
	if (mem_fd < 0) {
		printf("%s: failed to open\n", __func__);
		exit(EXIT_FAILURE);
	}
}

void LadiAudioSource::IPCDelete() {

	DEBUG_PRINT(("====== LadiAudioSource::IPCDelete ======\n"));

	unsigned int count = 0;
  	struct msqid_ds msq_info;

	if(msgctl(msgid, IPC_STAT, &msq_info) < 0)
	{
		DEBUG_PRINT(("msgctl failed with error: %d\n", errno));
	}

	do{
		msgrcv(msgid, &audio_que_data, sizeof(audio_que_data) - sizeof(long), 2, IPC_NOWAIT);
		count++;
	}while(count < msq_info.msg_qnum);
}

void *LadiAudioSource::fj_mm_mmap(unsigned int paddr, unsigned int length) {

	DEBUG_PRINT(("====== LadiAudioSource::fj_mm_mmap ======\n"));
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

int LadiAudioSource::fj_mm_munmap(void *vaddr, unsigned int length)
{
	unsigned int page_vaddr;
	unsigned int page_offset;
	unsigned int page_length;
	
	page_vaddr = (unsigned int)vaddr & PAGE_MASK;
	page_offset = (unsigned int)vaddr & ~PAGE_MASK;
	page_length = length + page_offset;
		
	return munmap((void*)page_vaddr, page_length);
}

int LadiAudioSource::debug_dump(char* buf, unsigned long size)
{
	FILE* fp;

	int ret;

	fp = fopen("/tmp/test.m4a", "ab+");
	if (fp < 0) {
		printf("%s: failed to open %s \n", __func__, "/tmp/test.m4a");
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

unsigned LadiAudioSource::maxFrameSize() const {
  return 2048000;
}

