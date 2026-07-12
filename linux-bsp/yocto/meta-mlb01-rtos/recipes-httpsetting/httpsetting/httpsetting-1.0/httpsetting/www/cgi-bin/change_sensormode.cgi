#!/bin/sh

echo Content-type: application/json; charset=UTF-8
echo 
read QUERY_STRING

JO="jo"
JQ="jq"
CAMCOMM="camera_cmd"
TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

# Read and localize from json structure ------------------------------
# Read Camera Mode
CAMERA_MODE=$(echo "$QUERY_STRING" | $JQ ".camera_mode")
RESOLUTION=$(echo "$QUERY_STRING" | $JQ ".resolution")
ISO=$(echo "$QUERY_STRING" | $JQ ".iso")
WB_MODE=$(echo "$QUERY_STRING" | $JQ ".wb_mode")

# Read IQ.bin's ID
IQ_BIN_INDEX=$(echo "$QUERY_STRING" | $JQ ".iq_bin_index")
AE_BIN_INDEX=$(echo "$QUERY_STRING" | $JQ ".ae_bin_index")

# Set Image Configuration --------------------------------------------
# if Capturing is done.
case $CAMERA_MODE in
	"3") 
                # Set Image Capture Configuration ------------------------------------
                # IPCU Command: Set Image Capture Configuration
                # Sub Commnad : Image Type
                # Params      : 0x00(JPEG)
                echo $CAMCOMM s_i_image_type 0x0 >> $LOGF
                RESULT=$($CAMCOMM s_i_image_type 0x0)

                # IPCU Command: Set Image Capture Configuration
                # Sub Commnad : Store Channel
                # Params      : 0x01(SD1 Card)
                echo $CAMCOMM s_i_store_channel 0x1 >> $LOGF
                RESULT=$($CAMCOMM s_i_store_channel 0x1)

                # IPCU Command: Set Image Capture Configuration
                # Sub Commnad : Capture Size
                # Params      : 3000x2250 or 4000x3000 
                echo $CAMCOMM s_i_capture_size $RESOLUTION >> $LOGF
                RESULT=$($CAMCOMM s_i_capture_size $RESOLUTION)

                # IPCU Command: Set Image Capture Configuration
                # Sub Commnad : Quality
                # Params      : 0x02(High)
                echo $CAMCOMM s_i_quality 0x2 >> $LOGF
                RESULT=$($CAMCOMM s_i_quality 0x2)

                # Change Camera Mode -------------------------------------------------
                # IPCU Command: Change Camera Mode
                # Sub Commnad : Change Mode
                # Params      : Still Capture
                echo $CAMCOMM chg_camera_mode 3 >> $LOGF
                RESULT=$($CAMCOMM chg_camera_mode 3)
                
                CAMERA_MODE=2
                ;;
        "2")
                # Update resolution
                echo sh ../set_resolution.sh $CAMERA_MODE $RESOLUTION >> $LOGF
                RESULT=$(sh ../set_resolution.sh $CAMERA_MODE $RESOLUTION)
                ;;
        *)
                ;;
esac

