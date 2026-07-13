.SUFFIX: .a .axf .map .bin .scat .via .cfg .o .objs .txt



#
# Project and ARM settings
#

### The Name of Project
PRJ_NAME        := MILB_SDK
export PRJ_NAME

### ARM core (used by option)
ARMCORE         := cortex-a7
export ARMCORE

TCHAIN  := $(GNUarm)

### CPU STATE
IS_ARM_STATE    ?=
export IS_ARM_STATE

### C source -> ASM
IS_GET_ASM      ?=
export IS_GET_ASM

### Feedback
IS_USE_FEEDBACK ?=
export IS_USE_FEEDBACK

##### REMOVE_PGRELIEF BEGIN #####
### PGRelief
IS_USE_PGRELIEF ?=
export IS_USE_PGRELIEF
##### REMOVE_PGRELIEF END #####

### Board
IS_BOARD ?=
export IS_BOARD

### FAST BOOT
IS_FAST_BOOT    ?=
export IS_FAST_BOOT

### FILE SYSTEM
IS_FS_USED    ?=
export IS_FS_USED

#
# Directory and File names
#

### Root Directory of the Project
ROOT_DIR        := ../..

### The Name of Directory
OBJ_DIR_NAME    := obj
BIN_DIR_NAME    := bin
LIB_DIR_NAME    := lib
WS_DIR_NAME     := project
export WS_DIR_NAME

### The List of Sub-Directory which have Makefile
SUBDIRS         :=

### MILB_API Library
MILB_API_NAME    ?= MILB_API
MILB_API_DIR     ?= $(ROOT_DIR)/cpu_tkernel/basefw/$(MILB_API_NAME)

### MILB_Header Library
MILB_HEADER_NAME ?= MILB_Header
MILB_HEADER_DIR  ?= $(MILB_API_DIR)/$(MILB_HEADER_NAME)

### The List of Library
BASEFW_LIBS     := $(ROOT_DIR)/cpu_tkernel/basefw/$(LIB_DIR_NAME)/BaseFW.a
OS_LIBS         := ../$(ROOT_DIR)/lib/libkernel.a
MILB_LIBS        :=
MILB_LIBS        += $(MILB_API_DIR)/$(LIB_DIR_NAME)/MILB_API.a
MILB_LIBS        += $(MILB_HEADER_DIR)/$(LIB_DIR_NAME)/MILB.a
FJ_LIBS         :=
### --- REMOVE_AAC BEGIN ---
FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/audio/aac/$(LIB_DIR_NAME)/libaacenc.a
FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/audio/aac/$(LIB_DIR_NAME)/libaacdec.a
### --- REMOVE_AAC END ---
FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/audio/wav/$(LIB_DIR_NAME)/WAV.a
FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/audio/wav/$(LIB_DIR_NAME)/WAV_CODEC.a

FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/fatsafe/$(LIB_DIR_NAME)/libfatsafe.a
FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/$(LIB_DIR_NAME)/libprfile2.a
FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/unicodeif/$(LIB_DIR_NAME)/libunicodeif.a
FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/exfat/$(LIB_DIR_NAME)/libexfat.a

FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/media_driver/nf/$(LIB_DIR_NAME)/NF.a
FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/media_driver/sdio/$(LIB_DIR_NAME)/SDIO_ch0.a
FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/media_driver/sdio/$(LIB_DIR_NAME)/SDIO_ch1.a
FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/media_driver/emmc/$(LIB_DIR_NAME)/eMMC.a
FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/imgproc/jpgepc/$(LIB_DIR_NAME)/libjpgepc.a

### --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
ifeq "$(IS_MEMORY_SIZE)" "sdram_1GB"
### --- REMOVE_MEMORY_SIZE_OPT END ---
	### --- REMOVE_MEMORY_1GB BEGIN ---
	FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/h264/rb/$(LIB_DIR_NAME)/RB_1gb.a
	FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/h265/SR/$(LIB_DIR_NAME)/SR_1gb.a
	### --- REMOVE_MEMORY_1GB END ---
### --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
endif
ifeq "$(IS_MEMORY_SIZE)" "sdram_2GB"
### --- REMOVE_MEMORY_SIZE_OPT END ---
	### --- REMOVE_MEMORY_2GB BEGIN ---
	FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/h264/rb/$(LIB_DIR_NAME)/RB.a
	FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/h265/SR/$(LIB_DIR_NAME)/SR.a
	### --- REMOVE_MEMORY_2GB END ---
