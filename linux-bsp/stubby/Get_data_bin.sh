#!/bin/sh
printb() { # num (0-255)
  /bin/echo -ne "\\x"`printf "%02x" $1`
}

addHeader() { # type param file
  TYPE=$1
  PARAM=$2
  INFILE=$3

  # Check parameters
  case $TYPE in
    0)
      case $PARAM in 0|1|2) ;; *) return 1 ;; esac ;;
    4)
      test $PARAM -lt 0 -o $PARAM -gt 15 && return 1 ;;
    6)
      test $PARAM -lt 0 -o $PARAM -gt 15 -a $PARAM -ne 255 && return 1 ;;
    16)
      case $PARAM in 0|1) ;; *) return 1 ;; esac ;;
    1|2|3)
      ;;
    *)
      return 1;;
  esac

  [ ! -f $INFILE ] && return 1
  LEN=`stat -c%s $INFILE`
  if [ $TYPE -eq 3 ]; then
    PADLEN=`echo "( 2048 - $LEN % 2048 ) % 2048" | bc`
  else
    PADLEN=`echo "( 16 - $LEN % 16 ) % 16" | bc` # 0 - 15
  fi
  ALGLEN=$((LEN + PADLEN))

  echo -n "MILBEAUT"
  printb $TYPE ; printb 0
  printb $PARAM ; printb 0
  printb $((ALGLEN % 256))
  printb $((ALGLEN / 256 % 256))
  printb $((ALGLEN / 65536 % 256))
  printb $((ALGLEN / 16777216 % 256))
  cat $INFILE
  while [ $PADLEN -ne 0 ]; do
    printb 0; PADLEN=$((PADLEN - 1))
  done
}

if [ $# -lt 2 ]; then
	echo "$0 P1 P2"
	echo "P1: sc2000/stubby-sc2000.bin"
	echo "P2: 1=ES1 2=ES3"
	exit 1
fi
echo "Start make DATA.BIN"
if [ $2 -eq 1 ]; then
    M0=RS_MILBCM0.bin
else
    M0=RS_MILBCM0-ES3.bin
fi
echo "M0 is $M0"
cp -p $1 stubby.bin
     (addHeader 3 0 $M0
	addHeader 4 0 stubby.bin) > DATA.bin

echo ""
echo "Done"

exit


