ifeq ($(CC),)
	CC = /opt/poky/2.2/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc
endif

ifeq ($(LINUX_INCLUDE),)
	LINUX_INCLUDE = ../include/linux
endif

ifeq ($(INSTALL_PATH),)
	INSTALL_PATH = $(CURDIR)/../out
endif

TARGET = pmgr
SRCS = $(shell ls *.c | xargs -i basename {})
OBJS = $(SRCS:.c=.o)

INCDIR := -I$(LINUX_INCLUDE)
INCDIR += -I${STAGING_LIBDIR}/ipcu/include
LIBDIR := -L$(STAGING_LIBDIR)/ipcu

LIBS = -lcmfwk -lpthread

CFLAGS := -Wall -O4
ifeq ($(SYSROOT),)
	CFLAGS += --sysroot=/opt/poky/2.2/sysroots/cortexa7-poky-linux-gnueabi/
else
	CFLAGS += --sysroot=$(SYSROOT)
endif
CFLAGS += -DPMGR_VERSION="\"${PMGR_VERSION}\""

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
	echo $(INSTALL_PATH)
	mkdir -p $(INSTALL_PATH)
	cp -f $(TARGET) $(INSTALL_PATH)

