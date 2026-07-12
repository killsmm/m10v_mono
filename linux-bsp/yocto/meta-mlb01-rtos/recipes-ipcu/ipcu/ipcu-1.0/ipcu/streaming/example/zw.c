#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include "cmfwk_mm.h"
#include <sys/mman.h>

static int app_abort = 0;

const char* help_msg[]=
{
	"Usage:",
	"  busybox nc -l -p 3333 | zw [-p ADDRESS] [-s SIZE] [-i INDEX]",
	" ",
	"zw Options:",
	"  -p ADDRESS   : Physical Memory Address to be used for RTOS to update FW",
	"                 (default address will be 0xa9343000)",
	"  -s SIZE      : Size of FW, in HEX",
	"  -i INDEX     : Index for FW type, LINUX.BIN:5, initramfs:6, DATA.bin:16",	
	NULL,
};

#if 0
static void signal_handler(int signal)
{
	app_abort = 1;
}

static void sigterm_handler(int signal)
{
	app_abort = 1;
}
#endif
void *zw_mmap(int paddr, int length)
{
	void *vaddr, *page_vaddr;
	int page_paddr;
	int page_offset;
	int page_length;
	void *ret = 0;
	static int mem_fd;

	mem_fd = open("/dev/mem", (O_RDWR | O_SYNC));
	if (mem_fd < 0)
	{
		printf("%s: not ready \n", __func__);
		ret = (void*)MAP_FAILED;
	}
	else
	{
		page_paddr = paddr & PAGE_MASK;
		page_offset = paddr & ~PAGE_MASK;
		page_length = length + page_offset;

		page_vaddr = mmap(0, page_length, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, page_paddr);
		if (page_vaddr == MAP_FAILED)
		{
			ret = (void*)MAP_FAILED;
		}
		else
		{
			vaddr = page_vaddr + page_offset;
			ret = (void*)vaddr;
		}
	}
	return (void*)ret;
}

int main(int argc, char *argv[])
{
    //struct timeval tv;
    fd_set    rfds;
	int ret, errno, loop, opt;
    int STDIN = 0, base = 0, ptr = 0;
	int rdsize, counting=0, processing_size=0;
	unsigned long pa, va, len, index;
	char cmd[256];
	float f=0;

	//param1: size	; param2:index(linux or initramfs)
	pa = (unsigned long)0xa9343000;
	len = 0;
	index = 0;
	while ((opt = getopt(argc, argv, "p:s:i:h")) != -1)
	{
		switch( opt )
		{
			case 'p':
				pa = strtoul(optarg, NULL, 16);
				break;
			case 's':
				len = strtoul(optarg, NULL, 16);
				break;
			case 'i':
				index = strtoul(optarg, NULL, 16);
				break;
			case '?':
			case 'h':
				for(loop = 0; help_msg[loop] != NULL; loop++) {
					printf("%s\n",help_msg[loop]);
				}
				return 0;
			default:
				break;
		}
	}

	printf("paddr:%lx, len:%lx, index:%lx\n", pa, len, index);

	if (pa <= 0 || len <= 0 || index <= 0){
		printf("address or size or index error \n");
		return -1;
	}
	base = 10;
	va = (unsigned long)zw_mmap(pa, len );

	while (!app_abort) {
		FD_ZERO(&rfds);
		FD_SET( STDIN, &rfds);

		/* Timeout. */
		//tv.tv_sec = 5;
		//tv.tv_usec = 0;

		ret = select( STDIN+1 , &rfds, NULL, NULL, NULL);

		if (-1 == ret) {
			printf("select error %d, %s\n", errno, strerror (errno));
			if (EINTR == errno)
				continue;

			break;
		}

		if (0 == ret) {
			printf("select timeout\n");
			continue;
		}

		if (FD_ISSET(STDIN , &rfds)){		
			rdsize = read(0, (char*)va , len);
			if( rdsize > 0){
				counting++;
				processing_size += rdsize;
				va+=rdsize;
				
				if(f > ptr){
					printf("received data %d %%\n",ptr);
					ptr += base;
					if (ptr == 100)
						printf("received data 100 %%\n");
				}
				f = (float)100*processing_size/len;
			}
		}
		if(processing_size>=len){
			printf("successful, already received %d bytes after %d times\n", processing_size, counting);
			break;
		}		
	}

	//update
	sprintf(cmd, "camera_if_direct 0xe 0x0 0x%lx", index);
	system(cmd);

	sprintf(cmd, "camera_if_direct 0xe 0x1 0x%lx", pa);
	system(cmd);

	sprintf(cmd, "camera_if_direct 0xe 0x2 0x%lx", len);
	system(cmd);

	system("camera_if_direct 0xe 0x3 0x1");
	return 0;
}
