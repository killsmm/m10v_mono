#!/bin/sh

export MTP_EXPORT_DIRS=/mtpexport:
if [ -e "/dev/mtp_usb" ]; then
   mtp-server &
fi

