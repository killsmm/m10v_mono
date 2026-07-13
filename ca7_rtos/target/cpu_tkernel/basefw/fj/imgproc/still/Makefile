### Root Directory of the Project
ROOT_DIR     := ../../../../..

### The current path from Root Directory.
DIR          := fj/imgproc/still

### The List of Sub-Directory which have Makefile
### note: If current directory does not have the sub-directory, Variable "SUBDIR" is empty.
SUBDIRS      :=

### List of source files
### note: If current directory does not have C/ASM source files in "src" directory, CSRC/ASRC is empty.
CSRC :=
CSRC += still.c
CSRC += still_attr.c
CSRC += still_basic.c
#Delete basefw/fj/imgproc/still
#CSRC += still_b2r_ctrl.c
#CSRC += still_b2r_ctrl_tbl.c
CSRC += still_boot.c
#CSRC += still_cap_ctrl.c
#CSRC += still_cnr_otf_ctrl.c
#CSRC += still_cnr_otf_ctrl_tbl.c
#CSRC += still_common.c
#CSRC += still_connectivity_manager.c
CSRC += still_dcache.c
#CSRC += still_debug.c
#CSRC += still_dzoom.c
#CSRC += still_effect.c
#CSRC += still_ela_nr.c
#CSRC += still_frect_task.c
#CSRC += still_frect_tbl_mng.c
CSRC += still_iip.c
CSRC += still_img_info.c
#CSRC += still_jpeg_capture.c
#CSRC += still_ltm_ctrl.c
#CSRC += still_ltm_ctrl_tbl.c
#CSRC += still_memory_ctrl.c
CSRC += still_mov_ctrl.c
#CSRC += still_piv_manager.c
#CSRC += still_preview.c
#CSRC += still_pro_ctrl.c
CSRC += still_proc_subr.c
#CSRC += still_r2y_ctrl.c
#CSRC += still_r2y_ctrl_tbl.c
#CSRC += still_snr_seq.c
#CSRC += still_spr_nr.c
#CSRC += still_sync_ctrl.c
CSRC += still_utility.c
#CSRC += still_view.c
#CSRC += still_view_seq.c
#CSRC += still_view_seq_worker.c
#CSRC += still_pro_sen_ctrl.c
#CSRC += still_pro_sro_ctrl.c
#CSRC += still_pro_b2b_ctrl.c
#CSRC += still_pro_stat_ctrl.c
#CSRC += still_pro_pas_ctrl.c
#CSRC += still_pro_common_ctrl.c
#CSRC += still_pro_interrupt_ctrl.c
ASRC :=





include $(ROOT_DIR)/common.mk





### Special compile/assemble

$(OBJ_DIR_NAME)/still_pro_interrupt_ctrl.o : $(SRC_DIR_NAME)/still_pro_interrupt_ctrl.c
	-$(QUIET)if [ ! -d $(OBJ_DIR_NAME) ] ; then mkdir $(OBJ_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo custom compiling $< ...; \
	$(QUIET)$(CC) $(CFLAGS) -Os -mthumb $(CINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)
