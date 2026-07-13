#! /bin/sh

#
# Subroutine of build.sh, update.sh and clean.sh scripts.
#


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
error_exit() {
	echo ''
	echo error !
	exit 1
}


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
ok_exit() {
	echo ''
	basename "$0"
	echo complete !
	exit 0
}

### REMOVE_RELEASE BEGIN
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
setenv_error_exit() {
	echo ''
	echo '"setenv.sh" is not created.'
	echo 'Please execute "config.sh" to create "setenv.sh".'
	exit 1
}

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
chip_model_error_exit() {
	echo ''
	echo Chip model is not selected.
	echo 'Please execute "config.sh" to select model.'
	exit 1
}
### REMOVE_RELEASE END

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
set_t_kernel_environ() {
	if [ -z "${BD}" ] ; then
		BD="`pwd`"
		export BD="`dirname ${BD}`"
	fi
	if [ ! -d "${BD}" ] ; then
		echo "Erorr: ${BD}" is not a directory.
		echo 'Check "BD" environment variable.'
		exit 1
	fi
	if [ -z "${PREFIX}" ] ; then
		export PREFIX="/home/mptk/prefix_for_bin"
	fi
	if [ ! -d "${PREFIX}" ] ; then
		echo "Erorr: ${PREFIX}" is not a directory.
		echo 'Check "PREFIX" environment variable.'
		exit 1
	fi
	if [ -z "${GNUarmTop}" ] ; then
		export GNUarmTop="$PREFIX"
	fi
	if [ -z "${GNUarm}" ] ; then
		export GNUarm="${GNUarmTop}/bin/arm-none-eabi-"
	fi
	if [ ! -x "${GNUarm}gcc" ] ; then
		echo Error: "${GNUarm}gcc" not found.
		echo 'Check "PREFIX" and "GNUarm" environment variable.'
		exit 1
	fi
### REMOVE_PGRELIEF BEGIN

	# for PG-Relief
	if [ -z "${GNUarmInclude1}" ] ; then
		export GNUarmInclude1="${GNUarmTop}/arm-none-eabi/include"
	fi
	if [ -z "${GNUarmInclude2}" ] ; then
		export GNUarmInclude2="${GNUarmTop}/lib/gcc/arm-none-eabi/4.9.3/include"
	fi
	if [ -z "${GNUarmInclude3}" ] ; then
		export GNUarmInclude3="${GNUarmTop}/lib/gcc/arm-none-eabi/4.9.3/include-fixed"
	fi
	if [ ! -d "${GNUarmInclude1}" ] ; then
		echo Error: "${GNUarmInclude2}" not found.
	fi
	if [ ! -d "${GNUarmInclude2}" ] ; then
		echo Error: "${GNUarmInclude2}" not found.
	fi
	if [ ! -d "${GNUarmInclude3}" ] ; then
		echo Error: "${GNUarmInclude3}" not found.
	fi
### REMOVE_PGRELIEF END
	if [ -z "${ENABLE_NEON}" ] ; then
		export ENABLE_NEON=yes
	fi
	if [ -z "${USE_NEWLIB}" ] ; then
		export USE_NEWLIB=yes
	fi
}

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

set_t_kernel_environ

if [ -z "${BD}" ] ; then
	echo ''
	echo BD is not defined.
	exit 1
fi

if [ "${GNUarm}" ] ; then
	export TOOLCHAIN=gnu
fi

### REMOVE_RELEASE BEGIN
if [ ! -f ./setenv.sh ] ; then
	/bin/sh ./config.sh
	if [ ! -f ./setenv.sh ] ; then
		setenv_error_exit
	fi
fi
.  ./setenv.sh
if [ -z "${IS_CONFIG}" ] ; then
	chip_model_error_exit
fi

### REMOVE_RELEASE END

IS_CHIP_ES=`cat ./cpu_tkernel/project/"${PROJ_NAME}"_CHIP_ES.ini | head -c 8`
export IS_CHIP_ES

IS_MEMORY_SIZE=`cat ./cpu_tkernel/project/"${PROJ_NAME}"_MEM_SIZE.ini | head -c 9`
export IS_MEMORY_SIZE

IS_USBPTP_MODE="OFF"
export IS_USBPTP_MODE

IS_FAST_BOOT="CO_FAST_BOOT_OFF"
export IS_FAST_BOOT

IS_FS_USED="MDF"
export IS_FS_USED


OPT_MOVIE_LIB=`cat ./cpu_tkernel/project/"${PROJ_NAME}"_AUDIO.opt | sed -n -e 's/.*CO_MOVIE_FF_AUDIO_PATTERN=\([0-9][0-9]*\).*/\1/p' | head -1`

OPT_IVE_QR_LIB=`cat ./cpu_tkernel/project/"${PROJ_NAME}"_IVE_QR_MARKER.opt | sed -n -e 's/.*CO_IVE_QR_MARKER_ON=\([0-9][0-9]*\).*/\1/p' | head -1`
export OPT_IVE_QR_LIB

if [ -z "${make_cmd}" ] ; then
	if which gmake > /dev/null 2>&1; then
		# for "xxxBSD" platform.
		make_cmd=gmake
	else
		make_cmd=make
	fi
