var OBJECT_DETECTION_UI_ID = {
// ISP Option for Video
// Object Detection Button
    Button_object: {
        update: "#Btn_OBJECT_DETECTION",
        result1: "#SNI_ButtonRet1"
    },
// Object Detection Configuration for Surv
// Meta Button
    Button_od_meta: {
        update: "#Btn_OD_META",
        result1: "#SNI_ButtonRet1"
    },
// Draw Frame Button
    Button_od_draw_frame: {
        update: "#Btn_OD_DRAW_FRAME",
        result1: "#SNI_ButtonRet1"
    },
// Intrusion
    Button_od_intrusion: {
        update: "#Btn_OD_INTRUSION",
        result1: "#SNI_ButtonRet1"
    },
// Crossline
    Button_od_crossline: {
        update: "#Btn_OD_CROSSLINE",
        result1: "#SNI_ButtonRet1"
    },
// Abandoned
    Button_od_abandoned: {
        update: "#Btn_OD_ABANDONED",
        result1: "#SNI_ButtonRet1"
    },
// Theft
    Button_od_theft: {
        update: "#Btn_OD_THEFT",
        result1: "#SNI_ButtonRet1"
    },
// Leaving
    Button_od_leaving: {
        update: "#Btn_OD_LEAVING",
        result1: "#SNI_ButtonRet1"
    },
// Function Area Conf / Area Conf / Vertex Conf
    Button_od_function: {
        update: "#Btn_OD_FUNCTION",
        result1: "#SNI_ButtonRet1"
    },
// Unmoving Conf
    Button_od_unmoving: {
        update: "#Btn_OD_UNMOVING",
        result1: "#SNI_ButtonRet1"
    },
// Detection Size Conf
    Button_od_detection_size: {
        update: "#Btn_OD_DETECTION_SIZE",
        result1: "#SNI_ButtonRet1"
    }
};


// Object Detection Button
$(OBJECT_DETECTION_UI_ID.Button_object.update).click(function()
{
    var args = {
        arg1: $(OBJECT_DETECTION).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/09_od/object.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Object_Detection(Enabled) OK\n");
        },
        function(data)
        {
           window.alert("Object_Detection(Enabled) NG");
        }
    );

});

