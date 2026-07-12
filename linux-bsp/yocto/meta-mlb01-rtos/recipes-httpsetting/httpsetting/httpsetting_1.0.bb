DESCRIPTION = "http setting"
SECTION = "httpsetting"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"
PR = "r0"

SRC_URI += "file://httpsetting"

S = "${WORKDIR}/httpsetting/"

www_dir = "/var/www"
FILES_${PN} += "${www_dir}"
FILES_${PN}-dbg += "${www_dir}/.debug"

cgi_dir = "/var/www/cgi-bin"
FILES_${PN} += "${cgi_dir}"
FILES_${PN}-dbg += "${cgi_dir}/.debug"

css_dir = "/var/www/css"
FILES_${PN} += "${css_dir}"
FILES_${PN}-dbg += "${css_dir}/.debug"

font_dir = "/var/www/fonts"
FILES_${PN} += "${font_dir}"
FILES_${PN}-dbg += "${font_dir}/.debug"

img_dir = "/var/www/images"
FILES_${PN} += "${img_dir}"
FILES_${PN}-dbg += "${img_dir}/.debug"

js_dir = "/var/www/js"
FILES_${PN} += "${js_dir}"
FILES_${PN}-dbg += "${js_dir}/.debug"

webui_dir = "/var/www/webui"
FILES_${PN} += "${webui_dir}"
FILES_${PN}-dbg += "${webui_dir}/.debug"
webui_dir_cgi = "/var/www/cgi-bin/webui"
FILES_${PN} += "${webui_dir_cgi}"
FILES_${PN}-dbg += "${webui_dir_cgi}/.debug"
webui_dir_css = "/var/www/webui/css"
FILES_${PN} += "${webui_dir_css}"
FILES_${PN}-dbg += "${webui_dir_css}/.debug"
webui_dir_js = "/var/www/webui/js"
FILES_${PN} += "${webui_dir_js}"
FILES_${PN}-dbg += "${webui_dir_js}/.debug"
webui_dir_parameter = "/var/www/webui/parameter"
FILES_${PN} += "${webui_dir_parameter}"
FILES_${PN}-dbg += "${webui_dir_parameter}/.debug"

evrec_dir = "/var/www/evrec"
FILES_${PN} += "${evrec_dir}"
FILES_${PN}-dbg += "${evrec_dir}/.debug"
evrec_dir_cgi = "/var/www/cgi-bin/evrec"
FILES_${PN} += "${evrec_dir_cgi}"
FILES_${PN}-dbg += "${evrec_dir_cgi}/.debug"
evrec_dir_js = "/var/www/evrec/js"
FILES_${PN} += "${evrec_dir_js}"
FILES_${PN}-dbg += "${evrec_dir_js}/.debug"
evrec_dir_css = "/var/www/evrec/css"
FILES_${PN} += "${evrec_dir_css}"
FILES_${PN}-dbg += "${evrec_dir_css}/.debug"


