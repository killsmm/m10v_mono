### Root Directory of the Project
ROOT_DIR     := ../../..

### The current path from Root Directory.
DIR          := Project/DeviceDriver/ARM

### The List of Sub-Directory which have Makefile
### note: If current directory does not have the sub-directory, Variable "SUBDIR" is empty.
SUBDIRS      :=

### List of source files
### note: If current directory does not have C/ASM source files in "src" directory, CSRC/ASRC is empty.
CSRC :=
CSRC += dd_cache.c
CSRC += dd_exiu.c
CSRC += dd_gic.c
CSRC += dd_nic.c
CSRC += dd_timestamp.c
CSRC += dd_wdog.c
CSRC += dd_xdmasnap.c
CSRC += dd_ipcu.c

ASRC :=
ASRC += dd_arm_asm.s
ASRC += dd_cache_asm.s





WS_DIR_NAME ?= Project
include $(ROOT_DIR)/$(WS_DIR_NAME)/common.mk





### Special compile/assemble
