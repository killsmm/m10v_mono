### Root Directory of the Project
ROOT_DIR     := ../../..

### The current path from Root Directory.
DIR          := Project/DeviceDriver/Exs

### The List of Sub-Directory which have Makefile
### note: If current directory does not have the sub-directory, Variable "SUBDIR" is empty.
SUBDIRS      :=

### List of source files
### note: If current directory does not have C/ASM source files in "src" directory, CSRC/ASRC is empty.
CSRC := dd_exs.c
CSRC += dd_gpv.c
CSRC += dd_hdmac.c
##### REMOVE_RELEASE BEGIN #####
CSRC += dd_relc.c
##### REMOVE_RELEASE END #####
CSRC += dd_pcie_rc.c
CSRC += dd_pcie_ep.c
CSRC += dd_xdmac.c

ASRC :=
ASRC +=





WS_DIR_NAME ?= Project
include $(ROOT_DIR)/$(WS_DIR_NAME)/common.mk





### Special compile/assemble
