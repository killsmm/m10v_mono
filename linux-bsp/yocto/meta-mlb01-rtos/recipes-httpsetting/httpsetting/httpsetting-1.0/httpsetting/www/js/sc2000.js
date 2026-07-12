// JavaScript Document
//$('[data-spy="scroll"]').each(function () {
//  var $spy = $(this).scrollspy('refresh')
//});
var app_data = {};
var app_ui = {};
var pIndex = -1;

var VALUE_GAIN_BOTTOM = 0;					// const value
var VALUE_SV_BOTTOM = 5;					// const value
var LOG10_2 = (Math.log(2) / Math.log(10));	// const value
var FIX_20xLOG10_2 = (20 * LOG10_2);		// const value 6.020599913
var DEBUG_CGI = false;						// const value
var DEBUG_UI = false;						// const value

// -------------------------------------------------------------------
// Check UI IDLE (Relationary Camera Mode)
// -------------------------------------------------------------------
function app_check_disable()
{
	return $('#select_Control_CameraSetting_CameraMode').val() == '1';
}

// -------------------------------------------------------------------
// Check UI Video Preview  (Relationary Camera Mode)
// -------------------------------------------------------------------
function app_check_video_preview()
{
	return $('#select_Control_CameraSetting_CameraMode').val() == '2';
}

// -------------------------------------------------------------------
// Check UI Still Capture  (Relationary Camera Mode)
// -------------------------------------------------------------------
function app_check_still_capture()
{
	return $('#select_Control_CameraSetting_CameraMode').val() == '3';
}

// -------------------------------------------------------------------
// Reload camera mode settings.
// -------------------------------------------------------------------
function app_reload_cameramode()
{
	var param = {};
	var camera_mode;
	var iso;
	var wb_mode;

	// Update UI data
	camera_mode = $('#select_Control_CameraSetting_CameraMode').val();
	iso = $('#select_Control_CameraSetting_ISO').val();
	wb_mode = $('#select_Control_CameraSetting_WBMode').val();

	app_data.Statics.control["camera_mode"] = parseInt(camera_mode);
	app_data.Statics.control["iso"] = parseInt(iso);		
	app_data.Statics.control["wb_mode"] = parseInt(wb_mode);
	
	// Prepare the arguments
	param = app_get_rtos_args();
	param.camera_mode = app_check_disable() ? 1 : 2;

	// Request changing Camera Mode to RTOS
	rtos_change_camera_mode(param, function(data)
	{
		// Hide wait modal
		loading_close();

		// Refresh UI's data
		app_refresh_ui_data(data);
	});
}

// -------------------------------------------------------------------
// Update the Resolution's UI contents
// -------------------------------------------------------------------
function app_updateUI_List_Resolution()
{
	var camera_mode;
	var resolution;
	var uiList;

	// Read Camera Mode
	camera_mode = $('#select_Control_CameraSetting_CameraMode').val();

	// Init the Resolution UI's contents
	uiList = env_get_uiList_resolution(parseInt(camera_mode));
	app_updateUI_List('#select_Control_CameraSetting_Resolution', uiList);
	
	if (app_check_video_preview())
	{
		resolution = app_data.Statics.control["v_resolution"];
	}
	else if (app_check_still_capture())
	{
		resolution = app_data.Statics.control["s_resolution"];
	}
	else
	{
		resolution = 0;
	}

	$('#select_Control_CameraSetting_Resolution').val(resolution);
}

// -------------------------------------------------------------------
// Update UI core
// -------------------------------------------------------------------
function app_updateUI_core(value, ui_val)
{
	$(ui_val).val(value);
}

// -------------------------------------------------------------------
// Update slider UI core
// -------------------------------------------------------------------
function app_updateUI_core_slider(value, ui_val, ui_id)
{
	app_updateUI_core(value, ui_val);
	$(ui_id).slider('value', value);

	$(ui_id).slider("option", "disabled", app_check_disable() ? true : false);
}

// -------------------------------------------------------------------
// Update the Statics's UI
// -------------------------------------------------------------------
function app_updateUI_Statics(data)
{
	var uiInfoList = {
		"iso" : {
			"val" : '#slider_Control_Statics_ISO_val',
			"id" : '#slider_Control_Statics_ISO'
		},
		"a_gain" : {
			"val" : '#slider_Control_Statics_AGain_val',
			"id" : '#slider_Control_Statics_AGain'
		},
		"d_gain" : {
			"val" : '#slider_Control_Statics_DGain_val',
			"id" : '#slider_Control_Statics_DGain'
		},
		"expo" : {
			"val" : '#slider_Control_Statics_Exposure_val',
			"id" : '#slider_Control_Statics_Exposure'
		},
		"c_temp" : {
			"val" : '#slider_Control_Statics_ColorTemp_val',
			"id" : '#slider_Control_Statics_ColorTemp'
		},
		"wb_r" : {
			"val" : '#slider_Control_Statics_RGain_val',
			"id" : '#slider_Control_Statics_RGain'
		},
		"wb_b" : {
			"val" : '#slider_Control_Statics_BGain_val',
			"id" : '#slider_Control_Statics_BGain'
		}
	};

	// Regist value to UI
	for (key in uiInfoList)
	{
		app_updateUI_core_slider(
			parseFloat(data.Statics.statics[key]),
			uiInfoList[key]["val"],
			uiInfoList[key]["id"]);
		$(uiInfoList[key]["id"]).slider("option", "disabled", true);
	}
}

// -------------------------------------------------------------------
// Reflect enable/disable to input ui as idle mode
// -------------------------------------------------------------------
function app_reflect_ui_idle_mode(ui)
{
	ui.prop("disabled", app_check_disable() ? true : false);
}

// -------------------------------------------------------------------
// Update the IQ's UI
// -------------------------------------------------------------------
function app_updateUI_IQ(data)
{
	var param = {
		camera_mode : 0
	};
	var iq_bin_ui = [
		'#select_IQ_Gamma_IQBin',
		'#select_IQ_Sharpness_IQBin',
		'#select_IQ_Denoise_IQBin',
		'#select_IQ_NR3D_IQBin',
		'#select_IQ_Sensor_IQBin',
		'#select_IQ_Color_IQBin'
	]; 
	
	var iso;
	var camera_mode;
	var iq_bin;

	// Read Camera mode
	param.camera_mode = app_data.Statics.control["camera_mode"];
	
	if (app_check_video_preview())
	{
		param.resolution = app_data.Statics.control["v_resolution"];
	}
	else if (app_check_still_capture())
	{
		param.resolution = app_data.Statics.control["s_resolution"];
	}

	// Read IQ.bin index
	iq_bin = rtos_get_iq_bin_index(param);

	// Regist IQ.bin index
	iq_bin_ui.forEach(function(value, index, array)
	{
		app_updateUI_core(iq_bin, value);
	});

	// Update IQ Gamma
	app_reflect_ui_idle_mode($("#radio_IQ_Gamma_Curve input[name='pattern']:radio"));

	$("#radio_IQ_Gamma_Curve input[name='pattern']:radio").filter(
		function()
		{
			return ($(this).val() == data.gamma["index"]);
		}).prop("checked", true);

	// Update IQ Sharpness
	app_reflect_ui_idle_mode($('#select_IQ_Sharpness_ISO'));
	app_reflect_ui_idle_mode($('#slider_IQ_Sharpness_Sharp_val'));
	app_reflect_ui_idle_mode($('#slider_IQ_Sharpness_Edge_val'));

	iso = parseInt($('#select_IQ_Sharpness_ISO').val());
	app_updateUI_core_slider(
		parseInt(data.sharpness["sharpness"][iso]) - 7,
		'#slider_IQ_Sharpness_Sharp_val',
		'#slider_IQ_Sharpness_Sharp');
	app_updateUI_core_slider(
		parseInt(data.sharpness["edge"][iso]) - 7,
		'#slider_IQ_Sharpness_Edge_val',
		'#slider_IQ_Sharpness_Edge');

	// Update IQ Denoise
	app_reflect_ui_idle_mode($('#select_IQ_Denoise_ISO'));
	app_reflect_ui_idle_mode($('#slider_IQ_Denoise_NR_Y_val'));
	app_reflect_ui_idle_mode($('#slider_IQ_Denoise_NR_C_val'));

	iso = parseInt($('#select_IQ_Denoise_ISO').val());
	app_updateUI_core_slider(
		parseInt(data.denoise["nr_y"][iso]) - 7,
		'#slider_IQ_Denoise_NR_Y_val',
		'#slider_IQ_Denoise_NR_Y');
	app_updateUI_core_slider(
		parseInt(data.denoise["nr_c"][iso]) - 7,
		'#slider_IQ_Denoise_NR_C_val',
		'#slider_IQ_Denoise_NR_C');

	// Update IQ NR3D
	app_reflect_ui_idle_mode($('#select_IQ_NR3D_ISO'));
	app_reflect_ui_idle_mode($('#slider_IQ_NR3D_NR_val'));

	iso = parseInt($('#select_IQ_NR3D_ISO').val());
	app_updateUI_core_slider(
		parseInt(data.nr3d["nr"][iso]) - 7,
		'#slider_IQ_NR3D_NR_val',
		'#slider_IQ_NR3D_NR');

	// Update IQ Sensor
	app_reflect_ui_idle_mode($('#select_IQ_Sensor_ISO'));
	app_reflect_ui_idle_mode($('#slider_IQ_Sensor_BlackLevel_R_val'));
	app_reflect_ui_idle_mode($('#slider_IQ_Sensor_BlackLevel_GR_val'));
	app_reflect_ui_idle_mode($('#slider_IQ_Sensor_BlackLevel_GB_val'));
	app_reflect_ui_idle_mode($('#slider_IQ_Sensor_BlackLevel_B_val'));

	iso = parseInt($('#select_IQ_Sensor_ISO').val());
	app_updateUI_core_slider(
		parseInt(data.sensor["r"][iso]) * (-1),
		'#slider_IQ_Sensor_BlackLevel_R_val',
		'#slider_IQ_Sensor_BlackLevel_R');
	app_updateUI_core_slider(
		parseInt(data.sensor["gr"][iso]) * (-1),
		'#slider_IQ_Sensor_BlackLevel_GR_val',
		'#slider_IQ_Sensor_BlackLevel_GR');
	app_updateUI_core_slider(
		parseInt(data.sensor["gb"][iso]) * (-1),
		'#slider_IQ_Sensor_BlackLevel_GB_val',
		'#slider_IQ_Sensor_BlackLevel_GB');
	app_updateUI_core_slider(
		parseInt(data.sensor["b"][iso]) * (-1),
		'#slider_IQ_Sensor_BlackLevel_B_val',
		'#slider_IQ_Sensor_BlackLevel_B');

	// Update IQ Color
	app_reflect_ui_idle_mode($('#slider_IQ_Color_Saturation_val'));

	app_updateUI_core_slider(
		parseInt(data.color["saturation"][0]) - 7,
		'#slider_IQ_Color_Saturation_val',
		'#slider_IQ_Color_Saturation');
}

