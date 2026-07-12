SUMMARY = "System Init Application"
SECTION = "snisysinit"
#LICENSE = "GPLv2"
LICENSE = "Proprietary"
#LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

SRC_URI += " \
	file://finit.c \
	file://sd.rules \
	file://storage.sh \
	file://interfaces \
	file://rc.local \
	file://S1BootCompNotify \
	file://S40StartServer \
	file://bootchartd.conf \
	file://wired.config \
	https://github.com/wkennington/linux-firmware/raw/2017-06-26/mrvl/sd8887_uapsta.bin \
	"

SRC_URI[md5sum] = "dd7e89898e155de779740ae1c41eded8"
SRC_URI[sha256sum] = "0ad067059e5468638618802599bc95d4739f9902e07374de128188ee4031e813"

TARGET_CC_ARCH += "${LDFLAGS}"

S = "${WORKDIR}"

rules_dir = "/etc/udev/rules.d"
FILES_${PN} += "${rules_dir}"
FILES_${PN}-dbg += "${rules_dir}/.debug"

storage_dir = "/lib/udev"
FILES_${PN} += "${storage_dir}"
FILES_${PN}-dbg += "${storage_dir}/.debug"

network_dir = "/etc/network"
FILES_${PN} += "${network_dir}"
FILES_${PN}-dbg += "${network_dir}/.debug"

initd_dir = "/etc/init.d"
FILES_${PN} += "${initd_dir}"
FILES_${PN}-dbg += "${initd_dir}/.debug"

mrvl_dir = "/lib/firmware/mrvl"
FILES_${PN} += "${mrvl_dir}"
FILES_${PN}-dbg += "${mrvl_dir}/.debug"


sd_dir = "/run/SD"
FILES_${PN} += "${sd_dir}"
FILES_${PN}-dbg += "${sd_dir}/.debug"

mtp_dir = "/media/root/test_mtp_sd"
FILES_${PN} += "${mtp_dir}"
FILES_${PN}-dbg += "${mtp_dir}/.debug"

conm_dir = "/var/lib/connman"
FILES_${PN} += "${conm_dir}"
FILES_${PN}-dbg += "${conm_dir}/.debug"

do_compile() {
	${CC} ${CFLAGS} ${LDFLAGS} ${WORKDIR}/finit.c -o ${WORKDIR}/finit
}

do_install() {
	install -d ${D}${base_sbindir}
	install -m 0755 ${WORKDIR}/finit ${D}${base_sbindir}/
	install -d ${D}${rules_dir}
	install -m 0755 ${WORKDIR}/sd.rules ${D}${rules_dir}/
	install -d ${D}${storage_dir}
	install -m 0755 ${WORKDIR}/storage.sh ${D}${storage_dir}/
	install -d ${D}${network_dir}
	install -m 0544 ${WORKDIR}/interfaces ${D}${network_dir}/
	install -d ${D}${initd_dir}
	install -m 0755 ${WORKDIR}/rc.local ${D}${initd_dir}/
	install -m 0755 ${WORKDIR}/S1BootCompNotify ${D}${initd_dir}/
	install -m 0755 ${WORKDIR}/S40StartServer ${D}${initd_dir}/
	install -d ${D}${mrvl_dir}
	install -m 0755 ${WORKDIR}/sd8887_uapsta.bin ${D}${mrvl_dir}/
	install -d ${D}${sysconfdir}
	install -m 0755 ${WORKDIR}/bootchartd.conf ${D}${sysconfdir}/
	install -d ${D}${conm_dir}
	install -m 0755 ${WORKDIR}/wired.config ${D}${conm_dir}/

	install -d ${D}${sd_dir}
	install -d ${D}${mtp_dir}
}

