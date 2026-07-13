.SUFFIX: .c .cpp .s .o .d .h .inc

ifeq "$(OS)" "Windows_NT"
PATH_CONVERT = $(shell cygpath -m $(1))
else
PATH_CONVERT = $(1)
endif

#
# Project and ARM settings
#

PRIORITY_OPTION  := fast
#PRIORITY_OPTION  := size
#PRIORITY_OPTION  := debug

### ARM core (used by option)
ARMCORE     ?= cortex-a7

TCHAIN  := $(GNUarm)



#
# MILB Libraries
#

### MILB_API Library
MILB_API_NAME    ?= MILB_API
MILB_API_DIR     ?= $(ROOT_DIR)/cpu_tkernel/basefw/$(MILB_API_NAME)

### MILB_Header Library
MILB_HEADER_NAME ?= MILB_Header
MILB_HEADER_DIR  ?= $(MILB_API_DIR)/$(MILB_HEADER_NAME)



#
# Include paths
#

### Used by the option of armcc
CMD_CINCPATH    := -I$(ROOT_DIR)/cpu_tkernel/cmd_entry/include
OS_CINCPATH     := -I$(ROOT_DIR)/cpu_tkernel/os/include
OS_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/project/obj
MILB_CINCPATH    := -I$(MILB_API_DIR)/include
MILB_CINCPATH    += -I$(MILB_HEADER_DIR)/include
MILB_CINCPATH    += -I$(MILB_HEADER_DIR)/include/Arm
MILB_CINCPATH    += -I$(MILB_HEADER_DIR)/include/Exs
MILB_CINCPATH    += -I$(MILB_HEADER_DIR)/include/Image
MILB_CINCPATH    += -I$(MILB_HEADER_DIR)/include/Image/PRO
MILB_CINCPATH    += -I$(MILB_HEADER_DIR)/include/Peri
MILB_CINCPATH    += -I$(MILB_HEADER_DIR)/include/Pmc
MILB_CINCPATH    += -I$(MILB_HEADER_DIR)/include/Top
FJ_CINCPATH     := -I$(ROOT_DIR)/cpu_tkernel/basefw/include
# delete_basefw\custom
#FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/custom/view_snr/include
# delete_basefw\fj\3a
#FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/3a/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/audio/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/audio/wav/include
### --- REMOVE_AAC BEGIN ---
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/audio/aac/include
### --- REMOVE_AAC END ---
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/device_driver/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/disp/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/fs/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/exfat/config
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/exfat/config/prfile2
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/exfat/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/exfat/include/prfile2
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/exfat/prfile2/dskmng
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/exfat/prfile2/fatts
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/exfat/prfile2/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/exfat/prfile2/include/dskmng
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/fatsafe/config
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/fatsafe/config/prfile2
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/fatsafe/config/prfile2/plugin/safe/fatsafe
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/fatsafe/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/fatsafe/include/prfile2
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/fatsafe/include/prfile2/plugin/safe/fatsafe
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/fatsafe/prfile2
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/fatsafe/prfile2/plugin/safe/fatsafe
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/bios_ep/board/generic/prfile2/skel_drv
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/config
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/config/bios_ep/generic/prfile2/skel_drv
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/config/prfile2
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/include/bios_ep/generic/prfile2/skel_drv
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/include/bios_ep/kernel/prkernelv4/prfile2
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/include/bios_ep/kernel/tk/prfile2
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/include/prfile2
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/prfile2/api/module
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/prfile2/api/standard
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/prfile2/common
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/prfile2/dskmng
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/prfile2/ebinder
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/prfile2/fatfs
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/prfile2/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/prfile2/include/dskmng
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/prfile2/locale
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/prfile2/pfs
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/prfile2/stub/extret
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/prfile2/stub/module
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/prfile2/stub/standard
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/prfile2/prfile2/system
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/unicodeif/config
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/unicodeif/config/prfile2
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/unicodeif/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/unicodeif/include/prfile2
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/unicodeif/prfile2/api/standard/unicode
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/unicodeif/prfile2/common
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/unicodeif/prfile2/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/unicodeif/prfile2/pfs
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/prfile2/unicodeif/prfile2/stub/standard/unicode
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/vfs/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/ipcufs/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/media_driver/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/media_driver/nf/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/media_driver/sdio/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/media_driver/sdio/sdio_ch0/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/media_driver/sdio/sdio_ch1/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/file_mng/media_driver/emmc/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/iip_lock/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/iip_lock/iip_resmng/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/imgproc/jpgepc/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/imgproc/still/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/iq/include
# delete_basefw\fj\iq
#FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/iq/cag/include
#FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/iq/fshdl0/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/common/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/h264/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/h264/rb/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/h265/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/h265/SR/include
### --- REMOVE_ES_COMPILE_OPT BEGIN ---
ifeq "$(IS_CHIP_ES)" "chip_es3"
### --- REMOVE_ES_COMPILE_OPT END ---
### --- REMOVE_ES3_HARDWARE BEGIN ---
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/h265/WAVE/include
### --- REMOVE_ES3_HARDWARE END ---
### --- REMOVE_ES_COMPILE_OPT BEGIN ---
endif
### --- REMOVE_ES_COMPILE_OPT END ---
### --- REMOVE_XCODE BEGIN ---
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/xcode/include
### --- REMOVE_XCODE END ---
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/play/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/system/include
### --- REMOVE_USB BEGIN ---
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/usb/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/usb/func/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/usb/mtp/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/usb/mwusb4/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/usb/pictbridge/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/usb/ptp/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/usb/sic/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/usb/storagelib/include
### --- REMOVE_USB END ---
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/utility/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/utility/mba/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/utility/os_tools/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/movie/netsec/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/board/sdram/include
### --- REMOVE_ES_COMPILE_OPT END ---
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/board/storage/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/board/peripheral/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/board/lcd/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/board/audio_codec/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/board/camera/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/board/camera/MotionDriver_ICM20948/include
FJ_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/board/bridge/include