case $CAMERA_MODE in
        "1")
                # Change Camera Mode -------------------------------------------------
                # IPCU Command: Change Camera Mode
                # Sub Commnad : Change Mode
                # Params      : UI指定値
                echo $CAMCOMM chg_camera_mode $CAMERA_MODE >> $LOGF
                RESULT=$($CAMCOMM chg_camera_mode $CAMERA_MODE)

                $JO -p Result="Done."
                ;;
	*) 
                # Change Camera Mode -------------------------------------------------
                # IPCU Command: Change Camera Mode
                # Sub Commnad : Change Mode
                # Params      : UI指定値
                echo $CAMCOMM chg_camera_mode $CAMERA_MODE >> $LOGF
                RESULT=$($CAMCOMM chg_camera_mode $CAMERA_MODE)
                
                # IPCU Command: Set Image Configuration
                # Sub Commnad : ISO
                # Params      : 0x00(Auto)
                # TODO        : UIの値が設定されるんだけど、Autoにしたいわけじゃないの？？
                echo $CAMCOMM s_i_iso $ISO >> $LOGF
                RESULT=$($CAMCOMM s_i_iso $ISO)

		# Update module
		# IPCU Command: Simple Tool
		# Sub Commnad : Update
		# Params      : video update
		echo $CAMCOMM simple_tool_update 0 >> $LOGF
		RESULT=$($CAMCOMM simple_tool_update 0)
                
                # IPCU Command: Set Image Configuration
                # Sub Commnad : White Balance (Mode)
                # Params      : 0x00(Auto), 256(x1.0), 256(x1.0)
                echo $CAMCOMM s_i_white_balance $WB_MODE 256 256 >> $LOGF
                RESULT=$($CAMCOMM s_i_white_balance $WB_MODE 256 256)

                # Read and localize IQ.bin address -----------------------------------
                # IPCU Command: Simple Tool
                # Sub Commnad : Get Address
                # Params      : 0x00(IQ), ID
                # Return      : Address
                echo $CAMCOMM -r 2 simple_tool_get_addr 0x0 $IQ_BIN_INDEX >> $LOGF
                IQ_BINX=$($CAMCOMM -r 2 simple_tool_get_addr 0x0 $IQ_BIN_INDEX)

                # IPCU Command: Simple Tool
                # Sub Commnad : Get Address
                # Params      : 0x01(AE), ID
                # Return      : Common's Address
                echo $CAMCOMM -r 2 simple_tool_get_addr 0x1 0x0 >> $LOGF
                AE_COMM=$($CAMCOMM -r 2 simple_tool_get_addr 0x1 0x0)

                # IPCU Command: Simple Tool
                # Sub Commnad : Get Address
                # Params      : 0x01(AE), ID
                # Return      : Address
                echo $CAMCOMM -r 2 simple_tool_get_addr 0x1 $AE_BIN_INDEX >> $LOGF
                AE_BINX=$($CAMCOMM -r 2 simple_tool_get_addr 0x1 $AE_BIN_INDEX)

                # IPCU Command: Simple Tool
                # Sub Commnad : Get Address
                # Params      : 0x02(AWB), ID
                # Return      : Common's Address
                echo $CAMCOMM -r 2 simple_tool_get_addr 0x2 0x0 >> $LOGF
                AWB_COMM=$($CAMCOMM -r 2 simple_tool_get_addr 0x2 0x0)

                # Read SDRAM data ----------------------------------------------------
                IQBINRD="membinrdwriq -r -m $IQ_BINX -o 216"
                IQBINRDB="membinrdwriq -r -m $IQ_BINX -o 52"
                AEBINRD0="membinrdwr -r -m $AE_COMM -o 40"
                AEBINRD1="membinrdwr -r -m $AE_BINX -o 40"
                AWBBINRD="membinrdwrawb -r -m $AWB_COMM -o 40"

                # Read IQ data

                # Read Gamma Index
                IQ_GAMMA_INDEX=$($IQBINRD -i 14 -n 1 -a 0)

                # Read Gamma Index 0 --- Gamma Off(flat)
                IQ_GAMMA_R_OPEPOS0=$($IQBINRDB -i 13 -b 0 -p 8 -n 40 -f 2 -a 0)
                IQ_GAMMA_G_OPEPOS0=$($IQBINRDB -i 13 -b 0 -p 88 -n 40 -f 2 -a 0)
                IQ_GAMMA_B_OPEPOS0=$($IQBINRDB -i 13 -b 0 -p 168 -n 40 -f 2 -a 0)
                IQ_GAMMA_Y_OPEPOS0=$($IQBINRDB -i 13 -b 0 -p 248 -n 40 -f 2 -a 0)
                # Read Gamma Index 1 --- Pattern 1
                IQ_GAMMA_R_OPEPOS1=$($IQBINRDB -i 13 -b 0 -p 12816 -n 40 -f 2 -a 0)
                IQ_GAMMA_G_OPEPOS1=$($IQBINRDB -i 13 -b 0 -p 12896 -n 40 -f 2 -a 0)
                IQ_GAMMA_B_OPEPOS1=$($IQBINRDB -i 13 -b 0 -p 12976 -n 40 -f 2 -a 0)
                IQ_GAMMA_Y_OPEPOS1=$($IQBINRDB -i 13 -b 0 -p 13056 -n 40 -f 2 -a 0)

                IQ_GAMMA_R_FULL1=$($IQBINRDB -i 13 -b 0 -p 23568 -n 256 -f 2 -a 0)
                IQ_GAMMA_G_FULL1=$($IQBINRDB -i 13 -b 0 -p 24080 -n 256 -f 2 -a 0)
                IQ_GAMMA_B_FULL1=$($IQBINRDB -i 13 -b 0 -p 24592 -n 256 -f 2 -a 0)
                IQ_GAMMA_Y_FULL1=$($IQBINRDB -i 13 -b 0 -p 25104 -n 256 -f 2 -a 0)

                IQ_GAMMA_R_DIFF1=$($IQBINRDB -i 13 -b 0 -p 14864 -n 512 -f 4 -a 0)
                IQ_GAMMA_G_DIFF1=$($IQBINRDB -i 13 -b 0 -p 16912 -n 512 -f 4 -a 0)
                IQ_GAMMA_B_DIFF1=$($IQBINRDB -i 13 -b 0 -p 18960 -n 512 -f 4 -a 0)
                IQ_GAMMA_Y_DIFF1=$($IQBINRDB -i 13 -b 0 -p 21008 -n 512 -f 4 -a 0)

                # Read Gamma Index 2 --- Pattern 2
                IQ_GAMMA_R_OPEPOS2=$($IQBINRDB -i 13 -b 0 -p 25624 -n 40 -f 2 -a 0)
                IQ_GAMMA_G_OPEPOS2=$($IQBINRDB -i 13 -b 0 -p 25704 -n 40 -f 2 -a 0)
                IQ_GAMMA_B_OPEPOS2=$($IQBINRDB -i 13 -b 0 -p 25784 -n 40 -f 2 -a 0)
                IQ_GAMMA_Y_OPEPOS2=$($IQBINRDB -i 13 -b 0 -p 25864 -n 40 -f 2 -a 0)

                IQ_GAMMA_R_FULL2=$($IQBINRDB -i 13 -b 0 -p 36376 -n 256 -f 2 -a 0)
                IQ_GAMMA_G_FULL2=$($IQBINRDB -i 13 -b 0 -p 36888 -n 256 -f 2 -a 0)
                IQ_GAMMA_B_FULL2=$($IQBINRDB -i 13 -b 0 -p 37400 -n 256 -f 2 -a 0)
                IQ_GAMMA_Y_FULL2=$($IQBINRDB -i 13 -b 0 -p 37912 -n 256 -f 2 -a 0)

                IQ_GAMMA_R_DIFF2=$($IQBINRDB -i 13 -b 0 -p 27672 -n 512 -f 4 -a 0)
                IQ_GAMMA_G_DIFF2=$($IQBINRDB -i 13 -b 0 -p 29720 -n 512 -f 4 -a 0)
                IQ_GAMMA_B_DIFF2=$($IQBINRDB -i 13 -b 0 -p 31768 -n 512 -f 4 -a 0)
                IQ_GAMMA_Y_DIFF2=$($IQBINRDB -i 13 -b 0 -p 33816 -n 512 -f 4 -a 0)

                # Read Gamma Index 3 --- Pattern 3
                IQ_GAMMA_R_OPEPOS3=$($IQBINRDB -i 13 -b 0 -p 38432 -n 40 -f 2 -a 0)
                IQ_GAMMA_G_OPEPOS3=$($IQBINRDB -i 13 -b 0 -p 38512 -n 40 -f 2 -a 0)
                IQ_GAMMA_B_OPEPOS3=$($IQBINRDB -i 13 -b 0 -p 38592 -n 40 -f 2 -a 0)
                IQ_GAMMA_Y_OPEPOS3=$($IQBINRDB -i 13 -b 0 -p 38672 -n 40 -f 2 -a 0)

                IQ_GAMMA_R_FULL3=$($IQBINRDB -i 13 -b 0 -p 49184 -n 256 -f 2 -a 0)
                IQ_GAMMA_G_FULL3=$($IQBINRDB -i 13 -b 0 -p 49696 -n 256 -f 2 -a 0)
                IQ_GAMMA_B_FULL3=$($IQBINRDB -i 13 -b 0 -p 50208 -n 256 -f 2 -a 0)
                IQ_GAMMA_Y_FULL3=$($IQBINRDB -i 13 -b 0 -p 50720 -n 256 -f 2 -a 0)

                IQ_GAMMA_R_DIFF3=$($IQBINRDB -i 13 -b 0 -p 40480 -n 512 -f 4 -a 0)
                IQ_GAMMA_G_DIFF3=$($IQBINRDB -i 13 -b 0 -p 42528 -n 512 -f 4 -a 0)
                IQ_GAMMA_B_DIFF3=$($IQBINRDB -i 13 -b 0 -p 44576 -n 512 -f 4 -a 0)
                IQ_GAMMA_Y_DIFF3=$($IQBINRDB -i 13 -b 0 -p 46624 -n 512 -f 4 -a 0)

                # Read Gamma Index 4 --- Pattern 4
                IQ_GAMMA_R_OPEPOS4=$($IQBINRDB -i 13 -b 0 -p 51240 -n 40 -f 2 -a 0)
                IQ_GAMMA_G_OPEPOS4=$($IQBINRDB -i 13 -b 0 -p 51320 -n 40 -f 2 -a 0)
                IQ_GAMMA_B_OPEPOS4=$($IQBINRDB -i 13 -b 0 -p 51400 -n 40 -f 2 -a 0)
                IQ_GAMMA_Y_OPEPOS4=$($IQBINRDB -i 13 -b 0 -p 51480 -n 40 -f 2 -a 0)

                IQ_GAMMA_R_FULL4=$($IQBINRDB -i 13 -b 0 -p 61992 -n 256 -f 2 -a 0)
                IQ_GAMMA_G_FULL4=$($IQBINRDB -i 13 -b 0 -p 62504 -n 256 -f 2 -a 0)
                IQ_GAMMA_B_FULL4=$($IQBINRDB -i 13 -b 0 -p 63016 -n 256 -f 2 -a 0)
                IQ_GAMMA_Y_FULL4=$($IQBINRDB -i 13 -b 0 -p 63528 -n 256 -f 2 -a 0)

                IQ_GAMMA_R_DIFF4=$($IQBINRDB -i 13 -b 0 -p 53288 -n 512 -f 4 -a 0)
                IQ_GAMMA_G_DIFF4=$($IQBINRDB -i 13 -b 0 -p 55336 -n 512 -f 4 -a 0)
                IQ_GAMMA_B_DIFF4=$($IQBINRDB -i 13 -b 0 -p 57384 -n 512 -f 4 -a 0)
                IQ_GAMMA_Y_DIFF4=$($IQBINRDB -i 13 -b 0 -p 59432 -n 512 -f 4 -a 0)

                IQ_SHARP_SHARPNESS=$($IQBINRD -i 6 -n 7 -a 0)

                IQ_SHARP_EDGE=$($IQBINRD -i 20 -n 7 -a 0)

                IQ_DENOISE_NR_Y=$($IQBINRD -i 3 -n 7 -a 15)

                IQ_DENOISE_NR_C=$($IQBINRD -i 32 -n 7 -a 15)

                IQ_NR3D_NR=$($IQBINRD -i 36 -n 7 -a 0)

                TMP_OB0=$($IQBINRDB -i 1 -b 0 -p 0 -f 2 -a 0)
                TMP_OB1=$($IQBINRDB -i 1 -b 0 -p 8 -f 2 -a 0)
                TMP_OB2=$($IQBINRDB -i 1 -b 0 -p 16 -f 2 -a 0)
                TMP_OB3=$($IQBINRDB -i 1 -b 0 -p 24 -f 2 -a 0)
                TMP_OB4=$($IQBINRDB -i 1 -b 0 -p 32 -f 2 -a 0)
                TMP_OB5=$($IQBINRDB -i 1 -b 0 -p 40 -f 2 -a 0)
                TMP_OB6=$($IQBINRDB -i 1 -b 0 -p 48 -f 2 -a 0)
                IQ_SENSOR_OBR="["$TMP_OB0","$TMP_OB1","$TMP_OB2","$TMP_OB3","$TMP_OB4","$TMP_OB5","$TMP_OB6"]"

                TMP_OB0=$($IQBINRDB -i 1 -b 0 -p 2 -f 2 -a 0)
                TMP_OB1=$($IQBINRDB -i 1 -b 0 -p 10 -f 2 -a 0)
                TMP_OB2=$($IQBINRDB -i 1 -b 0 -p 18 -f 2 -a 0)
                TMP_OB3=$($IQBINRDB -i 1 -b 0 -p 26 -f 2 -a 0)
                TMP_OB4=$($IQBINRDB -i 1 -b 0 -p 34 -f 2 -a 0)
                TMP_OB5=$($IQBINRDB -i 1 -b 0 -p 42 -f 2 -a 0)
                TMP_OB6=$($IQBINRDB -i 1 -b 0 -p 50 -f 2 -a 0)
                IQ_SENSOR_OBGR="["$TMP_OB0","$TMP_OB1","$TMP_OB2","$TMP_OB3","$TMP_OB4","$TMP_OB5","$TMP_OB6"]"

                TMP_OB0=$($IQBINRDB -i 1 -b 0 -p 4 -f 2 -a 0)
                TMP_OB1=$($IQBINRDB -i 1 -b 0 -p 12 -f 2 -a 0)
                TMP_OB2=$($IQBINRDB -i 1 -b 0 -p 20 -f 2 -a 0)
                TMP_OB3=$($IQBINRDB -i 1 -b 0 -p 28 -f 2 -a 0)
                TMP_OB4=$($IQBINRDB -i 1 -b 0 -p 36 -f 2 -a 0)
                TMP_OB5=$($IQBINRDB -i 1 -b 0 -p 44 -f 2 -a 0)
                TMP_OB6=$($IQBINRDB -i 1 -b 0 -p 52 -f 2 -a 0)
                IQ_SENSOR_OBGB="["$TMP_OB0","$TMP_OB1","$TMP_OB2","$TMP_OB3","$TMP_OB4","$TMP_OB5","$TMP_OB6"]"

                TMP_OB0=$($IQBINRDB -i 1 -b 0 -p 6 -f 2 -a 0)
                TMP_OB1=$($IQBINRDB -i 1 -b 0 -p 14 -f 2 -a 0)
                TMP_OB2=$($IQBINRDB -i 1 -b 0 -p 22 -f 2 -a 0)
                TMP_OB3=$($IQBINRDB -i 1 -b 0 -p 30 -f 2 -a 0)
                TMP_OB4=$($IQBINRDB -i 1 -b 0 -p 38 -f 2 -a 0)
                TMP_OB5=$($IQBINRDB -i 1 -b 0 -p 46 -f 2 -a 0)
                TMP_OB6=$($IQBINRDB -i 1 -b 0 -p 54 -f 2 -a 0)
                IQ_SENSOR_OBB="["$TMP_OB0","$TMP_OB1","$TMP_OB2","$TMP_OB3","$TMP_OB4","$TMP_OB5","$TMP_OB6"]"

                IQ_COLOR_SATURATION=$($IQBINRD -i 28 -n 7 -a 0)

                # Read AE
                AE_COMMON_FNUMBER=$($AEBINRD0 -i 0 -p 0 -f 64)

                AE_EVPROGRAM_PROG0=$($AEBINRD0 -i 1 -p 96 -s 32 -n 8)
                AE_EVPROGRAM_PROG1=$($AEBINRD0 -i 1 -p 288 -s 36 -n 8)
                AE_EVPROGRAM_PROG2=$($AEBINRD0 -i 1 -p 480 -s 40 -n 8)
                AE_EVPROGRAM_PROG3=$($AEBINRD0 -i 1 -p 672 -s 44 -n 8)
                AE_EVPROGRAM_PROG4=$($AEBINRD0 -i 1 -p 864 -s 48 -n 8)
                AE_EVPROGRAM_PROG5=$($AEBINRD0 -i 1 -p 1056 -s 52 -n 8)
                AE_EVPROGRAM_PROG6=$($AEBINRD0 -i 1 -p 1248 -s 56 -n 8)
                AE_EVPROGRAM_PROG7=$($AEBINRD0 -i 1 -p 1440 -s 60 -n 8)
                AE_EVPROGRAM_PROG8=$($AEBINRD0 -i 1 -p 1632 -s 64 -n 8)
                AE_EVPROGRAM_PROG9=$($AEBINRD0 -i 1 -p 1824 -s 68 -n 8)
                AE_EVPROGRAM_PROG10=$($AEBINRD0 -i 1 -p 2016 -s 72 -n 8)
                AE_EVPROGRAM_PROG11=$($AEBINRD0 -i 1 -p 2208 -s 76 -n 8)
                AE_EVPROGRAM_PROG12=$($AEBINRD0 -i 1 -p 2400 -s 80 -n 8)
                AE_EVPROGRAM_PROG13=$($AEBINRD0 -i 1 -p 2592 -s 84 -n 8)
                AE_EVPROGRAM_PROG14=$($AEBINRD0 -i 1 -p 2784 -s 88 -n 8)
                AE_EVPROGRAM_PROG15=$($AEBINRD0 -i 1 -p 2976 -s 92 -n 8)

                AE_DEFAULTSET_AE_METERING=$($AEBINRD1 -i 1 -p 0)
                AE_DEFAULTSET_AE_MODE=$($AEBINRD1 -i 1 -p 4)
                AE_DEFAULTSET_AE_SPEED=$($AEBINRD1 -i 1 -p 8)
                AE_DEFAULTSET_AE_ADAPTIVE=$($AEBINRD1 -i 1 -p 12)
                AE_DEFAULTSET_EVPROGRAM=$($AEBINRD1 -i 1 -p 16)
                AE_DEFAULTSET_FLICKER_COMP=$($AEBINRD1 -i 1 -p 24)
                AE_DEFAULTSET_EV_COMP=$($AEBINRD1 -i 1 -p 20)
                AE_DEFAULTSET_SENSOR_GAIN=$($AEBINRD1 -i 1 -p 28)
                AE_DEFAULTSET_SENSOR_EXPOSURE=$($AEBINRD1 -i 1 -p 32 -f 64)

                # Reflect AESettings
                echo sh ../reflect_aesettings.sh $AE_DEFAULTSET_AE_METERING $AE_DEFAULTSET_AE_MODE $AE_DEFAULTSET_AE_SPEED $AE_DEFAULTSET_AE_ADAPTIVE $AE_DEFAULTSET_EVPROGRAM $AE_DEFAULTSET_FLICKER_COMP $AE_DEFAULTSET_EV_COMP $AE_DEFAULTSET_SENSOR_GAIN $AE_DEFAULTSET_SENSOR_EXPOSURE >> $LOGF
                RESULT=$(sh ../reflect_aesettings.sh $AE_DEFAULTSET_AE_METERING $AE_DEFAULTSET_AE_MODE $AE_DEFAULTSET_AE_SPEED $AE_DEFAULTSET_AE_ADAPTIVE $AE_DEFAULTSET_EVPROGRAM $AE_DEFAULTSET_FLICKER_COMP $AE_DEFAULTSET_EV_COMP $AE_DEFAULTSET_SENSOR_GAIN $AE_DEFAULTSET_SENSOR_EXPOSURE)

                # Read AWB
                AWB_BASIC_AWBSPEED=$($AWBBINRD -i 0 -p 24 -f 1)
                AWB_PREFEREDCAST=$($AWBBINRD -i 1 -p 0)
                AWB_MANUALWB=$($AWBBINRD -i 2 -p 0)

                # Regist ack returns in json structure
                IQ_GAMMA_DATA0=$($JO r_opepos=$IQ_GAMMA_R_OPEPOS0 g_opepos=$IQ_GAMMA_G_OPEPOS0 b_opepos=$IQ_GAMMA_B_OPEPOS0 y_opepos=$IQ_GAMMA_Y_OPEPOS0)
                IQ_GAMMA_DATA1=$($JO r_opepos=$IQ_GAMMA_R_OPEPOS1 g_opepos=$IQ_GAMMA_G_OPEPOS1 b_opepos=$IQ_GAMMA_B_OPEPOS1 y_opepos=$IQ_GAMMA_Y_OPEPOS1 r_full=$IQ_GAMMA_R_FULL1 r_diff=$IQ_GAMMA_R_DIFF1 g_full=$IQ_GAMMA_G_FULL1 g_diff=$IQ_GAMMA_G_DIFF1 b_full=$IQ_GAMMA_B_FULL1 b_diff=$IQ_GAMMA_B_DIFF1 y_full=$IQ_GAMMA_Y_FULL1 y_diff=$IQ_GAMMA_Y_DIFF1)
                IQ_GAMMA_DATA2=$($JO r_opepos=$IQ_GAMMA_R_OPEPOS2 g_opepos=$IQ_GAMMA_G_OPEPOS2 b_opepos=$IQ_GAMMA_B_OPEPOS2 y_opepos=$IQ_GAMMA_Y_OPEPOS2 r_full=$IQ_GAMMA_R_FULL2 r_diff=$IQ_GAMMA_R_DIFF2 g_full=$IQ_GAMMA_G_FULL2 g_diff=$IQ_GAMMA_G_DIFF2 b_full=$IQ_GAMMA_B_FULL2 b_diff=$IQ_GAMMA_B_DIFF2 y_full=$IQ_GAMMA_Y_FULL2 y_diff=$IQ_GAMMA_Y_DIFF2)
                IQ_GAMMA_DATA3=$($JO r_opepos=$IQ_GAMMA_R_OPEPOS3 g_opepos=$IQ_GAMMA_G_OPEPOS3 b_opepos=$IQ_GAMMA_B_OPEPOS3 y_opepos=$IQ_GAMMA_Y_OPEPOS3 r_full=$IQ_GAMMA_R_FULL3 r_diff=$IQ_GAMMA_R_DIFF3 g_full=$IQ_GAMMA_G_FULL3 g_diff=$IQ_GAMMA_G_DIFF3 b_full=$IQ_GAMMA_B_FULL3 b_diff=$IQ_GAMMA_B_DIFF3 y_full=$IQ_GAMMA_Y_FULL3 y_diff=$IQ_GAMMA_Y_DIFF3)
                IQ_GAMMA_DATA4=$($JO r_opepos=$IQ_GAMMA_R_OPEPOS4 g_opepos=$IQ_GAMMA_G_OPEPOS4 b_opepos=$IQ_GAMMA_B_OPEPOS4 y_opepos=$IQ_GAMMA_Y_OPEPOS4 r_full=$IQ_GAMMA_R_FULL4 r_diff=$IQ_GAMMA_R_DIFF4 g_full=$IQ_GAMMA_G_FULL4 g_diff=$IQ_GAMMA_G_DIFF4 b_full=$IQ_GAMMA_B_FULL4 b_diff=$IQ_GAMMA_B_DIFF4 y_full=$IQ_GAMMA_Y_FULL4 y_diff=$IQ_GAMMA_Y_DIFF4)
                IQ_GAMMA=$($JO index=$IQ_GAMMA_INDEX data0=$IQ_GAMMA_DATA0 data1=$IQ_GAMMA_DATA1 data2=$IQ_GAMMA_DATA2 data3=$IQ_GAMMA_DATA3 data4=$IQ_GAMMA_DATA4)
                IQ_SHARPNESS=$($JO sharpness=$IQ_SHARP_SHARPNESS edge=$IQ_SHARP_EDGE)
                IQ_DENOISE=$($JO nr_y=$IQ_DENOISE_NR_Y nr_c=$IQ_DENOISE_NR_C)
                IQ_3DNR=$($JO nr=$IQ_NR3D_NR)
                IQ_SENSOR=$($JO r=$IQ_SENSOR_OBR gr=$IQ_SENSOR_OBGR gb=$IQ_SENSOR_OBGB b=$IQ_SENSOR_OBB)
                IQ_COLOR=$($JO saturation=$IQ_COLOR_SATURATION)
                JO_IQ=$($JO gamma=$IQ_GAMMA sharpness=$IQ_SHARPNESS denoise=$IQ_DENOISE nr3d=$IQ_3DNR sensor=$IQ_SENSOR color=$IQ_COLOR)

                AE_COMMON=$($JO f_number=$AE_COMMON_FNUMBER)
                AE_EVPROGRAM=$($JO prog0=$AE_EVPROGRAM_PROG0 prog1=$AE_EVPROGRAM_PROG1 prog2=$AE_EVPROGRAM_PROG2 prog3=$AE_EVPROGRAM_PROG3 prog4=$AE_EVPROGRAM_PROG4 prog5=$AE_EVPROGRAM_PROG5 prog6=$AE_EVPROGRAM_PROG6 prog7=$AE_EVPROGRAM_PROG7 prog8=$AE_EVPROGRAM_PROG8 prog9=$AE_EVPROGRAM_PROG9 prog10=$AE_EVPROGRAM_PROG10 prog11=$AE_EVPROGRAM_PROG11 prog12=$AE_EVPROGRAM_PROG12 prog13=$AE_EVPROGRAM_PROG13 prog14=$AE_EVPROGRAM_PROG14 prog15=$AE_EVPROGRAM_PROG15)
                AE_DEFSET=$($JO ae_metering=$AE_DEFAULTSET_AE_METERING ae_mode=$AE_DEFAULTSET_AE_MODE ae_speed=$AE_DEFAULTSET_AE_SPEED ae_adaptive=$AE_DEFAULTSET_AE_ADAPTIVE evprogram=$AE_DEFAULTSET_EVPROGRAM flicker_comp=$AE_DEFAULTSET_FLICKER_COMP ev_comp=$AE_DEFAULTSET_EV_COMP sensor_gain=$AE_DEFAULTSET_SENSOR_GAIN sensor_exposure=$AE_DEFAULTSET_SENSOR_EXPOSURE)
                JO_AE=$($JO common=$AE_COMMON evprogram=$AE_EVPROGRAM defaultset=$AE_DEFSET)

                JO_AWB=$($JO basic=$($JO awb_speed=$AWB_BASIC_AWBSPEED) preferedcast=$AWB_PREFEREDCAST manualwb=$AWB_MANUALWB) 

                $JO -p IQ="$JO_IQ" AE="$JO_AE" AWB="$JO_AWB"
                ;;
esac
