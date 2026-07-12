DESCRIPTION = "Driver test Application"
SECTION = "drvtest"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"
PR = "r0"

TARGET_CC_ARCH += "${LDFLAGS}" 

SRC_URI += "file://drvtest"

S = "${WORKDIR}/drvtest"

do_install() {
		install -d ${D}${bindir}
		install -m 0755  ${S}/test_fs/test_fs ${D}${bindir}
		install -m 0755  ${S}/test_time/test_time ${D}${bindir}
		install -m 0755  ${S}/test_freerun_timer/test_freerun_timer ${D}${bindir}
		install -m 0755  ${S}/test_spidev/test_spidev ${D}${bindir}
		install -m 0755  ${S}/test_ipcufs/test_ipcufs ${D}${bindir}
		install -m 0755  ${S}/test_fb/dtest ${D}${bindir}
}
