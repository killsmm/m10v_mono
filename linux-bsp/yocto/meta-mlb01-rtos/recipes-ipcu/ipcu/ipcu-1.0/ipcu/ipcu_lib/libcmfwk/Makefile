ifeq ($(LINUX_INCLUDE),)
	LINUX_INCLUDE = ../../include/linux
endif

ifeq ($(INSTALL_PATH),)
	INSTALL_PATH = $(CURDIR)/../out/lib
endif

TARGET = libcmfwk.so
SRCS = \
	cmfwk_ipcu.c \
	cmfwk_mm.c 
	
OBJS = $(SRCS:.c=.o)

INCDIR = -I$(LINUX_INCLUDE) \
	-I../../include

ifeq ($(SURVEILLANCE),1)
	CFLAGS := -Wall -O4
else
	CFLAGS := -Wall -O4 -D_IPCU_QUEUING_
endif

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