### --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
endif
ifeq "$(IS_MEMORY_SIZE)" "sdram_3GB"
### --- REMOVE_MEMORY_SIZE_OPT END ---
	### --- REMOVE_MEMORY_3GB BEGIN ---
	FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/h264/rb/$(LIB_DIR_NAME)/RB.a
	FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/h265/SR/$(LIB_DIR_NAME)/SR.a
	### --- REMOVE_MEMORY_3GB END ---
### --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
endif
### --- REMOVE_MEMORY_SIZE_OPT END ---

### --- REMOVE_ES_COMPILE_OPT BEGIN ---
ifeq "$(IS_CHIP_ES)" "chip_es3"
### --- REMOVE_ES_COMPILE_OPT END ---
	### --- REMOVE_ES3_HARDWARE BEGIN ---
	FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/h265/WAVE/$(LIB_DIR_NAME)/WAVE.a
	### --- REMOVE_ES3_HARDWARE END ---
### --- REMOVE_ES_COMPILE_OPT BEGIN ---
endif
### --- REMOVE_ES_COMPILE_OPT END ---

### --- REMOVE_USB BEGIN ---
FJ_LIBS         += $(ROOT_DIR)/cpu_tkernel/basefw/fj/usb/storagelib/$(LIB_DIR_NAME)/USB_Storage.a
### --- REMOVE_USB END ---
IP_LIBS         :=
### --- REMOVE_AHIP BEGIN ---
### --- REMOVE_ES_COMPILE_OPT BEGIN ---
ifeq "$(IS_CHIP_ES)" "chip_es3"
### --- REMOVE_ES_COMPILE_OPT END ---
### --- REMOVE_ES3_HARDWARE BEGIN ---
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/ahip_es3/$(LIB_DIR_NAME)/ahip.a
### --- REMOVE_ES3_HARDWARE END ---
### --- REMOVE_ES_COMPILE_OPT BEGIN ---
else
### --- REMOVE_ES_COMPILE_OPT END ---
### --- REMOVE_ES1_HARDWARE BEGIN ---
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/ahip/$(LIB_DIR_NAME)/ahip.a
### --- REMOVE_ES1_HARDWARE END ---
### --- REMOVE_ES_COMPILE_OPT BEGIN ---
endif
### --- REMOVE_ES_COMPILE_OPT END ---
### --- REMOVE_AHIP END ---
### --- REMOVE_EIS BEGIN ---
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/EIS/$(LIB_DIR_NAME)/LibDeVIS.a
### --- REMOVE_EIS END ---
### --- REMOVE_FD BEGIN ---
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/FaceDetection/$(LIB_DIR_NAME)/LibAHFD.a
### --- REMOVE_FD END ---
### --- REMOVE_PD BEGIN ---
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/PersonDetection/$(LIB_DIR_NAME)/LibAHOF_PD.a
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/PersonDetection/$(LIB_DIR_NAME)/LibAHOFManager.a
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/PersonDetection/$(LIB_DIR_NAME)/LibOT.a
### --- REMOVE_PD END ---
### --- REMOVE_PANO BEGIN ---
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/Panorama/$(LIB_DIR_NAME)/LibPan.a
### --- REMOVE_PANO END ---
### --- REMOVE_SHDR2 BEGIN ---
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/StillHDR/$(LIB_DIR_NAME)/libHDR.a
### --- REMOVE_QR_MARKER BEGIN ---
ifeq "$(OPT_IVE_QR_LIB)" "1"
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/IVE_Marker/$(LIB_DIR_NAME)/libIVEMark.a
endif
### --- REMOVE_QR_MARKER END ---
### --- REMOVE_SHDR2 END ---
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IPC/IPC_GMD/$(LIB_DIR_NAME)/ipc_gmd.a
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IPC/IPC_SHDR/$(LIB_DIR_NAME)/ipc_shdr.a
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IPC/IPC_PFC/$(LIB_DIR_NAME)/ipc_pfc.a
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IPC/IPC_IIP3/$(LIB_DIR_NAME)/ipc_iip3.a
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IPC/IPC_BLEND/$(LIB_DIR_NAME)/ipc_blend.a
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IPC/IPC_HDRPRM/$(LIB_DIR_NAME)/ipc_hdrprm.a
##IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IPC/IPC_VHDR/$(LIB_DIR_NAME)/ipc_vhdr.a
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/fj_ip/3a/$(LIB_DIR_NAME)/Ae_CA7.a
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/fj_ip/3a/$(LIB_DIR_NAME)/AlAwblib.a
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/fj_ip/3a/$(LIB_DIR_NAME)/AlFixLib.a
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/fj_ip/3a/$(LIB_DIR_NAME)/HSC.a
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/ip/fj_ip/PDAF/$(LIB_DIR_NAME)/PDAF.a
IP_LIBS         += $(ROOT_DIR)/cpu_tkernel/hostfw/dsp/dsp_if/dsp_peri/$(LIB_DIR_NAME)/dsp_peri.a
### --- REMOVE_GPU BEGIN ---
IP_LIBS			+= $(ROOT_DIR)/cpu_tkernel/ip/fj_ip/gpu/$(LIB_DIR_NAME)/vg_lite.a
### --- REMOVE_GPU END ---
### --- REMOVE_AHA BEGIN ---
IP_LIBS			+= $(ROOT_DIR)/cpu_tkernel/ip/fj_ip/panorama_grid/$(LIB_DIR_NAME)/panorama_grid.a
### --- REMOVE_AHA END ---
HOST_LIBS       :=
FWORK_LIBS      :=
### --- REMOVE_MP4 BEGIN ---
ifeq "$(OPT_MOVIE_LIB)" "0"
FWORK_LIBS      += $(ROOT_DIR)/cpu_tkernel/media_meta/mp4/library/$(LIB_DIR_NAME)/MP4FLib.a
else
FWORK_LIBS      += $(ROOT_DIR)/cpu_tkernel/media_meta/mp4/library/$(LIB_DIR_NAME)/MOVFLib.a
endif
### --- REMOVE_MP4 END ---

