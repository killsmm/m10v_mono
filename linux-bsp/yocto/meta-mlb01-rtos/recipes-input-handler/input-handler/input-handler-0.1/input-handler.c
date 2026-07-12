#include <stdio.h>
#include <linux/input.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
struct event_status{
	int AF_status;
	int Shutter_status;
	int Cross_Center_status;

};

void INThandler(){
       exit(0);

}
#if 0
static void uvc_start(int start){
	if (start)
		system("camera_if_direct 0x8 0x2 0x1");
	else
		system("camera_if_direct 0x8 0x2 0x0");
}

static void Cross_Center_BTN_CB(int status){
        if(status){
                printf("cross_center pressed, start uvc-h264...\n");
                if ( access("/run/SD/mmcblk0p1/uvc264.sh", F_OK) != -1){ 
                        printf("uvc264.sh exists in sd card\n");
                        system("sh /run/SD/mmcblk0p1/uvc264.sh");
		}else{
			system("camera_if_direct 0x1 0x2 0x22");
                	sleep(1);
                	system("camera_if_direct 0x0 0xb 0x2");
                	sleep(1);
                	printf("start to run uvc\n");
                	sleep(1);
                	system("camera_if_direct 0x0 0xb 0x8");
		}
        }else{
                printf("Shutter pressed, stop uvc-h264\n");
		system("camera_if_direct 0xd 0x3 0x2");
		system("camera_if_direct 0x0 0xb 0x1");
        }
}
static void Shutter_BTN_CB(int status){
	//uvc_start(status);
	if(status){
		printf("Shutter pressed, start uvc-mjpeg...\n");

		if ( access("/run/SD/mmcblk0p1/uvcmjpeg.sh", F_OK) != -1){
			printf("uvcmjpeg.sh exists in sd card\n");
			system("sh /run/SD/mmcblk0p1/uvcmjpeg.sh");
		}else{
			printf("set res as 1280x720 \n");
			system("camera_if_direct 0x1 0xc 0x11"); //user_0_size = 1280x720
			sleep(1);
			system("camera_if_direct 0x0 0xb 0x2");
			sleep(1);
			uvc_start(status);
			printf("start to run uvc\n");
			//sleep(1);
			//system("camera_if_direct 0x0 0xb 0x7");
		}
	}else{
		printf("Shutter pressed, stop uvc-mjpeg\n");
		system("camera_if_direct 0x0 0xb 0x1");
		uvc_start(status);
	}
}

static void AF_BTN_CB(int status){
	if(status){
        	printf("AF pressed, start streaming\n");
		system("camera_if_direct 0x0 0x1 0x1");
		//system("camera_if_direct 0x0 0x6 0x00");
		//system("camera_if_direct 0x0 0x7 0xf");
		//system("camera_if_direct 0x0 0x8 0x70");
		//system("camera_if_direct 0x0 0x9 0x8");
		system("camera_if_direct 0x1 0xc 0x04");//user_0_size = 3840x2160
		system("camera_if_direct 0x0 0xa 0x0");
		system("camera_if_direct 0x0 0xb 0x2");
        }else{
                printf("AF pressed, stop streaming\n");
		system("camera_if_direct 0x0 0xb 0x1");
        }
	
}
#endif

static volatile int is_previewing = 1;