// -------------------------------------------------------------------
// Update the AE's UI
// -------------------------------------------------------------------
function app_updateUI_AE(data)
{
	var param = {};
	var iq_bin_ui = ['#select_AE_Default_AEBin'];

	var value;
	var array;
	var resolution;
	var iq_bin;

	// Read Camera mode
	camera_mode = $('#select_Control_CameraSetting_CameraMode').val();
	param.camera_mode = parseInt(camera_mode);

	if (app_check_video_preview())
	{
		param.resolution = app_data.Statics.control["v_resolution"];
	}
	else if (app_check_still_capture())
	{
		param.resolution = app_data.Statics.control["s_resolution"];
	}

	// Read IQ.bin index
	iq_bin = rtos_get_ae_bin_index(param);

	// Regist IQ.bin index
	iq_bin_ui.forEach(function(value, index, array)
	{
		app_updateUI_core(iq_bin, value);
	});

	// Update AE Common
	app_reflect_ui_idle_mode($('#number_AE_Common_Fnumber'));
	app_updateUI_core(
		parseFloat(data.common["f_number"]).toFixed(1),
		'#number_AE_Common_Fnumber');
	
	// Update AE EV Program	
	app_reflect_ui_idle_mode($('#select_AE_EVProgram_ProgramNo'));
	app_reflect_ui_idle_mode($('#slider_AE_EVProgram_GainMax_val'));
	app_reflect_ui_idle_mode($('#slider_AE_EVProgram_GainMin_val'));
	app_reflect_ui_idle_mode($('#slider_AE_EVProgram_ExpMax_val'));
	app_reflect_ui_idle_mode($('#slider_AE_EVProgram_ExpMin_val'));

	array = app_data.AE.evprogram_ui[$('#select_AE_EVProgram_ProgramNo').val()];
	app_updateUI_core_slider(
		parseInt(array[0]),
		'#slider_AE_EVProgram_GainMax_val',
		'#slider_AE_EVProgram_GainMax');
	app_updateUI_core_slider(
		parseInt(array[1]),
		'#slider_AE_EVProgram_GainMin_val',
		'#slider_AE_EVProgram_GainMin');
	app_updateUI_core_slider(
		array[2],
		'#slider_AE_EVProgram_ExpMax_val',
		'#slider_AE_EVProgram_ExpMax');
	app_updateUI_core_slider(
		array[3],
		'#slider_AE_EVProgram_ExpMin_val',
		'#slider_AE_EVProgram_ExpMin');
	
	// Update AE Default
	app_reflect_ui_idle_mode($('#select_AE_Default_AEMetering'));
	app_reflect_ui_idle_mode($('#select_AE_Default_AEMode'));
	app_reflect_ui_idle_mode($('#select_AE_Default_AEAdaptive'));
	app_reflect_ui_idle_mode($('#select_AE_Default_EVProgram'));
	app_reflect_ui_idle_mode($('#slider_AE_Default_AESpeed_val'));
	app_reflect_ui_idle_mode($('#select_AE_Default_EVProgram'));
	app_reflect_ui_idle_mode($('#select_AE_Default_FlickerComp'));
	app_reflect_ui_idle_mode($('#select_AE_Default_EVComp'));
	app_reflect_ui_idle_mode($('#select_AE_Default_SensorGain'));
//	app_reflect_ui_idle_mode($('#slider_AE_Default_SensorExposure_val'));
	app_reflect_ui_idle_mode($('#select_AE_Default_SensorExposure'));
	
	app_updateUI_core(
		parseInt(data.defaultset["ae_metering"]),
		'#select_AE_Default_AEMetering');
	app_updateUI_core(
		parseInt(data.defaultset["ae_mode"]),
		'#select_AE_Default_AEMode');
		
	app_updateUI_core_slider(
		parseInt(data.defaultset["ae_speed"]),
		'#slider_AE_Default_AESpeed_val',
		'#slider_AE_Default_AESpeed');
	app_updateUI_core(
		parseInt(data.defaultset["ae_adaptive"]),
		'#select_AE_Default_AEAdaptive');

	app_updateUI_core(
		parseInt(data.defaultset["evprogram"]),
		'#select_AE_Default_EVProgram');
	app_updateUI_core(
		parseInt(data.defaultset["flicker_comp"]),
		'#select_AE_Default_FlickerComp');
	app_updateUI_core(
		parseInt(data.defaultset["ev_comp"]),
		'#select_AE_Default_EVComp');

	app_updateUI_core(
		parseInt(data.defaultset["sensor_gain"]),
		'#select_AE_Default_SensorGain');

	value = parseInt(1.0 / data.defaultset["sensor_exposure"] * 1000);

	if (value >= 32000000)
	{
		value = 32000000;
	}
	else if (value >= 16000000)
	{
		value = 16000000;		
	}
	else if (value >= 8000000)
	{
		value = 8000000;		
	}
	else if (value >= 4000000)
	{
		value = 4000000;		
	}
	else if (value >= 2000000)
	{
		value = 2000000;		
	}
	else if (value >= 1000000)
	{
		value = 1000000;		
	}
	else if (value >= 500000)
	{
		value = 500000;		
	}
	else if (value >= 250000)
	{
		value = 250000;		
	}
	else if (value >= 125000)
	{
		value = 125000;		
	}
	else if (value >= 60000)
	{
		value = 60000;		
	}
	else if (value >= 30000)
	{
		value = 30000;		
	}
	else if (value >= 15000)
	{
		value = 15000;		
	}
	else if (value >= 8000)
	{
		value = 8000;		
	}
	else if (value >= 4000)
	{
		value = 4000;		
	}
	else if (value >= 2000)
	{
		value = 2000;		
	}
	else if (value >= 1000)
	{
		value = 1000;		
	}
	else if (value >= 500)
	{
		value = 500;		
	}
	else if (value >= 250)
	{
		value = 250;		
	}
	else
	{
		value = 125;				
	}

	app_updateUI_core(
		value,
		'#select_AE_Default_SensorExposure');

	data.defaultset["sensor_exposure"] = parseFloat(1.0 / value * 1000);

//		app_updateUI_core_slider(
//		(parseFloat(data.defaultset["sensor_exposure"]) * 1000).toFixed(0),
//		'#slider_AE_Default_SensorExposure_val',
//		'#slider_AE_Default_SensorExposure');
}

// -------------------------------------------------------------------
// Get html for Preferd cast contents
// -------------------------------------------------------------------
function app_create_preferdcast_ui_html(index, data)
{
	return '<div class="form-contrl">\
				<div class="btn-group" role="group" aria-label="...">\
					<button role="button" class="btn btn-default" id="button_AWB_PreferedCast_List_'+ index +'_Select">' +
					data[index][0] + 'K (' +
					data[index][1] + ',' +
					data[index][2] + ')-(' +
					data[index][3] + ',' +
					data[index][4] + ')' +
					'</button>\
					<div class="btn-group" role="group">\
						<button role="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">\
							Action\
							<span class="caret"></span>\
						</button>\
						<ul class="dropdown-menu">\
							<li><a href="#" id="button_AWB_PreferedCast_List_' + index + '_BringUp">Bring to up</a></li>\
							<li><a href="#" id="button_AWB_PreferedCast_List_' + index + '_BringDown">Bring to down</a></li>\
							<li><a href="#" id="button_AWB_PreferedCast_List_' + index + '_Delete">Delete</a></li>\
						</ul>\
					</div>\
				</div>\
            </div>'
}

// -------------------------------------------------------------------
// Update the UI List
// -------------------------------------------------------------------
function app_updateUI_List(ui, uiList)
{
	// Init the UI's contents
	$(ui).empty();

	// Regist contents to the UI
	uiList.forEach(function(val, index, array)
	{
		$(ui).append(val);
	});
}

// -------------------------------------------------------------------
// Update the IQ's UI contents
// -------------------------------------------------------------------
function app_updateUI_IQ_List_Update()
{
	var binList = [
		"#select_IQ_Gamma_IQBin",
		"#select_IQ_Sharpness_IQBin",
		"#select_IQ_Denoise_IQBin",
		"#select_IQ_NR3D_IQBin",
		"#select_IQ_Sensor_IQBin",
		"#select_IQ_Color_IQBin"
	];
	var isoList = [
		"#select_IQ_Sharpness_ISO",
		"#select_IQ_Denoise_ISO",
		"#select_IQ_NR3D_ISO",
		"#select_IQ_Sensor_ISO"
	];
	var uiList;

	// IQ.bin List Update
	uiList = env_get_uiList_iqbin();

	binList.forEach(function(val, index, array)
	{
		app_updateUI_List(val, uiList);
	});

	// ISO List Update
	uiList = env_get_uiList_iso(false);

	isoList.forEach(function(val, index, array)
	{
		app_updateUI_List(val, uiList);
	});
}

// -------------------------------------------------------------------
// Update the AE's UI contents
// -------------------------------------------------------------------
function app_updateUI_AE_List_Update()
{
	var isoList = [
		"#select_IQ_Sharpness_ISO",
		"#select_IQ_Denoise_ISO",
		"#select_IQ_NR3D_ISO",
		"#select_IQ_Sensor_ISO"
	];
	var uiList;

	// AE.bin List Update
	uiList = env_get_uiList_aebin();
	app_updateUI_List("#select_AE_Default_AEBin", uiList);

	// AE Metering List Update
	uiList = env_get_uiList_aemetering();
	app_updateUI_List("#select_AE_Default_AEMetering", uiList);

	// ISO List Update
	uiList = env_get_uiList_iso(false);
	app_updateUI_List("#select_AE_Default_SensorGain", uiList);	
}

