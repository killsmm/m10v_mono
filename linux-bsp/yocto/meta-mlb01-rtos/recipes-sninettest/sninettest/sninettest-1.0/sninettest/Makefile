CC = /opt/poky/2.2/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc

CFLAGS:=-Wall -O4 \
	--sysroot=/opt/poky/2.2/sysroots/cortexa7-poky-linux-gnueabi/


LIBS = -lpthread

all: net_init.c stream_test.c exp_test.c mil_netsec_api.h
	$(CC) $(CFLAGS) $(LIBS) net_init.c -o net_init
	$(CC) $(CFLAGS) $(LIBS) stream_test.c -o stream_test
	$(CC) $(CFLAGS) $(LIBS) exp_test.c -o exp_test

clean:
	rm -f *.o net_init stream_test exp_test
