/******************************************************************************/
/*
/*  Copyright 2018 Socionext Inc.
/*
/******************************************************************************/
#include <stdio.h>

int main(int argc, char *argv[])
{
	FILE *fp;
	int data = 0;
	int number = 0;

	fp = fopen("/sys/class/gpio/gpio284/value","r");
	if(fp == NULL) {
		printf("Error: /sys/class/gpio/gpio284/value file not open\n");
		return -1;
	}
	fscanf(fp, "%d", &data);
	//printf("Debug: P34(%d)\n", data);
	number = number | data;
	fclose(fp);

	fp = fopen("/sys/class/gpio/gpio285/value","r");
	if(fp == NULL) {
		printf("Error: /sys/class/gpio/gpio285/value file not open\n");
		return -1;
	}
	fscanf(fp, "%d", &data);
	//printf("Debug: P35(%d)\n", data);
	number = number | (data << 1);
	fclose(fp);

	fp = fopen("/sys/class/gpio/gpio286/value","r");
	if(fp == NULL) {
		printf("Error: /sys/class/gpio/gpio286/value file not open\n");
		return -1;
	}
	fscanf(fp, "%d", &data);
	//printf("Debug: P36(%d)\n", data);
	number = number | (data << 2);
	fclose(fp);

	fp = fopen("/sys/class/gpio/gpio287/value","r");
	if(fp == NULL) {
		printf("Error: /sys/class/gpio/gpio287/value file not open\n");
		return -1;
	}
	fscanf(fp, "%d", &data);
	//printf("Debug: P37(%d)\n", data);
	number = number | (data << 3);
	fclose(fp);

	number = 15 - number;

	printf("%d\n", number);

	return 0;
}
