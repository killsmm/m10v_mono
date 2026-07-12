CC = /opt/poky/2.2/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc

ifeq ($(LINUX_INCLUDE),)
	LINUX_INCLUDE = ../../include/linux
endif

ifeq ($(INSTALL_PATH),)
	INSTALL_PATH = $(CURDIR)/../../out/lib
endif

TARGET = libsample.so
SRCS = \
	camera_if_command.c \
	ipcu_command.c
	
OBJS = $(SRCS:.c=.o)

INCDIR = -I$(LINUX_INCLUDE) \
	-I../include \
	-I../../include

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
	$(CC) $(CFLAGS) -shared -lpthread $^ -o $@

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -fPIC $(INCDIR) -c $(SRCS)

#-----------------------------------------
clean:
	rm -f $(TARGET) $(OBJS)

install: module
	mkdir -p $(INSTALL_PATH)
	cp -f *.so $(INSTALL_PATH)
