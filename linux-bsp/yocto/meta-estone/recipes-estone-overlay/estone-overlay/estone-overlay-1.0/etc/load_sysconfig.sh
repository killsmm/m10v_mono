#!/bin/sh

#load config in /media/config/sys.conf and export to environment

CONFIG_FILE="/media/config/sys.conf"

if cat $CONFIG_FILE > /dev/null; then
    echo "Configuration file found."
else
     cp /etc/sys.conf "$CONFIG_FILE"
    echo "Configuration file not found, use /etc/sys.conf."
fi

while IFS='=' read -r key value; do
    # Export each key-value pair as an environment variable
    export "$key=$value"
done < "$CONFIG_FILE"

ln -s "$CONFIG_FILE" /var/www/config