.SUFFIX: .c .cpp .s .o .d .h .inc

ifeq "$(OS)" "Windows_NT"
PATH_CONVERT = $(shell cygpath -m $(1))
else
PATH_CONVERT = $(1)
endif

#
# ARM settings
#

### ARM core (used by option)
ARMCORE         ?= cortex-a7

### Priority Option
PRIORITY_OPTION := fast
#PRIORITY_OPTION := size
#PRIORITY_OPTION := debug

### Tool Chain
TCHAIN          := $(GNUarm)



#
# Include paths
#

### Used by the option of armcc
CINCPATH        :=
CINCPATH        += -I$(ROOT_DIR)/include
CINCPATH        += -I$(ROOT_DIR)/include/Arm
CINCPATH        += -I$(ROOT_DIR)/include/Dsp
CINCPATH        += -I$(ROOT_DIR)/include/Exs
CINCPATH        += -I$(ROOT_DIR)/include/Image
CINCPATH        += -I$(ROOT_DIR)/include/Image/PRO
CINCPATH        += -I$(ROOT_DIR)/include/Peri
CINCPATH        += -I$(ROOT_DIR)/include/Pmc
CINCPATH        += -I$(ROOT_DIR)/include/Top
CINCPATH        += $(SUB_CINCPATH)
CPPINCPATH      :=

### Used by the option of armasm
AINCPATH        :=



#
# Toolchain options
#

### Compile Option File
-include $(ROOT_DIR)/$(WS_DIR_NAME)/$(PRJ_NAME).opt
-include $(ROOT_DIR)/$(WS_DIR_NAME)/MILB_COMMON.opt

### Link Option File
-include $(ROOT_DIR)/$(WS_DIR_NAME)/$(PRJ_NAME).via

### Compiler
CC              := $(TCHAIN)gcc
CFLAGS          := -c
CFLAGS          += -pipe
CFLAGS          += -g3
CFLAGS          += -gdwarf-2
CFLAGS          += -std=gnu99
ifneq "$(ARMCORE)" ""
CFLAGS          += -mlittle-endian
CFLAGS          += -mcpu=$(ARMCORE)
CFLAGS          += -mtune=$(ARMCORE)
ifneq "$(ENABLE_NEON)" ""
CFLAGS          += -mfloat-abi=hard
CFLAGS          += -mfpu=neon-vfpv4
else
CFLAGS          += -mfloat-abi=soft
endif
CFLAGS          += -mthumb-interwork
ifeq "$(PRIORITY_OPTION)" "debug"
CFLAGS          += -marm
else ifeq "$(PRIORITY_OPTION)" "size"
CFLAGS          += -mthumb
else
CFLAGS          += -marm
endif
endif
CFLAGS          += -fdollars-in-identifiers
CFLAGS          += -fno-strict-aliasing
CFLAGS          += -fno-delete-null-pointer-checks
CFLAGS          += -fno-dwarf2-cfi-asm
CFLAGS          += -fno-stack-protector
CFLAGS          += -fno-strict-overflow
CFLAGS          += -Wall
CFLAGS          += $(MILB_COMMON_OPT)
CFLAGS          += $(MILB_OPT)
CFLAGS          += $(MILB_VIA)
ifeq "$(PRIORITY_OPTION)" "debug"
CFLAGS          += -O0
CFLAGS          += -fno-inline
else ifeq "$(PRIORITY_OPTION)" "size"
CFLAGS          += -Os
else
CFLAGS          += -Ofast
endif

CFLAGS_DEPENDS  := -c
CFLAGS_DEPENDS  += -pipe
ifneq "$(ARMCORE)" ""
CFLAGS_DEPENDS  += -mcpu=$(ARMCORE)
endif
CFLAGS_DEPENDS  += -fsyntax-only
CFLAGS_DEPENDS  += $(MILB_COMMON_OPT)
CFLAGS_DEPENDS  += $(MILB_OPT)
CFLAGS_DEPENDS  += $(MILB_VIA)