OS_USER_CUSTOM_OBJ += $(ROOT_DIR)/if_tkernel/tk_usercustom/src/interrupt_set.o
OS_USER_CUSTOM_OBJ += $(ROOT_DIR)/if_tkernel/tk_usercustom/src/os_resource_id.o
OS_USER_CUSTOM_OBJ += $(ROOT_DIR)/if_tkernel/tk_usercustom/src/os_user_custom.o
OS_USER_CUSTOM_OBJ += $(ROOT_DIR)/if_tkernel/tk_usercustom/src/TKernel_id.o
OS_USER_CUSTOM_OBJ += $(ROOT_DIR)/if_tkernel/tk_usercustom/src/usermain.o
### OS_USER_CUSTOM_OBJ += $(ROOT_DIR)/if_tkernel/tk_usercustom/src/dummy.o

USR_OBJ_TK := $(ROOT_DIR)/if_tkernel/boot.o
USR_OBJ_TK += $(ROOT_DIR)/if_tkernel/excep_ints.o
USR_OBJ_TK += $(ROOT_DIR)/if_tkernel/icrt0.o
USR_OBJ_TK += $(ROOT_DIR)/if_tkernel/led.o
USR_OBJ_TK += $(ROOT_DIR)/if_tkernel/porting.o
USR_OBJ_TK += $(ROOT_DIR)/if_tkernel/putc.o
USR_OBJ_TK += $(ROOT_DIR)/if_tkernel/tkdev_timer.o
USR_OBJ_TK += $(ROOT_DIR)/if_tkernel/udelay.o
USR_OBJ_TK += $(ROOT_DIR)/if_tkernel/vector.o
USR_OBJ_TK += $(ROOT_DIR)/if_tkernel/task_control/task_control.o
USR_OBJ_TK += $(ROOT_DIR)/../config/kernel_cfg.o
USR_OBJ_TK += $(ROOT_DIR)/../lib/newlib_getreent.o
USR_OBJ_TK += $(ROOT_DIR)/../lib/newlib_syscalls.o
USR_OBJ_TK += $(ROOT_DIR)/../lib/newlib_syscalls_r.o

### Libraries
LIBS             = $(BASEFW_LIBS) $(OS_LIBS) $(MILB_LIBS) $(FJ_LIBS) $(IP_LIBS) $(HOST_LIBS) $(FWORK_LIBS)
LIBS            += ../../../lib/libkernel.a

