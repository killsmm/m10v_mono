#!/bin/sh

#/*--------------------------------------------------------------------------*/
#/*   - First parameter($1)                                                  */
#/*       USB mode    0: USB ehci test                                       */
#/*                   1: USB xhci test                                       */
#/*                                                                          */
#/*   - Second parameter($2) Optional                                        */
#/*       Test mode   0: TEST mode notenabled                                */
#/*                   1: TEST J                                              */
#/*                   2: TEST K                                              */
#/*                   3: TEST SE0 NAK                                        */
#/*                   4: TEST Packet                                         */
#/*                   5: TEST FOCE_ENABLE                                    */
#/*                                                                          */
#/*--------------------------------------------------------------------------*/

usb20(){
	echo 0x44 > test_regs
	#//echo 0x44 0x501000 > test_regs
	#//cat test_regs
	
	txt=`cat test_regs`
	r_reg_dec=$(echo $(( 0x${txt#*0x44 0x} )))		#// HEX -> DEC
	
	#//echo $(printf '%#x\n' $r_reg_dec) "<------ Read value"
	
	case "$1" in 
		"0") hex=00000 ;;							#// TEST mode notenabled 0000b
		"1") hex=10000 ;;							#// TEST J               0001b
		"2") hex=20000 ;;							#// TEST K               0010b
		"3") hex=30000 ;;							#// TEST SE0 NAK         0011b
		"4") hex=40000 ;;							#// TEST Packet          0100b
		"5") hex=50000 ;;							#// TEST FOCE_ENABLE     0101b
		* ) echo "Not supported second parameter : $1"
			return
			;;
	esac
	
	w_dec=$(echo $(( 0x$hex )))						#// HEX -> DEC
	msk_dec=$(echo $(( 0xFFF0FFFF )))				#// HEX -> DEC (Mask value)
	w_reg_dec=$(((r_reg_dec & msk_dec) | w_dec))	#// Mask and OR
	
	#//echo $(printf '%#x\n' $w_reg_dec) "<------ Write value"
	
	echo 0x44 $w_reg_dec > test_regs
}

usb30(){
	echo 0x424 > test_regs
	#//echo 0x424 0x20000000 > test_regs
	#//cat test_regs
	
	txt=`cat test_regs`
	r_reg_dec=$(echo $(( 0x${txt#*0x424 0x} )))		#// HEX -> DEC (Read value)
	
	#//echo $(printf '%#x\n' $r_reg_dec) "<------ Read value"
	
	case "$1" in 
		"0") hex=00000000 ;;						#// TEST mode notenabled 0000b
		"1") hex=10000000 ;;						#// TEST J               0001b
		"2") hex=20000000 ;;						#// TEST K               0010b
		"3") hex=30000000 ;;						#// TEST SE0 NAK         0011b
		"4") hex=40000000 ;;						#// TEST Packet          0100b
		"5") hex=50000000 ;;						#// TEST FOCE_ENABLE     0101b
		* ) echo "Not supported second parameter : $1"
			return
			;;
	esac
	
	w_dec=$(echo $(( 0x$hex )))						#// HEX -> DEC (TEST value)
	msk_dec=$(echo $(( 0xFFFFFFF )))				#// HEX -> DEC (Mask value)
	w_reg_dec=$(((r_reg_dec & msk_dec) | w_dec))	#// Mask and OR
	
	#//echo $(printf '%#x\n' $w_reg_dec) "<------ Write value"
	
	echo 0x424 $w_reg_dec > test_regs
}


input1(){
	echo ""
	echo "*** Test mode start!"
	input2;
}

input2(){
	echo ""
	echo "Test Mode Menu : Test mode function."
	echo "---------"
	echo "  1: TEST J"
	echo "  2: TEST K"
	echo "  3: TEST SE0 NAK"
	echo "  4: TEST PACKET"
#//	echo "  5: TEST FOCE_ENABLE"
#//	echo "  6: HS_HOST_PORT_SUSPEND_RESUME"
#//	echo "  7: SINGLE_STEP_GET_DEV_DESC"
#//	echo "  8: SINGLE_STEP_SET_FEATURE"
#//	echo "  9: TTST_CONFIG"
#//	echo " 10: Reset host"
	echo " 99: Exit"
	echo ""
	echo -n "Enter option: "

	read ans

	case $ans in 
		"0" ) echo
			  echo "   Not supported"
			  input2;
			  ;;
		"1" ) para2="1"
			  ;;
		"2" ) para2="2"
			  ;;
		"3" ) para2="3"
			  ;;
		"4" ) para2="4"
			  ;;
		"5" ) echo 
			  echo "   Not supported"
			  input2;
			  ;;
		"6" ) echo 
			  echo "   Not supported"
			  input2;
			  ;;
		"7" ) echo 
			  echo "   Not supported"
			  input2;
			  ;;
		"8" ) echo
			  echo "   Not supported"
			  input2;
			  ;;
		"9" ) echo
			  echo "   Not supported"
			  input2;
			  ;;
		"10" ) echo
			  echo "   Not supported"
			  input2;
			  ;;
		"99") echo
			  echo "   End of test mode!"
			  echo
			  exit 0;
			  ;;
		   *) echo
			  echo "   ERROR : invalid command !"
			  input2;
			  ;;
	esac
}


if [ $# = 0 ]; then
	echo " No parameter..."
	exit 0;
elif [ $# = 1 ]; then
	input1;
else
	para2=$2
fi

if [ "$1" = "0" ]; then
    mount -t debugfs debugfs /sys/kernel/debug
    cd /sys/kernel/debug/usb/ehci/1b070000.usb/
    #usb20 $2;
    usb20 $para2;
elif [ "$1" = "1" ]; then
    mount -t debugfs debugfs /sys/kernel/debug
    cd /sys/kernel/debug/usb/xhci/xhci-hcd.0.auto
    #usb30 $2;
    usb30 $para2;
else
    echo "Not supported first parameter : $1"
    exit 0;
fi

echo "-----------------------" 
pwd
cat test_regs
echo "-----------------------" 
echo

cd
umount /sys/kernel/debug

exit 0;
