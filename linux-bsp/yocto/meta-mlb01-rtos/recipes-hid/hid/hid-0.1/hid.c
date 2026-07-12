/*
 * Hidraw Userspace Example
 *
 * Copyright (c) 2010 Alan Ott <alan@signal11.us>
 * Copyright (c) 2010 Signal 11 Software
 *
 * The code may be used by anyone for any purpose,
 * and can serve as a starting point for developing
 * applications using hidraw.
 */

/* Linux */
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>

/*
 * Ugly hack to work around failing compilation on systems that don't
 * yet populate new version of hidraw.h to userspace.
 */
#ifndef HIDIOCSFEATURE
#warning Please have your distro update the userspace kernel headers
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif

/* Unix */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

/* C */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <linux/ipcu_userland.h>
#include "cmfwk_std.h"
#include "cmfwk_mm.h"
#include "cmfwk_ipcu.h"
#include <assert.h>
#include <stdarg.h>

static int app_abort = 0;
struct desc_info {
	unsigned char report_id;//85 : 01
	unsigned char usage;// 09 : 01
	unsigned char logical_min;// 15 : 00
	unsigned char logical_max;// 26: ff 00
	unsigned char report_count;// 95: 3f
	unsigned char report_size;// 75: 08
	unsigned char input;// 81: 02
	unsigned char report_id2;//85: 01
	unsigned char usage2;//09 : 01
	unsigned char output;// 91 02
};

#define PAGE_MASK  (~((1 << PAGE_SHIFT) - 1))
#define PAGE_SHIFT 12
typedef unsigned int        UINT32;

#define HID_GET_STATUS 0x0
#define HID_DEVICE_RESET 0x1
#define HID_GET_PACKAGE_VER 0x2
#define HID_SET_PACKAGE_VER 0x3
#define HID_GET_DEVICE_STATUS 0x4
#define HID_GET_INSTALLED_FW_INFO 0x5


#define HID_CAMERA_CONTROL 0x6
	#define HID_CAMERA_CONTROL_FACE 0x1 //0x6 0x1
	#define HID_CAMERA_CONTROL_H_OPEN 0x2 //0x6 0x2
	#define HID_CAMERA_CONTROL_CAM_ENABLE 0x3 //0x6 0x3
	#define HID_CAMERA_CONTROL_HWID 0x4 //0x6 x04
	#define HID_CAMERA_CONTROL_CAM_STATE 0x5 //0x6 0x5
	#define HID_CAMERA_CONTROL_CAM_PARA 0x6 //0x6 0x6
	#define HID_CAMERA_CONTROL_USER_PROFILE 0x7 //0x6 0x

#define HID_SPK_TRACKING 0x7
	#define HID_SPK_TRACKING_MODE_SW 0x1 //0x7 0x1

#define HID_VIDEO 0x8
	#define HID_VIDEO_GUI_CL 0x1 //0x8 0x1
		#define HID_VIDEO_GUI_CL_ROTATE_UP 0x1 //0x8 0x1 0x1
		#define HID_VIDEO_GUI_CL_HDR 0x2 //0x8 0x1 0x2
		#define HID_VIDEO_GUI_CL_WDR 0x3 //0x8 0x1 0x3
		#define HID_VIDEO_GUI_CL_PIC_ROTATE 0x4 //0x8 0x1 0x4
		#define HID_VIDEO_GUI_CL_AI_CL 0x5 //0x8 0x1 0x5
		#define HID_VIDEO_GUI_CL_PIP_CL 0x6 //0x8 0x1 0x6

	#define HID_VIDEO_MD 0x2 //0x8 0x2 Metadata
		#define HID_VIDEO_MD_RAW_VIDEO_STREAM 0x1 //0x8 0x2 0x1 
		#define HID_VIDEO_MD_MAN_DETECT_NUM 0x2  //0x8 0x2 0x2
		#define HID_VIDEO_MD_MAN_POS_X_Y 0x3 //0x8 0x2 0x3
		#define HID_VIDEO_MD_MAN_W_H 0x4 //0x8 0x2 0x4
		#define HID_VIDEO_MD_SPK_ACTIVE 0x5 //0x8 0x2 0x5
		#define HID_VIDEO_MD_FRAMING_POS 0x6 //0x8 0x2 0x6
		#define HID_VIDEO_MD_ACTIVE_SPK_POS 0x7 //0x8 0x2 0x7
	#define HID_VIDEO_AI_FW 0x3 //0x8 0x3 AI FW
		#define HID_VIDEO_AI_FW_VER 0x1 //0x8 0x3 0x1 firmware version

