#!/bin/sh

base="[ 0.000000] Machine model: Socionext SC2000A EVB w/ RTOS and NETSEC -   "
baselen=${#base}
s=$(dmesg | grep "Socionext SC2000A")
echo ${s:$baselen}

