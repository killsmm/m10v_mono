# warp helper Makefile
CROSS_BASE:=/opt/poky/2.2/sysroots
CROSS_COMPILE:=$(CROSS_BASE)/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-

CC	= $(CROSS_COMPILE)gcc

CFLAGS  := -Wall -O4 \
	--sysroot=$(CROSS_BASE)/cortexa7-poky-linux-gnueabi/

export CC
export CFLAGS

BIN_PATH = $(CURDIR)/out/usr/bin
LIB_PATH = $(CURDIR)/out/usr/lib

.PHONY: all
all:
	make install -C lib/libconfigread INSTALL_PATH=$(LIB_PATH)
	make install -C lib/libsrc INSTALL_PATH=$(LIB_PATH)
	make install -C warpd INSTALL_PATH=$(BIN_PATH)
	make install -C warp_command INSTALL_PATH=$(BIN_PATH)
	make install -C save_snapshot INSTALL_PATH=$(BIN_PATH)
	make install -C warp_notify INSTALL_PATH=$(BIN_PATH)
	cp shell/* $(BIN_PATH)

.PHONY: clean
clean:
	make clean -C lib/libconfigread
	make clean -C lib/libsrc
	make clean -C warpd
	make clean -C warp_command
	make clean -C save_snapshot
	make clean -C warp_notify
	rm -rf $(CURDIR)/out

