usage="
usage:
	./usr/bin/fwup_net.sh -p 0xa9343000 -s 0x3e8800 -i 0x5

parameters:
	p		physical address to be used for fw update
	s		fw size in bytes
	i		index for fw type(0x5 for LINUX.BIN, 0x6 for initramfs, 0x16 for DATA.bin"

paddr=0xa9343000
size=0
index=0

while getopts p:s:i: opt
do
  case $opt in
    "p") paddr="$OPTARG" ;;
    "s") size="$OPTARG" ;;
    "i") index="$OPTARG" ;;
  esac
done

if [ ${paddr} = 0 ] || [ ${size} = 0 ] || [ ${index} = 0 ] ; then
	echo "$usage"
	exit 1
else
	echo "start to update"
	busybox nc -l -p 3333 | zw -p ${paddr} -s ${size} -i ${index}	
fi

