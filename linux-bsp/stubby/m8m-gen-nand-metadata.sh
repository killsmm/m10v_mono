#!/bin/bash
# This file includes nand.par,SDRAM.PAR in "20140826_SD_writer".
#
#
usage()
{
cat << EOT
[M7M] parameter file generator for internal boot loader
$0 <DATA.bin> [<output dir>]

CAUTION:
 - following files are over written
  ./nand.par
  ./nand.par.in
  ./SDRAM.PAR

EOT
}

if [ ! -z "$2" ] ; then
	cd $2
fi

if [ ! -f "$1" ]; then
	echo "$0: \"$1\" not found" >&2
	exit 1
fi

echo target file: $1
# Page size in bytes = 2[KiB/page] x 64[page]
sz_nand_blk=$(( 64 * 2048 ))
# size of mtd0(2nd boot). This area reserved for boot-main/spare/spare.
sz_code_area=$(( 25 * 1024 * 1024))
# SDRAM load address
dram_load_addr=0x41000000
dram_binsize=$(stat --printf "%s" $1)
dram_code_size=$(printf "0x%08x" $dram_binsize)
#dram_load_size=0x0000005C
dram_load_size=$(printf "0x%08x" $(( ( ( $dram_binsize + $sz_nand_blk - 1 ) / $sz_nand_blk) )) )
device_code_size=$(printf "0x%08x" $(( ( $sz_code_area + $sz_nand_blk - 1 ) / $sz_nand_blk )))
dram_code_jump_address=0x41000000

ecc_enable=0x00000001
ecc_correction=0x00000008
#spare_area_skip_bytes=0x00000102
spare_area_skip_bytes=0x00000000


################################
# generate "nand.par"
cat > ./nand.par.in << EOT
0x00000000
$dram_load_addr
$dram_load_size
0x00000000
$dram_load_addr
$dram_load_addr
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x03000000
0x00000009
0x00000005
0x00000005
0x00000001
0x00000003
0x0000143F
0x0000003F
0x00001432
0x0000003F
0x00000000
0x00000000
$ecc_enable
$ecc_correction
0x00000000
0x00000000
$spare_area_skip_bytes
0x0000FFFF
0x00000000
P37
P41
0x00000000
0x00000000
0x00000000
$device_code_size
0x00000000
EOT

echo -ne ""  > ./nand.par
while read hoge ; do
  echo -ne "$hoge\r\n" >> ./nand.par
done < ./nand.par.in
rm -f ./nand.par.in


echo -ne "" > ./SDRAM.PAR

while read hoge; do
  echo -ne "$hoge\r\n" >> ./SDRAM.PAR
done << EOT
0x00000000
0x00000060
0x00000001
0x00010101
0x00000004
0x00000011
0x00002683
0x0000000f
0x00000009
0x00004CB5
0x0000000f
0x00000001
0x00000A50
0x00000004
0x00000010
0x02030100
0x00028499
0x00000059
0x00050501
0x0E020B04
0x00000004
0x020A0A07
0x02060205
0x00050055
0x0000007F
0x00010002
0x000200FF
0x00000008
0x00090000
0x00090006
0x00000A0E
0x02849880
0x00000000
0x00000B50
0x00000004
0x00000010
0x00000000
0x00000006
0x00000005
0x00000005
0x00000006
0x00000003
0x0003020C
0x00000012
0x01066406
0x00003613
0x00000022
0x00000022
0x00004555
0x00300036
0x01010004
0x0005B03E
0x0005803B
0x0001904F
0x0001905A
0x00000300
0x00000300
0x00000001
0x00000001
0x00000001
0x00000000
0x0007E00E
0x0007B00B
0x000000CF
0x000000DA
0x00000003
0x00010020
0x00010020
0x00010020
0x00010020
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x00000000
0x0000000F
0x0000000F
0x0000000F
0x0000000F
0x00000001
EOT

