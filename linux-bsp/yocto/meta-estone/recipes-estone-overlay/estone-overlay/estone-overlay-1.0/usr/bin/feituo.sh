#!/bin/sh
RAM_DCIM=/run/SD/mmcblk0p1/DCIM
DYNAMIC_IP=1
ETHER_TYPE=3 # 0 = 1000M ; 1 = USB HOST ; 2 = usb2 gadget 3 = usb3 gadget
DEFAULT_ETH="eth0" # "eth0" or "usb0"
CONFIG_FOLDER="/media/config"
SD_MOUNT_FOLDER="/media/SD"
UPDATE_FOLDER="/media/update"
DEFAULT_IP="192.168.137.16"
DEFAULT_GATEWAY="192.168.137.1"
HTTP_PORT="8080"


config_shutter_io() {
    echo 443 >/sys/class/gpio/export
    echo out >/sys/class/gpio/gpio443/direction
    echo 1 >/sys/class/gpio/gpio443/value
}

net_config() {

    if [ -x ${CONFIG_FOLDER}/ETHER_TYPE ];then
        ETHER_TYPE=$(cat ${CONFIG_FOLDER}/ETHER_TYPE)
    fi
    if [ "$ETHER_TYPE" -eq 2 ]; then
        echo "usb gadget ether"
        DEFAULT_ETH="usb0"
        insmod /lib/modules/"$(uname -r)"/kernel/drivers/usb/gadget/udc/sn_usb20_udc.ko
        modprobe libcomposite
        gadget_rndis.sh
    elif [ "$ETHER_TYPE" -eq 3 ]; then
        DEFAULT_ETH="usb0"
        modprobe dwc3_sn hostmode=0
        modprobe libcomposite
        gadget_rndis.sh
    elif [ "$ETHER_TYPE" -eq 0 ]; then
        echo "1000M ethernet mode"
        DEFAULT_ETH="eth0"
        net_init
    else
        echo "USB host ethernet mode"
        DEFAULT_ETH="eth0"
        modprobe dwc3_sn hostmode=1
    fi
    sleep 1s

    if [ -e "${CONFIG_FOLDER}/IP" ]; then
        DEFAULT_IP=$(cat ${CONFIG_FOLDER}/IP)
        DEFAULT_GATEWAY=$(echo ${DEFAULT_IP} | cut -d. -f1-3).1
        echo "set ip to ${DEFAULT_IP}, gateway to ${DEFAULT_GATEWAY}"
    else
        echo "no ip file, set ip to ${DEFAULT_IP}, gateway to ${DEFAULT_GATEWAY}"
    fi
}

net_up(){
    if [ $DYNAMIC_IP -eq 0 ]; then
        ###static ip###
        ifconfig ${DEFAULT_ETH} ${DEFAULT_IP}
        route add default gw ${DEFAULT_GATEWAY} ${DEFAULT_ETH}
        echo "nameserver ${DEFAULT_GATEWAY}" >/etc/resolv.conf
    else
        ###dynamic ip###
        mkdir /usr/share/udhcpc/
        cp /etc/udhcpc.d/50default /usr/share/udhcpc/default.script
        
        if ! udhcpc -i ${DEFAULT_ETH} -t 3 -n; then
            echo "failed to get ip from dhcp, set ip to ${DEFAULT_IP}"
            ifconfig ${DEFAULT_ETH} ${DEFAULT_IP}
            route add default gw ${DEFAULT_GATEWAY} ${DEFAULT_ETH}
            echo "nameserver ${DEFAULT_IP}" >/etc/resolv.conf
        else
            echo "obtain ip successfully"
        fi
    fi
    mkdir -p /var/run/sshd
    chmod 0755 /var/run/sshd
    /usr/sbin/sshd
}

make_DCIM_folder() {
    mkdir -p "$RAM_DCIM"
}

config_resolution() {
    camera_if_direct 0x1 0xc 44   #user0 resolution
    camera_if_direct 0x1 0xe 44   #h265 resolution
    camera_if_direct 0x1 0xf 0xb
}

rtos_init() {
    config_resolution
    camera_if_direct 0x1 0x19 0x8 #h264_0 bitrate
    camera_if_direct 0x1 0x24 0x3 #rotate
    camera_if_direct 0x1 0x2 0x222 #vbr h264_0 stream
    camera_if_direct 0x1 0x41 0x03ff
    camera_if_direct 0x8 0x3 0x1
    camera_if_direct 0x0 0xb 0x2
}


##############################################
################## main ######################
##############################################

# mkdir -p $CONFIG_FOLDER
# mount -t ipcufs mnt "$CONFIG_FOLDER" -ofs=0,1024
# mkdir -p $UPDATE_FOLDER
# mount -t ipcufs /media/update "$UPDATE_FOLDER" -ofs=1,1024
# mkdir -p "$SD_MOUNT_FOLDER" 
# mount -t ipcufs mnt "$SD_MOUNT_FOLDER" -ofs=8,1024

# if [ -e "${CONFIG_FOLDER}/app_file" ]; then
#     ${CONFIG_FOLDER}/app_file
#     exit 0
# fi

# config_shutter_io
# make_DCIM_folder
#net_config
# rtos_init
camera_if_direct_server > /dev/null &
camera_server -c "$CONFIG_FOLDER" > /dev/null &

# ln -s ${RAM_DCIM} /var/www/RAM_DCIM
# ln -s ${SD_MOUNT_FOLDER}/DCIM /var/www
REAL_IP=$(ifconfig ${DEFAULT_ETH} | grep "inet addr" | cut -d: -f2 | awk '{print $1}')
video_recorder -s test -b "http://${REAL_IP}:${HTTP_PORT}/RAM_DCIM" -j "$RAM_DCIM" -v "$RAM_DCIM" > /dev/null &



sleep 1s
start_ui.sh 2 &
sleep 1s
# busybox httpd -fv -h /run/SD/mmcblk0p1/DCIM -p 1234 &

if [ "$(cat ${CONFIG_FOLDER}/ENABLE_RTSP)" = "0" ]; then
    echo "RTSP not enabled"
else
    echo "enable RTSP"
    sleep 5s
    camera_if_direct 0x0 0xb 0x8
fi

net_up

if [ -e "${CONFIG_FOLDER}/app_file_append" ]; then
    ${CONFIG_FOLDER}/app_file_append
fi
