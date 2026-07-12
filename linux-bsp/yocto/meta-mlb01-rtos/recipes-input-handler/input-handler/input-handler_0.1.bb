SUMMARY = "input handler application"
SECTION = "input handler"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

SRC_URI = "file://input-handler.c"

TARGET_CC_ARCH += "${LDFLAGS}"

S = "${WORKDIR}"

do_compile() {
	     ${CC} input-handler.c -o input-handler
}

do_install() {
	     install -d ${D}${bindir}
	     install -m 0755 input-handler ${D}${bindir}
}
