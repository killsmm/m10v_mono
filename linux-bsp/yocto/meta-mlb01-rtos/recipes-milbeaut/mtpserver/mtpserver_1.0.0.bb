DESCRIPTION = "mtpserver"
SECTION = "server"
LICENSE = "Apache-1.0"
PR = "r0"

DEPENDS = "boost glog"

SRC_URI = "git://git.linaro.org/landing-teams/working/fujitsu/mtpserver.git;protocol=https"
#SRC_URI[md5sum] = "cfeb7ad73551e4a72b41f0c1faa73676"
#SRC_URI[sha256sum] = "aa7be86bd91e739f80b4f07595636b96e973b93f7f168e5a01ef2d1f713cfcc2"
SRC_URI += "file://mtpserver.patch;patchdir=${WORKDIR}/git"
SRC_URI += "file://server.patch;patchdir=${WORKDIR}/git"
SRCREV_pn-mtpserver ?= "${AUTOREV}"
LIC_FILES_CHKSUM = "file://NOTICE;md5=9645f39e9db895a4aa6e02cb57294595"
S = "${WORKDIR}/git"

inherit pkgconfig cmake

do_install() {
    install -d ${D}${bindir}
    install -m 0755 server/mtp-server ${D}${bindir}
    install -d ${D}${libdir}
    install -m 0755 libmtpserver.so.1.0.0 ${D}${libdir}
}