// -------------------------------------------------------------------
// Update the AWB's UI contents
// -------------------------------------------------------------------
function app_updateUI_AWB_List_Update()
{
	var length = app_data.AWB.preferedcast.length;

	$('#form_AWB_PreferedCast_List').empty();
	$('#form_AWB_PreferedCast_List').append('<label class="control-label">Select Item and Operation</label>');
	
	if (app_check_disable())
	{
		return;
	}

	for (var i = 0 ; i < length; i++)
	{ 	
		$('#form_AWB_PreferedCast_List').append(
			app_create_preferdcast_ui_html(
				i,
				app_data.AWB.preferedcast));

		$('#button_AWB_PreferedCast_List_' + i + '_Select').click(function()
		{
			var index=$(this).attr("id").replace('button_AWB_PreferedCast_List_', '');
			index=parseInt(index.replace('_Select', ''));
			
			app_ui.AWB.preferedcast["select"] = index;
			app_updateUI_AWB(app_data.AWB);
		});
		$('#button_AWB_PreferedCast_List_' + i + '_BringUp').click(function()
		{
			var index=$(this).attr("id").replace('button_AWB_PreferedCast_List_', '');
			index=parseInt(index.replace('_Select', ''));

			if (index - 1 >= 0)
			{
				var data = app_data.AWB.preferedcast[index];
				app_data.AWB.preferedcast[index] = app_data.AWB.preferedcast[index - 1];
				app_data.AWB.preferedcast[index - 1] = data;

				if (app_ui.AWB.preferedcast.selection == index)
				{
					app_ui.AWB.preferedcast.selection -= 1;
				}
				
				app_updateUI_AWB_List_Update();
				app_updateUI_AWB(app_data.AWB);
			}
		});
		$('#button_AWB_PreferedCast_List_' + i + '_BringDown').click(function()
		{
			var index=$(this).attr("id").replace('button_AWB_PreferedCast_List_', '');
			index=parseInt(index.replace('_Select', ''));

			if (index + 1 < app_data.AWB.preferedcast.length)
			{
				var data = app_data.AWB.preferedcast[index];
				app_data.AWB.preferedcast[index] = app_data.AWB.preferedcast[index + 1];
				app_data.AWB.preferedcast[index + 1] = data;
				
				if (app_ui.AWB.preferedcast.selection == index)
				{
					app_ui.AWB.preferedcast.selection += 1;
				}
				
				app_updateUI_AWB_List_Update();
				app_updateUI_AWB(app_data.AWB);
			}
		});
		$('#button_AWB_PreferedCast_List_' + i + '_Delete').click(function()
		{
			var index=$(this).attr("id").replace('button_AWB_PreferedCast_List_', '');
			index=parseInt(index.replace('_Select', ''));
			
			app_data.AWB.preferedcast.splice(index, 1);

			app_ui.AWB.preferedcast.selection = index < app_data.AWB.preferedcast.length
											  ? (index > 0 ? index - 1 : 0)
											  : (app_data.AWB.preferedcast.length - 1);

			app_ui.AWB.preferedcast["select"] = app_ui.AWB.preferedcast.selection;
			app_updateUI_AWB_List_Update();		
			app_updateUI_AWB(app_data.AWB);
		});
	}
}

// -------------------------------------------------------------------
// Redraw AWB's UI
// -------------------------------------------------------------------
function app_updateUI_AWB_List_Redraw(data)
{
	var length = app_data.AWB.preferedcast.length;

	for (var i = 0; i < length; i++)
	{ 	
		$('#button_AWB_PreferedCast_List_' + i + '_Select').html(
							app_data.AWB.preferedcast[i][0] + 'K (' +
							app_data.AWB.preferedcast[i][1] + ',' +
							app_data.AWB.preferedcast[i][2] + ')-(' +
							app_data.AWB.preferedcast[i][3] + ',' +
							app_data.AWB.preferedcast[i][4] + ')');
	}
}

// -------------------------------------------------------------------
// Update AWB's UI
// -------------------------------------------------------------------
function app_updateUI_AWB(data)
{
	var manualwb_selection;
	var selection;

	// Update AWB Basic
	app_reflect_ui_idle_mode($('#slider_AWB_Basic_AWBSpeed_val'));

	app_updateUI_core_slider(	
		(parseFloat(data.basic["awb_speed"]) * 100.0).toFixed(0),
		'#slider_AWB_Basic_AWBSpeed_val',
		'#slider_AWB_Basic_AWBSpeed');
	
	// Update AWB PreferdCast
	app_reflect_ui_idle_mode($('#button_AWB_PreferedCast_AddTop'));
	app_reflect_ui_idle_mode($('#button_AWB_PreferedCast_AddBottom'));
	app_reflect_ui_idle_mode($('#button_AWB_PreferedCast_ResetList'));
	app_reflect_ui_idle_mode($('#slider_AWB_PreferedCast_ColorTemperature_val'));
	app_reflect_ui_idle_mode($('#slider_AWB_PreferedCast_LowRGain_val'));
	app_reflect_ui_idle_mode($('#slider_AWB_PreferedCast_LowBGain_val'));
	app_reflect_ui_idle_mode($('#slider_AWB_PreferedCast_HighRGain_val'));
	app_reflect_ui_idle_mode($('#slider_AWB_PreferedCast_HighBGain_val'));

	selection = app_ui.AWB.preferedcast["select"];

	if (selection < data.preferedcast.length
	 && selection >= 0)
	{
		app_updateUI_core_slider(
			data.preferedcast[selection][0],
			'#slider_AWB_PreferedCast_ColorTemperature_val',
			'#slider_AWB_PreferedCast_ColorTemperature');
		app_updateUI_core_slider(
			data.preferedcast[selection][1],
			'#slider_AWB_PreferedCast_LowRGain_val',
			'#slider_AWB_PreferedCast_LowRGain');
		app_updateUI_core_slider(
			data.preferedcast[selection][2],
			'#slider_AWB_PreferedCast_LowBGain_val',
			'#slider_AWB_PreferedCast_LowBGain');
		app_updateUI_core_slider(
			data.preferedcast[selection][3],
			'#slider_AWB_PreferedCast_HighRGain_val',
			'#slider_AWB_PreferedCast_HighRGain');
		app_updateUI_core_slider(
			data.preferedcast[selection][4],
			'#slider_AWB_PreferedCast_HighBGain_val',
			'#slider_AWB_PreferedCast_HighBGain');
	}

	// Update AWB ManualWB	
	app_reflect_ui_idle_mode($('#select_AWB_ManualWB_Selection'));
	app_reflect_ui_idle_mode($('#slider_AWB_ManualWB_X_val'));
	app_reflect_ui_idle_mode($('#slider_AWB_ManualWB_Y_val'));

	manualwb_selection = parseInt($('#select_AWB_ManualWB_Selection').val());
	app_updateUI_core_slider(
		parseFloat(data.manualwb[manualwb_selection][0]),
		'#slider_AWB_ManualWB_X_val',
		'#slider_AWB_ManualWB_X');
	app_updateUI_core_slider(	
		parseFloat(data.manualwb[manualwb_selection][1]),
		'#slider_AWB_ManualWB_Y_val',
		'#slider_AWB_ManualWB_Y');
}

// -------------------------------------------------------------------
// Regist AE EV Program's UI value
// -------------------------------------------------------------------
function app_changeUI_EVProgram()
{
	var name;

	name = $('#select_AE_EVProgram_ProgramNo').val()
	app_data.AE.evprogram_ui[name] = [
		$('#slider_AE_EVProgram_GainMax').slider('option','value'),
		$('#slider_AE_EVProgram_GainMin').slider('option','value'),
		$('#slider_AE_EVProgram_ExpMax').slider('option','value'),
		$('#slider_AE_EVProgram_ExpMin').slider('option','value')
	];
}

// -------------------------------------------------------------------
// Convert signed 32bit int of fw
// -------------------------------------------------------------------
function app_to_fw_32bit(value)
{
	return (value * 65536) & 0xFFFFFFFF;
}

// -------------------------------------------------------------------
// Convert signed 32bit int of ui
// -------------------------------------------------------------------
function app_to_ui_32bit(value)
{
	return (value & 0xFFFFFFFF) / 65536.0 ;
}

// -------------------------------------------------------------------
// Get all write data of AE EV Program's UI value
// -------------------------------------------------------------------
function app_writeData_EVProgram_All(f_number)
{
// Editable EVP is only "User Define 1~3" (prog13~15)
// But other EVP is also updated by editing F number
	app_writeData_EVProgram("prog0", f_number);
	app_writeData_EVProgram("prog1", f_number);
	app_writeData_EVProgram("prog2", f_number);
	app_writeData_EVProgram("prog3", f_number);
	app_writeData_EVProgram("prog4", f_number);
	app_writeData_EVProgram("prog5", f_number);
	app_writeData_EVProgram("prog6", f_number);
	app_writeData_EVProgram("prog7", f_number);
	app_writeData_EVProgram("prog8", f_number);
	app_writeData_EVProgram("prog9", f_number);
	app_writeData_EVProgram("prog10", f_number);
	app_writeData_EVProgram("prog11", f_number);
	app_writeData_EVProgram("prog12", f_number);
	app_writeData_EVProgram("prog13", f_number);
	app_writeData_EVProgram("prog14", f_number);
	app_writeData_EVProgram("prog15", f_number);
}

