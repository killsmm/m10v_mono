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
# MILB Libraries
#

### MILB_Header
MILB_HEADER_NAME ?= MILB_Header
MILB_HEADER_DIR  ?= $(ROOT_DIR)/$(MILB_HEADER_NAME)



#
# Include paths
#

### Used by the option of armcc
CINCPATH        :=
CINCPATH        += -I$(ROOT_DIR)/include
CINCPATH        += -I$(MILB_HEADER_DIR)/include
CINCPATH        += -I$(MILB_HEADER_DIR)/include/Arm
CINCPATH        += -I$(MILB_HEADER_DIR)/include/Dsp
CINCPATH        += -I$(MILB_HEADER_DIR)/include/Exs
CINCPATH        += -I$(MILB_HEADER_DIR)/include/Image
CINCPATH        += -I$(MILB_HEADER_DIR)/include/Image/PRO
CINCPATH        += -I$(MILB_HEADER_DIR)/include/Peri
CINCPATH        += -I$(MILB_HEADER_DIR)/include/Pmc
CINCPATH        += -I$(MILB_HEADER_DIR)/include/Top
CINCPATH        += -I$(ROOT_DIR)/Project/ImageMacro/include
ifeq "$(CT)" "ON"
CINCPATH        += -I$(ROOT_DIR)/Project/ComponentTest/include
endif
##### REMOVE_RELEASE BEGIN #####
ifeq "$(PT)" "ON"
CINCPATH        += -I$(ROOT_DIR)/Project/PalladiumTest/include
endif
##### REMOVE_RELEASE END #####
CPPINCPATH      :=

### Used by the option of armasm
AINCPATH        :=
AINCPATH        += -I$(ROOT_DIR)/include
AINCPATH        += -I$(MILB_HEADER_DIR)/include
AINCPATH        += -I$(MILB_HEADER_DIR)/include/Arm
AINCPATH        += -I$(MILB_HEADER_DIR)/include/Peri
AINCPATH        += -I$(MILB_HEADER_DIR)/include/Top



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
##### REMOVE_RELEASE BEGIN #####
ifeq "$(PT)" "ON"
CFLAGS          += -mfloat-abi=soft
else
##### REMOVE_RELEASE END #####
ifneq "$(ENABLE_NEON)" ""
CFLAGS          += -mfloat-abi=hard
CFLAGS          += -mfpu=neon-vfpv4
else
CFLAGS          += -mfloat-abi=soft
endif
##### REMOVE_RELEASE BEGIN #####
endif
##### REMOVE_RELEASE END #####
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
CFLAGS          += $(MILB_API_OPT)
CFLAGS          += $(MILB_API_VIA)
##### REMOVE_RELEASE BEGIN #####
ifeq "$(PT)" "ON"
CFLAGS          += -DCO_PT_ENABLE
endif
##### REMOVE_RELEASE END #####
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
CFLAGS_DEPENDS  += $(MILB_API_OPT)
CFLAGS_DEPENDS  += $(MILB_API_VIA)

CPPFLAGS         =
CPPFLAGS_DEPENDS =

### Assember
AS              := $(TCHAIN)as
ASFLAGS         := -c
ASFLAGS         += -pipe
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
ASFLAGS         += $(MILB_API_OPT)
ASFLAGS         += $(MILB_API_VIA)

ASFLAGS_DEPENDS := -fsyntax-only
ASFLAGS_DEPENDS += -pipe
ASFLAGS_DEPENDS += -mcpu=$(ARMCORE)
ASFLAGS_DEPENDS += -x assembler-with-cpp
ASFLAGS_DEPENDS += $(MILB_COMMON_OPT)
ASFLAGS_DEPENDS += $(MILB_API_OPT)
ASFLAGS_DEPENDS += $(MILB_API_VIA)

##### REMOVE_PGRELIEF BEGIN #####
### PGRelief
ifeq "$(IS_USE_PGRELIEF)" "y"
# Create UNIX path
EPOMDIR_POSIX     = $(shell cygpath --unix "$(EPOMDIR)" | sed -e 's/\\/\\\\/g')

# Create Windows path
PGR_CMD           = "$(EPOMDIR_POSIX)/pgr5.exe"
PGR_TMP           = $(shell cygpath --windows "$(TMP)" | sed -e 's/\\/\\\\/g')
PGR_CURDIR        = $(shell cygpath --windows "$(CURDIR)" | sed -e 's/\\/\\\\/g')
PGR_STDC_INCLUDE1 = $(shell cygpath --windows "$(GNUarmInclude1)" | sed -e 's/\\/\\\\/g')
PGR_STDC_INCLUDE2 = $(shell cygpath --windows "$(GNUarmInclude2)" | sed -e 's/\\/\\\\/g')
PGR_STDC_INCLUDE3 = $(shell cygpath --windows "$(GNUarmInclude3)" | sed -e 's/\\/\\\\/g')
endif

