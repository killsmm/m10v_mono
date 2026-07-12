CROSS_BASE:=/opt/poky/2.2/sysroots
CROSS_COMPILE:=$(CROSS_BASE)/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-
CC	= $(CROSS_COMPILE)gcc

SYSROOT = $(CROSS_BASE)/cortexa7-poky-linux-gnueabi/

OBJ_PATH = $(CURDIR)/out/usr/bin
LIB_PATH = $(CURDIR)/out/usr/lib
CONF_PATH = $(CURDIR)/out/etc/pmgr
SYSINIT_PATH = $(CURDIR)/out/etc/init.d
LINUX_INCLUDE = ./include/linux/

export CC LINUX_INCLUDE SYSROOT

all:
	make install -C pmgr INSTALL_PATH=$(OBJ_PATH)
	cp shell/*.sh $(OBJ_PATH)
	mkdir -p $(CONF_PATH)
	cp conf/*.conf $(CONF_PATH)
	mkdir -p $(SYSINIT_PATH)
	cp sysinit/* $(SYSINIT_PATH)
clean:
	make clean -C pmgr
	rm -rf $(CURDIR)/out