// -------------------------------------------------------------------
// Get write data of AE EV Program's UI value
// -------------------------------------------------------------------
function app_writeData_EVProgram(evprogram_name, f_number)
{
	var param = {
		f_number : 0,
		gain : { min : 0, max : 0 },
		sv : { min : 0, max : 0 },
		av : { min : 0, val : 0, max : 0 },
		exp : { min : 0, max : 0 },
		tv : { min : 0, max : 0 }
	};

	param.f_number = f_number;
	param.av.val = 2 * Math.log(param.f_number) / Math.log(2);

	param.gain.max = app_data.AE.evprogram_ui[evprogram_name][0];
	param.sv.max = (param.gain.max - VALUE_GAIN_BOTTOM) / FIX_20xLOG10_2 + VALUE_SV_BOTTOM;
	param.av.max = param.av.val - (param.sv.max - VALUE_SV_BOTTOM);
		
	param.gain.min = app_data.AE.evprogram_ui[evprogram_name][1];
	param.sv.min = (param.gain.min - VALUE_GAIN_BOTTOM) / FIX_20xLOG10_2 + VALUE_SV_BOTTOM;
	param.av.min = param.av.val - (param.sv.min - VALUE_SV_BOTTOM);
		
	param.exp.max = app_data.AE.evprogram_ui[evprogram_name][2];
	param.tv.max = -Math.log(param.exp.max / 1000) / Math.log(2);
		
	param.exp.min =app_data.AE.evprogram_ui[evprogram_name][3];
	param.tv.min = -Math.log(param.exp.min / 1000) / Math.log(2);

	app_data.AE.evprogram[evprogram_name] = [
		// Bank 0
		app_to_fw_32bit(param.tv.max + param.av.max),
		app_to_fw_32bit(param.tv.max),
		app_to_fw_32bit(0),
		0,
		// Bank 1
		app_to_fw_32bit(param.tv.max + param.av.max),
		app_to_fw_32bit(param.tv.max),
		app_to_fw_32bit(0),
		1,
		// Bank 0
		app_to_fw_32bit(param.tv.max + param.av.min),
		app_to_fw_32bit(param.tv.max),
		app_to_fw_32bit(1),
		0,
		// Bank 1
		app_to_fw_32bit(param.tv.max + param.av.min),
		app_to_fw_32bit(param.tv.max),
		app_to_fw_32bit(1),
		1,
		// Bank 0
		app_to_fw_32bit(param.tv.min + param.av.min),
		app_to_fw_32bit(param.tv.min),
		app_to_fw_32bit(0),
		0,
		// Bank 1
		app_to_fw_32bit(param.tv.min + param.av.min),
		app_to_fw_32bit(param.tv.min),
		app_to_fw_32bit(0),
		1
	];	
}

// -------------------------------------------------------------------
// Get all read data of AE EV Program's UI value
// -------------------------------------------------------------------
function app_readData_EVProgram_All()
{
	app_readData_EVProgram("prog0");
	app_readData_EVProgram("prog1");
	app_readData_EVProgram("prog2");
	app_readData_EVProgram("prog3");
	app_readData_EVProgram("prog4");
	app_readData_EVProgram("prog5");
	app_readData_EVProgram("prog6");
	app_readData_EVProgram("prog7");
	app_readData_EVProgram("prog8");
	app_readData_EVProgram("prog9");
	app_readData_EVProgram("prog10");
	app_readData_EVProgram("prog11");
	app_readData_EVProgram("prog12");		
	app_readData_EVProgram("prog13");
	app_readData_EVProgram("prog14");
	app_readData_EVProgram("prog15");
}

// -------------------------------------------------------------------
// Get read data of AE EV Program's UI value
// -------------------------------------------------------------------
function app_readData_EVProgram(evprogram_name)
{
	var param = {
		f_number : 0,
		gain : { min : 0, max : 0 },
		sv : { min : 0, max : 0 },
		av : { min : 0, val : 0, max : 0 },
		exp : { min : 0, max : 0 },
		tv : { min : 0, max : 0 }
	};

	var array = app_data.AE.evprogram[evprogram_name];

	param.f_number = parseFloat(app_data.AE.common["f_number"]).toFixed(1);
	param.av.val = 2 * Math.log(param.f_number) / Math.log(2);

	var valueEV_0 = app_to_ui_32bit(array[4 * 0 + 0]); 
	var valueTV_0 = app_to_ui_32bit(array[4 * 0 + 1]);
	var valueAV_0 = valueEV_0 - valueTV_0;
	param.sv.max = (param.av.val - valueAV_0) + VALUE_SV_BOTTOM;
	param.tv.max = valueTV_0;
	
	var valueEV_1 = app_to_ui_32bit(array[4 * 2 + 0]);
	var valueTV_1 = app_to_ui_32bit(array[4 * 2 + 1]);
	var valueAV_1 = valueEV_1 - valueTV_1;
	param.sv.max = (param.av.val - valueAV_0) + VALUE_SV_BOTTOM;
	param.sv.min = (param.av.val - valueAV_1) + VALUE_SV_BOTTOM;
	
	var valueEV_2 = app_to_ui_32bit(array[4 * 4 + 0]);
	var valueTV_2 = app_to_ui_32bit(array[4 * 4 + 1]);
	var valueAV_2 = valueEV_2 - valueTV_2;
	param.tv.min = valueTV_2;
	
	param.gain.max = (param.sv.max - VALUE_SV_BOTTOM) *  FIX_20xLOG10_2 + VALUE_GAIN_BOTTOM;
	param.gain.min = (param.sv.min - VALUE_SV_BOTTOM) *  FIX_20xLOG10_2 + VALUE_GAIN_BOTTOM;

	param.exp.max = Math.pow(2, -param.tv.max) * 1000;
	param.exp.min = Math.pow(2, -param.tv.min) * 1000;

	param.gain.max = (param.gain.max).toFixed(0);
	param.gain.min = (param.gain.min).toFixed(0);

	param.exp.max = (param.exp.max).toFixed(4);
	param.exp.min = (param.exp.min).toFixed(4);
	
	app_data.AE.evprogram_ui[evprogram_name] = [
		param.gain.max,
		param.gain.min,
		param.exp.max,
		param.exp.min
	];
}

// -------------------------------------------------------------------
// Get the arguments for RTOS API
// -------------------------------------------------------------------
function app_get_rtos_args()
{
	var param = {};
	var camera_mode = app_data.Statics.control["camera_mode"] 
	var resolution = app_data.Statics.control["v_resolution"];
	var iso = app_data.Statics.control["iso"];
	var wb_mode = app_data.Statics.control["wb_mode"];

	// Prepare the arguments
	param = rtos_args(camera_mode, resolution, iso, -1, wb_mode);

	return param;
}

// -------------------------------------------------------------------
// Update IQ.bin's parameter by UI's one
// -------------------------------------------------------------------
function app_update_parameter(uiinfo)
{
	var param = {};

	// Check Camera Mode
	if (app_check_disable())
	{
		window.alert("Please Change camera mode!");
		return;
	}

	// Prepare the arguments
	param = app_get_rtos_args();
	param.set_data = uiinfo.set_data;
	param.set_type = uiinfo.set_type;

	// Show wait modal
	loading_show();

	// Request uploading parameter to RTOS
	rtos_upload_param(param, function(data)
	{
		if (uiinfo.set_type == "AWB_manualwb" || uiinfo.set_type == "AWB_preferedcast")
		{
			// Reload WB Mode
			app_update_wb_mode();
		}
		else
		{
			// Hide wait modal
			loading_close();
		}
	});
}

// -------------------------------------------------------------------
// Update WB Mode
// -------------------------------------------------------------------
function app_update_wb_mode()
{
	var param = {};
	var wb_mode;

	// Update UI data
	wb_mode = $('#select_Control_CameraSetting_WBMode').val();
	app_data.Statics.control["wb_mode"] = parseInt(wb_mode);
	
	// Prepare the arguments
	param = app_get_rtos_args();
	param.camera_mode = app_check_disable() ? 1 : 2;

	// Request changing ISO to RTOS
	rtos_change_wbmode(param, function(data)
	{
		// Hide wait modal
		loading_close();
	});
}

// -------------------------------------------------------------------
// Refresh UI's data
// -------------------------------------------------------------------
function app_refresh_ui_data(data)
{
	var v_resolution;
	var s_resolution;
	
	if (!app_check_disable())
	{
		app_data.IQ = data.IQ;
		app_data.AE = data.AE;
		app_data.AWB = data.AWB;
		app_data.AE.evprogram_ui = {};
		// data.AE.common["f_number"] is actually AV value. so it need to be converted to F number.
		app_data.AE.common["f_number"] = Math.sqrt(Math.pow(2, data.AE.common["f_number"])) + 0.005;
		app_data.AE.common["f_number"] = app_data.AE.common["f_number"].toFixed(1);
		app_readData_EVProgram_All();
		app_data.IQ.gamma = gamma_convert_rtos_result(data.IQ.gamma);
	}
	else
	{
		v_resolution = app_data.Statics.control["v_resolution"];
		s_resolution = app_data.Statics.control["s_resolution"];
		app_data = app_get_init_data();
		app_data.Statics.control["v_resolution"] = v_resolution;
		app_data.Statics.control["s_resolution"] = s_resolution;
		app_data.AE.evprogram_ui = {};
		app_readData_EVProgram_All();
	}
	app_updateUI_IQ(app_data.IQ);
	app_updateUI_AE(app_data.AE);
	app_updateUI_AWB_List_Update();
	app_updateUI_AWB(app_data.AWB);
}

