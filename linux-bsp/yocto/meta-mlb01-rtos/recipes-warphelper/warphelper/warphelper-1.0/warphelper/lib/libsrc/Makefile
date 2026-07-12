ifeq ($(CC),)
CROSS_COMPILE:=/opt/poky/2.2/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc
endif

ifeq ($(CFLAGS),)
CFLAGS:=-Wall -Werror -O4 \
	--sysroot=/opt/poky/2.2/sysroots/cortexa7-poky-linux-gnueabi/
endif

ifeq ($(INSTALL_PATH),)
INSTALL_PATH=$(CURDIR)/../../out/usr/lib
endif

TARGET=libwarph.so.1.0.0
SRCS=warp_helper_lib.c
OBJS=$(SRCS:.c=.o)
INCDIR=-I../../include

#-----------------------------------------
all: module

module: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -shared -Wl,-soname=libwarph.so.1 $^ -o $@

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -fPIC $(INCDIR) -c $(SRCS)

#-----------------------------------------
clean:
	rm -f $(TARGET) $(OBJS)
	rm -f *.so* *.so.*

install: module
	mkdir -p $(INSTALL_PATH)
	ln -s libwarph.so.1.0.0 libwarph.so.1
	ln -s libwarph.so.1 libwarph.so
	cp -f libwarph.so.1.0.0 $(INSTALL_PATH)
	cp -d libwarph.so.1 $(INSTALL_PATH)
	cp -d libwarph.so $(INSTALL_PATH)

