#! /bin/sh

#
# Subroutine of build.sh, update.sh and clean.sh scripts.
#

if [ -z "${make_cmd}" ] ; then
	make_cmd=make
fi
make_option='-s -r'
#make_option='-r --trace'	# for Makefile debug
### REMOVE_RELEASE BEGIN
if [ -n "${MAKE_OPT}" ] ; then
	make_option="${make_option} ${MAKE_OPT}"
fi
### REMOVE_RELEASE END

make_logfile=log.txt
rm -f "${make_logfile}"
make_logout="tee ${make_logfile}"



set_toolchain_environ() {
	if [ -z "${PREFIX}" ] ; then
		export PREFIX="/home/mptk/prefix_for_bin"
	fi
	if [ -z "${GNUarmTop}" ] ; then
		export GNUarmTop="$PREFIX"
	fi
	if [ -z "${GNUarm}" ] ; then
		export GNUarm="${GNUarmTop}/bin/arm-none-eabi-"
	fi
	if [ ! -x "${GNUarm}gcc" ] ; then
		echo Error: "${GNUarm}gcc" not found.
	fi
	if [ -z "${ENABLE_NEON}" ] ; then
		export ENABLE_NEON=yes
	fi
}

# setting multi compile option.
calc_env_multi_job_num() {
	if [ -z "${NUMBER_OF_PROCESSORS}" ] ; then
		if [ -f /proc/cpuinfo ] ; then
			NUMBER_OF_PROCESSORS=`grep ^processor /proc/cpuinfo              | wc -l | sed -e 's/ //g'`
		elif [ -f /compat/linux/proc/cpuinfo ] ; then
			NUMBER_OF_PROCESSORS=`grep ^processor /compat/linux/proc/cpuinfo | wc -l | sed -e 's/ //g'`
		else
			NUMBER_OF_PROCESSORS=1	# failsafe
		fi
	fi

	ENV_MULTI_JOB_NUM=`expr ${NUMBER_OF_PROCESSORS} + ${NUMBER_OF_PROCESSORS} / 2`

	if [ "${ENV_MULTI_JOB_NUM}" = 0 ] ; then
		ENV_MULTI_JOB_NUM=2	# failsafe
	elif [ -z "${ENV_MULTI_JOB_NUM}" ] ; then
		ENV_MULTI_JOB_NUM=2	# failsafe
	fi
}

set_multi_compile_opt() {
	if [ -z "${ENV_MULTI_JOB_OPT}" ] ; then
		if [ -z "${ENV_MULTI_JOB_NUM}" ] ; then
			calc_env_multi_job_num
		fi
		ENV_MULTI_JOB_OPT="-j${ENV_MULTI_JOB_NUM}"
	fi
	if [ -n "${ENV_MULTI_JOB_OPT}" ] ; then
		make_option="${make_option} MULTI_JOB_OPT=${ENV_MULTI_JOB_OPT}"
	fi
}


error_exit() {
	echo ''
	echo error !
	echo press any key to exit.
	read
	exit 1
}


ok_exit() {
	echo ''
	basename "$0"
	echo complete !
	exit 0
}


run_make() {
	make_target=$1; shift
	"${make_cmd}" ${make_option} "${make_target}" -C Project 2>&1 | ${make_logout}
	if [ $? != 0 ] ; then
		error_exit
	fi
}


make_clean() {
	echo ''
	echo Cleaning ...

	make_target=clean
	"${make_cmd}" ${make_option} "${make_target}" -C Project
	if [ $? != 0 ] ; then
		error_exit
	fi
}


make_update() {
	echo ''
	echo 'Building (Only updated files) ...'

	run_make target
}


make_build() {
	echo ''
	echo Building ...

	run_make depends
	run_make target
}

set_toolchain_environ

