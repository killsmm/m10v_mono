DESCRIPTION = "UVC Test Sample"
SECTION = "sniuvctest"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"
PR = "r0"

SRC_URI += "file://sniuvctest"

TARGET_CC_ARCH += "${LDFLAGS}"

S = "${WORKDIR}/sniuvctest"

do_install() {
	install -d ${D}${bindir}
	
	install -m 0755 ${S}/uvc_host_test/uvc_host_test ${D}${bindir}
	install -m 0755 ${S}/uvc-gadget/uvc-gadget ${D}${bindir}
	install -m 0755 ${S}/uvc-gadget/uvc-gadget264 ${D}${bindir}
}
