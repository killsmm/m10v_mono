var M10V_UI_ID_R = {
// Crop Position
    ComboBox_cropstream: {
        sel_mode: "#SEL_CROPSTREAM",
        result : "#SNI_ComboRet"
    }
};


//******************************************************
// Open the parameter area of Video Control.
//******************************************************
$(function()
{
    // Video Image Stabilization
    get_VideoImageStabilization();
    wait();

    // Video HDR
    get_videohdr();
    wait();

    // 3DNR
    get_threednr();
    wait();

    // Backlight Compensation
    get_backlight();
    wait();

    // Simple Defog
    get_defog();
    wait();

    // Output Display
    get_display();
    wait();

    // SD Save On/Off
    get_sdsave();
    wait();

    // Crop Position
    get_SetCropStream();
    wait();

    // Center Offset
    get_offsetpixels();
    wait();

    // DOL Mode
    get_Changedolmode();
    wait();

});

//******************************************************
// Change key parameter.
//******************************************************
$(M10V_UI_ID_R.ComboBox_cropstream.sel_mode).change(function()
{
    // Crop Position
    get_SetCropStream();
});



//******************************************************
// Functiuon
//******************************************************
//------------------------------------------------------
// Video Image Stabilization
//------------------------------------------------------
function get_VideoImageStabilization()
{

    var args = {
        // Video Image Stabilization
        arg1: "0x80010900",
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
            // Video Image Stabilization
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Video Image Stabilization
            $('#VIDEO_IMAGE_STABILIZATION').val( ret1 );

            //window.alert("Video Image Stabilization OK");
        },
        function(data)
        {
            //window.alert("Video Image Stabilization NG");
        }
    );
}

//------------------------------------------------------
// Video HDR
//------------------------------------------------------
function get_videohdr()
{

    var args = {
        // Video HDR
        arg1: "0x80010900",
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
            // Video HDR
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Video HDR
            $('#VIDEO_HDR').val( ret1 );

            //window.alert("VIDEO_HDR OK");
        },
        function(data)
        {
            //window.alert("VIDEO_HDR NG");
        }
    );
}

//------------------------------------------------------
// 3DNR
//------------------------------------------------------
function get_threednr()
{

    var args = {
        // 3DNR
        arg1: "0x80010900",
        arg2: "0x00000400",
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
            // 3DNR
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // 3DNR
            $('#SEL_3DNR').val( ret1 );

            //window.alert("3DNR OK");
        },
        function(data)
        {
            //window.alert("3DNR NG");
        }
    );
}

//------------------------------------------------------
// Backlight Compensation
//------------------------------------------------------
function get_backlight()
{

    var args = {
        // Backlight Compensation
        arg1: "0x80010900",
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
            // Backlight Compensation
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Backlight Compensation
            $('#BACKLIGHT_COMPENSATION').val( ret1 );

            //window.alert("BACKLIGHT_COMPENSATION OK");
        },
        function(data)
        {
            //window.alert("BACKLIGHT_COMPENSATION NG");
        }
    );
}

//------------------------------------------------------
// Simple Defog
//------------------------------------------------------
function get_defog()
{

    var args = {
        // Simple Defog
        arg1: "0x80010900",
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
            // Simple Defog
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Simple Defog
            $('#DEFOG').val( ret1 );

            //window.alert("Simple Defog OK");
        },
        function(data)
        {
            //window.alert("Simple Defog NG");
        }
    );
}

//------------------------------------------------------
// Output Display
//------------------------------------------------------
function get_display()
{

    var args = {
        // Output Display
        arg1: "0x81010100",
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
            // Output Display
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            var dsp_ary = [
                // HDMI Enabled, LCD Enabled, Camera Select
                [0, 0, 1],      // OFF
                [1, 0, 1],      // HDMI - Front or Stitched Image
                [1, 0, 2],      // HDMI - Rear
                [1, 0, 3],      // HDMI - Left
                [1, 0, 4],      // HDMI - Right
                [0, 1, 1],      // LCD - Front or Stitched Image
                [0, 1, 2],      // LCD - Rear
                [0, 1, 3],      // LCD - Left
                [0, 1, 4]       // LCD - Right
            ];
            var dsp_ret = 0;        // non (not supported)
            for( var i = 0; i < dsp_ary.length; i++ ) {
               if( ret1==dsp_ary[i][0] && ret2==dsp_ary[i][1] && ret3==dsp_ary[i][2] ) {
                   // Display
                   dsp_ret = i;
                   break;
               }
            }

            // Output Display
            $('#SEL_DISPLAY').val( dsp_ret );

            //window.alert("Display OK");
        },
        function(data)
        {
            //window.alert("Display NG");
        }
    );
}

//------------------------------------------------------
// SD Save On/Off
//------------------------------------------------------
function get_sdsave()
{

    var args = {
        // SD Save On/Off
        arg1: "0x81010200",
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
            // SD Save On/Off
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // SD Save On/Off
            $('#SEL_SDSAVE').val( ret1 );

            //window.alert("SD save OK");
        },
        function(data)
        {
            //window.alert("SD save NG");
        }
    );
}

//------------------------------------------------------
// Crop Position
//------------------------------------------------------
function get_SetCropStream()
{

    var args = {
        // Crop Position
        arg1: "0x81010900",
        arg2: "0x00000100",
        arg3: $(SEL_CROPSTREAM).val(),
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
            $('#TXT_CROP_STREAM_X').val( ret1 );
            $('#TXT_CROP_STREAM_Y').val( ret2 );

            //window.alert("CropStream OK");
        },
        function(data)
        {
            //window.alert("CropStream NG");
        }
    );
}

//------------------------------------------------------
// Center Offset
//------------------------------------------------------
function get_offsetpixels()
{

    var args = {
        // Center Offset
        arg1: "0x81010A00",
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
            // Center Offset
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Center Offset
            $('#TXT_STITCH_OFFSET_PIXELS').val( ret1 );

            //window.alert("Center Offset OK");
        },
        function(data)
        {
            //window.alert("Center Offset NG");
        }
    );
}

//------------------------------------------------------
// DOL Mode
//------------------------------------------------------
function get_Changedolmode()
{

    var args = {
        // DOL Mode
        arg1: "0x81030200",
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
            // DOL Mode
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // DOL Mode
            $('#CHANGE_DOL_MODE').val( ret1 );

            //window.alert("DOL Mode OK");
        },
        function(data)
        {
            //window.alert("DOL Mode NG");
        }
    );
}
