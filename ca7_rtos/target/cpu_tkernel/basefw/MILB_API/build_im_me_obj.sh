#! /bin/sh

proj_top_dir=`dirname "$0"`
cd "${proj_top_dir}"

. ./shell_subr.sh


set_multi_compile_opt


make_clean

make_option="${make_option} BUILD_IM_ME_OBJ=yes"

# Compile "im_me.o".
echo ''
echo 'Compile im_me.o.'
run_make_dir Project/ImageMacro target
log_check

# Compile "ct_im_me.o".
echo ''
echo 'Compile ct_im_me.o.'
run_make_dir Project/ComponentTest target
log_check

ok_exit

