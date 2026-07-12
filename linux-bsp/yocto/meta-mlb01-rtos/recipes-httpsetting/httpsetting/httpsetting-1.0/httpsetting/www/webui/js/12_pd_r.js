//******************************************************
// Open the parameter area of Video Control.
//******************************************************
$(function()
{
    // Person Detection
    get_person();
    wait();

    // Meta
    get_pd_meta();
    wait();

    // Draw Frame
    get_pd_drawframe();
    wait();

});

//******************************************************
// Change key parameter.
//******************************************************



//******************************************************
// Functiuon
//******************************************************
//------------------------------------------------------
// Person Detection
//------------------------------------------------------
function get_person()
{

    var args = {
        // Person Detection
        arg1: "0x80010900",
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
            // Person Detection
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Person Detection
            $('#PERSON_DETECTION').val( ret1 );

            //window.alert("Person Detection OK");
        },
        function(data)
        {
            //window.alert("Person Detection NG");
        }
    );
}

//------------------------------------------------------
// Meta
//------------------------------------------------------
function get_pd_meta()
{

    var args = {
        // Meta
        arg1: "0x81010C00",
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
            $('#SEL_PD_META').val( ret1 );

            //window.alert("Meta(Enabled) OK");
        },
        function(data)
        {
            //window.alert("Meta(Enabled) NG");
        }
    );
}

//------------------------------------------------------
// Draw Frame
//------------------------------------------------------
function get_pd_drawframe()
{

    var args = {
        // Draw Frame
        arg1: "0x81010C00",
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
            // Draw Frame
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Draw Frame
            $('#SEL_PD_DRAW_FRAME').val( ret1 );

            //window.alert("Draw_Frame(Enabled) OK");
        },
        function(data)
        {
            //window.alert("Draw_Frame(Enabled) NG");
        }
    );
}