do_install() {
	install -d ${D}${www_dir}
#	install -m 0664 ${S}/www/index.html ${D}${www_dir}/
#	install -m 0664 ${S}/www/*.sh ${D}${www_dir}/
#	install -d ${D}${cgi_dir}
#	install -m 0755 ${S}/www/cgi-bin/*.cgi ${D}${cgi_dir}/
#	install -d ${D}${css_dir}
#	install -m 0664 ${S}/www/css/*.css ${D}${css_dir}/
#	install -d ${D}${font_dir}
#	install -m 0664 ${S}/www/fonts/glyphicons-halflings-regular.* ${D}${font_dir}/
#	install -d ${D}${img_dir}
#	install -m 0664 ${S}/www/images/*.jpg ${D}${img_dir}/
#	install -m 0664 ${S}/www/images/*.gif ${D}${img_dir}/
#	install -d ${D}${js_dir}
#	install -m 0664 ${S}/www/js/*.js ${D}${js_dir}/

	install -d ${D}${sysconfdir}
	install -m 0544 ${S}etc/httpd2.conf ${D}${sysconfdir}/
	install -m 0755 ${S}etc/udhcpd.conf ${D}${sysconfdir}/
	install -m 0755 ${S}etc/udhcpd_wlan.conf ${D}${sysconfdir}/
	install -m 0755 ${S}etc/udhcpd_eth.conf ${D}${sysconfdir}/

#	install -d ${D}${webui_dir}
#	install -m 0664 ${S}/www/webui/*.html ${D}${webui_dir}/
#	install -m 0664 ${S}/www/webui/*.cab ${D}${webui_dir}/
#	install -d ${D}${webui_dir_cgi}
#	install -m 0755 ${S}/www/cgi-bin/webui/*.cgi ${D}${webui_dir_cgi}/
#	install -d ${D}${webui_dir_cgi}/01_videoencode
#	install -d ${D}${webui_dir_cgi}/02_videocontrol
#	install -d ${D}${webui_dir_cgi}/03_privacymask
#	install -d ${D}${webui_dir_cgi}/04_onscreendisplay
#	install -d ${D}${webui_dir_cgi}/08_audioencode
#	install -d ${D}${webui_dir_cgi}/09_od
#	install -d ${D}${webui_dir_cgi}/11_fd
#	install -d ${D}${webui_dir_cgi}/12_pd
#	install -d ${D}${webui_dir_cgi}/10_ta
#	install -d ${D}${webui_dir_cgi}/99_m10v
#	install -d ${D}${webui_dir_cgi}/AA_hard_management
#	install -m 0755 ${S}/www/cgi-bin/webui/01_videoencode/*.cgi ${D}${webui_dir_cgi}/01_videoencode/ 
#	install -m 0755 ${S}/www/cgi-bin/webui/02_videocontrol/*.cgi ${D}${webui_dir_cgi}/02_videocontrol/
#	install -m 0755 ${S}/www/cgi-bin/webui/03_privacymask/*.cgi ${D}${webui_dir_cgi}/03_privacymask/
#	install -m 0755 ${S}/www/cgi-bin/webui/04_onscreendisplay/*.cgi ${D}${webui_dir_cgi}/04_onscreendisplay/
#	install -m 0755 ${S}/www/cgi-bin/webui/08_audioencode/*.cgi ${D}${webui_dir_cgi}/08_audioencode/
#	install -m 0755 ${S}/www/cgi-bin/webui/09_od/*.cgi ${D}${webui_dir_cgi}/09_od/
#	install -m 0755 ${S}/www/cgi-bin/webui/11_fd/*.cgi ${D}${webui_dir_cgi}/11_fd/
#	install -m 0755 ${S}/www/cgi-bin/webui/12_pd/*.cgi ${D}${webui_dir_cgi}/12_pd/
#	install -m 0755 ${S}/www/cgi-bin/webui/10_ta/*.cgi ${D}${webui_dir_cgi}/10_ta/
#	install -m 0755 ${S}/www/cgi-bin/webui/99_m10v/*.cgi ${D}${webui_dir_cgi}/99_m10v/
#	install -m 0755 ${S}/www/cgi-bin/webui/AA_hard_management/*.cgi ${D}${webui_dir_cgi}/AA_hard_management/
#	install -d ${D}${webui_dir_css}
#	install -m 0664 ${S}/www/webui/css/*.css ${D}${webui_dir_css}/
#	install -d ${D}${webui_dir_js}
#	install -m 0664 ${S}/www/webui/js/*.js ${D}${webui_dir_js}/
#	install -d ${D}${webui_dir_parameter}
#	install -m 0664 ${S}/www/webui/parameter/*.html ${D}${webui_dir_parameter}/

#	install -d ${D}${evrec_dir}
#	install -m 0664 ${S}/www/evrec/*.html ${D}${evrec_dir}/
#	install -d ${D}${evrec_dir_cgi}
#	install -m 0755 ${S}/www/cgi-bin/evrec/*.cgi ${D}${evrec_dir_cgi}/
#	install -d ${D}${evrec_dir_js}
#	install -m 0664 ${S}/www/evrec/js/*.js ${D}${evrec_dir_js}/
#	install -d ${D}${evrec_dir_css}
#	install -m 0664 ${S}/www/evrec/css/*.css ${D}${evrec_dir_css}/

}
