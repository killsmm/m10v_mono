#! /bin/sh

proj_top_dir=`dirname "$0"`
cd "${proj_top_dir}"


PROJ_NAME=MILB_SDK
export PROJ_NAME


. ./shell_common.sh
set_multi_compile_opt


PGR_SETTINGS
(
	make_update
	log_check
	PGR_POST_PROCESS
) 2>&1 | ${CHARCODE_COMV_CMD}


ok_exit
