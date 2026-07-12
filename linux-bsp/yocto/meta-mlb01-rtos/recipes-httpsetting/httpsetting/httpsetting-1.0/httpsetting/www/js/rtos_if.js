// JavaScript Document
// -------------------------------------------------------------------
// CGI Debug print
// -------------------------------------------------------------------
function rtos_debug_alert(data)
{
    if (DEBUG_CGI)
    {
        window.alert(JSON.stringify(data));
	}
}

// -------------------------------------------------------------------
// CGI Debug error print
// -------------------------------------------------------------------
function rtos_debug_error_alert(data)
{
	if (DEBUG_CGI)
    {
	    window.alert(
            'error!!!\n'
            + JSON.stringify(data));
    }
}

// -------------------------------------------------------------------
// Get the arguments for RTOS API
// -------------------------------------------------------------------
function rtos_args(camera_mode, resolution, iso, fps, wb_mode)
{
   	var param = {};

	param.camera_mode = parseInt(camera_mode);//2:video 3:photo
	param.resolution = parseInt(resolution);
	param.iso = parseInt(iso);
	param.fps = parseInt(fps);
   	param.wb_mode = parseInt(wb_mode);
	param.iq_bin_index = rtos_get_iq_bin_index(param);
	param.ae_bin_index = rtos_get_ae_bin_index(param);

    return param;
}

// -------------------------------------------------------------------
// Get AE's IQ.bin index(for IPCU command param)
// -------------------------------------------------------------------
function rtos_get_ae_bin_index(param)
{
    param.ae_bin_index = env_get_ae_bin_index(param.camera_mode, param.resolution);
	
	return param.ae_bin_index;	
}

// -------------------------------------------------------------------
// Get IQ's IQ.bin index(for IPCU command param)
// -------------------------------------------------------------------
function rtos_get_iq_bin_index(param)
{
    param.iq_bin_index = env_get_iq_bin_index(param.camera_mode, param.resolution);
	
	return param.iq_bin_index;	
}

// -------------------------------------------------------------------
// Request SDCard Stauts to RTOS
// -------------------------------------------------------------------
function rtos_check_sd_card(func)
{
    var param = {};

    // Request to RTOS
    rtos_request_core(
        "cgi-bin/get_hardware_status.cgi",
        param,
        function(data)
        {
            // Debug print
            rtos_debug_alert(data);

            // Callback
            func(data);
        },
		rtos_debug_error_alert);
}

// -------------------------------------------------------------------
// Request changing Camera Mode to RTOS
// -------------------------------------------------------------------
function rtos_change_camera_mode(param, func)
{
    // Request to RTOS
    rtos_request_core(
        "cgi-bin/change_sensormode.cgi",
        param,
        function(data)
        {
            // Debug print
            rtos_debug_alert(data);

            // Callback
            func(data);
        },
		function(data)
        {
            // Debug print
            rtos_debug_error_alert(data);
        });
}

// -------------------------------------------------------------------
// Request changing Resolution to RTOS
// -------------------------------------------------------------------
function rtos_change_resolution(param, func)
{
    // Request to RTOS
    rtos_request_core(
        "cgi-bin/change_sensorresolution.cgi",
        param,
        function(data)
        {
            // Debug print
            rtos_debug_alert(data);

            // Callback
            func(data);
        },
		function(data)
        {
            // Debug print
            rtos_debug_error_alert(data);
        });
}

// -------------------------------------------------------------------
// Request changing ISO to RTOS
// -------------------------------------------------------------------
function rtos_change_iso(param, func)
{
    // Request to RTOS
    rtos_request_core(
        "cgi-bin/change_sensoriso.cgi",
        param,
        function(data)
        {
            // Debug print
            rtos_debug_alert(data);

            // Callback
            func(data);
        },
		function(data)
        {
            // Debug print
            rtos_debug_error_alert(data);
        });
}

// -------------------------------------------------------------------
// Request changing WB Mode to RTOS
// -------------------------------------------------------------------
function rtos_change_wbmode(param, func)
{
    // Request to RTOS
    rtos_request_core(
        "cgi-bin/change_wbmode.cgi",
        param,
        function(data)
        {
            // Debug print
            rtos_debug_alert(data);

            // Callback
            func(data);
        },
		function(data)
        {
            // Debug print
            rtos_debug_error_alert(data);
        });
}

// -------------------------------------------------------------------
// Request reading sensor info to RTOS
// -------------------------------------------------------------------
function rtos_get_sensorinfo(func)
{
    var param = {};
    
    // Request to RTOS
    rtos_request_core(
        "cgi-bin/get_sensorinfo.cgi",
        param,
        function(data)
        {
            // Debug print
            rtos_debug_alert(data);

            // Callback
            func(data);
        },
		rtos_debug_error_alert);
}

