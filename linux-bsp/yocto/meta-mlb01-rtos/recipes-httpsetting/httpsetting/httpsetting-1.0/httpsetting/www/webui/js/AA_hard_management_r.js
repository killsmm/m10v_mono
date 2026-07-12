//******************************************************
// Open the parameter area of Hard Management.
//******************************************************
$(function()
{
   
    // Bad Block Threshold
    get_bb_threshold();
    wait();

});

//******************************************************
// Change key parameter.
//******************************************************



//******************************************************
// Functiuon
//******************************************************
//------------------------------------------------------
// Bad Block Threshold
//------------------------------------------------------
function get_bb_threshold()
{

    var args = {
        // Bad Block Threshold
        arg1: "0x80040500",
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
            // Bad Block Threshold
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Bad Block Threshold
            $('#TXT_BADBLOCK_THRESHOLD').val( ret1 );

            //window.alert("Bad Block Threshold OK");
        },
        function(data)
        {
            //window.alert("Bad Block Threshold NG");
        }
    );
}

