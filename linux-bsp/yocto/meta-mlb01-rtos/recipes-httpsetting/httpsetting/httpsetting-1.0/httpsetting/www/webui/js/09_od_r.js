var OBJECT_DETECTION_UI_ID_R = {
// Function Area Conf
    combobox_function: {
        sel_mode: "#SEL_OD_FUNCTION",
        result : "#SNI_ComboRet"
    },

// Vertex Conf
    combobox_vertex_no: {
        sel_mode: "#SEL_OD_VERTEX_NO",
        result : "#SNI_ComboRet"
    },

    Text: {
        // CH
        ch: "#TXT_CH",
        // Area No
        no: "#TXT_AREANO",
        // Unmoving Conf
        um_ch: "#TXT_UNMOVING_CH",
        // Detection Size Conf
        ds_ch: "#TXT_DETECTION_SIZE_CH"
    }

}

//******************************************************
// Open the parameter area of Video Control.
//******************************************************
$(function()
{

    // Object Detection
    get_object();
    wait();

    // Meta
    get_od_meta();
    wait();

    // Draw Frame
    get_od_drawframe();
    wait();

    // Intrusion
    get_Intrusion();
    wait();

    // Crossline
    get_Crossline();
    wait();

    // Abandoned
    get_Abandoned();
    wait();

    // Theft
    get_Theft();
    wait();

    // Leaving
    get_Leaving();
    wait();

    // Function Area Conf
    get_Function_Area_Conf();
    wait();

    // Area Conf
    get_Area_Conf();
    wait();

    // Threshold Conf
    get_Threshold_Conf();
    wait();

    // Vertex Conf
    get_Vertex_Conf();
    wait();

    // Unmoving Conf
    get_Unmoving_Conf();
    wait();
    
    // Detection Size Conf
    get_Detection_Size_Conf();
    wait();

});

//******************************************************
// Change key parameter.
//******************************************************

$(OBJECT_DETECTION_UI_ID_R.combobox_function.sel_mode).change(function()
{
    // Function Area Conf
    get_Function_Area_Conf();

    // Area Conf
    get_Area_Conf();

    // Threshold Conf
    get_Threshold_Conf();

    // Vertex Conf
    get_Vertex_Conf();

});

$(OBJECT_DETECTION_UI_ID_R.Text.ch).change(function()
{
    // Function Area Conf
    get_Function_Area_Conf();

    // Area Conf
    get_Area_Conf();

    // Threshold Conf
    get_Threshold_Conf();

    // Vertex Conf
    get_Vertex_Conf();
});

$(OBJECT_DETECTION_UI_ID_R.Text.no).change(function()
{
    // Function Area Conf
    get_Function_Area_Conf();

    // Area Conf
    get_Area_Conf();

    // Threshold Conf
    get_Threshold_Conf();

    // Vertex Conf
    get_Vertex_Conf();
});

$(OBJECT_DETECTION_UI_ID_R.combobox_vertex_no.sel_mode).change(function()
{
    // Vertex Conf
    get_Vertex_Conf();
});

$(OBJECT_DETECTION_UI_ID_R.Text.um_ch).change(function()
{
    // Unmoving Conf
    get_Unmoving_Conf();
});

$(OBJECT_DETECTION_UI_ID_R.Text.ds_ch).change(function()
{
    // Detection Size Conf
    get_Detection_Size_Conf();
});

//******************************************************
// Functiuon
//******************************************************
//------------------------------------------------------
// Object Detection
//------------------------------------------------------
function get_object()
{

    var args = {
        // Object Detection
        arg1: "0x80010900",
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
            // Object Detection
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Object Detection
            $('#OBJECT_DETECTION').val( ret1 );

            //window.alert("Object_Detection(Enabled) OK");
        },
        function(data)
        {
            //window.alert("Object_Detection(Enabled) NG");
        }
    );
}

//------------------------------------------------------
// Meta
//------------------------------------------------------
function get_od_meta()
{

    var args = {
        // Meta
        arg1: "0x81010D00",
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
            $('#SEL_OD_META').val( ret1 );

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
function get_od_drawframe()
{

    var args = {
        // Draw Frame
        arg1: "0x81010D00",
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
            $('#SEL_OD_DRAW_FRAME').val( ret1 );

            //window.alert("Draw_Frame(Enabled) OK");
        },
        function(data)
        {
            //window.alert("Draw_Frame(Enabled) NG");
        }
    );
}

//------------------------------------------------------
// Intrusion
//------------------------------------------------------
function get_Intrusion()
{
    var args = {
        // Intrusion
        arg1: "0x81010D00",
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
            // Intrusion
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Intrusion
            $('#SEL_OD_INTRUSION').val( ret1 );

            //window.alert("Intrusion(Enabled) OK");
        },
        function(data)
        {
            //window.alert("Intrusion(Enabled) NG");
        }
    );
}

//------------------------------------------------------
// Crossline
//------------------------------------------------------
function get_Crossline()
{

    var args = {
        // Crossline
        arg1: "0x81010D00",
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
            // Crossline
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Crossline
            $('#SEL_OD_CROSSLINE').val( ret1 );

            //window.alert("Crossline(Enabled) OK");
        },
        function(data)
        {
            //window.alert("Crossline(Enabled) NG");
        }
    );
}

