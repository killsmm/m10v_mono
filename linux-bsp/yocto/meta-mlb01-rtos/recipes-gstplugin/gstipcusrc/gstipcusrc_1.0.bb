DESCRIPTION = "Gstremaer plugin"
SECTION = "gst-ipcusrc"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

SRC_URI = " \
file://gstipcusrc/ \
"

DEPENDS = "gstreamer1.0-plugins-base gstreamer1.0 ipcu"

inherit autotools
inherit pkgconfig

RDEPENDS_${PN}="gstreamer1.0 gstreamer1.0-plugins-base ipcu"

S = "${WORKDIR}/gstipcusrc/"

FILES_${PN} = "${libdir}/gstreamer-1.0/*.so"

IPCUDEVPATH="../../../../ipcu/1.0-r0/ipcu"

export CFLAGS="-I${IPCUDEVPATH}/include -I${IPCUDEVPATH}/streaming" 

do_configure() {
	cd ../gstipcusrc
	chmod 777 ./autogen.sh
	./autogen.sh
	./configure \
	    --host=arm-poky-linux-gnueabi \
		--target=arm-poky-linux-gnueabi \
		--build=x86_64-pc-linux-gnu

}
do_compile() {
	cd ../gstipcusrc
    make
}
do_install() {
	install -d ${D}${libdir}/gstreamer-1.0
	install -m 0755 ${S}/src/.libs/libgstipcu.so ${D}${libdir}/gstreamer-1.0/
}

