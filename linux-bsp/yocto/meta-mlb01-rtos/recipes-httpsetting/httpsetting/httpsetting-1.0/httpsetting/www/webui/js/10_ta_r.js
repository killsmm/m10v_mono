//******************************************************
// Open the parameter area of Video Control.
//******************************************************
$(function()
{
    // Tampering Alert
    get_tampering();
    wait();

    // Meta
    get_ta_meta();
    wait();

    // Draw Alarm
    get_ta_drawalarm();
    wait();

    // GCD Threshold Button
    get_gcd_threshold();
    wait();

});

//******************************************************
// Change key parameter.
//******************************************************



//******************************************************
// Functiuon
//******************************************************
//------------------------------------------------------
// Tampering Alert
//------------------------------------------------------
function get_tampering()
{

    var args = {
        // Tampering Alert
        arg1: "0x80010900",
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
            // Tampering Alert
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Tampering Alert
            $('#TAMPERING_ALERT').val( ret1 );

            //window.alert("Tampering_Alert(Enabled) OK");
        },
        function(data)
        {
            //window.alert("Tampering_Alert(Enabled) NG");
        }
    );
}

//------------------------------------------------------
// Meta
//------------------------------------------------------
function get_ta_meta()
{

    var args = {
        // Meta
        arg1: "0x81010E00",
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
            // Meta
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Meta
            $('#SEL_TA_META').val( ret1 );

            //window.alert("Meta(Enabled) OK");
        },
        function(data)
        {
            //window.alert("Meta(Enabled) NG");
        }
    );
}

//------------------------------------------------------
// Draw Alarm
//------------------------------------------------------
function get_ta_drawalarm()
{

    var args = {
        // Draw Alarm
        arg1: "0x81010E00",
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
            // Draw Alarm
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Draw Alarm
            $('#SEL_TA_DRAW_ALARM').val( ret1 );

            //window.alert("Draw_Alarm(Enabled) OK");
        },
        function(data)
        {
            //window.alert("Draw_Alarm(Enabled) NG");
        }
    );
}

//------------------------------------------------------
// GCD Threshold Button
//------------------------------------------------------
function get_gcd_threshold()
{

    var args = {
        // Quality
        arg1: "0x81010E00",
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
            // Quality
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // GCD Threshold
            $('#TXT_GCD_THRESHOLD').val( ret1 );

            //window.alert("GCD Threshold OK");
        },
        function(data)
        {
            //window.alert("GCD Threshold NG");
        }
    );
}
