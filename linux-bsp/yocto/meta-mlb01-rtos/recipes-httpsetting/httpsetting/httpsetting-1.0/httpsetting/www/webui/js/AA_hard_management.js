var HARD_MANAGEMENT_UI_ID = {
// Bad Block Management
// Bad Block Threshold
    Button_bb_threshold: {
        update: "#Btn_BADBLOCK_THRESHOLD",
        result1: "#SNI_ButtonRet1"
    },
// Bad Block Threshold Text
    Text: {
        bb_threshold: "#TXT_BADBLOCK_THRESHOLD"
    }
};


// PERSON Detection Button
$(HARD_MANAGEMENT_UI_ID.Button_bb_threshold.update).click(function()
{
    var args = {
        arg1: $(HARD_MANAGEMENT_UI_ID.Text.bb_threshold).val()
    };

    $.ajax({
        url: "/cgi-bin/webui/AA_hard_management/bb_threshold.cgi",
        type: "POST",
        dataType: "json",
        contentType: "application/json",
        data: JSON.stringify(args),
        scriptCharset: "utf-8"
    }).then(
        function(data)
        {
           window.alert("Bad Block Threshold OK\n");
        },
        function(data)
        {
           window.alert("Bad Block Threshold NG");
        }
    );

});