/*
#define HID_SET_MAC 0x6
#define HID_GET_MAC 0x7
#define HID_GET_PROUCT_NAME 0x8
#define HID_SET_HOST_CHASSIS_TYPE 0x9
#define HID_GET_HOST_CHASSIS_TYPE 0xA
#define HID_NIC_CONTROL 0x10
#define HID_DP_LANE_WIDTH 0x40
#define HID_GET_THERMAL_DATA 0x51
#define HID_GET_CYCLE_DOCK_PWR 0xf0
*/
#define HID_DOWNLOAD_MODE 0xff

void err_msg(const char *file, int line, const char *type, const char *fmt, ...);
#define ERROR_LOG(fmt, ...) err_msg(__FILE__, __LINE__, "ERROR", fmt, ##__VA_ARGS__)
#define ASSERT(x) assert(x)
#define CMD_ID_NOT_SET 0xFF
static UINT8 g_cmd_send_id = CMD_ID_NOT_SET;
static UINT8 g_cmd_recv_id = CMD_ID_NOT_SET;

static pthread_mutex_t g_mutex_recv = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_cond_recv = PTHREAD_COND_INITIALIZER;

static T_CPUCMD_COMMAND_ACK g_cmd_ack;
static int g_cmd_ack_result = -1;

void err_msg(const char *file, int line, const char *type, const char *fmt, ...)
{
    fprintf(stderr, "[%s] %s(%d): ", type, file, line);

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fputc('\n', stderr);
}

static void cmd_rcv_callback(UINT8 id, UINT32 pdata, UINT32 length, UINT32 cont, UINT32 total_length)
{
    if (id != g_cmd_recv_id) {
        ERROR_LOG("recv id error.(%lu)", id);
        goto error;
    }
    if ((length != sizeof(g_cmd_ack)) || (total_length != length)) {
        ERROR_LOG("recv size error.(%lu, %lu)", length, total_length);
        goto error;
    }
    if (cont == D_CONTINUE) {
        ERROR_LOG("cont error.(%lu)", cont);
        goto error;
    }

    ASSERT(!pthread_mutex_lock(&g_mutex_recv));
    memcpy(&g_cmd_ack, (void *)pdata, sizeof(g_cmd_ack));
    g_cmd_ack_result = 0;
    ASSERT(!pthread_cond_signal(&g_cond_recv));
    ASSERT(!pthread_mutex_unlock(&g_mutex_recv));

    return;

error:
    ASSERT(!pthread_mutex_lock(&g_mutex_recv));
    memset(&g_cmd_ack, 0, sizeof(g_cmd_ack));
    g_cmd_ack_result = -1;
    ASSERT(!pthread_cond_signal(&g_cond_recv));
    ASSERT(!pthread_mutex_unlock(&g_mutex_recv));
}

void ipcuhid_close()
{
    if (g_cmd_send_id != CMD_ID_NOT_SET) {
        FJ_IPCU_Close(g_cmd_send_id);
    }
    if (g_cmd_recv_id != CMD_ID_NOT_SET) {
        FJ_IPCU_SetReceiverCB(g_cmd_recv_id, NULL);
        FJ_IPCU_Close(g_cmd_recv_id);
    }
    g_cmd_send_id = CMD_ID_NOT_SET;
    g_cmd_recv_id = CMD_ID_NOT_SET;
}

int ipcuhid_open()
{
    FJ_ERR_CODE ret;

    ret = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_1, &g_cmd_send_id);
    if (ret != FJ_ERR_OK) {
        ERROR_LOG("FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_1) error.");
        goto error;
    }

    ret = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_0, &g_cmd_recv_id);
    if (ret != FJ_ERR_OK) {
        ERROR_LOG("FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_0) error.");
        goto error;
    }

    ret = FJ_IPCU_SetReceiverCB(g_cmd_recv_id, cmd_rcv_callback);
    if (ret != FJ_ERR_OK) {
        ERROR_LOG("FJ_IPCU_SetReceiverCB() error.");
        goto error;
    }

    return 0;

error:
    ipcuhid_close();
    return -1;
}

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

//const char *bus_str(int bus);
int desc_parser(struct desc_info *desc, struct hidraw_report_descriptor *src){
	int i;
	for (i = 0; i < src->size; i++){
		if(src->value[i]==0x95){
			desc->report_count = src->value[++i];
			printf("desc->report_count:%hhx \n", desc->report_count);
		}	
		//printf("%hhx ", src.value[i]);
	}
	return 0;
}

