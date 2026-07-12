# Example use of configuration fragments for busybox, which uses the same
# mechanism as the linux-yocto kernel recipe.
#
# The entries here will override any entries in the base busybox recipe
#
# More details can be found in the Kernel Dev Manual
# http://www.yoctoproject.org/docs/current/kernel-dev/kernel-dev.html#changing-the-configuration
FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
	file://lspci.cfg \
	file://udhcp.cfg \
	file://httpd.cfg \
	file://arping.cfg \
	file://server.cfg \
	file://usb.cfg \
	file://misc.cfg \
	"
