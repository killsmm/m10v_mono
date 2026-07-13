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

inherit externalsrc

EXTERNALSRC = "${M10V_MONOREPO_ROOT}/app/vue-app"
PV = "1.0"

do_compile() {
    cd ${S}
    npm install  @vue/cli
    npm install  @vue/cli-service
    npm run build
}

do_install(){
    install -d ${D}${localstatedir}/www
    bbplain "Installing files to ${D}${localstatedir}/www"
    cp -r ${S}/dist/* ${D}${localstatedir}/www

}

# add the corresponding directories to FILES_${PN} and FILES_${PN}-dev
FILES_${PN} += "${localstatedir}/www"
