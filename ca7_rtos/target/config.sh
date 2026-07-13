#! /bin/sh

proj_top_dir=`dirname "$0"`
cd "${proj_top_dir}"

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
if [ -f ./setenv.sh ] ; then
	rm -f ./setenv.bak
	mv ./setenv.bat ./setenv.bak > /dev/null 2>&1
fi
SETENV=./setenv.sh
echo '#! /bin/sh' > "${SETENV}"
echo "IS_CONFIG=1; export IS_CONFIG" >> "${SETENV}"
echo "IS_ARM_STATE=; export IS_ARM_STATE" >> "${SETENV}"
echo "IS_GET_ASM=; export IS_GET_ASM" >> "${SETENV}"
echo "IS_USE_FEEDBACK=; export IS_USE_FEEDBACK" >> "${SETENV}"
echo "IS_USE_PGRELIEF=; export IS_USE_PGRELIEF" >> "${SETENV}"
echo "IS_BOARD=; export IS_BOARD" >> "${SETENV}"
echo "IS_CHIP_ES=; export IS_CHIP_ES" >> "${SETENV}"
echo "IS_MEMORY_SIZE=; export IS_MEMORY_SIZE" >> "${SETENV}"
echo "IS_FAST_BOOT=; export IS_FAST_BOOT" >> "${SETENV}"
echo "IS_UI=; export IS_UI" >> "${SETENV}"
echo "IS_USBPTP_MODE=; export IS_USBPTP_MODE" >> "${SETENV}"
echo "IS_FS_USED=; export IS_FS_USED" >> "${SETENV}"
echo "MAKE_OPT=; export MAKE_OPT" >> "${SETENV}"
echo "OPT_IVE_QR_LIB=; export OPT_IVE_QR_LIB" >> "${SETENV}"

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
echo ''
basename "$0"
echo complete !
echo ''
exit 0
