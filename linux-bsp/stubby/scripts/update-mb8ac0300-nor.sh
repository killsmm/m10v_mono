#!/bin/sh

VER=`ls -1 panda/staging/lib/modules* | tail -n 1`
REMOTE=root@aa9-eb.local
PICKDIR=mb8ac0300-romfs

if [ -z "$1" ] ; then
	half=0
else
	half=$1
fi

if [ $half -eq 1 -o $half -eq 0 ] ; then
#scp panda/uImage $REMOTE:/root/picker/mb8ac0300-romfs/uImage
scp panda/arch/arm/boot/Image $REMOTE:/root/picker/mb8ac0300-romfs/
scp panda/*.dtb $REMOTE:/root/picker/mb8ac0300-romfs

rsync -av panda/staging/* $REMOTE:/

ssh $REMOTE rm -rf /root/picker/$PICKDIR/modules/*
fi

if [ $half -eq 2 -o $half -eq 0 ] ; then

rm -rf  panda/staging2/lib/modules/*
mkdir -p panda/staging2/lib/modules/$VER/kernel

for i in \
	drivers/gpu/drm/drm.ko \
	drivers/gpu/drm/drm_kms_helper.ko \
	drivers/gpu/drm/fujitsu/fdb-drm.ko \
	drivers/i2c/i2c-core.ko \
	drivers/mmc/card/mmc_block.ko \
	drivers/mmc/core/mmc_core.ko \
	drivers/mmc/host/sdhci.ko \
	drivers/mmc/host/sdhci_f_sdh30.ko \
	drivers/scsi/scsi_mod.ko \
	drivers/scsi/sd_mod.ko \
	drivers/video/cfbcopyarea.ko \
	drivers/video/cfbfillrect.ko \
	drivers/video/cfbimgblt.ko \
	drivers/video/fb_sys_fops.ko \
	drivers/video/fujitsu/f_mb8ac0300_dpi.ko \
	drivers/video/fujitsu/f_mb8ac0300_dvi.ko \
	drivers/video/fujitsu/f_hdmi_tx14.ko \
	drivers/video/fujitsu/f_mipidsi1_lp.ko \
	drivers/video/fujitsu/fdb.ko \
	drivers/video/fujitsu/mb86s70-fb.ko \
	drivers/video/fujitsu/picapdc-fb.ko \
	drivers/video/syscopyarea.ko \
	drivers/video/sysfillrect.ko \
	drivers/video/sysimgblt.ko \
; do
	mkdir -p panda/staging2/lib/modules/$VER/kernel/`dirname $i`
	cp -rp "panda/staging/lib/modules/$VER/kernel/$i" "panda/staging2/lib/modules/$VER/kernel/$i"
done
cp \
	panda/staging/lib/modules/$VER/modules.order \
	panda/staging/lib/modules/$VER/modules.builtin \
	panda/staging2/lib/modules/$VER
fi
if [ $half -eq 1 -o $half -eq 0 ] ; then

scp -rp panda/staging2/* $REMOTE:/root/picker/$PICKDIR/modules 

ssh $REMOTE "cd picker/$PICKDIR; ./mkromfs.sh"

#ssh $REMOTE ./update-rom-kernel.sh 
fi
#&& ssh root@aa9-eb.local reboot

