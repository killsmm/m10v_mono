var OSD_UI_ID = {
    Button_update: {
        update: "#Btn_OSD1",
	update0on: "#Btn_CAMINFO0_ON",
	update0off: "#Btn_CAMINFO0_OFF",
	update1on: "#Btn_CAMINFO1_ON",
	update1off: "#Btn_CAMINFO1_OFF",
	update2on: "#Btn_CAMINFO2_ON",
	update2off: "#Btn_CAMINFO2_OFF",
	update3on: "#Btn_CAMINFO3_ON",
	update3off: "#Btn_CAMINFO3_OFF",
        result1: "#SNI_ButtonRet1"
    },
    ComboBox_osd: {
        sel_mode: "#SEL_OSD",
        result : "#SNI_ComboRet"
    },
    Text: {
        arg3: "#TXT_OSDARG3",
        arg4: "#TXT_OSDARG4",
        arg7: "#TXT_OSDARG7"
    }
};
$(OSD_UI_ID.Text.arg1).on("change", function(event, ui)
{
    var outStr = $(this).val();
});
$(OSD_UI_ID.Text.arg2).on("change", function(event, ui)
{
    var outStr = $(this).val();
});
$(OSD_UI_ID.Text.arg3).on("change", function(event, ui)
{
    var outStr = $(this).val();
});
$(OSD_UI_ID.ComboBox_osd.sel_mode).change(function()
{
    var outStr = $(this).val();
  	var param = {};
    param.jsval = outStr;

    $.ajax({
        url: "/cgi-bin/webui/04_onscreendisplay/osd.cgi",
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

$(OSD_UI_ID.Button_update.update).click(function()
{
    var args = {
        arg1: $(SEL_OSDARG1).val(),
        arg2: $(SEL_OSDARG2).val(),
        arg3: $(OSD_UI_ID.Text.arg3).val(),
        arg4: $(OSD_UI_ID.Text.arg4).val(),
        arg5: $(SEL_OSDARG5).val(),
        arg6: $(SEL_OSDARG6).val(),
        arg7: $(OSD_UI_ID.Text.arg7).val(),
        arg8: $(SEL_OSDARG8).val()
    };
    var osdid = 0;
    var camid = 0;
    osdid = $(SEL_OSDARG1).val();
    camid = $(SEL_OSDARG2).val();

    if( osdid == 2 ){
        if( camid == 2147483647 ){
            IntoAjax( "2", "OFF" );
        }else{
            IntoAjax( "2", "ON" );
        }
    }
    else if( osdid == 5 ){
        if( camid == 2147483647 ){
            IntoAjax( "5", "OFF" );
        }else{
            IntoAjax( "5", "ON" );
        }
    }
    else if( osdid == 8 ){
        if( camid == 2147483647 ){
            IntoAjax( "8", "OFF" );
        }else{
            IntoAjax( "8", "ON" );
        }
    }
    else if( osdid == 11 ){
        if( camid == 2147483647 ){
            IntoAjax( "11", "OFF" );
        }else{
            IntoAjax( "11", "ON" );
        }
    }
    else{
    $.ajax({
        url: "/cgi-bin/webui/04_onscreendisplay/osd1.cgi",
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
    }
    // On Display
//    $(PRIVACY_UI_ID.Button.result1).text(data.result);
});

function IntoAjax(par0, par1)
{
    var args = {
        arg1: $(SEL_OSDARG1).val(),
        arg2: $(OSD_UI_ID.Text.arg3).val(),
        arg3: $(OSD_UI_ID.Text.arg4).val(),
        arg4: $(SEL_OSDARG5).val(),
        arg5: $(SEL_OSDARG6).val(),
        arg6: $(SEL_OSDARG8).val(),
        arg7: par1
    };

   $.ajax({
        url: "/cgi-bin/webui/04_onscreendisplay/osd2.cgi",
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
}

$(OSD_UI_ID.Button_update.update0on).click(function()
{
    IntoAjax("16","ON");
});
$(OSD_UI_ID.Button_update.update0off).click(function()
{
    IntoAjax("16","OFF");
});

$(OSD_UI_ID.Button_update.update1on).click(function()
{
    IntoAjax("17","ON");
});
$(OSD_UI_ID.Button_update.update1off).click(function()
{
    IntoAjax("17","OFF");
});

$(OSD_UI_ID.Button_update.update2on).click(function()
{
    IntoAjax("18","ON");
});
$(OSD_UI_ID.Button_update.update2off).click(function()
{
    IntoAjax("18","OFF");
});

$(OSD_UI_ID.Button_update.update3on).click(function()
{
    IntoAjax("19","ON");
});
$(OSD_UI_ID.Button_update.update3off).click(function()
{
    IntoAjax("19","OFF");
});
