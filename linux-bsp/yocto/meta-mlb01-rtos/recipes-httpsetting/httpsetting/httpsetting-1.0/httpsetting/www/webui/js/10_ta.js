var TAMPERING_ALERT_UI_ID = {
// ISP Option for Video
// Tampering Alert Button
    Button_tampering: {
        update: "#Btn_TAMPERING_ALERT",
        result1: "#SNI_ButtonRet1"
    },
// Tampering Alert Configuration for Surv
// Meta Button
    Button_ta_meta: {
        update: "#Btn_TA_META",
        result1: "#SNI_ButtonRet1"
    },
// Draw Alarm Button
    Button_ta_draw_alarm: {
        update: "#Btn_TA_DRAW_ALARM",
        result1: "#SNI_ButtonRet1"
    },
// GCD Threshold Button
    Button_gcd_threshold: {
        update: "#Btn_GCD_THRESHOLD",
        result1: "#SNI_ButtonRet1"
    },
    Text: {
        gcd: "#TXT_GCD_THRESHOLD"
    }
};


// Tampering Alert Button
$(TAMPERING_ALERT_UI_ID.Button_tampering.update).click(function()
{
    var args = {
        arg1: $(TAMPERING_ALERT).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/10_ta/tampering.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Tampering_Alert(Enabled) OK\n");
        },
        function(data)
        {
           window.alert("Tampering_Alert(Enabled) NG");
        }
    );

});

// TA Meta Button
$(TAMPERING_ALERT_UI_ID.Button_ta_meta.update).click(function()
{
    var args = {
        arg1: $(SEL_TA_META).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/10_ta/ta_meta.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            window.alert("Meta(Enabled) OK\n"); 
//            var res=data.result;
//            var err_cd=data.res2;
//            if(res==0){
//                window.alert("Meta(Enabled) OK\n");
//            }
//            else{
//                var err_msg="Meta(Enabled) NG "+err_cd;
//                window.alert(err_msg);
//            } 
        },
        function(data)
        {
//      Notice:Error return does not return soon.
            window.alert("Meta(Enabled) NG"); 
        }
    );
});

// TA Draw Alarm Button
$(TAMPERING_ALERT_UI_ID.Button_ta_draw_alarm.update).click(function()
{
    var args = {
        arg1: $(SEL_TA_DRAW_ALARM).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/10_ta/ta_drawalarm.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            window.alert("Draw_Alarm(Enabled) OK\n"); 
//            var res=data.result;
//            var err_cd=data.res2;
//            if(res==0){
//                window.alert("Draw_Alarm(Enabled) OK\n");
//            }
//            else{
//                var err_msg="Draw_Alarm(Enabled) NG "+err_cd;
//                window.alert(err_msg);
//            } 
        },
        function(data)
        {
//      Notice:Error return does not return soon.
            window.alert("Draw_Alarm(Enabled) NG"); 
        }
    );
});


// GCD Threshold Button
$(TAMPERING_ALERT_UI_ID.Button_gcd_threshold.update).click(function()
{
    var args = {
        arg1: $(TAMPERING_ALERT_UI_ID.Text.gcd).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/10_ta/ta_gcd_threshold.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            window.alert("GCD Threshold OK\n"); 
//            var res=data.result;
//            var err_cd=data.res2;
//            if(res==0){
//                window.alert(GCD Threshold OK\n");
//            }
//            else{
//                var err_msg="GCD Threshold NG "+err_cd;
//                window.alert(err_msg);
//            } 
        },
        function(data)
        {
//      Notice:Error return does not return soon.
            window.alert("GCD Threshold NG"); 
        }
    );
});

