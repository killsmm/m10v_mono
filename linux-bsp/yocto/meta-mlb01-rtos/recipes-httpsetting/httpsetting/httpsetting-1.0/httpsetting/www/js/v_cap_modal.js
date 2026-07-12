// JavaScript Document
var v_cap_en = false;

// -------------------------------------------------------------------
// Get video capture Modal's ID
// -------------------------------------------------------------------
function v_cap_modal_id()
{
    return '#VCapModal';
}

// -------------------------------------------------------------------
// SetUp video capture Modal
// -------------------------------------------------------------------
function v_cap_create_modal()
{
    // [Yossy memo]
    //  is not cool.
    // I have preserved intentionary the following code.
    // It seem better the modal is loading by jquery.
    // But only, Chrome is not working the load function on local files,
    // then this is unconvenience when debug.
    return '<!-- Modal -->\
            <div class="modal fade" id="VCapModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">\
                <div class="modal-dialog" style="width:400px; height:300px" role="document">\
                    <div class="modal-content">\
                        <div class="modal-header">\
                        <button type="button" class="close" data-dismiss="modal" aria-label="Close">\
                            <span aria-hidden="true">&times;</span>\
                        </button>\
                        <h5 class="modal-title" style="width:50%;" >Video Capture</h5>\
                        </div>\
                        <div class="modal-body">\
                            <textarea placeholder="log" class ="form-control" rows="5" id="vcap_log" readonly></textarea>\
                        </div>\
                        <div class="modal-footer">\
                            <button type="button" class="btn btn-primary" id="button_vcap_rec">Rec</button>\
                            <button type="button" class="btn btn-primary" id="button_vcap_stop">Stop</button>\
                            <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>\
                        </div>\
                    </div>\
                </div>\
            </div>';
}

// -------------------------------------------------------------------
// Clear message to log area
// -------------------------------------------------------------------
function v_cap_clearalert(txt)
{
    $('#vcap_log').val("");

    return;
}

// -------------------------------------------------------------------
// Add message to log area
// -------------------------------------------------------------------
function v_cap_addalert(txt)
{
    var str = $('#vcap_log').val();

    str = txt + "\n" + str;

    $('#vcap_log').val(str);

    return;
}

// -------------------------------------------------------------------
// Show loading Modal
// -------------------------------------------------------------------
function v_cap_show()
{
    v_cap_en = false;
    $(v_cap_modal_id()).modal();
    v_cap_clearalert();
    
    v_cap_reflect_rec_mode(false);
    v_cap_reflect_stop_mode(false);

    // Request SDCard Status to RTOS
    rtos_check_sd_card(function(data)
    {
        var param = {};
    
        // Check SDCard Status
        if (data.Status.hw_status == 0)
        {
            // SDCard is disconnected
            v_cap_addalert('Please Insert SD Card of Top Side for Capturing!');
        }

        v_cap_reflect_rec_mode(true);
    });
}

// -------------------------------------------------------------------
// Close video capture Modal
// -------------------------------------------------------------------
function v_cap_close()
{
    $(v_cap_modal_id()).modal("hide");
}

// -------------------------------------------------------------------
// Update stop button mode
// -------------------------------------------------------------------
function v_cap_reflect_stop_mode(enable)
{
	$("#button_vcap_stop").prop("disabled", !enable);
}

// -------------------------------------------------------------------
// Update rec button mode
// -------------------------------------------------------------------
function v_cap_reflect_rec_mode(enable)
{
	$("#button_vcap_rec").prop("disabled", !enable);
}

// -------------------------------------------------------------------
// Initialie modal
// -------------------------------------------------------------------
function v_cap_init()
{
    $('#button_vcap_rec').on('click', function() 
    {	
        var param = app_get_rtos_args();

        v_cap_reflect_rec_mode(false);

		rtos_check_sd_card(function(data)
		{
            // Check SDCard Status
            if (data.Status.hw_status == 0)
            {
                // SDCard is disconnected
                v_cap_addalert('Please Insert SD Card of Top Side for Capturing!');
                v_cap_reflect_rec_mode(true);
            }
            else
            {
                v_cap_en = true;
                v_cap_addalert('Please to wait...');

                rtos_v_capture_start(param, function(data)
                {
                    v_cap_addalert('Recording has started.');					
                    v_cap_reflect_stop_mode(true);
                });
            }
		});
    });

    $('#button_vcap_stop').on('click', function() 
    {	
        var param = app_get_rtos_args();
        
        v_cap_reflect_stop_mode(false);

		rtos_check_sd_card(function(data)
		{
            // Check SDCard Status
            if (data.Status.hw_status == 0)
            {
                // SDCard is disconnected
                v_cap_addalert('Please Insert SD Card of Top Side for Capturing!');
                v_cap_reflect_stop_mode(true);
            }
            else
            {
                v_cap_addalert('Please to wait...');

                rtos_v_capture_stop(param, function(data)
                {
                    v_cap_en = false;
                    v_cap_addalert('Success to recording!');			
                    v_cap_reflect_rec_mode(true);                       
                });
            }
		});
    });

    $(v_cap_modal_id()).on('hidden.bs.modal', function()
    {
        var param = app_get_rtos_args();
        
        if (v_cap_en)
        {
            rtos_check_sd_card(function(data)
            {
                s_cap_addalert('Please to wait...');

                // Check SDCard Status
                if (data.Status.hw_status == 0)
                {
                    rtos_v_capture_stop(param, function(data)
                    {
                        v_cap_addalert('Stop to recording.');			
                    });
                }
                else
                {
                    rtos_v_capture_stop(param, function(data)
                    {
                        v_cap_addalert('Success to recording!');			
                    });
                }
            });
        }
    });
}