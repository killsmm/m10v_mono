#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "cmfwk_mm.h"
#include "cif_stream.h"
#include <sys/mman.h>

#define SDRAM_STORE_ADDRESS 0x50000000
#define SDRAM_STORE_SIZE    100*1024*1024
static int mem_fd;

struct burn_param
{
	unsigned long pa;
	unsigned long size;
	unsigned long mode;
	unsigned long va;
	char* path;
};
int debug_dump(char* buf, unsigned long size)
{
	FILE* fp;
	char* filename = "/tmp/fwupdate.rom";
	int ret;

	fp = fopen(filename, "wb");
	if (fp < 0) {
		printf("%s: failed to open %s \n", __func__, filename);
		ret = -1;
	}
	else
	{
		fwrite(buf, sizeof(char), size, fp);
		ret = 0;
	}
	fclose(fp);

	return ret;
}

const char* help_msg[]=
{
	"Usage: ",
	"   fwupdate <option>",
	" option ",
	"        -a <address>   : Physical address(HEX without \"0x\")",
	"        -s <size>      : new FW size(HEX without \"0x\")",
	"        -f <File Path> : Path to new FW binary data",
	"        -m <mode>      : reserved",
	"SAMPLE",
	"DATASZ=$(printf \"%x\\n\" `stat -c %s /run/SD/mmcblk0p1/DATA.bin`)",
	"fwupdate -a 60400000 -s $DATASZ -f /run/SD/mmcblk0p1/DATA.bin",
	__DATE__,
	__TIME__,
	NULL,
};

void *fw_mmap(int paddr, int length)
{
	void *vaddr, *page_vaddr;
	int page_paddr;
	int page_offset;
	int page_length;
	int ret;

	mem_fd = open("/dev/mem", (O_RDWR | O_SYNC));
	if (mem_fd < 0)
	{
		LOGE("%s: not ready \n", __func__);
		ret = MAP_FAILED;
	}
	else
	{
		page_paddr = paddr & PAGE_MASK;
		page_offset = paddr & ~PAGE_MASK;
		page_length = length + page_offset;

		page_vaddr = mmap(0, page_length, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, page_paddr);
		if (page_vaddr == MAP_FAILED)
		{
			ret = MAP_FAILED;
		}
		else
		{
			vaddr = page_vaddr + page_offset;
			ret = vaddr;
		}
	}

	return ret;
}
INT32 fw_munmap(void *vaddr, UINT32 length)
{
	UINT32 page_vaddr;
	UINT32 page_offset;
	UINT32 page_length;
	
	page_vaddr = (UINT32)vaddr & PAGE_MASK;
	page_offset = (UINT32)vaddr & ~PAGE_MASK;
	page_length = length + page_offset;
	close(mem_fd);

	return munmap((void*)page_vaddr, page_length);
}

int toSdram(struct burn_param* bp)
{
	FILE* fp;
	int ret;
	char* p = bp->va;

	memset(p, 0, bp->size);

	fp = fopen(bp->path, "rb");
	if (fp < 0) {
		printf("%s: failed to open %s \n", __func__, bp->path);
		ret = -1;
	}
	else
	{
		fread(p, sizeof(char), bp->size, fp);
		ret = 0;
	}
	//if (fsync (fp) < 0)
	//{
	//	printf("%d %s\n",__LINE__,strerror(errno));
	//}
	fclose(fp);
	return ret;
}
int update_start(const struct burn_param* bp)
{
	struct camera_if_cmd cmd;
	int ret;

	printf("Update Start \n");
	memset(&cmd, 0, sizeof(struct camera_if_cmd));
	cmd.cmd_set = E_CAM_IF_COM_SET_START_FWUPDATE;
	cmd.cmd = E_CAM_IF_SUB_COM_START_FWUPDATE;
	cmd.send_param[0] = 1;
	cmd.send_param[1] = bp->pa;
	cmd.send_param[2] = bp->size;
	cmd.to_sec = 30;
	ret = camera_if_command(&cmd);
	if (ret < 0 || cmd.recv_param[0] != 0) {
		printf("camera_if_command error(%d). [%08X] [%08X] [%08X] [%08X] \n",ret ,cmd.recv_param[0],cmd.recv_param[1],cmd.recv_param[2],cmd.recv_param[3]);
		ret = -1;
	}
	else
	{
		ret = 0;
	}
	return ret;
}
int main(int argc, char** argv)
{
	int ret;
	int opt;
	int loop;
	int not_update;
	struct burn_param bp;
	char file[128] = "/run/SD/mmcblk0p1/DATA.bin";

	// initialize
	mem_fd = -1;
	memset(&bp, 0, sizeof(struct burn_param));

	not_update = 0;
	while ((opt = getopt(argc, argv, "a:s:m:f:n")) != -1)
	{
		switch( opt )
		{
			case 'a':
				bp.pa = strtoul(optarg, NULL, 16);
				break;
			case 's':
				bp.size = strtoul(optarg, NULL, 16);
				break;
			case 'f':
				memset(file, 0, sizeof(file));
				memcpy(file, optarg, strlen(optarg));
				break;
			case 'n':
				not_update = 1;;
				break;
			case '?':
				for(loop = 0; help_msg[loop] != NULL; loop++) {
					printf("%s\n",help_msg[loop]);
				}
				return;
			default:
				break;
		}
	}

	// valid check
	if( bp.pa == 0 || bp.size == 0 )
	{
		printf("Invalid arg. address 0x%08X, size 0x%08X\n",bp.pa, bp.size);
	}
	else
	{
		bp.path = file;
		bp.va = (unsigned long)fw_mmap(bp.pa, bp.size);
		printf("Phys Addr 0x%08X \n",bp.pa);
		printf("Virt Addr 0x%08X \n",bp.va);
		printf("Size      0x%08X \n",bp.size);
		printf("Update File %s\n",bp.path);

		ret = toSdram(&bp);
//		debug_dump(bp.va, bp.size);
		fw_munmap(bp.va, bp.size);
		system("echo 3 > /proc/sys/vm/drop_caches");
		if( not_update == 0 )
		{
			update_start(&bp);
		}
		//printf("Finish \n");
	}
	
	return ret;
}

