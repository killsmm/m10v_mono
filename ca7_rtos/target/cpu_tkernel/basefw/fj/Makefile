### Root Directory of the Project
ROOT_DIR     := ../../..

### The current path from Root Directory.
DIR          := fj

### Read Option file

### The List of Sub-Directory which have Makefile
### note: If current directory does not have the sub-directory, Variable "SUBDIR" is empty.
SUBDIRS      :=
#delete_basefw\fj\3a
#SUBDIRS      += 3a
SUBDIRS      += audio
#delete_basefw\fj\command
#SUBDIRS      += command
SUBDIRS      += device_driver
SUBDIRS      += disp
SUBDIRS      += file_mng
SUBDIRS      += iip_lock
SUBDIRS      += imgproc
SUBDIRS      += iq
SUBDIRS      += movie
SUBDIRS      += play
SUBDIRS      += system
SUBDIRS      += utility
### --- REMOVE_USB BEGIN ---
###ifneq "$(IS_USBPTP_MODE)" "OFF"
SUBDIRS      += usb
###endif
### --- REMOVE_USB END ---

### List of source files
### note: If current directory does not have C/ASM source files in "src" directory, CSRC/ASRC is empty.
CSRC :=
CSRC += fj_ae.c
CSRC += fj_af.c
CSRC += fj_arm.c
CSRC += fj_audio.c
CSRC += fj_awb.c
CSRC += fj_boot.c
CSRC += fj_busarb.c
CSRC += fj_calibration.c
### --- REMOVE_CAMERAIF BEGIN ---
CSRC += fj_common.c
### --- REMOVE_CAMERAIF END ---
CSRC += fj_custom.c
CSRC += fj_debug.c
### --- REMOVE_MBA BEGIN ---
CSRC += fj_debug_mba.c
### --- REMOVE_MBA END ---
CSRC += fj_disp.c
CSRC += fj_emergency_log.c
CSRC += fj_filesystem.c
CSRC += fj_flck.c
CSRC += fj_iip_resource.c
CSRC += fj_iq.c
CSRC += fj_media.c
CSRC += fj_memory.c
CSRC += fj_message.c
CSRC += fj_mode.c
CSRC += fj_monitor.c
CSRC += fj_encoder.c
CSRC += fj_decoder.c
CSRC += fj_param.c
CSRC += fj_peripheral.c
CSRC += fj_playback.c
CSRC += fj_still.c
CSRC += fj_top.c
CSRC += fj_updater.c
CSRC += fj_formatter.c
CSRC += fj_pcie_rc.c
CSRC += fj_pcie_ep.c
ASRC :=





include $(ROOT_DIR)/common.mk





### Special compile/assemble
