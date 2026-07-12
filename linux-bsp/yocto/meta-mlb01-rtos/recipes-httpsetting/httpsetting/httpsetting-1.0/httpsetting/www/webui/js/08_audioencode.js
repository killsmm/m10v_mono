var AUDIO_ENCODE_UI_ID = {
    ComboBox_audioenable: {
        sel_mode: "#SEL_AUDIOENABLE",
        result : "#SNI_ComboRet"
    },
    ComboBox_acodecmode: {
        sel_mode: "#SEL_CODECMODE",
        result : "#SNI_ComboRet"
    },
    ComboBox_input_ctrl: {
        sel_mode: "#SEL_INPUT_CTRL",
        result : "#SNI_ComboRet"
    },
    ComboBox_input_vol: {
        sel_mode: "#SEL_INPUT_VOL",
        result : "#SNI_ComboRet"
    },
    ComboBox_output_ctrl: {
        sel_mode: "#SEL_OUTPUT_CTRL",
        result : "#SNI_ComboRet"
    },
    ComboBox_output_vol: {
        sel_mode: "#SEL_OUTPUT_VOL",
        result : "#SNI_ComboRet"
    },
    ComboBox_asamplingrate: {
        sel_mode: "#SEL_ASAMPLINGRATE",
        result : "#SNI_ComboRet"
    },
    ComboBox_abitrate: {
        sel_mode: "#SEL_ABITRATE",
        result : "#SNI_ComboRet"
    },
    ComboBox_achanneltype: {
        sel_mode: "#SEL_ACHANNELTYPE",
        result : "#SNI_ComboRet"
    },
    Button_acodecmode: {
        update: "#Btn_ACODECMODE",
        result1: "#SNI_ButtonRet1"
    },
    Button_asamplingrate: {
        update: "#Btn_SAMPLINGRATE",
        result1: "#SNI_ButtonRet1"
    },
    Button_abitrate: {
        update: "#Btn_BITRATE",
        result1: "#SNI_ButtonRet1"
    },
    Button_achanneltype: {
        update: "#Btn_CHANNELTYPE",
        result1: "#SNI_ButtonRet1"
    },
   Button_audioenable: {
        update: "#Btn_AUDIOENABLE",
        result1: "#SNI_ButtonRet1"
    },
 
   Button_input_ctrl: {
        update: "#Btn_INPUT_CTRL",
        result1: "#SNI_ButtonRet1"
    },
   Button_input_vol: {
        update: "#Btn_INPUT_VOL",
        result1: "#SNI_ButtonRet1"
    },
   Button_output_ctrl: {
        update: "#Btn_OUTPUT_CTRL",
        result1: "#SNI_ButtonRet1"
    },
   Button_output_vol: {
        update: "#Btn_OUTPUT_VOL",
        result1: "#SNI_ButtonRet1"
    },

   Button_audio_lr_enable: {
        update: "#Btn_AUDIO_LR_ENABLE",
        result1: "#SNI_ButtonRet1"
    },
   Button_audio_lr_sampling: {
        update: "#Btn_AUDIO_LR_SAMPLINGRATE",
        result1: "#SNI_ButtonRet1"
    },
   Button_audio_lr_channeltype: {
        update: "#Btn_LR_CHANNELTYPE",
        result1: "#SNI_ButtonRet1"
    }

};

$(AUDIO_ENCODE_UI_ID.Button_audioenable.update).click(function()
{
    var args = {
        arg1: $(SEL_AUDIOENABLE).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/08_audioencode/audioenable.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );
});

$(AUDIO_ENCODE_UI_ID.Button_input_ctrl.update).click(function()
{
    var args = {
        arg1: $(SEL_INPUT_CTRL).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/08_audioencode/input_ctrl.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Input Control OK\n");
        },
        function(data)
        {
           window.alert("Input Control NG"); 
        }
    );
});

$(AUDIO_ENCODE_UI_ID.Button_input_vol.update).click(function()
{
    var args = {
        arg1: $(SEL_INPUT_VOL).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/08_audioencode/input_vol.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Input Volume OK\n");
        },
        function(data)
        {
           window.alert("Input Volume NG"); 
        }
    );
});

$(AUDIO_ENCODE_UI_ID.Button_output_ctrl.update).click(function()
{
    var args = {
        arg1: $(SEL_OUTPUT_CTRL).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/08_audioencode/output_ctrl.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Output Control OK\n");
        },
        function(data)
        {
           window.alert("Output Control NG"); 
        }
    );
});

$(AUDIO_ENCODE_UI_ID.Button_output_vol.update).click(function()
{
    var args = {
        arg1: $(SEL_OUTPUT_VOL).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/08_audioencode/output_vol.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Output Volume OK\n");
        },
        function(data)
        {
           window.alert("Output Volume NG"); 
        }
    );
});

$(AUDIO_ENCODE_UI_ID.Button_acodecmode.update).click(function()
{
    var args = {
        arg1: $(SEL_CODECMODE).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/08_audioencode/acodecmode.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Codec Mode OK\n");
        },
        function(data)
        {
           window.alert("Codec Mode NG"); 
        }
    );	
	
});

$(AUDIO_ENCODE_UI_ID.Button_asamplingrate.update).click(function()
{
    var args = {
        arg1: $(SEL_ASAMPLINGRATE).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/08_audioencode/asamplingrate.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );	
	
});

$(AUDIO_ENCODE_UI_ID.Button_abitrate.update).click(function()
{
    var args = {
        arg1: $(SEL_ABITRATE).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/08_audioencode/abitrate.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );	
	
});

$(AUDIO_ENCODE_UI_ID.Button_achanneltype.update).click(function()
{
    var args = {
        arg1: $(SEL_ACHANNELTYPE).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/08_audioencode/achanneltype.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );	
	
});

$(AUDIO_ENCODE_UI_ID.Button_audio_lr_enable.update).click(function()
{
    var args = {
        arg1: $(SEL_LtoR_Enable).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/08_audioencode/audio_lr_enable.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("LR Enable ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );	
	
});


$(AUDIO_ENCODE_UI_ID.Button_audio_lr_sampling.update).click(function()
{
    var args = {
        arg1: $(SEL_LtoR_SAMPLINGRATE).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/08_audioencode/audio_lr_sampling.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("LR sampling ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );	
	
});

$(AUDIO_ENCODE_UI_ID.Button_audio_lr_channeltype.update).click(function()
{
    var args = {
        arg1: $(SEL_LtoR_CHANNELTYPE).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/08_audioencode/audio_lr_channeltype.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("channel type ok\n");
        },
        function(data)
        {
           window.alert("NG"); 
        }
    );	
	
});
