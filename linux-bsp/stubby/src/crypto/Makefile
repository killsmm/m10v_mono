# Stubby Makefile
# Copyright (C) 2014 Linaro, Ltd
# Author: Andy Green <andy.green@linaro.org>
#

OUT=../$(O)/crypto/crypto.a

OBJS := asn1.o \
	bigint.o \
	crypto_misc.o \
	rsa.o \
	stubby-api.o \
	sha1.o \
	sha256.o
	
OBJECTS=$(patsubst %, ../$(O)/crypto/%, $(OBJS))

all:	$(OUT)

.PHONY: clean
clean:
	rm -f ../$(O)/*.o $(OUT)
	mkdir -p ../$(O)/crypto

%.o: %.c
	$(CC) -c $(CC_OPTS) $< -o ../$(O)/crypto/$@

%.o: %.S
	$(CC) -c $(CC_OPTS) $< -o ../$(O)/crypto/$@
	
$(OUT): $(OBJS)
	$(AR) rcs $(OUT) $(OBJECTS)