int main(int argc, char **argv)
{
	int fd, ret, count, ptr;
	int i, res, desc_size = 0;
	char buf[256];
	char cmd[256];
	struct hidraw_report_descriptor rpt_desc;
	struct hidraw_devinfo info;
	char *device = "/dev/hidg0";
	fd_set fdsv;
	struct desc_info desc = {0};
	FILE *fp;
	char str[32];
	char *loc;

	unsigned long phy_common_6 = 0x45C90050;
	char *buf_vaddr;
	char cmd_if[256];
	int retry_cnt = 0;

	if (argc > 1)
		device = argv[1];

	/* Open the Device with non-blocking reads. In real life,
	   don't use a hard coded path; use libudev instead. */
	fd = open(device, O_RDWR|O_NONBLOCK);

	if (fd < 0) {
		perror("Unable to open device");
		return 1;
	}

	memset(&rpt_desc, 0x0, sizeof(rpt_desc));
	memset(&info, 0x0, sizeof(info));
	memset(buf, 0x0, sizeof(buf));
	memset(cmd, 0x0, sizeof(cmd));


	/* Get Report Descriptor Size */
	res = ioctl(fd, HIDIOCGRDESCSIZE, &desc_size);
	if (res < 0)
		perror("HIDIOCGRDESCSIZE");
	else
		printf("Report Descriptor Size: %d\n", desc_size);

	/* Get Report Descriptor */
	rpt_desc.size = desc_size;
	res = ioctl(fd, HIDIOCGRDESC, &rpt_desc);
	if (res < 0) {
		perror("HIDIOCGRDESC");
	} else {
		printf("Report Descriptor:\n");
		for (i = 0; i < rpt_desc.size; i++)
			printf("%hhx ", rpt_desc.value[i]);
		puts("\n");
	}

	desc_parser(&desc, &rpt_desc);
	printf("desc.report_count:%hhx \n", desc.report_count);

	count = desc.report_count + 1;//63 bytes report count + 1 report id
	ptr = 0;
	while (!app_abort) {
		FD_ZERO(&fdsv);
		FD_SET(fd, &fdsv);
		//printf("count %d\n", count);

		ret = select( fd+1 , &fdsv, NULL, NULL, NULL);
		
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

		if (FD_ISSET(fd , &fdsv)){
			res = read(fd, buf, desc.report_count);
			for(i = 0; i< res; i++) //copy buf to cmd
				cmd[ptr++] = buf[i];

			count -= res;
			//printf("read %d bytes, count:%d, ptr=%d\n", res, count, ptr);
			if (res < 0) {
				perror("read");
			} else {
				if(count == 0){ //read finished
					count = desc.report_count + 1;//63 bytes report count + 1 report id
					ptr = 0;
					printf("read data:\n\t[");
					for (i = 0; i < count; i++)
						printf("%hhx ", cmd[i]);
					puts("]\n");

#if 1 //test
					/* Send a Report to the Device */
					switch(cmd[0]){
						case 1:
							printf("report ID 1, cmd[1]:%x\n", cmd[1]);
							buf[0] = cmd[0];
							buf[1] = cmd[1];
							switch(cmd[1]){
								case HID_GET_STATUS://0x00
									buf[2] = 0x0;//0x0 = success
									break;
								case HID_DEVICE_RESET://0x1
									buf[2] = 0x0;
									//RTOS cmd 1.d 2.cpu reboot
									//RTOS category cmd : 0xd 0x1f 0x
									//system("rm /media/root/MTP/download_finish_please_reboot");
									system("camera_if_direct 0xd 0x1f 0x1");
									break;
								case HID_GET_PACKAGE_VER://0x2
									buf[2] = 0x02;
									buf[3] = 0x02;
									break;
								case HID_SET_PACKAGE_VER://0x3
									break;
								case HID_GET_DEVICE_STATUS://0x4
									buf[2] = 0x02;//0x02 = default FW update error
									fp = fopen("/tmp.log", "r");
									if (fp == NULL){
										puts("Could not open tmp.log file");
										break;
									}
									while (fgets(str, 32, fp) != NULL) {
										loc = strstr(str,"0xe:0x3 successfully.");
										if( loc != NULL) {
											buf[2] = 0x0;//0x0 = success
											puts("find the 0xe:0x3 successfully.");
											break;
										}
									};
									//puts("can not find 0xe:0x3 successfully.");
									fclose(fp);
									break;
								case HID_GET_INSTALLED_FW_INFO://0x5
									buf[2] = 0xaa;
									buf[3] = 0xbb;
									buf[4] = 0xcc;
									buf[5] = 0xdd;
									break;
#if 0
								case HID_CAMERA_CONTROL://0x6
									buf[2] = cmd[2];
									switch(cmd[2]){
										case HID_CAMERA_CONTROL_FACE://0x1 0x6 0x1
											puts("0x1 0x6 0x1");
											buf[3] = 0x61;
											break;
										case HID_CAMERA_CONTROL_H_OPEN://0x1 0x6 0x2
											puts("0x1 0x6 0x2");
											buf[3] = 0x62;
											break;
										case HID_CAMERA_CONTROL_CAM_ENABLE://0x1 0x6 0x3
											puts("0x1 0x6 0x3");
											buf[3] = 0x63;
											break;
										case HID_CAMERA_CONTROL_HWID://0x1 0x6 0x4
											puts("0x1 0x6 0x4");
											buf[3] = 0x64;
											break;
										case HID_CAMERA_CONTROL_CAM_STATE://0x1 0x6 0x5
											puts("0x1 0x6 0x5");
											buf[3] = 0x65;
											break;
										case HID_CAMERA_CONTROL_CAM_PARA://0x1 0x6 0x6
											puts("0x1 0x6 0x6");
											buf[3] = 0x66;
											break;
										case HID_CAMERA_CONTROL_USER_PROFILE://0x1 0x6 0x7
											puts("0x1 0x6 0x7");
											buf[3] = 0x67;
											break;
										default:
											break;
									}
									break;
								case HID_SPK_TRACKING://0x7
									buf[2] = cmd[2];
									switch(cmd[2]){
										case HID_SPK_TRACKING_MODE_SW://0x1 0x7 0x1
											puts("0x1 0x7 0x1");
											buf[3] = 0x71;
											break;
										default:
											break;
									}
									break;
								case HID_VIDEO://0x8
									buf[2] = cmd[2];
									switch(cmd[2]){
										case HID_VIDEO_GUI_CL://0x1 0x8 0x1
											puts("0x1 0x8 0x1");
											buf[3] = cmd[3];
											switch(cmd[3]){
												case HID_VIDEO_GUI_CL_ROTATE_UP://0x1 0x8 0x1 0x1
													puts("0x1 0x8 0x1 0x1");
													buf[4] = 0x11;
													break;
												case HID_VIDEO_GUI_CL_HDR://0x1 0x8 0x1 0x2
													puts("0x1 0x8 0x1 0x2");
													buf[4] = 0x12;
													break;
												case HID_VIDEO_GUI_CL_WDR://0x1 0x8 0x1 0x3
													puts("0x1 0x8 0x1 0x3");
													buf[4] = 0x13;
													break;
												case HID_VIDEO_GUI_CL_PIC_ROTATE://0x1 0x8 0x1 0x4
													puts("0x1 0x8 0x1 0x4");
													buf[4] = 0x14;
													break;
												case HID_VIDEO_GUI_CL_AI_CL://0x1 0x8 0x1 0x5
													puts("0x1 0x8 0x1 0x5");
													buf[4] = 0x15;
													break;
												case HID_VIDEO_GUI_CL_PIP_CL://0x1 0x8 0x1 0x6
													puts("0x1 0x8 0x1 0x6");
													buf[4] = 0x16;
													break;
												default:
													break;
											}
											break;
										case HID_VIDEO_MD://0x1 0x8 0x2
											puts("0x1 0x8 0x2");
											buf[3] = cmd[3];
											switch(cmd[3]){
												case HID_VIDEO_MD_RAW_VIDEO_STREAM://0x1 0x8 0x2 0x1
													puts("0x1 0x8 0x2 0x1");
													buf[4] = 0x21;
													break;
												case HID_VIDEO_MD_MAN_DETECT_NUM://0x1 0x8 0x2 0x2
													puts("0x1 0x8 0x2 0x2");
													buf[4] = 0x22;
													break;
												case HID_VIDEO_MD_MAN_POS_X_Y://0x1 0x8 0x2 0x3
													puts("0x1 0x8 0x2 0x3");
													buf[4] = 0x23;
													break;
												case HID_VIDEO_MD_MAN_W_H://0x1 0x8 0x2 0x4
													puts("0x1 0x8 0x2 0x4");
													buf[4] = 0x24;
													break;
												case HID_VIDEO_MD_SPK_ACTIVE://0x1 0x8 0x2 0x5
													puts("0x1 0x8 0x2 0x5");
													buf[4] = 0x25;
													break;
												case HID_VIDEO_MD_FRAMING_POS://0x1 0x8 0x2 0x6
													puts("0x1 0x8 0x2 0x6");
													buf[4] = 0x26;
													break;
												case HID_VIDEO_MD_ACTIVE_SPK_POS://0x1 0x8 0x2 0x7
													puts("0x1 0x8 0x2 0x7");
													buf[4] = 0x27;
													break;
												default:
													break;
											}
											break;
										case HID_VIDEO_AI_FW://0x1 0x8 0x3
											puts("0x1 0x8 0x3");
											buf[3] = cmd[3];
											switch(cmd[3]){
												case HID_VIDEO_AI_FW_VER://0x1 0x8 0x3 0x1
													puts("0x1 0x8 0x3 0x1");
													buf[4] = 0x31;
													break;
												default:
													break;
											}
											break;
										default:
											break;
									}
									break;
								case HID_SET_MAC://0x6
									break;
								case HID_GET_MAC:// 0x7
									buf[2] = 0x28;
									buf[3] = 0xe3;
									buf[4] = 0x47;
									buf[5] = 0xac;
									buf[6] = 0xf6;
									buf[7] = 0xa6;
									break;
								case HID_GET_PROUCT_NAME://0x8
									buf[2] = 0xab;
									buf[3] = 0xcd;
									break;
								case HID_SET_HOST_CHASSIS_TYPE://0x9
									break;
								case HID_GET_HOST_CHASSIS_TYPE://0xA
									break;
								case HID_NIC_CONTROL://0x10
									break;
								case HID_DP_LANE_WIDTH://0x40
									break;
								case HID_GET_THERMAL_DATA://0x51
									break;
								case HID_GET_CYCLE_DOCK_PWR://0xf0
									break;
#endif
								case HID_DOWNLOAD_MODE://0xff
									//execute the fwupdate_extra.sh
									//puts("excute the /usr/bin/fwupdate_extra.sh");
									system("/usr/bin/fwupdate_extra.sh &");
									//puts("exit the /usr/bin/fwupdate_extra.sh");
									buf[2] = 0x0;//return 0x0 = success
									break;
								case HID_CAMERA_CONTROL://0x06
								case HID_SPK_TRACKING://0x7
								case HID_VIDEO://0x8
									puts("FOR RTOS BY PASS MODE");
									//buf_vaddr = zw_mmap(phy_common_6, 64);
									//printf("zw_mmap addr:0x%08X \n", zw_mmap(phy_common_6, 64));
									//puts("use ipcuhid_open\n");
									ipcuhid_open();
									buf_vaddr = (void*)FJ_MM_phys_to_virt(phy_common_6);
									//printf("fisrt get virtual addr:0x%08X \n",buf_vaddr);
									//puts("memset\n");
									memset(buf_vaddr, 0x0, 65);
									//puts("memcpy\n");
									memcpy(buf_vaddr+1, cmd, 64);
									//puts("use ipcuhid_close\n");
									ipcuhid_close();

#if 0
									printf("read buf_vaddr data:\n\t[");
									for (i = 0; i < 64; i++) {
										printf("%hhx ", buf_vaddr[i]);
									}
									puts("]\n");
#endif
									//update
									system("camera_if_direct 0xe 0x21 0x34");

									ipcuhid_open();
									buf_vaddr = (void*)FJ_MM_phys_to_virt(phy_common_6);
									//printf("second get virtual addr:0x%08X \n",buf_vaddr);
									while(buf_vaddr[0] != 1) {
										usleep(100000);
										retry_cnt++;
										if(retry_cnt > 50)
											break;
										printf("wait for RTOS response, retry_cnt = %d\n", retry_cnt);
									}
									if(retry_cnt <= 50)
										memcpy(buf, buf_vaddr+1, 64);
									//puts("use ipcuhid_close\n");
									ipcuhid_close();	
#if 0
									printf("read buf data:\n\t[");
									for (i = 0; i < 64; i++)
										printf("%hhx ", buf[i]);
									puts("]\n");
#endif
								default:
									break;
							}
							break;

						case 2:
							printf("report ID 2\n");
							break;

						default:
							break;
					}
					res = write(fd, buf, 64);
					if (res < 0) {
						printf("Error: %d\n", errno);
						perror("write");
					} else {
						printf("write() wrote %d bytes\n", res);
					}
#endif
					
				}
			}
		}
	}
	close(fd);
	return 0;
}
#if 0
const char *
bus_str(int bus)
{
	switch (bus) {
	case BUS_USB:
		return "USB";
		break;
	case BUS_HIL:
		return "HIL";
		break;
	case BUS_BLUETOOTH:
		return "Bluetooth";
		break;
	case BUS_VIRTUAL:
		return "Virtual";
		break;
	default:
		return "Other";
		break;
	}
}
#endif
