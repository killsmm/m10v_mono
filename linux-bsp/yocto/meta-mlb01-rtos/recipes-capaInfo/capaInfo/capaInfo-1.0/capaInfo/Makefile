CROSS_BASE:=/opt/poky/2.2/sysroots
CROSS_COMPILE:=$(CROSS_BASE)/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-
CXX = $(CROSS_COMPILE)g++
SYSROOT = $(CROSS_BASE)/cortexa7-poky-linux-gnueabi/

INSTALL_TOP = $(CURDIR)/work
INSTALL_INC = $(INSTALL_TOP)$(CAPAINFO_INC)
INSTALL_LIB = $(INSTALL_TOP)$(CAPAINFO_LIB)

export CXX SYSROOT INSTALL_LIB INSTALL_INC

all:
	make install -C libcapaInfo INSTALL_LIB=$(INSTALL_LIB) INSTALL_INC=$(INSTALL_INC)

clean:
	make clean -C libcapaInfo
	rm -rf $(INSTALL_TOP)

