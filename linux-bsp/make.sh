#!/bin/bash
usage="
============================================================
Before compile Yocto/Linux,
please make sure the cores configuration.
(Please check MemoryMap.xlsm under Board_Config folder)

To build 1gb with 1core ver:             ./make.sh 1g1c
To build 2gb with 1core ver:             ./make.sh 2g1c
To build 2gb with 2cores ver:            ./make.sh 2g2c
To build 1gb with 2cores ver:            ./make.sh 1g2c
============================================================"


if [ "$1" = "-help" ] || [ "$1" = "-h" ];then
echo "$usage"
exit;
fi

DTS=""

if [ "$1" = "1g1c" ];then
ROMSEL=2
DTS="./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos-okpf1g.dts"
elif [ "$1" = "2g2c" ];then
DTS="./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos-act.dts"
ROMSEL=1
elif [ "$1" = "2g1c" ];then
DTS="./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos-act.dts"
ROMSEL=0
elif [ "$1" = "1g2c" ];then
ROMSEL=3
DTS="./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos-okpf1g.dts"
else
echo "$usage"
exit;
fi

# ROMSEL=0 means 2gb with 1core
# ROMSEL=1 means 2gb with 2core
# ROMSEL=2 means 1gb with 1core
# ROMSEL=3 means 1gb with 2core

echo " "
echo "===================================="
echo "start building poky, the cmd will be" 
echo ". ./build_poky.sh rtos 8 8"
echo "please edit make.sh if need to change"
echo "====================================="
echo " " 


if [ ! -d ./stubby/sc2000-rtos-2core ]; then
cd ./stubby
make
cd ..
elif [ ! -d ./stubby/sc2000-rtos ]; then
cd ./stubby
make
cd ..
elif [ ! -d ./stubby/sc2000-rtos-okpf1g ]; then
cd ./stubby
make
cd ..
fi
mkdir -p ./shrivel/stubby
if [ "$ROMSEL" -eq 0 ];then
	echo "#######################"
	echo "    2GB ROM with 1core"
	echo "#######################"
	cp ./stubby/sc2000-rtos/stubby-sc2000-rtos.bin ./shrivel/stubby/stubby.bin
	sed -i -e "22d" ./make_BIN_linux/make_bin_linux.sh 
	sed -i -e "22i DEVICETREE=mlb01-evb-rtos-act" ./make_BIN_linux/make_bin_linux.sh
	sed -i -e "25d" ./make_BIN_linux/make_bin_linux.sh 
	sed -i -e "25i MAKELDIMAGE=0" ./make_BIN_linux/make_bin_linux.sh
	echo "changed ./make_BIN_linux/make_bin_linux.sh"
        sed -i -e "16d" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
        sed -i -e "16i /*" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
        sed -i -e "22d" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
        sed -i -e "22i */" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
        echo "changed ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi"
elif [ "$ROMSEL" -eq 1 ];then
        echo "###########################"
        echo "    2GB ROM with 2cores"
        echo "###########################"
        cp ./stubby/sc2000-rtos-2core/stubby-sc2000-rtos-2core.bin ./shrivel/stubby/stubby.bin
        sed -i -e "22d" ./make_BIN_linux/make_bin_linux.sh
        sed -i -e "22i DEVICETREE=mlb01-evb-rtos-act" ./make_BIN_linux/make_bin_linux.sh
        sed -i -e "25d" ./make_BIN_linux/make_bin_linux.sh
        sed -i -e "25i MAKELDIMAGE=0" ./make_BIN_linux/make_bin_linux.sh
        echo "changed ./make_BIN_linux/make_bin_linux.sh"
	sed -i -e "16d" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
	sed -i -e "16i /**/" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
        sed -i -e "22d" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
        sed -i -e "22i /**/" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
	echo "changed ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi"
elif [ "$ROMSEL" -eq 2 ];then
	echo "#####################"
	echo "  1GB ROM with 1core"
	echo "#####################"
	cp ./stubby/sc2000-rtos-okpf1g/stubby-sc2000-rtos-okpf1g.bin ./shrivel/stubby/stubby.bin
	sed -i -e "22d" ./make_BIN_linux/make_bin_linux.sh 
	sed -i -e "22i DEVICETREE=mlb01-evb-rtos-okpf1g" ./make_BIN_linux/make_bin_linux.sh
	sed -i -e "25d" ./make_BIN_linux/make_bin_linux.sh 
	sed -i -e "25i MAKELDIMAGE=1" ./make_BIN_linux/make_bin_linux.sh
	echo "changed ./make_BIN_linux/make_bin_linux.sh"
        sed -i -e "16d" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
        sed -i -e "16i /*" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
        sed -i -e "22d" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
        sed -i -e "22i */" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
        echo "changed ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi"
