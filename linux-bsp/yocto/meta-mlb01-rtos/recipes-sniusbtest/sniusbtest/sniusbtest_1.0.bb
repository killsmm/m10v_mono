DESCRIPTION = "USB Test Shell"
SECTION = "sniusbtest"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"
PR = "r0"

SRC_URI += "file://cdc_func.sh \
	file://cdc_stop.sh \
	file://com_mtp_cdc_func.sh \
	file://msc_stop.sh \
	file://msc_func.sh \
	file://msc_upper_func.sh \
	file://mtp_func.sh \
	file://mtp_stop.sh \
	file://usb30dev_msc_func.sh \
	file://usb30dev_msc_upper_func.sh \
	file://usb30dev_mtp_func.sh \
	file://usb30dev_cdc_func.sh \
	file://insert_mtp.sh \
	file://eject_usb.sh \
	file://msc_connect_upper.sh \
	file://msc_SD_switch.sh \
	file://10-usb.rules \
	file://usb20dev_uvc_func.sh \
	file://usb30dev_uvc_func.sh \
	file://usb20dev_uvc_func_264.sh \
	file://h264 \
	"

home_dir = "/home/root/usb_test"
rules_dir = "/etc/udev/rules.d"

FILES_${PN} += "${home_dir}"
FILES_${PN} += "${rules_dir}"
FILES_${PN}-dbg += "${home_dir}/.debug"
FILES_${PN}-dbg += "${rules_dir}/.debug"

do_install() {
	install -d ${D}${home_dir}
	install -m 0755 ${WORKDIR}/cdc_func.sh ${D}${home_dir}/
	install -m 0755 ${WORKDIR}/cdc_stop.sh ${D}${home_dir}/
	install -m 0755 ${WORKDIR}/com_mtp_cdc_func.sh ${D}${home_dir}/
	install -m 0755 ${WORKDIR}/msc_stop.sh ${D}${home_dir}/
	install -m 0755 ${WORKDIR}/msc_func.sh ${D}${home_dir}/
	install -m 0755 ${WORKDIR}/msc_upper_func.sh ${D}${home_dir}/
	install -m 0755 ${WORKDIR}/mtp_func.sh ${D}${home_dir}/
	install -m 0755 ${WORKDIR}/mtp_stop.sh ${D}${home_dir}/
	install -m 0755 ${WORKDIR}/usb30dev_msc_func.sh ${D}${home_dir}/
	install -m 0755 ${WORKDIR}/usb30dev_msc_upper_func.sh ${D}${home_dir}/
	install -m 0755 ${WORKDIR}/usb30dev_mtp_func.sh ${D}${home_dir}/
	install -m 0755 ${WORKDIR}/usb30dev_cdc_func.sh ${D}${home_dir}/
	install -m 0755 ${WORKDIR}/usb20dev_uvc_func.sh ${D}${home_dir}/
	install -m 0755 ${WORKDIR}/usb30dev_uvc_func.sh ${D}${home_dir}/
	install -m 0755 ${WORKDIR}/usb20dev_uvc_func_264.sh ${D}${home_dir}/
	install -m 0755 ${WORKDIR}/h264 ${D}${home_dir}/

	install -d ${D}${bindir}
	install -m 0755 ${WORKDIR}/insert_mtp.sh ${D}${bindir}
	install -m 0755 ${WORKDIR}/eject_usb.sh ${D}${bindir}
	install -m 0755 ${WORKDIR}/msc_SD_switch.sh ${D}${bindir}
	install -m 0755 ${WORKDIR}/msc_connect_upper.sh ${D}${bindir}

	install -d ${D}${rules_dir}
	install -m 0755 ${WORKDIR}/10-usb.rules ${D}${rules_dir}/
}

