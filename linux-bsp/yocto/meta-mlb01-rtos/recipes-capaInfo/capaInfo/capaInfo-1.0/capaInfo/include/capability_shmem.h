#ifndef CAPABILITY_SHMEM_H_
#define CAPABILITY_SHMEM_H_

#include <stdio.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <typeinfo>
#include <sys/types.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>

#include "capability_info.h"


#define PAGE_SHIFT	12
#define PAGE_MASK	(~((1 << PAGE_SHIFT) - 1))
#define COMMEM_PATH	"/dev/mem"

class CapAutoLock
{
public:
	CapAutoLock(pthread_mutex_t* pMutex) {
		mutex_ = pMutex;
		pthread_mutex_lock(mutex_);
	}
	~CapAutoLock() {
		pthread_mutex_unlock(mutex_);
	}
private:
	pthread_mutex_t* mutex_;
};

class CapSharedMemory {
public:
	static CapSharedMemory* GetInstance() {
		CapAutoLock lock(&mutex_);
		if (! instance_) {
			instance_ = new CapSharedMemory;
			if (instance_->open() == 0) {
				if (instance_->init() != 0) {
					perror("Shared Memory Fault");
					instance_ = NULL;
				}
			}
		}
		return instance_;
	}
	static void DeleteInstance() {
		CapAutoLock lock(&mutex_);
		delete instance_;
		instance_ = NULL;
	}
	void* GetShmAddr(){return addr_;}
	unsigned long GetShmSize(){return size_;}
	std::uint32_t GetPhyAddr(){return phy_addr_;}
	std::uint32_t GetPhySize(){return phy_size_;}

private:
	CapSharedMemory() :
		fd_(0),
		dev_addr_("/proc/device-tree/shared_commem/cap-addr"),
		dev_size_("/proc/device-tree/shared_commem/cap-size")
	{
	}
	virtual ~CapSharedMemory() 
	{
		munmap((void*)map_1st, offset_1st + 4);
		munmap((void*)map_2ed, offset_2ed + 4);
		munmap((void*)map_size, offset_size + 4);
	}

	static CapSharedMemory* instance_;
	static pthread_mutex_t mutex_;

	unsigned long* addr_;
	unsigned long size_;
	std::uint32_t phy_addr_;
	std::uint32_t phy_size_;
	int fd_;
	std::ifstream in_addr;
	std::ifstream in_size;
	const char* dev_addr_;
	const char* dev_size_;
	const char* devname_;

	void* map_1st;
	unsigned int offset_1st;
	void* map_2ed;
	unsigned int offset_2ed;
	void* map_size;
	unsigned int offset_size;

	void* addPlus(void* add, unsigned int offset)
	{
		char* p = static_cast<char*>(add);
		return static_cast<void*>(p + offset);
	}

	int open()
	{
		in_addr.open(dev_addr_, std::ios::in | std::ios::binary);
		if (!in_addr) {
			perror("shared address open");
			return -1;
		}
		in_size.open(dev_size_, std::ios::in | std::ios::binary);
		if (!in_size) {
			perror("shared size open");
			return -1;
		}
		return 0;
	}

	int init()
	{
		std::uint32_t c;
		std::uint32_t s;
		std::uint32_t phy_addr;
		std::uint32_t phy_size;
		unsigned long page_1st;
		unsigned long page_2ed;
		unsigned long page_size;
		unsigned long* addr_iomap;
		unsigned long* size_iomap;

		in_addr.read((char *)&c, sizeof(std::uint32_t));
		phy_addr = static_cast<std::uint32_t>(
				(c & 0x000000FF) << 24 |
				(c & 0x0000FF00) << 8  |
				(c & 0xFF000000) >> 24 |
				(c & 0x00FF0000) >> 8);
		in_size.read((char *)&s, sizeof(std::uint32_t));
		phy_size = static_cast<std::uint32_t>(
				(s & 0x000000FF) << 24 |
				(s & 0x0000FF00) << 8  |
				(s & 0xFF000000) >> 24 |
				(s & 0x00FF0000) >> 8);
		fd_ = ::open(COMMEM_PATH, O_RDWR);
		if( fd_ <= 0 ) {
			perror("mem path open");
			return -1;
		}

		page_1st = phy_addr & PAGE_MASK;
		offset_1st = phy_addr - page_1st;

		map_1st = mmap(
				0,
				(offset_1st + 4),
				PROT_READ|PROT_WRITE,
				MAP_SHARED,
				fd_,
				page_1st);
		addr_iomap = static_cast<unsigned long*>(addPlus(map_1st, offset_1st));
		phy_addr_ = *addr_iomap;

		page_size = phy_size & PAGE_MASK;
		offset_size = phy_size - page_size;

		map_size = mmap(
				0,
				(offset_size + 4),
				PROT_READ|PROT_WRITE,
				MAP_SHARED,
				fd_,
				page_size);
		size_iomap = static_cast<unsigned long*>(addPlus(map_size, offset_size));
		size_ = *size_iomap;
		phy_size_ = *size_iomap;

		page_2ed = *addr_iomap & PAGE_MASK;
		offset_2ed = *addr_iomap - page_2ed;

		map_2ed = mmap(
				0,
				size_,
				PROT_READ|PROT_WRITE,
				MAP_SHARED,
				fd_,
				page_2ed);
		addr_ = static_cast<unsigned long*>(addPlus(map_2ed, offset_2ed));

		in_addr.close();
		in_size.close();

		return 0;
	}
};

#endif /* CAPABILITY_SHMEM_H_ */

