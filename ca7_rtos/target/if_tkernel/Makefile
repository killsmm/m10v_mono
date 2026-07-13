# T-Engine type
MACHINE = smp_ca7mpcore
TETYPE	= std
Q = @

# default rules
include ../../etc/makerules


##### REMOVE_RELEASE BEGIN #####
### PGRelief
IS_USE_PGRELIEF ?=
export IS_USE_PGRELIEF
##### REMOVE_RELEASE END #####

USR_SRC  = ./tk_usercustom/src/os_user_custom.c
USR_SRC += ./tk_usercustom/src/os_resource_id.c
USR_SRC += ./tk_usercustom/src/TKernel_id.c
USR_SRC += ./tk_usercustom/src/usermain.c
USR_SRC += ./tk_usercustom/src/interrupt_set.c

USR_DEPS+=$(patsubst %.c,%.d,$(USR_SRC))

HWDEPEND_SRC_C=\
	excep_ints.c \
	led.c \
	tkdev_timer.c \
	porting.c \
	$(NULL)
HWDEPEND_DEPS+=$(patsubst %.c,%.d,$(HWDEPEND_SRC_C))

HWDEPEND_SRC+=\
	$(HWDEPEND_SRC_C) \
	icrt0.S \
	boot.S \
	vector.S \
	udelay.S \
	putc.S \
	$(NULL)

# source file dependencies (generated automatically)
DEPS = Dependencies
DEPENDENCIES_OUTPUT := $(DEPS)

# ----------------------------------------------------------------------------

# header file path (kernel settings)

HEADER  += ./
HEADER  += ./tk_usercustom/include
HEADER  += ../cpu_tkernel/basefw/include
HEADER  += ../cpu_tkernel/basefw/fj/file_mng/media_driver/include
HEADER  += ../cpu_tkernel/basefw/fj/system/include
HEADER  += ../cpu_tkernel/basefw/fj/utility/os_tools/include
HEADER  += ../cpu_tkernel/basefw/MILB_API/include
HEADER  += ../cpu_tkernel/board/sdram/include

# linker options
LINKSCRIPT = gnu.lnk
ifeq ($(USE_NEWLIB), yes)
LDFLAGS += -nostartfiles -static -T $(LINKSCRIPT)
else
LDFLAGS += -nostdlib -static -T $(LINKSCRIPT)
endif

# compiler options
include ../cpu_tkernel/project/MILB_COMMON.opt
include ../cpu_tkernel/project/MILB_SDK.opt
CFLAGS += -g3
CFLAGS += $(CFLAGS_WARNING)
CFLAGS += $(MILB_OPT)
CFLAGS += $(USER_OPT)
CFLAGS += $(BOOTVERSION_OPT)
CFLAGS += $(MILB_COMMON_OPT)
CFLAGS += $(USB_OPT)
CPPFLAGS += -DTKERNEL_ONLY
ifeq ($(TOOLCHAIN), gnu)
CFLAGS += -pipe
ASFLAGS += -pipe
endif

LIBDIR=../../lib
CFGFDIR=../../config

# ----------------------------------------------------------------------------

.PHONY: all clean cfg uImage depends target

all:
	$(MAKE) IS_INCLUDE_DEPS=y image

ifeq ($(USE_NEWLIB), yes)
LIBCSUPPORT_SRC=$(LIBDIR)/newlib_getreent.c \
				$(LIBDIR)/newlib_syscalls.c \
				$(LIBDIR)/newlib_syscalls_r.c
LIBCSUPPORT_OBJ=$(addsuffix .o, $(basename $(LIBCSUPPORT_SRC)))
LIBCSUPPORT_DEPS+=$(patsubst %.c,%.d,$(LIBCSUPPORT_SRC))
endif

CFG_SRC=$(CFGFDIR)/kernel_cfg.c
CFG_OBJ=$(addsuffix .o, $(basename $(CFG_SRC)))

HWDEPEND_OBJ=$(addsuffix .o, $(basename $(HWDEPEND_SRC)))
USR_OBJ=$(addsuffix .o, $(basename $(USR_SRC)))

image: $(LIBDIR)/libkernel.a $(HWDEPEND_OBJ) $(USR_OBJ) $(LIBCSUPPORT_OBJ) $(CFG_OBJ)
###	$(CC) $(TARGET_ARCH) $(LDFLAGS) $(HWDEPEND_OBJ) $(USR_OBJ) $(LIBCSUPPORT_OBJ) $(CFG_OBJ) $(LIBDIR)/libkernel.a -lgcc -lm -o image.elf
### #$(STATIC_LIBS) 	
###	$(Q)$(NM) $(NMFLAGS) image.elf > image.map
###	$(Q)$(OBJCOPY) -O binary image.elf image.bin

cfg:
	rm -f $(CFG_SRC)
	$(MAKE) $(CFG_SRC)

$(CFG_OBJ): $(CFG_SRC)

$(CFG_SRC): $(CFGFDIR)/kernel_cfg.cfg
	echo creating $(CFG_SRC) ...
	perl $(CFGFDIR)/cnvcfg.pl


# objects for disk loading (RAM)
clean:
	$(Q)echo "   Cleaning..."
	$(Q)$(RM) image.elf image.bin image.map $(CFG_SRC) $(CFG_OBJ) *.o $(DEPS)  $(USR_DEPS) $(HWDEPEND_DEPS) $(LIBCSUPPORT_DEPS) ./tk_usercustom/src/*.o $(LIBCSUPPORT_OBJ)
	$(Q)echo "              ...Done."


depends:

target: all


# generate dependencies
$(DEPS): ; $(TOUCH) $(DEPS)
	$(Q)include $(DEPS)


uImage :
	$(Q)mkimage -A arm -O linux -C none -T kernel -a 0x40008000 -e 0x40008000 \
		-n 'SMP T-Kernel v1.0' -d image.bin uImage
	$(Q)ls -l uImage

%.d: %.c
	echo creating $@ ...
	@set -e; $(CC) -MM $(CPPFLAGS) $< \
		| sed -e 's,^$(patsubst %.d,%.o,$(notdir $@))[ :]*.*,$(patsubst %.d,%.o,$@) $@ : $(patsubst %.d,%.c,$@) \\,g' \
	       -e 's,\([a-zA-Z0-9\._\\/]\)[\\]\([a-zA-Z0-9\._\\/]\),\1/\2,g' \
	       -e 's,//,/,g' \
		   > $@; \
	 [ -s $@ ] || rm -f $@
ifeq "$(IS_INCLUDE_DEPS)" "y"
-include $(USR_DEPS) $(HWDEPEND_DEPS) $(LIBCSUPPORT_DEPS)
endif

