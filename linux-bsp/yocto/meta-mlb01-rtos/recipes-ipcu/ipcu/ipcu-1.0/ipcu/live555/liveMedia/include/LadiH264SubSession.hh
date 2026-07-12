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
// on demand, from a H264 Elementary Stream video file.
// C++ header

#ifndef _LADI_H264_SUBSESSION_HH
#define _LADI_H264_SUBSESSION_HH

#include "LadiH264Source.hh"

#ifndef _ON_DEMAND_SERVER_MEDIA_SUBSESSION_HH
#include "OnDemandServerMediaSubsession.hh"
#endif

class LadiH264SubSession: public OnDemandServerMediaSubsession {
public:
  static LadiH264SubSession* createNew(UsageEnvironment& env, char request_source);

protected:
  LadiH264SubSession(UsageEnvironment& env, char request_source);
      // called only by createNew();
  virtual ~LadiH264SubSession();

protected: // redefined virtual functions
  virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
  virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);
  char frequest_source;
  virtual char const* getAuxSDPLine(RTPSink* rtpSink,
  				    FramedSource* inputSource);
  virtual char const* sdpLines();

private:
  LadiH264Source *fH264InputSource;
};

#endif
