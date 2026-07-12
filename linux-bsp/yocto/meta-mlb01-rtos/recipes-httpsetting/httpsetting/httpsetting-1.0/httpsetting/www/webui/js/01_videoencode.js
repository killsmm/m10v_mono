var VIDEO_ENCODE_UI_ID = {
// Video Capture Configuration
// Sensor Type COMBO box
    ComboBox_sensortype: {
        sel_mode: "#SEL_SENSORTYPE",
        result : "#SNI_ComboRet"
    },
// Video Channel COMBO box
    ComboBox_videochannel: {
        sel_mode: "#SEL_VIDEOCHANNELS",
        result : "#SNI_ComboRet"
    },
// Framerate COMBO box
    ComboBox_framerate: {
        sel_mode: "#SEL_FRAMERATE",
        result : "#SNI_ComboRet"
    },
// Codec Type COMBO box
    ComboBox_codectype: {
        sel_mode: "#SEL_CODECTYPE",
        result : "#SNI_ComboRet"
    },
// Algorithm COMBO box
    ComboBox_algorithm: {
        sel_mode: "#SEL_BITRATEALGORITHM",
        result : "#SNI_ComboRet"
    },
// GOP COMBO box
    ComboBox_gop: {
        sel_mode: "#SEL_GOPSTRUCT",
        result : "#SNI_ComboRet"
    },
// ImageSize COMBO box
    ComboBox_video_imagesize: {
        sel_mode: "#SEL_IMAGESIZE",
        result : "#SNI_ComboRet"
    },
// SensorType Button
    Button_sensor_type: {
        update: "#Btn_SENSORTYPE",
        result1: "#SNI_ButtonRet1"
    },
// BitRate Button
    Button_bitrate: {
        update: "#Btn_BITRATE",
        result1: "#SNI_ButtonRet1"
    },
// GOP Button
    Button_gop: {
        update: "#Btn_GOP",
        result1: "#SNI_ButtonRet1"
    },
// SizeVariation Button
    Button_variation: {
        update: "#Btn_VARIATION",
        result1: "#SNI_ButtonRet1"
    },
// Profile Button
    Button_profile: {
        update: "#Btn_PROFILE",
        result1: "#SNI_ButtonRet1"
    },
// Quality Button
    Button_quality: {
        update: "#Btn_QUALITY",
        result1: "#SNI_ButtonRet1"
    },
    Text: {
        ave: "#TXT_AVEBITRATE",
        max: "#TXT_BITRATE",
        q_val: "#TXT_Q_VALUE"
    },
// Image Effect Configuration
// RotateDegree COMBO box
    ComboBox_rotatedegree: {
        sel_mode: "#SEL_ROTATE_DEGREE",
        result : "#SNI_ComboRet"
    },
// Mirror COMBO box
    ComboBox_mirror: {
        sel_mode: "#SEL_MIRROR",
        result : "#SNI_ComboRet"
    },
// ImageSelect COMBO box
    ComboBox_imageselect: {
        sel_mode: "#SEL_IMAGE_SELECT",
        result : "#SNI_ComboRet"
    },
// ImageSelect Enabled COMBO box
    ComboBox_imageselect_en: {
        sel_mode: "#SEL_IMAGE_SELECT_EN",
        result : "#SNI_ComboRet"
    },
// Image Effect Enabled Button
    Button_imageeffect_en: {
        update: "#Btn_UPDATE_IMAGE_EFFECT_EN",
        result1: "#SNI_ButtonRet1"
    },
// Image Select Button
    Button_imageselect: {
        update: "#Btn_UPDATE_IMAGE_SELECT",
        result1: "#SNI_ButtonRet1"
    },
// Crop Position Button
    Button_croppos: {
        update: "#Btn_UPDATE_CROP_POS",
        result1: "#SNI_ButtonRet1"
    },
    Text_I: {
        x: "#TXT_X_COORDINATE",
        y: "#TXT_Y_COORDINATE",
        w: "#TXT_CROP_W",
        l: "#TXT_CROP_LINES"
    },

// ROI Configuration
// Mode Button
    Button_roi_mode: {
        update: "#Btn_UPDATE_ROI_MODE",
        result1: "#SNI_ButtonRet1"
    },
// Operation Button
    Button_roi_operation: {
        update: "#Btn_UPDATE_ROI_OPERATION",
        result1: "#SNI_ButtonRet1"
    },
// COMMAND1(QP Area Control,Coordinate,Important Level for HEVC,QP Value for H264) Button
    Button_roi_command1: {
        update: "#Btn_UPDATE_ROI_COMMAND1",
        result1: "#SNI_ButtonRet1"
    },
// COMMAND2(Quality Settings for HEVC, Outrange Settimg for H264) Button
    Button_roi_command2: {
        update: "#Btn_UPDATE_ROI_COMMAND2",
        result1: "#SNI_ButtonRet1"
    },
    Text_R: {
        start_x: "#TXT_ROI_START_X",
        start_y: "#TXT_ROI_START_Y",
        end_x: "#TXT_ROI_END_X",
        end_y: "#TXT_ROI_END_Y",
        qp_down_range: "#TXT_ROI_QP_DOWN_RANGE",
        qp_min: "#TXT_ROI_QP_MIN",
        deltaqp: "#TXT_ROI_DELTAQP",
        qp_up_range: "#TXT_ROI_QP_UP_RANGE",
        qp_max: "#TXT_ROI_QP_MAX"
    }
};

