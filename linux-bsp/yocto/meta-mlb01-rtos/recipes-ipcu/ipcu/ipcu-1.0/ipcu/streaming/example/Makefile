INCDIR = -I$(LINUX_INCLUDE) -I../ -I../../include
LIBDIR = -L../ -L../../ipcu_lib/libcmfwk/
LIBS = -lipcustream -lpthread -lcmfwk -lm
CFLAGS := -Wall -O4
CFLAGS += --sysroot=$(SYSROOT)
ALSALIB_DIR = ../../../../../alsa-lib/1.1.2-r0/image/usr
ALSALIB_INCDIR = -I$(ALSALIB_DIR)/include
ALSALIB_LIBDIR = -L$(ALSALIB_DIR)/lib
INCDIR += $(ALSALIB_INCDIR)

TARGETAPPS = zw camifts ipcuvc streamd set_streamd netseclpb ipcuvc15-ift ipcuac ipcuvc-m live555_ipcu ipcu_playstream_file
SRCS = camifts.c \
       zw.c \
       netsecrtp_api.c netsecrtp_rtcp.c \
       uvc-gadget.c uvc15-gadget-ift.c uac-gadget.c uvc-gadget_surv.c uvc-gadget_act.c uac-gadget_surv.c uac-gadget_act.c \
       streamd.c \
       set_streamd.c \
       netsecrtp_main_lpb.c \
       uvc-gadget-m.c \
       live555_ipcu.c \
       ipcu_playstream_file.c
OBJS = $(SRCS:.c=.o)

ifeq ($(SURVEILLANCE),1)
UVC_EXT_OBJS := uvc-gadget_surv.o
UAC_EXT_OBJS := uac-gadget_surv.o
else
UVC_EXT_OBJS := uvc-gadget_act.o
UAC_EXT_OBJS := uac-gadget_act.o
endif

all: module
module: $(TARGETAPPS)
camifts: $(OBJS)
	$(CC) $(CFLAGS) -o $@ camifts.o $(LIBDIR) $(LIBS)
zw: $(OBJS)
	$(CC) $(CFLAGS) -o $@ zw.o $(LIBDIR) $(LIBS)
ipcuvc: $(OBJS)
	$(CC) $(CFLAGS) -o $@ uvc-gadget.o $(UVC_EXT_OBJS) $(LIBDIR) $(LIBS)
ipcuvc15-ift: $(OBJS)
	$(CC) $(CFLAGS) -o $@ uvc15-gadget-ift.o $(LIBDIR) $(LIBS) $(LIBDIR)
ipcuvc-m: $(OBJS)
	$(CC) $(CFLAGS) -o $@ uvc-gadget-m.o $(LIBDIR) $(LIBS) $(LIBDIR)
ipcuac: $(OBJS)
	$(CC) $(CFLAGS) -o $@ uac-gadget.o $(UAC_EXT_OBJS) $(LIBDIR) $(LIBS) $(ALSALIB_LIBDIR) -lasound
streamd: $(OBJS)
	$(CC) $(CFLAGS) -o $@ streamd.o netsecrtp_rtcp.o netsecrtp_api.o $(LIBDIR) $(LIBS)
netseclpb: $(OBJS)
	$(CC) $(CFLAGS) -o $@ netsecrtp_main_lpb.o netsecrtp_api.o $(LIBDIR) $(LIBS)
live555_ipcu: $(OBJS)
	$(CC) $(CFLAGS) -o $@ live555_ipcu.o netsecrtp_api.o $(LIBDIR) $(LIBS)
ipcu_playstream_file: $(OBJS)
	$(CC) $(CFLAGS) -o $@ ipcu_playstream_file.o $(LIBDIR) $(LIBS)
set_streamd: $(OBJS)
	$(CC) $(CFLAGS) -o $@ set_streamd.o $(LIBDIR) $(LIBS)
$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)
clean:
	rm -f $(TARGETAPPS) $(OBJS)
install: module
	mkdir -p $(INSTALL_PATH)
	cp -f $(TARGETAPPS) $(INSTALL_PATH)
