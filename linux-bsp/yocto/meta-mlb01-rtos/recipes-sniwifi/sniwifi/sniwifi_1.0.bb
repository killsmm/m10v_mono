SUMMARY = "SNI wifi driver for SNI-RK wifi board sd8887"
SECTION = "sniwifi"
LICENSE = "Proprietary"
#LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"
PR = "r0"

SRC_URI += " \
	file://FwImage/ \
	file://bin_sd8xxx/ \
	"

S = "${WORKDIR}/"
home_dir = "/home/ssc"
FILES_${PN} += "${home_dir}"
FILES_${PN}-dbg += "${home_dir}/.debug"
INSANE_SKIP_${PN} = "ldflags"
#INSANE_SKIP_${PN}-dev = "ldflags"
INSANE_SKIP_${PN} += "already-stripped"

lib_firm_dir = "/lib/firmware/mrvl"
FILES_${PN} += "${lib_firm_dir}"
FILES_${PN}-dbg += "${lib_firm_dir}/.debug"

lib_firm_dir_bcm = "/lib/firmware/bcm"
FILES_${PN} += "${lib_firm_dir_bcm}"

do_install() {
	install -d ${D}${home_dir}
	install -d ${D}${home_dir}/bin_sd8xxx
	install -m 0664 ${S}/bin_sd8xxx/bt8xxx.ko ${D}${home_dir}/bin_sd8xxx
	install -m 0755 ${S}/bin_sd8xxx/iperf ${D}${home_dir}/bin_sd8xxx
	install -m 0755 ${S}/bin_sd8xxx/libselinux.so.1 ${D}${home_dir}/bin_sd8xxx
	install -m 0664 ${S}/bin_sd8xxx/mlan.ko ${D}${home_dir}/bin_sd8xxx
	install -m 0664 ${S}/bin_sd8xxx/sd8xxx.ko ${D}${home_dir}/bin_sd8xxx
	install -m 0755 ${S}/bin_sd8xxx/uaputl.exe ${D}${home_dir}/bin_sd8xxx
	install -d ${D}${lib_firm_dir}
	install -m 0755 ${S}/FwImage/sd8887_bt_a2.bin ${D}${lib_firm_dir}
	install -m 0755 ${S}/FwImage/sd8887_uapsta_a2.bin ${D}${lib_firm_dir}
	install -d ${D}${lib_firm_dir_bcm}
	install -m 0755 ${S}/FwImage/fw_bcm43456c5_ag.bin ${D}${lib_firm_dir_bcm}
	install -m 0755 ${S}/FwImage/fw_bcm43456c5_ag_apsta.bin ${D}${lib_firm_dir_bcm}
	install -m 0755 ${S}/FwImage/nvram_ap6256.txt ${D}${lib_firm_dir_bcm}
}

