.SUFFIX: .a .axf .map .bin .scat .via .cfg .o .objs .txt

### The Name of Project
PRJ_NAME       := CheckHeaderLib

### ARM core (used by option)
ARMCORE        := Cortex-A5
###ARMCORE        := ARM946E-S
export ARMCORE

### Endian (used by option)
ENDIAN         := li
export ENDIAN

### Root Directory of the Project
ROOT_DIR       := ..

### The Name of Directory
OBJ_DIR_NAME   := obj
PRJ_DIR_NAME   := Project
export PRJ_DIR_NAME
BIN_DIR_NAME   := bin
LIB_DIR_NAME   := $(ROOT_DIR)\..\lib

### The List of Sub-Directory which have Makefile
SUBDIRS        := $(ROOT_DIR)\System

### Library
LIBS           := 

### File (related to armlink)
ELF_FILE        = $(BIN_DIR_NAME)\$(PRJ_NAME).axf
LIST_FILE       = $(BIN_DIR_NAME)\$(PRJ_NAME).map
SCAT_FILE       = $(PRJ_NAME).scat
LD_VIA_FILE     = armlink.via

### File (related to fromelf)
CVT_FILE        = $(BIN_DIR_NAME)\$(PRJ_NAME).bin

### Linker
LD             := armlink
LDFLAGS         = --list $(LIST_FILE) --via $(LD_VIA_FILE) --scatter $(SCAT_FILE)
OBJS            = $(patsubst %.o, $(ROOT_DIR)\\%.o, $(shell for %%d in ($(OBJ_DIR_NAME)\*.objs) do type %%d))

### Conveting Tool
CV             := fromelf
CVFLAGS         = --bin

### Target
DEFAULT_TARGET := target

QUIET := @

.PHONY: default all target clean clean-obj recursive-target $(SUBDIRS) \
        target-self clean-self clean-obj-self convert clean-elf

default: $(DEFAULT_TARGET)

all: clean target convert

target: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=target-self
	
clean: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=clean-self
	
clean-obj: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=clean-obj-self

recursive-target: $(PRE_TARGET) $(SUBDIRS) $(POST_TARGET)

ifneq "$(SUBDIRS)" ""
$(SUBDIRS): 
	$(QUIET)$(MAKE) recursive-target -C $@
endif

clean-elf: 
	$(QUIET)if exist $(ELF_FILE) del /f /q $(ELF_FILE)

target-self: $(CVT_FILE)

$(ELF_FILE): $(OBJS) $(SCAT_FILE) $(LD_VIA_FILE) $(LIBS)
	$(QUIET)if not exist $(BIN_DIR_NAME) mkdir $(BIN_DIR_NAME)
	$(QUIET)echo Creating $@ ...
	$(QUIET)$(LD) $(OBJS) $(LDFLAGS) $(LIBS) -o $@
	$(QUIET)echo done.

clean-self: clean-obj-self
	$(QUIET)if exist $(BIN_DIR_NAME) rd /s /q $(BIN_DIR_NAME)

clean-obj-self: 
	$(QUIET)if exist $(OBJ_DIR_NAME) rd /s /q $(OBJ_DIR_NAME)

convert: $(CVT_FILE)

$(CVT_FILE): $(ELF_FILE)
	$(QUIET)echo Converting to $@ from $< ...
	$(QUIET)if not exist $(BIN_DIR_NAME) mkdir $(BIN_DIR_NAME)
	$(QUIET)$(CV) $(CVFLAGS) --output=$@ $<
	$(QUIET)echo done.