// -------------------------------------------------------------------
// Get initialize app data 
// -------------------------------------------------------------------
function app_get_init_data()
{
	return {
		Statics : { 
			control : {
				"camera_mode" : 0,
				"v_resolution": env_resolution_init(2),
				"s_resolution": env_resolution_init(3),
				"iso": 0,
				"wb_mode": 0
			},
			statics : {
				"iso": 0,
				"a_gain": 0,
				"d_gain": 0,
				"expo": 0,
				"c_temp": 0,
				"wb_r": 0,
				"wb_b": 0
			}
		},
		IQ : { 
			gamma : {
				"index": 4,
				"lut": [
				gamma_init_data(),
				gamma_init_data(1),
				gamma_init_data(2),
				gamma_init_data(3),
				gamma_init_data(4)]
			},
			sharpness : {
				"sharpness": [7,7,7,7,7,7,7],
				"edge": [7,7,7,7,7,7,7]
			},
			denoise : {
				"nr_y": [7,7,7,7,7,7,7],
				"nr_c": [7,7,7,7,7,7,7]
			},
			nr3d : {
				"nr": [7,7,7,7,7,7,7]
			},
			sensor : env_get_ob_init(),
			color : {
				"saturation" : [7,7,7,7,7,7,7]
			}
		},
		AE : {
			common : {
				"f_number": 2.0
			},
			evprogram : {
				"prog0" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
				"prog1" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
				"prog2" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
				"prog3" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
				"prog4" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
				"prog5" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
				"prog6" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
				"prog7" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
				"prog8" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
				"prog9" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
				"prog10" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
				"prog11" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
				"prog12" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
				"prog13" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
				"prog14" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
				"prog15" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
			},
			evprogram_ui : {
				"prog0" : [0, 0, 1, 1],
				"prog1" : [0, 0, 1, 1],
				"prog2" : [0, 0, 1, 1],
				"prog3" : [0, 0, 1, 1],
				"prog4" : [0, 0, 1, 1],
				"prog5" : [0, 0, 1, 1],
				"prog6" : [0, 0, 1, 1],
				"prog7" : [0, 0, 1, 1],
				"prog8" : [0, 0, 1, 1],
				"prog9" : [0, 0, 1, 1],
				"prog10" : [0, 0, 1, 1],
				"prog11" : [0, 0, 1, 1],
				"prog12" : [0, 0, 1, 1],
				"prog13" : [0, 0, 1, 1],
				"prog14" : [0, 0, 1, 1],
				"prog15" : [0, 0, 1, 1]
			},
			defaultset : {
				"ae_metering": 1,
				"ae_mode": 1,
				"ae_speed": 30,
				"ae_adaptive": 1,
				"evprogram": 1,
				"flicker_comp": 1,
				"ev_comp": 9,
				"sensor_gain": 1,
				"sensor_exposure": 0.033
			}
		},
		AWB : {
			basic : {
				"awb_speed": 0.5
			},
			preferedcast : [
				[3000,1.0,1.1,1.2,1.3],
				[4000,1.1,1.2,1.3,1.4],
				[5000,1.2,1.3,1.4,1.5]
			],
			manualwb : [
				[0.328,0.341],	// 1
				[0.315,0.327],	// 2
				[0.375,0.400],	// 3
				[0.349,0.382],	// 4
				[0.411,0.412],	// 5
				[0.461,0.414],	// 6
				[0.297,0.306],	// 7
				[0.000,0.000],	// 8
				[0.000,0.000],	// 9
				[0.000,0.000],	// 10
				[0.000,0.000]	// 11
			]
		}
	};
}

// -------------------------------------------------------------------
// Init Slider UI
// -------------------------------------------------------------------
function app_initUI_slider(value, min, max, step, ui_val, ui_id)
{
	$(ui_id).slider({
		value : value,
		min : min,
		max : max,
		step : step,
		change: function(event, ui)
		{
			$(ui_val).val(ui.value);
		},
	  	create: function(event, ui)
		  {
			$(ui_val).val($(this).slider('option', 'value'));
		}
	});
}

// -------------------------------------------------------------------
// Init Slider UI (with callback)
// -------------------------------------------------------------------
function app_initUI_slider2(value, min, max, step, ui_val, ui_id, func)
{
	$(ui_id).slider({
		value : value,
		min : min,
		max : max,
		step : step,
		change: function (event, ui)
		{
			// Call delegate function
			func(ui_val, ui);
		},
	  	create: function (event, ui)
		{
			$(ui_val).val($(this).slider('option', 'value'));
		}
	});	
	$(ui_val).on('change', function(event, ui){
		$(ui_id).slider('value', $(this).val());
	});
}

// -------------------------------------------------------------------
// IQ's UI changing handler
// -------------------------------------------------------------------
function app_iq_handler(ui_val, ui)
{
	var iso;
	
	// Set value to UI
	$(ui_val).val(ui.value);
	
	// Update UI's data
	switch (ui_val)
	{
		case '#slider_IQ_Sharpness_Sharp_val':
			iso = parseInt($('#select_IQ_Sharpness_ISO').val());
			app_data.IQ.sharpness["sharpness"][iso] = ui.value + 7;
			break;
		case '#slider_IQ_Sharpness_Edge_val':
			iso = parseInt($('#select_IQ_Sharpness_ISO').val());
			app_data.IQ.sharpness["edge"][iso] = ui.value + 7;
			break;
		case '#slider_IQ_Denoise_NR_Y_val':
			iso = parseInt($('#select_IQ_Denoise_ISO').val());
			app_data.IQ.denoise["nr_y"][iso] = ui.value + 7;
			break;
		case '#slider_IQ_Denoise_NR_C_val':
			iso = parseInt($('#select_IQ_Denoise_ISO').val());
			app_data.IQ.denoise["nr_c"][iso] = ui.value + 7;
			break;
		case '#slider_IQ_NR3D_NR_val':
			iso = parseInt($('#select_IQ_NR3D_ISO').val());
			app_data.IQ.nr3d["nr"][iso] = ui.value + 7;
			break;
		case '#slider_IQ_Sensor_BlackLevel_R_val':
			iso = parseInt($('#select_IQ_Sensor_ISO').val());
			app_data.IQ.sensor["r"][iso] = ui.value * (-1);
			break;
		case '#slider_IQ_Sensor_BlackLevel_GR_val':
			iso = parseInt($('#select_IQ_Sensor_ISO').val());
			app_data.IQ.sensor["gr"][iso] = ui.value * (-1);
			break;
		case '#slider_IQ_Sensor_BlackLevel_GB_val':
			iso = parseInt($('#select_IQ_Sensor_ISO').val());
			app_data.IQ.sensor["gb"][iso] = ui.value * (-1);
			break;
		case '#slider_IQ_Sensor_BlackLevel_B_val':
			iso = parseInt($('#select_IQ_Sensor_ISO').val());
			app_data.IQ.sensor["b"][iso] = ui.value * (-1);
			break;
		case '#slider_IQ_Color_Saturation_val':
			for ( i = 0; i < 7; i++ ) {
				app_data.IQ.color["saturation"][i] = ui.value + 7;
			}
			break;
		default:
			// Empty
			break;
	}
}

// -------------------------------------------------------------------
// AE's UI changing handler
// -------------------------------------------------------------------
function app_ae_handler(ui_val, ui)
{
	// Set value to UI
	$(ui_val).val(ui.value);
}

// -------------------------------------------------------------------
// AE's UI changing handler for EV Program
// -------------------------------------------------------------------
function app_aeev_handler(ui_val, ui)
{
	// Set value to UI
	app_ae_handler(ui_val, ui);
	
	// Update UI's data
	app_changeUI_EVProgram();
}

// -------------------------------------------------------------------
// AWB's UI changing handler
// -------------------------------------------------------------------
function app_awb_handler(ui_val, ui)
{
	var selection = app_ui.AWB.preferedcast["select"];
	var manualwb_index = parseInt($('#select_AWB_ManualWB_Selection').val());

	// Set value to UI
	$(ui_val).val(ui.value);
	
	// Update UI's data
	switch (ui_val)
	{
		case '#slider_AWB_Basic_AWBSpeed_val':
			// Adjust to AWB_SPEED(AWB.in) range: 0.0 ｡�1.0
			app_data.AWB.basic["awb_speed"] = ui.value / 100;
			break;
		case '#slider_AWB_PreferedCast_ColorTemperature_val':
			if (selection < app_data.AWB.preferedcast.length
	 		 && selection >= 0)
			{
				app_data.AWB.preferedcast[selection][0] = ui.value;
				app_updateUI_AWB_List_Redraw(app_data.AWB);
			}
			break;
		case '#slider_AWB_PreferedCast_LowRGain_val':
			if (selection < app_data.AWB.preferedcast.length
	 		 && selection >= 0)
			{
				app_data.AWB.preferedcast[selection][1] = ui.value;
				app_updateUI_AWB_List_Redraw(app_data.AWB);
			}
			break;
		case '#slider_AWB_PreferedCast_LowBGain_val':
			if (selection < app_data.AWB.preferedcast.length
	 		 && selection >= 0)
			{
				app_data.AWB.preferedcast[selection][2] = ui.value;
				app_updateUI_AWB_List_Redraw(app_data.AWB);
			}
			break;
		case '#slider_AWB_PreferedCast_HighRGain_val':
			if (selection < app_data.AWB.preferedcast.length
	 		 && selection >= 0)
			{
				app_data.AWB.preferedcast[selection][3] = ui.value;
				app_updateUI_AWB_List_Redraw(app_data.AWB);
			}
			break;
		case '#slider_AWB_PreferedCast_HighBGain_val':
			if (selection < app_data.AWB.preferedcast.length
	 		 && selection >= 0)
			{
				app_data.AWB.preferedcast[selection][4] = ui.value;
				app_updateUI_AWB_List_Redraw(app_data.AWB);
			}
			break;
		case '#slider_AWB_ManualWB_X_val':
			app_data.AWB.manualwb[manualwb_index][0] = ui.value;
			break;
		case '#slider_AWB_ManualWB_Y_val':
			app_data.AWB.manualwb[manualwb_index][1] = ui.value;
			break;
		default:
			// Empty
			break;
	}
}

