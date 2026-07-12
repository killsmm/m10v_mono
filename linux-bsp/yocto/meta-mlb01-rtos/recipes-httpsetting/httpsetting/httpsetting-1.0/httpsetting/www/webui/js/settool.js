/***
 * JavaScript Document sample
 * There are no main functions.
 * When it's loaded by a HTML,it's processed from the top
 * If the function isn't called specifically, it's ignored.
***/

// Variable
// The optional type can be input
// Input values must be managed correctly
var UI_ID = {
    Button: {
        cmdboot: "#BtnBOOT",
        cmdidle: "#BtnIDLE",
	cmdprev: "#BtnPREV",
        cmdvcap: "#BtnVCAP",
        cmdpmgrsave: "#BtnSAVE",
        cmdpmgrerase: "#BtnERASE",
        cmdreset: "#Btn_UPDATE_SOFTWARE_RESET",  // Software Reset
        result1: "#SNI_ButtonRet1",
        result2: "#SNI_ButtonRet2"
    },
    Radio: {
        sample: "#SNI_Radio01",
        result: "#SNI_RadioRet"
    },
    CheckBox: {
        sample: "#SNI_CheckBox01",
        result: "#SNI_CheckRet"
    },
    Text: {
        m10vipadd: "#M10V_IPadd",
        pcmac	 : "#PC_MAC",
        pcipadd  : "#PC_IPadd",
        result1  : "#SNI_Text01Ret",
        result2  : "#SNI_Text02Ret",
        result3  : "#SNI_Text03Ret"
    },
	/* parameter setting */
    ComboBox_hdr: {
        sel_mode: "#SEL_HDR",
        result : "#SNI_ComboRet"
    },
    ComboBox_stitching: {
        sel_mode: "#SEL_STITCHING",
        result : "#SNI_ComboRet"
    },
	/* Streaming(Viewer) setting */
    ComboBox_changemode: {
        sel_mode: "#SEL_CHANGEMODE",
        result : "#SNI_ComboRet"
    },
    ComboBox_interval: {
        sel_mode: "#SEL_INTERVAL",
        result : "#SNI_ComboRet"
    },
    ComboBox_attachedboard: {
        sel_mode: "#SEL_ATTACHEDBOARD",
        result : "#SNI_ComboRet"
    }
};

// function
/**
 * SNI_ButtonClickedHandler :
 *   button click handler
 * @param {String} uiId (buttonUI's ID)
 */
function SNI_ButtonClickedHandler(uiId)
{
    var outStr = "none";

    // get UI character
    switch (uiId)
    {
        case UI_ID.Button.cmdboot:
            outStr = "Get Status";
            break;
        case UI_ID.Button.cmdprev:
            outStr = "Button sample02";
            break;
        case UI_ID.Button.cmdvcap:
            outStr = "Button sample03";
            break;
        default:
            break;
    }

    // On Display
    $(UI_ID.Button.result).text(outStr);

    return;
}

/**
 * SNI_AjaxSample :
 *   Ajax call sample.It will fail if there is no server
 * @param {String} value01 (send param1)
 * @param {String} value02 (send param2)
 */
function SNI_AjaxSample(value01, value02)
{
    var args = {
        data0: value01,
        data1: value02
    };
    window.alert("The character to send to the server:\n" + JSON.stringify(args));

    $.ajax({
        url: "/cgi-bin/webui/boot.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Success\n"
           + "Receive character :\n"
           + "data.result = " + data.result + "\n"
           + "data.value = " + data.value + "\n");
        },
        function(data)
        {
           window.alert("Error"); 
        }
    );
    // On Display
    $(UI_ID.Button.result1).text(data.result);
}

// Event Handler
// Boot Button 
$(UI_ID.Button.cmdboot).click(function()
{
    var args = {
        m10vipadd: $(UI_ID.Text.m10vipadd).val(),
        pcmac    : $(UI_ID.Text.pcmac).val(),
        pcipadd  : $(UI_ID.Text.pcipadd).val()
    };
	window.alert("initialize and boot start\n");

    $.ajax({
        url: "/cgi-bin/webui/boot_eth.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("initileze and boot OK\n"
           + "preview mode");
        },
        function(data)
        {
//      Notice:Error return does not return soon.
//           window.alert("initileze and boot NG"); 
        }
    );
    // On Display	
//    $(UI_ID.Button.result1).text(data.result);
});

// IDLE mode Button 
$(UI_ID.Button.cmdidle).click(function()
{
  	var param = {};

	param.camera_mode = 1;
    $.ajax({
        url: "/cgi-bin/webui/change_senmode.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(param),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("IDLE mode"); 
        },
        function(data)
        {
           window.alert("mode change NG"); 
        }
    );
    // On Display	
    $(UI_ID.Button.cmdidle).text(data.result);
});

// Preview mode(VideoCap stop) Button 
$(UI_ID.Button.cmdprev).click(function()
{
  	var param = {};
	param.camera_mode = 2;
    $.ajax({
        url: "/cgi-bin/webui/change_senmode.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(param),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Preview mode"); 
        },
        function(data)
        {
           window.alert("mode change NG"); 
        }
    );
    // On Display	
//    $(UI_ID.Button.result2).text(data.result);
});

