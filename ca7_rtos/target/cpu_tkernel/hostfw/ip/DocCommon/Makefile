### Root Directory of the Project
ROOT_DIR     := ../../../..

### The current path from Root Directory.
DIR          := hostfw/ip/DocCommon

### The List of Sub-Directory which have Makefile
### note: If current directory does not have the sub-directory, Variable "SUBDIR" is empty.
SUBDIRS      := 

### List of source files
### note: If current directory does not have C/ASM source files in "src" directory, CSRC/ASRC is empty.
CSRC :=
CSRC += ahip_custom.c
CSRC += ip_fnc.c
ASRC :=

SUB_CINCPATH := -I$(RTOS)/rtos2/ARM/syslib
SUB_CINCPATH += -I$(ROOT_DIR)/../include

include $(ROOT_DIR)/common.mk

### Special compile/assemble