IP_CINCPATH     :=
### --- REMOVE_AHIP BEGIN ---
### --- REMOVE_ES_COMPILE_OPT BEGIN ---
ifeq "$(IS_CHIP_ES)" "chip_es3"
### --- REMOVE_ES_COMPILE_OPT END ---
### --- REMOVE_ES3_HARDWARE BEGIN ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/ahip_es3/include
### --- REMOVE_ES3_HARDWARE END ---
### --- REMOVE_ES_COMPILE_OPT BEGIN ---
else
### --- REMOVE_ES_COMPILE_OPT END ---
### --- REMOVE_ES1_HARDWARE BEGIN ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/ahip/include
### --- REMOVE_ES1_HARDWARE END ---
### --- REMOVE_ES_COMPILE_OPT BEGIN ---
endif
### --- REMOVE_ES_COMPILE_OPT END ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/DocCommon/include
### --- REMOVE_AHIP END ---
### --- REMOVE_EIS BEGIN ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/EIS/include
### --- REMOVE_EIS END ---
### --- REMOVE_FD BEGIN ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/FaceDetection/include
### --- REMOVE_FD END ---
### --- REMOVE_PD BEGIN ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/PersonDetection/include
### --- REMOVE_PD END ---
### --- REMOVE_PANO BEGIN ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/Panorama/include
### --- REMOVE_PANO END ---
### --- REMOVE_SHDR2 BEGIN ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/StillHDR/include
### --- REMOVE_SHDR2 END ---
### --- REMOVE_QR_MARKER BEGIN ---
ifeq "$(OPT_IVE_QR_LIB)" "1"
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/IVE_Marker/include
endif
### --- REMOVE_QR_MARKER END ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/3rd_ip/Hre/include
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/fj_ip/3a/include
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/fj_ip/3a/include/EXXX
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/fj_ip/PDAF/include
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/fj_ip/ImageTuning/include

