#!/bin/bash


#COMPILER_DOCKER_IMAGE="m10v_yocto_compiler:1.0"
COMPILER_DOCKER_IMAGE="m10v-compiler:latest"
BB_ENV_EXTRAWHITE="BUILD_VERSION BUILD_DATE"
BUILD_DATE="$(date)"
BUILD_VERSION_MANUAL="1.0.0"

WORK_DIR=$(dirname $(readlink -f $0))

print_usage(){
	echo -e "$0 <options>"
	echo -e "options :"
	echo -e " -i : ignore unstaged files (for development use only)"
	echo -e " -c [manifest file] : specify the version manifest file"
	echo -e " -m manual build"
}

while getopts "mc:ihv:" arg; do
    case $arg in
        h)
            print_usage
            exit 1
            ;;
		m)
			MANUAL_BUILD=1
			echo "manual"
			;;
        i)
            IGNORE_UNSTATGED_FILES=1
            YOCTO_GIT_VERSION="unstaged"
            ;;
        c)
            MANIFEST_FILE=$OPTARG
            echo "manifest file : $MANIFEST_FILE"
            ;;
        v)
            BUILD_VERSION_MANUAL=$OPTARG
            ;;
        *)
            print_usage
            exit 1
            ;;
    esac
done

BUILD_VERSION="${BUILD_VERSION_MANUAL}-$(git rev-parse HEAD)"

if [ -n "$MANUAL_BUILD" ];then
	docker run -it --rm -v "${WORK_DIR}":/home/m10v/M10v/LinuxRelease_mlb01_20200720 -v /home/ft/.ssh:/home/m10v/.ssh \
        -u m10v -w /home/m10v/M10v/LinuxRelease_mlb01_20200720 ${COMPILER_DOCKER_IMAGE} /bin/bash
	exit 0
fi

docker run -it --rm -v "${WORK_DIR}":/home/m10v/M10v/LinuxRelease_mlb01_20200720 -v /home/ft/.ssh:/home/m10v/.ssh \
        -u m10v -w /home/m10v/M10v/LinuxRelease_mlb01_20200720 ${COMPILER_DOCKER_IMAGE} \
        bash -c "export BB_ENV_EXTRAWHITE=\"${BB_ENV_EXTRAWHITE}\" && export BUILD_VERSION=\"${BUILD_VERSION}\" && export BUILD_DATE=\"${BUILD_DATE}\" && ./make.sh 2g2c"
