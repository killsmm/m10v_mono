DESCRIPTION = "USB Host Compliance Support Script"
SECTION = "usbhostcomp"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"
PR = "r0"

SRC_URI += "file://usbhostcomp"

S = "${WORKDIR}/usbhostcomp"

home_dir = "/home/root/usb_hostcomp"

FILES_${PN} += "${home_dir}"
FILES_${PN}-dbg += "${home_dir}/.debug"

do_install() {
	install -d ${D}${home_dir}
	install -m 0755 ${S}/cn9hdc_host.sh ${D}${home_dir}/
	install -m 0755 ${S}/cn10hdc_host.sh ${D}${home_dir}/
	install -m 0755 ${S}/usbhdc_host.sh ${D}${home_dir}/
	install -m 0755 ${S}/README ${D}${home_dir}/

}