CPPFLAGS         =
CPPFLAGS_DEPENDS =

### Assember
AS              := $(TCHAIN)as
ASFLAGS         := -c
ASFLAGS         := -pipe
ASFLAGS         += -g
ASFLAGS         += -gdwarf-2
ifneq "$(ARMCORE)" ""
ASFLAGS         += -mcpu=$(ARMCORE)
ASFLAGS         += -mlittle-endian
ASFLAGS         += -mthumb-interwork
endif
ASFLAGS         += -x assembler-with-cpp
ASFLAGS         += -Wa,--keep-locals
ASFLAGS         += $(MILB_COMMON_OPT)
ASFLAGS         += $(MILB_OPT)
ASFLAGS         += $(MILB_VIA)

ASFLAGS_DEPENDS := -fsyntax-only
ASFLAGS_DEPENDS += -pipe
ASFLAGS_DEPENDS += -mcpu=$(ARMCORE)
ASFLAGS_DEPENDS += -x assembler-with-cpp
ASFLAGS_DEPENDS += $(MILB_COMMON_OPT)
ASFLAGS_DEPENDS += $(MILB_OPT)
ASFLAGS_DEPENDS += $(MILB_VIA)



#
# Directory and File names
#

OBJ_DIR_NAME     ?= obj
SRC_DIR_NAME     := src
DEP_DIR_NAME     := dep
INC_DIR_NAME     := include
OBJ_FILE          = $(OBJ_DIR_NAME)/%.o
SRC_CFILE         = $(SRC_DIR_NAME)/%.c
SRC_CPPFILE       = $(SRC_DIR_NAME)/%.cpp
SRC_AFILE         = $(SRC_DIR_NAME)/%.s
DEP_FILE          = $(DEP_DIR_NAME)/%.d
CUROBJSFILE       = $(ROOT_DIR)/$(WS_DIR_NAME)/$(OBJ_DIR_NAME)/$(strip $(subst /,_, $(DIR))).objs
CSRC_TMP          = $(patsubst %.c,$(SRC_DIR_NAME)/%.c,$(CSRC))
CPPSRC_TMP        = $(patsubst %.cpp,$(SRC_DIR_NAME)/%.cpp,$(CPPSRC))
ASRC_TMP          = $(patsubst %.s,$(SRC_DIR_NAME)/%.s,$(ASRC))
OBJS              = $(CSRC_TMP:$(SRC_DIR_NAME)/%.c=$(OBJ_DIR_NAME)/%.o)
OBJS             += $(ASRC_TMP:$(SRC_DIR_NAME)/%.s=$(OBJ_DIR_NAME)/%.o)
OBJS             += $(CPPSRC_TMP:$(SRC_DIR_NAME)/%.cpp=$(OBJ_DIR_NAME)/%.o)
DEPS              = $(CSRC_TMP:$(SRC_DIR_NAME)/%.c=$(DEP_DIR_NAME)/%.d)
DEPS             += $(ASRC_TMP:$(SRC_DIR_NAME)/%.s=$(DEP_DIR_NAME)/%.d)
DEPS             += $(CPPSRC_TMP:$(SRC_DIR_NAME)/%.cpp=$(DEP_DIR_NAME)/%.d)
ALL_SRC           = $(CSRC)
ALL_SRC          += $(ASRC)
ALL_SRC          += $(CPPSRC)



#
# Targets
#

QUIET := 

.PHONY: default target-self recursive-target depends-self clean-self distclean-self $(SUBDIRS) \
        target target-objs-self clean clean-obj clean-obj-self clean-depends-self depends message message-self

default: target

target-self:
ifneq "$(words $(OBJS))" "0"
	$(MAKE) target-objs-self; \
	if [ ! -d $(OBJ_DIR_NAME) ] ; then mkdir $(OBJ_DIR_NAME); fi; \
	$(MAKE) IS_INCLUDE_DEPS=y $(MULTI_JOB_OPT) $(OBJS);
endif

