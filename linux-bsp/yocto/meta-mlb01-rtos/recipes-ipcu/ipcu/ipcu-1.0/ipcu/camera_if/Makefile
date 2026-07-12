ifeq ($(CC),)
	CC = /opt/poky/2.2/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc
endif

ifeq ($(LINUX_INCLUDE),)
	LINUX_INCLUDE = ../include/linux
endif

ifeq ($(INSTALL_PATH),)
	INSTALL_PATH = $(CURDIR)/../out
endif

TARGET = camera_if
SRCS = camera_if.c
OBJS = $(SRCS:.c=.o)

TARGET2 = camera_if_direct
SRCS2 = camera_if_direct.c
OBJS2 = $(SRCS2:.c=.o)

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

module: $(TARGET) $(TARGET2)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBDIR) $(LIBS)

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)

$(TARGET2): $(OBJS2)
	$(CC) $(CFLAGS) -o $@ $(OBJS2) $(LIBDIR) $(LIBS)

$(OBJS2): $(SRCS2)
	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS2)

#-----------------------------------------
clean:
	rm -f $(TARGET) $(OBJS) $(TARGET2) $(OBJS2)

install: module
	mkdir -p $(INSTALL_PATH)
	cp -f $(TARGET) $(INSTALL_PATH)
	cp -f $(TARGET2) $(INSTALL_PATH)

