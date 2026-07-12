# Recipe created by recipetool
# This is the basis of a recipe and may need further editing in order to be fully functional.
# (Feel free to remove these comments when editing.)

# Unable to find any files that looked like license statements. Check the accompanying
# documentation and source headers and set LICENSE and LIC_FILES_CHKSUM accordingly.
#
# NOTE: LICENSE is being set to "CLOSED" to allow you to at least start building - if
# this is not accurate with respect to the licensing of the software being built (it
# will not be in most cases) you must specify the correct value before using this
# recipe for anything other than initial testing/development!
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

SRC_URI = "git://git@github.com/killsmm/M10V-CA7-RTOS.git;protocol=ssh;branch=main"

# Modify these as desired
PV = "1.0+git${SRCPV}"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"


# NOTE: the following library dependencies are unknown, ignoring: libgroupsock libUsageEnvironment libBasicUsageEnvironment libliveMedia
#       (this is based on recipes that have previously been built and packaged)
# inherit make

# Specify any options you want to pass to cmake using EXTRA_OECMAKE:
# EXTRA_OECMAKE = ""

# do_install(){
#      install -d ${D}${bindir}
#      install -m 0755 ${WORKDIR}/m10v-multimedia-server-${PV}/usr/bin/video_recorder ${D}/{bindir}
# }

do_prepare(){
    wget https://launchpad.net/gcc-arm-embedded/4.9/4.9-2015-q3-update/+download/gcc-arm-none-eabi-4_9-2015q3-20150921-linux.tar.bz2
    mkdir -p ${WORKDIR}/host_toolchain
    tar -xvf gcc-arm-none-eabi-4_9-2015q3-20150921-linux.tar.bz2 -C ${WORKDIR}/host_toolchain
}

do_patch(){

    chmod a+x ${S}/target/*.sh
    sed -i 's|\/home\/mptk\/prefix_for_bin|${WORKDIR}\/host_toolchain\/gcc-arm-none-eabi-4_9-2015q3|g' ${S}/target/shell_common.sh
    version=$(cd ${S} && git rev-parse --short=8 HEAD)
    bbwarn "$version"
    sed -i "s|#define CA7_RTOS_VERSION 0x00000000|#define CA7_RTOS_VERSION 0x$version|g" ${S}/target/cpu_tkernel/hostfw/user/include/category_parameter.h
    echo $version > ${S}/compiled_rtos_githash
}

do_compile(){
    ${S}/target/build.sh
}

do_deploy(){
    cp ${S}/target/cpu_tkernel/project/bin/MILB_SDK.bin ${DEPLOY_DIR_IMAGE} 
}

do_install(){
    install -d ${D}${etcdir}
    install -m 0755 ${S}/compiled_rtos_githash ${D}${etcdir}
}

FILES_${PN} += "compiled_rtos_githash"

addtask deploy after do_compile
addtask prepare before do_fetch 