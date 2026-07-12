DESCRIPTION = "Terminal for isp"
SECTION = "ispterm"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"
PR = "r0"

SRC_URI += "file://ispterm.c \
	"
TARGET_CC_ARCH += "${LDFLAGS}"

S = "${WORKDIR}"

do_compile() {
	${CC} ${WORKDIR}/ispterm.c -o ${WORKDIR}/ispterm -lpthread
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${WORKDIR}/ispterm ${D}${bindir}/
}

