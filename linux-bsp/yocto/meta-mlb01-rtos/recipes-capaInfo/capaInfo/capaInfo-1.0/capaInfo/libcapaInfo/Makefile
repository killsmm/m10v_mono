ifeq ($(INSTALL_LIB),)
	INSTALL_LIB = $(CURDIR)/../work/usr/lib/capaInfo
endif
ifeq ($(INSTALL_INC),)
	INSTALL_INC = $(CURDIR)/../work/usr/include/capaInfo
endif

TARGET = capaInfo
SONAME = lib$(TARGET).so

SRCS = \
	capability_shmem.cpp \
	capability_info.cpp \
	cap_command.cpp

OBJS := $(SRCS:.cpp=.o)

INCDIR = \
	-I../include

CFLAGS := -Wall -O4
#CFLAGS := -Wall -g3 -O0
ifeq ($(SYSROOT),)
	CFLAGS += --sysroot=/opt/poky/2.2/sysroots/cortexa7-poky-linux-gnueabi/
else
	CFLAGS += --sysroot=$(SYSROOT)
endif
CFLAGS += -I${STAGING_LIBDIR}/ipcu/include
CFLAGS += -I${STAGING_LIBDIR}/ipcu/streaming
LDFLAGS = -L$(STAGING_LIBDIR)/ipcu -Wl,-rpath-link,$(STAGING_LIBDIR)/ipcu
LDFLAGS += -lipcustream -lcmfwk

#-----------------------------------------
all: module

module: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) -shared -Wl,-soname,$(SONAME) -o $(SONAME) $^ $(LDFLAGS)


$(OBJS): $(SRCS)
	$(CXX) $(CFLAGS) $(INCDIR) -fPIC -c $(SRCS)

#-----------------------------------------
clean:
	rm -f $(TARGET) $(OBJS)

install: module
	mkdir -p $(INSTALL_LIB)
	cp -f *.so $(INSTALL_LIB)
	mkdir -p $(INSTALL_INC)
	cp -f ../include/*.h $(INSTALL_INC)

