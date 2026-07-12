#!/bin/sh

#HASH_TYPE=-sha1
HASH_TYPE=-sha256

if [ -z "$1" ] ; then
	echo "Usage: $0 <file to create sig file for> [...] > output.sig"
	exit 1
fi

rm -f .payload
while (( "$#" )); do
	cat "$1" >> .payload
	shift
done

openssl dgst $HASH_TYPE .payload > hash
cat hash | sed 's|(.*/|(|g' > hash.1
openssl rsautl -sign -inkey privkey.pem -keyform PEM -in hash.1 > .payload.sig
#openssl rsautl -verify -inkey pubkey.der  -keyform DER -pubin -in .payload.sig
cat .payload.sig