$(VIDEO_ENCODE_UI_ID.Text.arg1).on("change", function(event, ui)
{
    var outStr = $(this).val();
});
$(VIDEO_ENCODE_UI_ID.Text.arg2).on("change", function(event, ui)
{
    var outStr = $(this).val();
});

// SensorType Button
$(VIDEO_ENCODE_UI_ID.Button_sensor_type.update).click(function()
{
    var args = {
        arg1: $(SEL_SENSORTYPE).val(),
        arg2: $(SEL_VIDEOCHANNELS).val(),
        arg3: $(SEL_FRAMERATE).val(),
        arg4: $(SEL_CODECTYPE).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/sensortype.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Sensor Type OK\n");
        },
        function(data)
        {
//      Notice:Error return does not return soon.
//           window.alert("initileze and boot NG"); 
        }
    );
    // On Display	
//    $(PRIVACY_UI_ID.Button.result1).text(data.result);
});

// Bitrate Button
$(VIDEO_ENCODE_UI_ID.Button_bitrate.update).click(function()
{
    var args = {
        arg1: $(SEL_BR_VIDEOCHANNELS).val(),
        arg2: $(VIDEO_ENCODE_UI_ID.Text.ave).val(),
        arg3: $(VIDEO_ENCODE_UI_ID.Text.max).val(),
        arg4: $(SEL_BITRATEALGORITHM).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/bitrate.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("bitrate OK\n");
        },
        function(data)
        {
//      Notice:Error return does not return soon.
//           window.alert("initileze and boot NG"); 
        }
    );
    // On Display	
//    $(PRIVACY_UI_ID.Button.result1).text(data.result);
});

// GOP Button
$(VIDEO_ENCODE_UI_ID.Button_gop.update).click(function()
{
    var args = {
        arg1: $(SEL_GOP_VIDEOCHANNELS).val(),
        arg2: $(SEL_GOPSTRUCT).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/gop.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("gop OK\n");
        },
        function(data)
        {
//      Notice:Error return does not return soon.
//           window.alert("initileze and boot NG"); 
        }
    );
    // On Display	
//    $(PRIVACY_UI_ID.Button.result1).text(data.result);
});

// Size Variation Button
$(VIDEO_ENCODE_UI_ID.Button_variation.update).click(function()
{
    var args = {
        arg1: $(SEL_SZ_VIDEOCHANNELS).val(),
        arg2: $(SEL_IMAGESIZE).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/sizevariation.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("size variation OK\n");
        },
        function(data)
        {
//      Notice:Error return does not return soon.
//           window.alert("initileze and boot NG"); 
        }
    );
    // On Display	
//    $(PRIVACY_UI_ID.Button.result1).text(data.result);
});


// Profile Button
$(VIDEO_ENCODE_UI_ID.Button_profile.update).click(function()
{
    var args = {
        arg1: $(SEL_PF_VIDEOCHANNELS).val(),
        arg2: $(SEL_PROFILE).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/profile.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            window.alert("profile OK\n"); 
//            var res=data.result;
//            var err_cd=data.res2;
//            if(res==0){
//               window.alert("profile OK\n");
//            }
//            else
//            {
//                var err_msg="profile NG"+err_cd;
//                window.alert(err_msg);
//            }
        },
        function(data)
        {
//      Notice:Error return does not return soon.
            window.alert("profile NG"); 
        }
    );
});


