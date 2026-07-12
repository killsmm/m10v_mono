DESCRIPTION = "Capability information library"
SECTION = "capaInfo"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"
PR = "r0"

DEPENDS = "ipcu"
RDEPENDS_${PN} = "libstdc++ ipcu"

SRC_URI = "file://capaInfo"

S = "${WORKDIR}/capaInfo"

CAPAINFO_LIB = "/usr/lib/capaInfo"
CAPAINFO_INC ="/usr/include/capaInfo"

EXTRA_OEMAKE+="STAGING_INCDIR=${STAGING_INCDIR}"
EXTRA_OEMAKE+="STAGING_LIBDIR=${STAGING_LIBDIR}"
EXTRA_OEMAKE+="CAPAINFO_LIB=${CAPAINFO_LIB}"
EXTRA_OEMAKE+="CAPAINFO_INC=${CAPAINFO_INC}"

FILES_${PN} += "${CAPAINFO_LIB}/"
FILES_${PN} += "${CAPAINFO_INC}/"
FILES_${PN}-dbg += "${CAPAINFO_LIB}/.debug"
FILES_${PN}-dbg += "${CAPAINFO_INC}/.debug"

do_install() {
	install -Dm755 ${B}/work${CAPAINFO_LIB}/libcapaInfo.so ${D}${CAPAINFO_LIB}/libcapaInfo.so
#	ln -sf libcapaInfo.so ${D}${CAPAINFO_LIB}/libcapaInfo.so.0
#	ln -sf libcapaInfo.so ${D}${CAPAINFO_LIB}/libcapaInfo.so.0.0.1
}

SYSROOT_PREPROCESS_FUNCS += "capaInfo_sysroot_preprocess"
capaInfo_sysroot_preprocess () {
#	install -d ${STAGING_DIR_HOST}${CAPAINFO_LIB}
#	install -m 0755 ${S}/work${CAPAINFO_LIB}/*.so ${STAGING_DIR_HOST}${CAPAINFO_LIB}
	install -d ${STAGING_DIR_HOST}${CAPAINFO_INC}
	install -m 644 ${S}/work${CAPAINFO_INC}/*.h ${STAGING_DIR_HOST}${CAPAINFO_INC}
}


