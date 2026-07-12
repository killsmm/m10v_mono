SUMMARY = "Lightweight and flexible command-line JSON processor"
DESCRIPTION = "jo is like sed for JSON data, you can use it to slice and \
               filter and map and transform structured data with the same \
               ease that sed, awk, grep and friends let you play with text."
HOMEPAGE = "https://stedolan.github.io/jo/"
BUGTRACKER = "https://github.com/stedolan/jo/issues"
SECTION = "utils"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=28faf13505089136797188cb50e5c4c0"

SRC_URI = "https://github.com/jpmens/jo/releases/download/v1.0/jo-1.0.tar.gz \
"

SRC_URI[md5sum] = "529373bef809ab1281b9e3c01c65f331"
SRC_URI[sha256sum] = "d66ec97258d1afad15643fb2d5b5e807153a732ba45c2417adc66669acbde52e"

#inherit autotools

#PACKAGECONFIG[docs] = "--enable-docs,--disable-docs,ruby-native"
#PACKAGECONFIG[maintainer-mode] = "--enable-maintainer-mode,--disable-maintainer-mode,flex-native bison-native"

#OE_EXTRACONF += " \
#    --disable-valgrind \
#"

#BBCLASSEXTEND = "native"

S = "${WORKDIR}/jo-1.0/"

do_configure() {
	cd ../jo-1.0

	autoreconf -i
	export ac_cv_func_realloc_0_nonnull=yes
	export ac_cv_func_malloc_0_nonnull=yes
	./configure \
	    --host=arm-poky-linux-gnueabi \
	        --target=arm-poky-linux-gnueabi 

}
do_compile() {
	cd ../jo-1.0
	
	make clean
	make all
}
do_install() {
	install -d ${D}${bindir}
	install -m 0755  ${S}/jo      ${D}${bindir}
}