// Quality Button
$(VIDEO_ENCODE_UI_ID.Button_quality.update).click(function()
{
    var args = {
        arg1: $(VIDEO_ENCODE_UI_ID.Text.q_val).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/quality.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            window.alert("Quality OK\n"); 
//            var res=data.result;
//            var err_cd=data.res2;
//            if(res==0){
//               window.alert("Quality OK\n");
//            }
//            else
//            {
//                var err_msg="Quality NG"+err_cd;
//                window.alert(err_msg);
//            }
        },
        function(data)
        {
//      Notice:Error return does not return soon.
            window.alert("Quality NG"); 
        }
    );
});

// Image Effect Enabled Button
$(VIDEO_ENCODE_UI_ID.Button_imageeffect_en.update).click(function()
{
    var args = {
        arg1: $(SEL_ROTATE_DEGREE).val(),
        arg2: $(SEL_MIRROR).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/imageeffectenabled.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Image Effect enabled OK\n");
        },
        function(data)
        {
           window.alert("NG");
        }
    );
});


// Image Sellect Button
$(VIDEO_ENCODE_UI_ID.Button_imageselect.update).click(function()
{
    var args = {
        arg1: $(SEL_IMAGE_SELECT).val(),
        arg2: $(SEL_IMAGE_SELECT_EN).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/imageselect.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Image select OK\n");
        },
        function(data)
        {
           window.alert("NG");
        }
    );
});


// Crop Position Button
$(VIDEO_ENCODE_UI_ID.Button_croppos.update).click(function()
{
    var args = {
        arg1: $(VIDEO_ENCODE_UI_ID.Text_I.x).val(),
        arg2: $(VIDEO_ENCODE_UI_ID.Text_I.y).val(),
        arg3: $(VIDEO_ENCODE_UI_ID.Text_I.w).val(),
        arg4: $(VIDEO_ENCODE_UI_ID.Text_I.l).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/croppos.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Crop Pos OK\n");
        },
        function(data)
        {
           window.alert("NG");
        }
    );
});




// Mode Button
$(VIDEO_ENCODE_UI_ID.Button_roi_mode.update).click(function()
{
    var args = {
        arg1: $(SEL_ROI_MODE).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/roi_mode.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("ROI Mode OK\n");
        },
        function(data)
        {
           window.alert("NG");
        }
    );
});


// Operation Button
$(VIDEO_ENCODE_UI_ID.Button_roi_operation.update).click(function()
{
    var args = {
        arg1: $(SEL_ROI_OPERATION).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/roi_operation.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("ROI Operation OK\n");
        },
        function(data)
        {
           window.alert("NG");
        }
    );
});



// Command1
$(VIDEO_ENCODE_UI_ID.Button_roi_command1.update).click(function()
{
    var sel_stream = 0; // Select stream
    var ret_cd = 0;  // cgi return code(0:OK -1:NG)
    var dmy_now = 0; // Dummy now(Sub-command send order: Area Control,Coordinate,Coordinate,Important Level or QP Value)
    var dmy_start = 0; // Dummy start time
    var dmy_diff = 0; // Dummy diff

    sel_stream = $(SEL_ROI_SELECT_STREAM1).val(); // select stream 0:HEVC 1:H264

    // Area Control
    ret_cd = roi_area_control();
    if ( ret_cd != 0){
        window.alert("Area Control NG");
        return;
    }
    // Dummy Wait(1000ms)
    dmy_start = $.now(); // Start
    for( ; ; ){
        dmy_now = $.now(); // now
        dmy_diff = dmy_now-dmy_start;  // calc
        if( dmy_diff > 1000 ){  // 1sec(1000ms)over
            break;
        }
    }

    // Coordinate
    ret_cd = roi_coordinate();
    if ( ret_cd != 0){
        window.alert("Coordinate NG");
        return;
    }
    // Dummy Wait(5000ms)
    dmy_start = $.now(); // Start
    for( ; ; ){
        dmy_now = $.now(); // now
        dmy_diff = dmy_now-dmy_start;  // calc
        if( dmy_diff > 5000 ){  // 5sec(5000ms)over
            break;
        }
    }

    if ( sel_stream == 0){
    // HEVC
    // Important Level
        ret_cd = roi_important_level();
        if ( ret_cd != 0){
            window.alert("Important Level NG");
            return;
        }
    }
    else{
    // H264
    // QP Value
        ret_cd = roi_qp_value();
        if ( ret_cd != 0){
            window.alert("QP Value NG");
            return;
        }
    }

    // ALL OK
    window.alert("OK");
});