### --- REMOVE_MFNR BEGIN ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/ip/mfnr/include
### --- REMOVE_MFNR END ---
### --- REMOVE_SHDR BEGIN ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/ip/shdr/include
### --- REMOVE_SHDR END ---
### --- REMOVE_WDR BEGIN ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/basefw/ip/wdr/include
### --- REMOVE_WDR END ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IPC/IPC_IF3/include
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IPC/IPC_IIP3/include
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IPC/IPC_GMD/include
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IPC/IPC_SHDR/include
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IPC/IPC_PFC/include
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IPC/IPC_BLEND/include
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IPC/IPC_HDRPRM/include
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IPC/IPC_VHDR/include
### --- REMOVE_GPU BEGIN ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/fj_ip/gpu/include
### --- REMOVE_GPU END ---
### --- REMOVE_AHA BEGIN ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/fj_ip/panorama_grid/include
### --- REMOVE_AHA END ---
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/user_ip/drv/include
IP_CINCPATH     += -I$(ROOT_DIR)/cpu_tkernel/ip/user_ip/lib/include
HOST_CINCPATH   := -I$(ROOT_DIR)/cpu_tkernel/hostfw/user/include
HOST_CINCPATH   += -I$(ROOT_DIR)/cpu_tkernel/hostfw/host/include
HOST_CINCPATH   += -I$(ROOT_DIR)/cpu_tkernel/hostfw/dsp/dsp_if/include
HOST_CINCPATH   += -I$(ROOT_DIR)/cpu_tkernel/hostfw/dsp/load/include
### --- REMOVE_CEVA_CV_SF BEGIN
HOST_CINCPATH   += -I$(ROOT_DIR)/cpu_tkernel/hostfw/dsp/ceva_cv_smartframe_host/include
### --- REMOVE_CEVA_CV_SF END
### --- REMOVE_AHIP BEGIN ---
HOST_CINCPATH   += -I$(ROOT_DIR)/cpu_tkernel/hostfw/ip/ahip/include
HOST_CINCPATH   += -I$(ROOT_DIR)/cpu_tkernel/hostfw/ip/DocCommon/include
### --- REMOVE_AHIP END ---
### --- REMOVE_EIS BEGIN ---
HOST_CINCPATH   += -I$(ROOT_DIR)/cpu_tkernel/hostfw/ip/eis/include
### --- REMOVE_EIS END ---
### --- REMOVE_FD BEGIN ---
HOST_CINCPATH   += -I$(ROOT_DIR)/cpu_tkernel/hostfw/ip/fd/include
### --- REMOVE_FD END ---
### --- REMOVE_PD BEGIN ---
HOST_CINCPATH   += -I$(ROOT_DIR)/cpu_tkernel/hostfw/ip/pd/include
### --- REMOVE_PD END ---
### --- REMOVE_PANO BEGIN ---
HOST_CINCPATH   += -I$(ROOT_DIR)/cpu_tkernel/hostfw/ip/panorama/include
### --- REMOVE_PANO END ---
### --- REMOVE_SHDR2 BEGIN ---
HOST_CINCPATH   += -I$(ROOT_DIR)/cpu_tkernel/hostfw/ip/shdr/include
### --- REMOVE_SHDR2 END ---
### --- REMOVE_AHIP BEGIN ---
HOST_CINCPATH   += -I$(ROOT_DIR)/cpu_tkernel/hostfw/ip/warp/include
### --- REMOVE_AHIP END ---
### --- REMOVE_SGDE BEGIN ---
HOST_CINCPATH   += -I$(ROOT_DIR)/cpu_tkernel/hostfw/ip/sgde/include
### --- REMOVE_SGDE END ---
### --- REMOVE_GPU BEGIN ---
HOST_CINCPATH   += -I$(ROOT_DIR)/cpu_tkernel/hostfw/ip/gpu/include
### --- REMOVE_GPU END ---
HOST_CINCPATH   += -I$(ROOT_DIR)/cpu_tkernel/hostfw/ip/stc/include
FWORK_CINCPATH  := -I$(ROOT_DIR)/cpu_tkernel/media_meta/include
### --- REMOVE_DCF_EXIF BEGIN ---
FWORK_CINCPATH  += -I$(ROOT_DIR)/cpu_tkernel/media_meta/dcf/include
FWORK_CINCPATH  += -I$(ROOT_DIR)/cpu_tkernel/media_meta/exif/include
### --- REMOVE_DCF_EXIF END ---
FWORK_CINCPATH  += -I$(ROOT_DIR)/cpu_tkernel/media_meta/mp4/demux/include
FWORK_CINCPATH  += -I$(ROOT_DIR)/cpu_tkernel/media_meta/mp4/mux/include
FWORK_CINCPATH  += -I$(ROOT_DIR)/cpu_tkernel/media_meta/mp4/include
### --- REMOVE_MP4 BEGIN ---
FWORK_CINCPATH  += -I$(ROOT_DIR)/cpu_tkernel/media_meta/mp4/library/include
### --- REMOVE_MP4 END ---

SYSTEM_CINCPATH  := -I$(ROOT_DIR)/cpu_tkernel/system/include
SYSTEM_CINCPATH  += -I$(ROOT_DIR)/cpu_tkernel/ip/fj_ip/IQBin/include
SHARE_CINCPATH   := -I$(ROOT_DIR)/cpu_tkernel/share/include

