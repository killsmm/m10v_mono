//******************************************************
// Open the parameter area of Video Control.
//******************************************************
$(function()
{
    // OSD On/Off
    get_osd();
    wait();

});

//******************************************************
// Change key parameter.
//******************************************************



//******************************************************
// Functiuon
//******************************************************
//------------------------------------------------------
// OSD On/Off
//------------------------------------------------------
function get_osd()
{

    var args = {
        // OSD On/Off
        arg1: "0x81010300",
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
            // OSD On/Off
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // OSD On/Off
            $('#SEL_OSD').val( ret1 );

            //window.alert("OSD OK");
        },
        function(data)
        {
            //window.alert("OSD NG");
        }
    );
}
