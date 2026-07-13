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

EXTERNALSRC = "${M10V_MONOREPO_ROOT}/app/m10v_HDMI_UI"
PV = "1.0"


# NOTE: the following library dependencies are unknown, ignoring: libgroupsock libUsageEnvironment libBasicUsageEnvironment libliveMedia
#       (this is based on recipes that have previously been built and packaged)
inherit qmake_base

DEPENDS_prepend = "qt4-tools-native "

export QMAKESPEC = "${STAGING_DATADIR}/qtopia/mkspecs/${TARGET_OS}-oe-g++"
export OE_QMAKE_QT_CONFIG = "${STAGING_DATADIR}/qtopia/mkspecs/qconfig.pri"
export OE_QMAKE_UIC = "${STAGING_BINDIR_NATIVE}/uic4"
export OE_QMAKE_UIC3 = "${STAGING_BINDIR_NATIVE}/uic34"
export OE_QMAKE_MOC = "${STAGING_BINDIR_NATIVE}/moc4"
export OE_QMAKE_RCC = "${STAGING_BINDIR_NATIVE}/rcc4"
export OE_QMAKE_QDBUSCPP2XML = "${STAGING_BINDIR_NATIVE}/qdbuscpp2xml4"
export OE_QMAKE_QDBUSXML2CPP = "${STAGING_BINDIR_NATIVE}/qdbusxml2cpp4"
export OE_QMAKE_QMAKE = "${STAGING_BINDIR_NATIVE}/qmake2"
export OE_QMAKE_LINK = "${CXX}"
export OE_QMAKE_CXXFLAGS = "${CXXFLAGS}"
export OE_QMAKE_INCDIR_QT = "${STAGING_INCDIR}/qtopia"
export OE_QMAKE_LIBDIR_QT = "${STAGING_LIBDIR}"
export OE_QMAKE_LIBS_QT = "qt"
export OE_QMAKE_LIBS_X11 = "-lXext -lX11 -lm"
export OE_QMAKE_LIBS_X11SM = "-lSM -lICE"
export OE_QMAKE_LCONVERT = "${STAGING_BINDIR_NATIVE}/lconvert4"
export OE_QMAKE_LRELEASE = "${STAGING_BINDIR_NATIVE}/lrelease4"
export OE_QMAKE_LUPDATE = "${STAGING_BINDIR_NATIVE}/lupdate4"
export OE_QMAKE_XMLPATTERNS = "${STAGING_BINDIR_NATIVE}/xmlpatterns4"

# Specify any options you want to pass to cmake using EXTRA_OECMAKE:
EXTRA_OECMAKE = ""

do_install(){
     install -d ${D}${bindir}
     install -m 0755 ${S}/qte-fthdmi ${D}/${bindir}
}

do_compile_prepend(){
    bbwarn "PV = ${PV}"
}

FILES_${PN} += "${bindir}"

DEPENDS += "zeromq qt4-embedded json-c libexif nativesdk-qt4-tools"