recursive-target: $(PRE_TARGET) $(SUBDIRS) $(POST_TARGET)

depends-self:
ifneq "$(words $(DEPS))" "0"
	$(MAKE) clean-depends-self; \
	$(MAKE) $(MULTI_JOB_OPT) $(DEPS);
endif

clean-self: clean-obj-self clean-depends-self

distclean-self: clean-self

ifeq "$(IS_INCLUDE_DEPS)" "y"
-include $(DEPS)
endif

### Sub Folders
ifneq "$(SUBDIRS)" ""
$(SUBDIRS): 
	$(QUIET)$(MAKE) recursive-target -C $@
endif

### Build
target: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=target-self

target-objs-self: 
ifneq "$(words $(ALL_SRC))" "0"
	$(QUIET)if [ ! -d $(ROOT_DIR)/$(WS_DIR_NAME)/$(OBJ_DIR_NAME) ] ; then mkdir $(ROOT_DIR)/$(WS_DIR_NAME)/$(OBJ_DIR_NAME); fi
	$(QUIET)if [ -f $(CUROBJSFILE) ] ; then rm -f $(CUROBJSFILE); fi
	$(QUIET)echo $(patsubst $(OBJ_DIR_NAME)/%.o, $(DIR)/$(OBJ_DIR_NAME)/%.o, $(OBJS)) > $(CUROBJSFILE)
endif

$(OBJ_FILE): $(SRC_CFILE)
	-$(QUIET)echo compiling $< ...; \
	$(QUIET)$(CC) $(CFLAGS) $(CINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)

$(OBJ_FILE): $(SRC_CPPFILE)
	-$(QUIET)echo compiling $< ...; \
	$(QUIET)$(CC) $(CPPFLAGS) $(CPPINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)

$(OBJ_FILE): $(SRC_AFILE)
	-$(QUIET)echo assembling $< ...; \
	$(QUIET)$(CC) $(ASFLAGS) $(AINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)

### Clean
clean: 
	$(QUIET)$(MAKE) $(MULTI_JOB_OPT) recursive-target POST_TARGET=clean-self

clean-obj: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=clean-obj-self

clean-obj-self: 
	$(QUIET)if [ -d $(OBJ_DIR_NAME) ] ; then rm -rf $(OBJ_DIR_NAME); fi

clean-depends-self: 
	$(QUIET)if [ -d $(DEP_DIR_NAME) ] ; then rm -rf $(DEP_DIR_NAME); fi

### Depends
depends: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=depends-self

$(DEP_FILE): $(SRC_CFILE)
	-$(QUIET)if [ ! -d $(DEP_DIR_NAME) ] ; then mkdir $(DEP_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo creating $@ ...; \
	$(QUIET)$(CC) $(CFLAGS_DEPENDS) $(CINCPATH) -MT $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%.o,$<) -MT $@ -M -MF $@ $< 

$(DEP_FILE): $(SRC_CPPFILE)
	-$(QUIET)if [ ! -d $(DEP_DIR_NAME) ] ; then mkdir $(DEP_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo creating $@ ...; \
	$(QUIET)$(CC) $(CPPFLAGS_DEPENDS) $(CPPINCPATH) -MT $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%.o,$<) -MT $@ -M -MF $@ $< 

$(DEP_FILE): $(SRC_AFILE)
	-$(QUIET)if [ ! -d $(DEP_DIR_NAME) ] ; then mkdir $(DEP_DIR_NAME) >/dev/null 2>&1; fi; \
	$(QUIET)echo creating $@ ...; \
	$(QUIET)$(CC) $(ASFLAGS_DEPENDS) $(AINCPATH) -MT $(patsubst $(SRC_DIR_NAME)/%.s,$(OBJ_DIR_NAME)/%.o,$<) -MT $@ -M -MF $@ $< 

### Message
message: 
	$(QUIET)$(MAKE) recursive-target PRE_TARGET=message-self

message-self: 
ifneq "$(MESSAGE)" ""
	$(QUIET)echo $(MESSAGE)
endif
