#ifeq ($(CC),)
	CC = /opt/poky/2.2/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc
#endif

ifeq ($(LINUX_INCLUDE),)
	LINUX_INCLUDE = ../include/linux
endif

ifeq ($(INSTALL_PATH),)
	INSTALL_PATH = $(CURDIR)/../out
endif

TARGET = osdcmd
TARGET2 = osdtimeonoff
TARGET3 = osdtimechg
TARGETAPPS = $(TARGET) $(TARGET2) $(TARGET3)
OBJS = osd_main.o osd_draw_font.o osd_fontdata.o
OBJS2= osd_time_main.o
OBJS3= osd_param_change.o

INCDIR = -I$(LINUX_INCLUDE) -I../include
LIBDIR = -L../ipcu_lib/libcmfwk/
LIBS = -lpthread -lcmfwk

CFLAGS := -Wall -O3
ifeq ($(SYSROOT),)
	CFLAGS += --sysroot=/opt/poky/2.2/sysroots/cortexa7-poky-linux-gnueabi/
else
	CFLAGS += --sysroot=$(SYSROOT)
endif

CFLAGS += -mcpu=cortex-a7 -mtune=cortex-a7 -mthumb

#-----------------------------------------
.SUFFIXES: .c .o
all: module
module: $(TARGETAPPS)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)

$(TARGET2): $(OBJS2)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)

$(TARGET3): $(OBJS3)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) -c $<


#-----------------------------------------
.PHONY: clean
clean:
	$(RM) $(TARGETAPPS) 
	$(RM) $(TARGET2) $(OBJS) $(OBJS2) $(OBJS3)
install: module
	mkdir -p $(INSTALL_PATH)
	cp -f $(TARGET) $(INSTALL_PATH)
	cp -f $(TARGET2) $(INSTALL_PATH)
	cp -f $(TARGET3) $(INSTALL_PATH)


