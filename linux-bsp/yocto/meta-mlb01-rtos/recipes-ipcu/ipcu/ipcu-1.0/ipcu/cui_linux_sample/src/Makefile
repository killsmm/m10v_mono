CC = /opt/poky/2.2/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc

ifeq ($(LINUX_INCLUDE),)
	LINUX_INCLUDE = ../../include/linux
endif

ifeq ($(INSTALL_PATH),)
	INSTALL_PATH = $(CURDIR)/../../out
endif

TARGET = preview
SRCS = preview.c
OBJS = $(SRCS:.c=.o)

TARGET2 = media_format
SRCS2 = media_format.c
OBJS2 = $(SRCS2:.c=.o)

TARGET4 = single_capture
SRCS4 = single_capture.c
OBJS4 = $(SRCS4:.c=.o)

TARGET5 = video_capture
SRCS5 = video_capture.c
OBJS5 = $(SRCS5:.c=.o)

INCDIR = -I$(LINUX_INCLUDE) \
	-I../include \
	-I../../include
LIBDIR = -L../../ipcu_lib/libcmfwk
LIBDIR += -L../lib
LIBS = -lcmfwk -lpthread
LIBS += -lsample -lpthread

CFLAGS := -Wall -O4
ifeq ($(SYSROOT),)
	CFLAGS += --sysroot=/opt/poky/2.2/sysroots/cortexa7-poky-linux-gnueabi/
else
	CFLAGS += --sysroot=$(SYSROOT)
endif

#-----------------------------------------
all: module

module: $(TARGET)  \
        $(TARGET2) \
		$(TARGET4) \
		$(TARGET5)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBDIR) $(LIBS)

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)

$(TARGET2): $(OBJS2)
	$(CC) $(CFLAGS) -o $@ $(OBJS2) $(LIBDIR) $(LIBS)

$(OBJS2): $(SRCS2)
	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS2)

$(TARGET4): $(OBJS4)
	$(CC) $(CFLAGS) -o $@ $(OBJS4) $(LIBDIR) $(LIBS)

$(OBJS4): $(SRCS4)
	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS4)

$(TARGET5): $(OBJS5)
	$(CC) $(CFLAGS) -o $@ $(OBJS5) $(LIBDIR) $(LIBS)

$(OBJS5): $(SRCS5)
	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS5)
	
#-----------------------------------------
clean:
	rm -f $(TARGET) $(OBJS)
	rm -f $(TARGET2) $(OBJS2)
	rm -f $(TARGET4) $(OBJS4)
	rm -f $(TARGET5) $(OBJS5)
	
install: module
	mkdir -p $(INSTALL_PATH)
	cp -f $(TARGET) $(INSTALL_PATH)
	cp -f $(TARGET2) $(INSTALL_PATH)
	cp -f $(TARGET4) $(INSTALL_PATH)
	cp -f $(TARGET5) $(INSTALL_PATH)