int main(int argc, char *argv[])
{

        int fd;
	struct input_event ev;
	struct event_status *evs;

	evs = malloc(sizeof(struct event_status));
	evs->AF_status = 0;
	evs->Shutter_status = -1;
	evs->Cross_Center_status = -1;

        fd = open("/dev/input/event0", O_RDWR, 0666);
        if (fd < 0)
                printf("input open error\n");
        else {
                signal(SIGINT, INThandler);

		while(1){
			read(fd, &ev, sizeof(ev));
			printf("ev.code = %d, ev.type = %d, ev.value = %d\n", ev.code, ev.type, ev.value);
	               	if(ev.type == 1  && ev.value == 1){//listen for EV_KEY

				switch (ev.code){
					case BTN_0:
					//	printf("BTN_0, AF, it will run uvc_ipcu.sh -usb3 later\n");
					//	sleep(0.3);
					//	printf("Please run Windows application, such as Amcap or Virtualdub\n");
					//	sleep(2);
					//	//evs->AF_status = !evs->AF_status;
					//	//AF_BTN_CB(evs->AF_status);
					//	system("sh /usr/bin/uvc_ipcu.sh -usb3");
						system("camera_if_direct 0x0 0xb 0x7");
					
					break;
                                        case BTN_1:

                                        //        printf("BTN_1, Shutter\n");
					//	system("camera_if_direct 0x0 0xb 0x7");
#if 0
						if( evs->Shutter_status == -1){
							printf("this is the first time to run uvc-mjpeg, try to run uvc-mjpeg script first\n");
							sleep(1);
							system("sh /usr/bin/uvc_ipcu.sh -usb3 f=mjpeg w=1280 h=720");
							evs->Shutter_status = 0;
						}
						evs->Shutter_status = !evs->Shutter_status;
						Shutter_BTN_CB(evs->Shutter_status);
#endif						
                                        break;
                                        case BTN_2:
                                                printf("BTN_2, Cross_up\n");
                                        break;
                                        case BTN_3:
                                                printf("BTN_3, Cross_left\n");
                                        break;
                                        case BTN_4:
                                                printf("BTN_4, Cross_down\n");
                                        break;
                                        case BTN_5:
                                                printf("BTN_5, Cross_right\n");
                                        break;
                                        case BTN_6:
                                                printf("BTN_6, Cross_center\n");
#if 0
                                                if( evs->Cross_Center_status == -1){
                                                        printf("this is the first time to run uvc-h264, try to run uvc-h264 script first\n");
                                                        sleep(1);
                                                        system("sh /usr/bin/uvc_ipcu.sh -usb3 f=h264 w=1920 h=1080");
                                                        evs->Cross_Center_status = 0;
                                                }
                                                evs->Cross_Center_status = !evs->Cross_Center_status;
                                                Cross_Center_BTN_CB(evs->Cross_Center_status);		
#endif					
                                        break;


                                        case BTN_7:
                                                printf("BTN_7, P17\n");
                                        break;
                                        case BTN_8:
                                                printf("BTN_8, P31\n");
                                        break;
                                        case BTN_9:
                                                printf("BTN_9, P32\n");
                                        break;
                                        case BTN_A:
                                                printf("BTN_A, P33\n");
						if (is_previewing){
							system("camera_if_direct 0x0 0xb 0x1");
							is_previewing = 0;
							
						}else {
							system("camera_if_direct 0x0 0xb 0x2");
							is_previewing = 1;
						}
		
                                        break;

                                        case KEY_0:
                                                printf("KEY_0, Rotary\n");
                                        break;
                                        case KEY_1:
                                                printf("KEY_1, Rotary\n");
                                        break;
                                        case KEY_2:
                                                printf("KEY_2, Rotary\n");
                                        break;
                                        case KEY_3:
                                                printf("KEY_3, Rotary\n");
                                        break;
                                        case KEY_4:
                                                printf("KEY_4, Rotary\n");
                                        break;
                                        case KEY_5:
                                                printf("KEY_5, Rotary\n");
                                        break;
                                        case KEY_6:
                                                printf("KEY_6, Rotary\n");
                                        break;
                                        case KEY_7:
                                                printf("KEY_7, Rotary\n");
                                        break;
                                        case KEY_8:
                                                printf("KEY_8, Rotary\n");
                                        break;
                                        case KEY_9:
                                                printf("KEY_9, Rotary\n");
                                        break;
                                        case KEY_A:
                                                printf("KEY_A, Rotary\n");
                                        break;
                                        case KEY_B:
                                                printf("KEY_B, Rotary\n");
                                        break;
                                        case KEY_C:
                                                printf("KEY_C, Rotary\n");
                                        break;
                                        case KEY_D:
                                                printf("KEY_D, Rotary\n");
                                        break;
                                        case KEY_E:
                                                printf("KEY_E, Rotary\n");
                                        break;
                                        case KEY_F:
                                                printf("KEY_F, Rotary\n");
                                        break;

					default:
					break;
				}
                	}

		}
        }

	return 0;
}
