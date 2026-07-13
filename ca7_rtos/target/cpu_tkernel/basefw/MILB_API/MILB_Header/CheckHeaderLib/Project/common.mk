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
ARMCORE     ?= Cortex-A5

### Endian (used by option)
ENDIAN      ?= li



#
# Include paths
#

### Used by the option of armcc
CINCPATH   :=
CINCPATH   += -I$(ROOT_DIR)\..\include
CINCPATH   += -I$(ROOT_DIR)\..\include\Arm
CINCPATH   += -I$(ROOT_DIR)\..\include\Dsp
CINCPATH   += -I$(ROOT_DIR)\..\include\Exs
CINCPATH   += -I$(ROOT_DIR)\..\include\Image
CINCPATH   += -I$(ROOT_DIR)\..\include\Image\PRO
CINCPATH   += -I$(ROOT_DIR)\..\include\Peri
CINCPATH   += -I$(ROOT_DIR)\..\include\Top
CINCPATH   += $(SUB_CINCPATH)
CPPINCPATH :=

### Used by the option of armasm
AINCPATH   :=



#
# Toolchain options
#

### Debug Information
DEBUG_INFO       := -g

### Compiler
CC               := armcc
CFLAGS_COMMON     = -c
CFLAGS_COMMON    += --cpu=$(ARMCORE)
CFLAGS_COMMON    += --$(ENDIAN)
CFLAGS_COMMON    += --strict_warnings
CFLAGS_COMMON    += --dollar
CFLAGS_COMMON    += --C99
CFLAGS_COMMON    += --diag_suppress=230
ifeq "$(IS_USE_FEEDBACK)" "y"
CFLAGS_COMMON    += --feedback=$(ROOT_DIR)\$(WS_DIR_NAME)\$(FEEDBACK_FILE)
CFLAGS_COMMON    += --diag_suppress=C3052
endif
CFLAGS            = $(CFLAGS_COMMON)
CFLAGS           += $(DEBUG_INFO)
CFLAGS_DEPENDS    = $(CFLAGS_COMMON)
CFLAGS_DEPENDS   += --no_code_gen
CFLAGS_DEPENDS   += --no_depend_system_headers
CPPFLAGS          =
CPPFLAGS_DEPENDS  =

### Assember
AS               := armasm
AFLAGS_COMMON     = --cpu=$(ARMCORE)
AFLAGS_COMMON    += --$(ENDIAN)
AFLAGS_COMMON    += --keep
AFLAGS_COMMON    += --apcs=/interwork
AFLAGS_COMMON    += --cpreproc
AFLAGS            = $(AFLAGS_COMMON)
AFLAGS           += $(DEBUG_INFO)
AFLAGS_DEPENDS    = $(AFLAGS_COMMON)
AFLAGS_DEPENDS   += --no_code_gen
AFLAGS_DEPENDS   += --cpreproc



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
CUROBJSFILE       = $(ROOT_DIR)\$(WS_DIR_NAME)\$(OBJ_DIR_NAME)\$(strip $(subst \,_, $(DIR))).objs
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

QUIET := @

.PHONY: default target-self recursive-target depends-self clean-self distclean-self $(SUBDIRS) \
        target target-objs-self clean clean-obj clean-obj-self clean-depends-self depends message message-self

default: target

target-self: target-objs-self $(OBJS)

recursive-target: $(PRE_TARGET) $(SUBDIRS) $(POST_TARGET)

depends-self: clean-depends-self $(DEPS)

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
	$(QUIET)$(MAKE) recursive-target POST_TARGET=target-self IS_INCLUDE_DEPS=y

target-objs-self: 
ifneq "$(words $(ALL_SRC))" "0"
	$(QUIET)if not exist $(ROOT_DIR)\$(WS_DIR_NAME)\$(OBJ_DIR_NAME) mkdir $(ROOT_DIR)\$(WS_DIR_NAME)\$(OBJ_DIR_NAME)
	$(QUIET)if exist $(CUROBJSFILE) del /f $(CUROBJSFILE)
	$(QUIET)echo $(patsubst $(OBJ_DIR_NAME)/%.o, $(DIR)\$(OBJ_DIR_NAME)\\%.o, $(OBJS)) > $(CUROBJSFILE)
endif

$(OBJ_FILE): $(SRC_CFILE)
	$(QUIET)if not exist $(OBJ_DIR_NAME) mkdir $(OBJ_DIR_NAME)
	$(QUIET)echo compiling $< ...
	$(QUIET)$(CC) $(CFLAGS) $(CINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)

$(OBJ_FILE): $(SRC_CPPFILE)
	$(QUIET)if not exist $(OBJ_DIR_NAME) mkdir $(OBJ_DIR_NAME)
	$(QUIET)echo compiling $< ...
	$(QUIET)$(CC) $(CPPFLAGS) $(CPPINCPATH) -o $@ $(call PATH_CONVERT, $(CURDIR)/$<)

$(OBJ_FILE): $(SRC_AFILE)
	$(QUIET)if not exist $(OBJ_DIR_NAME) mkdir $(OBJ_DIR_NAME)
	$(QUIET)echo assembling $< ...
	$(QUIET)$(AS) $(AFLAGS) $(AINCPATH) -o $@ "$(CURDIR)/$<"

### Clean
clean: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=clean-self

clean-obj: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=clean-obj-self

clean-obj-self: 
	$(QUIET)if exist $(OBJ_DIR_NAME) rd /s /q $(OBJ_DIR_NAME)

clean-depends-self: 
	$(QUIET)if exist $(DEP_DIR_NAME) rd /s /q $(DEP_DIR_NAME)

### Depends
depends: 
	$(QUIET)$(MAKE) recursive-target POST_TARGET=depends-self

$(DEP_FILE): $(SRC_CFILE)
	$(QUIET)if not exist $(DEP_DIR_NAME) mkdir $(DEP_DIR_NAME)
	$(QUIET)echo creating $@ ...
	$(QUIET)$(CC) $(CFLAGS_DEPENDS) $(CINCPATH) --depend=$@ $< -o $(patsubst $(SRC_DIR_NAME)/%.c,$(OBJ_DIR_NAME)/%.o,$<)

$(DEP_FILE): $(SRC_CPPFILE)
	$(QUIET)if not exist $(DEP_DIR_NAME) mkdir $(DEP_DIR_NAME)
	$(QUIET)echo creating $@ ...
	$(QUIET)$(CC) $(CPPFLAGS_DEPENDS) $(CPPINCPATH) --depend=$@ $< -o $(patsubst $(SRC_DIR_NAME)/%.cpp,$(OBJ_DIR_NAME)/%.o,$<)

$(DEP_FILE): $(SRC_AFILE)
	$(QUIET)if not exist $(DEP_DIR_NAME) mkdir $(DEP_DIR_NAME)
	$(QUIET)echo creating $@ ...
	$(QUIET)$(AS) $(AFLAGS_DEPENDS) $(AINCPATH) --depend=$@ $< -o $(patsubst $(SRC_DIR_NAME)/%.s,$(OBJ_DIR_NAME)/%.o,$<)

### Message
message: 
	$(QUIET)$(MAKE) recursive-target PRE_TARGET=message-self

message-self: 
ifneq "$(MESSAGE)" ""
	$(QUIET)echo $(MESSAGE)
endif

