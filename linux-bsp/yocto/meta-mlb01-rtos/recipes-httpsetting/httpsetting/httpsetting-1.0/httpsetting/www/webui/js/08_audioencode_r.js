//******************************************************
// Open the parameter area of Video Control.
//******************************************************
$(function()
{
    // Codec Mode
    get_acodecmode();
    wait();

    // Sampling Rate
    get_asamplingrate();
    wait();

    // Bitrate
    get_abitrate();
    wait();

    // Channel Type
    get_achanneltype();
    wait();

    // Audio Enable
    get_audioenable();
    wait();

    // Input Control
    get_input_ctrl();
    wait();

    // Input Volume
    get_input_vol();
    wait();

    // Output Control
    get_output_ctrl();
    wait();

    // Output Volume
    get_output_vol();
    wait();

    // Enabled(ON/OFF)
    get_audio_lr_enable();
    wait();

    // Sampling Rate
    get_audio_lr_sampling();
    wait();

    // Channel Type
    get_audio_lr_channeltype();
    wait();

});

//******************************************************
// Change key parameter.
//******************************************************



//******************************************************
// Functiuon
//******************************************************
//------------------------------------------------------
// Codec Mode
//------------------------------------------------------
function get_acodecmode()
{

    var args = {
        // Codec Mode
        arg1: "0x80010200",
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
            // Codec Mode
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Codec Mode
            $('#SEL_CODECMODE').val( ret1 );

            //window.alert("Codec Mode OK");
        },
        function(data)
        {
            //window.alert("Codec Mode NG");
        }
    );
}

//------------------------------------------------------
// Sampling Rate
//------------------------------------------------------
function get_asamplingrate()
{

    var args = {
        // Sampling Rate
        arg1: "0x80010200",
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
            // Sampling Rate
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Sampling Rate
            $('#SEL_ASAMPLINGRATE').val( ret1 );

            //window.alert("Sampling Rate OK");
        },
        function(data)
        {
            //window.alert("Sampling Rate NG");
        }
    );
}

//------------------------------------------------------
// Bitrate
//------------------------------------------------------
function get_abitrate()
{

    var args = {
        // Bitrate
        arg1: "0x80010200",
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
            // Bitrate
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            var bitrate_ary = [
                            32000,
                            40000,
                            48000,
                            64000,
                            96000,
                            112000,
                            160000,
                            192000
                          ];
            var bitrate_ret = 0;
            for( var i = 0; i < bitrate_ary.length; i++ ) {
               if( ret1==bitrate_ary[i] ) {
                   // Color
                   bitrate_ret = i;
                   break;
               }
            }

            // Bitrate
            $('#SEL_ABITRATE').val( bitrate_ret );

            //window.alert("Bit Rate OK");
        },
        function(data)
        {
            //window.alert("Bit Rate NG");
        }
    );
}

//------------------------------------------------------
// Channel Type
//------------------------------------------------------
function get_achanneltype()
{

    var args = {
        // Channel Type
        arg1: "0x80010200",
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
            // Channel Type
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Channel Type
            $('#SEL_ACHANNELTYPE').val( ret1 );

            //window.alert("Channel Type OK");
        },
        function(data)
        {
            //window.alert("Channel Type NG");
        }
    );
}

//------------------------------------------------------
// Audio Enable
//------------------------------------------------------
function get_audioenable()
{

    var args = {
        // Audio Enable
        arg1: "0x80010200",
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
            // Audio Enable
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Audio Enable
            $('#SEL_AUDIOENABLE').val( ret1 );

            //window.alert("Audio Enable OK");
        },
        function(data)
        {
            //window.alert("Audio Enable NG");
        }
    );
}

//------------------------------------------------------
// Input Control
//------------------------------------------------------
function get_input_ctrl()
{

    var args = {
        // Input Control
        arg1: "0x80010200",
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
            // Input Control
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Input Control
            $('#SEL_INPUT_CTRL').val( ret1 );

            //window.alert("Input Control OK");
        },
        function(data)
        {
            //window.alert("Input Control NG");
        }
    );
}

//------------------------------------------------------
// Input Volume
//------------------------------------------------------
function get_input_vol()
{

    var args = {
        // Input Volume
        arg1: "0x80010200",
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
            // Input Volume
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Input Volume
            $('#SEL_INPUT_VOL').val( ret1 );

            //window.alert("Input Volume OK");
        },
        function(data)
        {
            //window.alert("Input Volume NG");
        }
    );
}

//------------------------------------------------------
// Output Control
//------------------------------------------------------
function get_output_ctrl()
{

    var args = {
        // Output Control
        arg1: "0x80010200",
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
            // Output Control
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Output Control
            $('#SEL_OUTPUT_CTRL').val( ret1 );

            //window.alert("Output Control OK");
        },
        function(data)
        {
            //window.alert("Output Control NG");
        }
    );
}

//------------------------------------------------------
// Output Volume
//------------------------------------------------------
function get_output_vol()
{

    var args = {
        // Output Volume
        arg1: "0x80010200",
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
            // Output Volume
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Output Volume
            $('#SEL_OUTPUT_VOL').val( ret1 );

            //window.alert("Output Volume OK");
        },
        function(data)
        {
            //window.alert("Output Volume NG");
        }
    );
}

//------------------------------------------------------
// Enabled(ON/OFF)
//------------------------------------------------------
function get_audio_lr_enable()
{

    var args = {
        // Enabled(ON/OFF)
        arg1: "0x81010800",
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
            $('#SEL_LtoR_Enable').val( ret1 );

            //window.alert("LR Enable OK");
        },
        function(data)
        {
            //window.alert("LR Enable NG");
        }
    );
}

//------------------------------------------------------
// Sampling Rate
//------------------------------------------------------
function get_audio_lr_sampling()
{

    var args = {
        // Sampling Rate
        arg1: "0x81010800",
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
            // Sampling Rate
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Sampling Rate
            $('#SEL_LtoR_SAMPLINGRATE').val( ret1 );

            //window.alert("LR sampling OK");
        },
        function(data)
        {
            //window.alert("LR sampling NG");
        }
    );
}

//------------------------------------------------------
// Channel Type
//------------------------------------------------------
function get_audio_lr_channeltype()
{

    var args = {
        // Channel Type
        arg1: "0x81010800",
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
            // Channel Type
            var ret1=data.ret1;
            var ret2=data.ret2;
            var ret3=data.ret3;
            var ret4=data.ret4;

            // Channel Type
            $('#SEL_LtoR_CHANNELTYPE').val( ret1 );

            //window.alert("channel type OK");
        },
        function(data)
        {
            //window.alert("channel type NG");
        }
    );
}