### Objects
OBJS             = $(patsubst %.o, $(ROOT_DIR)/cpu_tkernel/%.o, $(shell cat $(ROOT_DIR)/cpu_tkernel/$(OBJ_DIR_NAME)/*.objs 2>/dev/null))
OBJS            += $(OS_USER_CUSTOM_OBJ) $(USR_OBJ_TK)

### File (related to armlink)
### note : If you want to make library file, Variable "ELF_FILE" must be empty.
ifeq "$(IS_MAKE_LIB)" ""
ELF_FILE        := $(BIN_DIR_NAME)/$(PRJ_NAME).axf
LIST_FILE       := $(BIN_DIR_NAME)/$(PRJ_NAME).map
LD_SCRIPT_NAME_FAST := $(ROOT_DIR)/cpu_tkernel/board/sdram/$(PRJ_NAME)_FAST.ld
LD_SCRIPT_NAME      := $(ROOT_DIR)/cpu_tkernel/board/sdram/$(PRJ_NAME).ld
ifeq "$(IS_FAST_BOOT)" "CO_FAST_BOOT_ON"
SCAT_FILE       := $(LD_SCRIPT_NAME_FAST)
else
SCAT_FILE       := $(LD_SCRIPT_NAME)
endif

LD_VIA_FILE     := gcclink.via
FEEDBACK_FILE   := armlink_feedback.txt
export FEEDBACK_FILE
FEEDBACK_BATCH  := feedback_full_build.bat
endif

### File (related to armar)
ifeq "$(IS_MAKE_LIB)" "y"
LIB_FILE        := $(LIB_DIR_NAME)/$(PRJ_NAME).a
LIB_INFO_FILE   := $(LIB_DIR_NAME)/$(PRJ_NAME).txt
endif

### File (related to fromelf)
ifeq "$(IS_MAKE_LIB)" ""
CVT_FILE            := $(BIN_DIR_NAME)/$(PRJ_NAME).bin
CVT_FILE_ZERO       := $(BIN_DIR_NAME)/$(PRJ_NAME)_ZERO.bin
CVT_FILE_FIRST      := $(BIN_DIR_NAME)/$(PRJ_NAME)_FIRST.bin
CVT_FILE_SECOND     := $(BIN_DIR_NAME)/$(PRJ_NAME)_SECOND.bin
CVT_FILE_THIRD      := $(BIN_DIR_NAME)/$(PRJ_NAME)_THIRD.bin
CVT_FILE_ZERO_SEC   := --only-section=.zero_sec
CVT_FILE_FIRST_SEC  := --only-section=.first_sec
CVT_FILE_SECOND_SEC := --only-section=.second_sec
CVT_FILE_THIRD_SEC  := --only-section=.third_sec
endif



#
# Toolchain
#

### Linker
-include $gcclink.via
LD       := $(TCHAIN)gcc
### LDFLAGS  := -T./$(SCAT_FILE) -Wl,-Map=$(LIST_FILE),--cref,--gc-sections -symbolic
### LDFLAGS  += $(GCCLINK_VIA)
### ifeq "$(IS_USE_FEEDBACK)" "y"
### LDFLAGS  += --feedback=$(FEEDBACK_FILE)
### endif
LDFLAGS  := -march=armv7-a
LDFLAGS  += -mfpu=neon-vfpv4
LDFLAGS  += -mfloat-abi=hard
LDFLAGS  += -DENABLE_NEON
LDFLAGS  += -mthumb-interwork
LDFLAGS  += -nostdlib
LDFLAGS  += -static
LDFLAGS  += -T $(SCAT_FILE)
LDFLAGS  += -Wl,-Map,$(LIST_FILE)
ifeq "$(IS_MAKE_LIB)" ""
LDFLAGS  += $(GCCLINK_VIA)
endif
LDFLAGS  += -Wl,--wrap=vprintf
LDFLAGS  += -Wl,--wrap=printf

### Librarian
AR       := $(TCHAIN)ar
ARFLAGS  := -r

### Conveting Tool
CV       := $(TCHAIN)objcopy
CVFLAGS  := -Obinary
BCB      := ./BinCombine.exe



#
# Targets
#

ifeq "$(ELF_FILE)" ""
DEFAULT_TARGET := lib
else
DEFAULT_TARGET := target
endif

QUIET := @

.PHONY: default all recursive-target target-self lib lib-self convert distclean-self depends-self $(SUBDIRS) \
        target clean clean-self clean-obj clean-obj-self clean-elf clean-feedback-info distclean \
        config depends message message-self feedback-all

default: $(DEFAULT_TARGET)

all: clean config depends $(DEFAULT_TARGET) convert

recursive-target: $(PRE_TARGET) $(SUBDIRS) $(POST_TARGET)

target-self: $(CVT_FILE) lib-self

lib: config depends target lib-self

lib-self: $(LIB_FILE)

convert: $(CVT_FILE)

distclean-self: clean-self

config: 

depends-self: 

### Sub Folders
ifneq "$(SUBDIRS)" ""
$(SUBDIRS): 
	$(QUIET)$(MAKE) recursive-target -C $@
endif

### Build
target: config
	$(QUIET)$(MAKE) recursive-target POST_TARGET=target-self

$(LIB_FILE): $(OBJS)
	$(QUIET)if [ ! -d $(LIB_DIR_NAME) ] ; then mkdir $(LIB_DIR_NAME); fi
	$(QUIET)echo Creating $@ ...
	$(QUIET)$(AR) $(ARFLAGS) $@ $(OBJS)
	$(QUIET)$(AR) -t $@ > $(LIB_INFO_FILE)
	$(QUIET)echo done.

$(ELF_FILE): $(OBJS) $(LIBS) $(SCAT_FILE) $(LD_VIA_FILE)
	$(QUIET)if [ ! -d $(BIN_DIR_NAME) ] ; then mkdir $(BIN_DIR_NAME); fi
	$(QUIET)echo Creating $@ ...
	$(QUIET)$(LD) $(OBJS) $(LDFLAGS) $(LIBS) -o $@ -lm -lc -lgcc
	$(QUIET)echo done.

$(CVT_FILE): $(ELF_FILE)
	$(QUIET)echo Converting to $@ from $< ...
	$(QUIET)if [ ! -d $(BIN_DIR_NAME) ] ; then mkdir $(BIN_DIR_NAME); fi
ifeq "$(IS_FAST_BOOT)" "CO_FAST_BOOT_ON"
	$(QUIET)$(CV) $(CVFLAGS) $(CVT_FILE_ZERO_SEC) $< $(CVT_FILE_ZERO)
	$(QUIET)$(CV) $(CVFLAGS) $(CVT_FILE_FIRST_SEC) $< $(CVT_FILE_FIRST)
###	$(QUIET)$(CV) $(CVFLAGS) $(CVT_FILE_SECOND_SEC) $< $(CVT_FILE_SECOND)
###	$(QUIET)$(CV) $(CVFLAGS) $(CVT_FILE_THIRD_SEC) $< $(CVT_FILE_THIRD)
	$(QUIET)$(BCB)
else
	$(QUIET)$(CV) $(CVFLAGS) $< $@
endif
	$(QUIET)echo done.

### Clean
clean: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=clean-self

clean-self: clean-obj-self
	$(QUIET)if [ -d $(BIN_DIR_NAME) ] ; then rm -rf $(BIN_DIR_NAME); fi
	$(QUIET)if [ -d $(LIB_DIR_NAME) ] ; then rm -rf $(LIB_DIR_NAME); fi

clean-obj: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=clean-obj-self

clean-obj-self: 
	$(QUIET)if [ -d $(ROOT_DIR)/cpu_tkernel/$(OBJ_DIR_NAME) ] ; then rm -rf $(ROOT_DIR)/cpu_tkernel/$(OBJ_DIR_NAME); fi
	$(QUIET)if [ -d $(OBJ_DIR_NAME) ] ; then rm -rf $(OBJ_DIR_NAME); fi

clean-elf: 
	$(QUIET)if [ -f $(ELF_FILE) ] ; then rm -f $(ELF_FILE); fi

clean-feedback-info: 
	$(QUIET)if [ -f $(FEEDBACK_FILE) ] ; then rm -f $(FEEDBACK_FILE); fi

distclean: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=distclean-self

### Depends
depends: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=depends-self

### Message
message: 
	$(QUIET)$(MAKE) recursive-target PRE_TARGET=message-self

message-self: 
ifneq "$(MESSAGE)" ""
	$(QUIET)echo $(MESSAGE)
endif

### Feedback
feedback-all: 
	$(QUIET)$(FEEDBACK_BATCH)

