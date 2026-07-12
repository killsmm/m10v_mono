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
// A 'ServerMediaSubsession' object that creates new, unicast, "RTPSink"s
// on demand, from a H264 video file.
// Implementation

#include "LadiAudioSubSession.hh"
#include "MPEG4GenericRTPSink.hh"
#include "SimpleRTPSink.hh"
#include "uLawAudioFilter.hh"

#define MSGKEY_AUDIO_CATCMD 0x9535
#define MSGKEY_AUDIO_SAMPLE 0x9536
#define MAX_TEXT 4

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

static int msgid_catcmd = -1;
static int msgid_audio_samplingrate = -1;
static struct msg_que_cmd g_catcmd_que_data;
static struct msg_que_audio_sample g_audio_sample;

LadiAudioSubSession*
LadiAudioSubSession::createNew(UsageEnvironment& env, char request_source, char audio_fmt) {
  LadiAudioSubSession::IPCCreate();
  return new LadiAudioSubSession(env, request_source, audio_fmt);
}

LadiAudioSubSession::LadiAudioSubSession(UsageEnvironment& env, char request_source, char audio_fmt)
  : OnDemandServerMediaSubsession(env, True /*reuse the first source*/), frequest_source(request_source), audio_format(audio_fmt) {
}

LadiAudioSubSession::~LadiAudioSubSession() {
}

FramedSource* LadiAudioSubSession::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
	estBitrate = 1536; // 48K x 16 bits x 2 channels, Kbps, estimate

	// Create the video source:
	fAudioInputSource = LadiAudioSource::createNew(envir(), frequest_source);
	if (fAudioInputSource == NULL){
		printf("%s, %d, return here\n", __func__, __LINE__);
		return NULL;
	}

	if (audio_format == D_AUDIO_PCM) {
		return EndianSwap16::createNew(envir(), fAudioInputSource);
	} else {
		return fAudioInputSource;
	}
}

RTPSink* LadiAudioSubSession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char rtpPayloadTypeIfDynamic,
		   FramedSource* /*inputSource*/) {

	unsigned audio_samplingrate = 48000;

	g_catcmd_que_data.msg_type = 4;
	g_catcmd_que_data.cmd = 0x8000000e;
	g_catcmd_que_data.sub = 0x19;
	g_catcmd_que_data.data = 0x1;
	if (msgsnd(msgid_catcmd, &g_catcmd_que_data, sizeof(struct msg_que_cmd) - sizeof(long), IPC_NOWAIT) == -1) {
		printf("%s, %d, catcmd msgsnd enable failed\n", __func__, __LINE__);
	}

	if(msgrcv(msgid_audio_samplingrate, (void *)&g_audio_sample, sizeof(struct msg_que_audio_sample) - sizeof(long), 4, MSG_NOERROR) != -1) {
		audio_samplingrate = g_audio_sample.samplingrate;
		printf("%s, %d, audio catcmd msgrcv OK, samplingrate:%d\n", __func__, __LINE__, audio_samplingrate);
	} else {
		printf("%s, %d, audio catcmd msgrcv FAILED\n", __func__, __LINE__);
	}

	if (audio_format == D_AUDIO_PCM) {
		return SimpleRTPSink::createNew(envir(), rtpGroupsock, 112, audio_samplingrate, "audio", "L16", 2);
	} else {
		return MPEG4GenericRTPSink::createNew(envir(), rtpGroupsock, 96, audio_samplingrate, "audio", "AAC-hbr", "1190", 2);
	}
}

char const* LadiAudioSubSession::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource) {

  return OnDemandServerMediaSubsession::getAuxSDPLine(rtpSink, inputSource);
}

char const*
LadiAudioSubSession::sdpLines() {
  if (fSDPLines != NULL) {
    delete []fSDPLines;
    fSDPLines = NULL;
  }

  if (fLastStreamToken == NULL){ // the channel is not open
    OnDemandServerMediaSubsession::sdpLines();
  }else{ // the channel is open(a setup is established, however the stream may not play)
    RTPSink* dummyRTPSink = ((StreamState*)fLastStreamToken)->rtpSink();
    FramedSource *inputSource = ((StreamState*)fLastStreamToken)->mediaSource();
    setSDPLinesFromRTPSink(dummyRTPSink, inputSource, 1536);
  }

  return fSDPLines;
}

void LadiAudioSubSession::IPCCreate() {

	msgid_catcmd = msgget(MSGKEY_AUDIO_CATCMD, 0666 | IPC_CREAT);

	if (msgid_catcmd == -1) {
		printf("%s, %d, msgget failed \n", __func__, __LINE__);
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	msgid_audio_samplingrate = msgget(MSGKEY_AUDIO_SAMPLE, 0666 | IPC_CREAT);

	if (msgid_audio_samplingrate == -1) {
		printf("%s, %d, msgget failed \n", __func__, __LINE__);
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
}
