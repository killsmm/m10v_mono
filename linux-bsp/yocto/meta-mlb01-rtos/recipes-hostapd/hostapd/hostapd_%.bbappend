HOMEPAGE = "http://hostap.epitest.fi"
SECTION = "kernel/userland"
LICENSE = "GPLv2 | BSD"
LIC_FILES_CHKSUM = "file://README;md5=4d53178f44d4b38418a4fa8de365e11c"
DEPENDS = "libnl openssl"
SUMMARY = "User space daemon for extended IEEE 802.11 management"

PV="2.5"
FILESEXTRAPATHS_prepend := "${THISDIR}/hostapd-${PV}:"

inherit update-rc.d systemd
INITSCRIPT_NAME = "hostapd"

SYSTEMD_SERVICE_${PN} = "hostapd.service"
SYSTEMD_AUTO_ENABLE_${PN} = "disable"

DEFAULT_PREFERENCE = "-1"

SRC_URI = " \
    http://hostap.epitest.fi/releases/hostapd-${PV}.tar.gz \
    file://defconfig \
    file://init \
    file://hostapd.service \
    file://hostapd.conf \
    file://hostapd_azureware_5g.conf \
    file://hostapd_azureware \
    file://hostapd_eth.conf \
    file://hostapd_wlan.conf \
    file://hostapd_wlan5g.conf \
    file://hostapd_wlan5g_dfs.conf \
    file://hostapd_ap6265.conf \
"

S = "${WORKDIR}/hostapd-${PV}/hostapd"


do_configure() {
    install -m 0644 ${WORKDIR}/defconfig ${S}/.config
	cp ${WORKDIR}/hostapd.conf ./
}

do_compile() {
    export CFLAGS="-MMD -O2 -Wall -g -I${STAGING_INCDIR}/libnl3"
    make
}

do_install() {
    install -d ${D}${sbindir} ${D}${sysconfdir}/init.d ${D}${systemd_unitdir}/system/
    install -m 0644 ${S}/hostapd.conf ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/hostapd_azureware_5g.conf ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/hostapd_eth.conf ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/hostapd_wlan.conf ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/hostapd_wlan5g.conf ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/hostapd_wlan5g_dfs.conf ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/hostapd_ap6265.conf ${D}${sysconfdir}
    install -m 0755 ${S}/hostapd ${D}${sbindir}
    install -m 0755 ${WORKDIR}/hostapd_azureware ${D}${sbindir}
    install -m 0755 ${S}/hostapd_cli ${D}${sbindir}
    install -m 755 ${WORKDIR}/init ${D}${sysconfdir}/init.d/hostapd
    install -m 0644 ${WORKDIR}/hostapd.service ${D}${systemd_unitdir}/system/
    sed -i -e 's,@SBINDIR@,${sbindir},g' -e 's,@SYSCONFDIR@,${sysconfdir},g' ${D}${systemd_unitdir}/system/hostapd.service
}

CONFFILES_${PN} += "${sysconfdir}/hostapd.conf"

SRC_URI[md5sum] = "69f9cec3f76d74f402864a43e4f8624f"
SRC_URI[sha256sum] = "8e272d954dc0d7026c264b79b15389ec2b2c555b32970de39f506b9f463ec74a"
