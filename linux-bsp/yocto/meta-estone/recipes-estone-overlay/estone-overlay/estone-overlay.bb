# Recipe created by recipetool
# This is the basis of a recipe and may need further editing in order to be fully functional.
# (Feel free to remove these comments when editing.)

# Unable to find any files that looked like license statements. Check the accompanying
# documentation and source headers and set LICENSE and LIC_FILES_CHKSUM accordingly.
#
# NOTE: LICENSE is being set to "CLOSED" to allow you to at least start building - if
# this is not accurate with respect to the licensing of the software being built (it
# will not be in most cases) you must specify the correct value before using this
# recipe for anything other than initial testing/development!
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

SRC_URI += "file://etc \
			file://usr/bin \
			"

S = "${WORKDIR}"


do_install () {
	install -d ${D}${sysconfdir}/ssh
	install -m 0700 ${S}/etc/ssh/* ${D}${sysconfdir}/ssh
	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${S}/etc/init.d/* ${D}${sysconfdir}/init.d
	install -d ${D}${sysconfdir}
	install -m 0755 ${S}/etc/load_sysconfig.sh ${D}${sysconfdir}
	install -m 0755 ${S}/etc/sys.conf ${D}${sysconfdir}
	install -d ${D}${bindir}
	install -m 0755 ${S}/usr/bin/* ${D}${bindir}
}

