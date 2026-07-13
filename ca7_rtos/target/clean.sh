#! /bin/sh

proj_top_dir=`dirname "$0"`
cd "${proj_top_dir}"


PROJ_NAME=MILB_SDK


. ./shell_common.sh
set_multi_compile_opt


make_clean


ok_exit
