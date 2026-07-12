var SD_WRITECOUNTER_UI_ID = {
// Get SD Write Counter Button
    Button_sd_writecounter: {
        get: "#Btn_GET_SD_WRITECOUNTER",
        result1: "#SNI_ButtonRet1"
    },
    Text: {
        counter: "#TXT_SD_WRITE_COUNTER"
    }
};


// SensorType Button
$(SD_WRITECOUNTER_UI_ID.Button_sd_writecounter.get).click(function()
{
    var param = {};

    $.ajax({
        url: "/cgi-bin/webui/AA_hard_management/get_writecounter.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(param),
        scriptCharset: "utf-8"
    }).then(
        function(result)
        {
//            window.alert(result.res2);
            $(SD_WRITECOUNTER_UI_ID.Text.counter).val(result.res3);
        },
        function(result)
        {
            window.alert("get counter NG");
        }
    );
});

