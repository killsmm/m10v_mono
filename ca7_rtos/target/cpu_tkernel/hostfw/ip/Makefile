### Root Directory of the Project
ROOT_DIR     := ../../..

### The current path from Root Directory.
DIR          := hostfw/ip

### The List of Sub-Directory which have Makefile
### note: If current directory does not have the sub-directory, Variable "SUBDIR" is empty.
SUBDIRS      := 
### --- REMOVE_AHIP BEGIN ---
SUBDIRS      += ahip
SUBDIRS      += DocCommon
SUBDIRS      += warp
### --- REMOVE_AHIP END ---
### --- REMOVE_EIS BEGIN ---
SUBDIRS      += eis
### --- REMOVE_EIS END ---
### --- REMOVE_FD BEGIN ---
SUBDIRS      += fd
### --- REMOVE_FD END ---
### --- REMOVE_PD BEGIN ---
SUBDIRS      += pd
### --- REMOVE_PD END ---
### --- REMOVE_PANO BEGIN ---
SUBDIRS      += panorama
### --- REMOVE_PANO END ---
### --- REMOVE_SGDE BEGIN ---
SUBDIRS      += sgde
### --- REMOVE_SGDE END ---
### --- REMOVE_GPU BEGIN ---
SUBDIRS      += gpu
### --- REMOVE_GPU END ---
### --- REMOVE_SHDR2 BEGIN ---
SUBDIRS      += shdr
### --- REMOVE_SHDR2 END ---
SUBDIRS      += stc

### List of source files
### note: If current directory does not have C/ASM source files in "src" directory, CSRC/ASRC is empty.
CSRC :=
ASRC :=

include $(ROOT_DIR)/common.mk

### Special compile/assemble
