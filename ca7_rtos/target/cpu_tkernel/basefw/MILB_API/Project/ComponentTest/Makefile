### Root Directory of the Project
ROOT_DIR     := ../..

### The current path from Root Directory.
DIR          := Project/ComponentTest

### The List of Sub-Directory which have Makefile
### note: If current directory does not have the sub-directory, Variable "SUBDIR" is empty.
SUBDIRS      :=

### List of source files
### note: If current directory does not have C/ASM source files in "src" directory, CSRC/ASRC is empty.
CSRC :=
CSRC += ct_dd_arm.c
CSRC += ct_dd_audio.c
CSRC += ct_dd_cache.c
CSRC += ct_dd_csio.c
CSRC += ct_dd_exiu.c
CSRC += ct_dd_exs.c
CSRC += ct_dd_gic.c
CSRC += ct_dd_gpv.c
CSRC += ct_dd_hdmac.c
CSRC += ct_dd_hdmac0.c
CSRC += ct_dd_hdmac1.c
CSRC += ct_dd_ipcu.c
CSRC += ct_dd_i2c.c
CSRC += ct_dd_mcc.c
CSRC += ct_dd_nic.c
CSRC += ct_dd_pcie_ep.c
CSRC += ct_dd_pcie_rc.c
CSRC += ct_dd_pmu.c
CSRC += ct_dd_pwm.c
##### REMOVE_RELEASE BEGIN #####
CSRC += ct_dd_relc.c
##### REMOVE_RELEASE END #####
CSRC += ct_dd_timestamp.c
CSRC += ct_dd_tmr32.c
CSRC += ct_dd_tmr64.c
CSRC += ct_dd_top.c
CSRC += ct_dd_uart.c
CSRC += ct_dd_udc.c
CSRC += ct_dd_wdog.c
CSRC += ct_dd_xdmasnap.c
CSRC += ct_dd_xdmac.c
CSRC += ct_dd_spi.c
CSRC += ct_dd_pdm.c
CSRC += ct_dd_slimbus.c
CSRC += ct_im_b2r.c
CSRC += ct_im_bmh.c
CSRC += ct_im_cnr.c
CSRC += ct_im_disp.c
CSRC += ct_im_ela.c
CSRC += ct_im_fpt.c
CSRC += ct_im_iip.c
CSRC += ct_im_iip_afn.c
CSRC += ct_im_iip_blend.c
CSRC += ct_im_iip_cfl.c
CSRC += ct_im_iip_csc.c
CSRC += ct_im_iip_flt.c
CSRC += ct_im_iip_frect.c
CSRC += ct_im_iip_frecttbl.c
CSRC += ct_im_iip_gpc.c
CSRC += ct_im_iip_ld.c
CSRC += ct_im_iip_lut.c
CSRC += ct_im_iip_mft.c
CSRC += ct_im_iip_mon.c
CSRC += ct_im_iip_sl.c
CSRC += ct_im_iip_utility.c
CSRC += ct_im_img.c
CSRC += ct_im_ltm_rbk.c
CSRC += ct_im_ltm_map.c
CSRC += ct_im_ltm_ltm.c
CSRC += ct_im_mxic.c
CSRC += ct_im_r2y.c
CSRC += ct_im_raw.c
CSRC += ct_im_sdramc.c
CSRC += ct_im_shdr.c
CSRC += ct_im_xch.c
CSRC += ct_im_jpeg.c
CSRC += ct_im_hdmi.c
CSRC += ct_im_mipi.c
CSRC += ct_im_pro.c
##### REMOVE_ME_RELEASE BEGIN #####
CSRC += ct_im_me.c
##### REMOVE_ME_RELEASE END #####


ASRC :=
ASRC +=

##### REMOVE_ME_RELEASE BEGIN #####
ifeq "$(BUILD_IM_ME_OBJ)" "yes"
# target source overwrite only "ct_im_me.c".
CSRC := ct_im_me.c
ASRC :=
endif
##### REMOVE_ME_RELEASE END #####





WS_DIR_NAME ?= Project
include $(ROOT_DIR)/$(WS_DIR_NAME)/common.mk





### Special compile/assemble
