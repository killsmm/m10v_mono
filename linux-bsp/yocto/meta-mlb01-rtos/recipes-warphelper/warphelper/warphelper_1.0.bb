DESCRIPTION = "IPCU Application"
SECTION = "ipcu"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"
PR = "r0"

SRC_URI += "file://warphelper"

TARGET_CC_ARCH += "${LDFLAGS}"

S = "${WORKDIR}/warphelper"

EXTRA_OEMAKE+='LDFLAGS="${LDFLAGS}"'

etc_dir = "/etc"
FILES_${PN} += "${etc_dir}"
FILES_${PN}-dbg += "${etc_dir}/.debug"

INSANE_SKIP_${PN} = "ldflags"

do_install() {
		install -d ${D}${libdir}
		mkdir ${D}${libdir}/warphelper
		cp -p ${S}/out/usr/lib/libwarph* ${D}${libdir}/warphelper

		install -d ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/warpd ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/snapshot ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/snapshot_wrapper_nf_0 ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/snapshot_wrapper_nf_1 ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/snapshot_wrapper_emmc_0 ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/snapshot_wrapper_emmc_1 ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/start_snapshot ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/warph_com ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/warphelper_command ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/save_snapshot ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/warp_notify ${D}${bindir}

		install -d ${D}${etc_dir}
		install -m 0755 ${S}/config/warpd_nf_0.conf ${D}${etc_dir}/
		install -m 0755 ${S}/config/warpd_nf_1.conf ${D}${etc_dir}/
		install -m 0755 ${S}/config/warpd_emmc_0.conf ${D}${etc_dir}/
		install -m 0755 ${S}/config/warpd_emmc_1.conf ${D}${etc_dir}/
}