// -------------------------------------------------------------------
// Request saving binary's to NAND
// -------------------------------------------------------------------
function rtos_do_save(param, func)
{
    // Request to RTOS
    rtos_request_core(
        "cgi-bin/do_save.cgi",
        param,
        function(data)
        {
            // Debug print
            rtos_debug_alert(data);

            // Callback
            func(data);
        },
		rtos_debug_error_alert);
}

// -------------------------------------------------------------------
// Request exporting in SDCard
// -------------------------------------------------------------------
function rtos_do_export(func)
{
    var param = {};
    
    // Request to RTOS
    rtos_request_core(
        "cgi-bin/do_export.cgi",
        param,
        function(data)
        {
            // Debug print
            rtos_debug_alert(data);

            // Callback
            func(data);
        },
		rtos_debug_error_alert);
}

// -------------------------------------------------------------------
// Request importing Setting info
// -------------------------------------------------------------------
function rtos_do_import(func)
{
    var param = {};
    
    // Request to RTOS
    rtos_request_core(
        "cgi-bin/do_import.cgi",
        param,
        function(data)
        {
            // Debug print
            rtos_debug_alert(data);

            // Callback
            func(data);
        },
		rtos_debug_error_alert);
}

// -------------------------------------------------------------------
// Request tuning stop to RTOS
// -------------------------------------------------------------------
function rtos_tuning_stop(func)
{
    var param = {};

    // Request to RTOS
    rtos_request_core(
        "cgi-bin/tuning_stop.cgi",
        param,
        function(data)
        {
            // Debug print
            rtos_debug_alert(data);

            // Callback
            func(data);
        },
		rtos_debug_error_alert);
}


// -------------------------------------------------------------------
// Request uploading parameter to RTOS
// -------------------------------------------------------------------
function rtos_upload_param(param, func)
{
    // Request to RTOS
    rtos_request_core(
        "cgi-bin/upload_parameter.cgi",
        param,
        function(data)
        {
            // Debug print
            rtos_debug_alert(data);

            // Callback
            func(data);
        },
		function(data)
        {
            // Debug print
            rtos_debug_error_alert(data);
        });
}

// -------------------------------------------------------------------
// Request getting AE weight to RTOS
// -------------------------------------------------------------------
function rtos_get_aeweight(param, func)
{
    // Request to RTOS
    rtos_request_core(
        "cgi-bin/get_aeweight.cgi",
        param,
        function(data)
        {
            // Debug print
            rtos_debug_alert(data);

            // Callback
            func(data);
        },
		function(data)
        {
            // Debug print
            rtos_debug_error_alert(data);
        });
}

// -------------------------------------------------------------------
// Request setting AE weight to RTOS
// -------------------------------------------------------------------
function rtos_set_aeweight(param, func)
{
    // Request to RTOS
    rtos_request_core(
        "cgi-bin/set_aeweight.cgi",
        param,
        function(data)
        {
            // Debug print
            rtos_debug_alert(data);

            // Callback
            func(data);
        },
		function(data)
        {
            // Debug print
            rtos_debug_error_alert(data);
        });
}

// -------------------------------------------------------------------
// Request to start Video capture to RTOS
// -------------------------------------------------------------------
function rtos_v_capture_start(param, func)
{
    param.set_type = "Start";

    // Request to RTOS
    rtos_request_core(
        "cgi-bin/video_capture.cgi",
        param,
        function(data)
        {
            // Debug print
            rtos_debug_alert(data);

            // Callback
            func(data);
        },
		function(data)
        {
            // Debug print
            rtos_debug_error_alert(data);
        });
}

// -------------------------------------------------------------------
// Request to stop Video capture to RTOS
// -------------------------------------------------------------------
function rtos_v_capture_stop(param, func)
{
    param.set_type = "Stop";
   
    // Request to RTOS
    rtos_request_core(
        "cgi-bin/video_capture.cgi",
        param,
        function(data)
        {
            // Debug print
            rtos_debug_alert(data);
    
            // Callback
            func(data);
        },
        function(data)
        {
            // Debug print
            rtos_debug_error_alert(data);
        });
}

// -------------------------------------------------------------------
// Request any command to RTOS
// -------------------------------------------------------------------
function rtos_request_core(cgi, param, success_func, error_func)
{
    if (DEBUG_CGI)
    {
        if (param.set_type != undefined)
        {
            window.alert("call:" + cgi + ", [input] ->" + param.set_type + ": " + JSON.stringify(param));
        }
        else
        {
            window.alert("call:" + cgi + ", [input] ->" + JSON.stringify(param));
        }
    }
    
    // Then使うように変えたい。そっちのほうがクールらしい。
    $.ajax({
		url: cgi,
		type: 'POST',
		dataType: 'json',
		contentType: 'application/json',
		data: JSON.stringify(param),
		scriptCharset: 'utf-8'
    }).then(
		success_func,
		error_func
    );
}