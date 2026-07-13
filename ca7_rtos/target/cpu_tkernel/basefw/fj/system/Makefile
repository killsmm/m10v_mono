### Root Directory of the Project
ROOT_DIR     := ../../../..

### The current path from Root Directory.
DIR          := fj/system

### The List of Sub-Directory which have Makefile
### note: If current directory does not have the sub-directory, Variable "SUBDIR" is empty.
SUBDIRS      :=

### List of source files
### note: If current directory does not have C/ASM source files in "src" directory, CSRC/ASRC is empty.
CSRC :=
CSRC += bf_arm.c
CSRC += bf_startup.c
CSRC += bf_version.c
CSRC += boot.c
CSRC += boot_loader.c
CSRC += device_config.c
CSRC += host.c
CSRC += interrupt.c
CSRC += memory.c
CSRC += mode.c
CSRC += system_attr.c
CSRC += updater.c
CSRC += bus_arb.c
CSRC += top.c
CSRC += bf_compile_info_build.c
CSRC += bf_compile_info_update.c
ASRC :=
ASRC += cacheops-v7.s
ASRC += init.s
ASRC += gic.s





include $(ROOT_DIR)/common.mk

.PHONY: re_create_update_compile_info re_create_build_compile_info re_create_build_compile_info_once

re_create_update_compile_info:
	$(QUIET)echo creating $(SRC_DIR_NAME)/bf_compile_info_update.c ...; \
	cat $(SRC_DIR_NAME)/bf_compile_info_xxxx_template.c \
		| sed \
			-e 's/%%FILENAME%%/bf_compile_info_update/g' \
			-e 's/%%FUNC_PREFIX%%/Update/g' \
			-e 's/%%DATETIME_VAR_NAME%%/g_bf_compile_info_update_datetime_str/g' \
			-e 's!%%DATETIME_VAL%%!'"`LANG=C date '+%Y/%m/%d %H:%M:%S %Z'`"'!g' \
			-e 's/%%OPT_VAR_NAME%%/g_bf_compile_info_update_opt_str/g' \
			-e '/%%OPT_VAL%%/{' \
				-e 's/"/%%DQUAT%%/g' \
				-e 's/%%OPT_VAL%%/$(CFLAGS)/g' \
				-e 's/"/\\"/g' \
				-e 's/[	 ][	 ]*/ /g' \
				-e 's/%%DQUAT%%/"/g' \
			-e '}' \
		> $(SRC_DIR_NAME)/bf_compile_info_update.c

re_create_build_compile_info_once:
	if [ ! -f $(SRC_DIR_NAME)/bf_compile_info_build.c ] ; then \
		$(MAKE) re_create_build_compile_info; \
	fi

re_create_build_compile_info:
	$(QUIET)echo creating $(SRC_DIR_NAME)/bf_compile_info_build.c ...; \
	cat $(SRC_DIR_NAME)/bf_compile_info_xxxx_template.c \
		| sed \
			-e 's/%%FILENAME%%/bf_compile_info_build/g' \
			-e 's/%%FUNC_PREFIX%%/Build/g' \
			-e 's/%%DATETIME_VAR_NAME%%/g_bf_compile_info_build_datetime_str/g' \
			-e 's!%%DATETIME_VAL%%!'"`LANG=C date '+%Y/%m/%d %H:%M:%S %Z'`"'!g' \
			-e 's/%%OPT_VAR_NAME%%/g_bf_compile_info_build_opt_str/g' \
			-e '/%%OPT_VAL%%/{' \
				-e 's/"/%%DQUAT%%/g' \
				-e 's/%%OPT_VAL%%/$(CFLAGS)/g' \
				-e 's/"/\\"/g' \
				-e 's/[	 ][	 ]*/ /g' \
				-e 's/%%DQUAT%%/"/g' \
			-e '}' \
		> $(SRC_DIR_NAME)/bf_compile_info_build.c

$(SRC_DIR_NAME)/bf_compile_info_build.c:
	$(MAKE) re_create_build_compile_info

$(SRC_DIR_NAME)/bf_compile_info_update.c:
	$(MAKE) re_create_update_compile_info

clean-self:
	rm -f $(SRC_DIR_NAME)/bf_compile_info_build.c $(SRC_DIR_NAME)/bf_compile_info_update.c


### Special compile/assemble
