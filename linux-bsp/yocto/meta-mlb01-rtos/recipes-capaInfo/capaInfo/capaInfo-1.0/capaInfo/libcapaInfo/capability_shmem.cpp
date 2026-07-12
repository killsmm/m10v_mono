#include "capability_shmem.h"
#include <sys/ioctl.h>

CapSharedMemory* CapSharedMemory::instance_ = NULL;
pthread_mutex_t CapSharedMemory::mutex_ = PTHREAD_MUTEX_INITIALIZER;

