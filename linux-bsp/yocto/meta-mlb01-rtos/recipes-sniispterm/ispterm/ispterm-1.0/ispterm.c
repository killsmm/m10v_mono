/******************************************************************************
 *  @file   ispterm.c
 *  @brief  sample code for RTOS terminal
 *
 *  Copyright 2016 Socionext Inc.
 ******************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <pthread.h>

#define VERSION_STRING "version 1.0"
#define PHYBASE_ADDR   0x4FE90050
#define PHYBASE_LENGTH 0x32014
#define COM_BUFSIZE    100*1024

#define SHARED_MEM_ADDR "/proc/device-tree/shared_commem/term-io-addr"
#define SHARED_MEM_SIZE "/proc/device-tree/shared_commem/term-io-size"
#define CLOSE_STRING "close ispterm"
#define MAX_LENGTH 1024
#define SHARE_MEM  "/dev/mem"
#define SERIAL_PORT "/dev/ttyGS0"
#define PAGE_SHIFT 12
#define PAGE_SIZE	(1 << PAGE_SHIFT)
#define PAGE_MASK  (~((1 << PAGE_SHIFT) - 1))

#define COM_EN_OFS  0x0
#define RX_WP_OFS   0x4
#define RX_RP_OFS   0x8
#define RX_BUF_OFS  0xC
#define TX_WP_OFS  (RX_BUF_OFS + COM_BUFSIZE + 0x0)
#define TX_RP_OFS  (RX_BUF_OFS + COM_BUFSIZE + 0x4)
#define TX_BUF_OFS (RX_BUF_OFS + COM_BUFSIZE + 0x8)

typedef struct ispcmd
{
	volatile unsigned char *iomap;
	int tty_fd;
	int mem_fd;
	volatile unsigned long *com_enable;
	volatile unsigned long *Rx_rp;
	volatile unsigned long *Rx_wp;
	volatile unsigned long *Tx_rp;
	volatile unsigned long *Tx_wp;
	unsigned long rx_string_buf;
	unsigned long tx_string_buf;
	unsigned long phyfrom;
	unsigned long phylength;
	pthread_t       poll_th;
	struct termios old_ttyarg;
}ispcmd_t;
ispcmd_t info;

static int app_abort = 0;
const char* help_msg[]=
{
	VERSION_STRING,
	"Usage:",
	"  ispcmd [t=TTY]",
	"  t=TTY        : tty path (TTY=Full path) ",
	"",
	"  The RTOS connection is closed when you input \"close ispterm\".",
	"",
	"Example:",
	"  ispcmd t=/dev/ttyGS0",
	NULL,
};

void terminate()
{
	int errno;
	
	if ( info.poll_th != (pthread_t)NULL ) {
		pthread_cancel(info.poll_th);
		pthread_join(info.poll_th, NULL);
		info.poll_th = (pthread_t)NULL;
	}
	*info.com_enable = 0;
	if( info.tty_fd >= 0 )
	{
		ioctl(info.tty_fd, TCSETS, &info.old_ttyarg);
		close(info.tty_fd);
		info.tty_fd = -1;
	}

	if( info.iomap > 0 )
	{
		munmap((void*)info.iomap,info.phylength);
		info.iomap = (void*)-1;
	}
	if( info.mem_fd > 0 )
	{
		close(info.mem_fd);
		info.mem_fd = -1;
	}
}

static void signal_handler(int signal)
{
	app_abort = 1;
}
void* poll_thread(void *p)
{
	volatile unsigned long *wp;
	volatile unsigned long *rp;
	volatile unsigned long *p_adr;

	rp = (unsigned long*)info.Rx_rp;
	wp = (unsigned long*)info.Rx_wp;

	while ( !app_abort )
	{
		if( *wp == *rp )
		{
			usleep(40 * 1000);//wait for receive
		}
		else
		{
			p_adr = (unsigned long*)(*rp + info.rx_string_buf);
			write(info.tty_fd, (unsigned long*)p_adr, 1);
			if( *rp < COM_BUFSIZE -1 )
			{
				*rp = *rp + 1;
			}
			else
			{
				*rp = 0;
			}
		}
	}
	return NULL;
}
int send(void)
{
	char     in_char;
	char *buf;
	volatile char *p_adr;
	volatile unsigned long *wp;
	volatile unsigned long *rp;
	unsigned long length;
	int memcpy_cnt;

	rp = (unsigned long*)info.Tx_rp;
	wp = (unsigned long*)info.Tx_wp;
	*rp = *wp = 0;

	/* Prepare input buffer */
	buf = (char *)malloc(MAX_LENGTH);
	memset(buf, 0, MAX_LENGTH);
	length = 0;
	while ( !app_abort )
	{
		if( *wp + 1 == *rp )
		{
			usleep(40 * 1000);//wait for receiver side
			continue;
		}
		else
		{
			if (read(info.tty_fd, &in_char, 1) <= 0) {
				continue;
			}
			// Non-Control Character
			if (!(iscntrl(in_char))) {
				if (length < (MAX_LENGTH - 2)) {
					buf[length++] = in_char;
					putchar(in_char);
					fflush(NULL);
					continue;
				}
			}
			
			// Control Character
			switch (in_char)
			{
			case 0x08:	// BS
			case 0x7F:	// Del
				if (length > 0) {
					buf[--length] = '\0';
					char bs = 0x08;
					char space = ' ';
					putchar(bs);
					putchar(space);
					putchar(bs);
					fflush(NULL);
				}
				break;
			case 0x0a:	// Enter
				if( !strcmp(buf,CLOSE_STRING) )
				{
					write(info.tty_fd, "\n", 1);
					app_abort = 1;
				}
				else
				{
					buf[length++] = '\n';
					putchar(in_char);
					p_adr = (char*)(info.tx_string_buf);
					for( memcpy_cnt = 0; memcpy_cnt < length; memcpy_cnt++ )
					{
						*((char*)p_adr + *wp) = buf[memcpy_cnt];
						if( *wp + 1 >= COM_BUFSIZE )
						{
							*wp = 0;
						}
						else
						{
							*wp = *wp + 1;
						}
					}
					memset(buf, 0, MAX_LENGTH);
					length = 0;
				}
				break;
			}
		}
	}

	/* Clean up input buffer */
	free(buf);

	return 0;
}

