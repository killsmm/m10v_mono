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
// Copyright (c) 1996-2018, Live Networks, Inc.  All rights reserved
// A test program that demonstrates how to stream - via unicast RTP
// - various kinds of file on demand, using a built-in RTSP server.
// main program

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "LadiH265Source.hh"
#include "LadiAudioSource.hh"
#include "GroupsockHelper.hh"
#include "MPEG4GenericRTPSink.hh"
#include "SimpleRTPSink.hh"
#include "uLawAudioFilter.hh"
#include "H265VideoStreamDiscreteFramer.hh"

#define AUDIO_MAX_FRAME_SIZE 20480
#define VIDEO_MAX_FRAME_SIZE 3000000

#define D_AUDIO_NONE	0
#define D_AUDIO_PCM		1
#define D_AUDIO_AAC		2

UsageEnvironment* env;
H265VideoStreamDiscreteFramer* videoSource;
FramedSource* audioSource;
LadiAudioSource* audioSource_temp;
RTPSink* videoSink;
RTPSink* audioSink;

static char audio_fmt = D_AUDIO_NONE;
static unsigned int sampling = 48000;

void play(); // forward
static void usage(void);

static void usage()
{
	printf("Usage:\n");
	printf("live555_rtsp_mul_hevc <Param1> <Param2>\n");
	printf("  Param1: pcm or aac\n");
	printf("  Param2: audio sampling rate like 48000\n");

	printf("Example:\n");
	printf("  live555_rtsp_mul_hevc pcm 48000\n");
}

static unsigned int set_argv(char *param)
{
	unsigned int ret_param;


	if (strlen(param) > 2) {
		if (param[0] == '0' && (param[1] == 'x' || param[1] == 'X'))
			ret_param = (unsigned int)strtoul(param, NULL, 16);
		else
			ret_param = (unsigned int)strtoul(param, NULL, 10);
	} else {
		ret_param = (unsigned int)strtoul(param, NULL, 10);
	}

	return ret_param;
}