// -------------------------------------------------------------------
// Main
// -------------------------------------------------------------------
$(function()
{
	// Check Env Data
	env_check();

	// Init UI contents --------------------------------------------------
	// Init Control
	app_initUI_slider(50, 50, 6400, 50, '#slider_Control_Statics_ISO_val', '#slider_Control_Statics_ISO');
	app_initUI_slider(0.0, 0, 60.0, 0.01,'#slider_Control_Statics_AGain_val', '#slider_Control_Statics_AGain');
	app_initUI_slider(0.0, 0, 40.0, 0.01, '#slider_Control_Statics_DGain_val', '#slider_Control_Statics_DGain');
	app_initUI_slider(1.0, 1, 500.0, 0.01, '#slider_Control_Statics_Exposure_val', '#slider_Control_Statics_Exposure');
	app_initUI_slider(1000, 1000, 15000, 100, '#slider_Control_Statics_ColorTemp_val', '#slider_Control_Statics_ColorTemp');
	app_initUI_slider(1.0, 1, 4.0, 0.01, '#slider_Control_Statics_RGain_val', '#slider_Control_Statics_RGain');
	app_initUI_slider(1.0, 1, 4.0, 0.01, '#slider_Control_Statics_BGain_val', '#slider_Control_Statics_BGain');

	// Disbled Statics UI
	$('#slider_Control_Statics_ISO').slider("option", "disabled", true);
	$('#slider_Control_Statics_AGain').slider("option", "disabled", true);
	$('#slider_Control_Statics_DGain').slider("option", "disabled", true);
	$('#slider_Control_Statics_Exposure').slider("option", "disabled", true);
	$('#slider_Control_Statics_ColorTemp').slider("option", "disabled", true);
	$('#slider_Control_Statics_RGain').slider("option", "disabled", true);
	$('#slider_Control_Statics_BGain').slider("option", "disabled", true);

	// Init IQ
	app_initUI_slider2(0, -7, 7, 1, '#slider_IQ_Sharpness_Sharp_val', '#slider_IQ_Sharpness_Sharp', app_iq_handler);
	app_initUI_slider2(0, -7, 7, 1, '#slider_IQ_Sharpness_Edge_val', '#slider_IQ_Sharpness_Edge', app_iq_handler);
	app_initUI_slider2(0, -7, 7, 1, '#slider_IQ_Denoise_NR_Y_val','#slider_IQ_Denoise_NR_Y', app_iq_handler);
	app_initUI_slider2(0, -7, 7, 1, '#slider_IQ_Denoise_NR_C_val','#slider_IQ_Denoise_NR_C', app_iq_handler);
	app_initUI_slider2(0, -7, 7, 1, '#slider_IQ_NR3D_NR_val', '#slider_IQ_NR3D_NR', app_iq_handler);
	app_initUI_slider2(800, 0, 2048, 1, '#slider_IQ_Sensor_BlackLevel_R_val', '#slider_IQ_Sensor_BlackLevel_R', app_iq_handler);
	app_initUI_slider2(800, 0, 2048, 1, '#slider_IQ_Sensor_BlackLevel_GR_val', '#slider_IQ_Sensor_BlackLevel_GR', app_iq_handler);
	app_initUI_slider2(800, 0, 2048, 1, '#slider_IQ_Sensor_BlackLevel_GB_val', '#slider_IQ_Sensor_BlackLevel_GB', app_iq_handler);
	app_initUI_slider2(800, 0, 2048, 1, '#slider_IQ_Sensor_BlackLevel_B_val', '#slider_IQ_Sensor_BlackLevel_B', app_iq_handler);
	app_initUI_slider2(0, -7, 7, 1, '#slider_IQ_Color_Saturation_val', '#slider_IQ_Color_Saturation', app_iq_handler);

	// Init AE
	app_initUI_slider2(0, 0, 80, 1, '#slider_AE_EVProgram_GainMax_val', '#slider_AE_EVProgram_GainMax', app_aeev_handler);
	app_initUI_slider2(0, 0, 80, 1, '#slider_AE_EVProgram_GainMin_val', '#slider_AE_EVProgram_GainMin', app_aeev_handler);
	app_initUI_slider2(0,      1, 1000, 0.0001, '#slider_AE_EVProgram_ExpMax_val', '#slider_AE_EVProgram_ExpMax', app_aeev_handler);
	app_initUI_slider2(0, 0.0001, 1000, 0.0001, '#slider_AE_EVProgram_ExpMin_val', '#slider_AE_EVProgram_ExpMin', app_aeev_handler);
	app_initUI_slider2(0, 0, 100, 1, '#slider_AE_Default_AESpeed_val', '#slider_AE_Default_AESpeed', app_ae_handler);
//	app_initUI_slider2(0, 1, 500, 1, '#slider_AE_Default_SensorExposure_val', '#slider_AE_Default_SensorExposure', app_ae_handler);

	// Init AWB
	app_initUI_slider2(30, 0, 100, 1, '#slider_AWB_Basic_AWBSpeed_val', '#slider_AWB_Basic_AWBSpeed', app_awb_handler);
	app_initUI_slider2(0, 1000, 15000, 100, '#slider_AWB_PreferedCast_ColorTemperature_val', '#slider_AWB_PreferedCast_ColorTemperature', app_awb_handler);
	app_initUI_slider2(1.0, 0.0, 2.0, 0.01, '#slider_AWB_PreferedCast_LowRGain_val', '#slider_AWB_PreferedCast_LowRGain', app_awb_handler);
	app_initUI_slider2(1.0, 0.0, 2.0, 0.01, '#slider_AWB_PreferedCast_LowBGain_val', '#slider_AWB_PreferedCast_LowBGain', app_awb_handler);
	app_initUI_slider2(1.0, 0.0, 2.0, 0.01, '#slider_AWB_PreferedCast_HighRGain_val', '#slider_AWB_PreferedCast_HighRGain', app_awb_handler);
	app_initUI_slider2(1.0, 0.0, 2.0, 0.01, '#slider_AWB_PreferedCast_HighBGain_val', '#slider_AWB_PreferedCast_HighBGain', app_awb_handler);
	app_initUI_slider2(0.2, 0, 1.0, 0.01, '#slider_AWB_ManualWB_X_val', '#slider_AWB_ManualWB_X', app_awb_handler);
	app_initUI_slider2(0.2, 0, 1.0, 0.01, '#slider_AWB_ManualWB_Y_val', '#slider_AWB_ManualWB_Y', app_awb_handler);

	// Init UI data ------------------------------------------------------
	app_data = app_get_init_data();
	app_ui = 
	{
		AWB: {
			preferedcast : {
				"select" : 0
			}
		},
		AE: {
			ae_metering : {
				"weight" : 0
			},
			ae_weight_color : [
				"#fff",
				"#e00",
				"#ec0",
				"#ff0",
				"#8f0",
				"#8ff",
				"#88f",
				"#22f"
			],
			ae_weight : [[]]
		}
	};

	// Update UI data
	app_updateUI_List('#select_Control_CameraSetting_ISO', env_get_uiList_iso(true));
	app_updateUI_IQ_List_Update();
	app_updateUI_IQ(app_data.IQ);
	app_updateUI_List('#select_Control_CameraSetting_CameraMode', env_get_uiList_cameramode());
	app_updateUI_AE_List_Update();
	app_updateUI_AE(app_data.AE);
	app_updateUI_AWB_List_Update();
	app_updateUI_AWB(app_data.AWB);

	// Setup Modal's -----------------------------------------------------
	$('#loading_modal').append(loading_create_modal());
	$('#gamma_modal').append(gamma_create_modal());
	$('#aetbl_modal').append(aetbl_create_modal());
	$('#s_cap_modal').append(s_cap_create_modal());
	$('#v_cap_modal').append(v_cap_create_modal());

	app_data.IQ.gamma = gamma_init();
	aetbl_init();
	s_cap_init();
	v_cap_init();

	// Reflect Disable UI ------------------------------------------------
	env_disable_ui().forEach(function(val, index, array)
	{
		$(val).hide();
	});

	// Init UI event -----------------------------------------------------
	// Set Up General Operation
	window.onbeforeunload = function(e)
	{
		// [Yossy memo]
		// Is this Chrome's Bug?
		// The following message is not shown when unload.
		return 'OK to leave page?';
	};
	$(window).unload(function()
	{
		// Request tuning stop to RTOS
		rtos_tuning_stop(function(data)
		{
			// Empty
		});
	});
	
	// Set Up Header
	$('#button_Header_Save').on('click', function()
	{
		var param = {};

		// Show wait modal
		loading_show();

		// Prepare the arguments
		param = app_get_rtos_args();
		param.camera_mode = app_check_disable() ? 1 : 2;

		// Request saving binary's to NAND
		rtos_do_save(param, function(data)
		{
            // Hide wait modal
            loading_close();

			// Show success message
			window.alert("Succeeded to save binary's to HW.");
		});
    });	
    $('#button_Header_Export').on('click', function()
	{
		// Check Camera Mode
		if (app_check_disable())
		{
			window.alert("Please Change camera mode!");
			return;
		}

		// Request SDCard Status to RTOS
		rtos_check_sd_card(function(data)
		{
			var param = {};

			// Check SDCard Status
			if (data.Status.hw_status == 0)
			{
				// SDCard is disconnected
				window.alert('Please Insert SD Card of Top Side for Capturing!');
			}
			else
			{
				// Show wait modal
				loading_show();

				// SDCard is connected
				// Request exporting in SDCard
				rtos_do_export(function(data)
				{
					// Hide wait modal
					loading_close();

					// Show success message
					window.alert("Succeeded to export in SDCard");
				});
			}
		});
    });	
    $('#button_Header_IQ_def').on('click', function()
	{
		var param = app_get_rtos_args();
		var tmpparam;
		var tmpdata = app_get_init_data();

		// Check Camera Mode
		if (app_check_disable())
		{
			window.alert("Please Change camera mode!");
			return;
		}

		app_data.IQ = tmpdata.IQ;
		app_updateUI_IQ(app_data.IQ);
		param.ignore_update = "true";

		// Show wait modal
		loading_show();

		// Prepare the arguments
		tmpparam = gamma_get_rtos_argument(tmpdata.IQ.gamma);
		param.set_type = "IQ_gamma";
		param.set_data = tmpparam.set_data;

		// Request uploading parameter to RTOS
		rtos_upload_param(param, function(data)
		{
			param.set_type = "IQ_sharpness";
			param.set_data = tmpdata.IQ.sharpness;

			rtos_upload_param(param, function(data)
			{
				param.set_type = "IQ_denoise";
				param.set_data = tmpdata.IQ.denoise;

				rtos_upload_param(param, function(data)
				{
					param.set_type = "IQ_nr3d";
					param.set_data = tmpdata.IQ.nr3d;

					rtos_upload_param(param, function(data)
					{
						param.set_type = "IQ_sensor";
						param.set_data = {
							data : [
								[tmpdata.IQ.sensor.r[0],tmpdata.IQ.sensor.gr[0],tmpdata.IQ.sensor.gb[0],tmpdata.IQ.sensor.b[0]],
								[tmpdata.IQ.sensor.r[1],tmpdata.IQ.sensor.gr[1],tmpdata.IQ.sensor.gb[1],tmpdata.IQ.sensor.b[1]],
								[tmpdata.IQ.sensor.r[2],tmpdata.IQ.sensor.gr[2],tmpdata.IQ.sensor.gb[2],tmpdata.IQ.sensor.b[2]],
								[tmpdata.IQ.sensor.r[3],tmpdata.IQ.sensor.gr[3],tmpdata.IQ.sensor.gb[3],tmpdata.IQ.sensor.b[3]],
								[tmpdata.IQ.sensor.r[4],tmpdata.IQ.sensor.gr[4],tmpdata.IQ.sensor.gb[4],tmpdata.IQ.sensor.b[4]],
								[tmpdata.IQ.sensor.r[5],tmpdata.IQ.sensor.gr[5],tmpdata.IQ.sensor.gb[5],tmpdata.IQ.sensor.b[5]],
								[tmpdata.IQ.sensor.r[6],tmpdata.IQ.sensor.gr[6],tmpdata.IQ.sensor.gb[6],tmpdata.IQ.sensor.b[6]]
							]
						};

						rtos_upload_param(param, function(data)
						{
							param.set_type = "IQ_color";
							param.set_data = tmpdata.IQ.color;
							param.ignore_update = "false";

							rtos_upload_param(param, function(data)
							{
								// Hide wait modal
								loading_close();

								window.alert("done");
							});	
						});	
					});	
				});	
			});	
		});

		return;
    });	
    $('#button_Header_Import').on('click', function()
	{
		// Check Camera Mode
		if (app_check_disable())
		{
			window.alert("Please Change camera mode!");
			return;
		}

		// Request SDCard Status to RTOS
		rtos_check_sd_card(function(data)
		{
			var param = {};

			// Check SDCard Status
			if (data.Status.hw_status == 0)
			{
				// SDCard is disconnected
				window.alert('Please Insert SD Card of Top Side for Capturing!');
			}
			else
			{
				// Show wait modal
				loading_show();
				
				// SDCard is connected
				// Request importing Setting info
				rtos_do_import(function(data)
				{
					var param = {};
					var camera_mode;
					var iso;
					var wb_mode;

					// Update UI data
					camera_mode = $('#select_Control_CameraSetting_CameraMode').val();
					iso = $('#select_Control_CameraSetting_ISO').val();
					wb_mode = $('#select_Control_CameraSetting_WBMode').val();

					app_data.Statics.control["camera_mode"] = parseInt(camera_mode);
					app_data.Statics.control["iso"] = parseInt(iso);		
					app_data.Statics.control["wb_mode"] = parseInt(wb_mode);
					
					app_updateUI_List_Resolution();

					// Prepare the arguments
					param = app_get_rtos_args();
					param.camera_mode = app_check_disable() ? 1 : 2;

					// Request changing Camera Mode to RTOS
					rtos_change_camera_mode(param, function(data)
					{
						// Hide wait modal
						loading_close();

						// Refresh UI's data
						app_refresh_ui_data(data);

						// Show success message
						window.alert("Succeeded to import in SDCard");
					});
				});
			}
		});
    });	
		
	// Set Up Static
    $('#button_Control_Statics_Refresh').on('click', function()
	{	
		// Check Camera Mode
		if (app_check_disable())
		{
			window.alert("Please Change camera mode!");
			return;
		}
		
		// Show wait modal
		loading_show();

		// Request reading sensor info to RTOS
		rtos_get_sensorinfo(function(data)
		{
            // Hide wait modal
            loading_close();

			// Regist UI data
			app_data.Statics.statics["iso"] = parseInt(data.Statics.statics["iso"]);
			app_data.Statics.statics["a_gain"] = parseFloat(data.Statics.statics["a_gain"]) / 0x10000;
			app_data.Statics.statics["d_gain"] = parseInt(data.Statics.statics["d_gain"]);
			app_data.Statics.statics["expo"] = parseInt(data.Statics.statics["expo"]) / 0x10000;
			app_data.Statics.statics["c_temp"] = parseFloat(data.Statics.statics["c_temp"]) / 0x10000;
			app_data.Statics.statics["wb_r"] = parseFloat(data.Statics.statics["wb_r"]) / 0x100;
			app_data.Statics.statics["wb_b"] = parseFloat(data.Statics.statics["wb_b"]) / 0x100;

			app_updateUI_Statics(app_data);
		});
    });	
	$('#select_Control_CameraSetting_CameraMode').change(function()
	{
		var param = {};
		var camera_mode;
		var iso;
		var wb_mode;

		// Update UI data
		camera_mode = $('#select_Control_CameraSetting_CameraMode').val();
		iso = $('#select_Control_CameraSetting_ISO').val();
		wb_mode = $('#select_Control_CameraSetting_WBMode').val();

		app_data.Statics.control["camera_mode"] = parseInt(camera_mode);
		app_data.Statics.control["iso"] = parseInt(iso);		
		app_data.Statics.control["wb_mode"] = parseInt(wb_mode);
		
		app_updateUI_List_Resolution();

		// Prepare the arguments
		param = app_get_rtos_args();
		param.camera_mode = app_check_disable() ? 1 : 2;

		if (!DEBUG_UI)
		{
			// Show wait modal
			loading_show();

			// Request changing Camera Mode to RTOS
			rtos_change_camera_mode(param, function(data)
			{
				// Hide wait modal
				loading_close();

				// Refresh UI's data
				app_refresh_ui_data(data);
			});
		}
		else
		{
			app_refresh_ui_data(app_data);
		}
	});
	$('#select_Control_CameraSetting_Resolution').change(function() 
	{
		var param = {};
		var camera_mode;
		var resolution;
		var options;

		// Update UI data
		resolution = $('#select_Control_CameraSetting_Resolution').val();
		
		if (app_check_video_preview())
		{
			app_data.Statics.control["v_resolution"] = parseInt(resolution);
		}
		else if (app_check_still_capture())
		{
			app_data.Statics.control["s_resolution"] = parseInt(resolution);			
		}

		// Prepare the arguments
		param = app_get_rtos_args();
		
		// Show wait modal
		loading_show();

		// Request changing Camera Mode to RTOS
		rtos_change_resolution(param, function(data)
		{
			if (typeof env_changeresolution_callback == 'function')
			{
				env_changeresolution_callback();
			}
			else
			{
				// Hide wait modal
				loading_close();

				app_data.AE.defaultset = data.AE.defaultset;

				// Reflect app_data
				app_readData_EVProgram_All();

				app_updateUI_IQ(app_data.IQ);
				app_updateUI_AE(app_data.AE);
				app_updateUI_AWB_List_Update();
				app_updateUI_AWB(app_data.AWB);
			}
		});
	});
	$('#select_Control_CameraSetting_ISO').change(function()
	{
		var param = {};
		var iso;

		// Update UI data
		iso = $('#select_Control_CameraSetting_ISO').val();
		app_data.Statics.control["iso"] = parseInt(iso);

		// Prepare the arguments
		param = app_get_rtos_args();
		param.camera_mode = app_check_disable() ? 1 : 2;
		
		// Show wait modal
		loading_show();

		// Request changing ISO to RTOS
		rtos_change_iso(param, function(data)
		{
            // Hide wait modal
            loading_close();
		});
	});
	$('#select_Control_CameraSetting_WBMode').change(function()
	{
		// Show wait modal
		loading_show();

		// Update WB Mode
		app_update_wb_mode();
	});
	$('#button_Control_CameraSetting_Capture').on('click', function() 
	{	
		// Check Camera Mode
		if (app_check_still_capture())
		{
			s_cap_show();	
		}	
		else if (app_check_video_preview())
		{
			v_cap_show();
		}
		else
		{
			window.alert("Please Change camera mode!");
		}
		
		return;
    });
	
	// Set Up IQ Gamma
	$('#button_IQ_Gamma_Upload').click(function()
	{	
		var param = {};

		app_data.IQ.gamma["index"] = parseInt($("#radio_IQ_Gamma_Curve input[name='pattern']:radio:checked").val()); 
		param = gamma_get_rtos_argument(app_data.IQ.gamma);
		
		app_update_parameter(param);	
	});
    $("#radio_IQ_Gamma_Curve").on('click', function() 
	{	
		app_data.IQ.gamma["index"] = parseInt($("#radio_IQ_Gamma_Curve input[name='pattern']:radio:checked").val()); 
    });
    $('#button_IQ_Gamma_Edit').on('click', function() 
	{	
		var param = {};

		// Check Camera Mode
		if (app_check_disable())
		{
			window.alert("Please Change camera mode!");
			return;
		}
		
		app_data.IQ.gamma["index"] = parseInt($("#radio_IQ_Gamma_Curve input[name='pattern']:radio:checked").val()); 
		
		gamma_show_modal(app_data.IQ.gamma, function(index, gm_data)
		{
			// call back
			app_data.IQ.gamma["lut"][index] = gamma_copy_data(gm_data);
		});
    });
	
	// Set Up IQ Sharpness
	$('#button_IQ_Sharpness_Upload').click(function()
	{	
		var param = {};
		param.set_type = "IQ_sharpness";
		param.set_data = app_data.IQ.sharpness;

		app_update_parameter(param);
	});
	$('#select_IQ_Sharpness_ISO').change(function() 
	{
		app_updateUI_IQ(app_data.IQ);
	});

	// Set Up IQ Denoise
	$('#button_IQ_Denoise_Upload').click(function()
	{
		var param = {};
		param.set_type = "IQ_denoise";
		param.set_data = app_data.IQ.denoise;

		app_update_parameter(param);
	});
	$('#select_IQ_Denoise_ISO').change(function()
	{
		app_updateUI_IQ(app_data.IQ);
	});

	// Set Up IQ 3D NR
	$('#button_IQ_NR3D_Upload').click(function()
	{	
		var param = {};
		param.set_type = "IQ_nr3d";
		param.set_data = app_data.IQ.nr3d;
	
		app_update_parameter(param);
	});
	$('#select_IQ_NR3D_ISO').change(function()
	{
		app_updateUI_IQ(app_data.IQ);
	});

	// Set Up IQ Sensor
	$('#button_IQ_Sensor_Upload').click(function()
	{	
		var param = {};
		param.set_type = "IQ_sensor";
		param.set_data = {
			data : [
				[app_data.IQ.sensor.r[0],app_data.IQ.sensor.gr[0],app_data.IQ.sensor.gb[0],app_data.IQ.sensor.b[0]],
				[app_data.IQ.sensor.r[1],app_data.IQ.sensor.gr[1],app_data.IQ.sensor.gb[1],app_data.IQ.sensor.b[1]],
				[app_data.IQ.sensor.r[2],app_data.IQ.sensor.gr[2],app_data.IQ.sensor.gb[2],app_data.IQ.sensor.b[2]],
				[app_data.IQ.sensor.r[3],app_data.IQ.sensor.gr[3],app_data.IQ.sensor.gb[3],app_data.IQ.sensor.b[3]],
				[app_data.IQ.sensor.r[4],app_data.IQ.sensor.gr[4],app_data.IQ.sensor.gb[4],app_data.IQ.sensor.b[4]],
				[app_data.IQ.sensor.r[5],app_data.IQ.sensor.gr[5],app_data.IQ.sensor.gb[5],app_data.IQ.sensor.b[5]],
				[app_data.IQ.sensor.r[6],app_data.IQ.sensor.gr[6],app_data.IQ.sensor.gb[6],app_data.IQ.sensor.b[6]]
			]
		};

		app_update_parameter(param);
	});
	$('#select_IQ_Sensor_ISO').change(function() 
	{
		app_updateUI_IQ(app_data.IQ);
	});

	// Set Up IQ Color
	$('#button_IQ_Color_Upload').click(function()
	{	
		var param = {};
		param.set_type = "IQ_color";
		param.set_data = app_data.IQ.color;

		app_update_parameter(param);
	});
	
	// Set Up AE Common
	$('#number_AE_Common_Fnumber').on('change', function(event, ui){
		var val = $(this).val();

		if (val < 1.0)
		{
			val = 1.0;
		}
		else if (val > 4.0)
		{
			val = 4.0;
		}

		$(this).val(val);
		app_data.AE.common["f_number"] = parseFloat(val);
	});
	$('#button_AE_Common_Upload').click(function()
	{	
		// Update Values
		var val = $('#number_AE_Common_Fnumber').val();

		// Update Values
		app_writeData_EVProgram_All(val);
		// CGI need AV value. so F number need to be converted to AV value
		val = parseFloat(2 * Math.log(val) / Math.log(2));

		// Upload Values
		var param = {};
		param.set_type = "AE_common";
		param.set_data = {
			"f_number" : val,
			"prog0" : app_data.AE.evprogram["prog0"],
			"prog1" : app_data.AE.evprogram["prog1"],
			"prog2" : app_data.AE.evprogram["prog2"],
			"prog3" : app_data.AE.evprogram["prog3"],
			"prog4" : app_data.AE.evprogram["prog4"],
			"prog5" : app_data.AE.evprogram["prog5"],
			"prog6" : app_data.AE.evprogram["prog6"],
			"prog7" : app_data.AE.evprogram["prog7"],
			"prog8" : app_data.AE.evprogram["prog8"],
			"prog9" : app_data.AE.evprogram["prog9"],
			"prog10" : app_data.AE.evprogram["prog10"],
			"prog11" : app_data.AE.evprogram["prog11"],
			"prog12" : app_data.AE.evprogram["prog12"],
			"prog13" : app_data.AE.evprogram["prog13"],
			"prog14" : app_data.AE.evprogram["prog14"],
			"prog15" : app_data.AE.evprogram["prog15"]
		};

		app_update_parameter(param);
	});
	
	// Set Up AE EVProgram
	$('#button_AE_EVProgram_Upload').click(function()
	{
		// Update Values
		var val = $('#number_AE_Common_Fnumber').val();
			
		// Update Values
		app_writeData_EVProgram_All(val);
				
		// Upload Values
		var param = {};
		param.set_type = "AE_evprogram";
		param.set_data = app_data.AE.evprogram;

		app_update_parameter(param);
	});
	$('#select_AE_EVProgram_ProgramNo').change(function() 
	{
		// update data related to ev program
		app_updateUI_AE(app_data.AE);
	});
	
	// Set Up AE Default
	$('#button_AE_Default_Upload').click(function()
	{
		// Update Values
		var val;
		val = $('#select_AE_Default_AEMetering').val();
		app_data.AE.defaultset["ae_metering"] = parseInt(val);
		val = $('#select_AE_Default_AEMode').val();
		app_data.AE.defaultset["ae_mode"] =  parseInt(val);
		val = $('#slider_AE_Default_AESpeed').slider('option','value');
		app_data.AE.defaultset["ae_speed"] = parseInt(val);
		val = $('#select_AE_Default_AEAdaptive').val();
		app_data.AE.defaultset["ae_adaptive"] = parseInt(val);
		val = $('#select_AE_Default_EVProgram').val();
		app_data.AE.defaultset["evprogram"] = parseInt(val);
		val = $('#select_AE_Default_FlickerComp').val();
		app_data.AE.defaultset["flicker_comp"] = parseInt(val);
		val = $('#select_AE_Default_EVComp').val();
		app_data.AE.defaultset["ev_comp"] = parseInt(val);
		val = $('#select_AE_Default_SensorGain').val();
		app_data.AE.defaultset["sensor_gain"] = parseInt(val);
		val = $('#select_AE_Default_SensorExposure').val();
		app_data.AE.defaultset["sensor_exposure"] = parseFloat(1.0 / val * 1000);
		//val = $('#slider_AE_Default_SensorExposure').slider('option','value');
		//app_data.AE.defaultset["sensor_exposure"] = parseFloat(val / 1000);

		// Upload Values
		var param = {};
		param.set_type = "AE_default";
		param.set_data = app_data.AE.defaultset;

		app_update_parameter(param);
	});	
    $('#button_AE_Default_AEMetering_Edit').on('click', function()
	{
		var param = {};
		var ae_metering_mode = $('#select_AE_Default_AEMetering').val();

		// Check Camera Mode
		if (app_check_disable())
		{
			window.alert("Please Change camera mode!");
			return;
		}
		
		// Prepare the arguments
		param = app_get_rtos_args();
		param.camera_mode = app_check_disable() ? 1 : 2;

		param.ae_table_pos = 16 + 32 / 2 * 32 * ae_metering_mode;

		aetbl_show_modal(param, function(ui_data)
		{
			for (var i = 0; i < 32; i++)
			{
				app_ui.AE.ae_weight[i] = [];
					
				for (var j = 0; j < 32; j++)
				{
					app_ui.AE.ae_weight[i][j] = ui_data.ae_weight[i][j];
				}
			}
		});
    });	

	// Set Up AWB Basic
	$('#button_AWB_Basic_Upload').click(function()
	{
		var param = {};
		param.set_type = "AWB_basic";
		param.set_data = app_data.AWB.basic;

		app_update_parameter(param);
	});
	
	// Set Up AWB PreferedCast
	$('#button_AWB_PreferedCast_Upload').click(function()
	{	
		var length = app_data.AWB.preferedcast.length;
		var param = {};

		if (!app_check_disable() && length == 0)
		{
			window.alert("PreferedCast settings is empty!");
			return;
		}

		param.set_type = "AWB_preferedcast";
		param.set_data = app_data.AWB.preferedcast;

		app_update_parameter(param);
	});
	$('#button_AWB_PreferedCast_AddTop').click(function()
	{
		var newData = [
			parseInt($('#slider_AWB_PreferedCast_ColorTemperature_val').val()),
			parseInt($('#slider_AWB_PreferedCast_LowRGain_val').val()),
			parseInt($('#slider_AWB_PreferedCast_LowBGain_val').val()),
			parseInt($('#slider_AWB_PreferedCast_HighRGain_val').val()),
			parseInt($('#slider_AWB_PreferedCast_HighBGain_val').val())
		];

		if (app_data.AWB.preferedcast.length < 16)
		{
			if (app_data.AWB.preferedcast.length == 0)
			{
				app_data.AWB.preferedcast.push(newData);
			}
			else
			{
				app_data.AWB.preferedcast.unshift(newData);
			}
			app_ui.AWB.preferedcast.selection = 0;
			app_updateUI_AWB_List_Update();
		}
		else
		{
			window.alert('Num of list is 16.');
		}	
	});
	$('#button_AWB_PreferedCast_AddBottom').click(function()
	{
		var newData = [
			parseInt($('#slider_AWB_PreferedCast_ColorTemperature_val').val()),
			parseInt($('#slider_AWB_PreferedCast_LowRGain_val').val()),
			parseInt($('#slider_AWB_PreferedCast_LowBGain_val').val()),
			parseInt($('#slider_AWB_PreferedCast_HighRGain_val').val()),
			parseInt($('#slider_AWB_PreferedCast_HighBGain_val').val())
		];

		if (app_data.AWB.preferedcast.length < 16)
		{
			app_data.AWB.preferedcast.push(newData);
			app_ui.AWB.preferedcast.selection = app_data.AWB.preferedcast.length - 1;
			app_updateUI_AWB_List_Update();
		}
		else
		{
			window.alert('Num of list is 16.');
		}
	});
	$('#button_AWB_PreferedCast_ResetList').click(function()
	{
		app_data.AWB.preferedcast.length = 0;
		app_ui.AWB.preferedcast.selection =-1;
		app_updateUI_AWB_List_Update();
	});

	// Set Up AWB ManualWB
	$('#button_AWB_ManualWB_Upload').click(function()
	{
		var param = {};
		param.set_type = "AWB_manualwb";
		param.set_data = app_data.AWB.manualwb;

		app_update_parameter(param);
	});		
	$('#select_AWB_ManualWB_Selection').change(function()
	{
		// update data related to manual wb program
		app_updateUI_AWB(app_data.AWB);
	});
});

   
