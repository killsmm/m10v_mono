.SUFFIX: .a .o .objs .txt



#
# Project and ARM settings
#

### The Name of Project
PRJ_NAME        := BaseFW
export PRJ_NAME

### ARM core (used by option)
###ARMCORE         := Cortex-A5
ARMCORE         := cortex-a7
export ARMCORE

TCHAIN  := $(GNUarm)

### Feedback
IS_USE_FEEDBACK ?=
export IS_USE_FEEDBACK

### CPU STATE
IS_ARM_STATE    ?=
export IS_ARM_STATE

### C source -> ASM
IS_GET_ASM      ?=
export IS_GET_ASM

##### REMOVE_PGRELIEF BEGIN #####
### PGRelief
IS_USE_PGRELIEF ?=
export IS_USE_PGRELIEF
##### REMOVE_PGRELIEF END #####



#
# Directory and File names
#

### Root Directory of the Project
ROOT_DIR        := ../..

### The Name of Directory
OBJ_DIR_NAME    := obj
BIN_DIR_NAME    := bin
LIB_DIR_NAME    := lib
WS_DIR_NAME     := basefw
export WS_DIR_NAME

### The List of Sub-Directory which have Makefile
# delete_basefw\custom
#SUBDIRS         := $(ROOT_DIR)/cpu_tkernel/$(WS_DIR_NAME)/custom
SUBDIRS         += $(ROOT_DIR)/cpu_tkernel/$(WS_DIR_NAME)/fj
SUBDIRS         += $(ROOT_DIR)/cpu_tkernel/$(WS_DIR_NAME)/ip
SUBDIRS         += $(ROOT_DIR)/cpu_tkernel/$(WS_DIR_NAME)/milb_api_usercustom

### Libraries
LIBS             =

### Objects
OBJS             = $(patsubst %.o, %.o, $(shell cat $(OBJ_DIR_NAME)/*.objs 2>/dev/null))

### File (related to armar)
LIB_FILE        := $(LIB_DIR_NAME)/$(PRJ_NAME).a
LIB_INFO_FILE   := $(LIB_DIR_NAME)/$(PRJ_NAME).txt



#
# Toolchain
#

### Librarian
AR       := $(TCHAIN)ar
ARFLAGS  := -r



#
# Targets
#

DEFAULT_TARGET := lib

QUIET :=

.PHONY: default all recursive-target target-self lib lib-self distclean-self depends-self $(SUBDIRS) \
        target clean clean-self clean-obj clean-obj-self distclean config depends message message-self

default: $(DEFAULT_TARGET)

all: clean depends $(DEFAULT_TARGET)

recursive-target: $(PRE_TARGET) $(SUBDIRS) $(POST_TARGET)

target-self: lib-self

lib: depends target lib-self

lib-self: $(LIB_FILE)

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

### Clean
clean: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=clean-self

clean-self: clean-obj-self
	$(QUIET)if [ -f $(LIB_FILE) ] ; then rm -f $(LIB_FILE); fi
	$(QUIET)if [ -f $(LIB_INFO_FILE) ] ; then rm -f $(LIB_INFO_FILE); fi

clean-obj: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=clean-obj-self

clean-obj-self: 
	$(QUIET)if [ -d $(OBJ_DIR_NAME) ] ; then rm -rf $(OBJ_DIR_NAME); fi

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

