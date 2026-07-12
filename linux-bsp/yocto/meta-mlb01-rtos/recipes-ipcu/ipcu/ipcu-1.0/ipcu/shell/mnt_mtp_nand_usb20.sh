usage="
usage:
	./usr/bin/mnt_mtp_nand.sh 0 1 2

parameters:
	0 1 2 3		for nand flash partition
	8		for SD card on upper slot
	9 10 11 12	for emmc partition if support"

mnt_p(){
	var=$1
	echo "mount partition $var..."
	mkdir -p /media/root/mtp_nf$var
	sleep 1
	mount -t ipcufs mnt /media/root/mtp_nf$var -ofs=$var
}



if [ -z "$1" ]; then
	echo "$usage"
	exit 1
else
	if [ -d "/media/root/test_mtp_sd/" ]; then
        	rm -rf /media/root/test_mtp_sd
	fi
	insmod /lib/modules/`uname -r`/kernel/drivers/usb/gadget/udc/sn_usb20_udc.ko
        #mount partitions
        for var in "$@"
        do
                case "$var" in
                        0) mnt_p $var ;;
                        1) mnt_p $var ;;
                        2) mnt_p $var ;;
                        3) mnt_p $var ;;
                        8) mnt_p $var ;;
                        9) mnt_p $var ;;
                        10) mnt_p $var ;;
                        11) mnt_p $var ;;
                        12) mnt_p $var ;; 
                        *) echo "partition $var : unknow." ;;
                esac
        done

fi
sh /home/root/usb_test/mtp_func.sh

