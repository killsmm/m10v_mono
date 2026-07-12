#!/bin/sh

usage()
{
    cat <<EOF

Usage:
    ipcu.sh [command] [arguments..]

command:
    ts
    smartcmd
    test_ipcu
    wifitest

EOF
}

if [ $# = 0 ]; then
    usage;
    exit 1;
fi

case "$1" in 
    "ts")
        ;;
    "smartcmd")
        ;;
    "test_ipcu")
        ;;
    "wifitest")
        ;;
    *)
        usage
        exit 1
        ;;
esac

CMD=$1

LD_LIBRARY_PATH=/usr/lib $CMD $2 $3 $4 $5 $6 $7 $8 $9
