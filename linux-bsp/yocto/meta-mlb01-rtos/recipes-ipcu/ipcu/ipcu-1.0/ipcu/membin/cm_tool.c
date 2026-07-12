#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<getopt.h>

#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<sys/mman.h>

#include <linux/ipcu_userland.h>
#include "cmfwk_std.h"

typedef unsigned int UINT32;
typedef signed int INT32;
typedef unsigned char UCHAR;
typedef unsigned short UINT16;

/******************************************************************** 
 *  Common define definition
 ********************************************************************/
#define PAGE_SHIFT 12
#define PAGE_MASK  (~((1 << PAGE_SHIFT) - 1))

#define D_NON_SHARED                (0x00000000)   /* 0:Non-shared memory mode  */
#define D_SHARED                    (0x00000001)   /* 1:Shared memory mode      */

static int mem_fd = -1;


#define USE_FILE 0

#if USE_FILE

//void *fj_mm_mmap(UINT32 paddr, UINT32 length);
//INT32 fj_mm_munmap(void *vaddr, UINT32 length);

#define COMMEM_PATH "/dev/mem"

#define IQ_BIN0_PATH ""
#define IQ_BIN1_PATH "/home/nishio/IQ_VIDEO_4K.bin"
#define IQ_BIN1_SIZE (168*1024)

#define AE_BIN0_PATH "/home/nishio/AE_COMM.bin"
#define AE_BIN1_PATH "/home/nishio/AE_S_CAP.bin"
#define AE_BIN0_SIZE 16752
#define AE_BIN1_SIZE 64

#define AWB_BIN0_PATH "/home/nishio/AWB_COMM.bin"
#define AWB_BIN0_SIZE 744

struct s_mem_files 
{
	int id;
	char *path;
	int size;
};
struct s_mem_files mem_files[4] = {
	{0x12, IQ_BIN1_PATH, IQ_BIN1_SIZE},
	{0x21, AE_BIN0_PATH, AE_BIN0_SIZE},
	{0x22, AE_BIN1_PATH, AE_BIN1_SIZE},
	{0x31, AWB_BIN0_PATH, AWB_BIN0_SIZE}
};
#else
//FJ_ERR_CODE fj_mm_open(VOID);
//FJ_ERR_CODE fj_mm_close(VOID);
#endif

void *tool_mm_mmap(UINT32 paddr, UINT32 length)
{
	void *vaddr, *page_vaddr;
	UINT32 page_paddr;
	UINT32 page_offset;
	UINT32 page_length;
	
	if (mem_fd < 0) {
		LOGE("%s: not ready \n", __func__);
		return MAP_FAILED;
	}
	
	page_paddr = paddr & PAGE_MASK;
	page_offset = paddr & ~PAGE_MASK;
	page_length = length + page_offset;
	
	page_vaddr = mmap(0, page_length, PROT_READ | PROT_WRITE, 
			MAP_SHARED, mem_fd, page_paddr);
	if (page_vaddr == MAP_FAILED) {
		return MAP_FAILED;
	}
	vaddr = page_vaddr + page_offset;
	
	LOGV("%s: paddr=0x%08x vaddr=%p length=0x%x \n", __func__, paddr, vaddr, length);
	LOGV("%s: page_paddr=0x%08x page_vaddr=%p page_length=0x%x \n", __func__, page_paddr, page_vaddr, page_length);
	
	return vaddr;
}

INT32 tool_mm_munmap(void *vaddr, UINT32 length)
{
	UINT32 page_vaddr;
	UINT32 page_offset;
	UINT32 page_length;
	
	page_vaddr = (UINT32)vaddr & PAGE_MASK;
	page_offset = (UINT32)vaddr & ~PAGE_MASK;
	page_length = length + page_offset;
	
	LOGV("%s: vaddr=%p length=0x%x \n", __func__, vaddr, length);
	LOGV("%s: page_vaddr=0x%08x page_length=0x%x \n", __func__, page_vaddr, page_length);
	
	return munmap((void*)page_vaddr, page_length);
}


