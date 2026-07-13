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

EXTERNALSRC = "${M10V_MONOREPO_ROOT}/app/m10v_camera_server"
PV = "1.0"


# NOTE: the following library dependencies are unknown, ignoring: libgroupsock libUsageEnvironment libBasicUsageEnvironment libliveMedia
#       (this is based on recipes that have previously been built and packaged)
# inherit make

# Specify any options you want to pass to cmake using EXTRA_OECMAKE:
# EXTRA_OECMAKE = ""



do_compile(){
	cd ${S}
	GO_VERSION=$(go version)
	bbwarn "hello"
    bbwarn "go verison = ${GO_VERSION}"
	protoc  --proto_path=./prbfiles/ \
                --go_out=./api/services --go_opt=paths=source_relative  \
                --go-grpc_out=./api/services --go-grpc_opt=paths=source_relative   \
                --grpc-gateway_out=./api/services  --grpc-gateway_opt paths=source_relative \
                --grpc-gateway_opt logtostderr=true \
                prbfiles/*.proto 
    CGO_ENABLED=1  GOOS=linux GOARCH=arm GOARM=7 go build -o build/camera_server cmd/camera_server/main.go
}

do_install_append(){
    bbwarn "D= ${D}"
    bbwarn "bindir = ${bindir}"
    bbwarn "${FILES_${PN}}"
    bbwarn "${PN}"
    install -d ${D}${bindir}
    install -m 0755 ${S}/build/camera_server ${D}${bindir}
}
TARGET_CC_ARCH += "${LDFLAGS}"

FILES_${PN} += "${bindir}"

DEPENDS += "zeromq"
