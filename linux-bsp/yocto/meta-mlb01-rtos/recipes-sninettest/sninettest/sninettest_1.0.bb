DESCRIPTION = "Net Test Sample"
SECTION = "sninettest"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"
PR = "r0"

SRC_URI += "file://sninettest"

TARGET_CC_ARCH += "${LDFLAGS}"

S = "${WORKDIR}/sninettest"

root_dir = "/root"
FILES_${PN} += "${root_dir}"
FILES_${PN}-dbg += "${root_dir}/.debug"

do_install() {
	install -d ${D}${root_dir}
	install -d ${D}${bindir}
	
	install -m 0755 ${S}/net_init ${D}${bindir}/
	install -m 0755 ${S}/stream_test ${D}${bindir}/
	install -m 0755 ${S}/exp_test ${D}${bindir}/

	install -m 0644 ${S}/og_hm_core.bin ${D}${root_dir}/
	install -m 0644 ${S}/og_mh_core.bin ${D}${root_dir}/
	install -m 0644 ${S}/og_pk_core.bin ${D}${root_dir}/
}
