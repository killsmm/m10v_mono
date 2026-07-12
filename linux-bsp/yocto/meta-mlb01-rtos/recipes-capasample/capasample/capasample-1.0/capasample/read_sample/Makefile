CROSSBASE:=/opt/poky/2.2/sysroots
CROSS:=$(CROSSBASE)/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-

name := capa_read_sample
src_c := capa_read_sample.c

sources = $(src_c)
objects := $(subst .c,.o,$(sources))

#CFLAGS:=-Wall -Werror -O4

CFLAGS := -Wall -Werror -g3 -O0
CFLAGS += --sysroot=$(CROSSBASE)/cortexa7-poky-linux-gnueabi/
CFLAGS += -I./ -I../include
CFLAGS += -g -Wno-unused-result
CFLAGS += -I${STAGING_INCDIR}/capaInfo
LDFLAGS = -L$(STAGING_LIBDIR)/capaInfo -Wl,-rpath-link,$(STAGING_LIBDIR)/capaInfo
LDFLAGS += -L$(STAGING_LIBDIR)/ipcu -Wl,-rpath-link,$(STAGING_LIBDIR)/ipcu
LDFLAGS += -lcapaInfo -lipcustream

.PHONY: all
all: $(name)
$(name): $(src_c)
	$(CROSS)gcc $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) $(objects) $(name)
