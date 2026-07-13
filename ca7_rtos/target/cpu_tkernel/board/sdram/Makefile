### Root Directory of the Project
ROOT_DIR     := ../../..

### The current path from Root Directory.
DIR          := board/sdram

### The List of Sub-Directory which have Makefile
### note: If current directory does not have the sub-directory, Variable "SUBDIR" is empty.
SUBDIRS      := 

### List of source files
### note: If current directory does not have C/ASM source files in "src" directory, CSRC/ASRC is empty.
CSRC :=
CSRC += sdram_map_common.c
CSRC += sdram_map_movie_play.c 
CSRC += sdram_map_osd.c
CSRC += sdram_map_play.c
#CSRC += sdram_map_play2.c
#CSRC += sdram_map_view.c
#CSRC += sdram_map_fshd.c
CSRC += sdram_map_manager.c
CSRC += sdram_map_share.c

ifeq "$(IS_CHIP_ES)" "chip_es3"
CSRC += sdram_map_movie_record_es3.c
else
CSRC += sdram_map_movie_record_es1.c
endif

ASRC :=
ASRC += mmu.s





include $(ROOT_DIR)/common.mk





### Special compile/assemble