// OD Meta Button
$(OBJECT_DETECTION_UI_ID.Button_od_meta.update).click(function()
{
    var args = {
        arg1: $(SEL_OD_META).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/09_od/od_meta.cgi",
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

// OD Draw Frame Button
$(OBJECT_DETECTION_UI_ID.Button_od_draw_frame.update).click(function()
{
    var args = {
        arg1: $(SEL_OD_DRAW_FRAME).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/09_od/od_drawframe.cgi",
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

// OD Intrusion Button
$(OBJECT_DETECTION_UI_ID.Button_od_intrusion.update).click(function()
{
    var args = {
        arg1: $(SEL_OD_INTRUSION).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/09_od/od_intrusion.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            window.alert("Intrusion(Enabled) OK\n"); 
//            var res=data.result;
//            var err_cd=data.res2;
//            if(res==0){
//                window.alert("Intrusion(Enabled) OK\n");
//            }
//            else{
//                var err_msg="Intrusion(Enabled) NG "+err_cd;
//                window.alert(err_msg);
//            } 
        },
        function(data)
        {
//      Notice:Error return does not return soon.
            window.alert("Intrusion(Enabled) NG"); 
        }
    );
});

// OD Crossline Button
$(OBJECT_DETECTION_UI_ID.Button_od_crossline.update).click(function()
{
    var args = {
        arg1: $(SEL_OD_CROSSLINE).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/09_od/od_crossline.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            window.alert("Crossline(Enabled) OK\n"); 
//            var res=data.result;
//            var err_cd=data.res2;
//            if(res==0){
//                window.alert("Crossline(Enabled) OK\n");
//            }
//            else{
//                var err_msg="Crossline(Enabled) NG "+err_cd;
//                window.alert(err_msg);
//            } 
        },
        function(data)
        {
//      Notice:Error return does not return soon.
            window.alert("Crossline(Enabled) NG"); 
        }
    );
});

// OD Abandoned Button
$(OBJECT_DETECTION_UI_ID.Button_od_abandoned.update).click(function()
{
    var args = {
        arg1: $(SEL_OD_ABANDONED).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/09_od/od_abandoned.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            window.alert("Abandoned(Enabled) OK\n"); 
//            var res=data.result;
//            var err_cd=data.res2;
//            if(res==0){
//                window.alert("Abandoned(Enabled) OK\n");
//            }
//            else{
//                var err_msg="Abandoned(Enabled) NG "+err_cd;
//                window.alert(err_msg);
//            } 
        },
        function(data)
        {
//      Notice:Error return does not return soon.
            window.alert("Abandoned(Enabled) NG"); 
        }
    );
});

// OD Theft Button
$(OBJECT_DETECTION_UI_ID.Button_od_theft.update).click(function()
{
    var args = {
        arg1: $(SEL_OD_THEFT).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/09_od/od_theft.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            window.alert("Theft(Enabled) OK\n"); 
//            var res=data.result;
//            var err_cd=data.res2;
//            if(res==0){
//                window.alert("Theft(Enabled) OK\n");
//            }
//            else{
//                var err_msg="Theft(Enabled) NG "+err_cd;
//                window.alert(err_msg);
//            } 
        },
        function(data)
        {
//      Notice:Error return does not return soon.
            window.alert("Theft(Enabled) NG"); 
        }
    );
});

// OD Leaving Button
$(OBJECT_DETECTION_UI_ID.Button_od_leaving.update).click(function()
{
    var args = {
        arg1: $(SEL_OD_LEAVING).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/09_od/od_leaving.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            window.alert("Leaving(Enabled) OK\n"); 
//            var res=data.result;
//            var err_cd=data.res2;
//            if(res==0){
//                window.alert("Leaving(Enabled) OK\n");
//            }
//            else{
//                var err_msg="Leaving(Enabled) NG "+err_cd;
//                window.alert(err_msg);
//            } 
        },
        function(data)
        {
//      Notice:Error return does not return soon.
            window.alert("Leaving(Enabled) NG"); 
        }
    );
});

// OD Function Area Conf Button
$(OBJECT_DETECTION_UI_ID.Button_od_function.update).click(function()
{
    var args = {
        arg1: $(SEL_OD_FUNCTION).val(),
        arg2: $(TXT_CH).val(),
        arg3: $(TXT_AREANO).val(),
        arg4: $(SEL_OD_FUNCTION_AREA_CONF).val(),
        arg5: $(TXT_NUM_OF_VERTEX).val(),
        arg6: $(TXT_THRESHOLD).val(),
        arg7: $(SEL_OD_VERTEX_NO).val(),
        arg8: $(TXT_X).val(),
        arg9: $(TXT_Y).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/09_od/od_function.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            window.alert("Function Area Conf OK\n"); 
//            var res=data.result;
//            var err_cd=data.res2;
//            if(res==0){
//                window.alert("Function Area Conf OK\n");
//            }
//            else{
//                var err_msg="Function Area Conf NG "+err_cd;
//                window.alert(err_msg);
//            } 
        },
        function(data)
        {
//      Notice:Error return does not return soon.
            window.alert("Function Area Conf NG"); 
        }
    );
});

// OD Unmoving Conf Button
$(OBJECT_DETECTION_UI_ID.Button_od_unmoving.update).click(function()
{
    var args = {
        arg1: $(TXT_UNMOVING_CH).val(),
        arg2: $(TXT_UNMOVING).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/09_od/od_unmoving.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            window.alert("Unmoving Conf OK\n"); 
//            var res=data.result;
//            var err_cd=data.res2;
//            if(res==0){
//                window.alert("Unmoving Conf OK\n");
//            }
//            else{
//                var err_msg="Unmoving Conf NG "+err_cd;
//                window.alert(err_msg);
//            } 
        },
        function(data)
        {
//      Notice:Error return does not return soon.
            window.alert("Unmoving Conf NG"); 
        }
    );
});

// OD Detection Size Conf Button
$(OBJECT_DETECTION_UI_ID.Button_od_detection_size.update).click(function()
{
    var args = {
        arg1: $(TXT_DETECTION_SIZE_CH).val(),
        arg2: $(SEL_OD_MIN_PIXEL).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/09_od/od_detection_size.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            window.alert("Detection Size Conf OK\n"); 
//            var res=data.result;
//            var err_cd=data.res2;
//            if(res==0){
//                window.alert("Detection Size Conf OK\n");
//            }
//            else{
//                var err_msg="Detection Size Conf NG "+err_cd;
//                window.alert(err_msg);
//            } 
        },
        function(data)
        {
//      Notice:Error return does not return soon.
            window.alert("Detection Size Conf NG"); 
        }
    );
});

