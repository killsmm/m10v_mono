DESCRIPTION = "Gstremaer RTSP server"
SECTION = "sni-gstrtsp"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

SRC_URI = " \
file://sni-gstrtsp.c \
"
RDEPENDS_${PN}="gstreamer1.0 gstreamer1.0-plugins-base ipcu gstreamer1.0-rtsp-server"
DEPENDS = "gstreamer1.0-plugins-base gstreamer1.0 glib-2.0 glibc gstreamer1.0-rtsp-server"

S = "${WORKDIR}/"

do_configure() {
}
do_compile() {
	CC=g++
	BROOT=${TMPDIR}/work/${MULTIMACH_TARGET_SYS}
	GST_CFLAGS="-I${BROOT}/gstreamer1.0/1.8.3-r0/image/usr/include/gstreamer-1.0 -I${BROOT}/gstreamer1.0/1.8.3-r0/image/usr/lib/gstreamer-1.0/include"
	GSTBASE_CFLAGS="-I${BROOT}/gstreamer1.0-plugins-base/1.8.3-r0/image/usr/include/gstreamer-1.0"
	GSTRTSP_CFLAGS="-I${BROOT}/gstreamer1.0-rtsp-server/1.8.3-r0/image/usr/include/gstreamer-1.0"
	GLIB_CFLAGS="-I${BROOT}/glib-2.0/1_2.48.2-r0/image/usr/include/glib-2.0 -I${BROOT}/glib-2.0/1_2.48.2-r0/image/usr/lib/glib-2.0/include"
	GST_LDFLAGS="-L${BROOT}/gstreamer1.0/1.8.3-r0/image/usr/lib/gstreamer-1.0 -L${BROOT}/gstreamer1.0/1.8.3-r0/image/usr/lib"
	GSTBASE_LDFLAGS="-L${BROOT}/gstreamer1.0-plugins-base/1.8.3-r0/image/usr/lib/gstreamer-1.0"
	GSTRTSP_LDFLAGS="-L${BROOT}/gstreamer1.0-rtsp-server/1.8.3-r0/image/usr/lib/gstreamer-1.0 -L${BROOT}/gstreamer1.0-rtsp-server/1.8.3-r0/image/usr/lib"
	GLIB_LDFLAGS="-L${BROOT}/glib-2.0/1_2.48.2-r0/image/usr/lib"
	${CC} ${WORKDIR}/sni-gstrtsp.c -o ${WORKDIR}/sni-gstrtsp \
        ${CFLAGS} ${GST_CFLAGS} ${GLIB_CFLAGS} ${GSTRTSP_CFLAGS} ${GSTBASE_CFLAGS} \
        ${LDFLAGS} ${GST_LDFLAGS} ${GLIB_LDFLAGS} ${GSTRTSP_LDFLAGS} ${GSTBASE_LDFLAGS} \
	-lgstrtspserver-1.0 -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0
}
do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${S}/sni-gstrtsp ${D}${bindir}
}

