SUMMARY = "System Init Application"
SECTION = "survstart"
#LICENSE = "GPLv2"
LICENSE = "Proprietary"
#LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

SRC_URI += " \
	file://S30NetworkSetting \
	"

SRC_URI[md5sum] = "dd7e89898e155de779740ae1c41eded8"
SRC_URI[sha256sum] = "0ad067059e5468638618802599bc95d4739f9902e07374de128188ee4031e813"

TARGET_CC_ARCH += "${LDFLAGS}"

S = "${WORKDIR}"

initd_dir = "/etc/init.d"
FILES_${PN} += "${initd_dir}"
FILES_${PN}-dbg += "${initd_dir}/.debug"


do_install() {
	install -d ${D}${initd_dir}
	install -m 0755 ${WORKDIR}/S30NetworkSetting ${D}${initd_dir}/
}

