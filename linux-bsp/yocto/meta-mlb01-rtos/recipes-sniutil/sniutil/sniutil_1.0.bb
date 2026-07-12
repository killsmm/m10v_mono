DESCRIPTION = "Utilitys for Socionext"
SECTION = "sniutil"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"
PR = "r0"

SRC_URI += " \
	file://sdbw.c \
	file://mmiotool.c \
	file://athena_cif \
	"

do_compile() {
	${CC} ${CFLAGS} ${LDFLAGS} ${WORKDIR}/sdbw.c -o ${WORKDIR}/sdbw
	${CC} ${CFLAGS} ${LDFLAGS} ${WORKDIR}/mmiotool.c -o ${WORKDIR}/mmiotool
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${WORKDIR}/sdbw ${D}${bindir}/
	install -m 0755 ${WORKDIR}/mmiotool ${D}${bindir}/
	install -m 0755 ${WORKDIR}/athena_cif ${D}${bindir}/
}