CINCPATH        := $(SUB_CINCPATH)
CINCPATH        += $(CMD_CINCPATH) $(OS_CINCPATH) $(MILB_CINCPATH) $(FJ_CINCPATH) $(IP_CINCPATH) $(HOST_CINCPATH) $(FWORK_CINCPATH)
CINCPATH        += $(SYSTEM_CINCPATH) $(SHARE_CINCPATH)
CINCPATH        += -I$(ROOT_DIR)/if_tkernel/tk_usercustom/include
CPPINCPATH      :=

### Used by the option of armasm
AINCPATH        := -I$(MILB_HEADER_DIR)/include
AINCPATH        += -I$(MILB_HEADER_DIR)/include/Arm
AINCPATH        += -I$(MILB_HEADER_DIR)/include/Peri
AINCPATH        += -I$(MILB_HEADER_DIR)/include/Top
AINCPATH        += -I$(ROOT_DIR)/cpu_tkernel/basefw/fj/system/include
AINCPATH        += -I$(ROOT_DIR)/if_tkernel/tk_usercustom/include

### --- REMOVE_ES_COMPILE_OPT BEGIN ---
ifeq "$(IS_CHIP_ES)" "chip_es3"
### --- REMOVE_ES_COMPILE_OPT END ---
	### --- REMOVE_ES3_HARDWARE BEGIN ---
	AINCPATH        += -I$(ROOT_DIR)/cpu_tkernel/board/sdram/mmu/include
	AINCPATH        += -I$(ROOT_DIR)/cpu_tkernel/board/sdram/map_es3_3gb/include
	### --- REMOVE_ES3_HARDWARE END ---
### --- REMOVE_ES_COMPILE_OPT BEGIN ---
else
### --- REMOVE_ES_COMPILE_OPT END ---
	### --- REMOVE_ES1_HARDWARE BEGIN ---
	AINCPATH        += -I$(ROOT_DIR)/cpu_tkernel/board/sdram/mmu/include
	AINCPATH        += -I$(ROOT_DIR)/cpu_tkernel/board/sdram/map/include
	### --- REMOVE_ES1_HARDWARE END ---
### --- REMOVE_ES_COMPILE_OPT BEGIN ---
endif
### --- REMOVE_ES_COMPILE_OPT END ---

#
# Toolchain options
#

### Compile Option File
include $(ROOT_DIR)/cpu_tkernel/project/MILB_COMMON.opt
include $(ROOT_DIR)/cpu_tkernel/project/MILB_SDK.opt
include $(ROOT_DIR)/cpu_tkernel/project/MILB_SDK_AUDIO.opt
include $(ROOT_DIR)/cpu_tkernel/project/MILB_SDK_IVE_QR_MARKER.opt

### Compiler
CC  := $(TCHAIN)gcc
CFLAGS  := -c
CFLAGS  += -pipe
CFLAGS  += -g3
CFLAGS  += -gdwarf-2
CFLAGS  += -std=gnu99
CFLAGS  += -fno-builtin
CFLAGS  += -ffreestanding
CFLAGS  += -mtune=cortex-a7
ifeq "$(PRIORITY_OPTION)" "debug"
CFLAGS          += -marm
CFLAGS          += -O0
CFLAGS          += -fno-inline
else ifeq "$(PRIORITY_OPTION)" "size"
CFLAGS          += -mthumb
CFLAGS          += -Os
else
CFLAGS          += -marm
CFLAGS          += -Ofast
endif
CFLAGS  += -march=armv7-a
ifneq "$(ENABLE_NEON)" ""
CFLAGS  += -mfloat-abi=hard
CFLAGS  += -mfpu=neon-vfpv4
else
CFLAGS  += -mfloat-abi=soft
endif
CFLAGS  += -mthumb-interwork
CFLAGS  += -mstructure-size-boundary=8
CFLAGS  += -ffreestanding
CFLAGS  += -fsigned-char
CFLAGS  += -Wall
CFLAGS  += $(MILB_COMMON_OPT)
CFLAGS  += $(MILB_OPT)
CFLAGS  += $(AUDIO_OPT)
CFLAGS  += $(LCD_OPT)
CFLAGS  += $(USER_OPT)
CFLAGS  += $(CMOS_OPT)
CFLAGS  += $(FASTBOOT_OPT)
CFLAGS  += $(BOOTVERSION_OPT)
CFLAGS  += $(BOARD_OPT)
CFLAGS  += $(3A_OPT)
CFLAGS  += $(FS_OPT)
CFLAGS  += $(UI_OPT)
CFLAGS  += $(USB_OPT)
CFLAGS  += $(IVE_QR_MARKER_OPT)
# for format check of BF_MBA_IF_Printf().
CFLAGS  += -Wno-format-zero-length

