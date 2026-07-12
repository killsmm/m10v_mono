ifeq ($(CC),)
	CC = /opt/poky/2.2/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc
endif

OBJ_PATH = $(CURDIR)/out/usr/bin
LIB_PATH = $(CURDIR)/out/usr/lib

CFLAGS 		:=  -Werror -Wall
LDFLAGS		:= -g -static

all: uvc_host_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) uvc_host_test.c -o uvc_host_test

clean:
	rm -f *.o uvc_host_test

