### Root Directory of the Project
ROOT_DIR     := ../..

### The current path from Root Directory.
DIR          := media_meta

### The List of Sub-Directory which have Makefile
### note: If current directory does not have the sub-directory, Variable "SUBDIR" is empty.
SUBDIRS      := 
# --- REMOVE_DCF_EXIF BEGIN ---
SUBDIRS      += dcf
SUBDIRS      += exif
# --- REMOVE_DCF_EXIF END ---
SUBDIRS      += mp4

### List of source files
### note: If current directory does not have C/ASM source files in "src" directory, CSRC/ASRC is empty.
CSRC :=
# --- REMOVE_DCF_EXIF BEGIN ---
CSRC += fj_dcf.c
CSRC += fj_exif.c
# --- REMOVE_DCF_EXIF END ---
CSRC += fj_mux.c
CSRC += fj_demux.c
CSRC += fj_mux_demux_common.c

ASRC :=


include $(ROOT_DIR)/common.mk


### Special compile/assemble
