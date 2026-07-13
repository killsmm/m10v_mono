.SUFFIX: .a .axf .map .bin .scat .via .cfg .o .objs .txt



#
# Project and ARM settings
#

### The Name of Project
PRJ_NAME        := MILB_API
export PRJ_NAME

### ARM core (used by option)
ARMCORE         := cortex-a7
export ARMCORE

### CPU STATE
IS_ARM_STATE    ?=
export IS_ARM_STATE

### C source -> ASM
IS_GET_ASM      ?=
export IS_GET_ASM

### Tool Chain
TCHAIN  := $(GNUarm)

##### REMOVE_PGRELIEF BEGIN #####
### PGRelief
IS_USE_PGRELIEF ?=
export IS_USE_PGRELIEF
##### REMOVE_PGRELIEF END #####

#
# Directory and File names
#

### Root Directory of the Project
ROOT_DIR        := ..

### The Name of Directory
OBJ_DIR_NAME    := obj
BIN_DIR_NAME    := bin
LIB_DIR_NAME    := $(ROOT_DIR)/lib
WS_DIR_NAME     := Project
export WS_DIR_NAME

### The List of Sub-Directory which have Makefile
SUBDIRS         :=
SUBDIRS         += $(ROOT_DIR)/Project/DeviceDriver
SUBDIRS         += $(ROOT_DIR)/Project/ImageMacro
ifeq "$(CT)" "ON"
SUBDIRS         += $(ROOT_DIR)/Project/ComponentTest
endif
##### REMOVE_RELEASE BEGIN #####
ifeq "$(PT)" "ON"
SUBDIRS         += $(ROOT_DIR)/Project/PalladiumTest
endif
##### REMOVE_RELEASE END #####

### Libraries
LIBS            :=

### Objects
OBJS             = $(patsubst %.o, $(ROOT_DIR)/%.o, $(shell cat $(OBJ_DIR_NAME)/*.objs 2>/dev/null))
##### REMOVE_ME_RELEASE BEGIN #####
ifeq "no" "yes"	# Always else.
##### REMOVE_ME_RELEASE END #####
OBJS            += $(ROOT_DIR)/Project/ImageMacro/ip_obj/im_me.o
OBJS            += $(ROOT_DIR)/Project/ComponentTest/ip_obj/ct_im_me.o
##### REMOVE_ME_RELEASE BEGIN #####
endif
##### REMOVE_ME_RELEASE END #####

### File (related to armlink)
### note : If you want to make library file, Variable "ELF_FILE" must be empty.
ELF_FILE        :=
LIST_FILE       := $(BIN_DIR_NAME)/$(PRJ_NAME).map
SCAT_FILE       := $(PRJ_NAME).ld

### File (related to armar)
LIB_FILE        := $(LIB_DIR_NAME)/$(PRJ_NAME).a
LIB_INFO_FILE   := $(LIB_DIR_NAME)/$(PRJ_NAME).txt

### File (related to fromelf)
CVT_FILE        := $(BIN_DIR_NAME)/$(PRJ_NAME).bin



#
# Toolchain
#

### Linker
-include $MILB_API.via
LD              := $(TCHAIN)gcc
LDFLAGS         := -T./$(SCAT_FILE) -Wl,-Map=$(LIST_FILE),--cref,--gc-sections -symbolic
LDFLAGS         += $(MILB_API_VIA)

### Librarian
AR              := $(TCHAIN)ar
ARFLAGS          = -r

### Conveting Tool
CV              := $(TCHAIN)objcopy
CVFLAGS          = -Obinary



#
# Targets
#

ifeq "$(ELF_FILE)" ""
DEFAULT_TARGET := lib
else
DEFAULT_TARGET := target
endif

QUIET :=

.PHONY: default all recursive-target target-self lib lib-self convert distclean-self depends-self $(SUBDIRS) \
        target clean clean-self clean-obj clean-obj-self clean-elf distclean \
        depends message message-self

default: $(DEFAULT_TARGET)

all: clean depends $(DEFAULT_TARGET)

recursive-target: $(PRE_TARGET) $(SUBDIRS) $(POST_TARGET)

target-self: $(ELF_FILE) lib-self

lib: depends target lib-self

lib-self: $(LIB_FILE)

convert: $(CVT_FILE)

distclean-self: clean-self

depends-self: 

### Sub Folders
ifneq "$(SUBDIRS)" ""
$(SUBDIRS): 
	$(QUIET)$(MAKE) recursive-target -C $@
endif

### Build
target: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=target-self

$(LIB_FILE): $(OBJS)
	$(QUIET)echo Creating $@ ...
	$(QUIET)if [ ! -d $(LIB_DIR_NAME) ] ; then mkdir $(LIB_DIR_NAME); fi
	$(QUIET)$(AR) $(ARFLAGS) $@ $(OBJS)
	$(QUIET)$(AR) -t $@ > $(LIB_INFO_FILE)
	$(QUIET)echo done.

$(ELF_FILE): $(OBJS) $(SCAT_FILE) $(LIBS)
	$(QUIET)echo Creating $@ ...
	$(QUIET)if [ ! -d $(BIN_DIR_NAME) ] ; then mkdir $(BIN_DIR_NAME); fi
	$(QUIET)$(LD) $(OBJS) $(LDFLAGS) $(LIBS) -o $@ -lm
	$(QUIET)echo done.

$(CVT_FILE): $(ELF_FILE)
	$(QUIET)echo Converting to $@ from $< ...
	$(QUIET)if [ ! -d $(BIN_DIR_NAME) ] ; then mkdir $(BIN_DIR_NAME); fi
	$(QUIET)$(CV) $(CVFLAGS) $< $@ 
	$(QUIET)echo done.

### Clean
clean: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=clean-self

clean-self: clean-obj-self
	$(QUIET)if [ -d $(BIN_DIR_NAME) ] ; then rm -rf $(BIN_DIR_NAME); fi
	$(QUIET)if [ -f $(LIB_FILE) ] ; then rm -f $(LIB_FILE); fi
	$(QUIET)if [ -f $(LIB_INFO_FILE) ] ; then rm -f $(LIB_INFO_FILE); fi

clean-obj: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=clean-obj-self

clean-obj-self: 
	$(QUIET)if [ -d $(OBJ_DIR_NAME) ] ; then rm -rf $(OBJ_DIR_NAME); fi

clean-elf: 
	$(QUIET)if [ -f $(ELF_FILE) ] ; then rm -f $(ELF_FILE); fi

distclean: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=distclean-self

### Depends
depends: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=depends-self

### Message
message: 
	$(QUIET)$(MAKE) recursive-target PRE_TARGET=message-self

message-self: 
ifneq "$(MESSAGE)" ""
	$(QUIET)echo $(MESSAGE)
endif
