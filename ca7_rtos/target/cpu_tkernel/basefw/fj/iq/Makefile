### Root Directory of the Project
ROOT_DIR     := ../../../..

### The current path from Root Directory.
DIR          := fj/iq

### The List of Sub-Directory which have Makefile
### note: If current directory does not have the sub-directory, Variable "SUBDIR" is empty.
SUBDIRS      :=

### List of source files
### note: If current directory does not have C/ASM source files in "src" directory, CSRC/ASRC is empty.
CSRC :=
CSRC += iq_calib.c
CSRC += iq_common.c
CSRC += iq_dcache.c
CSRC += iq_dpc_conv.c
CSRC += iq_dpc_detect_info.c
CSRC += iq_dpc_detect_sw.c
CSRC += iq_dpc_detect_sw_if.c
CSRC += iq_dpc_file.c
CSRC += iq_dpc_flip.c
CSRC += iq_dpc_multipipe.c
CSRC += iq_dpc_multipipe_tbl.c
CSRC += iq_dpc_multipipe_tbl_mng.c
CSRC += iq_dpc_tbl.c
CSRC += iq_dpc_tbl_dec.c
CSRC += iq_dpc_tbl_enc.c
CSRC += iq_dpc_tbl_sel.c
CSRC += iq_storage.c
ASRC :=





include $(ROOT_DIR)/common.mk





### Special compile flags

#CFLAGS+=-O0
#CFLAGS+=-DFRECT_TBL_DUMP


