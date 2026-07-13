#! /bin/sh

proj_top_dir=`dirname "$0"`
cd "${proj_top_dir}"

PROJ_ROOT_DIR=../../


UPDATE_TIMESTAMP() {
	s_compile_opt="$1"

	tmpfile=update_timestamp_tmp.txt

	while [ -n "${s_compile_opt}" ] ; do
		one_opt="`echo ${s_compile_opt} | sed -e 's/^\([^ ][^ ]*\).*$/\1/'`"
		s_compile_opt="`echo ${s_compile_opt} | sed -e 's/^[^ ][^ ]* *\(.*\)$/\1/'`"

		echo Update timestamp '"'${one_opt}'"':
		grep -r -l --binary-files=without-match \
				--include=\*.\[chs\] --include=\*.inc \
				$one_opt \
				"${PROJ_ROOT_DIR}" \
			| tee $tmpfile
		if [ -s $tmpfile ] ; then
			cat $tmpfile | xargs touch
		fi
		echo ''
	done
	rm -f $tmpfile
}



INPUT_COMPILE_OPT() {
	S_COMPILE_OPT=
	echo "Input compile options > "
	read S_COMPILE_OPT

	echo ''
	UPDATE_TIMESTAMP "$S_COMPILE_OPT"
}


if [ "$1" != "" ] ; then
	for i in $*; do 
		UPDATE_TIMESTAMP "$i"
	done
else
	INPUT_COMPILE_OPT
fi

