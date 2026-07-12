DESCRIPTION = "Capability information read sample"
SECTION = "capasample"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"
PR = "r0"

DEPENDS = "capaInfo ipcu"
RDEPENDS_${PN} = "capaInfo ipcu"

SRC_URI = "file://capasample"

S = "${WORKDIR}/capasample"

EXTRA_OEMAKE+="STAGING_INCDIR=${STAGING_INCDIR}"
EXTRA_OEMAKE+="STAGING_LIBDIR=${STAGING_LIBDIR}"

do_install() {
	install -d ${D}${bindir}
	install -m 0755  ${S}/read_sample/capa_read_sample ${D}${bindir}
	install -m 0755  ${S}/shell/capsmp ${D}${bindir}
}