fi
if [ -z "${MAKE_SILENT}" ] ; then
	export MAKE_SILENT=yes
fi
if [ "${MAKE_SILENT}" = "yes" ] ; then
	make_option="-s -r OPT_MOVIE_LIB=${OPT_MOVIE_LIB}"
else
	make_option="-r OPT_MOVIE_LIB=${OPT_MOVIE_LIB}"
fi

if [ -n "${MAKE_OPT}" ] ; then
	make_option="${make_option} ${MAKE_OPT}"
fi

make_logfile=log.txt
rm -f "${make_logfile}"
make_logout="tee ${make_logfile}"
make_logout_a="tee -a ${make_logfile}"

if [ ! -x ../etc/platform ] ; then
	chmod a+x ../etc/platform
fi


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


### REMOVE_PGRELIEF BEGIN
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
EPOM_ERROR() {
	echo "pgr5.exe is not found."
	echo "Please set EPOMDIR environment variable."
	error_exit
}

PGR_SETTINGS() {
	if [ -z "${EPOMDIR}" ] ; then
		if [ -d 'C:\Program Files (x86)\PGRelief\PGRelief\Analyze\EPOM' ] ; then
			EPOMDIR='C:\Program Files (x86)\PGRelief\PGRelief\Analyze\EPOM'
			export EPOMDIR
		elif [ -d 'C:\Program Files\PGRelief\PGRelief\Analyze\EPOM' ] ; then
			EPOMDIR='C:\Program Files\PGRelief\PGRelief\Analyze\EPOM'
			export EPOMDIR
		fi
	fi

	if [ ! -f "${EPOMDIR}/pgr5.exe" ] ; then EPOM_ERROR; fi;

	make_option="${make_option} IS_USE_PGRELIEF=y"


	rm -f ./PGRelief_out.csv
	rm -f ./PGRelief_out_ng.csv

	cd ./cpu_tkernel/project
	rm -f ./"${PROJ_NAME}".pgr
	"${make_cmd}" -s -r -f ./Makefile_opt print-compile-option
	cd ../../

	# Search charcter code converter and set commandline.
	if [ -x "`which iconv 2> /dev/null`" ] ; then
		CHARCODE_COMV_CMD='iconv -f SJIS -t UTF8'
	else
		echo `basename $0` 'Warning: Command "iconv" not found.'
		CHARCODE_COMV_CMD=cat
	fi

	export LANG=ja_JP.SJIS
	export LC_CTYPE=${LANG}
}

PGR_POST_PROCESS() {
	rm -rf "${TMP}/PGR_output"
	if [ -f ./PGRelief_out.csv ] ; then
		perl ./PGR_check.pl ./PGRelief_out.csv
		grep -a -v '^"o",' ./PGRelief_out.csv > ./PGRelief_out_ng.csv
		echo `grep -a -c '^"",' ./PGRelief_out_ng.csv` PG-Relief Warning
	fi
	cd ./cpu_tkernel/project
	rm -f ./"${PROJ_NAME}".pgr
	cd ../../
}


### REMOVE_PGRELIEF END
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
make_clean() {
	echo ''
	echo Cleaning ...

	make_target=clean
	"${make_cmd}" ${make_option} "${make_target}" -C if_tkernel
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/basefw
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/board
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/cmd_entry
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/hostfw
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/media_meta
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/ip
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/system
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/share
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/project
}


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
run_make() {
	make_target=$1; shift

	"${make_cmd}" -s ${ENV_MULTI_JOB_OPT} "${make_target}" -C if_tkernel   2>&1 | ${make_logout}
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/basefw    2>&1 | ${make_logout_a}
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/board     2>&1 | ${make_logout_a}
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/cmd_entry 2>&1 | ${make_logout_a}
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/hostfw    2>&1 | ${make_logout_a}
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/media_meta 2>&1 | ${make_logout_a}
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/ip        2>&1 | ${make_logout_a}
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/system    2>&1 | ${make_logout_a}
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/share     2>&1 | ${make_logout_a}
	"${make_cmd}" ${make_option} "${make_target}" -C cpu_tkernel/project   2>&1 | ${make_logout_a}
}


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
make_depends() {
	echo ''
	echo Depending ...

	run_make depends
}


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
make_build() {
	echo ''
	echo Building ...

	"${make_cmd}" ${make_option} -C cpu_tkernel/basefw/fj/system re_create_build_compile_info
	"${make_cmd}" ${make_option} -C cpu_tkernel/basefw/fj/system re_create_update_compile_info
	run_make target
}


make_update() {
	echo ''
	echo 'Building (Only updated files) ...'

	"${make_cmd}" ${make_option} -C cpu_tkernel/basefw/fj/system re_create_build_compile_info_once
	"${make_cmd}" ${make_option} -C cpu_tkernel/basefw/fj/system re_create_update_compile_info
	run_make target
}


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
log_check() {
if [ -f "${make_logfile}" ] ; then
	errcnt=`grep -ic 'error:' "${make_logfile}"`
	warcnt=`grep -ic 'warning:' "${make_logfile}"`
	echo "${errcnt} Error"
	echo "${warcnt} Warning"
fi
}
