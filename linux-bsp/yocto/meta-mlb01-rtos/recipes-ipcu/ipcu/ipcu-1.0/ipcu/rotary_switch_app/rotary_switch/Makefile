CC = /opt/poky/2.2/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc

ifeq ($(INSTALL_PATH),)
	INSTALL_PATH = $(CURDIR)/../../out
endif

TARGET = rotary_switch
SRCS = ./src/rotary_switch.c
OBJS = $(SRCS:.c=.o)

INCDIR = -I./include
 	
CFLAGS := -Wall -O3
CFLAGS += --sysroot=/opt/poky/2.2/sysroots/cortexa7-poky-linux-gnueabi/

all: module

module: $(TARGET)
		
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)
	mv rotary_switch.o ./src 

clean:
	rm -f $(TARGET) $(OBJS)

install: module
	mkdir -p $(INSTALL_PATH)
	cp -f $(TARGET) $(INSTALL_PATH)
