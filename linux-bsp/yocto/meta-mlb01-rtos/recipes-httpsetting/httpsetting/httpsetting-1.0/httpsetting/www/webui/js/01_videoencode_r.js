var VIDEO_ENCODE_UI_ID_R = {
// Size & Framerate
    ComboBox_videochannel: {
        sel_mode: "#SEL_VIDEOCHANNELS",
        result : "#SNI_ComboRet"
    },

// Bitrate
    ComboBox_br_videochannel: {
        sel_mode: "#SEL_BR_VIDEOCHANNELS",
        result : "#SNI_ComboRet"
    },

// GOP 
    ComboBox_gop_videochannel: {
        sel_mode: "#SEL_GOP_VIDEOCHANNELS",
        result : "#SNI_ComboRet"
    },

// Image Size Variation
    ComboBox_img_videochannel: {
        sel_mode: "#SEL_SZ_VIDEOCHANNELS",
        result : "#SNI_ComboRet"
    },

// Profile for H264
    ComboBox_profile_videochannel: {
        sel_mode: "#SEL_PF_VIDEOCHANNELS",
        result : "#SNI_ComboRet"
    },

// Image Select (Crop Enabled)
    ComboBox_imageselect: {
        sel_mode: "#SEL_IMAGE_SELECT",
        result : "#SNI_ComboRet"
    },

// Area Control
    ComboBox_roi_stream_no: {
        sel_mode: "#SEL_ROI_STREAM_NO",
        result : "#SNI_ComboRet"
    },

// Area Control
    ComboBox_roi_area_no: {
        sel_mode: "#SEL_ROI_AREA_NO",
        result : "#SNI_ComboRet"
    }

};


//******************************************************
// Open the parameter area of Video Encode.
//******************************************************
$(function()
{
    var sel_stream;             // select stream1 0:HEVC 1:H264

    // Size & Framerate
    get_sensortype();
    wait();

    // Bitrate
    get_bitrate();
    wait();

    // GOP 
    get_gop();
    wait();

    // Image Size Variation
    get_sizevariation();
    wait();

    // Profile for H264
    get_profile();
    wait();

    // Quality
    get_quality();
    wait();

    // Enabled(ON/OFF)
    get_imageeffectenabled();
    wait();

    // Image Select (Crop Enabled)
    get_imageselect();
    wait();

    // Crop Position
    get_croppos();
    wait();

    // Mode
    get_roi_mode();
    wait();

    // Operation
    get_roi_operation();
    wait();

    // Select Stream1, Select Stream2
    sel_stream = get_roi_select_stream();
    if ( sel_stream === -1 ) {
        return;
    }
    wait();

    // Area Control
    get_roi_areacontrol();
    wait();

    // Coordinate
    get_roi_coordinate();
    wait();

    // Important Level for HEVC
    get_roi_importantlevel();
    wait();

    // QP Value for H264
    get_roi_qpvalue();
    wait();

    // Quality Settings for HEVC
    get_roi_qualitysetting();
    wait();

    // Outrange Setting for H264
    get_roi_outrangesetting();
    wait();

});


//******************************************************
// Change key parameter.
//******************************************************

$(VIDEO_ENCODE_UI_ID_R.ComboBox_videochannel.sel_mode).change(function()
{
    // Size & Framerate
    get_sensortype();
});

$(VIDEO_ENCODE_UI_ID_R.ComboBox_br_videochannel.sel_mode).change(function()
{
    // Bitrate
    get_bitrate();
});

$(VIDEO_ENCODE_UI_ID_R.ComboBox_gop_videochannel.sel_mode).change(function()
{
    // GOP 
    get_gop();
});

$(VIDEO_ENCODE_UI_ID_R.ComboBox_img_videochannel.sel_mode).change(function()
{
    // Image Size Variation
    get_sizevariation();
});

$(VIDEO_ENCODE_UI_ID_R.ComboBox_profile_videochannel.sel_mode).change(function()
{
    // Profile for H264
    get_profile();
});

$(VIDEO_ENCODE_UI_ID_R.ComboBox_imageselect.sel_mode).change(function()
{
    // Image Select (Crop Enabled)
    get_imageselect();

    // Crop Position
    get_croppos();

});

