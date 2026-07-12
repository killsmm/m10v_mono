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
#include "LadiH265SubSession.hh"
#include "LadiAudioSubSession.hh"
#include "GroupsockHelper.hh"

#define AUDIO_MAX_FRAME_SIZE 20480
#define VIDEO_MAX_FRAME_SIZE 3000000

static void usage(void);

UsageEnvironment* env;

static void usage()
{
	printf("Usage:\n");
	printf("live555_rtsp_hevc <Param1>\n");
	printf("  Param1: pcm or aac or none\n");

	printf("Example:\n");
	printf("  live555_rtsp_hevc pcm\n");
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

	char audio_fmt = D_AUDIO_PCM;

	/* Parameter Check */
	if (argc <= 0 || argc >= 3) {
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

  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  env = BasicUsageEnvironment::createNew(*scheduler);

  UserAuthenticationDatabase* authDB = NULL;

  //ReceivingInterfaceAddr = our_inet_addr("192.168.100.100");

#ifdef ACCESS_CONTROL
  // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord("username1", "password1"); // replace these with real strings
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif

  // Create the RTSP server:
  portNumBits rtspServerPortNum = 8554; // rtsp port

  RTSPServer* rtspServer = RTSPServer::createNew(*env, rtspServerPortNum, authDB);
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }

  char const* descriptionString = "RTSP/RTP stream";

  OutPacketBuffer::maxSize = VIDEO_MAX_FRAME_SIZE;

  char const* streamName = "hevc";
  ServerMediaSession* sms = ServerMediaSession::createNew(*env, streamName, streamName, descriptionString);
  sms->addSubsession(LadiH265SubSession::createNew(*env));
  if (audio_fmt != D_AUDIO_NONE) {
	  sms->addSubsession(LadiAudioSubSession::createNew(*env, VIDEO_HEVC, audio_fmt));
  }

  rtspServer->addServerMediaSession(sms);

  char* url = rtspServer->rtspURL(sms);
  *env << "Play this stream using the URL \"" << url << "\"\n";

  env->taskScheduler().doEventLoop(); // does not return

  return 0; // only to prevent compiler warning
}

