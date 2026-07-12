/******************************************************************************/
/*
/*  Copyright 2018 Socionext Inc.
/*
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <endian.h>

#include "sample_linux_app.h"

static int fd; // file descriptor
static int page_size;

volatile char gCamera_mode;
volatile char gVideo_switch;

void polling_camera_switch(void)
{
	FILE *fp;
	int data = 0;

	printf("Debug: polling shutter key (no signal)\n");

	while(1){
		system("echo 271 > /sys/class/gpio/export");
		fp = fopen("/sys/class/gpio/gpio271/value","r");
		if(fp == NULL) {
			printf("Error: /sys/class/gpio/gpio271/value file not open\n");
			return -1;
		}
		fscanf(fp, "%d", &data);
		fclose(fp);
		system("echo 271 > /sys/class/gpio/unexport");
		if(data == 1){
			gCamera_mode = VIDEO_MODE;
			printf("Debug: on P17(video capture)\n");
			while(1){
				system("echo 271 > /sys/class/gpio/export");
				fp = fopen("/sys/class/gpio/gpio271/value","r");
				if(fp == NULL) {
					printf("Error: /sys/class/gpio/gpio271/value file not open\n");
					return -1;
				}
				fscanf(fp, "%d", &data);
				fclose(fp);
				system("echo 271 > /sys/class/gpio/unexport");
				if(data == 0){
					printf("Debug: off P17(video capture)\n");
					gVideo_switch = 1;
					break;
				}
			}
			break;
		}

		system("echo 281 > /sys/class/gpio/export");
		fp = fopen("/sys/class/gpio/gpio281/value","r");
		if(fp == NULL) {
			printf("Error: /sys/class/gpio/gpio281/value file not open\n");
			return -1;
		}
		fscanf(fp, "%d", &data);
		fclose(fp);
		system("echo 281 > /sys/class/gpio/unexport");
		if(data == 1){
			gCamera_mode = STILL_MODE;
			printf("Debug: on P31(still capture)\n");
			while(1){
				system("echo 281 > /sys/class/gpio/export");
				fp = fopen("/sys/class/gpio/gpio281/value","r");
				if(fp == NULL) {
					printf("Error: /sys/class/gpio/gpio281/value file not open\n");
					return -1;
				}
				fscanf(fp, "%d", &data);
				fclose(fp);
				system("echo 281 > /sys/class/gpio/unexport");
				if(data == 0){
					printf("Debug: off P31(still capture)\n");
					break;
				}
			}
			break;
		}
	}

}

static void camera_if_for_video_capture(void)
{
	/* Camera_IF Video Capture */
	system("camera_if chg_camera_mode 8");
}

static void viode(void)
{
	FILE *fp;
	int data = 0;

	/* camera_if for video capture */
	camera_if_for_video_capture();

	while(1){
		system("echo 271 > /sys/class/gpio/export");
		fp = fopen("/sys/class/gpio/gpio271/value","r");
		if(fp == NULL) {
			printf("Error: /sys/class/gpio/gpio271/value file not open\n");
			return -1;
		}
		fscanf(fp, "%d", &data);
		system("echo 271 > /sys/class/gpio/unexport");
		if(data == 1){
			gVideo_switch = 0;
			system("camera_if chg_camera_mode 2");
			break;
		}
	}
}

static void led10_on(void)
{
	FILE *fp;
	int data = 0;

	system("echo 276 > /sys/class/gpio/export");
	fp = fopen("/sys/class/gpio/gpio276/direction","r+t");
	if(fp == NULL) {
		printf("Error: /sys/class/gpio/gpio276/direction file not open\n");
		return -1;
	}
	fputs("out", fp);
	fclose(fp);

	fp = fopen("/sys/class/gpio/gpio276/value","r+t");
	if(fp == NULL) {
		printf("Error: /sys/class/gpio/gpio276/value file not open\n");
		return -1;
	}
	fputc('1', fp);
	fclose(fp);
}

static void led10_off(void)
{
	FILE *fp;
	int data = 0;

	system("echo 276 > /sys/class/gpio/export");
	fp = fopen("/sys/class/gpio/gpio276/direction","r+t");
	if(fp == NULL) {
		printf("Error: /sys/class/gpio/gpio276/direction file not open\n");
		return -1;
	}
	fputs("out", fp);
	fclose(fp);

	fp = fopen("/sys/class/gpio/gpio276/value","r+t");
	if(fp == NULL) {
		printf("Error: /sys/class/gpio/gpio276/value file not open\n");
		return -1;
	}
	fputc('0', fp);
	fclose(fp);
}

static void still_capture(void)
{
	unsigned int addr = 0;
	unsigned int data = 0;

	/* Camera_IF Still Capture */
	system("camera_if chg_camera_mode 3");

	/* Blink on LED(P24) */
	led10_on();

	sleep(1);

	/* Blink off LED(P24) */
	led10_off();

	system("camera_if chg_camera_mode 2"); // Change Mode (Preview)
}

static void func_thread(void)
{
	unsigned int addr = 0;
	unsigned int data = 0;

	while(1){
		if(gCamera_mode == VIDEO_MODE){
			if(gVideo_switch == 1){
				/* Blink on LED(P24) */
				led10_on();

				usleep(500000);

				/* Blink off LED(P24) */
				led10_off();

				usleep(500000);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	pthread_t pthread;

	/* for LED blink */
	pthread_create( &pthread, NULL, &func_thread, NULL);

	while(1){
		/* polling GPIO P17=1 or P31=1 */
		polling_camera_switch();

		if(gCamera_mode == VIDEO_MODE){
			/* Video Capture */
			viode();
		}
		else if(gCamera_mode == STILL_MODE){
			/* Still Capture */
			still_capture();
		}
	}

	return 0;
}
