#!/bin/bash
#
# shrivel.sh
# Copyright (C) 2015 Linaro, Ltd
# Andy Green <andy.green@linaro.org>
#
# Needs the cross toolchain for cross objdump

# "shrivel" shrinks down the rootfs image based on whitelist.
# This tries to track down the dependency of given binary/symlinks by
# using objdump.

POKYDIR=/opt/poky/2.2/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/

if [ -z "$CROSS" ] ; then
	if [ -d $POKYDIR ]; then
		CROSS=$POKYDIR/arm-poky-linux-gnueabi-
	else
		CROSS=arm-linux-gnueabihf-
	fi
	# Even if we have no cross toolchain, objdump can analyze the ELF header.
	which ${CROSS}objdump &> /dev/null || CROSS=
fi

function usage {
	echo "Usage: $0 <original rootfs tarball> <whitelist>"
	exit 1
}

function cpdir {	# src dest
	[ -d $2 ] && return 0
	[ ! -d `dirname $2` ] && cpdir $1 `dirname $2`
	mkdir "$2"
	chown --reference="$1" "$2"
	chmod --reference="$1" "$2"
	touch -mr "$1" "$2"
}

function recurse {
	local x
	local y

	x=$1
	# relative path -> absolute path inside filesystem
	if [ ${x:0:1} != '/' ] ; then
		y=`find orig -name "$1" | head -n 1 | sed "s|^orig/||g"`
		if [ -z "$y" ]; then
			echo "failed to find $x"
			# echo $x >> _not_found
			return
		fi
		x=/$y
	elif [ ! -e "orig/$x" -a ! -L "orig/$x" ] ; then
		# echo $x >> _not_found
		return
	fi

	# we have to check directory first, because it can be copied via symlink
	if [ -d "orig/$x" ] ; then
		cpdir "orig/$x" "shrivelled/$x"
		ls orig/$x | while read d ; do
			recurse $x/$d
		done
		return
	elif [ -e "shrivelled/$x" -o -L "shrivelled/$x" ] ; then
		# if already included, don't add again
		return;
	fi

	if [ -L "orig/$x" ] ; then	# symlink?
		y=`readlink "orig/$x"`
		[ ${y:0:1} != '/' ] && y=`dirname $x`/$y
		recurse $y
		# fall through to copy symlink itself
	fi

	cpdir orig/`dirname $x` shrivelled/`dirname $x`
	cp -a orig/$x shrivelled/$x

	# Avoid objdump to track down the symlink
	[ -L "orig/$x" ] && return

	${CROSS}objdump -x orig/$x 2>/dev/null | \
		grep NEEDED | sed "s/.*NEEDED\ *//g" |\
		while read xx ; do
			recurse $xx
		done
}

if [ -z "$1" -o ! -e "$1" ] ; then
	usage
fi
if [ -z "$2" -o ! -e "$2" ] ; then
	usage
fi

# We have to make rootfs cleanly
rm -rf orig shrivelled
mkdir -p orig shrivelled
echo "Unpacking $1..."
tar -x -C orig -f "$1"

echo "Picking..."

cat $2 | while read line ; do
	if [ -z "$line" ] ; then
		continue;
	fi
	recurse $line
done

echo "Fixing up init files"
#
# add a symlink for /init needed by initramfs
pushd shrivelled >/dev/null
if [ ! -e init ]; then
  if [ -x sbin/init ]; then
    ln -sf /sbin/init init
  elif [ -x sbin/finit ]; then
    ln -sf /sbin/finit init
  elif [ -x bin/sh ]; then
    ln -sf /bin/sh init
  fi
fi
if [ ! -e sbin/finit ]; then
  echo "#!/bin/sh" > sbin/finit
  if [ -e etc/fstab ]; then
    echo "mount -a" >> sbin/finit
  else
    echo "mount -t proc proc /proc" >> sbin/finit
    echo "mount -t sysfs sysfs /sys" >> sbin/finit
    echo "mount -t devtmpfs devtmpfs /dev" >> sbin/finit
    echo "mount -t debugfs debugfs /sys/kernel/debug" >> sbin/finit
  fi
  echo "exec /bin/sh" >> sbin/finit
  chmod a+x sbin/finit
fi

# some fixups to make sure about critical file owner
sudo chown -h root:root init
if [ -L init ] ; then
  x=`readlink init`
  sudo chown root:root ./$x
fi
sudo chown -h root:root . bin boot dev etc lib sbin usr bin/*

echo "Packing initramfs..."
name=`basename $1 | sed "s/\.tar.*//g"`-`basename $2`

tar cjf ../$name.tar.bz2 *
find . | cpio -H newc -o > ../initramfs.cpio
#cat ../initramfs.cpio | \
#	xz --check=crc32 --lzma2=dict=1MiB -c - > ../$name.initramfs
#cat ../initramfs.cpio | \
#	bzip2 -9 -f -c  > ../$name.initramfs
cat ../initramfs.cpio | \
	gzip -9  > ../$name.initramfs

rm ../initramfs.cpio
popd >/dev/null

echo "Done"
echo 

du -h shrivelled --max-depth=0 2>/dev/null
ls -l $name.tar.bz2 $name.initramfs
# rm -rf orig
#rm -rf shrivelled

