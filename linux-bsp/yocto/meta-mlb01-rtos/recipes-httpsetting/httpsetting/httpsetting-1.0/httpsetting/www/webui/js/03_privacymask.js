var PRIVACY_UI_ID = {
    Button: {
        update: "#Btn_UPDATE",
        result1: "#SNI_ButtonRet1",
        result2: "#SNI_ButtonRet2",
        result3: "#SNI_ButtonRet3",
        result4: "#SNI_ButtonRet4"
    },
    Button_area: {
        update: "#Btn_AREAUPDATE",
        result1: "#SNI_ButtonRet1"
    },
    Button_privacycolor: {
        update: "#Btn_PM_COLOR",
        result1: "#SNI_ButtonRet1"
    },
    Text: {
        arg1: "#TXT_PRIVACYX",
        arg2: "#TXT_PRIVACYY",
        arg3: "#TXT_PRIVACYWIDTH",
        arg4: "#TXT_PRIVACYHEIGHT",
        result1  : "#SNI_Text01Ret",
        result2  : "#SNI_Text02Ret",
        result3  : "#SNI_Text03Ret"
    },
    ComboBox_privacyenable: {
        sel_mode: "#SEL_PRIVACYENABLE",
        result : "#SNI_ComboRet"
    },
    ComboBox_areaselect: {
        sel_mode: "#SEL_AREASELECT",
        result : "#SNI_ComboRet"
    },
    ComboBox_areaselectno: {
        sel_mode: "#SEL_AREASELECTNO",
        result : "#SNI_ComboRet"
    },
    ComboBox_areaselectenable: {
        sel_mode: "#SEL_AREASELECTENABLE",
        result : "#SNI_ComboRet"
    }
};
$(PRIVACY_UI_ID.Text.arg1).on("change", function(event, ui)
{
    var outStr = $(this).val();

    $(PRIVACY_UI_ID.Text.result1).text(outStr);
});
$(PRIVACY_UI_ID.Text.arg2).on("change", function(event, ui)
{
    var outStr = $(this).val();

    $(PRIVACY_UI_ID.Text.result2).text(outStr);
});
$(PRIVACY_UI_ID.Text.arg3).on("change", function(event, ui)
{
    var outStr = $(this).val();

    $(PRIVACY_UI_ID.Text.result3).text(outStr);
});
$(PRIVACY_UI_ID.Text.arg4).on("change", function(event, ui)
{
    var outStr = $(this).val();

    $(PRIVACY_UI_ID.Text.result4).text(outStr);
});
$(PRIVACY_UI_ID.Button.update).click(function()
{
    var args = {
        arg1: $(PRIVACY_UI_ID.Text.arg1).val(),
        arg2: $(PRIVACY_UI_ID.Text.arg2).val(),
        arg3: $(PRIVACY_UI_ID.Text.arg3).val(),
        arg4: $(PRIVACY_UI_ID.Text.arg4).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/03_privacymask/update.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("OK\n");
        },
        function(data)
        {
//      Notice:Error return does not return soon. 
//           window.alert("initileze and boot NG"); 
        }
    );
    // On Display
//    $(PRIVACY_UI_ID.Button.result1).text(data.result);
});
$(PRIVACY_UI_ID.Button_area.update).click(function()
{
    var args = {
        arg1: $(SEL_AREASELECT).val(),
        arg2: $(SEL_AREASELECTNO).val(),
        arg3: $(SEL_AREASELECTENABLE).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/03_privacymask/areaselect.cgi",
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
    // On Display	
//    $(PRIVACY_UI_ID.Button.result1).text(data.result);
});

$(PRIVACY_UI_ID.ComboBox_privacyenable.sel_mode).change(function()
{
    var outStr = $(this).val();
  	var param = {};
    param.jsval = outStr;

    $.ajax({
        url: "/cgi-bin/webui/03_privacymask/enable.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(param),
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
$(PRIVACY_UI_ID.Button_privacycolor.update).click(function()
{
    var outStr = $(SEL_PRIVACYCOLOR).val();
  	var param = {};
    param.jsval = outStr;

    $.ajax({
        url: "/cgi-bin/webui/03_privacymask/color.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(param),
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
