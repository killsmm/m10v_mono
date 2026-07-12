var PRIVACY_UI_ID_R = {
// Area Select (Enabled)
    ComboBox_areaselect: {
        sel_mode: "#SEL_AREASELECT",
        result : "#SNI_ComboRet"
    },
// Disp Position
    ComboBox_areaselectno: {
        sel_mode: "#SEL_AREASELECTNO",
        result : "#SNI_ComboRet"
    }
};


//******************************************************
// Open the parameter area of Video Control.
//******************************************************
$(function()
{
    // Privacy Mask
    get_enable();
    wait();

    // Area Select (Enabled)
    get_areaselect();
    wait();

    // Disp Position
    get_update();
    wait();

    // Color
    get_color();
    wait();

});

//******************************************************
// Change key parameter.
//******************************************************
$(PRIVACY_UI_ID_R.ComboBox_areaselect.sel_mode).change(function()
{
    // Area Select (Enabled)
    get_areaselect();

    // Disp Position
    get_update();

    // Color
    get_color();

});

$(PRIVACY_UI_ID_R.ComboBox_areaselectno.sel_mode).change(function()
{
    // Area Select (Enabled)
    get_areaselect();

    // Disp Position
    get_update();

    // Color
    get_color();

});



//******************************************************
// Functiuon
//******************************************************

//------------------------------------------------------
// Privacy Mask
//------------------------------------------------------
function get_enable()
{

    var args = {
        // Privacy Mask
        arg1: "0x81010500",
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
            // Privacy Mask
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Privacy Mask
            $('#SEL_PRIVACYENABLE').val( ret1 );

            //window.alert("Privacy Mask Enable OK");
        },
        function(data)
        {
            //window.alert("Privacy Mask Enable NG");
        }
    );
}

//------------------------------------------------------
// Area Select (Enabled)
//------------------------------------------------------
function get_areaselect()
{

    var args = {
        // Area Select (Enabled)
        arg1: "0x81010500",
        arg2: "0x00000200",
        arg3: $(SEL_AREASELECT).val(),
        arg4: $(SEL_AREASELECTNO).val(),
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
            // Area Select (Enabled)
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Area Select (Enabled)
            $('#SEL_AREASELECTENABLE').val( ret1 );

            //window.alert("Area Select OK");
        },
        function(data)
        {
            //window.alert("Area Select NG");
        }
    );
}

//------------------------------------------------------
// Disp Position
//------------------------------------------------------
function get_update()
{

    var args = {
        // Disp Position
        arg1: "0x81010500",
        arg2: "0x00000300",
        arg3: $(SEL_AREASELECT).val(),
        arg4: $(SEL_AREASELECTNO).val(),
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
            // Disp Position
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Disp Position
            $('#TXT_PRIVACYX').val( ret1 );
            $('#TXT_PRIVACYY').val( ret2 );
            $('#TXT_PRIVACYWIDTH').val( ret3 );
            $('#TXT_PRIVACYHEIGHT').val( ret4 );

            //window.alert("Disp Position OK");
        },
        function(data)
        {
            //window.alert("Disp Position NG");
        }
    );
}

//------------------------------------------------------
// Color
//------------------------------------------------------
function get_color()
{

    var args = {
        // Color
        arg1: "0x81010500",
        arg2: "0x00000400",
        arg3: $(SEL_AREASELECT).val(),
        arg4: $(SEL_AREASELECTNO).val(),
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

            var col_ary = [
                [0, 0, 0],               // non (not supported)
                [126, 128, 128],
                [82, 90, 240],
                [41, 240, 110],
                [210, 16, 146],
                [145, 54, 34],
                [170, 166, 16],
                [107, 202, 222],
                [235, 128, 128]
            ];
            var col_ret = 0;        // non (not supported)
            for( var i = 0; i < col_ary.length; i++ ) {
               if( ret1==col_ary[i][0] && ret2==col_ary[i][1] && ret3==col_ary[i][2] ) {
                   // Color
                   col_ret = i;
                   break;
               }
            }
            $('#SEL_PRIVACYCOLOR').val( col_ret );

            //window.alert("Color OK");
        },
        function(data)
        {
            //window.alert("Color NG");
        }
    );
}
