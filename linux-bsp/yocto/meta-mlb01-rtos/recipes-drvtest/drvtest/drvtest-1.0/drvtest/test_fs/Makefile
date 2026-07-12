CROSSBASE:=/opt/poky/2.2/sysroots
CROSS:=$(CROSSBASE)/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-

name := test_fs
src_c := test_fs.c

sources = $(src_c)
objects := $(subst .c,.o,$(sources))

CFLAGS:=-Wall -Werror -O4 \
	--sysroot=$(CROSSBASE)/cortexa7-poky-linux-gnueabi/ \

CFLAGS += -I./ -I../include
CFLAGS += -g -Wno-unused-result

.PHONY: all
all: $(name)
$(name): $(src_c)
	$(CROSS)gcc $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	$(RM) $(objects) $(name)
