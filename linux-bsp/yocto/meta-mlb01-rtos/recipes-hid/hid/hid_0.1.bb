SUMMARY = "sni hid application"
SECTION = "sni hid"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

SRC_URI = "file://hid.c"

TARGET_CC_ARCH += "${LDFLAGS}"

DEPENDS += "ipcu"
RDEPENDS_${PN}="ipcu"

S = "${WORKDIR}"

EXTRA_OEMAKE+="STAGING_INCDIR=${STAGING_INCDIR}"
EXTRA_OEMAKE+="STAGING_LIBDIR=${STAGING_LIBDIR}"

#IPCUDEVPATH="../../../../ipcu/1.0-r0/ipcu"
#export CFLAGS="-I${IPCUDEVPATH}/include" 

do_compile() {
	     ${CC} hid.c -o sn-hid -I${STAGING_LIBDIR}/ipcu/include -I${STAGING_LIBDIR}/ipcu/streaming -L$(STAGING_LIBDIR)/ipcu -Wl,-rpath-link,$(STAGING_LIBDIR)/ipcu -lipcustream -lcmfwk
}

do_install() {
	     install -d ${D}${bindir}
	     install -m 0755 sn-hid ${D}${bindir}
}
