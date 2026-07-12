// JavaScript Document
// -------------------------------------------------------------------
// Get still capture Modal's ID
// -------------------------------------------------------------------
function s_cap_modal_id()
{
    return '#SCapModal';
}

// -------------------------------------------------------------------
// SetUp still capture Modal
// -------------------------------------------------------------------
function s_cap_create_modal()
{
    // [Yossy memo]
    //  is not cool.
    // I have preserved intentionary the following code.
    // It seem better the modal is loading by jquery.
    // But only, Chrome is not working the load function on local files,
    // then this is unconvenience when debug.
    return '<!-- Modal -->\
            <div class="modal fade" id="SCapModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">\
                <div class="modal-dialog" style="width:400px; height:300px" role="document">\
                    <div class="modal-content">\
                        <div class="modal-header">\
                            <button type="button" class="close" data-dismiss="modal" aria-label="Close">\
                                <span aria-hidden="true">&times;</span>\
                            </button>\
                            <h5 class="modal-title" style="width:50%;" >Still Capture</h5>\
                        </div>\
                        <div class="modal-body">\
                            <textarea placeholder="log" class ="form-control" rows="5" id="scap_log" readonly></textarea>\
                        </div>\
                        <div class="modal-footer">\
                            <button type="button" class="btn btn-primary" id="button_scap_shot">Shot</button>\
                            <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>\
                        </div>\
                    </div>\
                </div>\
            </div>';
}

// -------------------------------------------------------------------
// Clear message to log area
// -------------------------------------------------------------------
function s_cap_clearalert(txt)
{
    $('#scap_log').val("");

    return;
}

// -------------------------------------------------------------------
// Add message to log area
// -------------------------------------------------------------------
function s_cap_addalert(txt)
{
    var str = $('#scap_log').val();

    str = txt + "\n" + str;

    $('#scap_log').val(str);

    return;
}

// -------------------------------------------------------------------
// Close still capture Modal
// -------------------------------------------------------------------
function s_cap_show()
{
    $(s_cap_modal_id()).modal();
    s_cap_clearalert();

    s_cap_reflect_shot_mode();

    // Request SDCard Status to RTOS
    rtos_check_sd_card(function(data)
    {
        var param = {};
    
        // Check SDCard Status
        if (data.Status.hw_status == 0)
        {
            // SDCard is disconnected
            s_cap_addalert('Please Insert SD Card of Top Side for Capturing!');
        }

        s_cap_reflect_shot_mode(true);
    });
}

// -------------------------------------------------------------------
// Close loading Modal
// -------------------------------------------------------------------
function s_cap_close()
{
    $(s_cap_modal_id()).modal("hide");
}

// -------------------------------------------------------------------
// Update shot button mode
// -------------------------------------------------------------------
function s_cap_reflect_shot_mode(enable)
{
	$("#button_scap_shot").prop("disabled", !enable);
}

// -------------------------------------------------------------------
// Initialie modal
// -------------------------------------------------------------------
function s_cap_init()
{
    $('#button_scap_shot').on('click', function() 
    {	
        var param = {};

        s_cap_reflect_shot_mode(false);

        // Request SDCard Status to RTOS
        rtos_check_sd_card(function(data)
        {
            var param = {};
            var resolution;

            // Check SDCard Status
            if (data.Status.hw_status == 0)
            {
                // SDCard is disconnected
                s_cap_addalert('Please Insert SD Card of Top Side for Capturing!');
                s_cap_reflect_shot_mode(true);
            }
            else
            {
                // SDCard is connected
                // Prepare the arguments
                resolution = $('#select_Control_CameraSetting_Resolution').val();
                
                param = app_get_rtos_args();
                param.camera_mode = 3;
                param.resolution = parseInt(resolution);

                s_cap_addalert('Please to wait...');

                // Request changing Camera Mode and capturing Picture to RTOS
                rtos_change_camera_mode(param, function(data)
                {
                    s_cap_addalert('Success to capture!');
                    s_cap_reflect_shot_mode(true);
                });
            }
        });
    });
}