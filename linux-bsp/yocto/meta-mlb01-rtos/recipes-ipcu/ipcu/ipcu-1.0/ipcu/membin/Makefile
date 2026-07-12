ifeq ($(CC),)
	CC = /opt/poky/2.2/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc
endif

ifeq ($(LINUX_INCLUDE),)
	LINUX_INCLUDE = ../include/linux
endif

ifeq ($(INSTALL_PATH),)
	INSTALL_PATH = $(CURDIR)/../out
endif

TARGET = cm_tool.o
SRCS = cm_tool.c
OBJS = $(SRCS:.c=.o)

INCDIR = -I$(LINUX_INCLUDE) \
	-I../include
LIBDIR = -L../ipcu_lib/libcmfwk
LIBS = -lcmfwk 

CFLAGS := -Wall -O4
ifeq ($(SYSROOT),)
	CFLAGS += --sysroot=/opt/poky/2.2/sysroots/cortexa5-poky-linux-gnueabi/
else
	CFLAGS += --sysroot=$(SYSROOT)
endif


#-----------------------------------------
all: module

module: $(TARGET)

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)

#-----------------------------------------
clean:
	rm -f $(TARGET) $(OBJS)

install: module
