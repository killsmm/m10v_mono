var PERSON_DETECTION_UI_ID = {
// ISP Option for Video
// Person Detection Button
    Button_person: {
        update: "#Btn_PERSON_DETECTION",
        result1: "#SNI_ButtonRet1"
    },
// Person Detection Configuration for Surv
// Meta Button
    Button_pd_meta: {
        update: "#Btn_PD_META",
        result1: "#SNI_ButtonRet1"
    },
// Draw Frame Button
    Button_pd_draw_frame: {
        update: "#Btn_PD_DRAW_FRAME",
        result1: "#SNI_ButtonRet1"
    }
};


// PERSON Detection Button
$(PERSON_DETECTION_UI_ID.Button_person.update).click(function()
{
    var args = {
        arg1: $(PERSON_DETECTION).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/12_pd/person.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert(" Person Detection ok\n");
        },
        function(data)
        {
           window.alert("NG");
        }
    );

});

// PD Meta Button
$(PERSON_DETECTION_UI_ID.Button_pd_meta.update).click(function()
{
    var args = {
        arg1: $(SEL_PD_META).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/12_pd/pd_meta.cgi",
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

// PD Draw Frame Button
$(PERSON_DETECTION_UI_ID.Button_pd_draw_frame.update).click(function()
{
    var args = {
        arg1: $(SEL_PD_DRAW_FRAME).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/12_pd/pd_drawframe.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            window.alert("Draw_Frame(Enabled) OK\n"); 
//            var res=data.result;
//            var err_cd=data.res2;
//            if(res==0){
//                window.alert("Draw_Frame(Enabled) OK\n");
//            }
//            else{
//                var err_msg="Draw_Frame(Enabled) NG "+err_cd;
//                window.alert(err_msg);
//            } 
        },
        function(data)
        {
//      Notice:Error return does not return soon.
            window.alert("Draw_Frame(Enabled) NG"); 
        }
    );
});