else
        echo "######################"
        echo "  1GB ROM with 2cores "
        echo "######################"
        cp ./stubby/sc2000-rtos-okpf1g/stubby-sc2000-rtos-okpf1g.bin ./shrivel/stubby/stubby.bin
        sed -i -e "22d" ./make_BIN_linux/make_bin_linux.sh
        sed -i -e "22i DEVICETREE=mlb01-evb-rtos-okpf1g" ./make_BIN_linux/make_bin_linux.sh
        sed -i -e "25d" ./make_BIN_linux/make_bin_linux.sh
        sed -i -e "25i MAKELDIMAGE=1" ./make_BIN_linux/make_bin_linux.sh
        echo "changed ./make_BIN_linux/make_bin_linux.sh"
        sed -i -e "16d" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
        sed -i -e "16i /**/" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
        sed -i -e "22d" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
        sed -i -e "22i /**/" ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi
        echo "changed ./linux-4.4.15/arch/arm/boot/dts/mlb01-evb-rtos.dtsi"
fi

#build time and revision
ver=2.3
d=$(date +"%F")
t=$(date +"%T")
version="SW Version:"$ver"("$d-$t")"
sed -i -e "6d" $DTS
sed -i -e "6i model = \"Socionext SC2000A EVB w/ RTOS and NETSEC - $version\";" $DTS
for i in {1..2}
do
echo "the $i time running" > tmp.txt

if [ -e ./make_BIN_linux/2GB/LINUX.BIN -a "$ROMSEL" -eq 0 ]; then
	echo "LINUX_2GB.BIN already exist, stop build"
	break;
elif [ -e ./make_BIN_linux/2GB/LINUX_2core.BIN -a "$ROMSEL" -eq 1 ]; then
	echo "LINUX_2core.BIN for 2GB already exist, stop build"
	break;
elif [ -e ./make_BIN_linux/1GB/LINUX.BIN -a "$ROMSEL" -eq 2 ]; then
        echo "LINUX_1GB.BIN already exist, stop build"
        break;
elif [ -e ./make_BIN_linux/1GB/LINUX_2core.BIN -a "$ROMSEL" -eq 3 ]; then
        echo "LINUX_2core.BIN for 1GB already exist, stop build"
        break;
else
cd yocto
echo "#############build_poky.sh###############"
. ./build_poky.sh rtos 8 8
echo "#############build_poky.sh end###############"
cd ../..
echo "start to build linux"
cd ./make_BIN_linux/
echo "#############make_bin_linux.sh###############"
./make_bin_linux.sh
echo "#############make_bin_linux.sh end###############"
cd ..
fi

mkdir -p ./make_BIN_linux/1GB/
mkdir -p ./make_BIN_linux/2GB/
if [ "$ROMSEL" -eq 0 ];then
mv ./make_BIN_linux/initramfs ./make_BIN_linux/2GB/initramfs
mv ./make_BIN_linux/LINUX.BIN ./make_BIN_linux/2GB/LINUX.BIN
elif [ "$ROMSEL" -eq 1 ];then
mv ./make_BIN_linux/initramfs ./make_BIN_linux/2GB/initramfs_2core
mv ./make_BIN_linux/LINUX.BIN ./make_BIN_linux/2GB/LINUX_2core.BIN
elif [ "$ROMSEL" -eq 2 ];then
mv ./make_BIN_linux/initramfs ./make_BIN_linux/1GB/initramfs
mv ./make_BIN_linux/LINUX.BIN ./make_BIN_linux/1GB/LINUX.BIN
elif [ "$ROMSEL" -eq 3 ];then
mv ./make_BIN_linux/initramfs ./make_BIN_linux/1GB/initramfs_2core
mv ./make_BIN_linux/LINUX.BIN ./make_BIN_linux/1GB/LINUX_2core.BIN
fi


echo " "
echo "building is finished, LINUX.BIN and initramfs will be created if there is no error"

done

