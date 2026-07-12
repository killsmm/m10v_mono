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
// C++ header

#ifndef _LADI_H265_SOURCE_HH
#define _LADI_H265_SOURCE_HH

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#ifndef _FRAMED_SOURCE_HH
#include "FramedSource.hh"
#endif

//#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x) do {} while (0)
#endif

class LadiH265Source: public FramedSource {
public:
  static LadiH265Source* createNew(UsageEnvironment& env);
  // redefined virtual functions:
  virtual void doGetNextFrame();
  virtual void doStopGettingFrames();

protected:
  LadiH265Source(UsageEnvironment& env);
  // called only by createNew();  
  virtual ~LadiH265Source();
  static void ReadDataHandler(LadiH265Source* source);
  static void IPCCreate();
  void IPCDelete();
  void doReadData();
  void *fj_mm_mmap(unsigned int paddr, unsigned int length);
  int fj_mm_munmap(void *vaddr, unsigned int length);
  int debug_dump(char* buf, unsigned long size);

private:
  virtual unsigned maxFrameSize() const;
};

#endif
