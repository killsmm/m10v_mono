SUMMARY = "check app file Application"
SECTION = "chk_app_file"
#LICENSE = "GPLv2"
LICENSE = "Proprietary"
#LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

SRC_URI += "file://chkappfile.sh"

TARGET_CC_ARCH += "${LDFLAGS}"

S = "${WORKDIR}"

bin_dir = "/usr/bin"
FILES_${PN} += "${bin_dir}/chkappfile.sh"


do_install() {
	install -d ${D}${bin_dir}
	install -m 0755 ${WORKDIR}/chkappfile.sh ${D}${bin_dir}/
}

