# Stubby makefile
#
# Copyright (C) 2014 Linaro, Ltd
# Author: Andy Green <andy.green@linaro.org>

CONCURRENT_MAKE ?= $(shell cat /proc/cpuinfo | grep vendor_id | wc -l)

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

CONFIGURATIONS := $(call rwildcard,./configs,config-*.h)

ifeq ($(CONFIG),)
	FAKE_OUTPUT := $(subst .h,.bin,$(CONFIGURATIONS))
else
	FAKE_OUTPUT := $(subst .h,.bin,$(CONFIG))
endif

%.bin: %.h
	@mkdir -p $(subst configs/config-,,$(subst .bin,,$@))
	@$(MAKE) CONFIG=$< -C src -s O=../$(subst configs/config-,,$(subst .bin,,$@)) clean
	@$(MAKE) -j$(CONCURRENT_MAKE) -s CONFIG=$< -C src O=../$(subst configs/config-,,$(subst .bin,,$@)) all

all: $(FAKE_OUTPUT)