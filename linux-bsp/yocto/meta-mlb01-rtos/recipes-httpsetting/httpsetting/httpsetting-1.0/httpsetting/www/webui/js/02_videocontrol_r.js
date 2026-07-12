var VIDEO_CONTROL_UI_ID_R = {
// CC Matrix
    ComboBox_matrix_line: {
        sel_mode: "#SEL_MATRIX_LINE",
        result : "#SNI_ComboRet"
    }
};


//******************************************************
// Open the parameter area of Video Control.
//******************************************************
$(function()
{
    // EV Compensation
    get_EVCompensation();
    wait();

    // White Balance
    get_WhiteBalance();
    wait();

    // Color
    get_Color();
    wait();

    // CC Matrix
    get_CCMatrix();
    wait();

    // Sharpness
    get_Sharpness();
    wait();

    // ISO
    get_ISO();
    wait();

    // Exposure Time
    get_ExposureTime();
    wait();

    // AE Metering
    get_AEMetering();
    wait();

    // AE Coverage Speed
    get_AECoverageSpeed();
    wait();

    // Flicker
    get_Flicker();
    wait();

    // AE Lock
    get_AELock();
    wait();

    // EVP table No.
    get_EVPtableNo();
    wait();
    
    // AE/AWB Mode
    get_AEAWBMode();
    wait();

});

//******************************************************
// Change key parameter.
//******************************************************
$(VIDEO_CONTROL_UI_ID_R.ComboBox_matrix_line.sel_mode).change(function()
{
    // CC Matrix
    get_CCMatrix();
});



//******************************************************
// Functiuon
//******************************************************
//------------------------------------------------------
// EV Compensation
//------------------------------------------------------
function get_EVCompensation()
{

    var args = {
        // EV Compensation
        arg1: "0x80010100",
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
            // EV Compensation
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // EV Compensation
            $('#SEL_EV_COMPENSATION').val( ret1 );

            //window.alert("EVCompensation OK");
        },
        function(data)
        {
            //window.alert("EVCompensation NG");
        }
    );
}

//------------------------------------------------------
// White Balance
//------------------------------------------------------
function get_WhiteBalance()
{

    var args = {
        // White Balance
        arg1: "0x80010100",
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
            // White Balance
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // White Balance
            $('#SEL_WHITE_BALANCE').val( ret1 );
            $('#TXT_R_GAIN').val( '0x' + ret2.toString(16).toUpperCase() );
            $('#TXT_B_GAIN').val( '0x' + ret3.toString(16).toUpperCase() );

            //window.alert("WhiteBalance OK");
        },
        function(data)
        {
            //window.alert("WhiteBalance NG");
        }
    );
}

//------------------------------------------------------
// Color
//------------------------------------------------------
function get_Color()
{

    var args = {
        // Color
        arg1: "0x80010100",
        arg2: "0x00000300",
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
            // Color
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Color
            $('#SEL_COLOR').val( ret1 );

            //window.alert("color OK");
        },
        function(data)
        {
            //window.alert("color NG");
        }
    );
}

//------------------------------------------------------
// CC Matrix
//------------------------------------------------------
function get_CCMatrix()
{

    var args = {
        // CC Matrix
        arg1: "0x80010100",
        arg2: "0x00000400",
        arg3: $(SEL_MATRIX_LINE).val(),
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
            // CC Matrix
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // CC Matrix
            $('#TXT_YC0').val( '0x' + ret1.toString(16).toUpperCase() );
            $('#TXT_YC1').val( '0x' + ret2.toString(16).toUpperCase() );
            $('#TXT_YC2').val( '0x' + ret3.toString(16).toUpperCase() );

            //window.alert("Matrix OK");
        },
        function(data)
        {
            //window.alert("Matrix NG");
        }
    );
}

