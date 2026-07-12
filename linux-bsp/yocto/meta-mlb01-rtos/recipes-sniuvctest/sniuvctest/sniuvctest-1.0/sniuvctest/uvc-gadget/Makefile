ifeq ($(CC),)
	CC = /opt/poky/2.2/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc
endif

OBJ_PATH = $(CURDIR)/out/usr/bin
LIB_PATH = $(CURDIR)/out/usr/lib

CFLAGS 		:=  -Werror -Wall
LDFLAGS		:= -g -static

all: uvc-gadget uvc-gadget264

uvc-gadget: uvc-gadget.o
	$(CC) $(LDFLAGS) -o $@ $^

uvc-gadget264: uvc-gadget264.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o
	rm -f uvc-gadget
	rm -f uvc-gadget264

