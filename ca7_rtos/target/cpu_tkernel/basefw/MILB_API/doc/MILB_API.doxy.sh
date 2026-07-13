#! /bin/sh

proj_top_dir=`dirname "$0"`
cd "${proj_top_dir}"

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
EXIT() {
	echo ''
	exit 0
}

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
SET_VAR() {
	if [ -z "${MILB_MODEL_NAME}" ] ; then
		MILB_MODEL_NAME=undefined_name
		export MILB_MODEL_NAME
	fi
	if [ -z "${MILB_MODEL_NUMBER}" ] ; then
		MILB_MODEL_NUMBER=undefined_model
		export MILB_MODEL_NUMBER
	fi
	LOG_FILE=./log.txt
	rm -f "${LOG_FILE}"
}

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
LOG_CHECK() {
	if [ -f "${LOG_FILE}" ] ; then
		errcnt=`grep -c 'error:' "${LOG_FILE}"`
		warcnt=`grep -c 'warning' "${LOG_FILE}"`
		echo "${errcnt} Error"
		echo "${warcnt} Warning"

		if [ "${errcnt}" -ne 0 ] ; then
			exit 1
		fi
		if [ "${warcnt}" -ne 0 ] ; then
			exit 1
		fi
	fi
}

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
REMOVE_LOG() {
	if [ "$1" = "TEST" ] ; then
		return
	fi

	rm -f "${LOG_FILE}"
}

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
GENERATE_HTMLHELP() {
	echo "*** HTML Help has started"
	rm -f ./MILB_API.chm
	if [ -f "${HCC1_PATH}"/hhc.exe ] ; then
		"${HCC1_PATH}"/hhc.exe ./MILB_API_Specification/index.hhp | tee -a ${LOG_FILE}
	fi
	if [ -f "${HCC2_PATH}"/hhc.exe ] ; then
		"${HCC2_PATH}"/hhc.exe ./MILB_API_Specification/index.hhp | tee -a ${LOG_FILE}
	fi
	rm -rf ./MILB_API_Specification
	echo "*** HTML Help has finished"
}

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
GENERATE_DOC() {
	echo "*** Doxygen has started"
	# Remove __attribute__() syntax and backup original file.
	# target file path array relative project root directory.
	as_target_path=( \
		include/ddim_user_custom.h \
		)
	rm -rf doxy_back
	mkdir doxy_back
	for s_file_path in ${as_target_path[@]}; do
		s_file_path="../${s_file_path}"
		echo Remove comments "${s_file_path}"
		cp -p "${s_file_path}" "doxy_back/`basename ${s_file_path}`.orig"
		perl ../Project/ReleaseTool.pl remove_comment "${s_file_path}" REMOVE_EXCLUDE_DOXYGEN remove
	done

	# Run doxygen.
	rm -rf ./MILB_API_Specification
	if [ -f "${DOXYGEN1_PATH}"/doxygen.exe ] ; then
		"${DOXYGEN1_PATH}"/doxygen.exe ./MILB_API.doxy 2>&1 | tee ${LOG_FILE}
	else
		if [ -f "${DOXYGEN2_PATH}"/doxygen.exe ] ; then
			"${DOXYGEN2_PATH}"/doxygen.exe ./MILB_API.doxy 2>&1 | tee ${LOG_FILE}
		fi
	fi
	echo "*** Doxygen has finished"

	# Restore bakcup files and remove doxygen comments.
	for s_file_path in ${as_target_path[@]}; do
		s_file_path="../${s_file_path}"
		echo Restore "doxy_back/`basename ${s_file_path}`.orig" '->' "${s_file_path}"
		cp -p "doxy_back/`basename ${s_file_path}`.orig" "${s_file_path}"
	done
	rm -rf doxy_back

	LOG_CHECK

	if [ -f "${HCC1_PATH}"/hhc.exe ] ; then
		GENERATE_HTMLHELP
	fi
	if [ -f "${HCC2_PATH}"/hhc.exe ] ; then
		GENERATE_HTMLHELP
	fi
}

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
DOXYGEN1_PATH=`cygpath --unix "C:\Program Files\doxygen\bin"`
DOXYGEN2_PATH=`cygpath --unix "C:\Program Files (x86)\doxygen\bin"`
HCC1_PATH=`cygpath --unix "C:\Program Files\HTML Help Workshop"`
HCC2_PATH=`cygpath --unix "C:\Program Files (x86)\HTML Help Workshop"`
SET_VAR
GENERATE_DOC
REMOVE_LOG
EXIT

