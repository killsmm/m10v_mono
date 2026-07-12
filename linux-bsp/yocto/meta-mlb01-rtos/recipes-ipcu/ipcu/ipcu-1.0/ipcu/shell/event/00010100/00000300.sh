#!/bin/sh

NBB_JSON=/tmp/NBB.json

#echo "bad block number : $1" > /dev/ttyUSI0

echo '' > $NBB_JSON
echo '{' >> $NBB_JSON
echo '    "num_bad_block": "'$1'"' >> $NBB_JSON
echo '}' >> $NBB_JSON
