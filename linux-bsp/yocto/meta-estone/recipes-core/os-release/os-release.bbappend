OS_RELEASE_FIELDS += "BUILD_VERSION BUILD_DATE"

# python do_compile_prepend(){
#     file_path = d.getVar('DEPLOY_DIR_IMAGE', True) + '/MILB_SDK.bin.SRCPV'
#     file = open(file_path, 'r')
#     version = file.read()
#     d.setVar('RTOS_VERSION', version)
#     bb.warn(d.getVar('RTOS_VERSION', True))
# }

do_install_append(){
    bbwarn "${OS_RELEASE_FIELDS}"
    bbwarn "${BUILD_VERSION}"
}

do_compile[vardeps] += "BUILD_VERSION BUILD_DATE"

DEPENDS += "ca7-rtos"

# addtask addv after do_compile before do_install