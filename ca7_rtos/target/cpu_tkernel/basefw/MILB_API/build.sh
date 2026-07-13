#! /bin/sh

proj_top_dir=`dirname "$0"`
cd "${proj_top_dir}"

. ./shell_subr.sh


set_multi_compile_opt


make_clean
make_build
log_check


ok_exit