$(VIDEO_ENCODE_UI_ID_R.ComboBox_roi_stream_no.sel_mode).change(function()
{
    var sel_stream;             // select stream1 0:HEVC 1:H264

    // Select Stream1, Select Stream2
    sel_stream = get_roi_select_stream();
    if ( sel_stream === -1 ) {
        return;
    }

    // Area Control
    get_roi_areacontrol();

    // Coordinate
    get_roi_coordinate();

    if ( sel_stream === 0 ) {
        // -- HEVC
        // Important Level for HEVC
        get_roi_importantlevel();

        // Quality Settings for HEVC
        get_roi_qualitysetting();
    } else {
        // -- H264
        // QP Value for H264
        get_roi_qpvalue();

        // Outrange Setting for H264
        get_roi_outrangesetting();
    }

});

$(VIDEO_ENCODE_UI_ID_R.ComboBox_roi_area_no.sel_mode).change(function()
{
    var sel_stream;             // select stream1 0:HEVC 1:H264

    // Select Stream1, Select Stream2
    sel_stream = get_roi_select_stream();
    if ( sel_stream === -1 ) {
        return;
    }

    // Area Control
    get_roi_areacontrol();

    // Coordinate
    get_roi_coordinate();

    if ( sel_stream === 0 ) {
        // -- HEVC
        // Important Level for HEVC
        get_roi_importantlevel();
    } else {
        // -- H264
        // QP Value for H264
        get_roi_qpvalue();
    }
});




//******************************************************
// Functiuon
//******************************************************

//------------------------------------------------------
// Select Stream1, Select Stream2
//------------------------------------------------------
function get_roi_select_stream()
{
    // Select Stream
    var sel_stream;     // Select Stream 0:HEVC 1:H264
    // Key Parameter
    var streamno        =$(SEL_ROI_STREAM_NO).val();
    var areano          =$(SEL_ROI_AREA_NO).val();

    if ( (streamno < 8) && (areano < 2) ){
        // H264
        sel_stream = 1;
    } else if ( streamno > 7 ) {
        // HEVC
        sel_stream = 0;
    } else {
        // NG
        window.alert("The combination of Stream No." + streamno + " and Area No." + areano + " is not supported.");
        sel_stream = -1;
    }
    
    if ( sel_stream !==-1 ) {
        $('#SEL_ROI_SELECT_STREAM1').val( sel_stream );
        $('#SEL_ROI_SELECT_STREAM2').val( sel_stream );
    }

    return sel_stream;
}

//------------------------------------------------------
// Size & Framerate
//------------------------------------------------------
function get_sensortype()
{

    var args = {
        // Size & Framerate
        arg1: "0x80010300",
        arg2: "0x00000100",
        arg3: $(SEL_VIDEOCHANNELS).val(),
        arg4: "",
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // Size & Framerate
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Size & Framerate
            $('#SEL_SENSORTYPE').val( ret1 );
            $('#SEL_VIDEOCHANNELS').val( ret2 );
            $('#SEL_FRAMERATE').val( ret3 );
            $('#SEL_CODECTYPE').val( ret4 );

            //window.alert("Sensor Type OK");
        },
        function(data)
        {
            //window.alert("Sensor Type NG");
        }
    );
}

//------------------------------------------------------
// Bitrate
//------------------------------------------------------
function get_bitrate()
{

    var args = {
        // Bitrate
        arg1: "0x80010300",
        arg2: "0x00000200",
        arg3: $(SEL_BR_VIDEOCHANNELS).val(),
        arg4: "",
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // Bitrate
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Bitrate
            $('#SEL_BR_VIDEOCHANNELS').val( ret1 );
            $('#TXT_AVEBITRATE').val( ret2 );
            $('#TXT_BITRATE').val( ret3 );
            $('#SEL_BITRATEALGORITHM').val( ret4 );

            //window.alert("Bitrate OK");
        },
        function(data)
        {
            //window.alert("Bitrate NG");
        }
    );
}

//------------------------------------------------------
// GOP 
//------------------------------------------------------
function get_gop()
{

    var args = {
        // GOP 
        arg1: "0x80010300",
        arg2: "0x00000300",
        arg3: $(SEL_GOP_VIDEOCHANNELS).val(),
        arg4: "",
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // GOP 
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // GOP 
            $('#SEL_GOP_VIDEOCHANNELS').val( ret1 );
            $('#SEL_GOPSTRUCT').val( ret2 );

            //window.alert("gop OK");
        },
        function(data)
        {
            //window.alert("gop NG");
        }
    );
}

