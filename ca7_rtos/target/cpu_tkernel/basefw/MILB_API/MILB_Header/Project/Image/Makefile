### Root Directory of the Project
ROOT_DIR     := ../..

### The current path from Root Directory.
DIR          := Project/Image

### The List of Sub-Directory which have Makefile
### note: If current directory does not have the sub-directory, Variable "SUBDIR" is empty.
SUBDIRS      :=

### List of source files
### note: If current directory does not have C/ASM source files in "src" directory, CSRC/ASRC is empty.
CSRC :=
CSRC += jdsb2r_f2e.c
CSRC += jdscnr1a.c
CSRC += jdsdisp3a.c
CSRC += jdsela2d.c
CSRC += jdsfpt.c
CSRC += jdsiip4b.c
CSRC += jdsimg.c
CSRC += jdsjp7.c
CSRC += jdsltm1a.c
CSRC += jdsmxic.c
CSRC += jdspro.c
CSRC += jdsr2y_f2e.c
CSRC += jdsraw3a.c
CSRC += jdsrdma.c
CSRC += jdsshdr.c
CSRC += jdsxch.c
CSRC += jmlbmh2a.c
##### REMOVE_ME_RELEASE BEGIN #####
CSRC += jdsme.c
##### REMOVE_ME_RELEASE END #####
CSRC += sdramc.c

ASRC :=

##### REMOVE_ME_RELEASE BEGIN #####
ifeq "$(BUILD_JDSME_OBJ)" "yes"
# target source overwrite only "jdsme.c".
CSRC :=
CSRC += jdsme.c
ASRC :=
endif
##### REMOVE_ME_RELEASE END #####




WS_DIR_NAME  ?= Project
include $(ROOT_DIR)/$(WS_DIR_NAME)/common.mk





### Special compile/assemble
