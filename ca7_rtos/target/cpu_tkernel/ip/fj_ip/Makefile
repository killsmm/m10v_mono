### Root Directory of the Project
ROOT_DIR     := ../../..

### The current path from Root Directory.
DIR          := ip/fj_ip

### The List of Sub-Directory which have Makefile
### note: If current directory does not have the sub-directory, Variable "SUBDIR" is empty.
SUBDIRS      := IPC
SUBDIRS      += IPT
SUBDIRS      += 3a
SUBDIRS		 += ImageTuning
SUBDIRS		 += IQBin
SUBDIRS		 += PDAF
### --- REMOVE_GPU BEGIN ---
SUBDIRS      += gpu
### --- REMOVE_GPU END ---
### --- REMOVE_AHA BEGIN ---
###SUBDIRS   += panorama_grid
### --- REMOVE_AHA END ---

### List of source files
### note: If current directory does not have C/ASM source files in "src" directory, CSRC/ASRC is empty.
CSRC :=
ASRC :=

include $(ROOT_DIR)/common.mk

### Special compile/assemble
