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

inherit externalsrc

EXTERNALSRC = "${M10V_MONOREPO_ROOT}/app/m10v_multimedia_server"
PV = "1.0"


# NOTE: the following library dependencies are unknown, ignoring: libgroupsock libUsageEnvironment libBasicUsageEnvironment libliveMedia
#       (this is based on recipes that have previously been built and packaged)
inherit cmake

# Specify any options you want to pass to cmake using EXTRA_OECMAKE:
EXTRA_OECMAKE = ""

# do_install(){
#      install -d ${D}${bindir}
#      install -m 0755 ${WORKDIR}/m10v-multimedia-server-${PV}/usr/bin/video_recorder ${D}/{bindir}
# }

do_compile_prepend(){
    bbwarn "PV = ${PV}"
}

FILES_${PN} += "${bindir}"

DEPENDS += "cppzmq ffmpeg json-c libexif ipcu live555"
