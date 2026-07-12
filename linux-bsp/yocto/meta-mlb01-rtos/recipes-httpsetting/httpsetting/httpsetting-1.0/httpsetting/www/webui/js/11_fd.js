var FACE_DETECTION_UI_ID = {
// ISP Option for Video
// Face Detection & Tracking(Enabled) Button
    Button_facedetection: {
        update: "#Btn_FACE_DETECTION_TRACKING",
        result1: "#SNI_ButtonRet1"
    },
// Face Detection Configuration
// Face Count Button
    Button_face_count: {
        update: "#Btn_FACECOUNT",
        result1: "#SNI_ButtonRet1"
    },
    Text: {
        count: "#TXT_FACECOUNT"
    },
// Face Detection Configuration for Surv
// Meta Button
    Button_fd_meta: {
        update: "#Btn_FD_META",
        result1: "#SNI_ButtonRet1"
    },
// Draw Frame Button
    Button_fd_draw_frame: {
        update: "#Btn_FD_DRAW_FRAME",
        result1: "#SNI_ButtonRet1"
    }
};


// Face Detection & Tracking(Enabled) Button
$(FACE_DETECTION_UI_ID.Button_facedetection.update).click(function()
{
    var args = {
        arg1: $(FACE_DETECTION_TRACKING).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/11_fd/FaceDetection.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert(" Face Detection ok\n");
        },
        function(data)
        {
           window.alert("NG");
        }
    );

});


// Face Count Button
$(FACE_DETECTION_UI_ID.Button_face_count.update).click(function()
{
    var args = {
        arg1: $(FACE_DETECTION_UI_ID.Text.count).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/11_fd/facecount.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
            window.alert("Face Count OK\n"); 
//            var res=data.result;
//            var err_cd=data.res2;
//            if(res==0){
//                window.alert("Face Count OK\n");
//            }
//            else{
//                var err_msg="Face Count NG "+err_cd;
//                window.alert(err_msg);
//            } 
        },
        function(data)
        {
//      Notice:Error return does not return soon.
            window.alert("Face Count NG"); 
        }
    );
});



// FD Meta Button
$(FACE_DETECTION_UI_ID.Button_fd_meta.update).click(function()
{
    var args = {
        arg1: $(SEL_FD_META).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/11_fd/fd_meta.cgi",
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

// FD Draw Frame Button
$(FACE_DETECTION_UI_ID.Button_fd_draw_frame.update).click(function()
{
    var args = {
        arg1: $(SEL_FD_DRAW_FRAME).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/11_fd/fd_drawframe.cgi",
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

