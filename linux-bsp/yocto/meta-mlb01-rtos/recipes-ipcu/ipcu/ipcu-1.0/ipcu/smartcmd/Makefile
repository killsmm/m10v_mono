ifeq ($(CC),)
	CC = /opt/poky/2.2/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc
endif

ifeq ($(LINUX_INCLUDE),)
	LINUX_INCLUDE = ../include/linux
endif

ifeq ($(INSTALL_PATH),)
	INSTALL_PATH = $(CURDIR)/../out
endif

TARGET = smartcmd
SRCS = smartcmd.c
OBJS = $(SRCS:.c=.o)

INCDIR = -I$(LINUX_INCLUDE) \
	-I../include
LIBDIR = -L../ipcu_lib/libcmfwk
LIBS = -lcmfwk -lpthread

CFLAGS := -Wall -O4
ifeq ($(SYSROOT),)
	CFLAGS += --sysroot=/opt/poky/2.2/sysroots/cortexa7-poky-linux-gnueabi/
else
	CFLAGS += --sysroot=$(SYSROOT)
endif

#-----------------------------------------
all: module

module: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBDIR) $(LIBS)

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)

#-----------------------------------------
clean:
	rm -f $(TARGET) $(OBJS)

install: module
	mkdir -p $(INSTALL_PATH)
	cp -f $(TARGET) $(INSTALL_PATH)
