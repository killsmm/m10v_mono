CROSS_BASE:=/opt/poky/2.2/sysroots
CROSS_COMPILE:=$(CROSS_BASE)/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-
CC	= $(CROSS_COMPILE)gcc

SYSROOT = $(CROSS_BASE)/cortexa7-poky-linux-gnueabi/


OBJ_PATH = $(CURDIR)/out/usr/bin
LIB_PATH = $(CURDIR)/out/usr/lib
LINUX_INCLUDE = ./include/linux/

export CC LINUX_INCLUDE SYSROOT

all:
	make install -C ipcu_lib/libcmfwk INSTALL_PATH=$(LIB_PATH)
	make install -C smartcmd INSTALL_PATH=$(OBJ_PATH)
	make install -C test_ipcu INSTALL_PATH=$(OBJ_PATH)
	make install -C camera_if INSTALL_PATH=$(OBJ_PATH)
	make install -C camera_Info_cmd INSTALL_PATH=$(OBJ_PATH)
	make install -C streaming INSTALL_PATH=$(LIB_PATH)
	make install -C streaming/example INSTALL_PATH=$(OBJ_PATH)
	make install -C osd_command INSTALL_PATH=$(OBJ_PATH)
	make install -C fb_connector INSTALL_PATH=$(OBJ_PATH)
	make install -C pmic_com INSTALL_PATH=$(OBJ_PATH)
	make install -C fwupdate INSTALL_PATH=$(OBJ_PATH)
	make install -C cui_linux_sample/lib INSTALL_PATH=$(LIB_PATH)
	make install -C cui_linux_sample/src INSTALL_PATH=$(OBJ_PATH)
	make install -C rotary_switch_app/make_app_file_ether INSTALL_PATH=$(OBJ_PATH)
	make install -C rotary_switch_app/make_app_file_wifi INSTALL_PATH=$(OBJ_PATH)
	make install -C rotary_switch_app/rotary_switch INSTALL_PATH=$(OBJ_PATH)
	make install -C rotary_switch_app/sample_linux_app INSTALL_PATH=$(OBJ_PATH)
	make install -C live555 INSTALL_PATH=$(OBJ_PATH)
	cp shell/*.sh $(OBJ_PATH)
	cp shell/apmode $(OBJ_PATH)
	mkdir $(OBJ_PATH)/event
	cp -rf shell/event $(OBJ_PATH)
	cp streaming/example/*.sh $(OBJ_PATH)
	cp cui_linux_sample/script/*.sh $(OBJ_PATH)

clean:
	make clean -C ipcu_lib/libcmfwk
	make clean -C smartcmd
	make clean -C test_ipcu
	make clean -C camera_if
	make clean -C camera_Info_cmd
	make clean -C osd_command
	make clean -C fb_connector
	make clean -C fwupdate
	make clean -C pmic_com
	make clean -C cui_linux_sample/lib
	make clean -C cui_linux_sample/src
	make clean -C rotary_switch_app/make_app_file_ether
	make clean -C rotary_switch_app/make_app_file_wifi
	make clean -C rotary_switch_app/rotary_switch
	make clean -C rotary_switch_app/sample_linux_app
	make clean -C live555
	rm -rf $(CURDIR)/out

