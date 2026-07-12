var VIDEO_CONTROL_UI_ID = {
// Image Configuration
// EV Compensation COMBO box
    ComboBox_ev_compensation: {
        sel_mode: "#SEL_EV_COMPENSATION",
        result : "#SNI_ComboRet"
    },
// White Balance COMBO box
    ComboBox_white_balance: {
        sel_mode: "#SEL_WHITE_BALANCE",
        result : "#SNI_ComboRet"
    },
// Color COMBO box
    ComboBox_color: {
        sel_mode: "#SEL_COLOR",
        result : "#SNI_ComboRet"
    },
// Matrix line COMBO box
    ComboBox_matrix_line: {
        sel_mode: "#SEL_MATRIX_LINE",
        result : "#SNI_ComboRet"
    },
// Sharpness COMBO box
    ComboBox_sharpness: {
        sel_mode: "#SEL_SHARPNESS",
        result : "#SNI_ComboRet"
    },
// Edge Strength COMBO box
    ComboBox_edge_strength: {
        sel_mode: "#SEL_EDGE_STRENGTH",
        result : "#SNI_ComboRet"
    },
// ISO COMBO box
    ComboBox_iso: {
        sel_mode: "#SEL_ISO",
        result : "#SNI_ComboRet"
    },
// Exposure Time COMBO box
    ComboBox_exposure_time: {
        sel_mode: "#SEL_EXPOSURE_TIME",
        result : "#SNI_ComboRet"
    },
// AE Metering COMBO box
    ComboBox_ae_metering: {
        sel_mode: "#SEL_AE_METERING",
        result : "#SNI_ComboRet"
    },
// AE Coverage Speed COMBO box
    ComboBox_ae_coverage_speed: {
        sel_mode: "#SEL_AE_COVERAGE_SPEED",
        result : "#SNI_ComboRet"
    },
// AE/AWB Mode COMBO box
    ComboBox_aeawb_mode: {
        sel_mode: "#SEL_AEAWB_MODE",
        result : "#SNI_ComboRet"
    },
// Sensor COMBO box
    ComboBox_aeawb_sensor: {
        sel_mode: "#SEL_AEAWB_SENSOR",
        result : "#SNI_ComboRet"
    },
// Flicker COMBO box
    ComboBox_flicker: {
        sel_mode: "#SEL_FLICKER",
        result : "#SNI_ComboRet"
    },
// AE Lock COMBO box
    ComboBox_ae_lock: {
        sel_mode: "#SEL_AE_LOCK",
        result : "#SNI_ComboRet"
    },
// EV Compensation Button
    Button_ev_compensation: {
        update: "#Btn_EV_COMP",
        result1: "#SNI_ButtonRet1"
    },
// WhiteBalance Button
    Button_whitebalance: {
        update: "#Btn_WHITEBALANCE",
        result1: "#SNI_ButtonRet1"
    },
// Color Button
    Button_color: {
        update: "#Btn_COLOR",
        result1: "#SNI_ButtonRet1"
    },
// Matrix Button
    Button_matrix: {
        update: "#Btn_UPDATE_MATRIX",
        result1: "#SNI_ButtonRet1"
    },
// Sharpness Button
    Button_sharpness: {
        update: "#Btn_UPDATE_SHARPNESS",
        result1: "#SNI_ButtonRet1"
    },
// ISO Button
    Button_iso: {
        update: "#Btn_UPDATE_ISO",
        result1: "#SNI_ButtonRet1"
    },
// Exposure Time Button
    Button_exposure_time: {
        update: "#Btn_UPDATE_EXPOSURE_TIME",
        result1: "#SNI_ButtonRet1"
    },
// AE Metering Button
    Button_ae_metering: {
        update: "#Btn_UPDATE_AEMETERING",
        result1: "#SNI_ButtonRet1"
    },
// AE Coverage Speed Button
    Button_ae_coverage_speed: {
        update: "#Btn_UPDATE_AECOVERAGESPEED",
        result1: "#SNI_ButtonRet1"
    },
// Flicker Button
    Button_flicker: {
        update: "#Btn_UPDATE_FLICKER",
        result1: "#SNI_ButtonRet1"
    },
// AE Lock Button
    Button_ae_lock: {
        update: "#Btn_UPDATE_AELOCK",
        result1: "#SNI_ButtonRet1"
    },
// EVP Table No Button
    Button_evp: {
        update: "#Btn_UPDATE_EVP",
        result1: "#SNI_ButtonRet1"
    },
// AE/AWB Mode Button
    Button_aeawb_mode: {
        update: "#Btn_UPDATE_AEAWB_MODE",
        result1: "#SNI_ButtonRet1"
    },
//  Set calibration COMBO box
    ComboBox_set_calibration: {
        sel_mode: "#SEL_SET_CALIBRATION",
        result : "#SNI_ComboRet"
    },
//  Cal Mode COMBO box
    ComboBox_cal_mode: {
        sel_mode: "#SEL_CAL_MODE",
        result : "#SNI_ComboRet"
    },
//  Calibration At Factory COMBO box
    ComboBox_cal_factory: {
        sel_mode: "#SEL_CAL_FACTORY",
        result : "#SNI_ComboRet"
    },
//  Calibration Mode Factory COMBO box
    ComboBox_cal_mode_factory: {
        sel_mode: "#SEL_CAL_MODE_FACTORY",
        result : "#SNI_ComboRet"
    },
//  Calibration Method Factory COMBO box
    ComboBox_cal_method_factory: {
        sel_mode: "#SEL_CAL_METHOD_FACTORY",
        result : "#SNI_ComboRet"
    },
//  Calibration Clear Table Factory COMBO box
    ComboBox_cal_clear_tbl_factory: {
        sel_mode: "#SEL_CAL_CLEAR_TABLE_FACTORY",
        result : "#SNI_ComboRet"
    },

// Calibration Address Button
    Button_cal_ad: {
        update: "#Btn_UPDATE_CAL_AD",
        result1: "#SNI_ButtonRet1"
    },

// Calibration Address Factory Button
    Button_cal_ad_f: {
        update: "#Btn_UPDATE_CAL_AD_F",
        result1: "#SNI_ButtonRet1"
    },

// R Gian,B Gain,YC 0,YC 1,YC2,EVP Text
    Text: {
        r_gain: "#TXT_R_GAIN",
        b_gain: "#TXT_B_GAIN",
        yc0: "#TXT_YC0",
        yc1: "#TXT_YC1",
        yc2: "#TXT_YC2",
        evp: "#TXT_EVP_TABLE_NO",
        cal_ad: "#TXT_CALIB_ADDRESS",
        cal_ad_f: "#TXT_CALIB_ADDRESS_F"
    }
};