// VideoCap start Button 
$(UI_ID.Button.cmdvcap).click(function()
{
  	var param = {};
	param.camera_mode = 8;
    $.ajax({
        url: "/cgi-bin/webui/change_senmode.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(param),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Video capture mode"); 
        },
        function(data)
        {
           window.alert("mode change NG"); 
        }
    );
    // On Display	
//    $(UI_ID.Button.result2).text(data.result);
});

$(UI_ID.Radio.sample + " input[name=radio_data]:radio").change(function()
{
    var radioVal = $(UI_ID.Radio.sample + " input[name=radio_data]:radio:checked").val();
    var outStr = "RadioButton" + radioVal;

    $(UI_ID.Radio.result).text(outStr);
});

$(UI_ID.CheckBox.sample + " input[name=check_data]:checkbox").change(function()
{
    var checkVal = $(UI_ID.CheckBox.sample + " input[name=check_data]:checkbox:checked").map(function(){
        return $(this).val();
    }).get();
    var outStr = "";

    checkVal.forEach(function(value, index, array) {
        outStr = outStr + "CheckBox" + value + ",";
    });

    $(UI_ID.CheckBox.result).text(outStr);
});

$(UI_ID.Text.m10vipadd).on("change", function(event, ui)
{
    var outStr = $(this).val();

    $(UI_ID.Text.result1).text(outStr);
});

$(UI_ID.Text.pcmac).on("change", function(event, ui)
{
    var outStr = $(this).val();

    $(UI_ID.Text.result2).text(outStr);
});

$(UI_ID.Text.pcipadd).on("change", function(event, ui)
{
    var outStr = $(this).val();

    $(UI_ID.Text.result3).text(outStr);
});

$(UI_ID.ComboBox_hdr.sel_mode).change(function()
{
    var outStr = $(this).val();
  	var param = {};
    param.hdr_mode = outStr;

    $.ajax({
        url: "/cgi-bin/webui/hdr.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(param),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("HDR change ok\n");
        },
        function(data)
        {
           window.alert("HDR change NG"); 
        }
    );	
	
});
$(UI_ID.ComboBox_stitching.sel_mode).change(function()
{
    var outStr = $(this).val();
  	var param = {};
    param.jsval = outStr;

    $.ajax({
        url: "/cgi-bin/webui/stitching.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(param),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Stitching change ok\n");
        },
        function(data)
        {
           window.alert("Stitching change NG"); 
        }
    );	
});
$(UI_ID.ComboBox_changemode.sel_mode).change(function()
{
    var outStr = $(this).val();
  	var param = {};
    param.jsval = outStr;

    $.ajax({
        url: "/cgi-bin/webui/changemode.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(param),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("MainOut change ok\n");
        },
        function(data)
        {
           window.alert("MainOut change NG"); 
        }
    );	
	
});
$(UI_ID.ComboBox_interval.sel_mode).change(function()
{
    var outStr = $(this).val();
  	var param = {};
    param.jsval = outStr;

    $.ajax({
        url: "/cgi-bin/webui/timelapse.cgi",
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
$(UI_ID.ComboBox_attachedboard.sel_mode).change(function()
{
    var outStr = $(this).val();
  	var param = {};
    param.jsval = outStr;

    $.ajax({
        url: "/cgi-bin/webui/attachedsensor.cgi",
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

// Parameter Management Save Button
$(UI_ID.Button.cmdpmgrsave).click(function()
{
    var param = {};
    var ret_cd = true;

    ret_cd = window.confirm("Save the setting values.");  // Dialog

    if( ret_cd == false){  // Canncel Bottun
        return;
    }

    $.ajax({
        url: "/cgi-bin/webui/pmgr_save.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(param),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
                if( data.result == 0){
                    window.alert("OK\nSaved the setting values.\n");
                }else{
                    window.alert("NG\nSave failed.\n");
                }

        },
        function(data)
        {
            window.alert("NG");
        }
    );
    // On Display
//    $(UI_ID.Button.result2).text(data.result);
});

// Parameter Management Erase Button
$(UI_ID.Button.cmdpmgrerase).click(function()
{
    var param = {};
    var ret_cd = true;

    ret_cd = window.confirm("Erase the setting values.\nM10V needs to reboot for creating DEFAULT.CSV after erasing.\n");  // Dialog

    if( ret_cd == false){  // Canncel Bottun
        return;
    }

    $.ajax({
        url: "/cgi-bin/webui/pmgr_erase.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(param),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
                if( data.result == 0){
                    window.alert("OK\nDeleted DEFAULT.CSV and PMGR.CSV.\nPlease reboot and create DEFAULT.CSV before clicking the Save button.\n");
                }else{
                    window.alert("NG\nErase failed.\n");
                }

        },
        function(data)
        {
            window.alert("NG");
        }
    );
    // On Display
//    $(UI_ID.Button.result2).text(data.result);
});



// Softwere Reset Button
$(UI_ID.Button.cmdreset).click(function()
{
    var param = {};
    var ret_cd = true;

    ret_cd = window.confirm("Software Reset");  // Dialog

    if( ret_cd == false){  // Canncel Bottun
        return;
    }

    $.ajax({
        url: "/cgi-bin/webui/software_reset.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(param),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
//            window.alert("Softwere Reset OK");
        },
        function(data)
        {
//            window.alert("NG");
        }
    );
    // On Display
//    $(UI_ID.Button.result2).text(data.result);
});


