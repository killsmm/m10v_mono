// JavaScript Document
// -------------------------------------------------------------------
// Get loading Modal's ID
// -------------------------------------------------------------------
function loading_modal_id()
{
    return '#LoadingModal';
}

// -------------------------------------------------------------------
// SetUp loading Modal
// -------------------------------------------------------------------
function loading_create_modal()
{
    // [Yossy memo]
    //  is not cool.
    // I have preserved intentionary the following code.
    // It seem better the modal is loading by jquery.
    // But only, Chrome is not working the load function on local files,
    // then this is unconvenience when debug.
    return '<!-- Modal -->\
            <div class="modal fade" id="LoadingModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" style="text-align:center; padding-top:20%;">\
                <div class="modal-dialog" role="document">\
                    <img src="images/image_loading.gif" />\
                    <div style="margin-top: 0px; color: white">\
                        <b>Please wait</b>\
                    </div>\
                </div>\
            </div>';
}

// -------------------------------------------------------------------
// Show loading Modal
// -------------------------------------------------------------------
function loading_show()
{
    $(loading_modal_id()).modal();
}

// -------------------------------------------------------------------
// Close loading Modal
// -------------------------------------------------------------------
function loading_close()
{
    $(loading_modal_id()).modal("hide");
}