//------------------------------------------------------
// Abandoned
//------------------------------------------------------
function get_Abandoned()
{

    var args = {
        // Abandoned
        arg1: "0x81010D00",
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
            // Abandoned
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Abandoned
            $('#SEL_OD_ABANDONED').val( ret1 );

            //window.alert("Abandoned(Enabled) OK");
        },
        function(data)
        {
            //window.alert("Abandoned(Enabled) NG");
        }
    );
}

//------------------------------------------------------
// Theft
//------------------------------------------------------
function get_Theft()
{

    var args = {
        // Theft
        arg1: "0x81010D00",
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
            // Theft
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Theft
            $('#SEL_OD_THEFT').val( ret1 );

            //window.alert("Theft(Enabled) OK");
        },
        function(data)
        {
            //window.alert("Theft(Enabled) NG");
        }
    );
}

//------------------------------------------------------
// Leaving
//------------------------------------------------------
function get_Leaving()
{

    var args = {
        // Leaving
        arg1: "0x81010D00",
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
            // Leaving
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Leaving
            $('#SEL_OD_LEAVING').val( ret1 );

            //window.alert("Leaving(Enabled) OK");
        },
        function(data)
        {
            //window.alert("Leaving(Enabled) NG");
        }
    );
}

//------------------------------------------------------
// Function Area Conf
//------------------------------------------------------
function get_Function_Area_Conf()
{

    var args = {
        // Function Area Conf
        arg1: "0x81010D00",
        arg2: "0x00000800",
        arg3: $(SEL_OD_FUNCTION).val(),
        arg4: $(TXT_CH).val(),
        arg5: $(TXT_AREANO).val(),
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
            // Function Area Conf
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Function Area Conf
            $('#SEL_OD_FUNCTION').val( ret1 );
            $('#TXT_CH').val( ret2 );
            $('#TXT_AREANO').val( ret3 );
            $('#SEL_OD_FUNCTION_AREA_CONF').val( ret4 );

            //window.alert("Function Area Conf OK");
        },
        function(data)
        {
            //window.alert("Function Area Conf NG");
        }
    );
}

//------------------------------------------------------
// Area Conf
//------------------------------------------------------
function get_Area_Conf()
{

    var args = {
        // Area Conf
        arg1: "0x81010D00",
        arg2: "0x00000A00",
        arg3: $(SEL_OD_FUNCTION).val(),
        arg4: $(TXT_CH).val(),
        arg5: $(TXT_AREANO).val(),
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
            // Area Conf
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Area Conf
            $('#TXT_NUM_OF_VERTEX').val( ret1 );

            //window.alert("Area Conf OK");
        },
        function(data)
        {
            //window.alert("Area Conf NG");
        }
    );
}

//------------------------------------------------------
// Threshold Conf
//------------------------------------------------------
function get_Threshold_Conf()
{

    var args = {
        // Threshold Conf
        arg1: "0x81010D00",
        arg2: "0x00000B00",
        arg3: $(SEL_OD_FUNCTION).val(),
        arg4: $(TXT_CH).val(),
        arg5: $(TXT_AREANO).val(),
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
            // Threshold Conf
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Threshold Conf
            $('#TXT_THRESHOLD').val( ret1 );

            //window.alert("Threshold Conf OK");
        },
        function(data)
        {
            //window.alert("Threshold Conf NG");
        }
    );
}

//------------------------------------------------------
// Vertex Conf
//------------------------------------------------------
function get_Vertex_Conf()
{

    var args = {
        // Vertex Conf
        arg1: "0x81010D00",
        arg2: "0x00000C00",
        arg3: $(SEL_OD_FUNCTION).val(),
        arg4: $(TXT_CH).val(),
        arg5: $(TXT_AREANO).val(),
        arg6: $(SEL_OD_VERTEX_NO).val()
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
            // Vertex Conf
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Vertex Conf
            $('#SEL_OD_VERTEX_NO').val( ret1 );
            $('#TXT_X').val( ret2 );
            $('#TXT_Y').val( ret3 );

            //window.alert("Vertex Conf OK");
        },
        function(data)
        {
            //window.alert("Vertex Conf NG");
        }
    );
}

//------------------------------------------------------
// Unmoving Conf
//------------------------------------------------------
function get_Unmoving_Conf()
{

    var args = {
        // Unmoving Conf
        arg1: "0x81010D00",
        arg2: "0x00000900",
        arg3: $(TXT_UNMOVING_CH).val(),
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
            // Unmoving Conf
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Unmoving Conf
            $('#TXT_UNMOVING_CH').val( ret1 );
            $('#TXT_UNMOVING').val( ret2 );

            //window.alert("Unmoving Conf OK");
        },
        function(data)
        {
            //window.alert("Unmoving Conf NG");
        }
    );
}

//------------------------------------------------------
// Detection Size Conf
//------------------------------------------------------
function get_Detection_Size_Conf()
{

    var args = {
        // Detection Size Conf
        arg1: "0x81010D00",
        arg2: "0x00000D00",
        arg3: $(TXT_DETECTION_SIZE_CH).val(),
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
            // Detection Size Conf
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Detection Size Conf
            $('#TXT_DETECTION_SIZE_CH').val( ret1 );
            $('#SEL_OD_MIN_PIXEL').val( ret2 );

            //window.alert("Detection Size Conf OK");
        },
        function(data)
        {
            //window.alert("Detection Size Conf NG");
        }
    );
}

