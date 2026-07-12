/******************************************************************************/
/*
/*  Copyright 2018 Socionext Inc.
/*
/******************************************************************************/
#ifndef __SAMPLE_LINUX_APP_H__
#define __SAMPLE_LINUX_APP_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <libgen.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sched.h>
#include <errno.h>
#include <semaphore.h>
#include <stdarg.h>

#define ADDR_GPIO_PDR10		0x1DFFB000
#define ADDR_GPIO_PDR32		0x1DFFB004

#define VIDEO_MODE	1
#define STILL_MODE	2

#endif /* __SAMPLE_LINUX_APP_H__ */