// Command2
$(VIDEO_ENCODE_UI_ID.Button_roi_command2.update).click(function()
{
    var sel_stream = 0; // Select stream
    var ret_cd = 0;  // cgi retrun code(0:OK -1:NG)

    sel_stream = $(SEL_ROI_SELECT_STREAM2).val(); // Select stream 0:HEVC 1:H264

    if ( sel_stream == 0){
    // HEVC
    // Quality Settings
        ret_cd = roi_quality_setting();
        if ( ret_cd != 0){
            window.alert("Quality SettingNG");
            return;
        }
    }
    else{
    // H264
    // Outrange Settimg
        ret_cd = roi_outrange_setting();
        if ( ret_cd != 0){
            window.alert("Outrange Setting NG");
            return;
        }
    }

    //  ALL OK
    window.alert("OK");
});



// Area Control
function roi_area_control()
{
    var ret_cd = 0;
    var args = {
        arg1: $(SEL_ROI_STREAM_NO).val(),
        arg2: $(SEL_ROI_AREA_NO).val(),
        arg3: $(SEL_ROI_AREA_ENABLED).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/roi_areacontrol.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
//           window.alert("ROI AreaControl OK\n");
            ret_cd = 0; // OK
        },
        function(data)
        {
//           window.alert("NG");
            ret_cd = -1; // NG
        }
    );
    return(ret_cd);
}


// Coordinate
function roi_coordinate()
{
    var ret_cd = 0;
    var args = {
        arg1: $(VIDEO_ENCODE_UI_ID.Text_R.start_x).val(),
        arg2: $(VIDEO_ENCODE_UI_ID.Text_R.start_y).val(),
        arg3: $(VIDEO_ENCODE_UI_ID.Text_R.end_x).val(),
        arg4: $(VIDEO_ENCODE_UI_ID.Text_R.end_y).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/roi_coordinate.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
//           window.alert("ROI Coordinate OK\n");
            ret_cd = 0; // OK
        },
        function(data)
        {
//           window.alert("NG");
            ret_cd = -1; // NG
        }
    );
    return(ret_cd);
}



// Important Level for HEVC
function roi_important_level()
{
    var ret_cd = 0;
    var args = {
        arg1: $(SEL_ROI_IMPORTANT_LEVEL).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/roi_importantlevel.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
//           window.alert("ROI ImportantLevel OK\n");
            ret_cd = 0; // OK
        },
        function(data)
        {
//           window.alert("NG");
            ret_cd = -1; // NG
        }
    );
    return(ret_cd);
}



// QP Value for H264
function roi_qp_value()
{
    var ret_cd = 0;
    var args = {
        arg1: $(VIDEO_ENCODE_UI_ID.Text_R.qp_down_range).val(),
        arg2: $(VIDEO_ENCODE_UI_ID.Text_R.qp_min).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/roi_qpvalue.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
//           window.alert("ROI QP Value OK\n");
            ret_cd = 0; // OK
        },
        function(data)
        {
//           window.alert("NG");
            ret_cd =-1; // NG
        }
    );
    return(ret_cd);
}



// Quality Settings for HEVC
function roi_quality_setting()
{
    var ret_cd = 0;
    var args = {
        arg1: $(VIDEO_ENCODE_UI_ID.Text_R.deltaqp).val(),
        arg2: $(SEL_ROI_BASE_IMPORTANT_LEVEL).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/roi_qualitysetting.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
//           window.alert("ROI QualitySetting OK\n");
            ret_cd = 0; // OK
        },
        function(data)
        {
//           window.alert("NG");
            ret_cd = -1; // NG
        }
    );
    return(ret_cd);
}



// Outrange Settimg for H264
function roi_outrange_setting()
{
    var ret_cd = 0;
    var args = {
        arg1: $(SEL_ROI_OUTSIDE_PARTICAL_CODIND_ENABLED).val(),
        arg2: $(VIDEO_ENCODE_UI_ID.Text_R.qp_up_range).val(),
        arg3: $(VIDEO_ENCODE_UI_ID.Text_R.qp_max).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/01_videoencode/roi_outrangesetting.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
//           window.alert("ROI OutrangeSetting OK\n");
            ret_cd = 0; // OK
        },
        function(data)
        {
//           window.alert("NG");
            ret_cd = -1; // NG
        }
    );
    return(ret_cd);
}


