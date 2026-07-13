### Root Directory of the Project
ROOT_DIR     := ../..

### The current path from Root Directory.
DIR          := Project/ImageMacro

### The List of Sub-Directory which have Makefile
### note: If current directory does not have the sub-directory, Variable "SUBDIR" is empty.
SUBDIRS      :=

### List of source files
### note: If current directory does not have C/ASM source files in "src" directory, CSRC/ASRC is empty.
CSRC :=
CSRC += im_b2r.c
CSRC += im_bmh.c
CSRC += im_cnr.c
CSRC += im_ela.c
CSRC += im_fpt.c
CSRC += im_iip.c
CSRC += im_iip_param.c
CSRC += im_img.c
CSRC += im_jpeg.c
CSRC += im_ltm_common.c
CSRC += im_ltm_rbk.c
CSRC += im_ltm_map.c
CSRC += im_ltm_ltm.c
CSRC += im_mxic.c
CSRC += im_pro_b2b.c
CSRC += im_pro_common.c
CSRC += im_pro_interrupt.c
CSRC += im_pro_pas.c
CSRC += im_pro_sen.c
CSRC += im_pro_sro.c
CSRC += im_pro_stat.c
CSRC += im_r2y.c
CSRC += im_raw.c
CSRC += im_rdma.c
CSRC += im_sdramc.c
CSRC += im_shdr.c
CSRC += im_xch.c
CSRC += im_disp.c
CSRC += im_hdmi.c
CSRC += im_mipi.c
##### REMOVE_ME_RELEASE BEGIN #####
CSRC += im_me.c
##### REMOVE_ME_RELEASE END #####


ASRC :=
ASRC += im_fpt_asm.s

##### REMOVE_ME_RELEASE BEGIN #####
ifeq "$(BUILD_IM_ME_OBJ)" "yes"
# target source overwrite only "im_me.c".
CSRC := im_me.c
ASRC :=
endif
##### REMOVE_ME_RELEASE END #####





WS_DIR_NAME ?= Project
include $(ROOT_DIR)/$(WS_DIR_NAME)/common.mk





### Special compile/assemble