CFLAGS_DEPENDS  := -c
CFLAGS_DEPENDS  += -pipe
CFLAGS_DEPENDS  += -mcpu=$(ARMCORE)
CFLAGS_DEPENDS  += -fsyntax-only
CFLAGS_DEPENDS  += $(MILB_COMMON_OPT)
CFLAGS_DEPENDS  += $(MILB_OPT)
CFLAGS_DEPENDS  += $(AUDIO_OPT)
CFLAGS_DEPENDS  += $(LCD_OPT)
CFLAGS_DEPENDS  += $(USER_OPT)
CFLAGS_DEPENDS  += $(CMOS_OPT)
CFLAGS_DEPENDS  += $(FASTBOOT_OPT)
CFLAGS_DEPENDS  += $(BOOTVERSION_OPT)
CFLAGS_DEPENDS  += $(BOARD_OPT)
CFLAGS_DEPENDS  += $(3A_OPT)
CFLAGS_DEPENDS  += $(FS_OPT)
CFLAGS_DEPENDS  += $(UI_OPT)
CFLAGS_DEPENDS  += $(USB_OPT)
CFLAGS_DEPENDS  += $(IVE_QR_MARKER_OPT)

### Assember
AS  := $(TCHAIN)as
ASFLAGS  := -c
ASFLAGS  += -pipe
ASFLAGS  += -fno-builtin
ASFLAGS  += -ffreestanding
ASFLAGS  += -mtune=cortex-a7
ASFLAGS  += -g3
ASFLAGS  += -march=armv7-a
ifneq "$(ENABLE_NEON)" ""
ASFLAGS  += -mfloat-abi=hard
ASFLAGS  += -mfpu=neon-vfpv4
else
ASFLAGS  += -mfloat-abi=soft
endif
ASFLAGS  += -mthumb-interwork
ASFLAGS  += -x assembler-with-cpp
ASFLAGS  += $(MILB_COMMON_OPT)
ASFLAGS  += $(MILB_OPT)
ASFLAGS  += $(AUDIO_OPT)
ASFLAGS  += $(LCD_OPT)
ASFLAGS  += $(USER_OPT)
ASFLAGS  += $(CMOS_OPT)
ASFLAGS  += $(FASTBOOT_OPT)
ASFLAGS  += $(BOOTVERSION_OPT)
ASFLAGS  += $(BOARD_OPT)
ASFLAGS  += $(3A_OPT)
ASFLAGS  += $(FS_OPT)
ASFLAGS  += $(UI_OPT)
ASFLAGS  += $(USB_OPT)
ASFLAGS  += $(IVE_QR_MARKER_OPT)

ASFLAGS_DEPENDS  := -fsyntax-only
ASFLAGS_DEPENDS  += -pipe
ASFLAGS_DEPENDS  += -mcpu=$(ARMCORE)
ASFLAGS_DEPENDS  += -x assembler-with-cpp
ASFLAGS_DEPENDS  += $(MILB_COMMON_OPT)
ASFLAGS_DEPENDS  += $(MILB_OPT)
ASFLAGS_DEPENDS  += $(AUDIO_OPT)
ASFLAGS_DEPENDS  += $(LCD_OPT)
ASFLAGS_DEPENDS  += $(USER_OPT)
ASFLAGS_DEPENDS  += $(CMOS_OPT)
ASFLAGS_DEPENDS  += $(FASTBOOT_OPT)
ASFLAGS_DEPENDS  += $(BOOTVERSION_OPT)
ASFLAGS_DEPENDS  += $(BOARD_OPT)
ASFLAGS_DEPENDS  += $(3A_OPT)
ASFLAGS_DEPENDS  += $(FS_OPT)
ASFLAGS_DEPENDS  += $(UI_OPT)
ASFLAGS_DEPENDS  += $(USB_OPT)
ASFLAGS_DEPENDS  += $(IVE_QR_MARKER_OPT)

##### REMOVE_PGRELIEF BEGIN #####
### PGRelief
ifeq "$(IS_USE_PGRELIEF)" "y"
# Create UNIX path
EPOMDIR_POSIX     = $(shell cygpath --unix "$(EPOMDIR)" | sed -e 's/\\/\\\\/g')

