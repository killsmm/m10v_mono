#!/bin/sh
set -x

mount -o rw,remount /
mount -t debugfs debug /sys/kernel/debug

cd /sys/kernel/debug/tracing
echo 4096 > buffer_size_kb
echo 1 > events/dwc3/enable
echo irq==64 > events/irq/filter
echo 1 > events/irq/irq_handler_entry/enable
echo 1 > events/irq/irq_handler_exit/enable
echo stacktrace > events/dwc3/dwc3_complete_trb/trigger
echo stacktrace > events/dwc3/dwc3_prepare_trb/trigger