// EV Compensation Button
$(VIDEO_CONTROL_UI_ID.Button_ev_compensation.update).click(function()
{
    var args = {
        arg1: $(SEL_EV_COMPENSATION).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/EVCompensation.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("EVCompensation ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );
});



// WhiteBalance Button
$(VIDEO_CONTROL_UI_ID.Button_whitebalance.update).click(function()
{
    var args = {
        arg1: $(SEL_WHITE_BALANCE).val(),
        arg2: $(VIDEO_CONTROL_UI_ID.Text.r_gain).val(),
        arg3: $(VIDEO_CONTROL_UI_ID.Text.b_gain).val()
    };
// change to Camera IF Setting value from R Gain,B Gain UI value
//    args.arg2 = gain(args.arg2);
//    args.arg3 = gain(args.arg3);

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/WhiteBalance.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("WhiteBalance Update ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );		
});


// Color Button
$(VIDEO_CONTROL_UI_ID.Button_color.update).click(function()
{
    var args = {
        arg1: $(SEL_COLOR).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/Color.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("color ok\n");
        },
        function(data)
        {
           window.alert("color NG"); 
        }
    );
});

// Matrix Button
$(VIDEO_CONTROL_UI_ID.Button_matrix.update).click(function()
{
    var args = {
        arg1: $(SEL_MATRIX_LINE).val(),
        arg2: $(VIDEO_CONTROL_UI_ID.Text.yc0).val(),
        arg3: $(VIDEO_CONTROL_UI_ID.Text.yc1).val(),
        arg4: $(VIDEO_CONTROL_UI_ID.Text.yc2).val()
    };
//    args.arg2 = matrix_yc(args.arg2);
//    args.arg3 = matrix_yc(args.arg3);
//    args.arg4 = matrix_yc(args.arg4);

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/CCMatrix.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Matrix Update ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );		
});



// Sharpness Button
$(VIDEO_CONTROL_UI_ID.Button_sharpness.update).click(function()
{
    var args = {
        arg1: $(SEL_SHARPNESS).val(),
        arg2: $(SEL_EDGE_STRENGTH).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/Sharpness.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Sharpness ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );
});


// ISO Button
$(VIDEO_CONTROL_UI_ID.Button_iso.update).click(function()
{
    var args = {
        arg1: $(SEL_ISO).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/ISO.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("ISO ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );
});

// Exposure Time Button
$(VIDEO_CONTROL_UI_ID.Button_exposure_time.update).click(function()
{
    var args = {
        arg1: $(SEL_EXPOSURE_TIME).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/ExposureTime.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Exposure Time ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );
});


// AE Metering Button
$(VIDEO_CONTROL_UI_ID.Button_ae_metering.update).click(function()
{
    var args = {
        arg1: $(SEL_AE_METERING).val()
    };
    // Spot Parameter(param2);0 FIX
    args.arg2 = 0;

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/AEMetering.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("AE Metering ok\n");
        },
        function(data)
        {
           window.alert("AE Metering NG"); 
        }
    );
});

// AE Coverage Speed Button
$(VIDEO_CONTROL_UI_ID.Button_ae_coverage_speed.update).click(function()
{
    var args = {
        arg1: $(SEL_AE_COVERAGE_SPEED).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/AECoverageSpeed.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("AE Coverage Speed ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );
});

// Flicker Button
$(VIDEO_CONTROL_UI_ID.Button_flicker.update).click(function()
{
    var args = {
        arg1: $(SEL_FLICKER).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/Flicker.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Flicker ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );
});

// AE Lock Button
$(VIDEO_CONTROL_UI_ID.Button_ae_lock.update).click(function()
{
    var args = {
        arg1: $(SEL_AE_LOCK).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/AELock.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("AE Lock ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );
});

// EVP Table No Button
$(VIDEO_CONTROL_UI_ID.Button_evp.update).click(function()
{
    var args = {
        arg1: $(VIDEO_CONTROL_UI_ID.Text.evp).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/EVPtableNo.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("EVP Table ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );		
});

// AE/AWB Mode
$(VIDEO_CONTROL_UI_ID.Button_aeawb_mode.update).click(function()
{
    var args = {
        arg1: $(SEL_AEAWB_MODE).val(),
        arg2: $(SEL_AEAWB_SENSOR).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/AEAWBMode.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("AE/AWB Mode OK\n");
        },
        function(data)
        {
           window.alert("AE/AWB Mode NG"); 
        }
    );
});

// Set Calibration COMBO box
$(VIDEO_CONTROL_UI_ID.ComboBox_set_calibration.sel_mode).change(function()
{
    var args = {
        arg1: $(SEL_SET_CALIBRATION).val(),
        arg2: $(SEL_CAL_MODE).val(),
        arg3: $(VIDEO_CONTROL_UI_ID.Text.cal_ad).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/SetCalibration.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Set Calibration ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );
});

// Cal Mode COMBO box
$(VIDEO_CONTROL_UI_ID.ComboBox_cal_mode.sel_mode).change(function()
{
    var args = {
        arg1: $(SEL_SET_CALIBRATION).val(),
        arg2: $(SEL_CAL_MODE).val(),
        arg3: $(VIDEO_CONTROL_UI_ID.Text.cal_ad).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/SetCalibration.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Calibration Mode ok\n");
        },
        function(data)
        {
           window.alert("NG");
        }
    );
});

// Calibration at Factory COMBO box
$(VIDEO_CONTROL_UI_ID.ComboBox_cal_factory.sel_mode).change(function()
{
    var args = {
        arg1: $(SEL_CAL_FACTORY).val(),
        arg2: $(SEL_CAL_MODE_FACTORY).val(),
        arg3: $(VIDEO_CONTROL_UI_ID.Text.cal_ad_f).val(),
        arg4: $(SEL_CAL_METHOD_FACTORY).val(),
        arg5: $(SEL_CAL_CLEAR_TABLE_FACTORY).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/SetCalibration_f.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Calibration factory ok\n");
        },
        function(data)
        {
           window.alert("NG");
        }
    );
});

// Calibration Mode Factory COMBO box
$(VIDEO_CONTROL_UI_ID.ComboBox_cal_mode_factory.sel_mode).change(function()
{
    var args = {
        arg1: $(SEL_CAL_FACTORY).val(),
        arg2: $(SEL_CAL_MODE_FACTORY).val(),
        arg3: $(VIDEO_CONTROL_UI_ID.Text.cal_ad_f).val(),
        arg4: $(SEL_CAL_METHOD_FACTORY).val(),
        arg5: $(SEL_CAL_CLEAR_TABLE_FACTORY).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/SetCalibration_f.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Calibration mode ok\n");
        },
        function(data)
        {
           window.alert("NG");
        }
    );
});


// Calibration Method Factory COMBO box
$(VIDEO_CONTROL_UI_ID.ComboBox_cal_method_factory.sel_mode).change(function()
{
    var args = {
        arg1: $(SEL_CAL_FACTORY).val(),
        arg2: $(SEL_CAL_MODE_FACTORY).val(),
        arg3: $(VIDEO_CONTROL_UI_ID.Text.cal_ad_f).val(),
        arg4: $(SEL_CAL_METHOD_FACTORY).val(),
        arg5: $(SEL_CAL_CLEAR_TABLE_FACTORY).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/SetCalibration_f.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Method ok\n");
        },
        function(data)
        {
           window.alert("NG");
        }
    );
});

// Calibration ClearTable Factory COMBO box
$(VIDEO_CONTROL_UI_ID.ComboBox_cal_clear_tbl_factory.sel_mode).change(function()
{
    var args = {
        arg1: $(SEL_CAL_FACTORY).val(),
        arg2: $(SEL_CAL_MODE_FACTORY).val(),
        arg3: $(VIDEO_CONTROL_UI_ID.Text.cal_ad_f).val(),
        arg4: $(SEL_CAL_METHOD_FACTORY).val(),
        arg5: $(SEL_CAL_CLEAR_TABLE_FACTORY).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/SetCalibration_f.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("ClearTable factory ok\n");
        },
        function(data)
        {
           window.alert("NG");
        }
    );
});









// Calibration Address Button
$(VIDEO_CONTROL_UI_ID.Button_cal_ad.update).click(function()
{ 
    var args = {
        arg1: $(SEL_SET_CALIBRATION).val(),
        arg2: $(SEL_CAL_MODE).val(),
        arg3: $(VIDEO_CONTROL_UI_ID.Text.cal_ad).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/SetCalibration.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Calibration Add ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );
});

// Calibration Address Factory Button
$(VIDEO_CONTROL_UI_ID.Button_cal_ad_f.update).click(function()
{ 
    var args = {
        arg1: $(SEL_CAL_FACTORY).val(),
        arg2: $(SEL_CAL_MODE_FACTORY).val(),
        arg3: $(VIDEO_CONTROL_UI_ID.Text.cal_ad_f).val(),
        arg4: $(SEL_CAL_METHOD_FACTORY).val(),
        arg5: $(SEL_CAL_CLEAR_TABLE_FACTORY).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/02_videocontrol/SetCalibration_f.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Calibration Add ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );
});

// change to Camera IF Setting value from YC UI value
// param I/ value  WebUI value
// ret yc Camera IF Setting value(-2048-2047)
// note:Spec -4.0~3.99 = -2048~2047, ex 1.0=512
// param is no chaeck
function matrix_yc(value){
//    var tmp = 0;
//    var tmp100 = 0;
    var yc = 0;

//    tmp = parseFloat(value);			// Digitize
//    tmp100 = value * 100;				// 100 times
//    tmp100 = Math.floor(tmp100);			// cut off
//    tmp = tmp100/100;					// 1/100
//    tmp = tmp * 512;					// cahnge Camera IF
//    yc = Math.round(tmp);				// round

//    if(yc >= 2043){					// 2043 over
//        yc = 2047;					// Set Max value 2047
//    } 
//    else if((2037 <= yc) && (yc < 2043) ){		// 
//        yc = 2040;					// Set 2040
//    }
    yc = value;
    yc = yc & 0xfff;					// value mask 0xfff(camera IF) 
    return yc;
}

// change to Camera IF setting value from R Gain,B Gain UI value
// param I/ value  WebUI value
// ret gain Camera IF Setting value(0-2047)
// note:Spec 0.0~7.99 = 0~2047, ex 1.0=256
// param is no cheack
function gain(value){
//    var tmp = 0;
//    var tmp100 = 0;
    var gain = 0;

//    tmp = parseFloat(value);			// Digitize
//    tmp100 = value* 100;				// 100 times
//    tmp100 = Math.floor(tmp100);			// cut off
//    tmp = tmp100/100;					// 1/100
//    tmp = tmp * 256;					// change Camera IF
//    gain = Math.round(tmp);				// round

//    if(gain >= 2045){					// 2045 over
//        gain = 2047;					// Set Max value 2047
//    } 
//    else if((2042 <= gain) && (gain < 2045) ){		// 
//        gain = 2043;					// Set 2043
//    } 
    gain = value;
    return gain;
}
