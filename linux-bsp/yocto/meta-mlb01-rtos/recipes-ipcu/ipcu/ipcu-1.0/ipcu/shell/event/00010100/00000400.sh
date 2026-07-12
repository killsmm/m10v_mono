#!/bin/sh

SSC_JSON=/tmp/SSC.json

#echo "sensor number : $1" > /dev/ttyUSI0

echo '' > $SSC_JSON
echo '{' >> $SSC_JSON
echo '    "sensor_status": "'$1'"' >> $SSC_JSON
echo '}' >> $SSC_JSON