//------------------------------------------------------
// Sharpness
//------------------------------------------------------
function get_Sharpness()
{

    var args = {
        // Sharpness
        arg1: "0x80010100",
        arg2: "0x00000500",
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
            // Sharpness
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Sharpness
            $('#SEL_SHARPNESS').val( ret1 );
            $('#SEL_EDGE_STRENGTH').val( ret2 );

            //window.alert("Sharpness OK");
        },
        function(data)
        {
            //window.alert("Sharpness NG");
        }
    );
}

//------------------------------------------------------
// ISO
//------------------------------------------------------
function get_ISO()
{

    var args = {
        // ISO
        arg1: "0x80010100",
        arg2: "0x00000600",
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
            // ISO
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // ISO
            $('#SEL_ISO').val( ret1 );

            //window.alert("ISO OK");
        },
        function(data)
        {
            //window.alert("ISO NG");
        }
    );
}

//------------------------------------------------------
// Exposure Time
//------------------------------------------------------
function get_ExposureTime()
{

    var args = {
        // Exposure Time
        arg1: "0x80010100",
        arg2: "0x00000700",
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
            // Exposure Time
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Exposure Time
            $('#SEL_EXPOSURE_TIME').val( ret1 );

            //window.alert("Exposure Time OK");
        },
        function(data)
        {
            //window.alert("Exposure Time NG");
        }
    );
}

//------------------------------------------------------
// AE Metering
//------------------------------------------------------
function get_AEMetering()
{

    var args = {
        // AE Metering
        arg1: "0x80010100",
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
            // AE Metering
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // AE Metering
            $('#SEL_AE_METERING').val( ret1 );

            //window.alert("AE Metering OK");
        },
        function(data)
        {
            //window.alert("AE Metering NG");
        }
    );
}

//------------------------------------------------------
// AE Coverage Speed
//------------------------------------------------------
function get_AECoverageSpeed()
{

    var args = {
        // AE Coverage Speed
        arg1: "0x80010100",
        arg2: "0x00000900",
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
            // AE Coverage Speed
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // AE Coverage Speed
            $('#SEL_AE_COVERAGE_SPEED').val( ret1 );

            //window.alert("AE Coverage Speed OK");
        },
        function(data)
        {
            //window.alert("AE Coverage Speed NG");
        }
    );
}

//------------------------------------------------------
// Flicker
//------------------------------------------------------
function get_Flicker()
{

    var args = {
        // Flicker
        arg1: "0x80010100",
        arg2: "0x00000A00",
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
            // Flicker
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Flicker
            $('#SEL_FLICKER').val( ret1 );

            //window.alert("Flicker OK");
        },
        function(data)
        {
            //window.alert("Flicker NG");
        }
    );
}

//------------------------------------------------------
// AE Lock
//------------------------------------------------------
function get_AELock()
{

    var args = {
        // AE Lock
        arg1: "0x80010100",
        arg2: "0x00000B00",
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
            // AE Lock
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // AE Lock
            $('#SEL_AE_LOCK').val( ret1 );

            //window.alert("AE Lock OK");
        },
        function(data)
        {
            //window.alert("AE Lock NG");
        }
    );
}

//------------------------------------------------------
// EVP table No.
//------------------------------------------------------
function get_EVPtableNo()
{

    var args = {
        // EVP table No.
        arg1: "0x80010100",
        arg2: "0x00000D00",
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
            // EVP table No.
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // EVP table No.
            $('#TXT_EVP_TABLE_NO').val( ret1 );

            //window.alert("EVP Table OK");
        },
        function(data)
        {
            //window.alert("EVP Table NG");
        }
    );
}

//------------------------------------------------------
// AE/AWB Mode
//------------------------------------------------------
function get_AEAWBMode()
{

    var args = {
        // AE/AWB Mode
        arg1: "0x80010100",
        arg2: "0x00001100",
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
            // AE/AWB Mode
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // AE/AWB Mode
            $('#SEL_AEAWB_MODE').val( ret1 );
            $('#SEL_AEAWB_SENSOR').val( ret2 );

            //window.alert("AE/AWB Mode OK");
        },
        function(data)
        {
            //window.alert("AE/AWB Mode NG");
        }
    );
}
