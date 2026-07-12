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
// on demand, from a H265 video file.
// Implementation

#include "LadiH265SubSession.hh"
#include "H265VideoRTPSink.hh"
#include "H265VideoStreamFramer.hh"
#include "H265VideoStreamDiscreteFramer.hh"

LadiH265SubSession*
LadiH265SubSession::createNew(UsageEnvironment& env) {
  return new LadiH265SubSession(env);
}

LadiH265SubSession::LadiH265SubSession(UsageEnvironment& env)
  : OnDemandServerMediaSubsession(env, true) {
}

LadiH265SubSession::~LadiH265SubSession() {
}

FramedSource* LadiH265SubSession::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
  estBitrate = 80000; //(Kbps 80Mbps)
  
  // Create the video source:
  fH265InputSource = LadiH265Source::createNew(envir());
  if (fH265InputSource == NULL){
   	return NULL;
  }
  // Create a framer for the Video Elementary Stream:
  return H265VideoStreamDiscreteFramer::createNew(envir(), fH265InputSource, false, true);
}

RTPSink* LadiH265SubSession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char rtpPayloadTypeIfDynamic,
		   FramedSource* /*inputSource*/) {
  return H265VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}

char const* LadiH265SubSession::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource) {

  return OnDemandServerMediaSubsession::getAuxSDPLine(rtpSink, inputSource);
}

char const*
LadiH265SubSession::sdpLines() {
  if (fSDPLines != NULL) {
    delete []fSDPLines;
    fSDPLines = NULL;
  }

  if (fLastStreamToken == NULL){ // the channel is not open
    OnDemandServerMediaSubsession::sdpLines();
  }else{ // the channel is open(a setup is established, however the stream may not play)
    RTPSink* dummyRTPSink = ((StreamState*)fLastStreamToken)->rtpSink();
    FramedSource *inputSource = ((StreamState*)fLastStreamToken)->mediaSource();
    setSDPLinesFromRTPSink(dummyRTPSink, inputSource, 80000);
  }

  return fSDPLines;
}
