DESCRIPTION = "Parameter Manager"
SECTION = "pmgr"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"
PR = "r0"

SRC_URI += "file://pmgr"

TARGET_CC_ARCH += "${LDFLAGS}"
INSANE_SKIP_${PN}-dev += "dev-elf"

S = "${WORKDIR}/pmgr/"
DEPENDS = "ipcu"
RDEPENDS_${PN}="ipcu"

EXTRA_OEMAKE+="STAGING_INCDIR=${STAGING_INCDIR}"
EXTRA_OEMAKE+="STAGING_LIBDIR=${STAGING_LIBDIR}"
EXTRA_OEMAKE+="PMGR_VERSION=${PV}-${PR}"

do_install() {
		install -d ${D}${bindir}
		install -d ${D}${sysconfdir}/pmgr
		install -d ${D}${sysconfdir}/init.d
		install -m 0755  ${S}/out/usr/bin/pmgr ${D}${bindir}
		install -m 0755  ${S}/out/usr/bin/pmgr.sh ${D}${bindir}
		install -m 0644  ${S}/out/etc/pmgr/pmgr.conf ${D}${sysconfdir}/pmgr
		install -m 0755  ${S}/out/etc/init.d/S20RtosSetting ${D}${sysconfdir}/init.d
}