int get_shared_val(char *str)
{
	volatile unsigned char *iomap;
	int ofs;
	FILE *fp;
	int proc_paddr;
	unsigned long proc_page;
	int share_data;

	share_data = 0;
	if( (fp = fopen(str,"rb")) == NULL )
	{
		printf("shared memory value get error");
	}
	else
	{
		proc_paddr = 0;
		proc_paddr += (getc(fp) << 24 );
		proc_paddr += (getc(fp) << 16 );
		proc_paddr += (getc(fp) << 8 );
		proc_paddr += (getc(fp) << 0 );
		fclose(fp);

		proc_page = proc_paddr & PAGE_MASK;
		ofs = proc_paddr - proc_page;

		iomap = mmap(0, (ofs + 4), PROT_READ|PROT_WRITE, MAP_SHARED, info.mem_fd, proc_page);
		share_data = *((int*)(iomap + ofs));
		munmap((void*)iomap, ofs + 4);

	//	printf( "%x\n", share_data);
	}

	return share_data;
}

int main(int argc, char *argv[])
{
	int  ret;
	int  loop;
	unsigned long arg_adr;
	unsigned long arg_sz;
	char   tty_path[255] = {0};
	char   argstr[32] = {0};
	int ofs;
	int help_on;
	//for tty
	struct termios new_ttyarg;
	struct sigaction act;

	//for pollin thread
	pthread_attr_t attr;
	struct sched_param param;

	help_on = 0;
	ret = 0;
	info.mem_fd = -1;
	info.tty_fd = -1;
	info.iomap = (void*)-1;
	arg_adr = arg_sz = 0;
	if (argc != 1)
	{
		for(loop = 1; loop < argc; loop++) {
			if( (*((int*)argv[loop]) & 0x0000FFFF) == 0x00003d61 ) // "a="
			{
				strncpy(argstr, &argv[loop][2], 8);
				arg_adr = strtoul(argstr, NULL, 16);
			}
			if( (*((int*)argv[loop]) & 0xFFFFFFFF) == 0x78303d61 ) // "a=0x"
			{
				strncpy(argstr, &argv[loop][4], 8);
				arg_adr = strtoul(argstr, NULL, 16);
			}
			if( (*((int*)argv[loop]) & 0x0000FFFF) == 0x00003d73 ) // "s="
			{
				strncpy(argstr, &argv[loop][2], 8);
				arg_sz = strtoul(argstr, NULL, 16);
			}
			if( (*((int*)argv[loop]) & 0xFFFFFFFF) == 0x78303d73 ) // "s=0x"
			{
				strncpy(argstr, &argv[loop][4], 8);
				arg_sz = strtoul(argstr, NULL, 16);
			}
			if( (*((int*)argv[loop]) & 0x0000FFFF) == 0x00003d74 ) // "t="
			{
				strncpy(tty_path, &argv[loop][2], sizeof(tty_path));
			}
			if( strcmp("-h",argv[1]) == 0 || strcmp("--help",argv[1]) == 0 )
			{
				help_on = 1;
			}
		}
	}

	if( help_on || tty_path[0] == 0 )
	{
		for(loop = 0; help_msg[loop] != NULL; loop++) {
			printf("%s\n",help_msg[loop]);
		}

	}
	else
	{
		info.mem_fd=open(SHARE_MEM, O_RDWR);
		if( info.mem_fd <= 0 )
		{
			perror(SHARE_MEM);
			exit(1);
		}
		else
		{
			arg_adr = get_shared_val(SHARED_MEM_ADDR);
			arg_sz =  get_shared_val(SHARED_MEM_SIZE);

			if( arg_adr == 0 )
			{
				arg_adr = PHYBASE_ADDR;
			}
			if(arg_sz == 0)
			{
				arg_sz = PHYBASE_LENGTH;
			}

			info.phyfrom = arg_adr & PAGE_MASK;
			ofs = arg_adr - info.phyfrom;
			info.phylength = arg_sz + ofs;

	//		printf("0x%08X : 0x%08X : 0x%08X : 0x%08X \n",PAGE_MASK, ofs, info.phyfrom, info.phylength);
			info.iomap = mmap(0, info.phylength, PROT_READ|PROT_WRITE, MAP_SHARED, info.mem_fd, info.phyfrom);
			if( info.iomap < 0 )
			{
				perror(SHARE_MEM);
				exit(1);
			}
			else
			{
				info.com_enable = (unsigned long*)(ofs + info.iomap + COM_EN_OFS);
				info.Rx_rp      = (unsigned long*)(ofs + info.iomap + RX_RP_OFS);
				info.Rx_wp      = (unsigned long*)(ofs + info.iomap + RX_WP_OFS);
				info.Tx_rp      = (unsigned long*)(ofs + info.iomap + TX_RP_OFS);
				info.Tx_wp      = (unsigned long*)(ofs + info.iomap + TX_WP_OFS);
				info.rx_string_buf = (unsigned long)(ofs + info.iomap + RX_BUF_OFS);
				info.tx_string_buf = (unsigned long)(ofs + info.iomap + TX_BUF_OFS);
			}
		}
		
		memset(&act, 0, sizeof(act));
		act.sa_handler = signal_handler;
		act.sa_flags = SA_RESETHAND;
		ret = sigaction(SIGINT, &act, NULL);
		if(ret < 0){
			printf("Error: sigaction() %s\n", strerror(errno));
			ret = -1;
		}
		if ( tty_path[0] == 0 )
		{
			//strcpy(tty_path,"/dev/pts/0");
			strcpy(tty_path,SERIAL_PORT);
		}
		info.tty_fd = open(tty_path, (O_RDWR | O_SYNC));
		if ( info.tty_fd < 0 )
		{
			printf("%s: failed to open %s \n", __func__, tty_path);
		}
		else
		{
			ioctl(info.tty_fd, TCGETS, &info.old_ttyarg);
			new_ttyarg = info.old_ttyarg;
			new_ttyarg.c_lflag &= ~(ECHO | ICANON);
			new_ttyarg.c_cc[VMIN] = 0;
			new_ttyarg.c_cc[VTIME] = 1;
			ioctl(info.tty_fd, TCSETS, &new_ttyarg);
		}
		pthread_attr_init (&attr);
		pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
		pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED);
		param.sched_priority = sched_get_priority_min(SCHED_FIFO);
		pthread_attr_setschedparam (&attr, &param);
		pthread_create(&info.poll_th, &attr, (void*)poll_thread, (void*)NULL);

		*info.com_enable = 1;
//		printf("%s : %s\n", __DATE__, __TIME__);
		send();//main loop

		terminate();
	}

	return ret;
}
