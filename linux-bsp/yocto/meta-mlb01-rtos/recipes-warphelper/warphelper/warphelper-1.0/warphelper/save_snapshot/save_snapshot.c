/******************************************************************************/
/**
 *  @file   save_snapshot.c
 *  @brief  snapshot save support application
 */
/*
 *  Copyright 2016 Socionext Inc.
 ******************************************************************************/
#include "save_snapshot.h"
#include "snapshot_common.h"
#include <sys/ioctl.h>
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
	volatile T_SAVE_SNAPSHOT_IPCU_IF*	snapshot_request;
	void* map_p;
	INT32 ret = D_SYS_NO_ERR;
	INT32 operation = D_SAVE_SNAPSHOT_OPERATION_NONE;

	ssize_t shmem_ret;
	unsigned char shmem[4];
	unsigned int shmem_addr;
	unsigned int shmem_offset;
	unsigned long *shmem_p;
	unsigned int buff_addr;
	unsigned int buff_offset;

	int page_size = sysconf(_SC_PAGESIZE);

	/* open syslog */
	openlog("save_snapshot", LOG_PID, LOG_DAEMON);

	if(strcmp(argv[1], "start") == 0){
		operation = D_SAVE_SNAPSHOT_OPERATION_START;
	}
	else if(strcmp(argv[1], "complete") == 0){
		operation = D_SAVE_SNAPSHOT_OPERATION_COMPLETE;
	}
	else{
		ERROR_PRINT("%s argument error!", __func__);
		return D_SYS_ERR_SYSCALL;
	}

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
			ERROR_PRINT("%s IPCU send ch open error! %s\n", __func__, strerror(errno));
			close(send_fd);
			return D_SYS_ERR_SYSCALL;
		}
		DEBUG_PRINT("%s IPCU send ch open OK!\n", __func__);
	}
	else{
		ERROR_PRINT("%s IPCU send device open error!\n", __func__);
		return D_SYS_ERR_SYSCALL;
	}

	recv_fd = open(IPCU_DEV_NAME3_UNIT0, 2);
	if (recv_fd != D_SYS_ERR_SYSCALL) {
		// open IPCU ch3
		recv_ioctlarg.direction = IPCU_DIR_RECV;
		ret = ioctl(recv_fd, IPCU_IOCTL_OPENCH, &recv_ioctlarg);
		if (ret == D_SYS_ERR_SYSCALL) {
			ERROR_PRINT("%s IPCU recv ch open error!\n", __func__);
			close(recv_fd);
			goto close_send_fd;
		}
		DEBUG_PRINT("%s IPCU recv ch open OK!\n", __func__);
	}
	else{
		ERROR_PRINT("%s IPCU recv device open error!", __func__);
		goto close_send_fd;
	}

	// send NF request to RTOS
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
					ERROR_PRINT("%s shmem mmap Error! %s\n", __func__, strerror(errno));
					goto close_shmem_fd;
				}

				// calculate offset
				shmem_p = (unsigned char*)shmem_p + shmem_offset;
				buff_addr = (unsigned long)*(shmem_p);
				buff_addr = buff_addr + (D_SNAPSHOT_COMMON_IPCU_BUFFER_OFFSET * D_SNAPSHOT_COMMON_IPCU_CH);

				// mmap ipcu request addr
				map_p = mmap((void*)NULL, sizeof(T_SAVE_SNAPSHOT_IPCU_IF), PROT_READ | PROT_WRITE,
						MAP_SHARED, mem_fd, buff_addr);
				if( map_p == (void *)(-1) ) {
					ERROR_PRINT("%s mmap Error! %s\n", __func__, strerror(errno));
					goto close_shmem_fd;
				}
				else{
					// Make IPCU IF between RTOS
					snapshot_request = (T_SAVE_SNAPSHOT_IPCU_IF*)map_p;

					if(operation == D_SAVE_SNAPSHOT_OPERATION_START) {
						// unmount request
						snapshot_request->command = D_SAVE_SNAPSHOT_NF_UNMOUNT_REQUEST;

						// copy snapshot parts to tmp
						system("cp -rf /usr/lib/warphelper /tmp");
						system("cp -rf /usr/bin/warpd /tmp");
						system("cp -rf /usr/bin/warphelper_command /tmp");
						system("cp -rf /usr/bin/warph_com /tmp");
						system("cp -rf /usr/bin/snapshot /tmp");

						system("cp -rf /etc/warpd_emmc_0.conf /tmp");
						system("cp -rf /usr/bin/snapshot_wrapper_emmc_0 /tmp");
						system("cp -rf /etc/warpd_emmc_1.conf /tmp");
						system("cp -rf /usr/bin/snapshot_wrapper_emmc_1 /tmp");

						system("cp -rf /etc/warpd_nf_0.conf /tmp");
						system("cp -rf /usr/bin/snapshot_wrapper_nf_0 /tmp");
						system("cp -rf /etc/warpd_nf_1.conf /tmp");
						system("cp -rf /usr/bin/snapshot_wrapper_nf_1 /tmp");
					}
					else{
						// mount request
						snapshot_request->command = D_SAVE_SNAPSHOT_NF_MOUNT_REQUEST;
					}
					snapshot_request->error_code = 0;

					send_msg.id		= E_FJ_IPCU_MAILBOX_TYPE_2;
					send_msg.buf		= buff_addr;
					send_msg.len		= sizeof(T_SAVE_SNAPSHOT_IPCU_IF);
					send_msg.cont		= 0;
					send_msg.total_len	= sizeof(T_SAVE_SNAPSHOT_IPCU_IF);

					send_req_msg.buf	= (void*)&send_msg;
					send_req_msg.len	= sizeof(send_msg);
					send_req_msg.flags	= FLAG_SEND_NOTIFY;

					// send request to RTOS
					ret = ioctl( send_fd, IPCU_IOCTL_SENDMSG, &send_req_msg);
					if( ret == D_SYS_ERR_SYSCALL ) {
						ERROR_PRINT("%s IPCU send Error! %s\n", __func__, strerror(errno));
						goto close_shmem_fd;
					}
					else{
						// message receive wait
						recv_req_msg.buf        = (void*)&recv_msg;
						recv_req_msg.len        = sizeof(recv_msg);
						recv_req_msg.flags      = FLAG_RECV_WAIT;

						ret = ioctl( recv_fd, IPCU_IOCTL_RECVMSG, &recv_req_msg );
						if( ret == D_SYS_ERR_SYSCALL ) {
							ERROR_PRINT("%s message receive Error! %s\n", __func__, strerror( errno ));
							goto close_shmem_fd;
						}

						ret = ioctl( recv_fd, IPCU_IOCTL_ACKSEND, &recv_req_msg );
						if( ret == D_SYS_ERR_SYSCALL ) {
							ERROR_PRINT("%s message ack send Error! %s\n", __func__, strerror( errno ));
							goto close_shmem_fd;
						}

						// close IPCU files
						ioctl(send_fd, IPCU_IOCTL_CLOSECH, &send_ioctlarg);
						close(send_fd);
						ioctl(recv_fd, IPCU_IOCTL_CLOSECH, &recv_ioctlarg);
						close(recv_fd);
						close(mem_fd);

						if(snapshot_request->error_code == D_SNAPSHOT_COMMON_RECV_ERR_NG) {
							if(operation == D_SAVE_SNAPSHOT_OPERATION_START) {
								ERROR_PRINT("%s NF unmount error!\n", __func__);
								return D_SYS_ERR_SYSCALL;
							}
							else{
								ERROR_PRINT("%s NF mount error!\n", __func__);
							}
						}

						if(operation == D_SAVE_SNAPSHOT_OPERATION_START) {
							if(snapshot_request->target == D_SAVE_SNAPSHOT_TARGET_EMMC_0) {
								system("LD_LIBRARY_PATH=/tmp/warphelper /tmp/warpd -c /tmp/warpd_emmc_0.conf");
								system("/tmp/warphelper_command 1 /tmp/warpd_emmc_0.conf");
							}
							else if(snapshot_request->target == D_SAVE_SNAPSHOT_TARGET_EMMC_1) {
								system("LD_LIBRARY_PATH=/tmp/warphelper /tmp/warpd -c /tmp/warpd_emmc_1.conf");
								system("/tmp/warphelper_command 1 /tmp/warpd_emmc_1.conf");
							}
							else if(snapshot_request->target == D_SAVE_SNAPSHOT_TARGET_NF_1) {
								system("LD_LIBRARY_PATH=/tmp/warphelper /tmp/warpd -c /tmp/warpd_nf_1.conf");
								system("/tmp/warphelper_command 1 /tmp/warpd_nf_1.conf");
							}
							else{
								system("LD_LIBRARY_PATH=/tmp/warphelper /tmp/warpd -c /tmp/warpd_nf_0.conf");
								system("/tmp/warphelper_command 1 /tmp/warpd_nf_0.conf");
							}
						}
					}
				}

			}
			else{
				ERROR_PRINT("%s shared memory info read Error! %s\n", __func__, strerror(errno));
				goto close_shmem_fd;
			}
		}
		else{
			ERROR_PRINT("%s shared memory info open Error! %s\n", __func__, strerror(errno));
			goto close_mem_fd;
		}
	}
	else{
		ERROR_PRINT("%s common memory open Error! %s\n", __func__, strerror(errno));
		goto close_send_fd;
	}
	/* close syslog */
	closelog();
	return D_SYS_NO_ERR;

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
