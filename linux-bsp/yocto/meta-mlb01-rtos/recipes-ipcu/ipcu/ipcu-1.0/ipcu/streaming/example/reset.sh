#!/bin/sh
echo 257 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio257/direction
echo 0 > /sys/class/gpio/gpio257/value