# Create Windows path
PGR_CMD           = "$(EPOMDIR_POSIX)/pgr5.exe"
PGR_TMP           = $(shell cygpath --windows "$(TMP)" | sed -e 's/\\/\\\\/g')
PGR_CURDIR        = $(shell cygpath --windows "$(CURDIR)" | sed -e 's/\\/\\\\/g')
PGR_STDC_INCLUDE1 = $(shell cygpath --windows "$(GNUarmInclude1)" | sed -e 's/\\/\\\\/g')
PGR_STDC_INCLUDE2 = $(shell cygpath --windows "$(GNUarmInclude2)" | sed -e 's/\\/\\\\/g')
PGR_STDC_INCLUDE3 = $(shell cygpath --windows "$(GNUarmInclude3)" | sed -e 's/\\/\\\\/g')
endif

PGRFLAGS          = -c
PGRFLAGS         += --csv
PGRFLAGS         += --output_code SJIS
PGRFLAGS         += --slashcomment
PGRFLAGS         += $(CINCPATH)
PGRFLAGS         += -O "$(PGR_OPT_FILE)"
PGRFLAGS         += -S UTF8
PGRFLAGS         += -Y "$(PGR_STDC_INCLUDE1)"
PGRFLAGS         += -Y "$(PGR_STDC_INCLUDE2)"
PGRFLAGS         += -Y "$(PGR_STDC_INCLUDE3)"
PGRFLAGS         += -Z "$(EPOMDIR)\MessageInfo\default-pgr.rul"
PGRFLAGS         += --C99
# Settings for arm-none-eabi-gcc.
#   investigated "/home/mptk/prefix/share/info/cpp.info" and "gcc -E -dM" output.
PGRFLAGS         += -K GNU/GCC4
PGRFLAGS         += -D__GNUC__
# Workaround of following PG-Relief error.
#   "C:\cygwin\home\mptk\prefix\arm-none-eabi\include\machine\ieeefp.h",406,"!","pgr0455","#error  Endianess not declared!!"
PGRFLAGS         += -D__IEEE_LITTLE_ENDIAN
# Workaround of following PG-Relief error.
#   "C:\cygwin\home\mptk\prefix\arm-none-eabi\include\sys\_intsup.h",74,"!","pgr0455","#error  ""Unable to determine type definition of intptr_t""","#error ""Unable to determine type definition of intptr_t"""
PGRFLAGS         += -D__INT8_TYPE__="signed char"
PGRFLAGS         += -D__INT16_TYPE__="short int"
PGRFLAGS         += -D__INT32_TYPE__="long int"
PGRFLAGS         += -D__INT64_TYPE__="long long int"
PGRFLAGS         += -D__UINT8_TYPE__="unsigned char"
PGRFLAGS         += -D__UINT16_TYPE__="short unsigned int"
PGRFLAGS         += -D__UINT32_TYPE__="long unsigned int"
PGRFLAGS         += -D__UINT64_TYPE__="long long unsigned int"
PGRFLAGS         += -D__INTPTR_TYPE__="int"
# Workaround of following PG-Relief error.
#   "C:\cygwin\home\mptk\prefix_for_bin\arm-none-eabi\include\stdatomic.h",204,"!","pgr0431"
PGRFLAGS         += -D___int_least8_t_defined=1
PGRFLAGS         += -Dint_least8_t="signed char"
PGRFLAGS         += -Duint_least8_t="unsigned char"
PGRFLAGS         += -D___int_least16_t_defined=1
PGRFLAGS         += -Dint_least16_t="short int"
PGRFLAGS         += -Duint_least16_t="short unsigned int"
PGRFLAGS         += -D___int_least32_t_defined=1
PGRFLAGS         += -Dint_least32_t="long int"
PGRFLAGS         += -Duint_least32_t="long unsigned int"
PGRFLAGS         += -D___int_least64_t_defined=1
PGRFLAGS         += -Dint_least64_t="long long int"
PGRFLAGS         += -Duint_least64_t="long long unsigned int"
PGRFLAGS         += -D___int_fast8_t_defined=1
PGRFLAGS         += -Dint_fast8_t="signed char"
PGRFLAGS         += -Duint_fast8_t="unsigned char"
PGRFLAGS         += -D___int_fast16_t_defined=1
PGRFLAGS         += -Dint_fast16_t="short int"
PGRFLAGS         += -Duint_fast16_t="short unsigned int"
PGRFLAGS         += -D___int_fast32_t_defined=1
PGRFLAGS         += -Dint_fast32_t="long int"
PGRFLAGS         += -Duint_fast32_t="long unsigned int"
PGRFLAGS         += -D___int_fast64_t_defined=1
PGRFLAGS         += -Dint_fast64_t="long long int"
PGRFLAGS         += -Duint_fast64_t="long long unsigned int"
PGRFLAGS         += -D__INTMAX_TYPE__="long long int"
PGRFLAGS         += -D__UINTMAX_TYPE__="long long unsigned int"
PGRFLAGS         += -Dintmax_t="int_fast64_t"
PGRFLAGS         += -Duintmax_t="uint_fast64_t"