char *tool_initialize(UINT32 param_mem, UINT32 param_mem_length)
{
#if USE_FILE
	int i;
	char *mem_file = NULL;
	UINT32 mem_addr = 0;
	UINT32 mem_size = 0;
	for ( i=0 ; i<4 ; i++ ){
		if ( mem_files[i].id == param_mem ){
			mem_file = mem_files[i].path;
			mem_size = mem_files[i].size;
		}
	}
	
	UINT32 addr_mem = 0;
	if ( param_mem==0 ){
		LOGE("%s: not ready \n", __func__);
		return 0;
	}else if ( mem_size!=0 ){
		mem_fd = open(mem_file, (O_RDWR /*| O_SYNC*/));
	}else{
		mem_fd = open(COMMEM_PATH, (O_RDWR /*| O_SYNC*/));	
		addr_mem = param_mem;
	}
	if (mem_fd < 0) {
		LOGE("%s: not ready \n", __func__);
		return 0;
	}
	
	char *pmem_bin;
	pmem_bin = fj_mm_mmap(addr_mem, mem_size);
	if ( pmem_bin==MAP_FAILED ){
		LOGE("%s: fj_mm_mmap failed \n", __func__);
		return 0;
	}
#else
	
	if (mem_fd == -1) {
		mem_fd = open(COMMEM_PATH, (O_RDWR | O_SYNC));
		if (mem_fd < 0) {
			LOGE("%s: failed to open %s \n", __func__, COMMEM_PATH);
			return FJ_ERR_NG;
		}
	}
	
	char *pmem_bin;
	pmem_bin = (char*)tool_mm_mmap(param_mem, param_mem_length);
	
#endif //
	return pmem_bin;
}

void tool_terminate(char *pmem_bin, UINT32 param_mem_length)
{
#if USE_FILE
	fj_mm_munmap(pmem_bin, MEM_MAP_SIZE);
	
	close(mem_fd);
#else
	tool_mm_munmap(pmem_bin, param_mem_length);
	
	close(mem_fd);
#endif 
}

#if USE_FILE
void *fj_mm_mmap(UINT32 paddr, UINT32 length)
{
	void *vaddr, *page_vaddr;
	UINT32 page_paddr;
	UINT32 page_offset;
	UINT32 page_length;
	
	if (mem_fd < 0) {
		LOGE("%s: not ready \n", __func__);
		return MAP_FAILED;
	}
	
	page_paddr = paddr & PAGE_MASK;
	page_offset = paddr & ~PAGE_MASK;
	page_length = length + page_offset;
	
	page_vaddr = mmap(0, page_length, PROT_READ | PROT_WRITE, 
			MAP_SHARED, mem_fd, page_paddr);
	if (page_vaddr == MAP_FAILED) {
		LOGV("%s: paddr=0x%08x vaddr=%p length=0x%x \n", __func__, paddr, vaddr, length);
		LOGV("%s: error %d - %s\n", __func__, errno, strerror(errno));
		return MAP_FAILED;
	}
	vaddr = page_vaddr + page_offset;
	
	LOGV("%s: paddr=0x%08x vaddr=%p length=0x%x \n", __func__, paddr, vaddr, length);
	LOGV("%s: page_paddr=0x%08x page_vaddr=%p page_length=0x%x \n", __func__, page_paddr, page_vaddr, page_length);
	
	return vaddr;
}

INT32 fj_mm_munmap(void *vaddr, UINT32 length)
{
	unsigned long page_vaddr;
	unsigned long page_offset;
	unsigned long page_length;
	
	page_vaddr = (unsigned long)vaddr & PAGE_MASK;
	page_offset = (unsigned long)vaddr & ~PAGE_MASK;
	page_length = length + page_offset;
	
	LOGV("%s: vaddr=%p length=0x%x \n", __func__, vaddr, length);
	LOGV("%s: page_vaddr=0x%08lx page_length=0x%lx \n", __func__, page_vaddr, page_length);
	
	return munmap((void*)page_vaddr, page_length);
}
#endif // USE_FILE

