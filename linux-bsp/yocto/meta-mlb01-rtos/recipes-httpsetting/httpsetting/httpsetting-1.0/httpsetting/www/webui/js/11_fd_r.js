//******************************************************
// Open the parameter area of Video Control.
//******************************************************
$(function()
{
    // Face Detection
    get_FaceDetection();
    wait();

    // Face Count
    get_facecount();
    wait();

    // Meta
    get_fd_meta();
    wait();

    // Draw Frame
    get_fd_drawframe();
    wait();

});

//******************************************************
// Change key parameter.
//******************************************************



//******************************************************
// Functiuon
//******************************************************
//------------------------------------------------------
// Face Detection
//------------------------------------------------------
function get_FaceDetection()
{

    var args = {
        // Face Detection
        arg1: "0x80010900",
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
            // Face Detection
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Face Detection
            $('#FACE_DETECTION_TRACKING').val( ret1 );

            //window.alert("Face Detection OK");
        },
        function(data)
        {
            //window.alert("Face Detection NG");
        }
    );
}

//------------------------------------------------------
// Face Count
//------------------------------------------------------
function get_facecount()
{

    var args = {
        // Face Count
        arg1: "0x80010A00",
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
            // Face Count
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Face Count
            $('#TXT_FACECOUNT').val( ret1 );

            //window.alert("Face Count OK");
        },
        function(data)
        {
            //window.alert("Face Count NG");
        }
    );
}

//------------------------------------------------------
// Meta
//------------------------------------------------------
function get_fd_meta()
{

    var args = {
        // Meta
        arg1: "0x81010B00",
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
            $('#SEL_FD_META').val( ret1 );

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
function get_fd_drawframe()
{

    var args = {
        // Draw Frame
        arg1: "0x81010B00",
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
            $('#SEL_FD_DRAW_FRAME').val( ret1 );

            //window.alert("Draw_Frame(Enabled) OK");
        },
        function(data)
        {
            //window.alert("Draw_Frame(Enabled) NG");
        }
    );
}