##### REMOVE_PGRELIEF END #####



#
# Directory and File names
#

OBJ_DIR_NAME     ?= obj
SRC_DIR_NAME     := src
ASM_DIR_NAME     := asm
DEP_DIR_NAME     := dep
INC_DIR_NAME     := include
OBJ_FILE          = $(OBJ_DIR_NAME)/%.o
SRC_CFILE         = $(SRC_DIR_NAME)/%.c
SRC_AFILE         = $(SRC_DIR_NAME)/%.s
DEP_FILE          = $(DEP_DIR_NAME)/%.d
CUROBJSFILE       = $(ROOT_DIR)/cpu_tkernel/$(WS_DIR_NAME)/$(OBJ_DIR_NAME)/$(strip $(subst /,_, $(DIR))).objs
OPT_FILES         = $(OPT_FILE_COMMON) $(OPT_FILE_AUDIO) $(OPT_FILE_CODEC) $(OPT_FILE_LCD) $(OPT_FILE_USER) $(OPT_FILE_CMOS) $(OPT_FILE_IPCU) $(OPT_FILE_FAST) $(OPT_FILE_BOOT) $(OPT_FILE_BOARD)
##### REMOVE_PGRELIEF BEGIN #####
PGR_OPT_FILE      = $(ROOT_DIR)/cpu_tkernel/project/MILB_SDK.pgr
PGR_CSV_FILE      = $(ROOT_DIR)/PGRelief_out.csv
##### REMOVE_PGRELIEF END #####
CSRC_TMP          = $(patsubst %.c,$(SRC_DIR_NAME)/%.c,$(CSRC))
ASRC_TMP          = $(patsubst %.s,$(SRC_DIR_NAME)/%.s,$(ASRC))
OBJS              = $(CSRC_TMP:$(SRC_DIR_NAME)/%.c=$(OBJ_DIR_NAME)/%.o)
OBJS             += $(ASRC_TMP:$(SRC_DIR_NAME)/%.s=$(OBJ_DIR_NAME)/%.o)
DEPS              = $(CSRC_TMP:$(SRC_DIR_NAME)/%.c=$(DEP_DIR_NAME)/%.d)
DEPS             += $(ASRC_TMP:$(SRC_DIR_NAME)/%.s=$(DEP_DIR_NAME)/%.d)
ALL_SRC           = $(CSRC)
ALL_SRC          += $(ASRC)



#
# Targets
#

QUIET :=

.PHONY: default target-self recursive-target depends-self clean-self distclean-self $(SUBDIRS) \
        target target-objs-self clean clean-obj clean-obj-self clean-depends-self depends message message-self

default: target

target-self:
ifneq "$(words $(OBJS))" "0"
##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)if [ ! -d $(TMP)/PGR_output ] ; then mkdir $(TMP)/PGR_output >/dev/null 2>&1; fi
endif
##### REMOVE_PGRELIEF END #####
	-$(QUIET)$(MAKE) target-objs-self; \
	$(QUIET)$(MAKE) IS_INCLUDE_DEPS=y $(MULTI_JOB_OPT) $(OBJS)
