DESCRIPTION = "IPCU Application"
SECTION = "ipcu"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"
PR = "r0"

SRC_URI += "file://ipcu"

TARGET_CC_ARCH += "${LDFLAGS}"
INSANE_SKIP_${PN}-dev += "dev-elf"
INSANE_SKIP_${PN}="ldflags"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_PACKAGE_STRIP = "1"

S = "${WORKDIR}/ipcu/"
DEPENDS="alsa-lib"
RDEPENDS_${PN}="alsa-lib libasound"

IPCU_MW_DIR = "/usr/lib/ipcu"
IPCU_MW_INC_DIR="${IPCU_MW_DIR}/include"
IPCU_MW_STM_DIR="${IPCU_MW_DIR}/streaming"

do_install() {
		install -d ${D}${libdir}
		install -m 0755  ${S}/out/usr/lib/libcmfwk.so ${D}${libdir}
		install -m 0755  ${S}/out/usr/lib/libipcustream.so ${D}${libdir}
		install -m 0755  ${S}/out/usr/lib/libsample.so ${D}${libdir}

		install -d ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/okpf.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/mnt_mtp_nand_usb20.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/usb20dev_mtp_par0.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/mnt_mtp_nand.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/hid.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/uvc_hid.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/mtp_hid.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/mtp_restart_hid.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/apmode ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/smartcmd ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/test_ipcu ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/ipcu.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/camera_if_direct ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/camera_if ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/camera_Info_cmd ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/camera_Ev_rec ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/camifts ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/zw ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/netseclpb ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/streamd ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/set_streamd ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/ipcuvc ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/usb20dev_uvc_ipcu.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/uvc_ipcu.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/uav_ipcu.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/fwupdate.sh ${D}${bindir}
		
		install -m 0755  ${S}/out/usr/bin/uvc_ipcu_phy.sh ${D}${bindir}
                install -m 0755  ${S}/out/usr/bin/ipcuvc-m ${D}${bindir}
                install -m 0755  ${S}/out/usr/bin/uvc_ipcu-m.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/ipcuvc15-ift ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/uvc15_ipcu-ift.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/usb30_fake_msc.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/ipcuac ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/uac_ipcu.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/uac_stop.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/uvc_mjpeg.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/uvc_yuv.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/uvc_stop.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/usb_wifi.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/osdcmd ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/osdtimeonoff ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/osdtimechg ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/fb_connector ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/fwupdate ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/fwup_net.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/pmic_com ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/media_format ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/preview ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/single_capture ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/video_capture ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/make_app_file_ether ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/make_app_file_wifi ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/rotary_switch ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/sample_linux_app ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/cui_sample.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/reset.sh ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/live555_ipcu ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/ipcu_playstream_file ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/fwupdate_extra.sh ${D}${bindir}
		install -m 0755  ${S}/live555/testProgs/live555_rtsp_hevc ${D}${bindir}
		install -m 0755  ${S}/live555/testProgs/live555_rtsp_mul_hevc ${D}${bindir}
		install -m 0755  ${S}/live555/testProgs/live555_rtsp_h264 ${D}${bindir}
		install -m 0755  ${S}/live555/testProgs/live555_rtsp_d1 ${D}${bindir}
		install -m 0755  ${S}/live555/testProgs/testOnDemandRTSPServer ${D}${bindir}




		

		mkdir ${D}${bindir}/event
		mkdir ${D}${bindir}/event/00010100
		mkdir ${D}${bindir}/event/00010200
		mkdir ${D}${bindir}/event/00010300
		mkdir ${D}${bindir}/event/00010400
		mkdir ${D}${bindir}/event/00010500
		mkdir ${D}${bindir}/event/00010600
		install -m 0755  ${S}/out/usr/bin/event/*.sh ${D}${bindir}/event/
		install -m 0755  ${S}/out/usr/bin/event/00010100/*.sh ${D}${bindir}/event/00010100
		install -m 0755  ${S}/out/usr/bin/event/00010200/*.sh ${D}${bindir}/event/00010200
		install -m 0755  ${S}/out/usr/bin/event/00010300/*.sh ${D}${bindir}/event/00010300
		install -m 0755  ${S}/out/usr/bin/event/00010400/*.sh ${D}${bindir}/event/00010400
		install -m 0755  ${S}/out/usr/bin/event/00010500/*.sh ${D}${bindir}/event/00010500
		install -m 0755  ${S}/out/usr/bin/event/00010600/*.sh ${D}${bindir}/event/00010600
}

SYSROOT_PREPROCESS_FUNCS += "ipcumw_sysroot_preprocess"

ipcumw_sysroot_preprocess () {
	install -d ${STAGING_DIR_HOST}${IPCU_MW_DIR}
	install -m 0755 ${S}/out/usr/lib/*.so ${STAGING_DIR_HOST}${IPCU_MW_DIR}
	install -d ${STAGING_DIR_HOST}${IPCU_MW_INC_DIR}
	cp -r ${S}/include/* ${STAGING_DIR_HOST}${IPCU_MW_INC_DIR}
	install -d ${STAGING_DIR_HOST}${IPCU_MW_STM_DIR}
	cp -r ${S}/streaming/*.h ${STAGING_DIR_HOST}${IPCU_MW_STM_DIR}
}

