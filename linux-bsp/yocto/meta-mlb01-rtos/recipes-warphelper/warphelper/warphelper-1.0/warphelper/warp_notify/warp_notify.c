/******************************************************************************/
/**
 *  @file   warp_notify.c
 *  @brief  warp result notify application
 */
/*
 *  Copyright 2017 Socionext Inc.
 ******************************************************************************/
#include <sys/ioctl.h>
#include <warp_notify.h>
#include <snapshot_common.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <syslog.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int write_bf_addr( int bf_address )
{
	int fd;
	int ret;
	char buf[9];

	fd = open(D_WARP_NOTIFY_BF_ADDRESS_FILE_PATH, O_CREAT | O_RDWR | O_TRUNC);
	if (fd < 0) {
		printf("bf_address write file open NG! %s\n", strerror(errno));
		return fd;
	}

	snprintf(buf, sizeof(buf), "%d", bf_address);

	// write to file without LF
	ret = write(fd, buf, sizeof(buf) - 1);
	if (ret < 0) {
		printf("bf_address write file NG! %s\n", strerror(errno));
		return ret;
	}

	close(fd);

	printf("write bf_addr path = %s\n", D_WARP_NOTIFY_BF_ADDRESS_FILE_PATH);

	return 0;
}

int main( int argc, char** argv )
{
	INT32 send_fd;
	INT32 recv_fd;
	INT32 mem_fd;
	INT32 shmem_fd;
	struct ipcu_open_close_ch_argv send_ioctlarg;
	struct ipcu_open_close_ch_argv recv_ioctlarg;
	struct ipcu_send_recv_msg_argv send_req_msg;
	struct ipcu_send_recv_msg_argv recv_req_msg;
	T_IPCU_IF	send_msg;
	T_IPCU_IF	recv_msg;
	volatile T_WARP_NOTIFY_IPCU_IF*	notify_request;
	void* map_p;
	UINT32*	sync_v_addr_2;
	INT32 ret = D_SYS_NO_ERR;
	INT32 err_no;

	ssize_t shmem_ret;
	unsigned char shmem[4];
	unsigned int shmem_addr;
	unsigned int shmem_offset;
	unsigned long *shmem_p;
	unsigned int buff_addr;
	unsigned int buff_offset;

	int page_size = sysconf(_SC_PAGESIZE);

	/* open syslog */
	openlog("warp_notify", LOG_PID, LOG_DAEMON);

	memset(&recv_req_msg, 0x0, sizeof(recv_req_msg));
	memset(&send_req_msg, 0x0, sizeof(send_req_msg));
	memset(&recv_msg, 0x0, sizeof(recv_msg));
	memset(&send_msg, 0x0, sizeof(send_msg));

	// open IPCU device
	send_fd = open(IPCU_DEV_NAME2_UNIT0, 2);
	if (send_fd != D_SYS_ERR_SYSCALL) {
		// open IPCU ch2
		send_ioctlarg.direction = IPCU_DIR_SEND;
		ret = ioctl(send_fd, IPCU_IOCTL_OPENCH, &send_ioctlarg);
		if (ret == D_SYS_ERR_SYSCALL) {
			ERROR_PRINT("IPCU send ch open error! %s\n", strerror(errno));
			close(send_fd);
			return D_SYS_ERR_SYSCALL;
		}
	}
	else{
		ERROR_PRINT("IPCU send device open error! %s\n", strerror(errno));
		return D_SYS_ERR_SYSCALL;
	}

	recv_fd = open(IPCU_DEV_NAME3_UNIT0, 2);
	if (recv_fd != D_SYS_ERR_SYSCALL) {
		// open IPCU ch3
		recv_ioctlarg.direction = IPCU_DIR_RECV;
		ret = ioctl(recv_fd, IPCU_IOCTL_OPENCH, &recv_ioctlarg);
		if (ret == D_SYS_ERR_SYSCALL) {
			ERROR_PRINT("IPCU recv ch open error! %s\n", strerror(errno));
			close(recv_fd);
			goto close_send_fd;
		}
	}
	else{
		ERROR_PRINT("IPCU recv device open error! %s\n", strerror(errno));
		goto close_send_fd;
	}

	// get hibernation result request
	// open common memory
	mem_fd = open(COMMEM_PATH, 2);
	if(mem_fd != D_SYS_ERR_SYSCALL) {
		// Get Shared Memory Information
		shmem_fd = open(D_SNAPSHOT_COMMON_SHARED_COMMEM_NAME, O_RDONLY);
		if(shmem_fd != D_SYS_ERR_SYSCALL){
			shmem_ret = read(shmem_fd, shmem, 4);
			if(shmem_ret != D_SYS_ERR_SYSCALL){
				// convert endian
				shmem_addr = (shmem[0] << 24) + (shmem[1] << 16) + (shmem[2] << 8) + (shmem[3]);

				shmem_offset = shmem_addr % page_size;

				shmem_addr = shmem_addr & 0xFFFFF000;

				shmem_p = mmap((void*)NULL, page_size, PROT_READ | PROT_WRITE,
						MAP_SHARED, mem_fd, shmem_addr);
				if( shmem_p == (void *)(-1) ) {
					ERROR_PRINT("shmem mmap Error! %s\n", strerror(errno));
					goto close_shmem_fd;
				}

				// calculate offset
				shmem_p = (unsigned char*)shmem_p + shmem_offset;
				buff_addr = (unsigned long)*(shmem_p);
				buff_addr = buff_addr + (D_SNAPSHOT_COMMON_IPCU_BUFFER_OFFSET * D_SNAPSHOT_COMMON_IPCU_CH);

				// mmap ipcu request addr
				map_p = mmap((void*)NULL, sizeof(T_WARP_NOTIFY_IPCU_IF), PROT_READ | PROT_WRITE,
						MAP_SHARED, mem_fd, buff_addr);
				if( map_p == (void *)(-1) ) {
					ERROR_PRINT("mmap Error! %s\n", strerror(errno));
					goto close_shmem_fd;
				}
				else{
					// Make IPCU IF between RTOS
					notify_request = (T_WARP_NOTIFY_IPCU_IF*)map_p;

					// unmount request
					notify_request->command = D_WARP_NOTIFY_GET_HIB_RESULT_REQUEST;

					notify_request->error_code = 0;

					send_msg.id		= E_FJ_IPCU_MAILBOX_TYPE_2;
					send_msg.buf		= buff_addr;
					send_msg.len		= sizeof(T_WARP_NOTIFY_IPCU_IF);
					send_msg.cont		= 0;
					send_msg.total_len	= sizeof(T_WARP_NOTIFY_IPCU_IF);

					send_req_msg.buf	= (void*)&send_msg;
					send_req_msg.len	= sizeof(send_msg);
					send_req_msg.flags	= FLAG_SEND_NOTIFY;

					// send request to RTOS
					ret = ioctl( send_fd, IPCU_IOCTL_SENDMSG, &send_req_msg);
					if( ret == D_SYS_ERR_SYSCALL ) {
						ERROR_PRINT("IPCU send Error! %s\n", strerror(errno));
						goto close_shmem_fd;
					}
					else{
						// message receive wait
						recv_req_msg.buf        = (void*)&recv_msg;
						recv_req_msg.len        = sizeof(recv_msg);
						recv_req_msg.flags      = FLAG_RECV_WAIT;

						ret = ioctl( recv_fd, IPCU_IOCTL_RECVMSG, &recv_req_msg );
						if( ret == D_SYS_ERR_SYSCALL ) {
							ERROR_PRINT("message receive Error! %s\n", strerror( errno ));
							goto close_shmem_fd;
						}

						ret = ioctl( recv_fd, IPCU_IOCTL_ACKSEND, &recv_req_msg );
						if( ret == D_SYS_ERR_SYSCALL ) {
							ERROR_PRINT("message ack send Error! %s\n", strerror( errno ));
							goto close_shmem_fd;
						}

						// close IPCU files
						ioctl(send_fd, IPCU_IOCTL_CLOSECH, &send_ioctlarg);
						close(send_fd);
						ioctl(recv_fd, IPCU_IOCTL_CLOSECH, &recv_ioctlarg);
						close(recv_fd);
						close(mem_fd);

						if(notify_request->error_code == D_SNAPSHOT_COMMON_RECV_ERR_NG) {
							printf("Get result error!\n");
							closelog();
							return D_SYS_ERR_SYSCALL;
						}
						else{
							if (notify_request->result == D_WARP_NOTIFY_HIB_RESULT_OK) {
								PRINT("Hibernation boot OK! result = %d, bf_addr = 0x%X\n", D_WARP_NOTIFY_HIB_RESULT_HIB_BOOT_OK, notify_request->bf_address);
								write_bf_addr(notify_request->bf_address);
								closelog();
								return D_WARP_NOTIFY_HIB_RESULT_HIB_BOOT_OK;
							}
							else if (notify_request->result == D_WARP_NOTIFY_HIB_RESULT_NG) {
								err_no = -(notify_request->error_no);
								PRINT("Hibernation boot NG! %s, bf_addr = 0x%X\n", strerror(err_no), notify_request->bf_address);
								write_bf_addr(notify_request->bf_address);
								closelog();
								return err_no;
							}
							else {
								PRINT("Normal boot result = %d\n", D_WARP_NOTIFY_HIB_RESULT_NORMAL_BOOT);
								closelog();
								return D_WARP_NOTIFY_HIB_RESULT_NORMAL_BOOT;
							}
						}
					}
				}

			}
			else{
				ERROR_PRINT("shared memory info read Error! %s\n", strerror(errno));
				goto close_shmem_fd;
			}
		}
		else{
			ERROR_PRINT("shared memory info open Error! %s\n", strerror(errno));
			goto close_mem_fd;
		}
	}
	else{
		ERROR_PRINT("common memory open Error! %s\n", strerror(errno));
		goto close_send_fd;
	}

close_shmem_fd:
	close(shmem_fd);
close_mem_fd:
	close(mem_fd);
close_send_fd:
	ioctl(send_fd, IPCU_IOCTL_CLOSECH, &send_ioctlarg);
	close(send_fd);
close_recv_fd:
	ioctl(recv_fd, IPCU_IOCTL_CLOSECH, &recv_ioctlarg);
	close(recv_fd);
	closelog();
	return D_SYS_ERR_SYSCALL;
}