//------------------------------------------------------
// Image Size Variation
//------------------------------------------------------
function get_sizevariation()
{

    var args = {
        // Image Size Variation
        arg1: "0x80010300",
        arg2: "0x00000600",
        arg3: $(SEL_SZ_VIDEOCHANNELS).val(),
        arg4: "",
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // Image Size Variation
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Image Size Variation
            $('#SEL_SZ_VIDEOCHANNELS').val( ret1 );
            $('#SEL_IMAGESIZE').val( ret2 );

            //window.alert("size variation OK");
        },
        function(data)
        {
            //window.alert("size variation NG");
        }
    );
}

//------------------------------------------------------
// Profile for H264
//------------------------------------------------------
function get_profile()
{

    var args = {
        // Profile for H264
        arg1: "0x80010300",
        arg2: "0x00000700",
        arg3: $(SEL_PF_VIDEOCHANNELS).val(),
        arg4: "",
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // Profile for H264
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Profile for H264
            $('#SEL_PF_VIDEOCHANNELS').val( ret1 );
            $('#SEL_PROFILE').val( ret2 );

            //window.alert("profile OK");
        },
        function(data)
        {
            //window.alert("profile NG");
        }
    );
}

//------------------------------------------------------
// Quality
//------------------------------------------------------
function get_quality()
{

    var args = {
        // Quality
        arg1: "0x80010300",
        arg2: "0x00000800",
        arg3: "",
        arg4: "",
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // Quality
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Quality
            $('#TXT_Q_VALUE').val( ret1 );

            //window.alert("Quality OK");
        },
        function(data)
        {
            //window.alert("Quality NG");
        }
    );
}

//------------------------------------------------------
// Mode
//------------------------------------------------------
function get_roi_mode()
{

    var args = {
        // Mode
        arg1: "0x81010400",
        arg2: "0x00000100",
        arg3: "",
        arg4: "",
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // Mode
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Mode
            $('#SEL_ROI_MODE').val( ret1 );

            //window.alert("ROI Mode OK");
        },
        function(data)
        {
            //window.alert("ROI Mode NG");
        }
    );
}

//------------------------------------------------------
// Operation
//------------------------------------------------------
function get_roi_operation()
{

    var args = {
        // Operation
        arg1: "0x81010400",
        arg2: "0x00000200",
        arg3: "",
        arg4: "",
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // Operation
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Operation
            $('#SEL_ROI_OPERATION').val( ret1 );

            //window.alert("ROI Operation OK");
        },
        function(data)
        {
            //window.alert("ROI Operation NG");
        }
    );
}

//------------------------------------------------------
// Enabled(ON/OFF)
//------------------------------------------------------
function get_imageeffectenabled()
{

    var args = {
        // Enabled(ON/OFF)
        arg1: "0x81010700",
        arg2: "0x00000100",
        arg3: "",
        arg4: "",
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // Enabled(ON/OFF)
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Enabled(ON/OFF)
            $('#SEL_ROTATE_DEGREE').val( ret1 );
            $('#SEL_MIRROR').val( ret2 );

            //window.alert("Image Effect enabled OK");
        },
        function(data)
        {
            //window.alert("Image Effect enabled NG");
        }
    );
}

//------------------------------------------------------
// Image Select (Crop Enabled)
//------------------------------------------------------
function get_imageselect()
{

    var args = {
        // Image Select (Crop Enabled)
        arg1: "0x81010700",
        arg2: "0x00000200",
        arg3: $(SEL_IMAGE_SELECT).val(),
        arg4: "",
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // Image Select (Crop Enabled)
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Image Select (Crop Enabled)
            $('#SEL_IMAGE_SELECT_EN').val( ret1 );

            //window.alert("Image select OK");
        },
        function(data)
        {
            //window.alert("Image select NG");
        }
    );
}

//------------------------------------------------------
// Crop Position
//------------------------------------------------------
function get_croppos()
{

    var args = {
        // Crop Position
        arg1: "0x81010700",
        arg2: "0x00000300",
        arg3: $(SEL_IMAGE_SELECT).val(),
        arg4: "",
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // Crop Position
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Crop Position
            $('#TXT_X_COORDINATE').val( ret1 );
            $('#TXT_Y_COORDINATE').val( ret2 );
            $('#TXT_CROP_W').val( ret3 );
            $('#TXT_CROP_LINES').val( ret4 );

            //window.alert("Crop Pos OK");
        },
        function(data)
        {
            //window.alert("Crop Pos NG");
        }
    );
}

