#!/bin/sh

# This file goes in /lib/udev/storage.sh # Based on http://superuser.com/a/630937

MOUNT_BASE='/run/SD/'
MOUNT_USER=root
MOUNT_GROUP=root
MOUNT_OPTIONS=rw,noatime,nodiratime,nosuid,nodev
UNSAFE_CHARS='\0-\037\041-\057\072-\100\133-\140\173-\177'

case $1 in
        mountpoint)
                # Pick the mountpoint name according to partition or device name
                dir="$(echo "$ID_FS_LABEL" | tr "$UNSAFE_CHARS" _ | sed -e 's/^_*//' -e 's/_*$//')"
                if [ -z "$dir" ]; then
                        dir="${DEVNAME##*/}"
                fi

                if [ -n "$dir" ]; then
                        # Make available to rules
                        echo "$MOUNT_BASE$dir"
                fi
        ;;
        add)
                mkdir -p "$2"
                chown $MOUNT_USER:$MOUNT_GROUP "$2"

                mount -t "$ID_FS_TYPE" -o "$MOUNT_OPTIONS" $DEVNAME "$2"
		
		fsg=/tmp/insert_fsg.sh
		if [ -f $fsg ]; then
			$fsg
		fi
        ;;
        remove)
                umount -l $2
                rmdir $2

		/usr/bin/eject_usb.sh
        ;;
esac

