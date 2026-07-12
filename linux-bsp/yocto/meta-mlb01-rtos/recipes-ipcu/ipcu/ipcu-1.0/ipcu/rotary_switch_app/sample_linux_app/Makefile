CC = /opt/poky/2.2/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc

ifeq ($(INSTALL_PATH),)
	INSTALL_PATH = $(CURDIR)/../../out
endif

TARGET = sample_linux_app
SRCS = ./src/sample_linux_app.c
OBJS = $(SRCS:.c=.o)

INCDIR = -I./include

LIBS = -lpthread
 	
CFLAGS := -Wall -O3
CFLAGS += --sysroot=/opt/poky/2.2/sysroots/cortexa7-poky-linux-gnueabi/

all: module

module: $(TARGET)
		
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)
	mv sample_linux_app.o ./src 

clean:
	rm -f $(TARGET) $(OBJS)

install: module
	mkdir -p $(INSTALL_PATH)
	cp -f $(TARGET) $(INSTALL_PATH)