//------------------------------------------------------
// Area Control
//------------------------------------------------------
function get_roi_areacontrol()
{

    var args = {
        // Area Control
        arg1: "0x81010400",
        arg2: "0x00000300",
        arg3: $(SEL_ROI_STREAM_NO).val(),
        arg4: $(SEL_ROI_AREA_NO).val(),
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // Area Control
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Area Control
            $('#SEL_ROI_AREA_ENABLED').val( ret1 );

            //window.alert("ROI AreaControl OK");
        },
        function(data)
        {
            //window.alert("ROI AreaControl NG");
        }
    );
}

//------------------------------------------------------
// Coordinate
//------------------------------------------------------
function get_roi_coordinate()
{

    var args = {
        // Coordinate
        arg1: "0x81010400",
        arg2: "0x00000400",
        arg3: $(SEL_ROI_STREAM_NO).val(),
        arg4: $(SEL_ROI_AREA_NO).val(),
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // Coordinate
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Coordinate
            $('#TXT_ROI_START_X').val( ret1 );
            $('#TXT_ROI_START_Y').val( ret2 );
            $('#TXT_ROI_END_X').val( ret3 );
            $('#TXT_ROI_END_Y').val( ret4 );

            //window.alert("ROI Coordinate OK");
        },
        function(data)
        {
            //window.alert("ROI Coordinate NG");
        }
    );
}

//------------------------------------------------------
// Important Level for HEVC
//------------------------------------------------------
function get_roi_importantlevel()
{

    var args = {
        // Important Level for HEVC
        arg1: "0x81010400",
        arg2: "0x00000500",
        arg3: $(SEL_ROI_STREAM_NO).val(),
        arg4: $(SEL_ROI_AREA_NO).val(),
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // Important Level for HEVC
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Important Level for HEVC
            $('#SEL_ROI_IMPORTANT_LEVEL').val( ret1 );

            //window.alert("Important Level OK");
        },
        function(data)
        {
            //window.alert("Important Level NG");
        }
    );
}

//------------------------------------------------------
// QP Value for H264
//------------------------------------------------------
function get_roi_qpvalue()
{

    var args = {
        // QP Value for H264
        arg1: "0x81010400",
        arg2: "0x00000600",
        arg3: $(SEL_ROI_STREAM_NO).val(),
        arg4: $(SEL_ROI_AREA_NO).val(),
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // QP Value for H264
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // QP Value for H264
            $('#TXT_ROI_QP_DOWN_RANGE').val( ret1 );
            $('#TXT_ROI_QP_MIN').val( ret2 );

            //window.alert("QP Value OK");
        },
        function(data)
        {
            //window.alert("QP Value NG");
        }
    );
}

//------------------------------------------------------
// Quality Settings for HEVC
//------------------------------------------------------
function get_roi_qualitysetting()
{

    var args = {
        // Quality Settings for HEVC
        arg1: "0x81010400",
        arg2: "0x00000700",
        arg3: $(SEL_ROI_STREAM_NO).val(),
        arg4: "",
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // Quality Settings for HEVC
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Quality Settings for HEVC
            $('#TXT_ROI_DELTAQP').val( ret1 );
            $('#SEL_ROI_BASE_IMPORTANT_LEVEL').val( ret2 );

            //window.alert("ROI QualitySetting OK");
        },
        function(data)
        {
            //window.alert("ROI QualitySetting NG");
        }
    );
}

//------------------------------------------------------
// Outrange Setting for H264
//------------------------------------------------------
function get_roi_outrangesetting()
{

    var args = {
        // Outrange Setting for H264
        arg1: "0x81010400",
        arg2: "0x00000800",
        arg3: $(SEL_ROI_STREAM_NO).val(),
        arg4: "",
        arg5: "",
        arg6: ""
    };

    $.ajax({
        url: "/cgi-bin/webui/read.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            // Outrange Setting for H264
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Outrange Setting for H264
            $('#SEL_ROI_OUTSIDE_PARTICAL_CODIND_ENABLED').val( ret1 );
            $('#TXT_ROI_QP_UP_RANGE').val( ret2 );
            $('#TXT_ROI_QP_MAX').val( ret3 );

            //window.alert("Outrange Setting OK");
        },
        function(data)
        {
            //window.alert("Outrange Setting NG");
        }
    );
}

