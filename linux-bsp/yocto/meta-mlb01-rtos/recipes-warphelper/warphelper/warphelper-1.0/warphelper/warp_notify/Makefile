ifeq ($(CC),)
CROSS_COMPILE:=/opt/poky/2.2/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc
endif

ifeq ($(CFLAGS),)
CFLAGS:=-Wall -Werror -O4 \
	--sysroot=/opt/poky/2.2/sysroots/cortexa7-poky-linux-gnueabi/
endif

ifeq ($(INSTALL_PATH),)
INSTALL_PATH=$(CURDIR)/../out/usr/bin
endif

TARGET=warp_notify
SRCS=warp_notify.c
OBJS=$(SRCS:.c=.o)
INCDIR=-I../include \
-I../include/linux
LIBDIR=-L../out/usr/lib

#-----------------------------------------
all: module

module: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)

#-----------------------------------------
clean:
	rm -f $(TARGET) $(OBJS)

install: module
	mkdir -p $(INSTALL_PATH)
	cp -f $(TARGET) $(INSTALL_PATH)