PGRFLAGS          = -c
PGRFLAGS         += --csv
PGRFLAGS         += --output_code SJIS
PGRFLAGS         += --slashcomment
PGRFLAGS         += --C99
PGRFLAGS         += $(CINCPATH)
PGRFLAGS         += -K GNU/GCC3_C90
PGRFLAGS         += -D__GNUC__
PGRFLAGS         += -S UTF8
PGRFLAGS         += -Y "$(PGR_STDC_INCLUDE1)"
PGRFLAGS         += -Y "$(PGR_STDC_INCLUDE2)"
PGRFLAGS         += -Y "$(PGR_STDC_INCLUDE3)"
PGRFLAGS         += -Z "$(EPOMDIR)\MessageInfo\default-pgr.rul"
# Workaround of following PG-Relief error.
#   "C:\cygwin\opt\arm-none-eabi\arm-none-eabi\include\machine\ieeefp.h",406,"!","pgr0455","#error  Endianess not declared!!"
PGRFLAGS         += -D__IEEE_LITTLE_ENDIAN
PGRFLAGS         += -DSOURCE_CHECKER=PGrelief


##### REMOVE_PGRELIEF END #####

#
# Directory and File names
#

OBJ_DIR_NAME     ?= obj
SRC_DIR_NAME     := src
ASM_DIR_NAME     := asm
DEP_DIR_NAME     := dep
INC_DIR_NAME     := include
OBJ_FILE          = $(OBJ_DIR_NAME)/%.o
SRC_CFILE         = $(SRC_DIR_NAME)/%.c
SRC_CPPFILE       = $(SRC_DIR_NAME)/%.cpp
SRC_AFILE         = $(SRC_DIR_NAME)/%.s
DEP_FILE          = $(DEP_DIR_NAME)/%.d
CUROBJSFILE       = $(ROOT_DIR)/$(WS_DIR_NAME)/$(OBJ_DIR_NAME)/$(strip $(subst /,_, $(DIR))).objs
##### REMOVE_PGRELIEF BEGIN #####
PGR_CSV_FILE      = $(ROOT_DIR)/PGRelief_out.csv
##### REMOVE_PGRELIEF END #####
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
##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)if [ ! -d $(TMP)/PGR_output ] ; then mkdir $(TMP)/PGR_output >/dev/null 2>&1; fi
endif
##### REMOVE_PGRELIEF END #####
	$(MAKE) target-objs-self; \
	if [ ! -d $(OBJ_DIR_NAME) ] ; then mkdir $(OBJ_DIR_NAME); fi; \
	$(MAKE) IS_INCLUDE_DEPS=y $(MULTI_JOB_OPT) $(OBJS);
##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)cat $(OBJ_DIR_NAME)/*_pgr.csv >> $(PGR_CSV_FILE) 2> /dev/null; \
	$(QUIET)rm -f $(OBJ_DIR_NAME)/*_pgr.csv
endif
##### REMOVE_PGRELIEF END #####
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
ifeq "$(IS_GET_ASM)" "y"
	$(QUIET)$(CC) $(CFLAGS) $(CINCPATH) -S $< -o $(patsubst $(SRC_DIR_NAME)/%.c,$(ASM_DIR_NAME)/%.s,$<)
endif

##### REMOVE_PGRELIEF BEGIN #####
ifeq "$(IS_USE_PGRELIEF)" "y"
	-$(QUIET)$(PGR_CMD) $(PGRFLAGS) --temp-file "$(PGR_TMP)\PGR_output\$(<F)" "$(PGR_CURDIR)/$<" > $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%_pgr.csv,$<)
endif
##### REMOVE_PGRELIEF END #####

$(OBJ_FILE): $(SRC_CPPFILE)
	-$(QUIET)echo compiling $< ...; \
	$(QUIET)$(CC) $(CPPFLAGS) $(CPPINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)
ifeq "$(IS_GET_ASM)" "y"
	$(QUIET)$(CC) $(CPPFLAGS) $(CPPINCPATH) -S $< -o $(patsubst $(SRC_DIR_NAME)/%.cpp,$(ASM_DIR_NAME)/%.s,$<)
endif

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
	$(QUIET)if [ -d $(ASM_DIR_NAME) ] ; then rm -rf $(ASM_DIR_NAME); fi

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