##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)cat $(OBJ_DIR_NAME)/*_pgr.csv >> $(PGR_CSV_FILE) 2> /dev/null; \
	$(QUIET)rm -f $(OBJ_DIR_NAME)/*_pgr.csv
endif
##### REMOVE_PGRELIEF END #####
endif

recursive-target: $(PRE_TARGET) $(SUBDIRS) $(POST_TARGET)

depends-self:
ifneq "$(words $(DEPS))" "0"
	-$(QUIET)$(MAKE) $(MULTI_JOB_OPT) $(DEPS)
endif

clean-self: clean-obj-self clean-depends-self

distclean-self: clean-self

ifeq "$(IS_INCLUDE_DEPS)" "y"
-include $(DEPS)
endif

### Sub Folders
ifneq "$(SUBDIRS)" ""
$(SUBDIRS): 
	$(QUIET)$(MAKE) recursive-target -C $@
endif

### Build
target: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=target-self

target-objs-self: 
ifneq "$(words $(ALL_SRC))" "0"
	-$(QUIET)if [ ! -d $(ROOT_DIR)/cpu_tkernel/$(WS_DIR_NAME)/$(OBJ_DIR_NAME) ] ; then mkdir $(ROOT_DIR)/cpu_tkernel/$(WS_DIR_NAME)/$(OBJ_DIR_NAME); fi; \
	$(QUIET)if [ -f $(CUROBJSFILE) ] ; then rm -f $(CUROBJSFILE); fi; \
	$(QUIET)echo $(patsubst $(OBJ_DIR_NAME)/%.o, $(DIR)/$(OBJ_DIR_NAME)/%.o, $(OBJS)) > $(CUROBJSFILE)
endif

$(OBJ_FILE): $(SRC_CFILE)
	-$(QUIET)if [ ! -d $(OBJ_DIR_NAME) ] ; then mkdir $(OBJ_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo compiling $@ ...; \
	$(QUIET)$(CC) $(CFLAGS) $(CINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)	
#	$(QUIET)$(CC) $(CFLAGS) -E -dM $(CINCPATH) $< -o $(patsubst %.c,%.m,$<)
ifeq "$(IS_GET_ASM)" "y"
	-$(QUIET)if [ ! -d $(ASM_DIR_NAME) ] ; then mkdir $(ASM_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)$(CC) $(CFLAGS) $(CINCPATH) -S $< -o $(patsubst $(SRC_DIR_NAME)/%.c,$(ASM_DIR_NAME)/%.s,$<)
endif
##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)$(PGR_CMD) $(PGRFLAGS) --temp-file "$(PGR_TMP)\PGR_output\$(<F)" "$(PGR_CURDIR)\$<" > $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%_pgr.csv,$<)
endif
##### REMOVE_PGRELIEF END #####

$(OBJ_FILE): $(SRC_AFILE)
	-$(QUIET)if [ ! -d $(OBJ_DIR_NAME) ] ; then mkdir $(OBJ_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo assembling $@ ...; \
	$(QUIET)$(CC) $(ASFLAGS) $(AINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)

### Clean
clean: 
	$(QUIET)$(MAKE) $(MULTI_JOB_OPT) recursive-target POST_TARGET=clean-self

clean-obj: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=clean-obj-self

clean-obj-self: 
	-$(QUIET)if [ -d $(OBJ_DIR_NAME) ] ; then rm -rf $(OBJ_DIR_NAME); fi; \
	$(QUIET)if [ -d $(ASM_DIR_NAME) ] ; then rm -rf $(ASM_DIR_NAME); fi;

clean-depends-self: 
	$(QUIET)if [ -d $(DEP_DIR_NAME) ] ; then rm -rf $(DEP_DIR_NAME); fi;

### Depends
depends: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=depends-self

$(DEP_FILE): $(SRC_CFILE)
	-$(QUIET)if [ ! -d $(DEP_DIR_NAME) ] ; then mkdir $(DEP_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo creating $@ ...; \
	$(QUIET)$(CC) $(CFLAGS_DEPENDS) $(CINCPATH) -MT $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%.o,$<) -MT $@ -M -MF $@ $< 

$(DEP_FILE): $(SRC_AFILE)
	-$(QUIET)if [ ! -d $(DEP_DIR_NAME) ] ; then mkdir $(DEP_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo creating $@ ...; \
	$(QUIET)$(CC) $(ASFLAGS_DEPENDS) $(AINCPATH) -MT $(patsubst $(SRC_DIR_NAME)/%.s,$(OBJ_DIR_NAME)/%.o,$<) -MT $@ -M -MF $@ $< 

### Message
message: 
	$(QUIET)$(MAKE) recursive-target PRE_TARGET=message-self

message-self: 
ifneq "$(MESSAGE)" ""
	$(QUIET)echo $(MESSAGE)
endif

print-compile-option:
	@echo $(MILB_OPT) $(AUDIO_OPT) $(LCD_OPT) $(USER_OPT) $(CMOS_OPT) $(FASTBOOT_OPT) $(BOOTVERSION_OPT) $(BOARD_OPT) $(CHIP_ES_OPT) $(MEM_SIZE_OPT) $(FS_OPT) $(UI_OPT) $(USB_OPT) > $(PGR_OPT_FILE)

