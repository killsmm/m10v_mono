#!/bin/bash

CURNTDIR=`pwd`

AddConfig() {
#  sed -i "15a \ \ $CURNTDIR/meta-openembedded/meta-oe \\\\" $BBLAYER
  sed -i "11a \ \ $CURNTDIR/meta-openembedded/meta-oe \\\\\n  $CURNTDIR/meta-openembedded/meta-filesystems \\\\\n  $CURNTDIR/meta-mlb01-$TARGET \\\\\n $CURNTDIR/meta-estone \\\\\n $CURNTDIR/meta-openembedded/meta-qt4 \\\\" $BBLAYER
  sed -i 's/MACHINE ??= "qemux86"/MACHINE ?= "'$CONFIG'"/' $LOCAL
  sed -i 's/DISTRO ?= "poky"/DISTRO ?= "estone"/' $LOCAL
  
  ADDLINE=`wc -l $LOCAL | awk '{ print $1 }'`
  echo "$ADDLINE"
  sed -i "${ADDLINE}a BB_NUMBER_THREADS = \"$THREADS\"\nPARALLEL_MAKE = \"-j $PARALLEL\"" $LOCAL
  
}

ModifyConfig() {
    echo "ModifyConfig!!!!!!"
  echo $LOCAL
  sed -i '/BB_NUMBER_THREADS =/d' $LOCAL
  sed -i '/PARALLEL_MAKE =/d' $LOCAL
  ADDLINE=$(wc -l $LOCAL | awk '{ print $1 }')
  sed -i "${ADDLINE}a BB_NUMBER_THREADS = \"$THREADS\"\nPARALLEL_MAKE = \"-j $PARALLEL\"" $LOCAL
  MODLINE=$(grep "MACHINE ?= \"mlb01" -n $LOCAL | cut -d ":" -f 1)
  echo ${MODLINE}
  sed -i -e "${MODLINE},${MODLINE}d" $LOCAL
  sed -i "${MODLINE}i MACHINE ?= \"$CONFIG\"" $LOCAL
  echo "ModifyConfig END!!!!!!"
}

Usage()
{
  echo "Usage:" 1>&2
  echo "      build_poky.sh <Param1> <Param2> <Param3> <Param4>" 1>&2
  echo "      Parameter1: linux / rtos" 1>&2
  echo "      Parameter2: Number threads 1~16" 1>&2
  echo "      Parameter3: Parallel make  1~16" 1>&2
  echo "      Parameter4: To specify when necessary Tiny-rootfs or Surveillance-rootfs" 1>&2
  echo "                  In the case of Tiny -tiny" 1>&2
  echo "                  In the case of Surveillance -surv" 1>&2
  echo "" 1>&2
  echo "Example:" 1>&2
  echo "      ./build_poky.sh rtos 8 8" 1>&2
  echo "Example: Tiny-rootfs" 1>&2
  echo "      ./build_poky.sh linux 8 8 -tiny" 1>&2
  echo "" 1>&2
}

case $# in
  3 ) {
      CONFIG=mlb01-sdk
      if [ $1 = linux ]; then
        echo "Not support....."
        exit
      fi
  };;
  4 ) {
    if [ $4 = -tiny ]; then
      CONFIG=mlb01-tiny
    elif [ $4 = -surv ]; then
      CONFIG=mlb01-surv
    else
      Usage
      exit
    fi
  };;
  * ) {
    Usage
    exit
  };;
esac

case $1 in
  "linux" | "rtos" ) {
    TARGET=$1
  };;
  * ) {
    Usage
    exit
  };;
esac

if [ 0 -lt $2 -a  $2 -lt 17 ]; then
  THREADS=$2
else
  Usage
  exit
fi

if [ 0 -lt $3 -a  $3 -lt 17 ]; then
  PARALLEL=$3
else
  Usage
  exit
fi

if [ -e $CURNTDIR/meta-openembedded ]; then
  echo "meta-openembedded OK"
else
  echo "git clone [meta-openembedded]"
  git clone -b morty http://git.openembedded.org/meta-openembedded
fi

source ./oe-init-build-env build-mlb01-$TARGET

BBLAYER=$CURNTDIR/build-mlb01-$TARGET/conf/bblayers.conf
LOCAL=$CURNTDIR/build-mlb01-$TARGET/conf/local.conf

grep -q "meta-mlb01-" $BBLAYER
if [ $? -eq 0 ]; then
  #Pre-Build
  ModifyConfig
  MSGFLG=1
else
  #First
  AddConfig
  MSGFLG=0
fi

if [ $MSGFLG -eq 1 ]; then
  echo "It takes a very long time to build."
fi
SECONDS=0
cd $CURNTDIR/build-mlb01-$TARGET
#bitbake -c cleansstate core-image-minimal
bitbake core-image-minimal
if [ $SECONDS -ge 3600 ]; then
  echo "Long time, it was cheers for good work."
fi