int main(int argc, char** argv) {
	/* Parameter Check */
	if (argc <= 1 || argc >= 4) {
		usage();
		return 0;
	}

	if (strcmp(argv[1], "pcm") == 0) {
		audio_fmt = D_AUDIO_PCM;
	} else if (strcmp(argv[1], "aac") == 0) {
		audio_fmt = D_AUDIO_AAC;
	} else if (strcmp(argv[1], "none") == 0) {
		audio_fmt = D_AUDIO_NONE;
	} else {
		usage();
		return 0;
	}

	sampling = set_argv(argv[2]);
	printf("audio sampling:%d\n", sampling);

	// Begin by setting up our usage environment:
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);

	ServerMediaSession* sms
		= ServerMediaSession::createNew(*env, "hevcMC", NULL,
				"Session streamed by \"hevcMC\"",
				True /*SSM*/);

	RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554);
	if (rtspServer == NULL) {
		*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		exit(1);
	}

	// Create 'groupsocks' for RTP and RTCP:
    struct in_addr destinationAddress;
    destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*env);
    printf("Multicast IP:%08X\n", destinationAddress.s_addr);
    // Note: This is a multicast address.  If you wish instead to stream
    // using unicast, then you should use the "testOnDemandRTSPServer"
    // test program - not this test program - as a model.

    // Video -------------------------------------------------------------------------------------
    const unsigned short rtpPortNum = 18888;
    const unsigned short rtcpPortNum = rtpPortNum+1;
    const unsigned char ttl = 255;

    const Port rtpPort(rtpPortNum);
    const Port rtcpPort(rtcpPortNum);

    Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, ttl);
    rtpGroupsock.multicastSendOnly(); // we're a SSM source
    Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, ttl);
    rtcpGroupsock.multicastSendOnly(); // we're a SSM source

    // Create a 'H265 Video RTP' sink from the RTP 'groupsock':
    OutPacketBuffer::maxSize = VIDEO_MAX_FRAME_SIZE;
    videoSink = H265VideoRTPSink::createNew(*env, &rtpGroupsock, 96);

    // Create (and start) a 'RTCP instance' for this RTP sink:
    const unsigned estimatedSessionBandwidth = 80000; // in kbps; for RTCP b/w share
    const unsigned maxCNAMElen = 100;
    unsigned char CNAME[maxCNAMElen+1];
    gethostname((char*)CNAME, maxCNAMElen);
    CNAME[maxCNAMElen] = '\0'; // just in case
    RTCPInstance* rtcp
    = RTCPInstance::createNew(*env, &rtcpGroupsock,
  			    estimatedSessionBandwidth, CNAME,
  			    videoSink, NULL /* we're a server */,
  			    True /* we're a SSM source */);
    // Note: This starts RTCP running automatically

    sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));

    // Audio -------------------------------------------------------------------------------------
    if (audio_fmt != D_AUDIO_NONE) {
    	const unsigned estimatedSessionBandwidthAudio = 1536; // in kbps; for RTCP b/w share
    	const unsigned short AudiortpPortNum = rtcpPortNum + 1;
		const unsigned short AudiortcpPortNum = AudiortpPortNum+1;

		const Port rtpPortAudio(AudiortpPortNum);
		const Port rtcpPortAudio(AudiortcpPortNum);

		Groupsock* rtpGroupsockAudio;
		Groupsock* rtcpGroupsockAudio;
		rtpGroupsockAudio = new Groupsock(*env, destinationAddress, rtpPortAudio, ttl);
		rtcpGroupsockAudio = new Groupsock(*env, destinationAddress, rtcpPortAudio, ttl);
		rtpGroupsockAudio->multicastSendOnly();
		rtcpGroupsockAudio->multicastSendOnly();

		if (audio_fmt == D_AUDIO_PCM) {
			audioSink = SimpleRTPSink::createNew(*env, rtpGroupsockAudio, 112, sampling, "audio", "L16", 2);
		} else if (audio_fmt == D_AUDIO_AAC) {
			audioSink = MPEG4GenericRTPSink::createNew(*env, rtpGroupsockAudio, 97, sampling, "audio", "AAC-hbr", "1190", 2);
		}

		RTCPInstance* rtcpAudio
			= RTCPInstance::createNew(*env, rtcpGroupsockAudio,
				estimatedSessionBandwidthAudio, CNAME,
				audioSink, NULL /* we're a server */,
				True /* we're a SSM source */);
		// Note: This starts RTCP running automatically

		sms->addSubsession(PassiveServerMediaSubsession::createNew(*audioSink, rtcpAudio));
    }
    // ------ -------------------------------------------------------------------------------------

    rtspServer->addServerMediaSession(sms);

    char* url = rtspServer->rtspURL(sms);
    *env << "Play this stream using the URL \"" << url << "\"\n";
    delete[] url;

    // Start the streaming:
    *env << "Beginning streaming...\n";
    play();

    env->taskScheduler().doEventLoop(); // does not return

    return 0; // only to prevent compiler warning
}

void afterPlaying(void* /*clientData*/) {
  *env << "...done reading video from IPCU\n";
  videoSink->stopPlaying();
  Medium::close(videoSource);
  // Note that this also closes the input file that this source read from.
}

void afterPlayingAudio(void* /*clientData*/) {
  *env << "...done reading audio from IPCU\n";
  audioSink->stopPlaying();
  Medium::close(audioSource);
  // Note that this also closes the input file that this source read from.
}

void play() {
	LadiH265Source* source;

	source = LadiH265Source::createNew(*env);
	FramedSource* videoES = source;
	// Create a framer for the Video Elementary Stream:
	videoSource = H265VideoStreamDiscreteFramer::createNew(*env, videoES, false, true);
	if (videoSource == NULL){
		printf("%s, %d, return here\n", __func__, __LINE__);
		return;
	}

	// Finally, start playing:
	*env << "Beginning to read video from IPCU...\n";
	videoSink->startPlaying(*videoSource, afterPlaying, videoSink);

	if (audio_fmt != D_AUDIO_NONE) {
		// Create the video source:
		audioSource_temp = LadiAudioSource::createNew(*env, VIDEO_HEVC);
		if (audioSource_temp == NULL){
			printf("%s, %d, return here\n", __func__, __LINE__);
			return;
		}

		if (audio_fmt == D_AUDIO_PCM) {
			audioSource = EndianSwap16::createNew(*env, audioSource_temp);
		} else {
			audioSource = audioSource_temp;
		}
		// Finally, start playing:
		*env << "Beginning to read audio from IPCU...\n";
		audioSink->startPlaying(*audioSource, afterPlayingAudio, audioSink);
	}
}