$(OBJ_DIR_NAME)/iq_frect_calc_divby2.o : $(SRC_DIR_NAME)/iq_frect_calc_divby2.c
	-$(QUIET)if [ ! -d $(OBJ_DIR_NAME) ] ; then mkdir $(OBJ_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo custom compiling $< ...; \
	$(QUIET)$(CC) $(CFLAGS) -Ofast -marm $(CINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)
##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)$(PGR_CMD) $(PGRFLAGS) --temp-file "$(PGR_TMP)\PGR_output\$(<F)" "$(PGR_CURDIR)\$<" > $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%_pgr.csv,$<)
endif
##### REMOVE_PGRELIEF END #####

$(OBJ_DIR_NAME)/iq_frect_file_read.o : $(SRC_DIR_NAME)/iq_frect_file_read.c
	-$(QUIET)if [ ! -d $(OBJ_DIR_NAME) ] ; then mkdir $(OBJ_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo custom compiling $< ...; \
	$(QUIET)$(CC) $(CFLAGS) -Ofast -marm $(CINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)
##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)$(PGR_CMD) $(PGRFLAGS) --temp-file "$(PGR_TMP)\PGR_output\$(<F)" "$(PGR_CURDIR)\$<" > $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%_pgr.csv,$<)
endif
##### REMOVE_PGRELIEF END #####

$(OBJ_DIR_NAME)/iq_frect_opt_zoom.o : $(SRC_DIR_NAME)/iq_frect_opt_zoom.c
	-$(QUIET)if [ ! -d $(OBJ_DIR_NAME) ] ; then mkdir $(OBJ_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo custom compiling $< ...; \
	$(QUIET)$(CC) $(CFLAGS) -Ofast -marm $(CINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)
##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)$(PGR_CMD) $(PGRFLAGS) --temp-file "$(PGR_TMP)\PGR_output\$(<F)" "$(PGR_CURDIR)\$<" > $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%_pgr.csv,$<)
endif
##### REMOVE_PGRELIEF END #####

$(OBJ_DIR_NAME)/iq_frect_pos2afn_tbl.o : $(SRC_DIR_NAME)/iq_frect_pos2afn_tbl.c
	-$(QUIET)if [ ! -d $(OBJ_DIR_NAME) ] ; then mkdir $(OBJ_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo custom compiling $< ...; \
	$(QUIET)$(CC) $(CFLAGS) -Ofast -marm $(CINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)
##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)$(PGR_CMD) $(PGRFLAGS) --temp-file "$(PGR_TMP)\PGR_output\$(<F)" "$(PGR_CURDIR)\$<" > $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%_pgr.csv,$<)
endif
##### REMOVE_PGRELIEF END #####

$(OBJ_DIR_NAME)/iq_frect_trim_resize.o : $(SRC_DIR_NAME)/iq_frect_trim_resize.c
	-$(QUIET)if [ ! -d $(OBJ_DIR_NAME) ] ; then mkdir $(OBJ_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo custom compiling $< ...; \
	$(QUIET)$(CC) $(CFLAGS) -Ofast -marm $(CINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)
##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)$(PGR_CMD) $(PGRFLAGS) --temp-file "$(PGR_TMP)\PGR_output\$(<F)" "$(PGR_CURDIR)\$<" > $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%_pgr.csv,$<)
endif
##### REMOVE_PGRELIEF END #####

$(OBJ_DIR_NAME)/iq_dpc_detect_sw.o : $(SRC_DIR_NAME)/iq_dpc_detect_sw.c
	-$(QUIET)if [ ! -d $(OBJ_DIR_NAME) ] ; then mkdir $(OBJ_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo custom compiling $< ...; \
	$(QUIET)$(CC) $(CFLAGS) -Ofast -marm $(CINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)
##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)$(PGR_CMD) $(PGRFLAGS) --temp-file "$(PGR_TMP)\PGR_output\$(<F)" "$(PGR_CURDIR)\$<" > $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%_pgr.csv,$<)
endif
##### REMOVE_PGRELIEF END #####

$(OBJ_DIR_NAME)/iq_frect_calc_split_vertical.o : $(SRC_DIR_NAME)/iq_frect_calc_split_vertical.c
	-$(QUIET)if [ ! -d $(OBJ_DIR_NAME) ] ; then mkdir $(OBJ_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo custom compiling $< ...; \
	$(QUIET)$(CC) $(CFLAGS) -Ofast -marm $(CINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)
##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)$(PGR_CMD) $(PGRFLAGS) --temp-file "$(PGR_TMP)\PGR_output\$(<F)" "$(PGR_CURDIR)\$<" > $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%_pgr.csv,$<)
endif
##### REMOVE_PGRELIEF END #####

$(OBJ_DIR_NAME)/iq_frect_conv_szpowv_8pix.o : $(SRC_DIR_NAME)/iq_frect_conv_szpowv_8pix.c
	-$(QUIET)if [ ! -d $(OBJ_DIR_NAME) ] ; then mkdir $(OBJ_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo custom compiling $< ...; \
	$(QUIET)$(CC) $(CFLAGS) -Ofast -marm $(CINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)
##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)$(PGR_CMD) $(PGRFLAGS) --temp-file "$(PGR_TMP)\PGR_output\$(<F)" "$(PGR_CURDIR)\$<" > $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%_pgr.csv,$<)
endif
##### REMOVE_PGRELIEF END #####

$(OBJ_DIR_NAME)/iq_fshd_calib.o : $(SRC_DIR_NAME)/iq_fshd_calib.c
	-$(QUIET)if [ ! -d $(OBJ_DIR_NAME) ] ; then mkdir $(OBJ_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo custom compiling $< ...; \
	$(QUIET)$(CC) $(CFLAGS) -Ofast -marm $(CINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)
##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)$(PGR_CMD) $(PGRFLAGS) --temp-file "$(PGR_TMP)\PGR_output\$(<F)" "$(PGR_CURDIR)\$<" > $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%_pgr.csv,$<)
endif
##### REMOVE_PGRELIEF END #####

$(OBJ_DIR_NAME)/iq_fshd.o : $(SRC_DIR_NAME)/iq_fshd.c
	-$(QUIET)if [ ! -d $(OBJ_DIR_NAME) ] ; then mkdir $(OBJ_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo custom compiling $< ...; \
	$(QUIET)$(CC) $(CFLAGS) -Ofast -marm $(CINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)
##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)$(PGR_CMD) $(PGRFLAGS) --temp-file "$(PGR_TMP)\PGR_output\$(<F)" "$(PGR_CURDIR)\$<" > $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%_pgr.csv,$<)
endif
##### REMOVE_PGRELIEF END #####
