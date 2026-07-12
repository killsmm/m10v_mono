# !/bin/sh

# check app_file exist to RTOS
camera_if chk_app_file_exist > /tmp/app_file_exist

# check file exist check result
cat /tmp/app_file_exist | grep "Return 1 = 0x00000000" > /dev/null

if [ $? = 0 ] ; then
	mount -t ipcufs mnt /media/root/test_mtp_sd -ofs=8
	/media/root/test_mtp_sd/app_file &
	exit 0
fi
exit 1
