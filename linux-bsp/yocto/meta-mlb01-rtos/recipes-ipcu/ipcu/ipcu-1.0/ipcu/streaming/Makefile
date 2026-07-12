TARGET = libipcustream.so
SRCS = \
	sample_library.c \
	
OBJS = $(SRCS:.c=.o)
LIBDIR = -L../ipcu_lib/libcmfwk/
LIBS = -lcmfwk

INCDIR = -I$(LINUX_INCLUDE) \
	-I../include

CFLAGS := -Wall -O4
CFLAGS += --sysroot=$(SYSROOT)

#-----------------------------------------
all: module

module: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -shared -lpthread $^ -o $@ $(LIBDIR) $(LIBS)

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -fPIC $(INCDIR) -c $(SRCS)

#-----------------------------------------
clean:
	rm -f $(TARGET) $(OBJS)

install: module
	mkdir -p $(INSTALL_PATH)
	cp -f *.so $(INSTALL_PATH)
