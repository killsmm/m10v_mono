# Stubby Makefile
# Copyright (C) 2013 Linaro, Ltd
# memory layout of copied kernel (from start of mem in config file)

OFFSET_KERNEL ?=		0x00008000
OFFSET_DDR_STUBBY ?=		0x00000000
OFFSET_DTB ?=			0x02800000
OFFSET_PAGETABLES ?=		0x18000000

CROSS ?= arm-linux-gnueabihf-

#GIT_HEAD_HASH=$(shell git log -n 1 --pretty=%h)
BUILD_DATE=$(shell date)
BUILD_BOX=$(shell whoami)"@"$(shell hostname)

#VARIANT=$(shell echo "$(CONFIG)" | sed "s|configs/config|stubby|g" | sed "s/\.h//g")-$(GIT_HEAD_HASH)
VARIANT=$(shell echo "$(CONFIG)" | sed "s|configs/config|stubby|g" | sed "s/\.h//g")

CC := $(CROSS)gcc
LD := $(CROSS)ld
AR := $(CROSS)ar
AS := $(CROSS)as
OBJCOPY := $(CROSS)objcopy
CC_OPTS := -fPIC -g \
	  -Os \
	  -DBUILD_STAMP="\"$(VARIANT) $(BUILD_DATE) $(BUILD_BOX)\"" \
	  -DOFFSET_KERNEL=$(OFFSET_KERNEL) \
	  -DOFFSET_DTB=$(OFFSET_DTB) \
	  -DOFFSET_SECONDARY=$(OFFSET_SECONDARY) \
	  -DOFFSET_PAGETABLES=$(OFFSET_PAGETABLES) \
	  -DOFFSET_DDR_STUBBY=$(OFFSET_DDR_STUBBY) \
	  -fno-common \
	  -mno-single-pic-base \
	  -mno-apcs-float \
	  -I$(shell pwd) \
	  --include=$(shell pwd)/../$(CONFIG) \
	  -D__KERNEL__ \
	  -fno-builtin \
	  -ffreestanding \
	  -nostdinc \
	  -pipe \
	  -DCONFIG_ARM \
	  -D__ARM__ \
	  -marm \
	  -mthumb-interwork \
	  -mabi=aapcs-linux \
	  -march=armv7ve \
	  -mthumb \
	  -nostdlib \
	  -mcpu=cortex-a7 \
          -mtune=cortex-a7 \
          -mfloat-abi=soft 

export CC_OPTS
export O
export CROSS
export CC
export LD
export AR
	  
OBJS := cache-cp15.o cache-pl310.o cache_v7.o lib1funcs.o\
	boot-sequence.o load_image.o \
	serial.o \
	lowlevel_init.o \
	nonsec_virt.o \
	nonsec.o \
	boot.o \
	romfs.o \
	string.o ctype.o utils.o \
	sdhci.o f_sdh30_sdhci.o mmc.o \
	part_dos.o \
	dtb.o dtb-cfg.o
	
POSTPROCESS=/bin/true

ifeq ($(CONFIG),configs/config-mb86s70.h)
	OBJS += mb86s70-dsi.o mb86s70-mhu.o mb86s7x-hsspi.o
endif

ifeq ($(CONFIG),configs/config-mb86s72.h)
        OBJS += mb86s70-dsi.o mb86s70-mhu.o mb86s7x-hsspi.o
endif

ifeq ($(CONFIG),configs/config-mb86s73.h)
	OBJS += mb86s73-fpd.o mb86s70-mhu.o mb86s7x-hsspi.o
endif

ifeq ($(CONFIG),configs/config-mb8ac0300.h)
	OBJS += ddr3c_init.o memc_init.o
endif

ifneq ($(shell $(CC) -dM -E -c ../$(CONFIG) | grep '.define CONFIG_M8M'),)
	OBJS += m8m-ddr.o m8m-clk-io.o m8m-init.o
	POSTPROCESS=../m8m-gen-nand-metadata.sh
endif

ifneq ($(shell $(CC) -dM -E -c ../$(CONFIG) | grep '.define CONFIG_RSA_SIGNATURE'),)
	OBJS += crypto/crypto.a
endif

ifneq ($(shell $(CC) -dM -E -c ../$(CONFIG) | grep '.define CONFIG_SPLASH'),)
	OBJS += bmpfile.o
endif

ifneq ($(shell $(CC) -dM -E -c ../$(CONFIG) | grep '.define CONFIG_FS_VFAT'),)
	OBJS += fat.o
endif

ifneq ($(shell $(CC) -dM -E -c ../$(CONFIG) | grep '.define CONFIG_FS_EXT2'),)
	OBJS += ext2.o
endif

ifneq ($(shell $(CC) -dM -E -c ../$(CONFIG) | grep '.define CONFIG_SYS_NS16550'),)
	OBJS +=  ns16550.o
endif


ifneq ($(shell $(CC) -dM -E -c ../$(CONFIG) | grep '.define CONFIG_SYS_UART_USIO'),)
	OBJS += serial-usio.o
endif

CONFIG_SYS_SDRAM_BASE=$(shell $(CC) -dM -E -c ../$(CONFIG) | grep '.define CONFIG_SYS_SDRAM_BASE' | cut -d' ' -f3)
OFFSET_STUBBY_DDR_VARS=$(shell $(CC) -dM -E -c ../$(CONFIG) | grep '.define OFFSET_STUBBY_DDR_VARS' | cut -d' ' -f3)
#BINARY=$(O)/$(shell echo "$(CONFIG)" | sed "s|configs/config|stubby|g" | sed "s/\.h/.bin/g")-$(GIT_HEAD_HASH)
BINARY=$(O)/$(shell echo "$(CONFIG)" | sed "s|configs/config|stubby|g" | sed "s/\.h/.bin/g")
MAP=$(O)/$(shell echo "$(CONFIG)" | sed "s|configs/config|stubby|g" | sed "s/\.h/.map/g")
OBJECTS=$(patsubst %, $(O)/%, $(OBJS))

all:	$(BINARY)

.PHONY: clean
clean:
	rm -rf $(O)
	mkdir -p $(O)
	$(MAKE) -C crypto clean

%.o: ../src/%.c
	$(CC) -c $(CC_OPTS) $< -o $@

%.o: ../src/%.S
	$(CC) -c $(CC_OPTS) $< -o $@

$(O)/crypto/crypto.a:
	$(MAKE) -C crypto

$(BINARY): $(OBJECTS)
	cat stubby.lds | sed s/__STUBBY_DDR_VARS__/'$(OFFSET_STUBBY_DDR_VARS)+$(CONFIG_SYS_SDRAM_BASE)'/g > $(O)/stubby.lds
	$(LD) -T $(O)/stubby.lds \
		-Map $(MAP) \
		-nostdlib \
		-o $(O)/stubby.elf \
		$(OBJECTS)
	$(OBJCOPY) --gap-fill=0xff -O binary $(O)/stubby.elf $(BINARY)
	ls -l $(BINARY)
	$(POSTPROCESS) $(BINARY) $(O)
