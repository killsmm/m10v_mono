#! /bin/sh

proj_top_dir=`dirname "$0"`
cd "${proj_top_dir}"

. ./shell_subr.sh


set_multi_compile_opt


make_clean

make_option="${make_option} BUILD_JDSME_OBJ=yes"

# Compile "jdsme.o".
echo ''
echo 'Compile jdsme.o.'
run_make_dir Project/Image target
log_check

ok_exit

