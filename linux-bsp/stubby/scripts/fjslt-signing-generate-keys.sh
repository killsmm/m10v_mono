#!/bin/sh

KEYLENGTH=2048

openssl genrsa -des3 -out privkey.pem $KEYLENGTH
openssl rsa -in privkey.pem -pubout -outform DER > pubkey.der
hexdump -C pubkey.der | tr -s ' ' | cut -d'|' -f1 | sed s/^........//g | sed "s/^\ /0x/g" | sed "s/\ /\,\ 0x/g" | sed "s/0x\$//g" > pubkey.h

echo "Copy pubkey.h into ./src/pubkey.h in Stubby and rebuild"
